#pragma once
#include <string>
#include <unordered_map>
#include <cstdint>

namespace atlas::ai {

struct AISignal {
    std::string name;
    std::string ns;
    float value = 0.0f;
    uint64_t lastUpdatedTick = 0;
};

class AISignalRegistry {
public:
    static AISignalRegistry& Get();

    void Register(const std::string& ns, const std::string& name, float initialValue = 0.0f);
    bool Has(const std::string& qualifiedName) const;

    float Read(const std::string& qualifiedName) const;
    void Write(const std::string& qualifiedName, float value, uint64_t tick);

    const AISignal* GetSignal(const std::string& qualifiedName) const;
    size_t SignalCount() const;

    void Clear();

private:
    std::unordered_map<std::string, AISignal> m_signals;

    static std::string Qualify(const std::string& ns, const std::string& name);
};

}
