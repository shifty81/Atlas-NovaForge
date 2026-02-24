#include "MarketplaceImporter.h"
#include "AssetImporter.h"
#include "../sim/StateHasher.h"
#include "../core/Logger.h"
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <cstring>

namespace atlas::asset {

// ---------------------------------------------------------------------------
// Helper functions
// ---------------------------------------------------------------------------

static bool FileExists(const std::string& path) {
    return std::filesystem::exists(path);
}

static std::string GetFileExtension(const std::string& path) {
    auto ext = std::filesystem::path(path).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return ext;
}

static std::string GetDownloadCacheDir(const MarketplaceImportOptions& options) {
    if (!options.downloadCache.empty()) {
        return options.downloadCache;
    }
    return std::filesystem::temp_directory_path().string() + "/atlas_marketplace_cache";
}

/// Validate that an asset ID does not contain path traversal sequences.
static bool IsValidAssetId(const std::string& assetId) {
    if (assetId.empty()) return false;
    if (assetId.find("..") != std::string::npos) return false;
    if (assetId.find('/') != std::string::npos) return false;
    if (assetId.find('\\') != std::string::npos) return false;
    if (assetId[0] == '.') return false;
    return true;
}

// ---------------------------------------------------------------------------
// MarketplaceImportRegistry
// ---------------------------------------------------------------------------

void MarketplaceImportRegistry::RegisterImporter(std::unique_ptr<IMarketplaceImporter> importer) {
    if (importer) {
        m_importers.push_back(std::move(importer));
    }
}

IMarketplaceImporter* MarketplaceImportRegistry::FindImporter(MarketplaceType type) const {
    for (const auto& imp : m_importers) {
        if (imp->GetMarketplaceType() == type) {
            return imp.get();
        }
    }
    return nullptr;
}

ImportResult MarketplaceImportRegistry::FetchAndImport(
    MarketplaceType marketplace,
    const std::string& assetId,
    const MarketplaceImportOptions& options) {
    
    IMarketplaceImporter* importer = FindImporter(marketplace);
    if (!importer) {
        ImportResult result;
        result.errorMessage = "No importer registered for the specified marketplace";
        return result;
    }
    
    if (!importer->IsAvailable()) {
        ImportResult result;
        result.errorMessage = "Marketplace importer is not available (check API keys/connectivity)";
        return result;
    }
    
    // Fetch the asset from the marketplace
    MarketplaceFetchResult fetchResult = importer->FetchAsset(assetId, options);
    if (!fetchResult.success) {
        ImportResult result;
        result.errorMessage = "Failed to fetch asset: " + fetchResult.errorMessage;
        return result;
    }
    
    // Import the fetched asset into Atlas format
    return importer->ImportAsset(fetchResult, options);
}

std::vector<MarketplaceType> MarketplaceImportRegistry::AvailableMarketplaces() const {
    std::vector<MarketplaceType> types;
    for (const auto& imp : m_importers) {
        if (imp->IsAvailable()) {
            types.push_back(imp->GetMarketplaceType());
        }
    }
    return types;
}

size_t MarketplaceImportRegistry::ImporterCount() const {
    return m_importers.size();
}

void MarketplaceImportRegistry::SetHttpClient(IHttpClient* client) {
    for (auto& imp : m_importers) {
        imp->SetHttpClient(client);
    }
}

// ---------------------------------------------------------------------------
// ItchIOImporter
// ---------------------------------------------------------------------------

MarketplaceType ItchIOImporter::GetMarketplaceType() const {
    return MarketplaceType::ItchIO;
}

MarketplaceFetchResult ItchIOImporter::FetchAsset(
    const std::string& assetId,
    const MarketplaceImportOptions& options) {
    
    MarketplaceFetchResult result;
    result.metadata.marketplace = MarketplaceType::ItchIO;
    result.metadata.id = assetId;
    
    if (!IsValidAssetId(assetId)) {
        result.errorMessage = "Invalid asset ID (contains path traversal characters): " + assetId;
        return result;
    }
    
    std::string cacheDir = GetDownloadCacheDir(options);
    std::filesystem::create_directories(cacheDir);
    
    std::string localPath = cacheDir + "/" + assetId;
    
    // Check if asset exists locally in cache
    if (FileExists(localPath)) {
        if (!ValidateItchAsset(localPath)) {
            result.errorMessage = "Asset validation failed for: " + assetId;
            return result;
        }
        result.success = true;
        result.localPath = localPath;
        result.metadata.name = assetId;
        return result;
    }
    
    // Attempt API download if HTTP client is available
    if (m_httpClient) {
        std::string metadataUrl = "https://api.itch.io/games/" + assetId;
        
        std::vector<std::pair<std::string, std::string>> headers;
        if (!options.apiKey.empty()) {
            headers.push_back({"Authorization", "Bearer " + options.apiKey});
        }
        
        Logger::Info("Fetching itch.io asset metadata: " + metadataUrl);
        HttpResponse metaResp = m_httpClient->Get(metadataUrl, headers);
        
        if (metaResp.IsError()) {
            result.errorMessage = "Failed to fetch itch.io metadata for '" + assetId + "': " + metaResp.errorMessage;
            return result;
        }
        
        Logger::Info("Downloading itch.io asset to: " + localPath);
        HttpResponse dlResp = m_httpClient->DownloadFile(metadataUrl + "/download", localPath, headers);
        
        if (dlResp.IsError()) {
            result.errorMessage = "Failed to download itch.io asset '" + assetId + "': " + dlResp.errorMessage;
            return result;
        }
        
        Logger::Info("Successfully downloaded itch.io asset: " + assetId);
        result.success = true;
        result.localPath = localPath;
        result.metadata.name = assetId;
        return result;
    }
    
    result.errorMessage = "Asset not found in cache (no HTTP client configured): " + assetId;
    return result;
}

ImportResult ItchIOImporter::ImportAsset(
    const MarketplaceFetchResult& fetchResult,
    const MarketplaceImportOptions& options) {
    
    if (!fetchResult.success) {
        ImportResult result;
        result.errorMessage = "Cannot import failed fetch result";
        return result;
    }
    
    // Use the standard asset importer based on file extension
    AssetImportRegistry standardRegistry;
    
    // Register standard importers
    standardRegistry.RegisterImporter(std::make_unique<MeshImporter>(ImportFormat::OBJ));
    standardRegistry.RegisterImporter(std::make_unique<MeshImporter>(ImportFormat::FBX));
    standardRegistry.RegisterImporter(std::make_unique<TextureImporter>(ImportFormat::PNG));
    standardRegistry.RegisterImporter(std::make_unique<AudioImporter>(ImportFormat::WAV));
    standardRegistry.RegisterImporter(std::make_unique<AudioImporter>(ImportFormat::OGG));
    
    ImportOptions importOpts;
    importOpts.outputDirectory = options.outputDirectory;
    importOpts.generateMipmaps = options.generateMipmaps;
    importOpts.compressTextures = options.compressTextures;
    
    return standardRegistry.Import(fetchResult.localPath, importOpts);
}

bool ItchIOImporter::IsAvailable() const {
    // itch.io doesn't require API keys for public assets
    return true;
}

void ItchIOImporter::SetHttpClient(IHttpClient* client) {
    m_httpClient = client;
}

bool ItchIOImporter::ValidateItchAsset(const std::string& path) const {
    // Basic validation: check if file exists and has recognized extension
    if (!FileExists(path)) {
        return false;
    }
    
    std::string ext = GetFileExtension(path);
    return ext == ".png" || ext == ".obj" || ext == ".fbx" || 
           ext == ".wav" || ext == ".ogg" || ext == ".ttf";
}

// ---------------------------------------------------------------------------
// UnrealMarketplaceImporter
// ---------------------------------------------------------------------------

MarketplaceType UnrealMarketplaceImporter::GetMarketplaceType() const {
    return MarketplaceType::UnrealEngine;
}

MarketplaceFetchResult UnrealMarketplaceImporter::FetchAsset(
    const std::string& assetId,
    const MarketplaceImportOptions& options) {
    
    MarketplaceFetchResult result;
    result.metadata.marketplace = MarketplaceType::UnrealEngine;
    result.metadata.id = assetId;
    
    if (!IsValidAssetId(assetId)) {
        result.errorMessage = "Invalid asset ID (contains path traversal characters): " + assetId;
        return result;
    }
    
    std::string cacheDir = GetDownloadCacheDir(options);
    std::filesystem::create_directories(cacheDir);
    
    std::string localPath = cacheDir + "/" + assetId + ".uasset";
    
    // Check if asset exists locally in cache
    if (FileExists(localPath)) {
        result.success = true;
        result.localPath = localPath;
        result.metadata.name = assetId;
        return result;
    }
    
    // Attempt API download if HTTP client is available
    if (m_httpClient) {
        std::string apiUrl = "https://www.unrealengine.com/marketplace/api/assets/" + assetId;
        
        std::vector<std::pair<std::string, std::string>> headers;
        if (!options.apiKey.empty()) {
            headers.push_back({"Authorization", "Bearer " + options.apiKey});
        }
        
        Logger::Info("Fetching Unreal Marketplace asset: " + apiUrl);
        HttpResponse dlResp = m_httpClient->DownloadFile(apiUrl, localPath, headers);
        
        if (dlResp.IsError()) {
            result.errorMessage = "Failed to download Unreal asset '" + assetId + "': " + dlResp.errorMessage;
            return result;
        }
        
        Logger::Info("Successfully downloaded Unreal asset: " + assetId);
        result.success = true;
        result.localPath = localPath;
        result.metadata.name = assetId;
        return result;
    }
    
    result.errorMessage = "Unreal asset not found in cache (no HTTP client configured): " + assetId;
    return result;
}

ImportResult UnrealMarketplaceImporter::ImportAsset(
    const MarketplaceFetchResult& fetchResult,
    const MarketplaceImportOptions& options) {
    
    if (!fetchResult.success) {
        ImportResult result;
        result.errorMessage = "Cannot import failed fetch result";
        return result;
    }
    
    ImportResult result;
    
    // Convert .uasset to intermediate format (FBX or similar)
    std::string intermediatePath = fetchResult.localPath + ".fbx";
    
    if (!ConvertUAsset(fetchResult.localPath, intermediatePath)) {
        result.errorMessage = "Failed to convert Unreal asset to intermediate format";
        return result;
    }
    
    // Import the intermediate format using standard importer
    AssetImportRegistry standardRegistry;
    standardRegistry.RegisterImporter(std::make_unique<MeshImporter>(ImportFormat::FBX));
    
    ImportOptions importOpts;
    importOpts.outputDirectory = options.outputDirectory;
    
    return standardRegistry.Import(intermediatePath, importOpts);
}

bool UnrealMarketplaceImporter::IsAvailable() const {
    return m_httpClient != nullptr || !m_apiCredential.empty();
}

void UnrealMarketplaceImporter::SetApiCredential(const std::string& cred) {
    m_apiCredential = cred;
}

bool UnrealMarketplaceImporter::HasApiCredential() const {
    return !m_apiCredential.empty();
}

void UnrealMarketplaceImporter::SetHttpClient(IHttpClient* client) {
    m_httpClient = client;
}

bool UnrealMarketplaceImporter::ConvertUAsset(
    const std::string& inputPath,
    const std::string& outputPath) const {
    
    if (!FileExists(inputPath)) {
        return false;
    }

    std::ifstream in(inputPath, std::ios::binary);
    if (!in.is_open()) return false;

    // Read magic number (4 bytes)
    uint8_t magic[4] = {};
    in.read(reinterpret_cast<char*>(magic), 4);
    if (in.gcount() < 4) return false;

    if (magic[0] != 0xC1 || magic[1] != 0x83 || magic[2] != 0x2A || magic[3] != 0x9E) {
        Logger::Error("Invalid UAsset magic number in: " + inputPath);
        return false;
    }

    Logger::Info("Parsing UAsset header: " + inputPath);

    // Read version at offset 4
    uint32_t version = 0;
    in.read(reinterpret_cast<char*>(&version), 4);

    // Read asset class name at offset 20 (null-terminated, max 64 chars)
    in.seekg(20, std::ios::beg);
    char className[65] = {};
    in.read(className, 64);
    className[64] = '\0';

    Logger::Info("UAsset version=" + std::to_string(version) + " class=" + std::string(className));

    // Read remaining mesh data
    in.seekg(0, std::ios::end);
    auto fileSize = in.tellg();
    in.seekg(0, std::ios::beg);
    std::vector<uint8_t> fileData(static_cast<size_t>(fileSize));
    in.read(reinterpret_cast<char*>(fileData.data()), fileSize);
    in.close();

    // Write output with FBX-like header
    std::ofstream out(outputPath, std::ios::binary);
    if (!out.is_open()) return false;

    // Write a simple FBX-like header marker
    const char fbxHeader[] = "ATLAS_FBX_CONVERTED";
    out.write(fbxHeader, sizeof(fbxHeader));
    // Write the mesh data bytes (everything after the header)
    // Offset breakdown: magic[4] + version[4] + padding[12] + className[64] = 84
    constexpr size_t UASSET_MESH_DATA_OFFSET = 84;
    size_t dataStart = std::min<size_t>(UASSET_MESH_DATA_OFFSET, fileData.size());
    if (fileData.size() > dataStart) {
        out.write(reinterpret_cast<const char*>(fileData.data() + dataStart),
                  fileData.size() - dataStart);
    }
    out.close();

    Logger::Info("Successfully converted Unreal asset: " + inputPath);
    return true;
}

bool UnrealMarketplaceImporter::ParseUAssetHeader(
    const std::string& inputPath, MarketplaceAssetMetadata& outMeta) const {

    std::ifstream in(inputPath, std::ios::binary);
    if (!in.is_open()) return false;

    uint8_t header[256] = {};
    in.read(reinterpret_cast<char*>(header), 256);
    auto bytesRead = in.gcount();
    in.close();

    if (bytesRead < 24) return false;

    // Check magic number 0xC1832A9E at offset 0
    if (header[0] != 0xC1 || header[1] != 0x83 || header[2] != 0x2A || header[3] != 0x9E) {
        return false;
    }

    // Read version uint32 at offset 4
    uint32_t version = 0;
    std::memcpy(&version, header + 4, 4);
    outMeta.version = std::to_string(version);

    // Read class name as null-terminated string at offset 20 (max 64 chars)
    char className[65] = {};
    size_t maxLen = std::min<size_t>(64, static_cast<size_t>(bytesRead) - 20);
    std::memcpy(className, header + 20, maxLen);
    className[maxLen] = '\0';
    outMeta.name = className;

    outMeta.marketplace = MarketplaceType::UnrealEngine;
    Logger::Info("ParseUAssetHeader: version=" + outMeta.version + " name=" + outMeta.name);
    return true;
}

// ---------------------------------------------------------------------------
// UnityAssetStoreImporter
// ---------------------------------------------------------------------------

MarketplaceType UnityAssetStoreImporter::GetMarketplaceType() const {
    return MarketplaceType::UnityAssetStore;
}

MarketplaceFetchResult UnityAssetStoreImporter::FetchAsset(
    const std::string& assetId,
    const MarketplaceImportOptions& options) {
    
    MarketplaceFetchResult result;
    result.metadata.marketplace = MarketplaceType::UnityAssetStore;
    result.metadata.id = assetId;
    
    if (!IsValidAssetId(assetId)) {
        result.errorMessage = "Invalid asset ID (contains path traversal characters): " + assetId;
        return result;
    }
    
    std::string cacheDir = GetDownloadCacheDir(options);
    std::filesystem::create_directories(cacheDir);
    
    std::string localPath = cacheDir + "/" + assetId + ".prefab";
    
    // Check if asset exists locally in cache
    if (FileExists(localPath)) {
        result.success = true;
        result.localPath = localPath;
        result.metadata.name = assetId;
        return result;
    }
    
    // Attempt API download if HTTP client is available
    if (m_httpClient) {
        std::string apiUrl = "https://assetstore.unity.com/api/content/" + assetId;
        
        std::vector<std::pair<std::string, std::string>> headers;
        if (!options.apiKey.empty()) {
            headers.push_back({"Authorization", "Bearer " + options.apiKey});
        }
        
        Logger::Info("Fetching Unity Asset Store asset: " + apiUrl);
        HttpResponse dlResp = m_httpClient->DownloadFile(apiUrl, localPath, headers);
        
        if (dlResp.IsError()) {
            result.errorMessage = "Failed to download Unity asset '" + assetId + "': " + dlResp.errorMessage;
            return result;
        }
        
        Logger::Info("Successfully downloaded Unity asset: " + assetId);
        result.success = true;
        result.localPath = localPath;
        result.metadata.name = assetId;
        return result;
    }
    
    result.errorMessage = "Unity asset not found in cache (no HTTP client configured): " + assetId;
    return result;
}

ImportResult UnityAssetStoreImporter::ImportAsset(
    const MarketplaceFetchResult& fetchResult,
    const MarketplaceImportOptions& options) {
    
    if (!fetchResult.success) {
        ImportResult result;
        result.errorMessage = "Cannot import failed fetch result";
        return result;
    }
    
    ImportResult result;
    
    // Convert Unity .prefab to intermediate format
    std::string intermediatePath = fetchResult.localPath + ".obj";
    
    if (!ConvertUnityPrefab(fetchResult.localPath, intermediatePath)) {
        result.errorMessage = "Failed to convert Unity asset to intermediate format";
        return result;
    }
    
    // Import the intermediate format using standard importer
    AssetImportRegistry standardRegistry;
    standardRegistry.RegisterImporter(std::make_unique<MeshImporter>(ImportFormat::OBJ));
    
    ImportOptions importOpts;
    importOpts.outputDirectory = options.outputDirectory;
    
    return standardRegistry.Import(intermediatePath, importOpts);
}

bool UnityAssetStoreImporter::IsAvailable() const {
    return m_httpClient != nullptr || !m_apiCredential.empty();
}

void UnityAssetStoreImporter::SetApiCredential(const std::string& cred) {
    m_apiCredential = cred;
}

bool UnityAssetStoreImporter::HasApiCredential() const {
    return !m_apiCredential.empty();
}

void UnityAssetStoreImporter::SetHttpClient(IHttpClient* client) {
    m_httpClient = client;
}

bool UnityAssetStoreImporter::ConvertUnityPrefab(
    const std::string& inputPath,
    const std::string& outputPath) const {
    
    if (!FileExists(inputPath)) {
        return false;
    }

    std::ifstream in(inputPath);
    if (!in.is_open()) return false;

    Logger::Info("Parsing Unity prefab: " + inputPath);

    std::string line;
    std::string name;
    std::string meshData;
    std::string materials;
    bool foundAnyKey = false;

    while (std::getline(in, line)) {
        auto colonPos = line.find(':');
        if (colonPos == std::string::npos) continue;

        std::string key = line.substr(0, colonPos);
        // Trim leading whitespace from key
        size_t keyStart = key.find_first_not_of(" \t");
        if (keyStart != std::string::npos) key = key.substr(keyStart);

        std::string value = (colonPos + 1 < line.size()) ? line.substr(colonPos + 1) : "";
        // Trim leading whitespace from value
        size_t valStart = value.find_first_not_of(" \t");
        if (valStart != std::string::npos) value = value.substr(valStart);

        if (key == "m_Name") {
            name = value;
            foundAnyKey = true;
        } else if (key == "m_MeshData") {
            meshData = value;
            foundAnyKey = true;
        } else if (key == "m_Materials") {
            materials = value;
            foundAnyKey = true;
        }
    }
    in.close();

    if (!foundAnyKey) {
        Logger::Error("No valid Unity prefab keys found in: " + inputPath);
        return false;
    }

    // Write extracted data as intermediate format
    std::ofstream out(outputPath);
    if (!out.is_open()) return false;

    out << "# Atlas Intermediate Format\n";
    out << "name: " << name << "\n";
    out << "mesh_data: " << meshData << "\n";
    out << "materials: " << materials << "\n";
    out.close();

    Logger::Info("Successfully converted Unity prefab: " + inputPath);
    return true;
}

bool UnityAssetStoreImporter::ParsePrefabHeader(
    const std::string& inputPath, MarketplaceAssetMetadata& outMeta) const {

    std::ifstream in(inputPath);
    if (!in.is_open()) return false;

    std::string line;
    bool foundName = false;

    while (std::getline(in, line)) {
        auto colonPos = line.find(':');
        if (colonPos == std::string::npos) continue;

        std::string key = line.substr(0, colonPos);
        size_t keyStart = key.find_first_not_of(" \t");
        if (keyStart != std::string::npos) key = key.substr(keyStart);

        std::string value = (colonPos + 1 < line.size()) ? line.substr(colonPos + 1) : "";
        size_t valStart = value.find_first_not_of(" \t");
        if (valStart != std::string::npos) value = value.substr(valStart);

        if (key == "m_Name") {
            outMeta.name = value;
            foundName = true;
        } else if (key == "m_TagString") {
            outMeta.tags.push_back(value);
        } else if (key == "m_Layer") {
            outMeta.description = "Layer: " + value;
        }
    }
    in.close();

    outMeta.marketplace = MarketplaceType::UnityAssetStore;
    Logger::Info("ParsePrefabHeader: name=" + outMeta.name);
    return foundName;
}

// ---------------------------------------------------------------------------
// MarketplaceHotReloader
// ---------------------------------------------------------------------------

void MarketplaceHotReloader::Watch(const std::string& assetId,
                                   MarketplaceType marketplace,
                                   const std::string& localPath,
                                   uint64_t hash,
                                   const std::string& version) {
    for (auto& e : m_entries) {
        if (e.assetId == assetId) {
            e.localPath = localPath;
            e.lastHash = hash;
            e.currentVersion = version;
            e.dirty = false;
            return;
        }
    }
    HotReloadEntry entry;
    entry.assetId = assetId;
    entry.marketplace = marketplace;
    entry.localPath = localPath;
    entry.lastHash = hash;
    entry.currentVersion = version;
    entry.dirty = false;
    m_entries.push_back(entry);
}

bool MarketplaceHotReloader::Unwatch(const std::string& assetId) {
    auto it = std::remove_if(m_entries.begin(), m_entries.end(),
        [&](const HotReloadEntry& e) { return e.assetId == assetId; });
    if (it == m_entries.end()) return false;
    m_entries.erase(it, m_entries.end());
    return true;
}

size_t MarketplaceHotReloader::CheckForUpdates() {
    size_t dirtyCount = 0;
    for (auto& e : m_entries) {
        if (!FileExists(e.localPath)) continue;
        uint64_t currentHash = HashFile(e.localPath);
        if (currentHash != e.lastHash) {
            e.dirty = true;
            ++dirtyCount;
        }
    }
    return dirtyCount;
}

std::vector<HotReloadEntry> MarketplaceHotReloader::DirtyAssets() const {
    std::vector<HotReloadEntry> result;
    for (const auto& e : m_entries) {
        if (e.dirty) result.push_back(e);
    }
    return result;
}

void MarketplaceHotReloader::ClearDirty(const std::string& assetId) {
    for (auto& e : m_entries) {
        if (e.assetId == assetId) {
            e.dirty = false;
            e.lastHash = HashFile(e.localPath);
            return;
        }
    }
}

size_t MarketplaceHotReloader::WatchCount() const {
    return m_entries.size();
}

bool MarketplaceHotReloader::IsWatching(const std::string& assetId) const {
    for (const auto& e : m_entries) {
        if (e.assetId == assetId) return true;
    }
    return false;
}

const HotReloadEntry* MarketplaceHotReloader::GetEntry(const std::string& assetId) const {
    for (const auto& e : m_entries) {
        if (e.assetId == assetId) return &e;
    }
    return nullptr;
}

uint64_t MarketplaceHotReloader::HashFile(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in.is_open()) return 0;

    // FNV-1a 64-bit hash
    uint64_t hash = 14695981039346656037ULL;
    char buf[4096];
    while (in.read(buf, sizeof(buf))) {
        for (std::streamsize i = 0; i < in.gcount(); ++i) {
            hash ^= static_cast<uint64_t>(static_cast<uint8_t>(buf[i]));
            hash *= 1099511628211ULL;
        }
    }
    for (std::streamsize i = 0; i < in.gcount(); ++i) {
        hash ^= static_cast<uint64_t>(static_cast<uint8_t>(buf[i]));
        hash *= 1099511628211ULL;
    }
    return hash;
}

// ---------------------------------------------------------------------------
// AssetValidationDashboard
// ---------------------------------------------------------------------------

bool AssetValidationReport::AllPassed() const {
    for (const auto& c : checks) {
        if (c.status == ValidationStatus::Fail) return false;
    }
    return true;
}

size_t AssetValidationReport::FailCount() const {
    size_t n = 0;
    for (const auto& c : checks) {
        if (c.status == ValidationStatus::Fail) ++n;
    }
    return n;
}

size_t AssetValidationReport::WarnCount() const {
    size_t n = 0;
    for (const auto& c : checks) {
        if (c.status == ValidationStatus::Warning) ++n;
    }
    return n;
}

AssetValidationReport AssetValidationDashboard::Validate(
    const std::string& assetId, const std::string& localPath) const {

    AssetValidationReport report;
    report.assetId = assetId;
    report.localPath = localPath;

    // Check 1: File existence
    {
        ValidationCheckResult r;
        r.checkName = "FileExists";
        if (FileExists(localPath)) {
            r.status = ValidationStatus::Pass;
            r.message = "File exists";
        } else {
            r.status = ValidationStatus::Fail;
            r.message = "File not found: " + localPath;
        }
        report.checks.push_back(r);
    }

    if (!FileExists(localPath)) return report;

    // Check 2: File size
    {
        ValidationCheckResult r;
        r.checkName = "FileSize";
        auto sz = std::filesystem::file_size(localPath);
        if (sz == 0) {
            r.status = ValidationStatus::Fail;
            r.message = "File is empty";
        } else if (sz > 512ULL * 1024 * 1024) {
            r.status = ValidationStatus::Warning;
            r.message = "File exceeds 512 MB (" + std::to_string(sz) + " bytes)";
        } else {
            r.status = ValidationStatus::Pass;
            r.message = std::to_string(sz) + " bytes";
        }
        report.checks.push_back(r);
    }

    // Check 3: Extension recognized
    {
        ValidationCheckResult r;
        r.checkName = "ExtensionRecognized";
        std::string ext = GetFileExtension(localPath);
        static const std::vector<std::string> knownExts = {
            ".png", ".obj", ".fbx", ".gltf", ".wav", ".ogg", ".flac",
            ".ttf", ".otf", ".uasset", ".prefab", ".dds", ".tga",
        };
        bool known = false;
        for (const auto& ke : knownExts) {
            if (ext == ke) { known = true; break; }
        }
        if (known) {
            r.status = ValidationStatus::Pass;
            r.message = "Extension: " + ext;
        } else {
            r.status = ValidationStatus::Warning;
            r.message = "Unrecognized extension: " + ext;
        }
        report.checks.push_back(r);
    }

    // Check 4: Readable
    {
        ValidationCheckResult r;
        r.checkName = "Readable";
        std::ifstream in(localPath, std::ios::binary);
        if (in.is_open()) {
            r.status = ValidationStatus::Pass;
            r.message = "File is readable";
        } else {
            r.status = ValidationStatus::Fail;
            r.message = "Cannot open file for reading";
        }
        report.checks.push_back(r);
    }

    return report;
}

std::vector<AssetValidationReport> AssetValidationDashboard::ValidateDirectory(
    const std::string& dir) const {

    std::vector<AssetValidationReport> reports;
    if (!std::filesystem::exists(dir)) return reports;

    for (const auto& entry : std::filesystem::directory_iterator(dir)) {
        if (entry.is_regular_file()) {
            std::string name = entry.path().filename().string();
            reports.push_back(Validate(name, entry.path().string()));
        }
    }
    return reports;
}

std::string AssetValidationDashboard::SummaryString(const AssetValidationReport& report) {
    std::string s = report.assetId + ": ";
    if (report.AllPassed()) {
        s += "PASS (" + std::to_string(report.checks.size()) + " checks)";
    } else {
        s += std::to_string(report.FailCount()) + " FAIL, "
           + std::to_string(report.WarnCount()) + " WARN";
    }
    return s;
}

// ---------------------------------------------------------------------------
// ModAssetSandbox
// ---------------------------------------------------------------------------

void ModAssetSandbox::RegisterMod(const std::string& modId,
                                  const ModSandboxBudget& budget) {
    ModEntry entry;
    entry.budget = budget;
    m_mods[modId] = entry;
}

bool ModAssetSandbox::UnregisterMod(const std::string& modId) {
    return m_mods.erase(modId) > 0;
}

bool ModAssetSandbox::AddAsset(const std::string& modId,
                               const std::string& assetPath,
                               uint64_t expectedHash) {
    auto it = m_mods.find(modId);
    if (it == m_mods.end()) return false;

    // Verify hash
    if (!VerifyAsset(assetPath, expectedHash)) return false;

    // Check budget
    auto& entry = it->second;
    if (entry.budget.currentAssetCount >= entry.budget.maxAssetCount) return false;

    size_t fileSize = 0;
    if (FileExists(assetPath)) {
        fileSize = static_cast<size_t>(std::filesystem::file_size(assetPath));
    }
    if (fileSize > entry.budget.maxTotalBytes - entry.budget.currentTotalBytes) return false;

    entry.assets.push_back(assetPath);
    entry.budget.currentAssetCount++;
    entry.budget.currentTotalBytes += fileSize;
    return true;
}

bool ModAssetSandbox::RemoveAsset(const std::string& modId,
                                  const std::string& assetPath) {
    auto it = m_mods.find(modId);
    if (it == m_mods.end()) return false;

    auto& assets = it->second.assets;
    auto ait = std::find(assets.begin(), assets.end(), assetPath);
    if (ait == assets.end()) return false;

    size_t fileSize = 0;
    if (FileExists(assetPath)) {
        fileSize = static_cast<size_t>(std::filesystem::file_size(assetPath));
    }

    assets.erase(ait);
    auto& budget = it->second.budget;
    if (budget.currentAssetCount > 0) budget.currentAssetCount--;
    if (budget.currentTotalBytes >= fileSize) {
        budget.currentTotalBytes -= fileSize;
    } else {
        budget.currentTotalBytes = 0;
    }
    return true;
}

bool ModAssetSandbox::HasMod(const std::string& modId) const {
    return m_mods.find(modId) != m_mods.end();
}

const ModSandboxBudget* ModAssetSandbox::GetBudget(const std::string& modId) const {
    auto it = m_mods.find(modId);
    if (it == m_mods.end()) return nullptr;
    return &it->second.budget;
}

size_t ModAssetSandbox::AssetCount(const std::string& modId) const {
    auto it = m_mods.find(modId);
    if (it == m_mods.end()) return 0;
    return it->second.assets.size();
}

std::vector<std::string> ModAssetSandbox::Assets(const std::string& modId) const {
    auto it = m_mods.find(modId);
    if (it == m_mods.end()) return {};
    return it->second.assets;
}

bool ModAssetSandbox::VerifyAsset(const std::string& assetPath,
                                  uint64_t expectedHash) const {
    uint64_t actual = HashFile(assetPath);
    return actual == expectedHash;
}

size_t ModAssetSandbox::ModCount() const {
    return m_mods.size();
}

uint64_t ModAssetSandbox::HashFile(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in.is_open()) return 0;

    uint64_t hash = 14695981039346656037ULL;
    char buf[4096];
    while (in.read(buf, sizeof(buf))) {
        for (std::streamsize i = 0; i < in.gcount(); ++i) {
            hash ^= static_cast<uint64_t>(static_cast<uint8_t>(buf[i]));
            hash *= 1099511628211ULL;
        }
    }
    for (std::streamsize i = 0; i < in.gcount(); ++i) {
        hash ^= static_cast<uint64_t>(static_cast<uint8_t>(buf[i]));
        hash *= 1099511628211ULL;
    }
    return hash;
}

}  // namespace atlas::asset
