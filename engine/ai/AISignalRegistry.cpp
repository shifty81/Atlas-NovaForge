#include "AISignalRegistry.h"

namespace atlas::ai {

AISignalRegistry& AISignalRegistry::Get() {
    static AISignalRegistry instance;
    return instance;
}

std::string AISignalRegistry::Qualify(const std::string& ns, const std::string& name) {
    return ns + "." + name;
}

void AISignalRegistry::Register(const std::string& ns, const std::string& name, float initialValue) {
    std::string key = Qualify(ns, name);
    AISignal sig;
    sig.name = name;
    sig.ns = ns;
    sig.value = initialValue;
    sig.lastUpdatedTick = 0;
    m_signals[key] = sig;
}

bool AISignalRegistry::Has(const std::string& qualifiedName) const {
    return m_signals.count(qualifiedName) > 0;
}

float AISignalRegistry::Read(const std::string& qualifiedName) const {
    auto it = m_signals.find(qualifiedName);
    if (it == m_signals.end()) return 0.0f;
    return it->second.value;
}

void AISignalRegistry::Write(const std::string& qualifiedName, float value, uint64_t tick) {
    auto it = m_signals.find(qualifiedName);
    if (it == m_signals.end()) return;
    it->second.value = value;
    it->second.lastUpdatedTick = tick;
}

const AISignal* AISignalRegistry::GetSignal(const std::string& qualifiedName) const {
    auto it = m_signals.find(qualifiedName);
    if (it != m_signals.end()) return &it->second;
    return nullptr;
}

size_t AISignalRegistry::SignalCount() const {
    return m_signals.size();
}

void AISignalRegistry::Clear() {
    m_signals.clear();
}

}
