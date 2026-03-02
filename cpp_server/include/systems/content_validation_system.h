#ifndef NOVAFORGE_SYSTEMS_CONTENT_VALIDATION_SYSTEM_H
#define NOVAFORGE_SYSTEMS_CONTENT_VALIDATION_SYSTEM_H

#include "ecs/system.h"
#include <string>

namespace atlas {
namespace systems {

/**
 * @brief Content validation for user-created game content
 *
 * Validates ships, modules, and missions against game rules to ensure
 * balance and correctness before they can be used in the game.
 */
class ContentValidationSystem : public ecs::System {
public:
    explicit ContentValidationSystem(ecs::World* world);
    ~ContentValidationSystem() override = default;

    void update(float delta_time) override;
    std::string getName() const override { return "ContentValidationSystem"; }

    bool createValidator(const std::string& entity_id);
    bool submitContent(const std::string& entity_id, const std::string& content_id,
                       int content_type, const std::string& name);
    bool runValidation(const std::string& entity_id, const std::string& content_id);
    bool approveContent(const std::string& entity_id, const std::string& content_id);
    bool rejectContent(const std::string& entity_id, const std::string& content_id,
                       const std::string& reason);
    int getContentState(const std::string& entity_id, const std::string& content_id) const;
    int getPendingCount(const std::string& entity_id) const;
    int getApprovedCount(const std::string& entity_id) const;
    int getRejectedCount(const std::string& entity_id) const;
    std::string getRejectionReason(const std::string& entity_id,
                                    const std::string& content_id) const;
    int getTotalValidations(const std::string& entity_id) const;
};

} // namespace systems
} // namespace atlas

#endif // NOVAFORGE_SYSTEMS_CONTENT_VALIDATION_SYSTEM_H
