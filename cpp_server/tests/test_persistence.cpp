/**
 * Persistence tests
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

// ==================== WorldPersistence Tests ====================

void testSerializeDeserializeBasicEntity() {
    std::cout << "\n=== Serialize/Deserialize Basic Entity ===" << std::endl;

    ecs::World world;
    auto* entity = world.createEntity("ship_1");

    auto pos = std::make_unique<components::Position>();
    pos->x = 100.0f; pos->y = 200.0f; pos->z = 300.0f; pos->rotation = 1.5f;
    entity->addComponent(std::move(pos));

    auto vel = std::make_unique<components::Velocity>();
    vel->vx = 10.0f; vel->vy = 20.0f; vel->vz = 30.0f; vel->max_speed = 500.0f;
    entity->addComponent(std::move(vel));

    data::WorldPersistence persistence;
    std::string json = persistence.serializeWorld(&world);

    assertTrue(!json.empty(), "Serialized JSON is not empty");
    assertTrue(json.find("ship_1") != std::string::npos, "JSON contains entity id");

    // Deserialize into a new world
    ecs::World world2;
    bool ok = persistence.deserializeWorld(&world2, json);
    assertTrue(ok, "Deserialize succeeds");
    assertTrue(world2.getEntityCount() == 1, "Loaded world has 1 entity");

    auto* loaded = world2.getEntity("ship_1");
    assertTrue(loaded != nullptr, "Loaded entity found by id");

    auto* lpos = loaded->getComponent<components::Position>();
    assertTrue(lpos != nullptr, "Loaded entity has Position");
    assertTrue(approxEqual(lpos->x, 100.0f), "Position.x preserved");
    assertTrue(approxEqual(lpos->y, 200.0f), "Position.y preserved");
    assertTrue(approxEqual(lpos->z, 300.0f), "Position.z preserved");
    assertTrue(approxEqual(lpos->rotation, 1.5f), "Position.rotation preserved");

    auto* lvel = loaded->getComponent<components::Velocity>();
    assertTrue(lvel != nullptr, "Loaded entity has Velocity");
    assertTrue(approxEqual(lvel->vx, 10.0f), "Velocity.vx preserved");
    assertTrue(approxEqual(lvel->max_speed, 500.0f), "Velocity.max_speed preserved");
}

void testSerializeDeserializeHealthCapacitor() {
    std::cout << "\n=== Serialize/Deserialize Health & Capacitor ===" << std::endl;

    ecs::World world;
    auto* entity = world.createEntity("tanker");

    auto hp = std::make_unique<components::Health>();
    hp->shield_hp = 450.0f; hp->shield_max = 500.0f;
    hp->armor_hp = 300.0f; hp->armor_max = 400.0f;
    hp->hull_hp = 200.0f; hp->hull_max = 250.0f;
    hp->shield_recharge_rate = 5.0f;
    hp->shield_em_resist = 0.1f;
    hp->armor_thermal_resist = 0.35f;
    entity->addComponent(std::move(hp));

    auto cap = std::make_unique<components::Capacitor>();
    cap->capacitor = 180.0f; cap->capacitor_max = 250.0f; cap->recharge_rate = 4.0f;
    entity->addComponent(std::move(cap));

    data::WorldPersistence persistence;
    std::string json = persistence.serializeWorld(&world);

    ecs::World world2;
    persistence.deserializeWorld(&world2, json);

    auto* loaded = world2.getEntity("tanker");
    assertTrue(loaded != nullptr, "Entity loaded");

    auto* lhp = loaded->getComponent<components::Health>();
    assertTrue(lhp != nullptr, "Health component loaded");
    assertTrue(approxEqual(lhp->shield_hp, 450.0f), "Shield HP preserved");
    assertTrue(approxEqual(lhp->shield_max, 500.0f), "Shield max preserved");
    assertTrue(approxEqual(lhp->armor_hp, 300.0f), "Armor HP preserved");
    assertTrue(approxEqual(lhp->hull_hp, 200.0f), "Hull HP preserved");
    assertTrue(approxEqual(lhp->shield_recharge_rate, 5.0f), "Shield recharge rate preserved");
    assertTrue(approxEqual(lhp->shield_em_resist, 0.1f), "Shield EM resist preserved");
    assertTrue(approxEqual(lhp->armor_thermal_resist, 0.35f), "Armor thermal resist preserved");

    auto* lcap = loaded->getComponent<components::Capacitor>();
    assertTrue(lcap != nullptr, "Capacitor component loaded");
    assertTrue(approxEqual(lcap->capacitor, 180.0f), "Capacitor current preserved");
    assertTrue(approxEqual(lcap->capacitor_max, 250.0f), "Capacitor max preserved");
    assertTrue(approxEqual(lcap->recharge_rate, 4.0f), "Capacitor recharge rate preserved");
}

void testSerializeDeserializeShipAndFaction() {
    std::cout << "\n=== Serialize/Deserialize Ship & Faction ===" << std::endl;

    ecs::World world;
    auto* entity = world.createEntity("player_ship");

    auto ship = std::make_unique<components::Ship>();
    ship->ship_type = "Cruiser";
    ship->ship_class = "Cruiser";
    ship->ship_name = "Caracal";
    ship->race = "Veyren";
    ship->cpu_max = 350.0f;
    ship->powergrid_max = 200.0f;
    ship->signature_radius = 140.0f;
    ship->scan_resolution = 250.0f;
    ship->max_locked_targets = 6;
    ship->max_targeting_range = 55000.0f;
    entity->addComponent(std::move(ship));

    auto fac = std::make_unique<components::Faction>();
    fac->faction_name = "Veyren";
    entity->addComponent(std::move(fac));

    data::WorldPersistence persistence;
    std::string json = persistence.serializeWorld(&world);

    ecs::World world2;
    persistence.deserializeWorld(&world2, json);

    auto* loaded = world2.getEntity("player_ship");
    assertTrue(loaded != nullptr, "Entity loaded");

    auto* lship = loaded->getComponent<components::Ship>();
    assertTrue(lship != nullptr, "Ship component loaded");
    assertTrue(lship->ship_name == "Caracal", "Ship name preserved");
    assertTrue(lship->race == "Veyren", "Ship race preserved");
    assertTrue(lship->ship_class == "Cruiser", "Ship class preserved");
    assertTrue(approxEqual(lship->cpu_max, 350.0f), "CPU max preserved");
    assertTrue(lship->max_locked_targets == 6, "Max locked targets preserved");
    assertTrue(approxEqual(lship->max_targeting_range, 55000.0f), "Max targeting range preserved");

    auto* lfac = loaded->getComponent<components::Faction>();
    assertTrue(lfac != nullptr, "Faction component loaded");
    assertTrue(lfac->faction_name == "Veyren", "Faction name preserved");
}

void testSerializeDeserializeStandings() {
    std::cout << "\n=== Serialize/Deserialize Standings ===" << std::endl;

    ecs::World world;
    auto* entity = world.createEntity("player_1");

    // Add Standings component with test data
    auto standings = std::make_unique<components::Standings>();
    standings->personal_standings["npc_pirate_001"] = -5.0f;
    standings->personal_standings["player_friend"] = 8.5f;
    standings->corporation_standings["Republic Fleet"] = 3.0f;
    standings->corporation_standings["Venom Syndicate"] = -7.5f;
    standings->faction_standings["Keldari"] = 2.5f;
    standings->faction_standings["Solari"] = -1.5f;
    entity->addComponent(std::move(standings));

    data::WorldPersistence persistence;
    std::string json = persistence.serializeWorld(&world);

    ecs::World world2;
    persistence.deserializeWorld(&world2, json);

    auto* loaded = world2.getEntity("player_1");
    assertTrue(loaded != nullptr, "Entity loaded");

    auto* lstandings = loaded->getComponent<components::Standings>();
    assertTrue(lstandings != nullptr, "Standings component loaded");
    
    // Check personal standings
    assertTrue(lstandings->personal_standings.size() == 2, "Personal standings count preserved");
    assertTrue(approxEqual(lstandings->personal_standings["npc_pirate_001"], -5.0f), "Personal standing (pirate) preserved");
    assertTrue(approxEqual(lstandings->personal_standings["player_friend"], 8.5f), "Personal standing (friend) preserved");
    
    // Check corporation standings
    assertTrue(lstandings->corporation_standings.size() == 2, "Corporation standings count preserved");
    assertTrue(approxEqual(lstandings->corporation_standings["Republic Fleet"], 3.0f), "Corporation standing (Republic Fleet) preserved");
    assertTrue(approxEqual(lstandings->corporation_standings["Venom Syndicate"], -7.5f), "Corporation standing (Venom Syndicate) preserved");
    
    // Check faction standings
    assertTrue(lstandings->faction_standings.size() == 2, "Faction standings count preserved");
    assertTrue(approxEqual(lstandings->faction_standings["Keldari"], 2.5f), "Faction standing (Keldari) preserved");
    assertTrue(approxEqual(lstandings->faction_standings["Solari"], -1.5f), "Faction standing (Solari) preserved");
}

void testStandingsGetStanding() {
    std::cout << "\n=== Standings getStandingWith ===" << std::endl;

    ecs::World world;
    auto* entity = world.createEntity("player_1");

    auto standings = std::make_unique<components::Standings>();
    standings->personal_standings["npc_001"] = -5.0f;
    standings->corporation_standings["TestCorp"] = 3.0f;
    standings->faction_standings["Veyren"] = 7.0f;
    entity->addComponent(std::move(standings));

    auto* comp = entity->getComponent<components::Standings>();
    
    // Personal standing has highest priority
    float standing1 = comp->getStandingWith("npc_001", "", "");
    assertTrue(approxEqual(standing1, -5.0f), "Personal standing returned");
    
    // Corporation standing used when no personal standing
    float standing2 = comp->getStandingWith("npc_002", "TestCorp", "");
    assertTrue(approxEqual(standing2, 3.0f), "Corporation standing returned");
    
    // Faction standing used when no personal or corp standing
    float standing3 = comp->getStandingWith("npc_003", "OtherCorp", "Veyren");
    assertTrue(approxEqual(standing3, 7.0f), "Faction standing returned");
    
    // Neutral (0) when no standing exists
    float standing4 = comp->getStandingWith("unknown", "UnknownCorp", "UnknownFaction");
    assertTrue(approxEqual(standing4, 0.0f), "Neutral standing for unknown entity");
    
    // Personal standing overrides corporation
    comp->personal_standings["npc_004"] = 9.0f;
    float standing5 = comp->getStandingWith("npc_004", "TestCorp", "");
    assertTrue(approxEqual(standing5, 9.0f), "Personal standing overrides corporation");
}

void testStandingsModify() {
    std::cout << "\n=== Standings modifyStanding ===" << std::endl;

    std::map<std::string, float> test_standings;
    
    // Start with no standing (implicit 0)
    components::Standings::modifyStanding(test_standings, "entity1", 2.5f);
    assertTrue(approxEqual(test_standings["entity1"], 2.5f), "Standing increased from 0 to 2.5");
    
    // Increase existing standing
    components::Standings::modifyStanding(test_standings, "entity1", 3.0f);
    assertTrue(approxEqual(test_standings["entity1"], 5.5f), "Standing increased to 5.5");
    
    // Decrease standing
    components::Standings::modifyStanding(test_standings, "entity1", -2.0f);
    assertTrue(approxEqual(test_standings["entity1"], 3.5f), "Standing decreased to 3.5");
    
    // Clamp at maximum (10.0)
    components::Standings::modifyStanding(test_standings, "entity1", 15.0f);
    assertTrue(approxEqual(test_standings["entity1"], 10.0f), "Standing clamped at max (10.0)");
    
    // Clamp at minimum (-10.0)
    components::Standings::modifyStanding(test_standings, "entity2", -20.0f);
    assertTrue(approxEqual(test_standings["entity2"], -10.0f), "Standing clamped at min (-10.0)");
    
    // Negative adjustment from positive
    test_standings["entity3"] = 5.0f;
    components::Standings::modifyStanding(test_standings, "entity3", -8.0f);
    assertTrue(approxEqual(test_standings["entity3"], -3.0f), "Standing went from +5 to -3");
}

void testSerializeDeserializeAIAndWeapon() {
    std::cout << "\n=== Serialize/Deserialize AI & Weapon ===" << std::endl;

    ecs::World world;
    auto* entity = world.createEntity("npc_1");

    auto ai = std::make_unique<components::AI>();
    ai->behavior = components::AI::Behavior::Aggressive;
    ai->state = components::AI::State::Attacking;
    ai->target_entity_id = "player_1";
    ai->orbit_distance = 2500.0f;
    ai->awareness_range = 60000.0f;
    entity->addComponent(std::move(ai));

    auto weapon = std::make_unique<components::Weapon>();
    weapon->weapon_type = "Missile";
    weapon->damage_type = "kinetic";
    weapon->damage = 75.0f;
    weapon->optimal_range = 20000.0f;
    weapon->rate_of_fire = 8.0f;
    weapon->capacitor_cost = 15.0f;
    weapon->ammo_type = "Scourge";
    weapon->ammo_count = 50;
    entity->addComponent(std::move(weapon));

    data::WorldPersistence persistence;
    std::string json = persistence.serializeWorld(&world);

    ecs::World world2;
    persistence.deserializeWorld(&world2, json);

    auto* loaded = world2.getEntity("npc_1");
    assertTrue(loaded != nullptr, "NPC entity loaded");

    auto* lai = loaded->getComponent<components::AI>();
    assertTrue(lai != nullptr, "AI component loaded");
    assertTrue(lai->behavior == components::AI::Behavior::Aggressive, "AI behavior preserved");
    assertTrue(lai->state == components::AI::State::Attacking, "AI state preserved");
    assertTrue(lai->target_entity_id == "player_1", "AI target preserved");
    assertTrue(approxEqual(lai->orbit_distance, 2500.0f), "AI orbit distance preserved");

    auto* lwep = loaded->getComponent<components::Weapon>();
    assertTrue(lwep != nullptr, "Weapon component loaded");
    assertTrue(lwep->weapon_type == "Missile", "Weapon type preserved");
    assertTrue(lwep->damage_type == "kinetic", "Damage type preserved");
    assertTrue(approxEqual(lwep->damage, 75.0f), "Weapon damage preserved");
    assertTrue(lwep->ammo_type == "Scourge", "Ammo type preserved");
    assertTrue(lwep->ammo_count == 50, "Ammo count preserved");
}

void testSerializeDeserializePlayerComponent() {
    std::cout << "\n=== Serialize/Deserialize Player Component ===" << std::endl;

    ecs::World world;
    auto* entity = world.createEntity("player_42");

    auto player = std::make_unique<components::Player>();
    player->player_id = "steam_12345";
    player->character_name = "TestPilot";
    player->credits = 5000000.0;
    player->corporation = "Test Corp";
    entity->addComponent(std::move(player));

    data::WorldPersistence persistence;
    std::string json = persistence.serializeWorld(&world);

    ecs::World world2;
    persistence.deserializeWorld(&world2, json);

    auto* loaded = world2.getEntity("player_42");
    assertTrue(loaded != nullptr, "Player entity loaded");

    auto* lp = loaded->getComponent<components::Player>();
    assertTrue(lp != nullptr, "Player component loaded");
    assertTrue(lp->player_id == "steam_12345", "Player ID preserved");
    assertTrue(lp->character_name == "TestPilot", "Character name preserved");
    assertTrue(lp->credits > 4999999.0 && lp->credits < 5000001.0, "Credits preserved");
    assertTrue(lp->corporation == "Test Corp", "Corporation preserved");
}

void testSerializeDeserializeMultipleEntities() {
    std::cout << "\n=== Serialize/Deserialize Multiple Entities ===" << std::endl;

    ecs::World world;

    // Create 3 entities with different component combinations
    auto* e1 = world.createEntity("ship_a");
    auto p1 = std::make_unique<components::Position>();
    p1->x = 10.0f;
    e1->addComponent(std::move(p1));

    auto* e2 = world.createEntity("ship_b");
    auto p2 = std::make_unique<components::Position>();
    p2->x = 20.0f;
    e2->addComponent(std::move(p2));
    auto h2 = std::make_unique<components::Health>();
    h2->shield_hp = 999.0f;
    e2->addComponent(std::move(h2));

    auto* e3 = world.createEntity("ship_c");
    auto p3 = std::make_unique<components::Position>();
    p3->x = 30.0f;
    e3->addComponent(std::move(p3));

    data::WorldPersistence persistence;
    std::string json = persistence.serializeWorld(&world);

    ecs::World world2;
    persistence.deserializeWorld(&world2, json);

    assertTrue(world2.getEntityCount() == 3, "All 3 entities loaded");
    assertTrue(world2.getEntity("ship_a") != nullptr, "ship_a loaded");
    assertTrue(world2.getEntity("ship_b") != nullptr, "ship_b loaded");
    assertTrue(world2.getEntity("ship_c") != nullptr, "ship_c loaded");

    auto* lb = world2.getEntity("ship_b");
    auto* lhp = lb->getComponent<components::Health>();
    assertTrue(lhp != nullptr, "ship_b has Health component");
    assertTrue(approxEqual(lhp->shield_hp, 999.0f), "ship_b shield HP preserved");
}

void testSaveLoadFile() {
    std::cout << "\n=== Save/Load World File ===" << std::endl;

    ecs::World world;
    auto* entity = world.createEntity("file_test");
    auto pos = std::make_unique<components::Position>();
    pos->x = 42.0f; pos->y = 84.0f;
    entity->addComponent(std::move(pos));

    data::WorldPersistence persistence;
    std::string filepath = "/tmp/eve_test_world.json";

    bool saved = persistence.saveWorld(&world, filepath);
    assertTrue(saved, "World saved to file");

    // Verify file exists
    std::ifstream check(filepath);
    assertTrue(check.good(), "Save file exists on disk");
    check.close();

    ecs::World world2;
    bool loaded = persistence.loadWorld(&world2, filepath);
    assertTrue(loaded, "World loaded from file");
    assertTrue(world2.getEntityCount() == 1, "Loaded world has 1 entity");

    auto* le = world2.getEntity("file_test");
    assertTrue(le != nullptr, "Entity loaded from file");
    auto* lpos = le->getComponent<components::Position>();
    assertTrue(lpos != nullptr, "Position loaded from file");
    assertTrue(approxEqual(lpos->x, 42.0f), "Position.x loaded from file");
    assertTrue(approxEqual(lpos->y, 84.0f), "Position.y loaded from file");

    // Clean up
    std::remove(filepath.c_str());
}

void testLoadNonexistentFile() {
    std::cout << "\n=== Load Nonexistent File ===" << std::endl;

    ecs::World world;
    data::WorldPersistence persistence;
    bool loaded = persistence.loadWorld(&world, "/tmp/does_not_exist_12345.json");
    assertTrue(!loaded, "Loading nonexistent file returns false");
    assertTrue(world.getEntityCount() == 0, "World unchanged on failed load");
}

void testSerializeDeserializeWormholeAndSolarSystem() {
    std::cout << "\n=== Serialize/Deserialize Wormhole & SolarSystem ===" << std::endl;

    ecs::World world;
    auto* entity = world.createEntity("wh_j123456");

    auto ss = std::make_unique<components::SolarSystem>();
    ss->system_id = "j123456";
    ss->system_name = "J123456";
    ss->wormhole_class = 3;
    ss->effect_name = "magnetar";
    ss->dormants_spawned = true;
    entity->addComponent(std::move(ss));

    auto* wh_entity = world.createEntity("wh_conn_1");
    auto wh = std::make_unique<components::WormholeConnection>();
    wh->wormhole_id = "wh_001";
    wh->source_system = "j123456";
    wh->destination_system = "jita";
    wh->max_mass = 1000000000.0;
    wh->remaining_mass = 750000000.0;
    wh->max_jump_mass = 300000000.0;
    wh->max_lifetime_hours = 16.0f;
    wh->elapsed_hours = 4.5f;
    wh->collapsed = false;
    wh_entity->addComponent(std::move(wh));

    data::WorldPersistence persistence;
    std::string json = persistence.serializeWorld(&world);

    ecs::World world2;
    persistence.deserializeWorld(&world2, json);

    auto* lss_entity = world2.getEntity("wh_j123456");
    assertTrue(lss_entity != nullptr, "SolarSystem entity loaded");
    auto* lss = lss_entity->getComponent<components::SolarSystem>();
    assertTrue(lss != nullptr, "SolarSystem component loaded");
    assertTrue(lss->system_id == "j123456", "System ID preserved");
    assertTrue(lss->wormhole_class == 3, "Wormhole class preserved");
    assertTrue(lss->effect_name == "magnetar", "Effect name preserved");
    assertTrue(lss->dormants_spawned == true, "Dormants spawned preserved");

    auto* lwh_entity = world2.getEntity("wh_conn_1");
    assertTrue(lwh_entity != nullptr, "WormholeConnection entity loaded");
    auto* lwh = lwh_entity->getComponent<components::WormholeConnection>();
    assertTrue(lwh != nullptr, "WormholeConnection component loaded");
    assertTrue(lwh->wormhole_id == "wh_001", "Wormhole ID preserved");
    assertTrue(lwh->remaining_mass > 749999999.0 && lwh->remaining_mass < 750000001.0,
               "Remaining mass preserved");
    assertTrue(approxEqual(lwh->elapsed_hours, 4.5f), "Elapsed hours preserved");
    assertTrue(lwh->collapsed == false, "Collapsed state preserved");
}

void testEmptyWorldSerialize() {
    std::cout << "\n=== Empty World Serialize ===" << std::endl;

    ecs::World world;
    data::WorldPersistence persistence;
    std::string json = persistence.serializeWorld(&world);

    assertTrue(!json.empty(), "Empty world produces valid JSON");
    assertTrue(json.find("entities") != std::string::npos, "JSON has entities key");

    ecs::World world2;
    bool ok = persistence.deserializeWorld(&world2, json);
    assertTrue(ok, "Deserialize empty world succeeds");
    assertTrue(world2.getEntityCount() == 0, "Empty world has 0 entities");
}

// ==================== Phase 5 Component Persistence Tests ====================

void testSerializeDeserializeStation() {
    std::cout << "\n=== Serialize/Deserialize Station/Docked/Wreck ===" << std::endl;

    ecs::World world;

    auto* e1 = world.createEntity("station_entity");
    auto* station = addComp<components::Station>(e1);
    station->station_name = "Test Station";
    station->docking_range = 3000.0f;
    station->repair_cost_per_hp = 2.5f;
    station->docked_count = 3;

    auto* e2 = world.createEntity("docked_entity");
    auto* docked = addComp<components::Docked>(e2);
    docked->station_id = "station_42";

    auto* e3 = world.createEntity("wreck_entity");
    auto* wreck = addComp<components::Wreck>(e3);
    wreck->source_entity_id = "npc_frigate_7";
    wreck->lifetime_remaining = 1200.0f;
    wreck->salvaged = true;

    data::WorldPersistence persistence;
    std::string json = persistence.serializeWorld(&world);

    ecs::World world2;
    assertTrue(persistence.deserializeWorld(&world2, json),
               "Station/Docked/Wreck deserialization succeeds");

    auto* s2 = world2.getEntity("station_entity");
    assertTrue(s2 != nullptr, "Station entity recreated");
    auto* station2 = s2->getComponent<components::Station>();
    assertTrue(station2 != nullptr, "Station component recreated");
    assertTrue(station2->station_name == "Test Station", "station_name preserved");
    assertTrue(approxEqual(station2->docking_range, 3000.0f), "docking_range preserved");
    assertTrue(approxEqual(station2->repair_cost_per_hp, 2.5f), "repair_cost_per_hp preserved");
    assertTrue(station2->docked_count == 3, "docked_count preserved");

    auto* d2 = world2.getEntity("docked_entity");
    assertTrue(d2 != nullptr, "Docked entity recreated");
    auto* docked2 = d2->getComponent<components::Docked>();
    assertTrue(docked2 != nullptr, "Docked component recreated");
    assertTrue(docked2->station_id == "station_42", "station_id preserved");

    auto* w2 = world2.getEntity("wreck_entity");
    assertTrue(w2 != nullptr, "Wreck entity recreated");
    auto* wreck2 = w2->getComponent<components::Wreck>();
    assertTrue(wreck2 != nullptr, "Wreck component recreated");
    assertTrue(wreck2->source_entity_id == "npc_frigate_7", "source_entity_id preserved");
    assertTrue(approxEqual(wreck2->lifetime_remaining, 1200.0f), "lifetime_remaining preserved");
    assertTrue(wreck2->salvaged == true, "salvaged preserved");
}

void testSerializeDeserializeCaptainPersonality() {
    std::cout << "\n=== Serialize/Deserialize CaptainPersonality ===" << std::endl;

    ecs::World world;
    auto* entity = world.createEntity("captain_test");
    auto* cp = addComp<components::CaptainPersonality>(entity);
    cp->aggression = 0.8f;
    cp->sociability = 0.3f;
    cp->optimism = 0.9f;
    cp->professionalism = 0.1f;
    cp->loyalty = 0.7f;
    cp->paranoia = 0.6f;
    cp->ambition = 0.4f;
    cp->adaptability = 0.2f;
    cp->captain_name = "Cap. Zara";
    cp->faction = "Solari";

    data::WorldPersistence persistence;
    std::string json = persistence.serializeWorld(&world);

    ecs::World world2;
    assertTrue(persistence.deserializeWorld(&world2, json),
               "CaptainPersonality deserialization succeeds");

    auto* e2 = world2.getEntity("captain_test");
    assertTrue(e2 != nullptr, "Captain entity recreated");
    auto* cp2 = e2->getComponent<components::CaptainPersonality>();
    assertTrue(cp2 != nullptr, "CaptainPersonality component recreated");
    assertTrue(approxEqual(cp2->aggression, 0.8f), "aggression preserved");
    assertTrue(approxEqual(cp2->sociability, 0.3f), "sociability preserved");
    assertTrue(approxEqual(cp2->optimism, 0.9f), "optimism preserved");
    assertTrue(approxEqual(cp2->professionalism, 0.1f), "professionalism preserved");
    assertTrue(approxEqual(cp2->loyalty, 0.7f), "loyalty preserved");
    assertTrue(approxEqual(cp2->paranoia, 0.6f), "paranoia preserved");
    assertTrue(approxEqual(cp2->ambition, 0.4f), "ambition preserved");
    assertTrue(approxEqual(cp2->adaptability, 0.2f), "adaptability preserved");
    assertTrue(cp2->captain_name == "Cap. Zara", "captain_name preserved");
    assertTrue(cp2->faction == "Solari", "faction preserved");
}

void testSerializeDeserializeFleetState() {
    std::cout << "\n=== Serialize/Deserialize FleetMorale/CaptainRelationship/EmotionalState ===" << std::endl;

    ecs::World world;
    auto* entity = world.createEntity("fleet_state_test");

    auto* morale = addComp<components::FleetMorale>(entity);
    morale->morale_score = 35.5f;
    morale->wins = 10;
    morale->losses = 3;
    morale->ships_lost = 1;
    morale->times_saved_by_player = 2;
    morale->times_player_saved = 1;
    morale->missions_together = 15;
    morale->morale_state = "Inspired";

    auto* rel = addComp<components::CaptainRelationship>(entity);
    components::CaptainRelationship::Relationship r1;
    r1.other_captain_id = "captain_alpha";
    r1.affinity = 75.0f;
    rel->relationships.push_back(r1);
    components::CaptainRelationship::Relationship r2;
    r2.other_captain_id = "captain_beta";
    r2.affinity = -30.0f;
    rel->relationships.push_back(r2);

    auto* emo = addComp<components::EmotionalState>(entity);
    emo->confidence = 80.0f;
    emo->trust_in_player = 65.0f;
    emo->fatigue = 20.0f;
    emo->hope = 70.0f;

    data::WorldPersistence persistence;
    std::string json = persistence.serializeWorld(&world);

    ecs::World world2;
    assertTrue(persistence.deserializeWorld(&world2, json),
               "FleetState deserialization succeeds");

    auto* e2 = world2.getEntity("fleet_state_test");
    assertTrue(e2 != nullptr, "Fleet state entity recreated");

    auto* morale2 = e2->getComponent<components::FleetMorale>();
    assertTrue(morale2 != nullptr, "FleetMorale component recreated");
    assertTrue(approxEqual(morale2->morale_score, 35.5f), "morale_score preserved");
    assertTrue(morale2->wins == 10, "wins preserved");
    assertTrue(morale2->losses == 3, "losses preserved");
    assertTrue(morale2->ships_lost == 1, "ships_lost preserved");
    assertTrue(morale2->times_saved_by_player == 2, "times_saved_by_player preserved");
    assertTrue(morale2->times_player_saved == 1, "times_player_saved preserved");
    assertTrue(morale2->missions_together == 15, "missions_together preserved");
    assertTrue(morale2->morale_state == "Inspired", "morale_state preserved");

    auto* rel2 = e2->getComponent<components::CaptainRelationship>();
    assertTrue(rel2 != nullptr, "CaptainRelationship component recreated");
    assertTrue(rel2->relationships.size() == 2, "relationship count preserved");
    assertTrue(rel2->relationships[0].other_captain_id == "captain_alpha", "relationship[0] id preserved");
    assertTrue(approxEqual(rel2->relationships[0].affinity, 75.0f), "relationship[0] affinity preserved");
    assertTrue(rel2->relationships[1].other_captain_id == "captain_beta", "relationship[1] id preserved");
    assertTrue(approxEqual(rel2->relationships[1].affinity, -30.0f), "relationship[1] affinity preserved");

    auto* emo2 = e2->getComponent<components::EmotionalState>();
    assertTrue(emo2 != nullptr, "EmotionalState component recreated");
    assertTrue(approxEqual(emo2->confidence, 80.0f), "confidence preserved");
    assertTrue(approxEqual(emo2->trust_in_player, 65.0f), "trust_in_player preserved");
    assertTrue(approxEqual(emo2->fatigue, 20.0f), "fatigue preserved");
    assertTrue(approxEqual(emo2->hope, 70.0f), "hope preserved");
}

void testSerializeDeserializeCaptainMemory() {
    std::cout << "\n=== Serialize/Deserialize CaptainMemory ===" << std::endl;

    ecs::World world;
    auto* entity = world.createEntity("memory_test");
    auto* mem = addComp<components::CaptainMemory>(entity);
    mem->max_memories = 25;

    components::CaptainMemory::MemoryEntry m1;
    m1.event_type = "combat_win";
    m1.context = "Defeated pirate frigate";
    m1.timestamp = 1000.0f;
    m1.emotional_weight = 0.8f;
    mem->memories.push_back(m1);

    components::CaptainMemory::MemoryEntry m2;
    m2.event_type = "ship_lost";
    m2.context = "Wingman destroyed";
    m2.timestamp = 2000.0f;
    m2.emotional_weight = -0.9f;
    mem->memories.push_back(m2);

    data::WorldPersistence persistence;
    std::string json = persistence.serializeWorld(&world);

    ecs::World world2;
    assertTrue(persistence.deserializeWorld(&world2, json),
               "CaptainMemory deserialization succeeds");

    auto* e2 = world2.getEntity("memory_test");
    assertTrue(e2 != nullptr, "Memory entity recreated");
    auto* mem2 = e2->getComponent<components::CaptainMemory>();
    assertTrue(mem2 != nullptr, "CaptainMemory component recreated");
    assertTrue(mem2->max_memories == 25, "max_memories preserved");
    assertTrue(mem2->memories.size() == 2, "memory count preserved");
    assertTrue(mem2->memories[0].event_type == "combat_win", "memory[0] event_type preserved");
    assertTrue(mem2->memories[0].context == "Defeated pirate frigate", "memory[0] context preserved");
    assertTrue(approxEqual(mem2->memories[0].timestamp, 1000.0f), "memory[0] timestamp preserved");
    assertTrue(approxEqual(mem2->memories[0].emotional_weight, 0.8f), "memory[0] emotional_weight preserved");
    assertTrue(mem2->memories[1].event_type == "ship_lost", "memory[1] event_type preserved");
    assertTrue(mem2->memories[1].context == "Wingman destroyed", "memory[1] context preserved");
    assertTrue(approxEqual(mem2->memories[1].timestamp, 2000.0f), "memory[1] timestamp preserved");
    assertTrue(approxEqual(mem2->memories[1].emotional_weight, -0.9f), "memory[1] emotional_weight preserved");
}

void testSerializeDeserializeFleetFormation() {
    std::cout << "\n=== Serialize/Deserialize FleetFormation ===" << std::endl;

    ecs::World world;
    auto* entity = world.createEntity("formation_test");
    auto* ff = addComp<components::FleetFormation>(entity);
    ff->formation = components::FleetFormation::FormationType::Wedge;
    ff->slot_index = 2;
    ff->offset_x = 150.0f;
    ff->offset_y = -50.0f;
    ff->offset_z = 0.0f;

    data::WorldPersistence persistence;
    std::string json = persistence.serializeWorld(&world);

    ecs::World world2;
    assertTrue(persistence.deserializeWorld(&world2, json),
               "FleetFormation deserialization succeeds");

    auto* e2 = world2.getEntity("formation_test");
    assertTrue(e2 != nullptr, "Formation entity recreated");
    auto* ff2 = e2->getComponent<components::FleetFormation>();
    assertTrue(ff2 != nullptr, "FleetFormation component recreated");
    assertTrue(static_cast<int>(ff2->formation) == 3, "formation type preserved (Wedge=3)");
    assertTrue(ff2->slot_index == 2, "slot_index preserved");
    assertTrue(approxEqual(ff2->offset_x, 150.0f), "offset_x preserved");
    assertTrue(approxEqual(ff2->offset_y, -50.0f), "offset_y preserved");
    assertTrue(approxEqual(ff2->offset_z, 0.0f), "offset_z preserved");
}

void testSerializeDeserializeFleetCargoAndRumors() {
    std::cout << "\n=== Serialize/Deserialize FleetCargoPool/RumorLog ===" << std::endl;

    ecs::World world;
    auto* entity = world.createEntity("cargo_rumor_test");

    auto* cargo = addComp<components::FleetCargoPool>(entity);
    cargo->total_capacity = 50000;
    cargo->used_capacity = 12000;
    cargo->pooled_items["Stellium"] = 5000;
    cargo->pooled_items["Vanthium"] = 2000;
    cargo->contributor_ship_ids.push_back("ship_1");
    cargo->contributor_ship_ids.push_back("ship_2");

    auto* rumor = addComp<components::RumorLog>(entity);
    components::RumorLog::Rumor r1;
    r1.rumor_id = "rumor_01";
    r1.text = "Pirates near gate";
    r1.belief_strength = 0.7f;
    r1.personally_witnessed = true;
    r1.times_heard = 3;
    rumor->rumors.push_back(r1);

    data::WorldPersistence persistence;
    std::string json = persistence.serializeWorld(&world);

    ecs::World world2;
    assertTrue(persistence.deserializeWorld(&world2, json),
               "FleetCargoPool/RumorLog deserialization succeeds");

    auto* e2 = world2.getEntity("cargo_rumor_test");
    assertTrue(e2 != nullptr, "Cargo/Rumor entity recreated");

    auto* cargo2 = e2->getComponent<components::FleetCargoPool>();
    assertTrue(cargo2 != nullptr, "FleetCargoPool component recreated");
    assertTrue(cargo2->total_capacity == 50000, "total_capacity preserved");
    assertTrue(cargo2->used_capacity == 12000, "used_capacity preserved");
    assertTrue(cargo2->pooled_items.size() == 2, "pooled_items count preserved");
    assertTrue(cargo2->pooled_items["Stellium"] == 5000, "Stellium quantity preserved");
    assertTrue(cargo2->pooled_items["Vanthium"] == 2000, "Vanthium quantity preserved");
    assertTrue(cargo2->contributor_ship_ids.size() == 2, "contributor count preserved");
    assertTrue(cargo2->contributor_ship_ids[0] == "ship_1", "contributor[0] preserved");
    assertTrue(cargo2->contributor_ship_ids[1] == "ship_2", "contributor[1] preserved");

    auto* rumor2 = e2->getComponent<components::RumorLog>();
    assertTrue(rumor2 != nullptr, "RumorLog component recreated");
    assertTrue(rumor2->rumors.size() == 1, "rumor count preserved");
    assertTrue(rumor2->rumors[0].rumor_id == "rumor_01", "rumor_id preserved");
    assertTrue(rumor2->rumors[0].text == "Pirates near gate", "rumor text preserved");
    assertTrue(approxEqual(rumor2->rumors[0].belief_strength, 0.7f), "belief_strength preserved");
    assertTrue(rumor2->rumors[0].personally_witnessed == true, "personally_witnessed preserved");
    assertTrue(rumor2->rumors[0].times_heard == 3, "times_heard preserved");
}

void testSerializeDeserializeEconomyComponents() {
    std::cout << "\n=== Serialize/Deserialize MineralDeposit/SystemResources/MarketHub ===" << std::endl;

    ecs::World world;

    auto* e1 = world.createEntity("deposit_entity");
    auto* deposit = addComp<components::MineralDeposit>(e1);
    deposit->mineral_type = "Galvite";
    deposit->quantity_remaining = 7500.0f;
    deposit->max_quantity = 10000.0f;
    deposit->yield_rate = 1.2f;
    deposit->volume_per_unit = 0.15f;

    auto* e2 = world.createEntity("sysres_entity");
    auto* sysres = addComp<components::SystemResources>(e2);
    components::SystemResources::ResourceEntry re1;
    re1.mineral_type = "Stellium";
    re1.total_quantity = 50000.0f;
    re1.remaining_quantity = 35000.0f;
    sysres->resources.push_back(re1);
    components::SystemResources::ResourceEntry re2;
    re2.mineral_type = "Vanthium";
    re2.total_quantity = 20000.0f;
    re2.remaining_quantity = 18000.0f;
    sysres->resources.push_back(re2);

    auto* e3 = world.createEntity("market_entity");
    auto* market = addComp<components::MarketHub>(e3);
    market->station_id = "hub_jita";
    market->broker_fee_rate = 0.03;
    market->sales_tax_rate = 0.05;
    components::MarketHub::Order order;
    order.order_id = "order_001";
    order.item_id = "trit";
    order.item_name = "Stellium";
    order.owner_id = "player_1";
    order.is_buy_order = true;
    order.price_per_unit = 5.5;
    order.quantity = 1000;
    order.quantity_remaining = 800;
    order.duration_remaining = 86400.0f;
    order.fulfilled = false;
    market->orders.push_back(order);

    data::WorldPersistence persistence;
    std::string json = persistence.serializeWorld(&world);

    ecs::World world2;
    assertTrue(persistence.deserializeWorld(&world2, json),
               "Economy components deserialization succeeds");

    auto* de = world2.getEntity("deposit_entity");
    assertTrue(de != nullptr, "Deposit entity recreated");
    auto* deposit2 = de->getComponent<components::MineralDeposit>();
    assertTrue(deposit2 != nullptr, "MineralDeposit component recreated");
    assertTrue(deposit2->mineral_type == "Galvite", "mineral_type preserved");
    assertTrue(approxEqual(deposit2->quantity_remaining, 7500.0f), "quantity_remaining preserved");
    assertTrue(approxEqual(deposit2->max_quantity, 10000.0f), "max_quantity preserved");
    assertTrue(approxEqual(deposit2->yield_rate, 1.2f), "yield_rate preserved");
    assertTrue(approxEqual(deposit2->volume_per_unit, 0.15f), "volume_per_unit preserved");

    auto* sr = world2.getEntity("sysres_entity");
    assertTrue(sr != nullptr, "SystemResources entity recreated");
    auto* sysres2 = sr->getComponent<components::SystemResources>();
    assertTrue(sysres2 != nullptr, "SystemResources component recreated");
    assertTrue(sysres2->resources.size() == 2, "resource count preserved");
    assertTrue(sysres2->resources[0].mineral_type == "Stellium", "resource[0] mineral_type preserved");
    assertTrue(approxEqual(sysres2->resources[0].total_quantity, 50000.0f), "resource[0] total_quantity preserved");
    assertTrue(approxEqual(sysres2->resources[0].remaining_quantity, 35000.0f), "resource[0] remaining_quantity preserved");
    assertTrue(sysres2->resources[1].mineral_type == "Vanthium", "resource[1] mineral_type preserved");
    assertTrue(approxEqual(sysres2->resources[1].total_quantity, 20000.0f), "resource[1] total_quantity preserved");
    assertTrue(approxEqual(sysres2->resources[1].remaining_quantity, 18000.0f), "resource[1] remaining_quantity preserved");

    auto* me = world2.getEntity("market_entity");
    assertTrue(me != nullptr, "MarketHub entity recreated");
    auto* market2 = me->getComponent<components::MarketHub>();
    assertTrue(market2 != nullptr, "MarketHub component recreated");
    assertTrue(market2->station_id == "hub_jita", "market station_id preserved");
    assertTrue(approxEqual(static_cast<float>(market2->broker_fee_rate), 0.03f), "broker_fee_rate preserved");
    assertTrue(approxEqual(static_cast<float>(market2->sales_tax_rate), 0.05f), "sales_tax_rate preserved");
    assertTrue(market2->orders.size() == 1, "order count preserved");
    assertTrue(market2->orders[0].order_id == "order_001", "order_id preserved");
    assertTrue(market2->orders[0].item_id == "trit", "order item_id preserved");
    assertTrue(market2->orders[0].item_name == "Stellium", "order item_name preserved");
    assertTrue(market2->orders[0].owner_id == "player_1", "order owner_id preserved");
    assertTrue(market2->orders[0].is_buy_order == true, "is_buy_order preserved");
    assertTrue(approxEqual(static_cast<float>(market2->orders[0].price_per_unit), 5.5f), "price_per_unit preserved");
    assertTrue(market2->orders[0].quantity == 1000, "order quantity preserved");
    assertTrue(market2->orders[0].quantity_remaining == 800, "order quantity_remaining preserved");
    assertTrue(approxEqual(market2->orders[0].duration_remaining, 86400.0f), "order duration_remaining preserved");
    assertTrue(market2->orders[0].fulfilled == false, "order fulfilled preserved");
}

// ==================== Phase 5 Continued: Compressed Persistence Tests ====================

void testPersistenceCompressedSaveLoad() {
    std::cout << "\n=== Persistence: Compressed Save/Load ===" << std::endl;
    ecs::World world;

    // Create entities with various components
    for (int i = 0; i < 50; ++i) {
        std::string id = "compress_ship_" + std::to_string(i);
        auto* e = world.createEntity(id);

        auto* pos = addComp<components::Position>(e);
        pos->x = static_cast<float>(i * 500);
        pos->y = static_cast<float>(i * 100);
        pos->z = 0.0f;

        auto* hp = addComp<components::Health>(e);
        hp->shield_hp = 300.0f + static_cast<float>(i);
        hp->shield_max = 500.0f;
        hp->armor_hp = 200.0f;
        hp->armor_max = 300.0f;
        hp->hull_hp = 100.0f;
        hp->hull_max = 200.0f;

        auto* ship = addComp<components::Ship>(e);
        ship->ship_type = "Frigate";
    }

    assertTrue(world.getEntityCount() == 50, "Created 50 entities for compressed test");

    data::WorldPersistence persistence;
    std::string filepath = "/tmp/eve_compressed_test.json.gz";

    bool saved = persistence.saveWorldCompressed(&world, filepath);
    assertTrue(saved, "Compressed save succeeded");

    // Load into fresh world
    ecs::World world2;
    bool loaded = persistence.loadWorldCompressed(&world2, filepath);
    assertTrue(loaded, "Compressed load succeeded");
    assertTrue(world2.getEntityCount() == 50, "Loaded world has 50 entities");

    // Verify a sample
    auto* e10 = world2.getEntity("compress_ship_10");
    assertTrue(e10 != nullptr, "Ship 10 exists after compressed load");
    auto* pos10 = e10->getComponent<components::Position>();
    assertTrue(pos10 != nullptr, "Position component preserved");
    assertTrue(approxEqual(pos10->x, 5000.0f), "Position x preserved through compression");

    auto* hp10 = e10->getComponent<components::Health>();
    assertTrue(hp10 != nullptr, "Health component preserved");
    assertTrue(approxEqual(hp10->shield_hp, 310.0f), "Health preserved through compression");

    // Clean up
    std::remove(filepath.c_str());
}

void testPersistenceCompressedSmaller() {
    std::cout << "\n=== Persistence: Compressed File Is Smaller ===" << std::endl;
    ecs::World world;

    for (int i = 0; i < 100; ++i) {
        std::string id = "size_ship_" + std::to_string(i);
        auto* e = world.createEntity(id);
        auto* pos = addComp<components::Position>(e);
        pos->x = static_cast<float>(i * 1000);
        pos->y = static_cast<float>(i * 200);
        pos->z = static_cast<float>(i * 50);
        auto* hp = addComp<components::Health>(e);
        hp->shield_hp = 500.0f;
        hp->shield_max = 500.0f;
        auto* ship = addComp<components::Ship>(e);
        ship->ship_type = "Cruiser";
    }

    data::WorldPersistence persistence;
    std::string jsonPath = "/tmp/eve_size_test.json";
    std::string gzPath   = "/tmp/eve_size_test.json.gz";

    persistence.saveWorld(&world, jsonPath);
    persistence.saveWorldCompressed(&world, gzPath);

    // Compare file sizes
    struct stat jsonStat, gzStat;
    stat(jsonPath.c_str(), &jsonStat);
    stat(gzPath.c_str(), &gzStat);

    assertTrue(gzStat.st_size < jsonStat.st_size,
               "Compressed file is smaller than JSON");
    assertTrue(gzStat.st_size > 0, "Compressed file is not empty");

    std::remove(jsonPath.c_str());
    std::remove(gzPath.c_str());
}

// ==================== Phase 5 Continued: 200-Ship Multi-System Stress Test ====================

void testStress200ShipMultiSystem() {
    std::cout << "\n=== Stress Test: 200 Ships Multi-System Tick ===" << std::endl;
    ecs::World world;
    systems::ShieldRechargeSystem shieldSys(&world);
    systems::MovementSystem moveSys(&world);
    systems::LODSystem lodSys(&world);
    systems::SpatialHashSystem spatialSys(&world);
    spatialSys.setCellSize(5000.0f);
    lodSys.setReferencePoint(0.0f, 0.0f, 0.0f);

    // Create 200 ships spread across space
    for (int i = 0; i < 200; ++i) {
        std::string id = "multi_ship_" + std::to_string(i);
        auto* e = world.createEntity(id);

        auto* pos = addComp<components::Position>(e);
        pos->x = static_cast<float>((i % 20) * 3000);
        pos->y = static_cast<float>((i / 20) * 3000);
        pos->z = 0.0f;

        auto* vel = addComp<components::Velocity>(e);
        vel->vx = 10.0f;
        vel->vy = 0.0f;
        vel->vz = 0.0f;

        auto* hp = addComp<components::Health>(e);
        hp->shield_hp = 250.0f;
        hp->shield_max = 500.0f;
        hp->shield_recharge_rate = 5.0f;

        auto* ship = addComp<components::Ship>(e);
        ship->ship_type = (i % 3 == 0) ? "Frigate" : ((i % 3 == 1) ? "Cruiser" : "Battleship");
        vel->max_speed = 300.0f;

        auto* lod = addComp<components::LODPriority>(e);
        lod->priority = 1.0f;
        lod->force_visible = (i == 0);  // only player ship forced
    }

    assertTrue(world.getEntityCount() == 200, "Created 200 ship entities");

    // Run 10 simulation ticks with all systems
    for (int tick = 0; tick < 10; ++tick) {
        float dt = 0.1f;  // 100ms tick (10 Hz)
        moveSys.update(dt);
        shieldSys.update(dt);
        lodSys.update(dt);
        spatialSys.update(dt);
    }

    // Verify shields recharged
    int recharged = 0;
    auto entities = world.getEntities();
    for (auto* e : entities) {
        auto* hp = e->getComponent<components::Health>();
        if (hp && hp->shield_hp > 250.0f) ++recharged;
    }
    assertTrue(recharged == 200, "All 200 ships recharged shields across 10 ticks");

    // Verify LOD computed (player ship should be full detail)
    auto* playerLod = world.getEntity("multi_ship_0")->getComponent<components::LODPriority>();
    assertTrue(approxEqual(playerLod->priority, 2.0f), "Player ship at full detail (force_visible)");

    // Verify LOD counts sum to 200
    int lodTotal = lodSys.getFullDetailCount() + lodSys.getReducedCount() +
                   lodSys.getMergedCount() + lodSys.getImpostorCount();
    assertTrue(lodTotal == 200, "LOD tier counts sum to 200");

    // Verify spatial hash indexed all 200 entities
    assertTrue(spatialSys.getIndexedEntityCount() == 200, "Spatial hash indexed 200 entities");

    // Verify spatial query returns reasonable neighbourhood
    auto nearby = spatialSys.queryNear(0.0f, 0.0f, 0.0f, 5000.0f);
    assertTrue(!nearby.empty(), "Spatial query finds nearby ships");
    assertTrue(nearby.size() <= 200, "Spatial query doesn't exceed total entities");

    // Verify entities moved (velocity applied over 10 ticks)
    auto* ship50 = world.getEntity("multi_ship_50");
    auto* pos50 = ship50->getComponent<components::Position>();
    // Original x was (50%20)*3000 = 30000.0, moved +10.0 * 10 ticks * 0.1s = +10.0m
    assertTrue(pos50->x > 30000.0f, "Ship 50 moved forward from velocity");
}

void testStress200ShipPersistence() {
    std::cout << "\n=== Stress Test: 200 Ships Save/Load ===" << std::endl;
    ecs::World world;

    for (int i = 0; i < 200; ++i) {
        std::string id = "persist200_ship_" + std::to_string(i);
        auto* e = world.createEntity(id);

        auto* pos = addComp<components::Position>(e);
        pos->x = static_cast<float>(i * 500);
        pos->y = static_cast<float>((i % 20) * 100);
        pos->z = 0.0f;

        auto* hp = addComp<components::Health>(e);
        hp->shield_hp = 400.0f + static_cast<float>(i % 100);
        hp->shield_max = 500.0f;

        auto* ship = addComp<components::Ship>(e);
        ship->ship_type = "Frigate";

        auto* lod = addComp<components::LODPriority>(e);
        lod->priority = (i < 10) ? 2.0f : 0.5f;
    }

    assertTrue(world.getEntityCount() == 200, "Created 200 entities");

    data::WorldPersistence persistence;
    std::string filepath = "/tmp/eve_stress_200ships.json.gz";

    bool saved = persistence.saveWorldCompressed(&world, filepath);
    assertTrue(saved, "200-ship compressed save succeeded");

    ecs::World world2;
    bool loaded = persistence.loadWorldCompressed(&world2, filepath);
    assertTrue(loaded, "200-ship compressed load succeeded");
    assertTrue(world2.getEntityCount() == 200, "Loaded world has 200 entities");

    // Verify sample entities
    auto* first = world2.getEntity("persist200_ship_0");
    assertTrue(first != nullptr, "First ship exists after load");
    auto* last = world2.getEntity("persist200_ship_199");
    assertTrue(last != nullptr, "Last ship exists after load");

    auto* hp199 = last->getComponent<components::Health>();
    assertTrue(hp199 != nullptr, "Health preserved on last ship");
    assertTrue(approxEqual(hp199->shield_hp, 499.0f), "Shield HP preserved (400 + 199%100 = 499)");

    std::remove(filepath.c_str());
}

// ==================== SnapshotReplicationSystem Tests ====================

void testSnapshotDeltaFirstSendFull() {
    std::cout << "\n=== Snapshot Delta: First Send is Full ===" << std::endl;

    ecs::World world;
    systems::SnapshotReplicationSystem srs(&world);

    auto* e = world.createEntity("ship_1");
    auto* pos = addComp<components::Position>(e);
    pos->x = 100.0f; pos->y = 200.0f; pos->z = 300.0f; pos->rotation = 1.0f;
    auto* vel = addComp<components::Velocity>(e);
    vel->vx = 10.0f; vel->vy = 0.0f; vel->vz = -5.0f;

    std::string msg = srs.buildDeltaUpdate(1, 1);

    // First send must include position data
    assertTrue(msg.find("\"ship_1\"") != std::string::npos,
               "First delta includes entity id");
    assertTrue(msg.find("\"pos\"") != std::string::npos,
               "First delta includes position");
    assertTrue(msg.find("\"vel\"") != std::string::npos,
               "First delta includes velocity");
    assertTrue(msg.find("\"delta\":true") != std::string::npos,
               "Message marked as delta");
}

void testSnapshotDeltaNoChangeEmpty() {
    std::cout << "\n=== Snapshot Delta: No Change Yields Empty Entities ===" << std::endl;

    ecs::World world;
    systems::SnapshotReplicationSystem srs(&world);

    auto* e = world.createEntity("ship_1");
    auto* pos = addComp<components::Position>(e);
    pos->x = 100.0f; pos->y = 200.0f; pos->z = 300.0f;

    // First call — full
    srs.buildDeltaUpdate(1, 1);

    // Second call — nothing changed
    std::string msg = srs.buildDeltaUpdate(1, 2);

    // Should have empty entities array since nothing changed
    assertTrue(msg.find("\"entities\":[]") != std::string::npos,
               "No-change delta has empty entities");
}

void testSnapshotDeltaPositionChange() {
    std::cout << "\n=== Snapshot Delta: Position Change Detected ===" << std::endl;

    ecs::World world;
    systems::SnapshotReplicationSystem srs(&world);

    auto* e = world.createEntity("ship_1");
    auto* pos = addComp<components::Position>(e);
    pos->x = 100.0f; pos->y = 200.0f; pos->z = 300.0f;
    auto* hp = addComp<components::Health>(e);
    hp->shield_hp = 100.0f; hp->shield_max = 100.0f;

    srs.buildDeltaUpdate(1, 1);

    // Change only position
    pos->x = 150.0f;

    std::string msg = srs.buildDeltaUpdate(1, 2);

    assertTrue(msg.find("\"pos\"") != std::string::npos,
               "Delta includes changed position");
    // Health didn't change, should be omitted
    assertTrue(msg.find("\"health\"") == std::string::npos,
               "Delta omits unchanged health");
}

void testSnapshotDeltaHealthChange() {
    std::cout << "\n=== Snapshot Delta: Health Change Only ===" << std::endl;

    ecs::World world;
    systems::SnapshotReplicationSystem srs(&world);

    auto* e = world.createEntity("ship_1");
    auto* pos = addComp<components::Position>(e);
    pos->x = 100.0f;
    auto* hp = addComp<components::Health>(e);
    hp->shield_hp = 100.0f; hp->shield_max = 100.0f;
    hp->armor_hp = 50.0f; hp->armor_max = 50.0f;
    hp->hull_hp = 200.0f; hp->hull_max = 200.0f;

    srs.buildDeltaUpdate(1, 1);

    // Only change health
    hp->shield_hp = 80.0f;

    std::string msg = srs.buildDeltaUpdate(1, 2);

    assertTrue(msg.find("\"health\"") != std::string::npos,
               "Delta includes changed health");
    assertTrue(msg.find("\"pos\"") == std::string::npos,
               "Delta omits unchanged position");
}

void testSnapshotFullUpdateResets() {
    std::cout << "\n=== Snapshot: Full Update Resets Tracking ===" << std::endl;

    ecs::World world;
    systems::SnapshotReplicationSystem srs(&world);

    auto* e = world.createEntity("ship_1");
    auto* pos = addComp<components::Position>(e);
    pos->x = 100.0f;

    srs.buildDeltaUpdate(1, 1);

    // Full update should resend everything
    std::string msg = srs.buildFullUpdate(1, 2);

    assertTrue(msg.find("\"pos\"") != std::string::npos,
               "Full update includes position even if unchanged");
}

void testSnapshotClearClient() {
    std::cout << "\n=== Snapshot: Clear Client ===" << std::endl;

    ecs::World world;
    systems::SnapshotReplicationSystem srs(&world);

    auto* e = world.createEntity("ship_1");
    addComp<components::Position>(e);

    srs.buildDeltaUpdate(1, 1);
    assertTrue(srs.getTrackedClientCount() == 1, "One client tracked");
    assertTrue(srs.getTrackedEntityCount(1) == 1, "One entity tracked for client");

    srs.clearClient(1);
    assertTrue(srs.getTrackedClientCount() == 0, "Client cleared");
    assertTrue(srs.getTrackedEntityCount(1) == 0, "No entities after clear");
}

void testSnapshotEpsilonFiltering() {
    std::cout << "\n=== Snapshot: Epsilon Filtering ===" << std::endl;

    ecs::World world;
    systems::SnapshotReplicationSystem srs(&world);
    srs.setPositionEpsilon(1.0f);

    auto* e = world.createEntity("ship_1");
    auto* pos = addComp<components::Position>(e);
    pos->x = 100.0f; pos->y = 200.0f; pos->z = 300.0f;

    srs.buildDeltaUpdate(1, 1);

    // Move by less than epsilon
    pos->x = 100.5f;
    std::string msg = srs.buildDeltaUpdate(1, 2);
    assertTrue(msg.find("\"entities\":[]") != std::string::npos,
               "Sub-epsilon change filtered out");

    // Move beyond epsilon
    pos->x = 102.0f;
    msg = srs.buildDeltaUpdate(1, 3);
    assertTrue(msg.find("\"pos\"") != std::string::npos,
               "Super-epsilon change included");
}

void testSnapshotMultipleClients() {
    std::cout << "\n=== Snapshot: Multiple Clients Independent ===" << std::endl;

    ecs::World world;
    systems::SnapshotReplicationSystem srs(&world);

    auto* e = world.createEntity("ship_1");
    auto* pos = addComp<components::Position>(e);
    pos->x = 100.0f;

    // Client 1 gets first update
    srs.buildDeltaUpdate(1, 1);

    // Client 2 has never seen the entity → should get full
    std::string msg2 = srs.buildDeltaUpdate(2, 1);
    assertTrue(msg2.find("\"pos\"") != std::string::npos,
               "Client 2 gets full state for unseen entity");

    // Client 1 gets no change
    std::string msg1 = srs.buildDeltaUpdate(1, 2);
    assertTrue(msg1.find("\"entities\":[]") != std::string::npos,
               "Client 1 gets empty delta (no change)");

    assertTrue(srs.getTrackedClientCount() == 2, "Two clients tracked");
}

// ==================== Wreck Persistence System Tests ====================

void testWreckPersistenceDefaults() {
    std::cout << "\n=== Wreck Persistence Defaults ===" << std::endl;
    ecs::World world;
    auto* e = world.createEntity("wreck1");
    auto* wreck = addComp<components::WreckPersistence>(e);
    assertTrue(approxEqual(wreck->lifetime, 7200.0f), "Default lifetime 7200s");
    assertTrue(approxEqual(wreck->elapsed, 0.0f), "Elapsed starts at 0");
    assertTrue(!wreck->salvage_npc_assigned, "No NPC assigned");
}

void testWreckPersistenceExpires() {
    std::cout << "\n=== Wreck Persistence Expires ===" << std::endl;
    ecs::World world;
    auto* e = world.createEntity("wreck2");
    auto* wreck = addComp<components::WreckPersistence>(e);
    wreck->lifetime = 10.0f;

    systems::WreckPersistenceSystem sys(&world);
    sys.update(5.0f);
    assertTrue(!sys.isExpired("wreck2"), "Not expired at 5s");
    sys.update(6.0f);
    assertTrue(sys.isExpired("wreck2"), "Expired after lifetime");
}

void testWreckPersistenceAssignNPC() {
    std::cout << "\n=== Wreck Persistence Assign NPC ===" << std::endl;
    ecs::World world;
    auto* e = world.createEntity("wreck3");
    auto* wreck = addComp<components::WreckPersistence>(e);

    systems::WreckPersistenceSystem sys(&world);
    sys.assignSalvageNPC("wreck3", "npc_salvager");
    assertTrue(wreck->salvage_npc_assigned, "NPC assigned");
    assertTrue(wreck->assigned_npc_id == "npc_salvager", "Correct NPC ID");
}

// ==================== DatabasePersistenceSystem Tests ====================

void testDatabaseCreate() {
    std::cout << "\n=== DatabasePersistence: Create ===" << std::endl;
    ecs::World world;
    systems::DatabasePersistenceSystem sys(&world);
    auto* e = world.createEntity("db1");
    assertTrue(sys.createDatabase("db1", "players", 30.0f), "Create database succeeds");
    auto* db = e->getComponent<components::DatabasePersistence>();
    assertTrue(db != nullptr, "Component exists");
    assertTrue(db->db_name == "players", "DB name set");
    assertTrue(approxEqual(db->auto_save_interval, 30.0f), "Auto-save interval set");
}

void testDatabaseWriteRead() {
    std::cout << "\n=== DatabasePersistence: Write/Read ===" << std::endl;
    ecs::World world;
    systems::DatabasePersistenceSystem sys(&world);
    world.createEntity("db1");
    sys.createDatabase("db1", "store", 60.0f);
    assertTrue(sys.write("db1", "name", "Alice"), "Write succeeds");
    assertTrue(sys.read("db1", "name") == "Alice", "Read returns written value");
    assertTrue(sys.read("db1", "missing") == "", "Read missing key returns empty");
}

void testDatabaseRemove() {
    std::cout << "\n=== DatabasePersistence: Remove ===" << std::endl;
    ecs::World world;
    systems::DatabasePersistenceSystem sys(&world);
    world.createEntity("db1");
    sys.createDatabase("db1", "store", 60.0f);
    sys.write("db1", "key1", "val1");
    assertTrue(sys.remove("db1", "key1"), "Remove succeeds");
    assertTrue(sys.read("db1", "key1") == "", "Removed key returns empty");
    assertTrue(!sys.remove("db1", "key1"), "Remove non-existent fails");
}

void testDatabaseDirty() {
    std::cout << "\n=== DatabasePersistence: Dirty Flag ===" << std::endl;
    ecs::World world;
    systems::DatabasePersistenceSystem sys(&world);
    world.createEntity("db1");
    sys.createDatabase("db1", "store", 60.0f);
    assertTrue(!sys.isDirty("db1"), "Not dirty initially");
    sys.write("db1", "k", "v");
    assertTrue(sys.isDirty("db1"), "Dirty after write");
    sys.save("db1");
    assertTrue(!sys.isDirty("db1"), "Not dirty after save");
}

void testDatabaseSave() {
    std::cout << "\n=== DatabasePersistence: Explicit Save ===" << std::endl;
    ecs::World world;
    systems::DatabasePersistenceSystem sys(&world);
    world.createEntity("db1");
    sys.createDatabase("db1", "store", 60.0f);
    sys.write("db1", "k", "v");
    assertTrue(sys.save("db1"), "Save succeeds");
    assertTrue(sys.getSaveCount("db1") == 1, "Save count is 1");
    sys.save("db1");
    assertTrue(sys.getSaveCount("db1") == 2, "Save count is 2");
}

void testDatabaseAutoSave() {
    std::cout << "\n=== DatabasePersistence: Auto-Save ===" << std::endl;
    ecs::World world;
    systems::DatabasePersistenceSystem sys(&world);
    auto* e = world.createEntity("db1");
    sys.createDatabase("db1", "store", 1.0f);
    sys.write("db1", "k", "v");
    sys.update(0.5f);
    assertTrue(sys.isDirty("db1"), "Still dirty before interval");
    sys.update(0.6f);
    assertTrue(!sys.isDirty("db1"), "Not dirty after auto-save");
    assertTrue(sys.getSaveCount("db1") == 1, "Auto-save incremented save count");
}

void testDatabaseEntryCount() {
    std::cout << "\n=== DatabasePersistence: Entry Count ===" << std::endl;
    ecs::World world;
    systems::DatabasePersistenceSystem sys(&world);
    world.createEntity("db1");
    sys.createDatabase("db1", "store", 60.0f);
    assertTrue(sys.getEntryCount("db1") == 0, "0 entries initially");
    sys.write("db1", "a", "1");
    sys.write("db1", "b", "2");
    assertTrue(sys.getEntryCount("db1") == 2, "2 entries after writes");
    sys.remove("db1", "a");
    assertTrue(sys.getEntryCount("db1") == 1, "1 entry after remove");
}

void testDatabaseWriteCount() {
    std::cout << "\n=== DatabasePersistence: Write Count ===" << std::endl;
    ecs::World world;
    systems::DatabasePersistenceSystem sys(&world);
    world.createEntity("db1");
    sys.createDatabase("db1", "store", 60.0f);
    sys.write("db1", "a", "1");
    sys.write("db1", "b", "2");
    sys.write("db1", "a", "3");
    assertTrue(sys.getTotalWrites("db1") == 3, "3 total writes");
}

void testDatabaseOverwrite() {
    std::cout << "\n=== DatabasePersistence: Overwrite ===" << std::endl;
    ecs::World world;
    systems::DatabasePersistenceSystem sys(&world);
    world.createEntity("db1");
    sys.createDatabase("db1", "store", 60.0f);
    sys.write("db1", "key", "old");
    sys.write("db1", "key", "new");
    assertTrue(sys.read("db1", "key") == "new", "Overwritten value returned");
    assertTrue(sys.getEntryCount("db1") == 1, "Still 1 entry after overwrite");
}

void testDatabaseMissing() {
    std::cout << "\n=== DatabasePersistence: Missing Entity ===" << std::endl;
    ecs::World world;
    systems::DatabasePersistenceSystem sys(&world);
    assertTrue(!sys.createDatabase("nonexistent", "db", 60.0f), "Create fails on missing");
    assertTrue(!sys.write("nonexistent", "k", "v"), "Write fails on missing");
    assertTrue(sys.read("nonexistent", "k") == "", "Read returns empty on missing");
    assertTrue(!sys.remove("nonexistent", "k"), "Remove fails on missing");
    assertTrue(!sys.save("nonexistent"), "Save fails on missing");
    assertTrue(sys.getEntryCount("nonexistent") == 0, "0 entries on missing");
    assertTrue(!sys.isDirty("nonexistent"), "Not dirty on missing");
    assertTrue(sys.getSaveCount("nonexistent") == 0, "0 saves on missing");
    assertTrue(sys.getTotalWrites("nonexistent") == 0, "0 writes on missing");
}

// ===== PersistenceDelta System Tests =====

void testPersistenceDeltaCreate() {
    std::cout << "\n=== PersistenceDelta: Create ===" << std::endl;
    ecs::World world;
    systems::PersistenceDeltaSystem sys(&world);
    world.createEntity("player1");
    assertTrue(sys.initializeTracker("player1"), "Init tracker succeeds");
    assertTrue(sys.getActionCount("player1") == 0, "Initial action count is 0");
    assertTrue(sys.getEntryCount("player1") == 0, "Initial entry count is 0");
    assertTrue(approxEqual(sys.getTotalImpact("player1"), 0.0f), "Initial total impact is 0");
    assertTrue(!sys.isConsequenceTriggered("player1"), "No consequence initially");
}

void testPersistenceDeltaRecord() {
    std::cout << "\n=== PersistenceDelta: Record ===" << std::endl;
    ecs::World world;
    systems::PersistenceDeltaSystem sys(&world);
    world.createEntity("player1");
    sys.initializeTracker("player1");
    assertTrue(sys.recordAction("player1", "act1", "Combat", 5.0f, 0.1f, false), "Record action");
    assertTrue(sys.getActionCount("player1") == 1, "Action count is 1");
    assertTrue(sys.getEntryCount("player1") == 1, "Entry count is 1");
    assertTrue(approxEqual(sys.getTotalImpact("player1"), 5.0f), "Total impact is 5");
    assertTrue(approxEqual(sys.getPositiveImpact("player1"), 5.0f), "Positive impact is 5");
}

void testPersistenceDeltaNegative() {
    std::cout << "\n=== PersistenceDelta: Negative ===" << std::endl;
    ecs::World world;
    systems::PersistenceDeltaSystem sys(&world);
    world.createEntity("player1");
    sys.initializeTracker("player1");
    assertTrue(sys.recordAction("player1", "crime1", "Crime", -8.0f, 0.1f, false), "Record negative");
    assertTrue(approxEqual(sys.getNegativeImpact("player1"), -8.0f), "Negative impact is -8");
    assertTrue(approxEqual(sys.getTotalImpact("player1"), -8.0f), "Total impact is -8");
}

void testPersistenceDeltaCategory() {
    std::cout << "\n=== PersistenceDelta: Category ===" << std::endl;
    ecs::World world;
    systems::PersistenceDeltaSystem sys(&world);
    world.createEntity("player1");
    sys.initializeTracker("player1");
    sys.recordAction("player1", "a1", "Combat", 3.0f, 0.1f, false);
    sys.recordAction("player1", "a2", "Trade", 7.0f, 0.1f, false);
    sys.recordAction("player1", "a3", "Combat", 2.0f, 0.1f, false);
    assertTrue(approxEqual(sys.getCategoryImpact("player1", "Combat"), 5.0f), "Combat impact is 5");
    assertTrue(approxEqual(sys.getCategoryImpact("player1", "Trade"), 7.0f), "Trade impact is 7");
    assertTrue(approxEqual(sys.getCategoryImpact("player1", "Diplomacy"), 0.0f), "Diplomacy impact is 0");
}

void testPersistenceDeltaDecay() {
    std::cout << "\n=== PersistenceDelta: Decay ===" << std::endl;
    ecs::World world;
    systems::PersistenceDeltaSystem sys(&world);
    world.createEntity("player1");
    sys.initializeTracker("player1");
    sys.recordAction("player1", "a1", "Combat", 1.0f, 1.0f, false); // decays at 1.0/s
    sys.update(0.5f); // magnitude: 1.0 - 0.5 = 0.5
    assertTrue(approxEqual(sys.getTotalImpact("player1"), 0.5f), "Impact decayed to 0.5");
    sys.update(0.5f); // magnitude: 0.5 - 0.5 = 0.0 => removed
    assertTrue(sys.getEntryCount("player1") == 0, "Entry removed after full decay");
}

void testPersistenceDeltaPermanent() {
    std::cout << "\n=== PersistenceDelta: Permanent ===" << std::endl;
    ecs::World world;
    systems::PersistenceDeltaSystem sys(&world);
    world.createEntity("player1");
    sys.initializeTracker("player1");
    sys.recordAction("player1", "a1", "Diplomacy", 5.0f, 1.0f, true);
    sys.update(10.0f); // permanent entries don't decay
    assertTrue(approxEqual(sys.getTotalImpact("player1"), 5.0f), "Permanent entry unchanged");
    assertTrue(sys.getEntryCount("player1") == 1, "Permanent entry still exists");
}

void testPersistenceDeltaConsequence() {
    std::cout << "\n=== PersistenceDelta: Consequence ===" << std::endl;
    ecs::World world;
    systems::PersistenceDeltaSystem sys(&world);
    world.createEntity("player1");
    sys.initializeTracker("player1");
    sys.recordAction("player1", "a1", "Crime", -12.0f, 0.0f, true);
    sys.update(0.0f);
    assertTrue(sys.isConsequenceTriggered("player1"), "Consequence triggered at -12 (threshold 10)");
}

void testPersistenceDeltaClear() {
    std::cout << "\n=== PersistenceDelta: Clear ===" << std::endl;
    ecs::World world;
    systems::PersistenceDeltaSystem sys(&world);
    world.createEntity("player1");
    sys.initializeTracker("player1");
    sys.recordAction("player1", "a1", "Combat", 15.0f, 0.0f, true);
    sys.update(0.0f);
    assertTrue(sys.isConsequenceTriggered("player1"), "Consequence triggered");
    assertTrue(sys.clearConsequence("player1"), "Clear consequence succeeds");
    assertTrue(!sys.isConsequenceTriggered("player1"), "Consequence cleared");
}

void testPersistenceDeltaMaxEntries() {
    std::cout << "\n=== PersistenceDelta: MaxEntries ===" << std::endl;
    ecs::World world;
    systems::PersistenceDeltaSystem sys(&world);
    world.createEntity("player1");
    sys.initializeTracker("player1");
    // Record 105 actions (max is 100)
    for (int i = 0; i < 105; i++) {
        sys.recordAction("player1", "a" + std::to_string(i), "Combat", 1.0f, 0.0f, true);
    }
    assertTrue(sys.getEntryCount("player1") <= 100, "Entries capped at max_entries");
    assertTrue(sys.getActionCount("player1") == 105, "Action count tracks all recordings");
}

void testPersistenceDeltaMissing() {
    std::cout << "\n=== PersistenceDelta: Missing ===" << std::endl;
    ecs::World world;
    systems::PersistenceDeltaSystem sys(&world);
    assertTrue(!sys.initializeTracker("nonexistent"), "Init fails on missing entity");
    assertTrue(!sys.recordAction("nonexistent", "a1", "Combat", 1.0f, 0.1f, false), "Record fails on missing");
    assertTrue(approxEqual(sys.getTotalImpact("nonexistent"), 0.0f), "0 impact on missing");
    assertTrue(approxEqual(sys.getCategoryImpact("nonexistent", "Combat"), 0.0f), "0 category on missing");
    assertTrue(sys.getActionCount("nonexistent") == 0, "0 actions on missing");
    assertTrue(!sys.isConsequenceTriggered("nonexistent"), "No consequence on missing");
    assertTrue(!sys.clearConsequence("nonexistent"), "Clear fails on missing");
    assertTrue(sys.getEntryCount("nonexistent") == 0, "0 entries on missing");
    assertTrue(approxEqual(sys.getPositiveImpact("nonexistent"), 0.0f), "0 positive on missing");
    assertTrue(approxEqual(sys.getNegativeImpact("nonexistent"), 0.0f), "0 negative on missing");
}

// ==================== SnapshotReplication System Tests ====================

void testSnapshotRepCreate() {
    std::cout << "\n=== SnapshotReplication: Create ===" << std::endl;
    ecs::World world;
    systems::SnapshotReplicationSystem2 sys(&world);
    world.createEntity("rep1");
    assertTrue(sys.initialize("rep1", "server_main"), "Init succeeds");
    assertTrue(sys.getCurrentFrame("rep1") == 0, "Frame 0 initially");
    assertTrue(sys.getHistorySize("rep1") == 0, "Empty history");
    assertTrue(sys.getClientCount("rep1") == 0, "No clients");
}

void testSnapshotRepCapture() {
    std::cout << "\n=== SnapshotReplication: Capture ===" << std::endl;
    ecs::World world;
    systems::SnapshotReplicationSystem2 sys(&world);
    world.createEntity("rep1");
    sys.initialize("rep1", "server_main");
    assertTrue(sys.captureSnapshot("rep1"), "Capture succeeds");
    assertTrue(sys.getCurrentFrame("rep1") == 1, "Frame advanced to 1");
    assertTrue(sys.getHistorySize("rep1") == 1, "1 frame in history");
    assertTrue(sys.getTotalSnapshotsSent("rep1") == 1, "1 snapshot sent");
}

void testSnapshotRepAddEntity() {
    std::cout << "\n=== SnapshotReplication: AddEntity ===" << std::endl;
    ecs::World world;
    systems::SnapshotReplicationSystem2 sys(&world);
    world.createEntity("rep1");
    sys.initialize("rep1", "server_main");
    sys.captureSnapshot("rep1");
    assertTrue(sys.addEntityToSnapshot("rep1", "ship1", 100.0f, 200.0f, 300.0f, 100.0f, 100.0f, 50.0f), "Add entity succeeds");
    assertTrue(!sys.addEntityToSnapshot("rep1", "ship1", 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f), "Duplicate rejected");
}

void testSnapshotRepRegisterClient() {
    std::cout << "\n=== SnapshotReplication: RegisterClient ===" << std::endl;
    ecs::World world;
    systems::SnapshotReplicationSystem2 sys(&world);
    world.createEntity("rep1");
    sys.initialize("rep1", "server_main");
    assertTrue(sys.registerClient("rep1", "client1"), "Register client succeeds");
    assertTrue(sys.getClientCount("rep1") == 1, "1 client");
    assertTrue(!sys.registerClient("rep1", "client1"), "Duplicate client rejected");
    assertTrue(sys.registerClient("rep1", "client2"), "Second client succeeds");
    assertTrue(sys.getClientCount("rep1") == 2, "2 clients");
}

void testSnapshotRepUnregisterClient() {
    std::cout << "\n=== SnapshotReplication: UnregisterClient ===" << std::endl;
    ecs::World world;
    systems::SnapshotReplicationSystem2 sys(&world);
    world.createEntity("rep1");
    sys.initialize("rep1", "server_main");
    sys.registerClient("rep1", "client1");
    assertTrue(sys.unregisterClient("rep1", "client1"), "Unregister succeeds");
    assertTrue(sys.getClientCount("rep1") == 0, "0 clients after unregister");
    assertTrue(!sys.unregisterClient("rep1", "client1"), "Unregister nonexistent fails");
}

void testSnapshotRepAcknowledge() {
    std::cout << "\n=== SnapshotReplication: Acknowledge ===" << std::endl;
    ecs::World world;
    systems::SnapshotReplicationSystem2 sys(&world);
    world.createEntity("rep1");
    sys.initialize("rep1", "server_main");
    sys.registerClient("rep1", "client1");
    sys.captureSnapshot("rep1");
    assertTrue(sys.acknowledgeFrame("rep1", "client1", 1), "Ack succeeds");
    assertTrue(sys.getClientLastAck("rep1", "client1") == 1, "Last ack is frame 1");
    // Old frame ack should not regress
    assertTrue(sys.acknowledgeFrame("rep1", "client1", 0), "Old ack accepted");
    assertTrue(sys.getClientLastAck("rep1", "client1") == 1, "Last ack still frame 1");
}

void testSnapshotRepDelta() {
    std::cout << "\n=== SnapshotReplication: Delta ===" << std::endl;
    ecs::World world;
    systems::SnapshotReplicationSystem2 sys(&world);
    world.createEntity("rep1");
    sys.initialize("rep1", "server_main");
    sys.registerClient("rep1", "client1");
    // Frame 1: add ship1
    sys.captureSnapshot("rep1");
    sys.addEntityToSnapshot("rep1", "ship1", 100.0f, 200.0f, 300.0f, 100.0f, 100.0f, 50.0f);
    sys.acknowledgeFrame("rep1", "client1", 1);
    // Frame 2: ship1 moves
    sys.captureSnapshot("rep1");
    sys.addEntityToSnapshot("rep1", "ship1", 110.0f, 210.0f, 310.0f, 100.0f, 100.0f, 50.0f);
    int delta = sys.getDeltaEntityCount("rep1", "client1");
    assertTrue(delta == 1, "1 entity changed in delta");
}

void testSnapshotRepAutoCapture() {
    std::cout << "\n=== SnapshotReplication: AutoCapture ===" << std::endl;
    ecs::World world;
    systems::SnapshotReplicationSystem2 sys(&world);
    world.createEntity("rep1");
    sys.initialize("rep1", "server_main");
    // Default interval 0.05s (20Hz)
    sys.update(0.05f);
    assertTrue(sys.getCurrentFrame("rep1") == 1, "Auto-captured frame 1");
    sys.update(0.05f);
    assertTrue(sys.getCurrentFrame("rep1") == 2, "Auto-captured frame 2");
    assertTrue(sys.getTotalSnapshotsSent("rep1") == 2, "2 snapshots sent");
}

void testSnapshotRepHistoryLimit() {
    std::cout << "\n=== SnapshotReplication: HistoryLimit ===" << std::endl;
    ecs::World world;
    systems::SnapshotReplicationSystem2 sys(&world);
    world.createEntity("rep1");
    sys.initialize("rep1", "server_main");
    auto* entity = world.getEntity("rep1");
    auto* sr = entity->getComponent<components::SnapshotReplication>();
    sr->max_history = 3;
    for (int i = 0; i < 5; i++) sys.captureSnapshot("rep1");
    assertTrue(sys.getHistorySize("rep1") == 3, "History trimmed to max");
    assertTrue(sys.getCurrentFrame("rep1") == 5, "Frame counter at 5");
}

void testSnapshotRepMissing() {
    std::cout << "\n=== SnapshotReplication: Missing ===" << std::endl;
    ecs::World world;
    systems::SnapshotReplicationSystem2 sys(&world);
    assertTrue(!sys.initialize("nonexistent", "s1"), "Init fails on missing");
    assertTrue(!sys.captureSnapshot("nonexistent"), "Capture fails on missing");
    assertTrue(!sys.addEntityToSnapshot("nonexistent", "e1", 0, 0, 0, 0, 0, 0), "Add entity fails on missing");
    assertTrue(!sys.registerClient("nonexistent", "c1"), "Register fails on missing");
    assertTrue(!sys.unregisterClient("nonexistent", "c1"), "Unregister fails on missing");
    assertTrue(!sys.acknowledgeFrame("nonexistent", "c1", 1), "Ack fails on missing");
    assertTrue(sys.getCurrentFrame("nonexistent") == 0, "0 frame on missing");
    assertTrue(sys.getHistorySize("nonexistent") == 0, "0 history on missing");
    assertTrue(sys.getClientCount("nonexistent") == 0, "0 clients on missing");
    assertTrue(sys.getTotalSnapshotsSent("nonexistent") == 0, "0 sent on missing");
}


void run_persistence_tests() {
    testSerializeDeserializeBasicEntity();
    testSerializeDeserializeHealthCapacitor();
    testSerializeDeserializeShipAndFaction();
    testSerializeDeserializeStandings();
    testStandingsGetStanding();
    testStandingsModify();
    testSerializeDeserializeAIAndWeapon();
    testSerializeDeserializePlayerComponent();
    testSerializeDeserializeMultipleEntities();
    testSaveLoadFile();
    testLoadNonexistentFile();
    testSerializeDeserializeWormholeAndSolarSystem();
    testEmptyWorldSerialize();
    testSerializeDeserializeStation();
    testSerializeDeserializeCaptainPersonality();
    testSerializeDeserializeFleetState();
    testSerializeDeserializeCaptainMemory();
    testSerializeDeserializeFleetFormation();
    testSerializeDeserializeFleetCargoAndRumors();
    testSerializeDeserializeEconomyComponents();
    testPersistenceCompressedSaveLoad();
    testPersistenceCompressedSmaller();
    testStress200ShipMultiSystem();
    testStress200ShipPersistence();
    testSnapshotDeltaFirstSendFull();
    testSnapshotDeltaNoChangeEmpty();
    testSnapshotDeltaPositionChange();
    testSnapshotDeltaHealthChange();
    testSnapshotFullUpdateResets();
    testSnapshotClearClient();
    testSnapshotEpsilonFiltering();
    testSnapshotMultipleClients();
    testWreckPersistenceDefaults();
    testWreckPersistenceExpires();
    testWreckPersistenceAssignNPC();
    testDatabaseCreate();
    testDatabaseWriteRead();
    testDatabaseRemove();
    testDatabaseDirty();
    testDatabaseSave();
    testDatabaseAutoSave();
    testDatabaseEntryCount();
    testDatabaseWriteCount();
    testDatabaseOverwrite();
    testDatabaseMissing();
    testPersistenceDeltaCreate();
    testPersistenceDeltaRecord();
    testPersistenceDeltaNegative();
    testPersistenceDeltaCategory();
    testPersistenceDeltaDecay();
    testPersistenceDeltaPermanent();
    testPersistenceDeltaConsequence();
    testPersistenceDeltaClear();
    testPersistenceDeltaMaxEntries();
    testPersistenceDeltaMissing();
    testSnapshotRepCreate();
    testSnapshotRepCapture();
    testSnapshotRepAddEntity();
    testSnapshotRepRegisterClient();
    testSnapshotRepUnregisterClient();
    testSnapshotRepAcknowledge();
    testSnapshotRepDelta();
    testSnapshotRepAutoCapture();
    testSnapshotRepHistoryLimit();
    testSnapshotRepMissing();
}
