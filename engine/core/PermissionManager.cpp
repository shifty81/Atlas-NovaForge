#include "PermissionManager.h"
#include <algorithm>

namespace atlas {

PermissionManager::PermissionManager() = default;
PermissionManager::~PermissionManager() = default;

void PermissionManager::SetDefaultTier(PermissionTier tier) {
    m_defaultTier = tier;
}

PermissionTier PermissionManager::GetDefaultTier() const {
    return m_defaultTier;
}

void PermissionManager::RegisterUser(const std::string& userId, PermissionTier tier) {
    UserPermissions up;
    up.userId = userId;
    up.tier = tier;
    m_users[userId] = up;
}

void PermissionManager::RemoveUser(const std::string& userId) {
    m_users.erase(userId);
}

PermissionTier PermissionManager::GetUserTier(const std::string& userId) const {
    auto it = m_users.find(userId);
    if (it != m_users.end()) {
        return it->second.tier;
    }
    return m_defaultTier;
}

bool PermissionManager::HasPermission(const std::string& userId, Permission perm) const {
    auto it = m_users.find(userId);
    if (it == m_users.end()) {
        return false;
    }

    const auto& up = it->second;

    // Check tier-based permissions
    auto tierPerms = TierPermissions(up.tier);
    for (auto tp : tierPerms) {
        if (tp == perm || tp == Permission::FullAccess) {
            return true;
        }
    }

    // Check custom permissions
    for (auto cp : up.customPermissions) {
        if (cp == perm) {
            return true;
        }
    }

    return false;
}

void PermissionManager::GrantPermission(const std::string& userId, Permission perm) {
    auto it = m_users.find(userId);
    if (it == m_users.end()) {
        return;
    }

    auto& customs = it->second.customPermissions;
    for (auto cp : customs) {
        if (cp == perm) {
            return;
        }
    }
    customs.push_back(perm);
}

void PermissionManager::RevokePermission(const std::string& userId, Permission perm) {
    auto it = m_users.find(userId);
    if (it == m_users.end()) {
        return;
    }

    auto& customs = it->second.customPermissions;
    customs.erase(std::remove(customs.begin(), customs.end(), perm), customs.end());
}

std::string PermissionManager::TierName(PermissionTier tier) {
    switch (tier) {
        case PermissionTier::ViewOnly:  return "ViewOnly";
        case PermissionTier::QA:        return "QA";
        case PermissionTier::Developer: return "Developer";
        case PermissionTier::CI:        return "CI";
        case PermissionTier::Admin:     return "Admin";
    }
    return "Unknown";
}

std::vector<Permission> PermissionManager::TierPermissions(PermissionTier tier) {
    std::vector<Permission> perms;

    // ViewOnly base permissions
    perms.push_back(Permission::ViewState);
    perms.push_back(Permission::InspectEntities);
    perms.push_back(Permission::ViewReplays);

    if (tier == PermissionTier::ViewOnly) {
        return perms;
    }

    // QA adds test/replay/input permissions
    perms.push_back(Permission::RunTests);
    perms.push_back(Permission::RecordReplays);
    perms.push_back(Permission::InjectInputs);

    if (tier == PermissionTier::QA) {
        return perms;
    }

    if (tier == PermissionTier::Developer) {
        perms.push_back(Permission::EditAssets);
        perms.push_back(Permission::EditGraphs);
        perms.push_back(Permission::ModifyState);
        perms.push_back(Permission::UseDebugTools);
        return perms;
    }

    if (tier == PermissionTier::CI) {
        perms.push_back(Permission::RunCI);
        perms.push_back(Permission::ValidateReplays);
        perms.push_back(Permission::CompareHashes);
        return perms;
    }

    // Admin gets all Developer + CI permissions plus FullAccess
    auto devPerms = TierPermissions(PermissionTier::Developer);
    auto ciPerms = TierPermissions(PermissionTier::CI);
    perms = devPerms;
    for (auto p : ciPerms) {
        if (std::find(perms.begin(), perms.end(), p) == perms.end()) {
            perms.push_back(p);
        }
    }
    perms.push_back(Permission::FullAccess);

    return perms;
}

size_t PermissionManager::UserCount() const {
    return m_users.size();
}

std::vector<std::string> PermissionManager::RegisteredUsers() const {
    std::vector<std::string> result;
    result.reserve(m_users.size());
    for (const auto& pair : m_users) {
        result.push_back(pair.first);
    }
    return result;
}

} // namespace atlas
