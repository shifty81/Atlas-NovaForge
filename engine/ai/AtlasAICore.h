#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

namespace atlas::ai {

enum class AIPermission : uint8_t {
    None = 0,
    ReadState = 1,
    SuggestChanges = 2,
    ApplyChanges = 4,
    ExecuteCommands = 8,
    All = 0xFF
};

struct AIRequest {
    std::string intentName;
    std::string context;
    std::unordered_map<std::string, std::string> parameters;
    uint8_t requiredPermissions = 0;
    uint64_t requestID = 0;
};

struct AIResponse {
    uint64_t requestID = 0;
    bool success = false;
    std::string result;
    std::string errorMessage;
    std::vector<std::string> suggestions;
    double confidence = 0.0;
};

struct AIIntentHandler {
    std::string name;
    std::string description;
    uint8_t requiredPermissions = 0;
    std::function<AIResponse(const AIRequest&)> handler;
};

class AtlasAICore {
public:
    void SetPermissions(uint8_t permissions);
    uint8_t GetPermissions() const;
    bool HasPermission(AIPermission perm) const;

    void RegisterIntent(const AIIntentHandler& handler);
    void UnregisterIntent(const std::string& name);
    const AIIntentHandler* GetIntent(const std::string& name) const;
    std::vector<std::string> ListIntents() const;
    size_t IntentCount() const;

    AIResponse ProcessRequest(const AIRequest& request);

    const std::vector<AIRequest>& RequestHistory() const;
    const std::vector<AIResponse>& ResponseHistory() const;
    size_t RequestCount() const;

    void Clear();

private:
    uint8_t m_permissions = 0;
    std::unordered_map<std::string, AIIntentHandler> m_intents;
    std::vector<AIRequest> m_requestHistory;
    std::vector<AIResponse> m_responseHistory;
    uint64_t m_nextRequestID = 1;
};

} // namespace atlas::ai
