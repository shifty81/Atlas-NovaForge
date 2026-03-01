#include "data/world_persistence.h"
#include <fstream>
#include <vector>
#include <iostream>
#include <zlib.h>

namespace atlas {
namespace data {

bool WorldPersistence::saveWorldCompressed(const ecs::World* world,
                                           const std::string& filepath) {
    std::string json = serializeWorld(world);
    if (json.empty()) {
        std::cerr << "[WorldPersistence] Nothing to compress" << std::endl;
        return false;
    }

    gzFile gz = gzopen(filepath.c_str(), "wb9");  // max compression
    if (!gz) {
        std::cerr << "[WorldPersistence] Cannot open compressed file for writing: "
                  << filepath << std::endl;
        return false;
    }

    int written = gzwrite(gz, json.data(), static_cast<unsigned>(json.size()));
    gzclose(gz);

    if (written <= 0) {
        std::cerr << "[WorldPersistence] Compressed write failed" << std::endl;
        return false;
    }

    std::cout << "[WorldPersistence] World saved (compressed) to " << filepath
              << " (" << written << " bytes source, compressed on disk)" << std::endl;
    return true;
}

bool WorldPersistence::loadWorldCompressed(ecs::World* world,
                                           const std::string& filepath) {
    gzFile gz = gzopen(filepath.c_str(), "rb");
    if (!gz) {
        std::cerr << "[WorldPersistence] Cannot open compressed file for reading: "
                  << filepath << std::endl;
        return false;
    }

    std::string json;
    char buf[8192];
    int n;
    while ((n = gzread(gz, buf, sizeof(buf))) > 0) {
        json.append(buf, static_cast<size_t>(n));
    }
    gzclose(gz);

    if (json.empty()) {
        std::cerr << "[WorldPersistence] Decompressed data is empty" << std::endl;
        return false;
    }

    return deserializeWorld(world, json);
}

} // namespace data
} // namespace atlas
