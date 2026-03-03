/**
 * Test runner for all Atlas NovaForge server system tests.
 *
 * This file defines the global test counters and calls each domain's
 * test runner function.
 */

#include "test_framework.h"
#include <iostream>

namespace atlas::test {
int testsRun = 0;
int testsPassed = 0;
}

void run_character_tests();
void run_chat_tests();
void run_combat_tests();
void run_core_systems_tests();
void run_data_tests();
void run_economy_tests();
void run_fleet_tests();
void run_fps_tests();
void run_infrastructure_tests();
void run_inventory_tests();
void run_misc_tests();
void run_mission_tests();
void run_movement_tests();
void run_navigation_tests();
void run_pcg_tests();
void run_persistence_tests();
void run_social_tests();
void run_special_systems_tests();
void run_station_tests();
void run_targeting_tests();
void run_tournament_tests();
void run_ui_tools_tests();
void run_wormhole_tests();

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "Nova Forge C++ Server System Tests" << std::endl;
    std::cout << "========================================" << std::endl;

    run_character_tests();
    run_chat_tests();
    run_combat_tests();
    run_core_systems_tests();
    run_data_tests();
    run_economy_tests();
    run_fleet_tests();
    run_fps_tests();
    run_infrastructure_tests();
    run_inventory_tests();
    run_misc_tests();
    run_mission_tests();
    run_movement_tests();
    run_navigation_tests();
    run_pcg_tests();
    run_persistence_tests();
    run_social_tests();
    run_special_systems_tests();
    run_station_tests();
    run_targeting_tests();
    run_tournament_tests();
    run_ui_tools_tests();
    run_wormhole_tests();

    std::cout << "\n========================================" << std::endl;
    std::cout << "Results: " << atlas::test::testsPassed << "/" << atlas::test::testsRun
              << " tests passed" << std::endl;
    std::cout << "========================================" << std::endl;

    return (atlas::test::testsPassed == atlas::test::testsRun) ? 0 : 1;
}
