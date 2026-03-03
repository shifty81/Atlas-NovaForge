/**
 * Economy tests
 */

#include "test_framework.h"
#include "systems/capacitor_system.h"
#include "systems/shield_recharge_system.h"
#include "systems/weapon_system.h"
#include "systems/targeting_system.h"
#include "data/ship_database.h"
#include "data/wormhole_database.h"
#include "systems/wormhole_system.h"
#include "systems/fleet_system.h"
#include "systems/mission_system.h"
#include "systems/skill_system.h"
#include "systems/module_system.h"
#include "systems/inventory_system.h"
#include "systems/loot_system.h"
#include "systems/drone_system.h"
#include "systems/insurance_system.h"
#include "systems/corporation_system.h"
#include "systems/bounty_system.h"
#include "systems/market_system.h"
#include "systems/contract_system.h"
#include "systems/pi_system.h"
#include "systems/manufacturing_system.h"
#include "systems/research_system.h"
#include "systems/chat_system.h"
#include "systems/character_creation_system.h"
#include "systems/tournament_system.h"
#include "systems/leaderboard_system.h"
#include "data/world_persistence.h"
#include "data/npc_database.h"
#include "systems/movement_system.h"
#include "systems/station_system.h"
#include "systems/wreck_salvage_system.h"
#include "systems/fleet_morale_system.h"
#include "systems/captain_personality_system.h"
#include "systems/fleet_chatter_system.h"
#include "systems/warp_anomaly_system.h"
#include "systems/captain_relationship_system.h"
#include "systems/emotional_arc_system.h"
#include "systems/fleet_cargo_system.h"
#include "systems/tactical_overlay_system.h"
#include "systems/combat_system.h"
#include "systems/ai_system.h"
#include "systems/mining_system.h"
#include "systems/fleet_formation_system.h"
#include "systems/captain_memory_system.h"
#include "systems/ship_fitting_system.h"
#include "systems/warp_cinematic_system.h"
#include "systems/warp_hud_travel_mode_system.h"
#include "systems/warp_auto_comfort_system.h"
#include "systems/refining_system.h"
#include "systems/anomaly_system.h"
#include "systems/scanner_system.h"
#include "systems/difficulty_scaling_system.h"
#include "systems/mission_template_system.h"
#include "systems/mission_generator_system.h"
#include "systems/reputation_system.h"
#include "systems/lod_system.h"
#include "systems/spatial_hash_system.h"
#include "systems/background_simulation_system.h"
#include "systems/npc_intent_system.h"
#include "systems/npc_behavior_tree_system.h"
#include "systems/combat_threat_system.h"
#include "systems/security_response_system.h"
#include "systems/ambient_traffic_system.h"
#include "systems/snapshot_replication_system.h"
#include "systems/interest_management_system.h"
#include "systems/fleet_progression_system.h"
#include "systems/station_deployment_system.h"
#include "systems/fleet_warp_formation_system.h"
#include "systems/warp_meditation_system.h"
#include "systems/rumor_questline_system.h"
#include "systems/captain_departure_system.h"
#include "systems/captain_transfer_system.h"
#include "systems/npc_rerouting_system.h"
#include "systems/local_reputation_system.h"
#include "systems/station_news_system.h"
#include "systems/wreck_persistence_system.h"
#include "systems/fleet_history_system.h"
#include "systems/rig_system.h"
#include "systems/legend_system.h"
#include "systems/ancient_tech_system.h"
#include "systems/docking_system.h"
#include "systems/survival_system.h"
#include "systems/menu_system.h"
#include "systems/crew_system.h"
#include "systems/salvage_exploration_system.h"
#include "systems/market_order_system.h"
#include "systems/ai_economic_actor_system.h"
#include "systems/turret_ai_system.h"
#include "systems/titan_assembly_system.h"
#include "systems/task_scheduler_system.h"
#include "systems/mod_manager_system.h"
#include "systems/ship_designer_system.h"
#include "systems/fleet_morale_resolution_system.h"
#include "systems/persistence_delta_system.h"
#include "systems/outer_rim_logistics_distortion_system.h"
#include "systems/rumor_propagation_system.h"
#include "systems/galactic_response_curve_system.h"
#include "systems/ship_capability_rating_system.h"
#include "systems/module_cascading_failure_system.h"
#include "systems/navigation_bookmark_system.h"
#include "systems/fleet_supply_line_system.h"
#include "systems/crew_training_system.h"
#include "systems/asteroid_belt_system.h"
#include "systems/scan_probe_system.h"
#include "systems/autopilot_system.h"
#include "network/protocol_handler.h"
#include "ui/server_console.h"
#include "utils/logger.h"
#include "utils/server_metrics.h"
#include "pcg/deterministic_rng.h"
#include "pcg/hash_utils.h"
#include "pcg/pcg_context.h"
#include "pcg/pcg_manager.h"
#include "pcg/ship_generator.h"
#include "pcg/fleet_doctrine.h"
#include "pcg/room_graph.h"
#include "pcg/deck_graph.h"
#include "pcg/elevator_system.h"
#include "pcg/hull_mesher.h"
#include "pcg/capital_ship_generator.h"
#include "pcg/ship_designer.h"
#include "pcg/snappable_grid.h"
#include "pcg/station_generator.h"
#include "pcg/salvage_system.h"
#include "pcg/rover_system.h"
#include "pcg/character_mesh_system.h"
#include "pcg/turret_generator.h"
#include "pcg/planet_generator.h"
#include "pcg/habitat_generator.h"
#include "pcg/grav_bike_generator.h"
#include "pcg/spine_hull_generator.h"
#include "pcg/terrain_generator.h"
#include "pcg/turret_placement_system.h"
#include "pcg/damage_state_generator.h"
#include "pcg/procedural_texture_generator.h"
#include "pcg/shield_effect_generator.h"
#include "pcg/economy_driven_generator.h"
#include "pcg/collision_manager.h"
#include "pcg/asteroid_field_generator.h"
#include "pcg/anomaly_generator.h"
#include "pcg/npc_encounter_generator.h"
#include "pcg/star_system_generator.h"
#include "pcg/galaxy_generator.h"
#include "systems/alliance_system.h"
#include "systems/sovereignty_system.h"
#include "systems/war_declaration_system.h"
#include "systems/convoy_ambush_system.h"
#include "systems/npc_dialogue_system.h"
#include "systems/station_monument_system.h"
#include "systems/information_propagation_system.h"
#include "systems/crew_activity_system.h"
#include "systems/visual_cue_system.h"
#include "systems/supply_demand_system.h"
#include "systems/black_market_system.h"
#include "systems/price_history_system.h"
#include "systems/propaganda_system.h"
#include "systems/rest_station_system.h"
#include "systems/myth_boss_system.h"
#include "systems/ancient_tech_upgrade_system.h"
#include "systems/mod_manifest_system.h"
#include "systems/ancient_ai_remnant_system.h"
#include "systems/character_creation_screen_system.h"
#include "systems/view_mode_transition_system.h"
#include "systems/station_hangar_system.h"
#include "systems/tether_docking_system.h"
#include "systems/fps_spawn_system.h"
#include "systems/fps_character_controller_system.h"
#include "systems/interior_door_system.h"
#include "systems/eva_airlock_system.h"
#include "systems/fps_interaction_system.h"
#include "systems/fps_combat_system.h"
#include "systems/fps_inventory_system.h"
#include "systems/ship_interior_layout_system.h"
#include "systems/environmental_hazard_system.h"
#include "systems/fps_objective_system.h"
#include "systems/wing_management_system.h"
#include "systems/economic_flow_system.h"
#include "systems/resource_production_chain_system.h"
#include "systems/fleet_doctrine_system.h"
#include "systems/player_progression_system.h"
#include "systems/terraforming_system.h"
#include "systems/food_processor_system.h"
#include "systems/fleet_squad_system.h"
#include "systems/cloaking_system.h"
#include "systems/jump_drive_system.h"
#include "systems/cargo_scan_system.h"
#include "systems/commander_disagreement_system.h"
#include "systems/imperfect_information_system.h"
#include "systems/captain_background_system.h"
#include "systems/rover_interior_system.h"
#include "systems/bike_garage_system.h"
#include "systems/visual_rig_system.h"
#include "systems/planetary_traversal_system.h"
#include "systems/solar_panel_system.h"
#include "systems/farming_deck_system.h"
#include "systems/docking_ring_extension_system.h"
#include "systems/rover_bay_ramp_system.h"
#include "systems/grid_construction_system.h"
#include "systems/space_planet_transition_system.h"
#include "systems/fleet_hangar_system.h"
#include "systems/hangar_environment_system.h"
#include "systems/dock_node_layout_system.h"
#include "systems/mission_consequence_system.h"
#include "systems/server_performance_monitor_system.h"
#include "systems/atlas_ui_panel_system.h"
#include "systems/keyboard_navigation_system.h"
#include "systems/data_binding_system.h"
#include "systems/entity_stress_test_system.h"
#include "systems/rig_locker_system.h"
#include "systems/visual_coupling_system.h"
#include "systems/fps_salvage_path_system.h"
#include "systems/lavatory_interaction_system.h"
#include "systems/eva_airlock_exit_system.h"
#include "systems/ancient_module_discovery_system.h"
#include "systems/client_prediction_system.h"
#include "systems/ship_template_mod_system.h"
#include "systems/database_persistence_system.h"
#include "systems/mission_editor_system.h"
#include "systems/content_validation_system.h"
#include "systems/cloud_deployment_config_system.h"
#include "systems/interest_management_priority_system.h"
#include "systems/visual_feedback_queue_system.h"
#include "systems/mod_doc_generator_system.h"
#include "systems/community_content_repository_system.h"
#include "systems/pvp_toggle_system.h"
#include "systems/dynamic_event_system.h"
#include "systems/jump_gate_system.h"
#include "systems/snapshot_replication_system2.h"
#include "systems/procedural_mission_generator_system.h"
#include "systems/incursion_system.h"
#include "systems/clone_bay_system.h"
#include "systems/loyalty_point_store_system.h"
#include <fstream>
#include <thread>
#include <sys/stat.h>

using namespace atlas;
using atlas::test::assertTrue;
using atlas::test::approxEqual;
using atlas::test::addComp;

// ==================== MarketSystem Tests ====================

void testMarketPlaceSellOrder() {
    std::cout << "\n=== Market Place Sell Order ===" << std::endl;
    ecs::World world;
    systems::MarketSystem marketSys(&world);

    auto* station = world.createEntity("station_1");
    auto* hub = addComp<components::MarketHub>(station);
    hub->station_id = "station_1";

    auto* seller = world.createEntity("seller_1");
    auto* pc = addComp<components::Player>(seller);
    pc->credits = 100000.0;

    std::string oid = marketSys.placeSellOrder("station_1", "seller_1", "stellium", "Stellium", 100, 5.0);
    assertTrue(!oid.empty(), "Sell order created");
    assertTrue(marketSys.getOrderCount("station_1") == 1, "One order on station");
    assertTrue(pc->credits < 100000.0, "Broker fee deducted from seller");
}

void testMarketBuyFromMarket() {
    std::cout << "\n=== Market Buy From Market ===" << std::endl;
    ecs::World world;
    systems::MarketSystem marketSys(&world);

    auto* station = world.createEntity("station_1");
    auto* hub = addComp<components::MarketHub>(station);
    hub->station_id = "station_1";

    auto* seller = world.createEntity("seller_1");
    auto* seller_pc = addComp<components::Player>(seller);
    seller_pc->credits = 100000.0;

    auto* buyer = world.createEntity("buyer_1");
    auto* buyer_pc = addComp<components::Player>(buyer);
    buyer_pc->credits = 100000.0;

    marketSys.placeSellOrder("station_1", "seller_1", "stellium", "Stellium", 100, 5.0);

    int bought = marketSys.buyFromMarket("station_1", "buyer_1", "stellium", 50);
    assertTrue(bought == 50, "Bought 50 units");
    assertTrue(buyer_pc->credits < 100000.0, "Buyer Credits decreased");
    assertTrue(seller_pc->credits > 100000.0 - 100000.0 * 0.02, "Seller Credits increased from sale");
}

void testMarketPriceQueries() {
    std::cout << "\n=== Market Price Queries ===" << std::endl;
    ecs::World world;
    systems::MarketSystem marketSys(&world);

    auto* station = world.createEntity("station_1");
    auto* hub = addComp<components::MarketHub>(station);
    hub->station_id = "station_1";

    auto* seller1 = world.createEntity("seller_1");
    auto* pc1 = addComp<components::Player>(seller1);
    pc1->credits = 1000000.0;

    auto* seller2 = world.createEntity("seller_2");
    auto* pc2 = addComp<components::Player>(seller2);
    pc2->credits = 1000000.0;

    auto* buyer1 = world.createEntity("buyer_1");
    auto* bpc = addComp<components::Player>(buyer1);
    bpc->credits = 1000000.0;

    marketSys.placeSellOrder("station_1", "seller_1", "stellium", "Stellium", 100, 5.0);
    marketSys.placeSellOrder("station_1", "seller_2", "stellium", "Stellium", 50, 4.5);
    marketSys.placeBuyOrder("station_1", "buyer_1", "stellium", "Stellium", 200, 4.0);

    double lowest = marketSys.getLowestSellPrice("station_1", "stellium");
    assertTrue(approxEqual(static_cast<float>(lowest), 4.5f), "Lowest sell is 4.5");

    double highest = marketSys.getHighestBuyPrice("station_1", "stellium");
    assertTrue(approxEqual(static_cast<float>(highest), 4.0f), "Highest buy is 4.0");

    double no_item = marketSys.getLowestSellPrice("station_1", "nonexistent");
    assertTrue(no_item < 0, "No sell price for nonexistent item");
}

void testMarketOrderExpiry() {
    std::cout << "\n=== Market Order Expiry ===" << std::endl;
    ecs::World world;
    systems::MarketSystem marketSys(&world);

    auto* station = world.createEntity("station_1");
    auto* hub = addComp<components::MarketHub>(station);
    hub->station_id = "station_1";

    auto* seller = world.createEntity("seller_1");
    auto* pc = addComp<components::Player>(seller);
    pc->credits = 1000000.0;

    marketSys.placeSellOrder("station_1", "seller_1", "stellium", "Stellium", 100, 5.0);
    assertTrue(marketSys.getOrderCount("station_1") == 1, "One active order");

    // Set order duration
    hub->orders[0].duration_remaining = 5.0f;

    marketSys.update(6.0f);
    assertTrue(marketSys.getOrderCount("station_1") == 0, "Order expired and removed");
}

// ==================== ContractSystem Tests ====================

void testContractCreate() {
    std::cout << "\n=== Contract Create ===" << std::endl;
    ecs::World world;
    systems::ContractSystem contractSys(&world);
    auto* station = world.createEntity("station_1");
    addComp<components::ContractBoard>(station);

    assertTrue(contractSys.createContract("station_1", "player_1", "item_exchange", 50000.0, 3600.0f),
               "Contract created successfully");
    assertTrue(contractSys.getActiveContractCount("station_1") == 1, "Active contract count is 1");
    assertTrue(contractSys.getContractsByStatus("station_1", "outstanding") == 1,
               "Outstanding contract count is 1");
}

void testContractAccept() {
    std::cout << "\n=== Contract Accept ===" << std::endl;
    ecs::World world;
    systems::ContractSystem contractSys(&world);
    auto* station = world.createEntity("station_1");
    addComp<components::ContractBoard>(station);

    contractSys.createContract("station_1", "player_1", "courier", 100000.0, -1.0f);
    auto* board = station->getComponent<components::ContractBoard>();
    std::string cid = board->contracts[0].contract_id;

    assertTrue(contractSys.acceptContract("station_1", cid, "player_2"),
               "Contract accepted");
    assertTrue(board->contracts[0].status == "in_progress", "Status changed to in_progress");
    assertTrue(board->contracts[0].assignee_id == "player_2", "Assignee set correctly");
    assertTrue(contractSys.getContractsByStatus("station_1", "outstanding") == 0,
               "No outstanding contracts after accept");
    assertTrue(contractSys.getContractsByStatus("station_1", "in_progress") == 1,
               "One in_progress contract after accept");
}

void testContractComplete() {
    std::cout << "\n=== Contract Complete ===" << std::endl;
    ecs::World world;
    systems::ContractSystem contractSys(&world);
    auto* station = world.createEntity("station_1");
    addComp<components::ContractBoard>(station);

    auto* acceptor = world.createEntity("player_2");
    auto* player = addComp<components::Player>(acceptor);
    player->credits = 10000.0;

    contractSys.createContract("station_1", "player_1", "item_exchange", 75000.0, -1.0f);
    auto* board = station->getComponent<components::ContractBoard>();
    std::string cid = board->contracts[0].contract_id;

    contractSys.acceptContract("station_1", cid, "player_2");
    assertTrue(contractSys.completeContract("station_1", cid), "Contract completed");
    assertTrue(board->contracts[0].status == "completed", "Status is completed");
    assertTrue(approxEqual(static_cast<float>(player->credits), 85000.0f), "Credits reward paid to acceptor");
}

void testContractExpiry() {
    std::cout << "\n=== Contract Expiry ===" << std::endl;
    ecs::World world;
    systems::ContractSystem contractSys(&world);
    auto* station = world.createEntity("station_1");
    addComp<components::ContractBoard>(station);

    contractSys.createContract("station_1", "player_1", "auction", 0.0, 10.0f);

    contractSys.update(5.0f);
    assertTrue(contractSys.getContractsByStatus("station_1", "outstanding") == 1,
               "Contract still outstanding at 5s");

    contractSys.update(6.0f);
    assertTrue(contractSys.getContractsByStatus("station_1", "outstanding") == 0,
               "No outstanding contracts after 11s");
    assertTrue(contractSys.getContractsByStatus("station_1", "expired") == 1,
               "Contract expired after 11s");
}

void testContractStatusQuery() {
    std::cout << "\n=== Contract Status Query ===" << std::endl;
    ecs::World world;
    systems::ContractSystem contractSys(&world);
    auto* station = world.createEntity("station_1");
    addComp<components::ContractBoard>(station);

    contractSys.createContract("station_1", "p1", "item_exchange", 1000.0, -1.0f);
    contractSys.createContract("station_1", "p2", "courier", 2000.0, 5.0f);
    contractSys.createContract("station_1", "p3", "auction", 3000.0, -1.0f);

    auto* board = station->getComponent<components::ContractBoard>();
    contractSys.acceptContract("station_1", board->contracts[0].contract_id, "buyer_1");
    contractSys.completeContract("station_1", board->contracts[0].contract_id);

    contractSys.update(6.0f); // expire the second contract

    assertTrue(contractSys.getContractsByStatus("station_1", "completed") == 1,
               "1 completed contract");
    assertTrue(contractSys.getContractsByStatus("station_1", "expired") == 1,
               "1 expired contract");
    assertTrue(contractSys.getContractsByStatus("station_1", "outstanding") == 1,
               "1 outstanding contract");
    assertTrue(contractSys.getActiveContractCount("station_1") == 1,
               "1 active contract (outstanding only)");
}

void testSerializeDeserializeContractBoard() {
    std::cout << "\n=== Serialize/Deserialize ContractBoard ===" << std::endl;

    ecs::World world;
    auto* entity = world.createEntity("board_test");
    auto* board = addComp<components::ContractBoard>(entity);

    components::ContractBoard::Contract c;
    c.contract_id = "contract_p1_0";
    c.issuer_id = "p1";
    c.assignee_id = "p2";
    c.type = "courier";
    c.status = "in_progress";
    c.isk_reward = 50000.0;
    c.isk_collateral = 10000.0;
    c.duration_remaining = 100.0f;
    c.days_to_complete = 7.0f;

    components::ContractBoard::ContractItem offered;
    offered.item_id = "trit"; offered.name = "Stellium";
    offered.quantity = 500; offered.volume = 0.01f;
    c.items_offered.push_back(offered);

    components::ContractBoard::ContractItem requested;
    requested.item_id = "pye"; requested.name = "Vanthium";
    requested.quantity = 100; requested.volume = 0.01f;
    c.items_requested.push_back(requested);

    board->contracts.push_back(c);

    data::WorldPersistence persistence;
    std::string json = persistence.serializeWorld(&world);

    ecs::World world2;
    assertTrue(persistence.deserializeWorld(&world2, json),
               "ContractBoard deserialization succeeds");

    auto* e2 = world2.getEntity("board_test");
    assertTrue(e2 != nullptr, "Board entity recreated");

    auto* board2 = e2->getComponent<components::ContractBoard>();
    assertTrue(board2 != nullptr, "ContractBoard component recreated");
    assertTrue(board2->contracts.size() == 1, "Contract count preserved");
    assertTrue(board2->contracts[0].contract_id == "contract_p1_0", "contract_id preserved");
    assertTrue(board2->contracts[0].issuer_id == "p1", "issuer_id preserved");
    assertTrue(board2->contracts[0].assignee_id == "p2", "assignee_id preserved");
    assertTrue(board2->contracts[0].type == "courier", "type preserved");
    assertTrue(board2->contracts[0].status == "in_progress", "status preserved");
    assertTrue(approxEqual(static_cast<float>(board2->contracts[0].isk_reward), 50000.0f), "isk_reward preserved");
    assertTrue(approxEqual(static_cast<float>(board2->contracts[0].isk_collateral), 10000.0f), "isk_collateral preserved");
    assertTrue(approxEqual(board2->contracts[0].duration_remaining, 100.0f), "duration_remaining preserved");
    assertTrue(approxEqual(board2->contracts[0].days_to_complete, 7.0f), "days_to_complete preserved");
    assertTrue(board2->contracts[0].items_offered.size() == 1, "items_offered count preserved");
    assertTrue(board2->contracts[0].items_offered[0].item_id == "trit", "offered item_id preserved");
    assertTrue(board2->contracts[0].items_offered[0].quantity == 500, "offered quantity preserved");
    assertTrue(board2->contracts[0].items_requested.size() == 1, "items_requested count preserved");
    assertTrue(board2->contracts[0].items_requested[0].item_id == "pye", "requested item_id preserved");
}

// ==================== PISystem Tests ====================

void testPIInstallExtractor() {
    std::cout << "\n=== PI Install Extractor ===" << std::endl;

    ecs::World world;
    systems::PISystem piSys(&world);

    auto* entity = world.createEntity("colony1");
    auto* colony = addComp<components::PlanetaryColony>(entity);
    colony->colony_id = "col_1";
    colony->owner_id = "player1";
    colony->planet_type = "barren";
    colony->cpu_max = 1675.0f;
    colony->powergrid_max = 6000.0f;

    bool ok = piSys.installExtractor("colony1", "base_metals", 100);
    assertTrue(ok, "Extractor installed successfully");
    assertTrue(piSys.getExtractorCount("colony1") == 1, "1 extractor present");
    assertTrue(colony->extractors[0].resource_type == "base_metals", "Extractor resource type correct");
    assertTrue(colony->extractors[0].quantity_per_cycle == 100, "Extractor quantity correct");
}

void testPIInstallProcessor() {
    std::cout << "\n=== PI Install Processor ===" << std::endl;

    ecs::World world;
    systems::PISystem piSys(&world);

    auto* entity = world.createEntity("colony2");
    auto* colony = addComp<components::PlanetaryColony>(entity);
    colony->colony_id = "col_2";
    colony->owner_id = "player1";
    colony->planet_type = "temperate";
    colony->cpu_max = 1675.0f;
    colony->powergrid_max = 6000.0f;

    bool ok = piSys.installProcessor("colony2", "base_metals", "refined_metals", 40, 5);
    assertTrue(ok, "Processor installed successfully");
    assertTrue(piSys.getProcessorCount("colony2") == 1, "1 processor present");
    assertTrue(colony->processors[0].input_type == "base_metals", "Processor input type correct");
    assertTrue(colony->processors[0].output_type == "refined_metals", "Processor output type correct");
}

void testPIExtractionCycle() {
    std::cout << "\n=== PI Extraction Cycle ===" << std::endl;

    ecs::World world;
    systems::PISystem piSys(&world);

    auto* entity = world.createEntity("colony3");
    auto* colony = addComp<components::PlanetaryColony>(entity);
    colony->colony_id = "col_3";
    colony->owner_id = "player1";
    colony->planet_type = "lava";
    colony->cpu_max = 1675.0f;
    colony->powergrid_max = 6000.0f;
    colony->storage_capacity = 10000.0f;

    piSys.installExtractor("colony3", "heavy_metals", 50);
    // Set short cycle time for testing
    colony->extractors[0].cycle_time = 10.0f;

    assertTrue(piSys.getTotalStored("colony3") == 0, "Storage starts empty");

    // Tick for one full cycle
    piSys.update(10.0f);
    assertTrue(piSys.getStoredResource("colony3", "heavy_metals") == 50,
               "50 heavy_metals extracted after 1 cycle");

    // Tick for another cycle
    piSys.update(10.0f);
    assertTrue(piSys.getStoredResource("colony3", "heavy_metals") == 100,
               "100 heavy_metals after 2 cycles");
}

void testPIProcessingCycle() {
    std::cout << "\n=== PI Processing Cycle ===" << std::endl;

    ecs::World world;
    systems::PISystem piSys(&world);

    auto* entity = world.createEntity("colony4");
    auto* colony = addComp<components::PlanetaryColony>(entity);
    colony->colony_id = "col_4";
    colony->owner_id = "player1";
    colony->planet_type = "oceanic";
    colony->cpu_max = 1675.0f;
    colony->powergrid_max = 6000.0f;
    colony->storage_capacity = 10000.0f;

    // Pre-load raw materials
    components::PlanetaryColony::StoredResource sr;
    sr.resource_type = "aqueous_liquids";
    sr.quantity = 200;
    colony->storage.push_back(sr);

    piSys.installProcessor("colony4", "aqueous_liquids", "water", 40, 5);
    colony->processors[0].cycle_time = 10.0f;

    piSys.update(10.0f);
    assertTrue(piSys.getStoredResource("colony4", "aqueous_liquids") == 160,
               "40 aqueous_liquids consumed");
    assertTrue(piSys.getStoredResource("colony4", "water") == 5,
               "5 water produced");
}

void testPICpuPowergridLimit() {
    std::cout << "\n=== PI CPU/PG Limit ===" << std::endl;

    ecs::World world;
    systems::PISystem piSys(&world);

    auto* entity = world.createEntity("colony5");
    auto* colony = addComp<components::PlanetaryColony>(entity);
    colony->colony_id = "col_5";
    colony->owner_id = "player1";
    colony->planet_type = "gas";
    colony->cpu_max = 100.0f;    // Very limited
    colony->powergrid_max = 600.0f;

    bool ok1 = piSys.installExtractor("colony5", "noble_gas", 50);
    assertTrue(ok1, "First extractor fits");

    // Second extractor should fail (cpu 45+45=90 fits, but pg 550+550=1100 > 600)
    bool ok2 = piSys.installExtractor("colony5", "reactive_gas", 30);
    assertTrue(!ok2, "Second extractor rejected (PG exceeded)");
    assertTrue(piSys.getExtractorCount("colony5") == 1, "Still only 1 extractor");
}

void testPIStorageCapacityLimit() {
    std::cout << "\n=== PI Storage Capacity Limit ===" << std::endl;

    ecs::World world;
    systems::PISystem piSys(&world);

    auto* entity = world.createEntity("colony6");
    auto* colony = addComp<components::PlanetaryColony>(entity);
    colony->colony_id = "col_6";
    colony->owner_id = "player1";
    colony->planet_type = "barren";
    colony->cpu_max = 1675.0f;
    colony->powergrid_max = 6000.0f;
    colony->storage_capacity = 100.0f;

    piSys.installExtractor("colony6", "base_metals", 60);
    colony->extractors[0].cycle_time = 10.0f;

    // First cycle: 60 extracted (< 100 capacity)
    piSys.update(10.0f);
    assertTrue(piSys.getStoredResource("colony6", "base_metals") == 60,
               "60 extracted (under capacity)");

    // Second cycle: 60 + 60 = 120 > 100 capacity, should not extract
    piSys.update(10.0f);
    assertTrue(piSys.getStoredResource("colony6", "base_metals") == 60,
               "Still 60 (storage full, extraction skipped)");
}

// ==================== ManufacturingSystem Tests ====================

void testManufacturingStartJob() {
    std::cout << "\n=== Manufacturing Start Job ===" << std::endl;

    ecs::World world;
    systems::ManufacturingSystem mfgSys(&world);

    auto* station = world.createEntity("station1");
    auto* facility = addComp<components::ManufacturingFacility>(station);
    facility->facility_id = "fac_1";
    facility->station_id = "station1";
    facility->max_jobs = 2;

    auto* player = world.createEntity("player1");
    auto* pcomp = addComp<components::Player>(player);
    pcomp->player_id = "player1";
    pcomp->credits = 100000.0;

    std::string job_id = mfgSys.startJob("station1", "player1", "fang_blueprint",
                                           "fang", "Fang Frigate", 1, 3600.0f, 1000.0);
    assertTrue(!job_id.empty(), "Job started successfully");
    assertTrue(mfgSys.getActiveJobCount("station1") == 1, "1 active job");
    assertTrue(approxEqual(static_cast<float>(pcomp->credits), 99000.0f), "Install cost deducted");
}

void testManufacturingJobCompletion() {
    std::cout << "\n=== Manufacturing Job Completion ===" << std::endl;

    ecs::World world;
    systems::ManufacturingSystem mfgSys(&world);

    auto* station = world.createEntity("station2");
    auto* facility = addComp<components::ManufacturingFacility>(station);
    facility->facility_id = "fac_2";
    facility->max_jobs = 1;

    auto* player = world.createEntity("player2");
    auto* pcomp = addComp<components::Player>(player);
    pcomp->player_id = "player2";
    pcomp->credits = 100000.0;

    mfgSys.startJob("station2", "player2", "autocannon_bp",
                     "autocannon_i", "150mm Autocannon I", 1, 100.0f, 500.0);

    assertTrue(mfgSys.getActiveJobCount("station2") == 1, "Job is active");
    assertTrue(mfgSys.getCompletedJobCount("station2") == 0, "No completed jobs yet");

    // Tick to completion
    mfgSys.update(100.0f);
    assertTrue(mfgSys.getActiveJobCount("station2") == 0, "No active jobs after completion");
    assertTrue(mfgSys.getCompletedJobCount("station2") == 1, "1 completed job");
    assertTrue(mfgSys.getTotalRunsCompleted("station2") == 1, "1 run completed");
}

void testManufacturingMultipleRuns() {
    std::cout << "\n=== Manufacturing Multiple Runs ===" << std::endl;

    ecs::World world;
    systems::ManufacturingSystem mfgSys(&world);

    auto* station = world.createEntity("station3");
    auto* facility = addComp<components::ManufacturingFacility>(station);
    facility->facility_id = "fac_3";
    facility->max_jobs = 1;

    auto* player = world.createEntity("player3");
    auto* pcomp = addComp<components::Player>(player);
    pcomp->player_id = "player3";
    pcomp->credits = 100000.0;

    mfgSys.startJob("station3", "player3", "drone_bp",
                     "hobgoblin_i", "Hobgoblin I", 3, 50.0f, 200.0);

    // First run
    mfgSys.update(50.0f);
    assertTrue(mfgSys.getTotalRunsCompleted("station3") == 1, "1 run after 50s");
    assertTrue(mfgSys.getActiveJobCount("station3") == 1, "Job still active (more runs)");

    // Second run
    mfgSys.update(50.0f);
    assertTrue(mfgSys.getTotalRunsCompleted("station3") == 2, "2 runs after 100s");

    // Third run (final)
    mfgSys.update(50.0f);
    assertTrue(mfgSys.getTotalRunsCompleted("station3") == 3, "3 runs after 150s");
    assertTrue(mfgSys.getCompletedJobCount("station3") == 1, "Job completed");
    assertTrue(mfgSys.getActiveJobCount("station3") == 0, "No active jobs");
}

void testManufacturingJobSlotLimit() {
    std::cout << "\n=== Manufacturing Job Slot Limit ===" << std::endl;

    ecs::World world;
    systems::ManufacturingSystem mfgSys(&world);

    auto* station = world.createEntity("station4");
    auto* facility = addComp<components::ManufacturingFacility>(station);
    facility->facility_id = "fac_4";
    facility->max_jobs = 1;

    auto* player = world.createEntity("player4");
    auto* pcomp = addComp<components::Player>(player);
    pcomp->player_id = "player4";
    pcomp->credits = 100000.0;

    std::string job1 = mfgSys.startJob("station4", "player4", "bp1",
                                         "item1", "Item 1", 1, 3600.0f, 100.0);
    assertTrue(!job1.empty(), "First job started");

    std::string job2 = mfgSys.startJob("station4", "player4", "bp2",
                                         "item2", "Item 2", 1, 3600.0f, 100.0);
    assertTrue(job2.empty(), "Second job rejected (slot full)");
    assertTrue(mfgSys.getActiveJobCount("station4") == 1, "Still 1 active job");
}

void testManufacturingCancelJob() {
    std::cout << "\n=== Manufacturing Cancel Job ===" << std::endl;

    ecs::World world;
    systems::ManufacturingSystem mfgSys(&world);

    auto* station = world.createEntity("station5");
    auto* facility = addComp<components::ManufacturingFacility>(station);
    facility->facility_id = "fac_5";
    facility->max_jobs = 2;

    auto* player = world.createEntity("player5");
    auto* pcomp = addComp<components::Player>(player);
    pcomp->player_id = "player5";
    pcomp->credits = 100000.0;

    std::string job_id = mfgSys.startJob("station5", "player5", "bp_test",
                                           "item_test", "Test Item", 1, 3600.0f, 100.0);
    assertTrue(mfgSys.getActiveJobCount("station5") == 1, "1 active job");

    bool cancelled = mfgSys.cancelJob("station5", job_id);
    assertTrue(cancelled, "Job cancelled successfully");
    assertTrue(mfgSys.getActiveJobCount("station5") == 0, "No active jobs after cancel");
}

void testManufacturingInsufficientFunds() {
    std::cout << "\n=== Manufacturing Insufficient Funds ===" << std::endl;

    ecs::World world;
    systems::ManufacturingSystem mfgSys(&world);

    auto* station = world.createEntity("station6");
    auto* facility = addComp<components::ManufacturingFacility>(station);
    facility->facility_id = "fac_6";
    facility->max_jobs = 1;

    auto* player = world.createEntity("player6");
    auto* pcomp = addComp<components::Player>(player);
    pcomp->player_id = "player6";
    pcomp->credits = 50.0;  // Not enough

    std::string job_id = mfgSys.startJob("station6", "player6", "bp_expensive",
                                           "item_expensive", "Expensive Item", 1, 3600.0f, 1000.0);
    assertTrue(job_id.empty(), "Job rejected (insufficient funds)");
    assertTrue(mfgSys.getActiveJobCount("station6") == 0, "No active jobs");
    assertTrue(approxEqual(static_cast<float>(pcomp->credits), 50.0f), "Credits unchanged");
}

// ==================== ResearchSystem Tests ====================

void testResearchME() {
    std::cout << "\n=== Research ME ===" << std::endl;

    ecs::World world;
    systems::ResearchSystem resSys(&world);

    auto* station = world.createEntity("lab1");
    auto* lab = addComp<components::ResearchLab>(station);
    lab->lab_id = "lab_1";
    lab->max_jobs = 1;

    auto* player = world.createEntity("researcher1");
    auto* pcomp = addComp<components::Player>(player);
    pcomp->player_id = "researcher1";
    pcomp->credits = 100000.0;

    std::string job_id = resSys.startMEResearch("lab1", "researcher1", "fang_blueprint",
                                                  5, 100.0f, 500.0);
    assertTrue(!job_id.empty(), "ME research started");
    assertTrue(resSys.getActiveJobCount("lab1") == 1, "1 active job");
    assertTrue(approxEqual(static_cast<float>(pcomp->credits), 99500.0f), "Install cost deducted");

    // Complete
    resSys.update(100.0f);
    assertTrue(resSys.getActiveJobCount("lab1") == 0, "No active jobs");
    assertTrue(resSys.getCompletedJobCount("lab1") == 1, "1 completed job");
}

void testResearchTE() {
    std::cout << "\n=== Research TE ===" << std::endl;

    ecs::World world;
    systems::ResearchSystem resSys(&world);

    auto* station = world.createEntity("lab2");
    auto* lab = addComp<components::ResearchLab>(station);
    lab->lab_id = "lab_2";
    lab->max_jobs = 1;

    auto* player = world.createEntity("researcher2");
    auto* pcomp = addComp<components::Player>(player);
    pcomp->player_id = "researcher2";
    pcomp->credits = 100000.0;

    std::string job_id = resSys.startTEResearch("lab2", "researcher2", "autocannon_bp",
                                                  10, 200.0f, 300.0);
    assertTrue(!job_id.empty(), "TE research started");
    assertTrue(resSys.getActiveJobCount("lab2") == 1, "1 active job");

    resSys.update(200.0f);
    assertTrue(resSys.getCompletedJobCount("lab2") == 1, "TE research completed");
}

void testResearchInvention() {
    std::cout << "\n=== Research Invention ===" << std::endl;

    ecs::World world;
    systems::ResearchSystem resSys(&world);

    auto* station = world.createEntity("lab3");
    auto* lab = addComp<components::ResearchLab>(station);
    lab->lab_id = "lab_3";
    lab->max_jobs = 1;

    auto* player = world.createEntity("researcher3");
    auto* pcomp = addComp<components::Player>(player);
    pcomp->player_id = "researcher3";
    pcomp->credits = 100000.0;

    std::string job_id = resSys.startInvention("lab3", "researcher3",
                                                "fang_blueprint", "fang_ii_blueprint",
                                                "datacore_mechanical_engineering",
                                                "datacore_electronic_engineering",
                                                1.0f, // 100% success for testing
                                                50.0f, 1000.0);
    assertTrue(!job_id.empty(), "Invention started");
    assertTrue(resSys.getActiveJobCount("lab3") == 1, "1 active job");

    resSys.update(50.0f);
    // With 100% success chance, it should complete
    assertTrue(resSys.getCompletedJobCount("lab3") == 1, "Invention succeeded");
    assertTrue(resSys.getFailedJobCount("lab3") == 0, "No failed jobs");
}

void testResearchInventionFailure() {
    std::cout << "\n=== Research Invention Failure ===" << std::endl;

    ecs::World world;
    systems::ResearchSystem resSys(&world);

    auto* station = world.createEntity("lab4");
    auto* lab = addComp<components::ResearchLab>(station);
    lab->lab_id = "lab_4";
    lab->max_jobs = 1;

    auto* player = world.createEntity("researcher4");
    auto* pcomp = addComp<components::Player>(player);
    pcomp->player_id = "researcher4";
    pcomp->credits = 100000.0;

    std::string job_id = resSys.startInvention("lab4", "researcher4",
                                                "fang_blueprint", "fang_ii_blueprint",
                                                "datacore_mechanical_engineering",
                                                "datacore_electronic_engineering",
                                                0.0f, // 0% success = guaranteed fail
                                                50.0f, 500.0);
    assertTrue(!job_id.empty(), "Invention job started");

    resSys.update(50.0f);
    assertTrue(resSys.getFailedJobCount("lab4") == 1, "Invention failed (0% chance)");
    assertTrue(resSys.getCompletedJobCount("lab4") == 0, "No completed jobs");
}

void testResearchJobSlotLimit() {
    std::cout << "\n=== Research Job Slot Limit ===" << std::endl;

    ecs::World world;
    systems::ResearchSystem resSys(&world);

    auto* station = world.createEntity("lab5");
    auto* lab = addComp<components::ResearchLab>(station);
    lab->lab_id = "lab_5";
    lab->max_jobs = 1;

    auto* player = world.createEntity("researcher5");
    auto* pcomp = addComp<components::Player>(player);
    pcomp->player_id = "researcher5";
    pcomp->credits = 100000.0;

    std::string job1 = resSys.startMEResearch("lab5", "researcher5", "bp1",
                                                5, 1000.0f, 100.0);
    assertTrue(!job1.empty(), "First research job started");

    std::string job2 = resSys.startTEResearch("lab5", "researcher5", "bp2",
                                                10, 1000.0f, 100.0);
    assertTrue(job2.empty(), "Second job rejected (slot full)");
    assertTrue(resSys.getActiveJobCount("lab5") == 1, "Still 1 active job");
}

void testResearchInsufficientFunds() {
    std::cout << "\n=== Research Insufficient Funds ===" << std::endl;

    ecs::World world;
    systems::ResearchSystem resSys(&world);

    auto* station = world.createEntity("lab6");
    auto* lab = addComp<components::ResearchLab>(station);
    lab->lab_id = "lab_6";
    lab->max_jobs = 1;

    auto* player = world.createEntity("researcher6");
    auto* pcomp = addComp<components::Player>(player);
    pcomp->player_id = "researcher6";
    pcomp->credits = 10.0;  // Not enough

    std::string job_id = resSys.startMEResearch("lab6", "researcher6", "bp_expensive",
                                                  5, 1000.0f, 500.0);
    assertTrue(job_id.empty(), "Job rejected (insufficient funds)");
    assertTrue(resSys.getActiveJobCount("lab6") == 0, "No active jobs");
    assertTrue(approxEqual(static_cast<float>(pcomp->credits), 10.0f), "Credits unchanged");
}

// ==================== Mining System Tests ====================

void testMiningCreateDeposit() {
    std::cout << "\n=== Mining: Create Deposit ===" << std::endl;

    ecs::World world;
    systems::MiningSystem mineSys(&world);

    std::string id = mineSys.createDeposit("Ferrite", 5000.0f, 100.0f, 0.0f, 0.0f);
    assertTrue(!id.empty(), "Deposit entity created");

    auto* entity = world.getEntity(id);
    assertTrue(entity != nullptr, "Deposit entity exists in world");

    auto* dep = entity->getComponent<components::MineralDeposit>();
    assertTrue(dep != nullptr, "Deposit has MineralDeposit component");
    assertTrue(dep->mineral_type == "Ferrite", "Mineral type is Ferrite");
    assertTrue(approxEqual(dep->quantity_remaining, 5000.0f), "Quantity remaining is 5000");
    assertTrue(!dep->isDepleted(), "Deposit is not depleted");

    auto* pos = entity->getComponent<components::Position>();
    assertTrue(pos != nullptr, "Deposit has Position component");
    assertTrue(approxEqual(pos->x, 100.0f), "Deposit x position correct");
}

void testMiningStartStop() {
    std::cout << "\n=== Mining: Start and Stop ===" << std::endl;

    ecs::World world;
    systems::MiningSystem mineSys(&world);

    std::string dep_id = mineSys.createDeposit("Galvite", 1000.0f, 0.0f, 0.0f, 0.0f);

    auto* miner = world.createEntity("miner_1");
    auto* pos = addComp<components::Position>(miner);
    pos->x = 5000.0f; // within default 10000m range
    auto* laser = addComp<components::MiningLaser>(miner);
    laser->yield_per_cycle = 50.0f;
    laser->cycle_time = 10.0f;
    addComp<components::Inventory>(miner);

    bool started = mineSys.startMining("miner_1", dep_id);
    assertTrue(started, "Mining started successfully");
    assertTrue(laser->active, "Laser is active");
    assertTrue(mineSys.getActiveMinerCount() == 1, "One active miner");

    bool stopped = mineSys.stopMining("miner_1");
    assertTrue(stopped, "Mining stopped successfully");
    assertTrue(!laser->active, "Laser is inactive after stop");
    assertTrue(mineSys.getActiveMinerCount() == 0, "No active miners");
}

void testMiningRangeCheck() {
    std::cout << "\n=== Mining: Range Check ===" << std::endl;

    ecs::World world;
    systems::MiningSystem mineSys(&world);

    std::string dep_id = mineSys.createDeposit("Ferrite", 1000.0f, 0.0f, 0.0f, 0.0f);

    auto* miner = world.createEntity("miner_far");
    auto* pos = addComp<components::Position>(miner);
    pos->x = 20000.0f; // too far (default range 10000m)
    addComp<components::MiningLaser>(miner);
    addComp<components::Inventory>(miner);

    bool started = mineSys.startMining("miner_far", dep_id);
    assertTrue(!started, "Mining fails when out of range");
}

void testMiningCycleCompletion() {
    std::cout << "\n=== Mining: Cycle Completion ===" << std::endl;

    ecs::World world;
    systems::MiningSystem mineSys(&world);

    std::string dep_id = mineSys.createDeposit("Ferrite", 1000.0f, 0.0f, 0.0f, 0.0f, 0.1f);

    auto* miner = world.createEntity("miner_1");
    auto* pos = addComp<components::Position>(miner);
    pos->x = 100.0f;
    auto* laser = addComp<components::MiningLaser>(miner);
    laser->yield_per_cycle = 50.0f;
    laser->cycle_time = 10.0f;
    auto* inv = addComp<components::Inventory>(miner);
    inv->max_capacity = 500.0f;

    mineSys.startMining("miner_1", dep_id);

    // Advance 10 seconds — one full cycle
    mineSys.update(10.0f);

    assertTrue(inv->items.size() == 1, "Ore item added to inventory");
    assertTrue(inv->items[0].item_id == "Ferrite", "Mined Ferrite");
    assertTrue(inv->items[0].quantity == 50, "Mined 50 units");

    auto* dep = world.getEntity(dep_id)->getComponent<components::MineralDeposit>();
    assertTrue(approxEqual(dep->quantity_remaining, 950.0f), "Deposit reduced by 50");
}

void testMiningDepletedDeposit() {
    std::cout << "\n=== Mining: Depleted Deposit ===" << std::endl;

    ecs::World world;
    systems::MiningSystem mineSys(&world);

    // Small deposit — only 20 units
    std::string dep_id = mineSys.createDeposit("Heliore", 20.0f, 0.0f, 0.0f, 0.0f, 0.1f);

    auto* miner = world.createEntity("miner_1");
    addComp<components::Position>(miner);
    auto* laser = addComp<components::MiningLaser>(miner);
    laser->yield_per_cycle = 50.0f;
    laser->cycle_time = 5.0f;
    auto* inv = addComp<components::Inventory>(miner);
    inv->max_capacity = 500.0f;

    mineSys.startMining("miner_1", dep_id);
    mineSys.update(5.0f);

    // Should only get 20 units (deposit was 20, yield was 50)
    assertTrue(inv->items.size() == 1, "Ore item added");
    assertTrue(inv->items[0].quantity == 20, "Only mined available 20 units");

    auto* dep = world.getEntity(dep_id)->getComponent<components::MineralDeposit>();
    assertTrue(dep->isDepleted(), "Deposit is depleted");
    assertTrue(!laser->active, "Laser stops when deposit depleted");
}

void testMiningCargoFull() {
    std::cout << "\n=== Mining: Cargo Full ===" << std::endl;

    ecs::World world;
    systems::MiningSystem mineSys(&world);

    std::string dep_id = mineSys.createDeposit("Ferrite", 10000.0f, 0.0f, 0.0f, 0.0f, 1.0f);

    auto* miner = world.createEntity("miner_1");
    addComp<components::Position>(miner);
    auto* laser = addComp<components::MiningLaser>(miner);
    laser->yield_per_cycle = 100.0f;
    laser->cycle_time = 5.0f;
    auto* inv = addComp<components::Inventory>(miner);
    inv->max_capacity = 50.0f; // only 50 m3 free, vol_per_unit=1.0

    mineSys.startMining("miner_1", dep_id);
    mineSys.update(5.0f);

    // Should only mine 50 units (cargo limit)
    assertTrue(inv->items.size() == 1, "Ore item added");
    assertTrue(inv->items[0].quantity == 50, "Capped by cargo capacity");
}

void testMiningOreStacking() {
    std::cout << "\n=== Mining: Ore Stacking ===" << std::endl;

    ecs::World world;
    systems::MiningSystem mineSys(&world);

    std::string dep_id = mineSys.createDeposit("Ferrite", 10000.0f, 0.0f, 0.0f, 0.0f, 0.1f);

    auto* miner = world.createEntity("miner_1");
    addComp<components::Position>(miner);
    auto* laser = addComp<components::MiningLaser>(miner);
    laser->yield_per_cycle = 30.0f;
    laser->cycle_time = 5.0f;
    auto* inv = addComp<components::Inventory>(miner);
    inv->max_capacity = 5000.0f;

    mineSys.startMining("miner_1", dep_id);

    // Complete two full cycles
    mineSys.update(5.0f);
    mineSys.update(5.0f);

    assertTrue(inv->items.size() == 1, "Ore stacked into single item");
    assertTrue(inv->items[0].quantity == 60, "Two cycles stacked: 30+30=60");
}

// ==================== Mining Drone Tests ====================

void testMiningDroneLaunchAndMine() {
    std::cout << "\n=== Mining Drone: Launch and Mine ===" << std::endl;

    ecs::World world;
    systems::DroneSystem droneSys(&world);
    systems::MiningSystem mineSys(&world);

    // Create a mineral deposit
    std::string dep_id = mineSys.createDeposit("Ferrite", 5000.0f, 0.0f, 0.0f, 0.0f, 0.1f);

    // Create a ship with mining drones
    auto* ship = world.createEntity("ship_1");
    auto* bay = addComp<components::DroneBay>(ship);
    auto* inv = addComp<components::Inventory>(ship);
    inv->max_capacity = 5000.0f;
    addComp<components::Position>(ship);

    components::DroneBay::DroneInfo mDrone;
    mDrone.drone_id = "mining_drone_1";
    mDrone.name = "Mining Drone I";
    mDrone.type = "mining_drone";
    mDrone.mining_yield = 25.0f;
    mDrone.rate_of_fire = 60.0f; // 60s cycle
    mDrone.hitpoints = 50.0f;
    mDrone.current_hp = 50.0f;
    mDrone.bandwidth_use = 5;
    mDrone.volume = 5.0f;
    bay->stored_drones.push_back(mDrone);

    // Launch drone and set mining target
    bool launched = droneSys.launchDrone("ship_1", "mining_drone_1");
    assertTrue(launched, "Mining drone launched");
    assertTrue(droneSys.getDeployedCount("ship_1") == 1, "One drone deployed");

    bool targeted = droneSys.setMiningTarget("ship_1", dep_id);
    assertTrue(targeted, "Mining target set");

    // Complete one cycle (drone rate_of_fire = 60s, but cooldown starts at 0)
    droneSys.update(0.0f); // first tick: mines immediately (cooldown=0)

    assertTrue(inv->items.size() == 1, "Ore mined by drone");
    assertTrue(inv->items[0].item_id == "Ferrite", "Mined correct mineral");
    assertTrue(inv->items[0].quantity == 25, "Mined correct amount");

    auto* dep = world.getEntity(dep_id)->getComponent<components::MineralDeposit>();
    assertTrue(approxEqual(dep->quantity_remaining, 4975.0f), "Deposit reduced by mining drone");
}

void testSalvageDroneLaunchAndSalvage() {
    std::cout << "\n=== Salvage Drone: Launch and Salvage ===" << std::endl;

    ecs::World world;
    systems::DroneSystem droneSys(&world);
    systems::WreckSalvageSystem wreckSys(&world);

    // Create a wreck with loot
    std::string wreck_id = wreckSys.createWreck("dead_npc", 100.0f, 0.0f, 0.0f);
    auto* wreck_entity = world.getEntity(wreck_id);
    auto* wreck_inv = wreck_entity->getComponent<components::Inventory>();
    components::Inventory::Item salvage;
    salvage.item_id = "salvage_metal";
    salvage.name = "Salvaged Metal";
    salvage.type = "salvage";
    salvage.quantity = 5;
    salvage.volume = 0.5f;
    wreck_inv->items.push_back(salvage);

    // Create ship with salvage drone
    auto* ship = world.createEntity("ship_1");
    auto* bay = addComp<components::DroneBay>(ship);
    auto* inv = addComp<components::Inventory>(ship);
    inv->max_capacity = 500.0f;
    addComp<components::Position>(ship);

    components::DroneBay::DroneInfo sDrone;
    sDrone.drone_id = "salvage_drone_1";
    sDrone.name = "Salvage Drone I";
    sDrone.type = "salvage_drone";
    sDrone.salvage_chance = 1.0f; // 100% for testing
    sDrone.rate_of_fire = 10.0f;
    sDrone.hitpoints = 30.0f;
    sDrone.current_hp = 30.0f;
    sDrone.bandwidth_use = 5;
    sDrone.volume = 5.0f;
    bay->stored_drones.push_back(sDrone);

    bool launched = droneSys.launchDrone("ship_1", "salvage_drone_1");
    assertTrue(launched, "Salvage drone launched");

    bool targeted = droneSys.setSalvageTarget("ship_1", wreck_id);
    assertTrue(targeted, "Salvage target set");

    // First tick: salvage immediately (cooldown=0), chance=1.0 guaranteed
    droneSys.update(0.0f);

    // Wreck should now be salvaged
    auto* wreck = wreck_entity->getComponent<components::Wreck>();
    assertTrue(wreck->salvaged, "Wreck marked as salvaged");

    // Items transferred to ship
    assertTrue(inv->items.size() == 1, "Salvage transferred to ship");
    assertTrue(inv->items[0].item_id == "salvage_metal", "Correct salvage item");
    assertTrue(inv->items[0].quantity == 5, "Correct salvage quantity");
}

void testSalvageDroneAlreadySalvaged() {
    std::cout << "\n=== Salvage Drone: Already Salvaged ===" << std::endl;

    ecs::World world;
    systems::DroneSystem droneSys(&world);
    systems::WreckSalvageSystem wreckSys(&world);

    std::string wreck_id = wreckSys.createWreck("dead_npc2", 0.0f, 0.0f, 0.0f);
    auto* wreck_entity = world.getEntity(wreck_id);
    auto* wreck = wreck_entity->getComponent<components::Wreck>();
    wreck->salvaged = true; // already salvaged

    auto* ship = world.createEntity("ship_2");
    auto* bay = addComp<components::DroneBay>(ship);
    addComp<components::Inventory>(ship);
    addComp<components::Position>(ship);

    // Cannot set salvage target on already-salvaged wreck
    bool targeted = droneSys.setSalvageTarget("ship_2", wreck_id);
    assertTrue(!targeted, "Cannot target already-salvaged wreck");
}

void testMiningDroneTargetDepletedDeposit() {
    std::cout << "\n=== Mining Drone: Depleted Deposit ===" << std::endl;

    ecs::World world;
    systems::DroneSystem droneSys(&world);
    systems::MiningSystem mineSys(&world);

    std::string dep_id = mineSys.createDeposit("Ferrite", 0.0f, 0.0f, 0.0f, 0.0f);

    auto* ship = world.createEntity("ship_3");
    auto* bay = addComp<components::DroneBay>(ship);
    addComp<components::Inventory>(ship);
    addComp<components::Position>(ship);

    bool targeted = droneSys.setMiningTarget("ship_3", dep_id);
    assertTrue(!targeted, "Cannot target depleted deposit");
}

// ==================== AI Profit-Based Deposit Selection Tests ====================

void testAIFindMostProfitableDepositWithPrices() {
    std::cout << "\n=== AI: Find Most Profitable Deposit With Market Prices ===" << std::endl;

    ecs::World world;
    systems::AISystem aiSys(&world);

    // Create a SupplyDemand entity with market prices
    auto* market = world.createEntity("system_market");
    auto* sd = addComp<components::SupplyDemand>(market);
    sd->addCommodity("Ferrite", 10.0f, 100.0f, 100.0f);   // cheap
    sd->addCommodity("Galvite", 500.0f, 100.0f, 100.0f);  // expensive

    // Cheap deposit close by
    auto* close_dep = world.createEntity("close_dep");
    auto* cdpos = addComp<components::Position>(close_dep);
    cdpos->x = 1000.0f; cdpos->y = 0.0f; cdpos->z = 0.0f;
    auto* cd = addComp<components::MineralDeposit>(close_dep);
    cd->mineral_type = "Ferrite";
    cd->quantity_remaining = 5000.0f;

    // Expensive deposit farther away (but high value/distance)
    auto* far_dep = world.createEntity("far_dep");
    auto* fdpos = addComp<components::Position>(far_dep);
    fdpos->x = 10000.0f; fdpos->y = 0.0f; fdpos->z = 0.0f;
    auto* fd = addComp<components::MineralDeposit>(far_dep);
    fd->mineral_type = "Galvite";
    fd->quantity_remaining = 5000.0f;

    // Create NPC at origin
    auto* npc = world.createEntity("profit_miner");
    auto* pos = addComp<components::Position>(npc);
    pos->x = 0.0f; pos->y = 0.0f; pos->z = 0.0f;
    auto* vel = addComp<components::Velocity>(npc);
    vel->max_speed = 100.0f;
    auto* ai = addComp<components::AI>(npc);
    ai->awareness_range = 50000.0f;

    // Ferrite: 10/1000 = 0.01 score
    // Galvite: 500/10000 = 0.05 score — should be selected
    auto* best = aiSys.findMostProfitableDeposit(npc);
    assertTrue(best != nullptr, "Found a profitable deposit");
    assertTrue(best->getId() == "far_dep", "Selected higher-value Galvite deposit");
}

void testAIFindMostProfitableDepositNoMarket() {
    std::cout << "\n=== AI: Find Most Profitable Deposit Without Market Data ===" << std::endl;

    ecs::World world;
    systems::AISystem aiSys(&world);

    // No SupplyDemand entity — should fall back to nearest
    auto* far = world.createEntity("far");
    auto* fpos = addComp<components::Position>(far);
    fpos->x = 20000.0f;
    auto* fd = addComp<components::MineralDeposit>(far);
    fd->quantity_remaining = 5000.0f;

    auto* near = world.createEntity("near");
    auto* npos = addComp<components::Position>(near);
    npos->x = 3000.0f;
    auto* nd = addComp<components::MineralDeposit>(near);
    nd->quantity_remaining = 5000.0f;

    auto* npc = world.createEntity("miner");
    auto* pos = addComp<components::Position>(npc);
    pos->x = 0.0f; pos->y = 0.0f; pos->z = 0.0f;
    addComp<components::Velocity>(npc);
    auto* ai = addComp<components::AI>(npc);
    ai->awareness_range = 50000.0f;

    auto* best = aiSys.findMostProfitableDeposit(npc);
    assertTrue(best != nullptr, "Found a deposit (fallback to nearest)");
    assertTrue(best->getId() == "near", "Falls back to nearest without market data");
}

void testAIFindMostProfitableDepositPrefersCloseWhenEqual() {
    std::cout << "\n=== AI: Profitable Deposit Prefers Close When Price Equal ===" << std::endl;

    ecs::World world;
    systems::AISystem aiSys(&world);

    auto* market = world.createEntity("sys");
    auto* sd = addComp<components::SupplyDemand>(market);
    sd->addCommodity("Ferrite", 100.0f, 100.0f, 100.0f);

    // Two deposits with same mineral at different distances
    auto* close = world.createEntity("close");
    auto* cpos = addComp<components::Position>(close);
    cpos->x = 2000.0f;
    auto* cd = addComp<components::MineralDeposit>(close);
    cd->mineral_type = "Ferrite";
    cd->quantity_remaining = 5000.0f;

    auto* far = world.createEntity("far");
    auto* fpos = addComp<components::Position>(far);
    fpos->x = 8000.0f;
    auto* fd = addComp<components::MineralDeposit>(far);
    fd->mineral_type = "Ferrite";
    fd->quantity_remaining = 5000.0f;

    auto* npc = world.createEntity("miner");
    auto* pos = addComp<components::Position>(npc);
    pos->x = 0.0f;
    addComp<components::Velocity>(npc);
    auto* ai = addComp<components::AI>(npc);
    ai->awareness_range = 50000.0f;

    auto* best = aiSys.findMostProfitableDeposit(npc);
    assertTrue(best != nullptr, "Found deposit");
    assertTrue(best->getId() == "close", "Prefers closer deposit when price is equal");
}

// ==================== AI Hauling Behavior Tests ====================

void testAIHaulingState() {
    std::cout << "\n=== AI: Hauling State ===" << std::endl;

    ecs::World world;
    auto* npc = world.createEntity("hauler_npc");
    auto* ai = addComp<components::AI>(npc);
    ai->state = components::AI::State::Hauling;

    assertTrue(ai->state == components::AI::State::Hauling, "AI state set to Hauling");
    assertTrue(ai->state != components::AI::State::Mining, "Hauling != Mining");
    assertTrue(ai->state != components::AI::State::Idle, "Hauling != Idle");
}

void testAIHaulingMovesToStation() {
    std::cout << "\n=== AI: Hauling Moves Toward Station ===" << std::endl;

    ecs::World world;
    systems::AISystem aiSys(&world);

    // Create a station far away
    auto* station = world.createEntity("station1");
    auto* spos = addComp<components::Position>(station);
    spos->x = 10000.0f; spos->y = 0.0f; spos->z = 0.0f;

    // Create NPC hauling toward station
    auto* npc = world.createEntity("hauler");
    auto* pos = addComp<components::Position>(npc);
    pos->x = 0.0f; pos->y = 0.0f; pos->z = 0.0f;
    auto* vel = addComp<components::Velocity>(npc);
    vel->max_speed = 200.0f;
    auto* ai = addComp<components::AI>(npc);
    ai->state = components::AI::State::Hauling;
    ai->haul_station_id = "station1";

    aiSys.update(1.0f);

    // Should be moving toward station (positive vx)
    assertTrue(vel->vx > 0.0f, "Hauler moves toward station (vx > 0)");
    assertTrue(ai->state == components::AI::State::Hauling, "Still hauling while far away");
}

void testAIHaulingSellsCargoAtStation() {
    std::cout << "\n=== AI: Hauling Sells Cargo At Station ===" << std::endl;

    ecs::World world;
    systems::AISystem aiSys(&world);

    // Create market with prices
    auto* market = world.createEntity("market_sys");
    auto* sd = addComp<components::SupplyDemand>(market);
    sd->addCommodity("Ferrite", 50.0f, 100.0f, 100.0f);

    // Create station within docking range
    auto* station = world.createEntity("station1");
    auto* spos = addComp<components::Position>(station);
    spos->x = 100.0f; spos->y = 0.0f; spos->z = 0.0f;

    // Create NPC near station with cargo
    auto* npc = world.createEntity("hauler");
    auto* pos = addComp<components::Position>(npc);
    pos->x = 50.0f; pos->y = 0.0f; pos->z = 0.0f;
    auto* vel = addComp<components::Velocity>(npc);
    vel->max_speed = 100.0f;
    auto* ai = addComp<components::AI>(npc);
    ai->state = components::AI::State::Hauling;
    ai->haul_station_id = "station1";
    auto* inv = addComp<components::Inventory>(npc);
    inv->max_capacity = 1000.0f;
    // Add 100 units of Ferrite to cargo
    components::Inventory::Item ore;
    ore.item_id = "Ferrite";
    ore.name = "Ferrite";
    ore.type = "ore";
    ore.quantity = 100;
    ore.volume = 0.1f;
    inv->items.push_back(ore);
    auto* intent = addComp<components::SimNPCIntent>(npc);
    intent->wallet = 1000.0;

    aiSys.update(1.0f);

    // Should have sold cargo: 100 * 50 = 5000 Credits earned
    assertTrue(inv->items.empty(), "Cargo sold (inventory cleared)");
    assertTrue(intent->wallet == 6000.0, "Wallet increased by 5000 (100 * 50)");
    assertTrue(ai->state == components::AI::State::Idle, "Returns to Idle after selling");
}

void testAIHaulingNoStationGoesIdle() {
    std::cout << "\n=== AI: Hauling Without Station Goes Idle ===" << std::endl;

    ecs::World world;
    systems::AISystem aiSys(&world);

    auto* npc = world.createEntity("hauler");
    auto* pos = addComp<components::Position>(npc);
    pos->x = 0.0f;
    auto* vel = addComp<components::Velocity>(npc);
    vel->max_speed = 100.0f;
    auto* ai = addComp<components::AI>(npc);
    ai->state = components::AI::State::Hauling;
    ai->haul_station_id = "";  // no station

    aiSys.update(1.0f);

    assertTrue(ai->state == components::AI::State::Idle, "Goes idle without station");
}

void testAIMiningCargoFullTransitionsToHauling() {
    std::cout << "\n=== AI: Mining Cargo Full Transitions to Hauling ===" << std::endl;

    ecs::World world;
    systems::AISystem aiSys(&world);

    // Create deposit
    auto* deposit = world.createEntity("deposit1");
    auto* dpos = addComp<components::Position>(deposit);
    dpos->x = 100.0f;
    auto* dep = addComp<components::MineralDeposit>(deposit);
    dep->mineral_type = "Ferrite";
    dep->quantity_remaining = 5000.0f;

    // Create miner with full cargo and haul station set
    auto* npc = world.createEntity("miner");
    auto* pos = addComp<components::Position>(npc);
    pos->x = 50.0f;
    auto* vel = addComp<components::Velocity>(npc);
    vel->max_speed = 100.0f;
    auto* ai = addComp<components::AI>(npc);
    ai->state = components::AI::State::Mining;
    ai->target_entity_id = "deposit1";
    ai->haul_station_id = "station1";
    auto* laser = addComp<components::MiningLaser>(npc);
    laser->active = true;
    auto* inv = addComp<components::Inventory>(npc);
    inv->max_capacity = 10.0f;  // tiny hold
    // Fill cargo completely
    components::Inventory::Item ore;
    ore.item_id = "Ferrite";
    ore.name = "Ferrite";
    ore.type = "ore";
    ore.quantity = 200;
    ore.volume = 0.1f;
    inv->items.push_back(ore);  // 200 * 0.1 = 20.0 > 10.0 max

    aiSys.update(1.0f);

    assertTrue(ai->state == components::AI::State::Hauling, "Cargo full transitions to Hauling");
    assertTrue(!laser->active, "Mining laser deactivated");
}

void testAIMiningCargoFullNoStationGoesIdle() {
    std::cout << "\n=== AI: Mining Cargo Full Without Station Goes Idle ===" << std::endl;

    ecs::World world;
    systems::AISystem aiSys(&world);

    auto* deposit = world.createEntity("deposit1");
    auto* dpos = addComp<components::Position>(deposit);
    dpos->x = 100.0f;
    auto* dep = addComp<components::MineralDeposit>(deposit);
    dep->mineral_type = "Ferrite";
    dep->quantity_remaining = 5000.0f;

    auto* npc = world.createEntity("miner");
    auto* pos = addComp<components::Position>(npc);
    pos->x = 50.0f;
    auto* vel = addComp<components::Velocity>(npc);
    vel->max_speed = 100.0f;
    auto* ai = addComp<components::AI>(npc);
    ai->state = components::AI::State::Mining;
    ai->target_entity_id = "deposit1";
    ai->haul_station_id = "";  // no station
    auto* laser = addComp<components::MiningLaser>(npc);
    laser->active = true;
    auto* inv = addComp<components::Inventory>(npc);
    inv->max_capacity = 10.0f;
    components::Inventory::Item ore;
    ore.item_id = "Ferrite";
    ore.name = "Ferrite";
    ore.type = "ore";
    ore.quantity = 200;
    ore.volume = 0.1f;
    inv->items.push_back(ore);

    aiSys.update(1.0f);

    assertTrue(ai->state == components::AI::State::Idle, "Cargo full goes Idle without station");
}

// ==================== AI Full Economic Cycle Tests ====================

void testAIFullMineHaulSellCycle() {
    std::cout << "\n=== AI: Full Mine-Haul-Sell Cycle ===" << std::endl;

    ecs::World world;
    systems::AISystem aiSys(&world);

    // Market prices
    auto* market = world.createEntity("market");
    auto* sd = addComp<components::SupplyDemand>(market);
    sd->addCommodity("Ferrite", 25.0f, 100.0f, 100.0f);

    // Station for selling
    auto* station = world.createEntity("sell_station");
    auto* spos = addComp<components::Position>(station);
    spos->x = 200.0f;

    // Deposit
    auto* deposit = world.createEntity("ore_deposit");
    auto* dpos = addComp<components::Position>(deposit);
    dpos->x = 100.0f;
    auto* dep = addComp<components::MineralDeposit>(deposit);
    dep->mineral_type = "Ferrite";
    dep->quantity_remaining = 5000.0f;

    // NPC miner starting idle
    auto* npc = world.createEntity("full_cycle_miner");
    auto* pos = addComp<components::Position>(npc);
    pos->x = 0.0f;
    auto* vel = addComp<components::Velocity>(npc);
    vel->max_speed = 100.0f;
    auto* ai = addComp<components::AI>(npc);
    ai->behavior = components::AI::Behavior::Passive;
    ai->state = components::AI::State::Idle;
    ai->awareness_range = 50000.0f;
    ai->haul_station_id = "sell_station";
    auto* laser = addComp<components::MiningLaser>(npc);
    laser->active = false;
    auto* inv = addComp<components::Inventory>(npc);
    inv->max_capacity = 10.0f;  // small hold
    auto* intent = addComp<components::SimNPCIntent>(npc);
    intent->wallet = 0.0;

    // Step 1: Idle → should find deposit and start approaching
    aiSys.update(1.0f);
    assertTrue(ai->state == components::AI::State::Approaching, "Step 1: Idle -> Approaching");
    assertTrue(ai->target_entity_id == "ore_deposit", "Step 1: Target is deposit");

    // Step 2: Simulate arrival at deposit (move NPC close)
    pos->x = 100.0f;
    ai->state = components::AI::State::Mining;
    aiSys.update(1.0f);
    assertTrue(laser->active, "Step 2: Laser activated while mining");

    // Step 3: Fill cargo, trigger hauling
    components::Inventory::Item ore;
    ore.item_id = "Ferrite";
    ore.name = "Ferrite";
    ore.type = "ore";
    ore.quantity = 200;
    ore.volume = 0.1f;
    inv->items.push_back(ore);  // overfill
    aiSys.update(1.0f);
    assertTrue(ai->state == components::AI::State::Hauling, "Step 3: Mining -> Hauling (cargo full)");

    // Step 4: Move NPC to station and sell
    pos->x = 200.0f;
    aiSys.update(1.0f);
    assertTrue(inv->items.empty(), "Step 4: Cargo sold");
    assertTrue(intent->wallet == 5000.0, "Step 4: Earned 200 * 25 = 5000 Credits");
    assertTrue(ai->state == components::AI::State::Idle, "Step 4: Back to Idle");
}

// ==================== Refining System Tests ====================

void testRefineOreBasic() {
    std::cout << "\n=== Refining: Basic Ore Refining ===" << std::endl;

    ecs::World world;
    systems::RefiningSystem refiningSys(&world);

    // Create a station with refining facility
    auto* station = world.createEntity("station1");
    auto* facility = addComp<components::RefiningFacility>(station);
    facility->efficiency = 1.0f;  // 100% efficiency
    facility->tax_rate = 0.0f;    // no tax

    // Add a recipe: 100 Ferrite → 415 Stellium
    components::RefiningFacility::RefineRecipe recipe;
    recipe.ore_type = "Ferrite";
    recipe.ore_units_required = 100;
    recipe.outputs.push_back({"Stellium", 415});
    facility->recipes.push_back(recipe);

    // Create a player with ore
    auto* player = world.createEntity("player1");
    auto* inv = addComp<components::Inventory>(player);
    inv->max_capacity = 10000.0f;
    components::Inventory::Item ore;
    ore.item_id = "Ferrite";
    ore.name = "Ferrite";
    ore.type = "ore";
    ore.quantity = 200;
    ore.volume = 0.1f;
    inv->items.push_back(ore);

    int refined = refiningSys.refineOre("player1", "station1", "Ferrite", 2);

    assertTrue(refined == 2, "Refined 2 batches");

    // Check ore consumed (200 - 200 = 0, should be removed)
    bool ore_found = false;
    int mineral_qty = 0;
    for (const auto& item : inv->items) {
        if (item.item_id == "Ferrite") ore_found = true;
        if (item.item_id == "Stellium") mineral_qty = item.quantity;
    }
    assertTrue(!ore_found, "Ore consumed completely");
    assertTrue(mineral_qty == 830, "Produced 830 Stellium (415 * 2)");
}

void testRefineOreWithEfficiency() {
    std::cout << "\n=== Refining: Efficiency Affects Yield ===" << std::endl;

    ecs::World world;
    systems::RefiningSystem refiningSys(&world);

    auto* station = world.createEntity("station2");
    auto* facility = addComp<components::RefiningFacility>(station);
    facility->efficiency = 0.5f;
    facility->tax_rate = 0.0f;

    components::RefiningFacility::RefineRecipe recipe;
    recipe.ore_type = "Ferrite";
    recipe.ore_units_required = 100;
    recipe.outputs.push_back({"Stellium", 400});
    facility->recipes.push_back(recipe);

    auto* player = world.createEntity("player2");
    auto* inv = addComp<components::Inventory>(player);
    inv->max_capacity = 10000.0f;
    components::Inventory::Item ore;
    ore.item_id = "Ferrite";
    ore.name = "Ferrite";
    ore.type = "ore";
    ore.quantity = 100;
    ore.volume = 0.1f;
    inv->items.push_back(ore);

    int refined = refiningSys.refineOre("player2", "station2", "Ferrite", 1);
    assertTrue(refined == 1, "Refined 1 batch at 50% efficiency");

    int mineral_qty = 0;
    for (const auto& item : inv->items) {
        if (item.item_id == "Stellium") mineral_qty = item.quantity;
    }
    // 400 * 0.5 efficiency * (1 - 0.0 tax) = 200
    assertTrue(mineral_qty == 200, "50% efficiency yields 200 Stellium");
}

void testRefineOreWithTax() {
    std::cout << "\n=== Refining: Tax Reduces Yield ===" << std::endl;

    ecs::World world;
    systems::RefiningSystem refiningSys(&world);

    auto* station = world.createEntity("station3");
    auto* facility = addComp<components::RefiningFacility>(station);
    facility->efficiency = 1.0f;
    facility->tax_rate = 0.1f;   // 10% tax

    components::RefiningFacility::RefineRecipe recipe;
    recipe.ore_type = "Ferrite";
    recipe.ore_units_required = 100;
    recipe.outputs.push_back({"Stellium", 400});
    facility->recipes.push_back(recipe);

    auto* player = world.createEntity("player3");
    auto* inv = addComp<components::Inventory>(player);
    inv->max_capacity = 10000.0f;
    components::Inventory::Item ore;
    ore.item_id = "Ferrite";
    ore.name = "Ferrite";
    ore.type = "ore";
    ore.quantity = 100;
    ore.volume = 0.1f;
    inv->items.push_back(ore);

    refiningSys.refineOre("player3", "station3", "Ferrite", 1);

    int mineral_qty = 0;
    for (const auto& item : inv->items) {
        if (item.item_id == "Stellium") mineral_qty = item.quantity;
    }
    // 400 * 1.0 * (1 - 0.1) = 360
    assertTrue(mineral_qty == 360, "10% tax yields 360 Stellium");
}

void testRefineOreInsufficientOre() {
    std::cout << "\n=== Refining: Insufficient Ore ===" << std::endl;

    ecs::World world;
    systems::RefiningSystem refiningSys(&world);

    auto* station = world.createEntity("station4");
    auto* facility = addComp<components::RefiningFacility>(station);
    facility->efficiency = 1.0f;
    facility->tax_rate = 0.0f;

    components::RefiningFacility::RefineRecipe recipe;
    recipe.ore_type = "Ferrite";
    recipe.ore_units_required = 100;
    recipe.outputs.push_back({"Stellium", 415});
    facility->recipes.push_back(recipe);

    auto* player = world.createEntity("player4");
    auto* inv = addComp<components::Inventory>(player);
    inv->max_capacity = 10000.0f;
    components::Inventory::Item ore;
    ore.item_id = "Ferrite";
    ore.name = "Ferrite";
    ore.type = "ore";
    ore.quantity = 50;  // not enough for 1 batch
    ore.volume = 0.1f;
    inv->items.push_back(ore);

    int refined = refiningSys.refineOre("player4", "station4", "Ferrite", 1);
    assertTrue(refined == 0, "Cannot refine with insufficient ore");
    assertTrue(inv->items[0].quantity == 50, "Ore not consumed when refining fails");
}

void testRefineOreNoRecipe() {
    std::cout << "\n=== Refining: No Recipe For Ore Type ===" << std::endl;

    ecs::World world;
    systems::RefiningSystem refiningSys(&world);

    auto* station = world.createEntity("station5");
    auto* facility = addComp<components::RefiningFacility>(station);
    // No recipes installed

    auto* player = world.createEntity("player5");
    auto* inv = addComp<components::Inventory>(player);
    inv->max_capacity = 10000.0f;
    components::Inventory::Item ore;
    ore.item_id = "Ferrite";
    ore.name = "Ferrite";
    ore.type = "ore";
    ore.quantity = 200;
    ore.volume = 0.1f;
    inv->items.push_back(ore);

    int refined = refiningSys.refineOre("player5", "station5", "Ferrite", 1);
    assertTrue(refined == 0, "Cannot refine without recipe");
}

void testRefineOreMultipleOutputs() {
    std::cout << "\n=== Refining: Multiple Mineral Outputs ===" << std::endl;

    ecs::World world;
    systems::RefiningSystem refiningSys(&world);

    auto* station = world.createEntity("station6");
    auto* facility = addComp<components::RefiningFacility>(station);
    facility->efficiency = 1.0f;
    facility->tax_rate = 0.0f;

    // Galvite → Stellium + Vanthium
    components::RefiningFacility::RefineRecipe recipe;
    recipe.ore_type = "Galvite";
    recipe.ore_units_required = 100;
    recipe.outputs.push_back({"Stellium", 346});
    recipe.outputs.push_back({"Vanthium", 173});
    facility->recipes.push_back(recipe);

    auto* player = world.createEntity("player6");
    auto* inv = addComp<components::Inventory>(player);
    inv->max_capacity = 10000.0f;
    components::Inventory::Item ore;
    ore.item_id = "Galvite";
    ore.name = "Galvite";
    ore.type = "ore";
    ore.quantity = 100;
    ore.volume = 0.15f;
    inv->items.push_back(ore);

    int refined = refiningSys.refineOre("player6", "station6", "Galvite", 1);
    assertTrue(refined == 1, "Refined 1 batch of Galvite");

    int stellium = 0, vanthium = 0;
    for (const auto& item : inv->items) {
        if (item.item_id == "Stellium") stellium = item.quantity;
        if (item.item_id == "Vanthium") vanthium = item.quantity;
    }
    assertTrue(stellium == 346, "Produced 346 Stellium from Galvite");
    assertTrue(vanthium == 173, "Produced 173 Vanthium from Galvite");
}

void testRefineDefaultRecipes() {
    std::cout << "\n=== Refining: Install Default Recipes ===" << std::endl;

    ecs::World world;
    systems::RefiningSystem refiningSys(&world);

    auto* station = world.createEntity("station7");
    addComp<components::RefiningFacility>(station);

    bool installed = refiningSys.installDefaultRecipes("station7");
    assertTrue(installed, "Default recipes installed");

    auto* facility = station->getComponent<components::RefiningFacility>();
    assertTrue(facility->recipes.size() == 4, "4 default recipes installed");
}

// ==================== Market Ore Pricing Tests ====================

void testMarketOrePricing() {
    std::cout << "\n=== Market: NPC Ore Pricing ===" << std::endl;

    ecs::World world;
    systems::MarketSystem marketSys(&world);

    // Create a station with market hub
    auto* station = world.createEntity("trade_hub");
    auto* hub = addComp<components::MarketHub>(station);
    hub->station_id = "trade_hub";
    hub->broker_fee_rate = 0.0;
    hub->sales_tax_rate = 0.0;

    // Create an NPC seller
    auto* npc_seller = world.createEntity("npc_ore_seller");
    auto* npc_player = addComp<components::Player>(npc_seller);
    npc_player->credits = 1000000.0;

    // NPC places sell orders for common ores
    std::string o1 = marketSys.placeSellOrder("trade_hub", "npc_ore_seller",
                                               "Ferrite", "Ferrite", 10000, 15.0);
    std::string o2 = marketSys.placeSellOrder("trade_hub", "npc_ore_seller",
                                               "Galvite", "Galvite", 5000, 38.0);
    std::string o3 = marketSys.placeSellOrder("trade_hub", "npc_ore_seller",
                                               "Cryolite", "Cryolite", 3000, 70.0);

    assertTrue(!o1.empty(), "Ferrite sell order placed");
    assertTrue(!o2.empty(), "Galvite sell order placed");
    assertTrue(!o3.empty(), "Cryolite sell order placed");

    double veldspar_price = marketSys.getLowestSellPrice("trade_hub", "Ferrite");
    double scordite_price = marketSys.getLowestSellPrice("trade_hub", "Galvite");
    double pyroxeres_price = marketSys.getLowestSellPrice("trade_hub", "Cryolite");

    assertTrue(veldspar_price == 15.0, "Ferrite price is 15 Credits");
    assertTrue(scordite_price == 38.0, "Galvite price is 38 Credits");
    assertTrue(pyroxeres_price == 70.0, "Cryolite price is 70 Credits");
    assertTrue(marketSys.getOrderCount("trade_hub") == 3, "3 orders on market");
}

void testMarketMineralPricing() {
    std::cout << "\n=== Market: NPC Mineral Buy Orders ===" << std::endl;

    ecs::World world;
    systems::MarketSystem marketSys(&world);

    auto* station = world.createEntity("mineral_hub");
    auto* hub = addComp<components::MarketHub>(station);
    hub->station_id = "mineral_hub";
    hub->broker_fee_rate = 0.0;
    hub->sales_tax_rate = 0.0;

    auto* npc_buyer = world.createEntity("npc_mineral_buyer");
    auto* npc_player = addComp<components::Player>(npc_buyer);
    npc_player->credits = 10000000.0;

    // NPC places buy orders for refined minerals
    std::string b1 = marketSys.placeBuyOrder("mineral_hub", "npc_mineral_buyer",
                                              "Stellium", "Stellium", 50000, 6.0);
    std::string b2 = marketSys.placeBuyOrder("mineral_hub", "npc_mineral_buyer",
                                              "Vanthium", "Vanthium", 20000, 9.0);

    assertTrue(!b1.empty(), "Stellium buy order placed");
    assertTrue(!b2.empty(), "Vanthium buy order placed");

    double trit_price = marketSys.getHighestBuyPrice("mineral_hub", "Stellium");
    double pyer_price = marketSys.getHighestBuyPrice("mineral_hub", "Vanthium");

    assertTrue(trit_price == 6.0, "Stellium buy price is 6 Credits");
    assertTrue(pyer_price == 9.0, "Vanthium buy price is 9 Credits");
}

// ==================== NPC Market Seeding Tests ====================

void testNPCMarketSeedCreatesOrders() {
    std::cout << "\n=== NPC Market Seed Creates Orders ===" << std::endl;
    ecs::World world;
    systems::MarketSystem market(&world);

    auto* station = world.createEntity("station_seed1");
    auto* hub = addComp<components::MarketHub>(station);
    hub->station_id = "station_seed1";

    int created = market.seedNPCOrders("station_seed1");
    assertTrue(created == 4, "seedNPCOrders creates 4 NPC sell orders");
    assertTrue(hub->orders.size() == 4, "MarketHub has 4 orders after seeding");
}

void testNPCMarketSeedPricesCorrect() {
    std::cout << "\n=== NPC Market Seed Prices Correct ===" << std::endl;
    ecs::World world;
    systems::MarketSystem market(&world);

    auto* station = world.createEntity("station_seed2");
    auto* hub = addComp<components::MarketHub>(station);
    hub->station_id = "station_seed2";

    market.seedNPCOrders("station_seed2");

    assertTrue(hub->orders[0].item_name == "Stellium", "First order is Stellium");
    assertTrue(approxEqual(hub->orders[0].price_per_unit, 6.0), "Stellium price is 6.0 Credits");
    assertTrue(hub->orders[1].item_name == "Vanthium", "Second order is Vanthium");
    assertTrue(approxEqual(hub->orders[1].price_per_unit, 10.0), "Vanthium price is 10.0 Credits");
    assertTrue(hub->orders[2].item_name == "Cydrium", "Third order is Cydrium");
    assertTrue(approxEqual(hub->orders[2].price_per_unit, 40.0), "Cydrium price is 40.0 Credits");
    assertTrue(hub->orders[3].item_name == "Nocxidium", "Fourth order is Nocxidium");
    assertTrue(approxEqual(hub->orders[3].price_per_unit, 800.0, 1.0), "Nocxidium price is 800.0 Credits");
}

void testNPCMarketSeedOrdersPermanent() {
    std::cout << "\n=== NPC Market Seed Orders Permanent ===" << std::endl;
    ecs::World world;
    systems::MarketSystem market(&world);

    auto* station = world.createEntity("station_seed3");
    auto* hub = addComp<components::MarketHub>(station);
    hub->station_id = "station_seed3";

    market.seedNPCOrders("station_seed3");

    for (const auto& order : hub->orders) {
        assertTrue(order.duration_remaining < 0.0f, "NPC order is permanent (duration_remaining < 0)");
        assertTrue(order.owner_id == "npc_market", "NPC order owned by npc_market");
        assertTrue(!order.is_buy_order, "NPC seed orders are sell orders");
    }
}

void testNPCMarketSeedBuyableByPlayer() {
    std::cout << "\n=== NPC Market Seed Buyable By Player ===" << std::endl;
    ecs::World world;
    systems::MarketSystem market(&world);

    auto* station = world.createEntity("station_seed4");
    auto* hub = addComp<components::MarketHub>(station);
    hub->station_id = "station_seed4";

    auto* buyer = world.createEntity("player_buyer");
    auto* player = addComp<components::Player>(buyer);
    player->credits = 10000.0;
    addComp<components::Inventory>(buyer);

    market.seedNPCOrders("station_seed4");

    double tritPrice = market.getLowestSellPrice("station_seed4", "mineral_tritanium");
    assertTrue(tritPrice > 0.0, "Stellium sell price exists after seeding");
    assertTrue(approxEqual(tritPrice, 6.0), "Stellium sell price is 6.0 Credits");
}

// ==================== Market Order Tests ====================

void testMarketOrderComponentDefaults() {
    std::cout << "\n=== Market Order Component Defaults ===" << std::endl;
    ecs::World world;
    auto* e = world.createEntity("order1");
    auto* order = addComp<components::MarketOrder>(e);
    assertTrue(!order->is_filled, "Not filled");
    assertTrue(approxEqual(order->elapsed_time, 0.0f), "No elapsed time");
}

void testMarketPlaceOrderSystem() {
    std::cout << "\n=== Market Place Order System ===" << std::endl;
    ecs::World world;
    auto* e = world.createEntity("order2");
    addComp<components::MarketOrder>(e);

    systems::MarketOrderSystem sys(&world);
    sys.placeOrder("order2", components::MarketOrder::OrderType::Sell,
                   "Stellium", 1000, 5.0f, "region_a", "station_1", "player_1");

    auto* order = e->getComponent<components::MarketOrder>();
    assertTrue(order->item_type == "Stellium", "Item is Stellium");
    assertTrue(order->quantity == 1000, "Quantity 1000");
    assertTrue(order->quantity_remaining == 1000, "Remaining 1000");
    assertTrue(approxEqual(order->price_per_unit, 5.0f), "Price 5.0");
}

void testMarketFillOrderSystem() {
    std::cout << "\n=== Market Fill Order System ===" << std::endl;
    ecs::World world;
    auto* e = world.createEntity("order3");
    addComp<components::MarketOrder>(e);

    systems::MarketOrderSystem sys(&world);
    sys.placeOrder("order3", components::MarketOrder::OrderType::Buy,
                   "Vanthium", 500, 10.0f, "region_b", "station_2", "player_2");

    int filled = sys.fillOrder("order3", 200);
    assertTrue(filled == 200, "Filled 200");
    auto* order = e->getComponent<components::MarketOrder>();
    assertTrue(order->quantity_remaining == 300, "300 remaining");
    assertTrue(!order->is_filled, "Not fully filled yet");

    int filled2 = sys.fillOrder("order3", 300);
    assertTrue(filled2 == 300, "Filled remaining 300");
    assertTrue(order->is_filled, "Now fully filled");
}

void testMarketOrderExpirySystem() {
    std::cout << "\n=== Market Order Expiry System ===" << std::endl;
    ecs::World world;
    auto* e = world.createEntity("order4");
    auto* order = addComp<components::MarketOrder>(e);
    order->expiry_time = 100.0f;

    systems::MarketOrderSystem sys(&world);
    sys.placeOrder("order4", components::MarketOrder::OrderType::Sell,
                   "Cydrium", 100, 20.0f, "region_c", "station_3", "npc_1");

    assertTrue(!sys.isOrderExpired("order4"), "Not expired initially");
    sys.update(101.0f);
    assertTrue(sys.isOrderExpired("order4"), "Expired after 101s");
}

void testAIFleetDispatchSystem() {
    std::cout << "\n=== AI Fleet Dispatch ===" << std::endl;
    ecs::World world;
    auto* e = world.createEntity("order5");
    addComp<components::MarketOrder>(e);

    systems::MarketOrderSystem sys(&world);
    std::string dispatch_id = sys.dispatchAIFleet("order5",
        components::AIFleetDispatch::DispatchType::Hauling, "system_alpha", 2);
    assertTrue(!dispatch_id.empty(), "Dispatch created");

    auto dispatches = sys.getActiveDispatches();
    assertTrue(static_cast<int>(dispatches.size()) == 1, "One active dispatch");

    auto* de = world.getEntity(dispatch_id);
    auto* dispatch = de->getComponent<components::AIFleetDispatch>();
    assertTrue(!dispatch->isComplete(), "Not complete yet");

    // Simulate enough time for completion
    sys.update(dispatch->estimated_completion + 1.0f);
    assertTrue(dispatch->isComplete(), "Complete after time");
}

// ==================== Black Market System Tests ====================

void testBlackMarketDefaults() {
    std::cout << "\n=== Black Market Defaults ===" << std::endl;
    components::BlackMarket bm;
    assertTrue(bm.listings.empty(), "Default listings is empty");
    assertTrue(approxEqual(bm.security_level, 0.5f), "Default security_level is 0.5");
    assertTrue(approxEqual(bm.detection_chance_base, 0.1f), "Default detection_chance_base is 0.1");
    assertTrue(approxEqual(bm.price_markup, 1.5f), "Default price_markup is 1.5");
    assertTrue(bm.max_listings == 20, "Default max_listings is 20");
    assertTrue(approxEqual(bm.listing_refresh_interval, 120.0f), "Default listing_refresh_interval is 120");
}

void testBlackMarketAddListing() {
    std::cout << "\n=== Black Market Add Listing ===" << std::endl;
    ecs::World world;
    systems::BlackMarketSystem bmSys(&world);
    auto* entity = world.createEntity("market_1");
    addComp<components::BlackMarket>(entity);

    bmSys.addListing("market_1", "stolen_ore", "seller_a", 100.0f, 5, true, 0.3f);
    assertTrue(bmSys.getListingCount("market_1") == 1, "One listing added");

    auto items = bmSys.getAvailableItems("market_1");
    assertTrue(items.size() == 1 && items[0] == "stolen_ore", "Available items contains stolen_ore");
}

void testBlackMarketPurchase() {
    std::cout << "\n=== Black Market Purchase ===" << std::endl;
    ecs::World world;
    systems::BlackMarketSystem bmSys(&world);
    auto* entity = world.createEntity("market_2");
    auto* bm = addComp<components::BlackMarket>(entity);
    bm->addListing("rare_gem", "seller_b", 200.0f, 3, false, 0.1f);

    bool ok = bmSys.purchaseItem("market_2", "rare_gem", "buyer_x");
    assertTrue(ok, "Purchase succeeds");
    auto* listing = bm->findListing("rare_gem");
    assertTrue(listing != nullptr && listing->quantity == 2, "Quantity decreased to 2");
}

void testBlackMarketPurchaseRemovesEmpty() {
    std::cout << "\n=== Black Market Purchase Removes Empty ===" << std::endl;
    ecs::World world;
    systems::BlackMarketSystem bmSys(&world);
    auto* entity = world.createEntity("market_3");
    auto* bm = addComp<components::BlackMarket>(entity);
    bm->addListing("last_item", "seller_c", 50.0f, 1, true, 0.5f);

    bool ok = bmSys.purchaseItem("market_3", "last_item", "buyer_y");
    assertTrue(ok, "Purchase of last item succeeds");
    assertTrue(bm->getListingCount() == 0, "Listing removed when quantity reaches 0");
}

void testBlackMarketExpiry() {
    std::cout << "\n=== Black Market Expiry ===" << std::endl;
    ecs::World world;
    systems::BlackMarketSystem bmSys(&world);
    auto* entity = world.createEntity("market_4");
    auto* bm = addComp<components::BlackMarket>(entity);
    bm->addListing("temp_goods", "seller_d", 80.0f, 2, false, 0.2f);
    bm->listings[0].max_expiry = 10.0f;

    bmSys.update(5.0f);
    assertTrue(bm->getListingCount() == 1, "Listing still present before expiry");

    bmSys.update(6.0f);
    assertTrue(bm->getListingCount() == 0, "Listing removed after expiry");
}

void testBlackMarketMaxListings() {
    std::cout << "\n=== Black Market Max Listings ===" << std::endl;
    components::BlackMarket bm;
    bm.max_listings = 3;
    bm.addListing("item_a", "s1", 10.0f, 1, false, 0.1f);
    bm.addListing("item_b", "s2", 20.0f, 1, false, 0.1f);
    bm.addListing("item_c", "s3", 30.0f, 1, false, 0.1f);
    assertTrue(bm.getListingCount() == 3, "3 listings at max");

    bm.addListing("item_d", "s4", 40.0f, 1, false, 0.1f);
    assertTrue(bm.getListingCount() == 3, "Still at max after adding 4th");
    assertTrue(bm.findListing("item_a") == nullptr, "Oldest listing (item_a) removed");
    assertTrue(bm.findListing("item_d") != nullptr, "Newest listing (item_d) present");
}

void testBlackMarketDetectionChance() {
    std::cout << "\n=== Black Market Detection Chance ===" << std::endl;
    ecs::World world;
    systems::BlackMarketSystem bmSys(&world);
    auto* entity = world.createEntity("market_5");
    auto* bm = addComp<components::BlackMarket>(entity);
    bm->detection_chance_base = 0.2f;
    bm->security_level = 0.5f;

    float chance = bmSys.getDetectionChance("market_5");
    assertTrue(approxEqual(chance, 0.1f), "Detection chance = base * security (0.2 * 0.5 = 0.1)");

    bmSys.setSecurityLevel("market_5", 1.0f);
    chance = bmSys.getDetectionChance("market_5");
    assertTrue(approxEqual(chance, 0.2f), "Detection chance scales with security level");
}

// ==================== Price History System Tests ====================

void testPriceHistoryDefaults() {
    std::cout << "\n=== Price History Defaults ===" << std::endl;
    ecs::World world;
    systems::PriceHistorySystem priceHistSys(&world);
    
    // No history recorded yet
    auto history = priceHistSys.getHistory("region1", "tritanium");
    assertTrue(history.empty(), "No history for untracked item");
    
    double avg = priceHistSys.getAveragePrice("region1", "tritanium", 3600.0f, 0.0f);
    assertTrue(avg < 0.0, "Average returns -1 when no data");
}

void testPriceHistoryRecording() {
    std::cout << "\n=== Price History Recording ===" << std::endl;
    ecs::World world;
    systems::PriceHistorySystem priceHistSys(&world);
    
    priceHistSys.recordPrice("jita", "tritanium", 6.0, 5.5, 1000, 0.0f);
    priceHistSys.recordPrice("jita", "tritanium", 6.2, 5.6, 800, 3600.0f);
    priceHistSys.recordPrice("jita", "tritanium", 6.5, 5.8, 1200, 7200.0f);
    
    auto history = priceHistSys.getHistory("jita", "tritanium");
    assertTrue(history.size() == 3, "3 price entries recorded");
    assertTrue(approxEqual(static_cast<float>(history[0].sell_price), 6.0f), "First entry sell price correct");
    assertTrue(approxEqual(static_cast<float>(history[2].sell_price), 6.5f), "Last entry sell price correct");
}

void testPriceHistoryAverage() {
    std::cout << "\n=== Price History Average ===" << std::endl;
    ecs::World world;
    systems::PriceHistorySystem priceHistSys(&world);
    
    priceHistSys.recordPrice("amarr", "mexallon", 40.0, 38.0, 500, 0.0f);
    priceHistSys.recordPrice("amarr", "mexallon", 44.0, 42.0, 600, 1800.0f);
    priceHistSys.recordPrice("amarr", "mexallon", 46.0, 44.0, 400, 3600.0f);
    
    double avg = priceHistSys.getAveragePrice("amarr", "mexallon", 4000.0f, 3600.0f);
    // Average of 40, 44, 46 = 43.33
    assertTrue(approxEqual(static_cast<float>(avg), 43.33f, 0.1f), "Average price calculated correctly");
}

void testPriceHistoryTrend() {
    std::cout << "\n=== Price History Trend ===" << std::endl;
    ecs::World world;
    systems::PriceHistorySystem priceHistSys(&world);
    
    // Rising prices
    priceHistSys.recordPrice("dodixie", "pyerite", 10.0, 9.0, 100, 0.0f);
    priceHistSys.recordPrice("dodixie", "pyerite", 12.0, 11.0, 100, 3600.0f);
    priceHistSys.recordPrice("dodixie", "pyerite", 14.0, 13.0, 100, 7200.0f);
    priceHistSys.recordPrice("dodixie", "pyerite", 16.0, 15.0, 100, 10800.0f);
    
    float trend = priceHistSys.getPriceTrend("dodixie", "pyerite", 4);
    assertTrue(trend > 0.0f, "Rising prices show positive trend");
}

void testPriceHistoryVolume() {
    std::cout << "\n=== Price History Volume ===" << std::endl;
    ecs::World world;
    systems::PriceHistorySystem priceHistSys(&world);
    
    priceHistSys.recordPrice("rens", "isogen", 50.0, 48.0, 1000, 0.0f);
    priceHistSys.recordPrice("rens", "isogen", 52.0, 50.0, 2000, 1800.0f);
    priceHistSys.recordPrice("rens", "isogen", 51.0, 49.0, 1500, 3600.0f);
    
    int vol = priceHistSys.getTotalVolume("rens", "isogen", 4000.0f, 3600.0f);
    assertTrue(vol == 4500, "Total volume is sum of all entries (1000+2000+1500=4500)");
}

void testPriceHistoryIntervalConfig() {
    std::cout << "\n=== Price History Interval Config ===" << std::endl;
    ecs::World world;
    systems::PriceHistorySystem priceHistSys(&world);
    
    assertTrue(approxEqual(priceHistSys.getSnapshotInterval(), 3600.0f), "Default interval is 1 hour");
    priceHistSys.setSnapshotInterval(1800.0f);
    assertTrue(approxEqual(priceHistSys.getSnapshotInterval(), 1800.0f), "Interval updated to 30 minutes");
    
    assertTrue(priceHistSys.getMaxHistoryEntries() == 720, "Default max entries is 720");
    priceHistSys.setMaxHistoryEntries(168);
    assertTrue(priceHistSys.getMaxHistoryEntries() == 168, "Max entries updated to 168");
}

// ==================== EconomicFlowSystem Tests ====================

void testEconomicFlowDefaults() {
    std::cout << "\n=== Economic Flow: Defaults ===" << std::endl;
    ecs::World world;
    auto* sys_entity = world.createEntity("system1");
    addComp<components::EconomicFlowState>(sys_entity);

    systems::EconomicFlowSystem sys(&world);
    assertTrue(approxEqual(sys.getEconomicHealth("system1"), 1.0f), "Default health is 1.0");
    assertTrue(approxEqual(sys.getTotalProduction("system1"), 0.0f), "Default production is 0");
    assertTrue(approxEqual(sys.getTotalConsumption("system1"), 0.0f), "Default consumption is 0");
}

void testEconomicFlowRecordProduction() {
    std::cout << "\n=== Economic Flow: Record Production ===" << std::endl;
    ecs::World world;
    auto* sys_entity = world.createEntity("system1");
    addComp<components::EconomicFlowState>(sys_entity);

    systems::EconomicFlowSystem sys(&world);
    sys.recordProduction("system1", "ore", 100.0f);
    assertTrue(approxEqual(sys.getProductionRate("system1", "ore"), 100.0f), "Production recorded");
}

void testEconomicFlowRecordConsumption() {
    std::cout << "\n=== Economic Flow: Record Consumption ===" << std::endl;
    ecs::World world;
    auto* sys_entity = world.createEntity("system1");
    addComp<components::EconomicFlowState>(sys_entity);

    systems::EconomicFlowSystem sys(&world);
    sys.recordConsumption("system1", "fuel", 50.0f);
    assertTrue(approxEqual(sys.getConsumptionRate("system1", "fuel"), 50.0f), "Consumption recorded");
}

void testEconomicFlowTransport() {
    std::cout << "\n=== Economic Flow: Transport ===" << std::endl;
    ecs::World world;
    auto* s1 = world.createEntity("system1");
    addComp<components::EconomicFlowState>(s1);
    auto* s2 = world.createEntity("system2");
    addComp<components::EconomicFlowState>(s2);

    systems::EconomicFlowSystem sys(&world);
    sys.recordTransport("system1", "system2", "ore", 30.0f);

    auto* flow1 = s1->getComponent<components::EconomicFlowState>();
    auto* flow2 = s2->getComponent<components::EconomicFlowState>();
    assertTrue(approxEqual(flow1->transport_out_rate["ore"], 30.0f), "Transport out recorded");
    assertTrue(approxEqual(flow2->transport_in_rate["ore"], 30.0f), "Transport in recorded");
}

void testEconomicFlowDestruction() {
    std::cout << "\n=== Economic Flow: Destruction ===" << std::endl;
    ecs::World world;
    auto* sys_entity = world.createEntity("system1");
    addComp<components::EconomicFlowState>(sys_entity);

    systems::EconomicFlowSystem sys(&world);
    sys.recordDestruction("system1", "cargo", 20.0f);

    auto* flow = sys_entity->getComponent<components::EconomicFlowState>();
    assertTrue(approxEqual(flow->destruction_rate["cargo"], 20.0f), "Destruction recorded");
}

void testEconomicFlowNetFlow() {
    std::cout << "\n=== Economic Flow: Net Flow ===" << std::endl;
    ecs::World world;
    auto* sys_entity = world.createEntity("system1");
    auto* flow = addComp<components::EconomicFlowState>(sys_entity);
    flow->production_rate["ore"] = 100.0f;
    flow->consumption_rate["ore"] = 40.0f;
    flow->transport_in_rate["ore"] = 10.0f;
    flow->transport_out_rate["ore"] = 20.0f;
    flow->destruction_rate["ore"] = 5.0f;

    systems::EconomicFlowSystem sys(&world);
    // net = 100 + 10 - 40 - 20 - 5 = 45
    float net = sys.getNetFlow("system1", "ore");
    assertTrue(approxEqual(net, 45.0f), "Net flow is 45");
}

void testEconomicFlowHealthUpdate() {
    std::cout << "\n=== Economic Flow: Health Update ===" << std::endl;
    ecs::World world;
    auto* sys_entity = world.createEntity("system1");
    auto* flow = addComp<components::EconomicFlowState>(sys_entity);
    flow->production_rate["ore"] = 100.0f;
    flow->consumption_rate["ore"] = 50.0f;

    systems::EconomicFlowSystem sys(&world);
    sys.update(1.0f);

    // health should move towards 100/50 = 2.0 (clamped)
    float health = sys.getEconomicHealth("system1");
    assertTrue(health > 1.0f, "Health above 1.0 with surplus production");
}

void testEconomicFlowMissingEntity() {
    std::cout << "\n=== Economic Flow: Missing Entity ===" << std::endl;
    ecs::World world;
    systems::EconomicFlowSystem sys(&world);
    assertTrue(approxEqual(sys.getEconomicHealth("nonexistent"), 1.0f), "Default health for missing");
    assertTrue(approxEqual(sys.getNetFlow("nonexistent", "ore"), 0.0f), "Default net flow for missing");
    assertTrue(approxEqual(sys.getTotalProduction("nonexistent"), 0.0f), "Default production for missing");
}

void testEconomicFlowAutoCreateComponent() {
    std::cout << "\n=== Economic Flow: Auto-create Component ===" << std::endl;
    ecs::World world;
    world.createEntity("system1");

    systems::EconomicFlowSystem sys(&world);
    sys.recordProduction("system1", "ore", 50.0f);
    assertTrue(approxEqual(sys.getProductionRate("system1", "ore"), 50.0f), "Component auto-created and production recorded");
}

// ==================== ResourceProductionChainSystem Tests ====================

void testProductionChainCreate() {
    std::cout << "\n=== Production Chain: Create ===" << std::endl;
    ecs::World world;
    world.createEntity("system1");

    systems::ResourceProductionChainSystem sys(&world);
    assertTrue(sys.createChain("system1", "ore_to_metal"), "Chain created");
    assertTrue(!sys.createChain("system1", "another"), "Duplicate chain rejected");
    assertTrue(!sys.createChain("nonexistent", "chain"), "Missing entity fails");
}

void testProductionChainAddStage() {
    std::cout << "\n=== Production Chain: Add Stage ===" << std::endl;
    ecs::World world;
    world.createEntity("system1");

    systems::ResourceProductionChainSystem sys(&world);
    sys.createChain("system1", "ore_pipeline");
    assertTrue(sys.addStage("system1", "mining", "asteroid", "ore", 1.0f), "Mining stage added");
    assertTrue(sys.addStage("system1", "refining", "ore", "mineral", 0.8f), "Refining stage added");
    assertTrue(sys.addStage("system1", "manufacturing", "mineral", "product", 0.5f), "Manufacturing stage added");
    assertTrue(!sys.addStage("system1", "mining", "x", "y", 1.0f), "Duplicate stage rejected");
    assertTrue(sys.getStageCount("system1") == 3, "Stage count is 3");
}

void testProductionChainRemoveStage() {
    std::cout << "\n=== Production Chain: Remove Stage ===" << std::endl;
    ecs::World world;
    world.createEntity("system1");

    systems::ResourceProductionChainSystem sys(&world);
    sys.createChain("system1", "chain1");
    sys.addStage("system1", "mining", "asteroid", "ore", 1.0f);
    sys.addStage("system1", "refining", "ore", "mineral", 0.8f);
    assertTrue(sys.removeStage("system1", "mining"), "Stage removed");
    assertTrue(sys.getStageCount("system1") == 1, "Stage count is 1 after removal");
    assertTrue(!sys.removeStage("system1", "nonexistent"), "Nonexistent stage removal fails");
}

void testProductionChainEfficiency() {
    std::cout << "\n=== Production Chain: Efficiency ===" << std::endl;
    ecs::World world;
    world.createEntity("system1");

    systems::ResourceProductionChainSystem sys(&world);
    sys.createChain("system1", "chain1");
    sys.addStage("system1", "mining", "asteroid", "ore", 1.0f);
    sys.addStage("system1", "refining", "ore", "mineral", 0.8f);
    sys.setStageEfficiency("system1", "mining", 0.9f);
    sys.setStageEfficiency("system1", "refining", 0.7f);
    sys.update(1.0f);
    // Overall = 0.9 * 0.7 = 0.63
    assertTrue(approxEqual(sys.getOverallEfficiency("system1"), 0.63f), "Overall efficiency is 0.63");
}

void testProductionChainThroughput() {
    std::cout << "\n=== Production Chain: Throughput ===" << std::endl;
    ecs::World world;
    world.createEntity("system1");

    systems::ResourceProductionChainSystem sys(&world);
    sys.createChain("system1", "chain1");
    sys.addStage("system1", "mining", "asteroid", "ore", 1.0f);
    sys.addStage("system1", "refining", "ore", "mineral", 0.5f);
    sys.update(1.0f);
    // mining: input=1.0, rate=1.0, eff=1.0, bottleneck=0.5 (downstream limited) → 0.5
    // refining: input=0.5, rate=0.5, eff=1.0, bottleneck=1.0 → 0.25
    float output = sys.getTotalOutput("system1");
    assertTrue(output > 0.0f, "Total output is positive");
    assertTrue(approxEqual(sys.getStageThroughput("system1", "refining"), 0.25f), "Refining throughput is 0.25");
}

void testProductionChainBottleneck() {
    std::cout << "\n=== Production Chain: Bottleneck ===" << std::endl;
    ecs::World world;
    world.createEntity("system1");

    systems::ResourceProductionChainSystem sys(&world);
    sys.createChain("system1", "chain1");
    sys.addStage("system1", "mining", "asteroid", "ore", 2.0f);     // high output
    sys.addStage("system1", "refining", "ore", "mineral", 0.5f);    // low capacity
    sys.update(1.0f);
    // mining bottleneck: downstream capacity (0.5*1.0) / upstream output (2.0*1.0) = 0.25
    float bottleneck = sys.getBottleneckFactor("system1", "mining");
    assertTrue(bottleneck < 1.0f, "Mining stage has bottleneck");
    assertTrue(approxEqual(bottleneck, 0.25f), "Bottleneck factor is 0.25");
}

void testProductionChainActiveToggle() {
    std::cout << "\n=== Production Chain: Active Toggle ===" << std::endl;
    ecs::World world;
    world.createEntity("system1");

    systems::ResourceProductionChainSystem sys(&world);
    sys.createChain("system1", "chain1");
    assertTrue(sys.isChainActive("system1"), "Chain starts active");
    sys.setChainActive("system1", false);
    assertTrue(!sys.isChainActive("system1"), "Chain deactivated");
    sys.setChainActive("system1", true);
    assertTrue(sys.isChainActive("system1"), "Chain reactivated");
}

void testProductionChainUptime() {
    std::cout << "\n=== Production Chain: Uptime ===" << std::endl;
    ecs::World world;
    world.createEntity("system1");

    systems::ResourceProductionChainSystem sys(&world);
    sys.createChain("system1", "chain1");
    sys.addStage("system1", "mining", "asteroid", "ore", 1.0f);
    assertTrue(approxEqual(sys.getUptime("system1"), 0.0f), "Initial uptime is 0");
    sys.update(5.0f);
    assertTrue(approxEqual(sys.getUptime("system1"), 5.0f), "Uptime after 5s tick");
    sys.update(3.0f);
    assertTrue(approxEqual(sys.getUptime("system1"), 8.0f), "Uptime after 8s total");
}

void testProductionChainMissing() {
    std::cout << "\n=== Production Chain: Missing Entity ===" << std::endl;
    ecs::World world;
    systems::ResourceProductionChainSystem sys(&world);
    assertTrue(sys.getOverallEfficiency("nonexistent") == 0.0f, "Default efficiency for missing");
    assertTrue(sys.getTotalOutput("nonexistent") == 0.0f, "Default output for missing");
    assertTrue(sys.getStageCount("nonexistent") == 0, "Default stage count for missing");
    assertTrue(!sys.isChainActive("nonexistent"), "Default active for missing");
}

void testProductionChainComponentDefaults() {
    std::cout << "\n=== Production Chain: Component Defaults ===" << std::endl;
    components::ResourceProductionChain chain;
    assertTrue(chain.stages.empty(), "Default stages empty");
    assertTrue(approxEqual(chain.overall_efficiency, 1.0f), "Default efficiency is 1.0");
    assertTrue(approxEqual(chain.total_output, 0.0f), "Default output is 0.0");
    assertTrue(chain.is_active, "Default is active");
    assertTrue(chain.chain_id.empty(), "Default chain_id empty");
}

// ==================== LoyaltyPointStore System Tests ====================

void testLPStoreCreate() {
    std::cout << "\n=== LPStore: Create ===" << std::endl;
    ecs::World world;
    systems::LoyaltyPointStoreSystem sys(&world);
    world.createEntity("lp1");
    assertTrue(sys.initialize("lp1", "store_caldari", "caldari_navy"), "Init succeeds");
    assertTrue(sys.getItemCount("lp1") == 0, "No items initially");
    assertTrue(sys.getPlayerCount("lp1") == 0, "No players initially");
    assertTrue(sys.getTotalPurchases("lp1") == 0, "No purchases initially");
    assertTrue(approxEqual(sys.getTotalISKCollected("lp1"), 0.0f), "No ISK collected");
}

void testLPStoreAddItem() {
    std::cout << "\n=== LPStore: AddItem ===" << std::endl;
    ecs::World world;
    systems::LoyaltyPointStoreSystem sys(&world);
    world.createEntity("lp1");
    sys.initialize("lp1", "store_caldari", "caldari_navy");
    assertTrue(sys.addItem("lp1", "item1", "Navy Raven", "Ship", 500000, 200000000.0f, 3), "Add Ship");
    assertTrue(sys.addItem("lp1", "item2", "Navy BCU", "Module", 50000, 5000000.0f, 2), "Add Module");
    assertTrue(sys.getItemCount("lp1") == 2, "2 items in store");
}

void testLPStoreDuplicate() {
    std::cout << "\n=== LPStore: Duplicate ===" << std::endl;
    ecs::World world;
    systems::LoyaltyPointStoreSystem sys(&world);
    world.createEntity("lp1");
    sys.initialize("lp1", "store_caldari", "caldari_navy");
    sys.addItem("lp1", "item1", "Navy Raven", "Ship", 500000, 200000000.0f, 3);
    assertTrue(!sys.addItem("lp1", "item1", "Other", "Module", 100, 100.0f, 1), "Duplicate item rejected");
    sys.registerPlayer("lp1", "player1");
    assertTrue(!sys.registerPlayer("lp1", "player1"), "Duplicate player rejected");
}

void testLPStoreEarnLP() {
    std::cout << "\n=== LPStore: EarnLP ===" << std::endl;
    ecs::World world;
    systems::LoyaltyPointStoreSystem sys(&world);
    world.createEntity("lp1");
    sys.initialize("lp1", "store_caldari", "caldari_navy");
    sys.registerPlayer("lp1", "player1");
    assertTrue(sys.earnLP("lp1", "player1", 10000), "Earn LP succeeds");
    assertTrue(sys.getBalance("lp1", "player1") == 10000, "Balance is 10000");
    assertTrue(sys.earnLP("lp1", "player1", 5000), "Earn more LP");
    assertTrue(sys.getBalance("lp1", "player1") == 15000, "Balance is 15000");
}

void testLPStorePurchase() {
    std::cout << "\n=== LPStore: Purchase ===" << std::endl;
    ecs::World world;
    systems::LoyaltyPointStoreSystem sys(&world);
    world.createEntity("lp1");
    sys.initialize("lp1", "store_caldari", "caldari_navy");
    sys.addItem("lp1", "item1", "Navy BCU", "Module", 50000, 5000000.0f, 2);
    sys.registerPlayer("lp1", "player1");
    sys.earnLP("lp1", "player1", 100000);
    assertTrue(sys.purchaseItem("lp1", "player1", "item1"), "Purchase succeeds");
    assertTrue(sys.getBalance("lp1", "player1") == 50000, "Balance reduced by 50000");
    assertTrue(sys.getTotalPurchases("lp1") == 1, "1 total purchase");
    assertTrue(approxEqual(sys.getTotalISKCollected("lp1"), 5000000.0f), "ISK collected 5M");
}

void testLPStorePurchaseFail() {
    std::cout << "\n=== LPStore: PurchaseFail ===" << std::endl;
    ecs::World world;
    systems::LoyaltyPointStoreSystem sys(&world);
    world.createEntity("lp1");
    sys.initialize("lp1", "store_caldari", "caldari_navy");
    sys.addItem("lp1", "item1", "Navy Raven", "Ship", 500000, 200000000.0f, 3);
    sys.registerPlayer("lp1", "player1");
    sys.earnLP("lp1", "player1", 100);
    assertTrue(!sys.purchaseItem("lp1", "player1", "item1"), "Insufficient LP rejected");
    assertTrue(sys.getBalance("lp1", "player1") == 100, "Balance unchanged");
    assertTrue(sys.getTotalPurchases("lp1") == 0, "No purchases");
}

void testLPStoreCategory() {
    std::cout << "\n=== LPStore: Category ===" << std::endl;
    ecs::World world;
    systems::LoyaltyPointStoreSystem sys(&world);
    world.createEntity("lp1");
    sys.initialize("lp1", "store_caldari", "caldari_navy");
    sys.addItem("lp1", "i1", "Navy Raven", "Ship", 500000, 200000000.0f, 3);
    sys.addItem("lp1", "i2", "Navy Caracal", "Ship", 200000, 50000000.0f, 2);
    sys.addItem("lp1", "i3", "Navy BCU", "Module", 50000, 5000000.0f, 2);
    sys.addItem("lp1", "i4", "Ammo Pack", "Ammunition", 1000, 100000.0f, 1);
    assertTrue(sys.getItemsByCategory("lp1", "Ship") == 2, "2 ships");
    assertTrue(sys.getItemsByCategory("lp1", "Module") == 1, "1 module");
    assertTrue(sys.getItemsByCategory("lp1", "Ammunition") == 1, "1 ammo");
    assertTrue(sys.getItemsByCategory("lp1", "Blueprint") == 0, "0 blueprints");
}

void testLPStoreMultiPlayer() {
    std::cout << "\n=== LPStore: MultiPlayer ===" << std::endl;
    ecs::World world;
    systems::LoyaltyPointStoreSystem sys(&world);
    world.createEntity("lp1");
    sys.initialize("lp1", "store_caldari", "caldari_navy");
    sys.registerPlayer("lp1", "player1");
    sys.registerPlayer("lp1", "player2");
    sys.earnLP("lp1", "player1", 5000);
    sys.earnLP("lp1", "player2", 10000);
    assertTrue(sys.getBalance("lp1", "player1") == 5000, "Player1 has 5000");
    assertTrue(sys.getBalance("lp1", "player2") == 10000, "Player2 has 10000");
    assertTrue(sys.getPlayerCount("lp1") == 2, "2 players");
}

void testLPStoreMaxLimit() {
    std::cout << "\n=== LPStore: MaxLimit ===" << std::endl;
    ecs::World world;
    systems::LoyaltyPointStoreSystem sys(&world);
    world.createEntity("lp1");
    sys.initialize("lp1", "store_caldari", "caldari_navy");
    auto* entity = world.getEntity("lp1");
    auto* store = entity->getComponent<components::LoyaltyPointStore>();
    store->max_items = 2;
    store->max_players = 2;
    sys.addItem("lp1", "i1", "Item1", "Ship", 100, 100.0f, 1);
    sys.addItem("lp1", "i2", "Item2", "Module", 100, 100.0f, 1);
    assertTrue(!sys.addItem("lp1", "i3", "Item3", "Ship", 100, 100.0f, 1), "Max items enforced");
    sys.registerPlayer("lp1", "p1");
    sys.registerPlayer("lp1", "p2");
    assertTrue(!sys.registerPlayer("lp1", "p3"), "Max players enforced");
}

void testLPStoreMissing() {
    std::cout << "\n=== LPStore: Missing ===" << std::endl;
    ecs::World world;
    systems::LoyaltyPointStoreSystem sys(&world);
    assertTrue(!sys.initialize("nonexistent", "s1", "f1"), "Init fails on missing");
    assertTrue(!sys.addItem("nonexistent", "i1", "N", "C", 1, 1.0f, 1), "AddItem fails on missing");
    assertTrue(!sys.removeItem("nonexistent", "i1"), "RemoveItem fails on missing");
    assertTrue(!sys.registerPlayer("nonexistent", "p1"), "RegisterPlayer fails on missing");
    assertTrue(!sys.earnLP("nonexistent", "p1", 100), "EarnLP fails on missing");
    assertTrue(!sys.purchaseItem("nonexistent", "p1", "i1"), "Purchase fails on missing");
    assertTrue(sys.getBalance("nonexistent", "p1") == 0, "0 balance on missing");
    assertTrue(sys.getItemCount("nonexistent") == 0, "0 items on missing");
    assertTrue(sys.getPlayerCount("nonexistent") == 0, "0 players on missing");
    assertTrue(sys.getTotalPurchases("nonexistent") == 0, "0 purchases on missing");
    assertTrue(approxEqual(sys.getTotalISKCollected("nonexistent"), 0.0f), "0 ISK on missing");
    assertTrue(sys.getItemsByCategory("nonexistent", "Ship") == 0, "0 category on missing");
}


void run_economy_tests() {
    testMarketPlaceSellOrder();
    testMarketBuyFromMarket();
    testMarketPriceQueries();
    testMarketOrderExpiry();
    testContractCreate();
    testContractAccept();
    testContractComplete();
    testContractExpiry();
    testContractStatusQuery();
    testSerializeDeserializeContractBoard();
    testPIInstallExtractor();
    testPIInstallProcessor();
    testPIExtractionCycle();
    testPIProcessingCycle();
    testPICpuPowergridLimit();
    testPIStorageCapacityLimit();
    testManufacturingStartJob();
    testManufacturingJobCompletion();
    testManufacturingMultipleRuns();
    testManufacturingJobSlotLimit();
    testManufacturingCancelJob();
    testManufacturingInsufficientFunds();
    testResearchME();
    testResearchTE();
    testResearchInvention();
    testResearchInventionFailure();
    testResearchJobSlotLimit();
    testResearchInsufficientFunds();
    testMiningCreateDeposit();
    testMiningStartStop();
    testMiningRangeCheck();
    testMiningCycleCompletion();
    testMiningDepletedDeposit();
    testMiningCargoFull();
    testMiningOreStacking();
    testMiningDroneLaunchAndMine();
    testSalvageDroneLaunchAndSalvage();
    testSalvageDroneAlreadySalvaged();
    testMiningDroneTargetDepletedDeposit();
    testAIFindMostProfitableDepositWithPrices();
    testAIFindMostProfitableDepositNoMarket();
    testAIFindMostProfitableDepositPrefersCloseWhenEqual();
    testAIHaulingState();
    testAIHaulingMovesToStation();
    testAIHaulingSellsCargoAtStation();
    testAIHaulingNoStationGoesIdle();
    testAIMiningCargoFullTransitionsToHauling();
    testAIMiningCargoFullNoStationGoesIdle();
    testAIFullMineHaulSellCycle();
    testRefineOreBasic();
    testRefineOreWithEfficiency();
    testRefineOreWithTax();
    testRefineOreInsufficientOre();
    testRefineOreNoRecipe();
    testRefineOreMultipleOutputs();
    testRefineDefaultRecipes();
    testMarketOrePricing();
    testMarketMineralPricing();
    testNPCMarketSeedCreatesOrders();
    testNPCMarketSeedPricesCorrect();
    testNPCMarketSeedOrdersPermanent();
    testNPCMarketSeedBuyableByPlayer();
    testMarketOrderComponentDefaults();
    testMarketPlaceOrderSystem();
    testMarketFillOrderSystem();
    testMarketOrderExpirySystem();
    testAIFleetDispatchSystem();
    testBlackMarketDefaults();
    testBlackMarketAddListing();
    testBlackMarketPurchase();
    testBlackMarketPurchaseRemovesEmpty();
    testBlackMarketExpiry();
    testBlackMarketMaxListings();
    testBlackMarketDetectionChance();
    testPriceHistoryDefaults();
    testPriceHistoryRecording();
    testPriceHistoryAverage();
    testPriceHistoryTrend();
    testPriceHistoryVolume();
    testPriceHistoryIntervalConfig();
    testEconomicFlowDefaults();
    testEconomicFlowRecordProduction();
    testEconomicFlowRecordConsumption();
    testEconomicFlowTransport();
    testEconomicFlowDestruction();
    testEconomicFlowNetFlow();
    testEconomicFlowHealthUpdate();
    testEconomicFlowMissingEntity();
    testEconomicFlowAutoCreateComponent();
    testProductionChainCreate();
    testProductionChainAddStage();
    testProductionChainRemoveStage();
    testProductionChainEfficiency();
    testProductionChainThroughput();
    testProductionChainBottleneck();
    testProductionChainActiveToggle();
    testProductionChainUptime();
    testProductionChainMissing();
    testProductionChainComponentDefaults();
    testLPStoreCreate();
    testLPStoreAddItem();
    testLPStoreDuplicate();
    testLPStoreEarnLP();
    testLPStorePurchase();
    testLPStorePurchaseFail();
    testLPStoreCategory();
    testLPStoreMultiPlayer();
    testLPStoreMaxLimit();
    testLPStoreMissing();
}
