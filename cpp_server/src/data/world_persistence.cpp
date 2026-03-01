#include "data/world_persistence.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace atlas {
namespace data {

// ---------------------------------------------------------------------------
// File I/O
// ---------------------------------------------------------------------------

bool WorldPersistence::saveWorld(const ecs::World* world,
                                 const std::string& filepath) {
    std::string json = serializeWorld(world);

    std::ofstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "[WorldPersistence] Cannot open file for writing: "
                  << filepath << std::endl;
        return false;
    }

    file << json;
    file.close();

    std::cout << "[WorldPersistence] World saved to " << filepath << std::endl;
    return true;
}

bool WorldPersistence::loadWorld(ecs::World* world,
                                 const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "[WorldPersistence] Cannot open file for reading: "
                  << filepath << std::endl;
        return false;
    }

    std::ostringstream ss;
    ss << file.rdbuf();
    file.close();

    return deserializeWorld(world, ss.str());
}

} // namespace data
} // namespace atlas
