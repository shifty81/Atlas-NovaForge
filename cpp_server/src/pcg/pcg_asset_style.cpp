#include "pcg/pcg_asset_style.h"

#include <sstream>
#include <algorithm>
#include <cmath>

namespace atlas {
namespace pcg {

// ── Surface treatment names ────────────────────────────────────────

const char* AssetStyleLibrary::surfaceTreatmentName(SurfaceTreatment t) {
    switch (t) {
        case SurfaceTreatment::None:          return "None";
        case SurfaceTreatment::PanelLines:    return "PanelLines";
        case SurfaceTreatment::Greeble:       return "Greeble";
        case SurfaceTreatment::Weathered:     return "Weathered";
        case SurfaceTreatment::BattleScarred: return "BattleScarred";
        case SurfaceTreatment::Pristine:      return "Pristine";
    }
    return "Unknown";
}

// ── Library construction ───────────────────────────────────────────

AssetStyleLibrary::AssetStyleLibrary() = default;

// ── Library management ─────────────────────────────────────────────

void AssetStyleLibrary::addStyle(const AssetStyle& style) {
    // Replace existing style with the same name.
    for (auto& s : styles_) {
        if (s.name == style.name) {
            s = style;
            return;
        }
    }
    styles_.push_back(style);
}

bool AssetStyleLibrary::removeStyle(const std::string& name) {
    auto it = std::remove_if(styles_.begin(), styles_.end(),
        [&](const AssetStyle& s) { return s.name == name; });
    if (it == styles_.end()) return false;
    styles_.erase(it, styles_.end());
    return true;
}

const AssetStyle* AssetStyleLibrary::findStyle(
        const std::string& name) const {
    for (const auto& s : styles_) {
        if (s.name == name) return &s;
    }
    return nullptr;
}

std::vector<std::string> AssetStyleLibrary::listStyles(
        GenerationStyleType type) const {
    std::vector<std::string> result;
    for (const auto& s : styles_) {
        if (s.targetType == type) {
            result.push_back(s.name);
        }
    }
    return result;
}

std::vector<std::string> AssetStyleLibrary::listAll() const {
    std::vector<std::string> result;
    result.reserve(styles_.size());
    for (const auto& s : styles_) {
        result.push_back(s.name);
    }
    return result;
}

size_t AssetStyleLibrary::size() const {
    return styles_.size();
}

void AssetStyleLibrary::clear() {
    styles_.clear();
}

// ── Shape application helpers ──────────────────────────────────────

/// Find the nearest control point and compute its influence on (px,py,pz).
static float computeInfluence(const ShapeProfile& shape,
                              float px, float py, float pz,
                              float& outScaleX,
                              float& outScaleY,
                              float& outScaleZ) {
    outScaleX = 1.0f;
    outScaleY = 1.0f;
    outScaleZ = 1.0f;

    if (shape.controlPoints.empty()) return 0.0f;

    float totalWeight = 0.0f;
    float accSX = 0.0f, accSY = 0.0f, accSZ = 0.0f;

    for (const auto& cp : shape.controlPoints) {
        float dx = px - cp.posX;
        float dy = py - cp.posY;
        float dz = pz - cp.posZ;
        float dist = std::sqrt(dx * dx + dy * dy + dz * dz);

        // Inverse-distance weighting with smoothing falloff.
        float falloff = 1.0f / (1.0f + dist * (1.0f - shape.smoothing + 0.01f));
        float w = cp.weight * falloff;

        accSX += cp.scaleX * w;
        accSY += cp.scaleY * w;
        accSZ += cp.scaleZ * w;
        totalWeight += w;
    }

    if (totalWeight > 0.0f) {
        outScaleX = accSX / totalWeight;
        outScaleY = accSY / totalWeight;
        outScaleZ = accSZ / totalWeight;
    }

    return totalWeight;
}

// ── Apply shape to ship ────────────────────────────────────────────

void AssetStyleLibrary::applyShapeToShip(GeneratedShip& ship,
                                          const ShapeProfile& shape) {
    if (shape.controlPoints.empty()) return;

    // Compute aggregate scale from all control points at the ship's
    // centre of mass (0,0,0).
    float sx, sy, sz;
    computeInfluence(shape, 0.0f, 0.0f, 0.0f, sx, sy, sz);

    // Scale the ship's geometry-influencing parameters.
    float avgScale = (sx + sy + sz) / 3.0f;
    if (avgScale > 0.001f) {
        ship.mass            *= avgScale;
        ship.signatureRadius *= (sx + sy) * 0.5f;
    }
}

// ── Apply shape to station ─────────────────────────────────────────

void AssetStyleLibrary::applyShapeToStation(GeneratedStation& station,
                                             const ShapeProfile& shape) {
    if (shape.controlPoints.empty()) return;

    for (auto& mod : station.modules) {
        float sx, sy, sz;
        computeInfluence(shape, mod.posX, mod.posY, mod.posZ,
                         sx, sy, sz);

        // Warp module dimensions by the local shape influence.
        mod.dimX *= sx;
        mod.dimY *= sy;
        mod.dimZ *= sz;

        // Mirror if requested.
        if (shape.mirrorX) {
            // Create implied symmetric deformation (noop on position,
            // but scale is applied symmetrically by the influence fn).
        }
    }
}

// ── Apply palette to ship ──────────────────────────────────────────

void AssetStyleLibrary::applyPaletteToShip(GeneratedShip& ship,
                                            const StylePalette& palette) {
    // Palette affects rendering properties which are not stored in
    // GeneratedShip directly.  For now we mark that a palette was
    // applied so downstream renderers can query it.  In a full
    // implementation the ship's material slots would be set here.
    (void)ship;
    (void)palette;
}

// ── Apply palette to station ───────────────────────────────────────

void AssetStyleLibrary::applyPaletteToStation(GeneratedStation& station,
                                               const StylePalette& palette) {
    (void)station;
    (void)palette;
}

// ── Combined application ───────────────────────────────────────────

void AssetStyleLibrary::applyToShip(GeneratedShip& ship,
                                     const AssetStyle& style) {
    applyShapeToShip(ship, style.shape);
    applyPaletteToShip(ship, style.palette);
}

void AssetStyleLibrary::applyToStation(GeneratedStation& station,
                                        const AssetStyle& style) {
    applyShapeToStation(station, style.shape);
    applyPaletteToStation(station, style.palette);
}

// ── Serialisation ──────────────────────────────────────────────────

std::string AssetStyleLibrary::serialize(const AssetStyle& style) {
    std::ostringstream os;
    os << "ASSET_STYLE_V1\n";
    os << "name=" << style.name << "\n";
    os << "target=" << static_cast<int>(style.targetType) << "\n";
    os << "version=" << style.version << "\n";

    // Shape profile.
    os << "shape_name=" << style.shape.name << "\n";
    os << "shape_mirrorX=" << (style.shape.mirrorX ? 1 : 0) << "\n";
    os << "shape_mirrorY=" << (style.shape.mirrorY ? 1 : 0) << "\n";
    os << "shape_smoothing=" << style.shape.smoothing << "\n";
    os << "shape_points=" << style.shape.controlPoints.size() << "\n";
    for (const auto& cp : style.shape.controlPoints) {
        os << "CP"
           << " " << cp.posX << " " << cp.posY << " " << cp.posZ
           << " " << cp.scaleX << " " << cp.scaleY << " " << cp.scaleZ
           << " " << cp.weight << "\n";
    }

    // Palette.
    os << "palette_name=" << style.palette.name << "\n";
    os << "palette_treatment=" << static_cast<int>(
            style.palette.surfaceTreatment) << "\n";
    os << "palette_detail=" << style.palette.detailLevel << "\n";
    os << "palette_colors=" << style.palette.colors.size() << "\n";
    for (const auto& c : style.palette.colors) {
        os << "COL " << c.r << " " << c.g << " " << c.b << " " << c.a
           << " " << c.regionName << "\n";
    }
    os << "palette_materials=" << style.palette.materials.size() << "\n";
    for (const auto& m : style.palette.materials) {
        os << "MAT " << m.metallic << " " << m.roughness
           << " " << m.emissive << " " << m.name << "\n";
    }

    os << "END\n";
    return os.str();
}

AssetStyle AssetStyleLibrary::deserialize(const std::string& data) {
    AssetStyle style{};
    style.valid = false;

    std::istringstream is(data);
    std::string line;

    if (!std::getline(is, line) || line != "ASSET_STYLE_V1") return style;

    auto readVal = [&](const std::string& key) -> std::string {
        if (!std::getline(is, line)) return "";
        auto pos = line.find('=');
        if (pos == std::string::npos) return "";
        if (line.substr(0, pos) != key) return "";
        return line.substr(pos + 1);
    };

    style.name       = readVal("name");
    style.targetType = static_cast<GenerationStyleType>(
                           std::stoi(readVal("target")));
    style.version    = static_cast<uint32_t>(
                           std::stoul(readVal("version")));

    // Shape.
    style.shape.name      = readVal("shape_name");
    style.shape.mirrorX   = (readVal("shape_mirrorX") == "1");
    style.shape.mirrorY   = (readVal("shape_mirrorY") == "1");
    style.shape.smoothing = std::stof(readVal("shape_smoothing"));

    int cpCount = std::stoi(readVal("shape_points"));
    for (int i = 0; i < cpCount; ++i) {
        if (!std::getline(is, line)) break;
        std::istringstream ls(line);
        std::string tag;
        ShapeControlPoint cp{};
        ls >> tag >> cp.posX >> cp.posY >> cp.posZ
           >> cp.scaleX >> cp.scaleY >> cp.scaleZ >> cp.weight;
        style.shape.controlPoints.push_back(cp);
    }

    // Palette.
    style.palette.name = readVal("palette_name");
    style.palette.surfaceTreatment = static_cast<SurfaceTreatment>(
        std::stoi(readVal("palette_treatment")));
    style.palette.detailLevel = std::stof(readVal("palette_detail"));

    int colCount = std::stoi(readVal("palette_colors"));
    for (int i = 0; i < colCount; ++i) {
        if (!std::getline(is, line)) break;
        std::istringstream ls(line);
        std::string tag;
        StyleColor c{};
        ls >> tag >> c.r >> c.g >> c.b >> c.a;
        std::getline(ls >> std::ws, c.regionName);
        style.palette.colors.push_back(c);
    }

    int matCount = std::stoi(readVal("palette_materials"));
    for (int i = 0; i < matCount; ++i) {
        if (!std::getline(is, line)) break;
        std::istringstream ls(line);
        std::string tag;
        StyleMaterial m{};
        ls >> tag >> m.metallic >> m.roughness >> m.emissive;
        std::getline(ls >> std::ws, m.name);
        style.palette.materials.push_back(m);
    }

    style.valid = true;
    return style;
}

} // namespace pcg
} // namespace atlas
