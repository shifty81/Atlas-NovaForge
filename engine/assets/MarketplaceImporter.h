#pragma once
// ============================================================
// Atlas Marketplace Asset Import — External Store Integration
// ============================================================
//
// Provides integration with external asset marketplaces to import
// assets directly from platforms like itch.io, Unreal Marketplace,
// and Unity Asset Store into Atlas's internal format.
//
// Each marketplace has specific format requirements and metadata
// handling that must be translated into Atlas's unified asset system.
//
// See: docs/03_ASSET_SYSTEM.md

#include "AssetImporter.h"
#include "HttpClient.h"
#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <unordered_map>

namespace atlas::asset {

/// Supported external marketplaces
enum class MarketplaceType {
    ItchIO,          // itch.io marketplace
    UnrealEngine,    // Unreal Engine Marketplace
    UnityAssetStore, // Unity Asset Store
};

/// Metadata for marketplace assets
struct MarketplaceAssetMetadata {
    std::string id;              // Marketplace-specific asset ID
    std::string name;            // Asset name
    std::string author;          // Asset author/creator
    std::string version;         // Asset version
    std::string description;     // Asset description
    std::vector<std::string> tags; // Asset tags/categories
    MarketplaceType marketplace; // Source marketplace
};

/// Result of a marketplace asset fetch operation
struct MarketplaceFetchResult {
    bool success = false;
    std::string localPath;       // Path to downloaded asset
    MarketplaceAssetMetadata metadata;
    std::string errorMessage;    // Non-empty on failure
};

/// Options for marketplace import
struct MarketplaceImportOptions : ImportOptions {
    bool validateAssets = true;   // Verify asset integrity
    bool preserveMetadata = true; // Keep marketplace metadata
    std::string apiKey;           // API key for marketplace (if required)
    std::string downloadCache;    // Cache directory for downloads
};

/// Base class for marketplace-specific importers
class IMarketplaceImporter {
public:
    virtual ~IMarketplaceImporter() = default;
    virtual MarketplaceType GetMarketplaceType() const = 0;
    
    /// Fetch an asset from the marketplace
    virtual MarketplaceFetchResult FetchAsset(
        const std::string& assetId,
        const MarketplaceImportOptions& options) = 0;
    
    /// Import a fetched asset into Atlas format
    virtual ImportResult ImportAsset(
        const MarketplaceFetchResult& fetchResult,
        const MarketplaceImportOptions& options) = 0;
    
    /// Check if the marketplace is accessible (API key valid, etc.)
    virtual bool IsAvailable() const = 0;
    
    /// Set the HTTP client for API downloads
    virtual void SetHttpClient(IHttpClient* client) = 0;
};

/// itch.io marketplace importer
/// Supports common formats: .png, .obj, .fbx from itch.io game assets
class ItchIOImporter : public IMarketplaceImporter {
public:
    MarketplaceType GetMarketplaceType() const override;
    MarketplaceFetchResult FetchAsset(
        const std::string& assetId,
        const MarketplaceImportOptions& options) override;
    ImportResult ImportAsset(
        const MarketplaceFetchResult& fetchResult,
        const MarketplaceImportOptions& options) override;
    bool IsAvailable() const override;
    void SetHttpClient(IHttpClient* client) override;
    
private:
    bool ValidateItchAsset(const std::string& path) const;
    IHttpClient* m_httpClient = nullptr;
};

/// Unreal Engine Marketplace importer
/// Handles .uasset and .umap formats and converts to Atlas-compatible formats
class UnrealMarketplaceImporter : public IMarketplaceImporter {
public:
    MarketplaceType GetMarketplaceType() const override;
    MarketplaceFetchResult FetchAsset(
        const std::string& assetId,
        const MarketplaceImportOptions& options) override;
    ImportResult ImportAsset(
        const MarketplaceFetchResult& fetchResult,
        const MarketplaceImportOptions& options) override;
    bool IsAvailable() const override;
    void SetHttpClient(IHttpClient* client) override;

    void SetApiCredential(const std::string& cred);
    bool HasApiCredential() const;

    /// Parse .uasset binary header and extract asset metadata
    bool ParseUAssetHeader(const std::string& inputPath, MarketplaceAssetMetadata& outMeta) const;

private:
    /// Convert Unreal .uasset to intermediate format
    bool ConvertUAsset(const std::string& inputPath, 
                       const std::string& outputPath) const;
    IHttpClient* m_httpClient = nullptr;
    std::string m_apiCredential;
};

/// Unity Asset Store importer
/// Handles .unity, .prefab, and .asset formats
class UnityAssetStoreImporter : public IMarketplaceImporter {
public:
    MarketplaceType GetMarketplaceType() const override;
    MarketplaceFetchResult FetchAsset(
        const std::string& assetId,
        const MarketplaceImportOptions& options) override;
    ImportResult ImportAsset(
        const MarketplaceFetchResult& fetchResult,
        const MarketplaceImportOptions& options) override;
    bool IsAvailable() const override;
    void SetHttpClient(IHttpClient* client) override;

    void SetApiCredential(const std::string& cred);
    bool HasApiCredential() const;

    /// Parse Unity .prefab YAML header and extract asset metadata
    bool ParsePrefabHeader(const std::string& inputPath, MarketplaceAssetMetadata& outMeta) const;

private:
    /// Convert Unity .prefab to intermediate format
    bool ConvertUnityPrefab(const std::string& inputPath,
                            const std::string& outputPath) const;
    IHttpClient* m_httpClient = nullptr;
    std::string m_apiCredential;
};

/// Central registry for marketplace importers
class MarketplaceImportRegistry {
public:
    void RegisterImporter(std::unique_ptr<IMarketplaceImporter> importer);
    IMarketplaceImporter* FindImporter(MarketplaceType type) const;
    
    /// Fetch and import asset from a marketplace in one operation
    ImportResult FetchAndImport(
        MarketplaceType marketplace,
        const std::string& assetId,
        const MarketplaceImportOptions& options);
    
    std::vector<MarketplaceType> AvailableMarketplaces() const;
    size_t ImporterCount() const;
    
    /// Set the HTTP client on all registered importers
    void SetHttpClient(IHttpClient* client);
    
private:
    std::vector<std::unique_ptr<IMarketplaceImporter>> m_importers;
};

// ---------------------------------------------------------------------------
// Hot-Reload for Marketplace Assets
// ---------------------------------------------------------------------------

/// Tracks a marketplace asset for hot-reload (file modification monitoring).
struct HotReloadEntry {
    std::string assetId;
    MarketplaceType marketplace = MarketplaceType::ItchIO;
    std::string localPath;
    uint64_t lastHash = 0;       ///< Hash of the local file when last imported
    std::string currentVersion;  ///< Version at last import
    bool dirty = false;          ///< True when change detected
};

/// Watches imported marketplace assets for changes and triggers re-import.
class MarketplaceHotReloader {
public:
    /// Register an imported asset for hot-reload monitoring.
    void Watch(const std::string& assetId, MarketplaceType marketplace,
               const std::string& localPath, uint64_t hash,
               const std::string& version = {});

    /// Unregister an asset from hot-reload monitoring.
    bool Unwatch(const std::string& assetId);

    /// Scan all watched assets and mark dirty any whose file hash changed.
    size_t CheckForUpdates();

    /// Return all currently dirty entries (assets that need re-import).
    std::vector<HotReloadEntry> DirtyAssets() const;

    /// Clear the dirty flag on an asset (e.g., after re-import).
    void ClearDirty(const std::string& assetId);

    /// Query
    size_t WatchCount() const;
    bool IsWatching(const std::string& assetId) const;
    const HotReloadEntry* GetEntry(const std::string& assetId) const;

private:
    std::vector<HotReloadEntry> m_entries;
    static uint64_t HashFile(const std::string& path);
};

// ---------------------------------------------------------------------------
// Asset Validation Dashboard
// ---------------------------------------------------------------------------

/// Result of a single asset validation check.
enum class ValidationStatus {
    Pass,
    Warning,
    Fail,
};

struct ValidationCheckResult {
    std::string checkName;
    ValidationStatus status = ValidationStatus::Pass;
    std::string message;
};

struct AssetValidationReport {
    std::string assetId;
    std::string localPath;
    std::vector<ValidationCheckResult> checks;

    bool AllPassed() const;
    size_t FailCount() const;
    size_t WarnCount() const;
};

/// Runs a suite of validation checks on marketplace-imported assets.
class AssetValidationDashboard {
public:
    /// Validate a single imported asset.
    AssetValidationReport Validate(const std::string& assetId,
                                   const std::string& localPath) const;

    /// Validate all assets in a directory.
    std::vector<AssetValidationReport> ValidateDirectory(const std::string& dir) const;

    /// Return a summary string suitable for display.
    static std::string SummaryString(const AssetValidationReport& report);
};

// ---------------------------------------------------------------------------
// Mod Asset Sandboxing
// ---------------------------------------------------------------------------

/// Per-mod resource budget (CPU time, memory, asset count).
struct ModSandboxBudget {
    size_t maxAssetCount = 256;
    size_t maxTotalBytes = 64 * 1024 * 1024; // 64 MB default
    size_t currentAssetCount = 0;
    size_t currentTotalBytes = 0;
};

/// Tracks which assets belong to a mod and enforces sandbox rules.
class ModAssetSandbox {
public:
    /// Register a mod sandbox with a given budget.
    void RegisterMod(const std::string& modId, const ModSandboxBudget& budget);

    /// Unregister a mod sandbox.
    bool UnregisterMod(const std::string& modId);

    /// Attempt to add an asset to a mod's sandbox.
    /// Returns false if the asset fails hash verification or exceeds budget.
    bool AddAsset(const std::string& modId, const std::string& assetPath,
                  uint64_t expectedHash);

    /// Remove an asset from a mod's sandbox.
    bool RemoveAsset(const std::string& modId, const std::string& assetPath);

    /// Query
    bool HasMod(const std::string& modId) const;
    const ModSandboxBudget* GetBudget(const std::string& modId) const;
    size_t AssetCount(const std::string& modId) const;
    std::vector<std::string> Assets(const std::string& modId) const;
    bool VerifyAsset(const std::string& assetPath, uint64_t expectedHash) const;
    size_t ModCount() const;

private:
    struct ModEntry {
        ModSandboxBudget budget;
        std::vector<std::string> assets;
    };
    std::unordered_map<std::string, ModEntry> m_mods;
    static uint64_t HashFile(const std::string& path);
};

}  // namespace atlas::asset
