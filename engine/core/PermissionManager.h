#pragma once
// PermissionManager – Phase 8 permission tiers for Atlas engine
// Defines ViewOnly, QA, Developer, CI, Admin access levels
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>

namespace atlas {

enum class PermissionTier : uint8_t {
    ViewOnly,
    QA,
    Developer,
    CI,
    Admin
};

enum class Permission : uint8_t {
    ViewState,
    InspectEntities,
    ViewReplays,
    RunTests,
    RecordReplays,
    InjectInputs,
    EditAssets,
    EditGraphs,
    ModifyState,
    UseDebugTools,
    RunCI,
    ValidateReplays,
    CompareHashes,
    FullAccess
};

struct UserPermissions {
    std::string userId;
    PermissionTier tier = PermissionTier::Developer;
    std::vector<Permission> customPermissions;
};

class PermissionManager {
public:
    PermissionManager();
    ~PermissionManager();

    void SetDefaultTier(PermissionTier tier);
    PermissionTier GetDefaultTier() const;

    void RegisterUser(const std::string& userId, PermissionTier tier);
    void RemoveUser(const std::string& userId);
    PermissionTier GetUserTier(const std::string& userId) const;

    bool HasPermission(const std::string& userId, Permission perm) const;
    void GrantPermission(const std::string& userId, Permission perm);
    void RevokePermission(const std::string& userId, Permission perm);

    static std::string TierName(PermissionTier tier);
    static std::vector<Permission> TierPermissions(PermissionTier tier);

    size_t UserCount() const;
    std::vector<std::string> RegisteredUsers() const;

private:
    PermissionTier m_defaultTier = PermissionTier::Developer;
    std::unordered_map<std::string, UserPermissions> m_users;
};

} // namespace atlas
