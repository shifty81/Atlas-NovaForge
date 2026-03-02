#ifndef NOVAFORGE_SYSTEMS_MISSION_EDITOR_SYSTEM_H
#define NOVAFORGE_SYSTEMS_MISSION_EDITOR_SYSTEM_H

#include "ecs/system.h"
#include <string>

namespace atlas {
namespace systems {

/**
 * @brief Mission template editor for creating and validating custom missions
 *
 * Provides a workflow for creating mission templates with objectives,
 * rewards, difficulty settings, and validation before publishing.
 */
class MissionEditorSystem : public ecs::System {
public:
    explicit MissionEditorSystem(ecs::World* world);
    ~MissionEditorSystem() override = default;

    void update(float delta_time) override;
    std::string getName() const override { return "MissionEditorSystem"; }

    bool createEditor(const std::string& entity_id);
    bool setMissionName(const std::string& entity_id, const std::string& name);
    bool setMissionLevel(const std::string& entity_id, int level);
    bool setMissionType(const std::string& entity_id, int type);
    int addObjective(const std::string& entity_id, const std::string& description, int obj_type);
    bool removeObjective(const std::string& entity_id, int objective_id);
    bool setReward(const std::string& entity_id, float credits, float standing);
    bool validate(const std::string& entity_id) const;
    bool publish(const std::string& entity_id);
    int getObjectiveCount(const std::string& entity_id) const;
    int getPublishedCount(const std::string& entity_id) const;
    std::string getValidationError(const std::string& entity_id) const;
};

} // namespace systems
} // namespace atlas

#endif // NOVAFORGE_SYSTEMS_MISSION_EDITOR_SYSTEM_H
