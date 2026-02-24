#pragma once
// ============================================================
// Atlas Asset Import Pipeline — External Format Conversion
// ============================================================
//
// Converts external asset formats (FBX, OBJ, glTF, PNG, etc.)
// into Atlas's internal .atlasb binary format. Each concrete
// importer handles a specific family of source formats and
// produces an AssetHeader + raw payload on disk.
//
// See: docs/IMPLEMENTATION_PATH.md (Priority 5.1)

#include "AssetFormat.h"
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace atlas::asset {

/// Supported source formats for asset import.
enum class ImportFormat : uint16_t {
    FBX,         // Autodesk FBX mesh/animation
    OBJ,         // Wavefront OBJ mesh
    GLTF,        // glTF 2.0 mesh
    PNG,         // PNG texture
    DDS,         // DirectDraw Surface texture
    TGA,         // Targa texture
    WAV,         // WAV audio
    OGG,         // Ogg Vorbis audio
    FLAC,        // FLAC audio
    TTF,         // TrueType font
    OTF,         // OpenType font
    SpriteSheet, // Sprite sheet with sequence metadata
};

/// Result of an import operation.
struct ImportResult {
    bool success = false;
    std::string outputPath;      // Path to the generated .atlasb file
    AssetType outputType;        // Type of the generated asset
    uint64_t outputHash = 0;     // Hash of the generated asset
    std::string errorMessage;    // Non-empty on failure
};

/// Options controlling import behavior.
struct ImportOptions {
    bool generateMipmaps = false;
    bool compressTextures = false;
    bool calculateNormals = true;
    bool flipUVs = false;
    uint32_t maxTextureSize = 4096;
    std::string outputDirectory;
};

/// Abstract base class for asset importers.
class IAssetImporter {
public:
    virtual ~IAssetImporter() = default;
    virtual ImportFormat Format() const = 0;
    virtual AssetType OutputType() const = 0;
    virtual bool CanImport(const std::string& filePath) const = 0;
    virtual ImportResult Import(const std::string& filePath, const ImportOptions& options) = 0;
};

/// Central registry for asset importers.
class AssetImportRegistry {
public:
    void RegisterImporter(std::unique_ptr<IAssetImporter> importer);
    IAssetImporter* FindImporter(const std::string& filePath) const;
    IAssetImporter* FindImporterByFormat(ImportFormat format) const;
    ImportResult Import(const std::string& filePath, const ImportOptions& options);
    std::vector<ImportFormat> SupportedFormats() const;
    size_t ImporterCount() const;

private:
    std::vector<std::unique_ptr<IAssetImporter>> m_importers;
};

// --- Concrete importers ---

class MeshImporter : public IAssetImporter {
public:
    explicit MeshImporter(ImportFormat format);
    ImportFormat Format() const override;
    AssetType OutputType() const override;
    bool CanImport(const std::string& filePath) const override;
    ImportResult Import(const std::string& filePath, const ImportOptions& options) override;

private:
    ImportFormat m_format;
};

class TextureImporter : public IAssetImporter {
public:
    explicit TextureImporter(ImportFormat format);
    ImportFormat Format() const override;
    AssetType OutputType() const override;
    bool CanImport(const std::string& filePath) const override;
    ImportResult Import(const std::string& filePath, const ImportOptions& options) override;

private:
    ImportFormat m_format;
};

class AudioImporter : public IAssetImporter {
public:
    explicit AudioImporter(ImportFormat format);
    ImportFormat Format() const override;
    AssetType OutputType() const override;
    bool CanImport(const std::string& filePath) const override;
    ImportResult Import(const std::string& filePath, const ImportOptions& options) override;

private:
    ImportFormat m_format;
};

class FontImporter : public IAssetImporter {
public:
    explicit FontImporter(ImportFormat format);
    ImportFormat Format() const override;
    AssetType OutputType() const override;
    bool CanImport(const std::string& filePath) const override;
    ImportResult Import(const std::string& filePath, const ImportOptions& options) override;

private:
    ImportFormat m_format;
};

class SpriteSheetImporter : public IAssetImporter {
public:
    ImportFormat Format() const override;
    AssetType OutputType() const override;
    bool CanImport(const std::string& filePath) const override;
    ImportResult Import(const std::string& filePath, const ImportOptions& options) override;
};

}  // namespace atlas::asset
