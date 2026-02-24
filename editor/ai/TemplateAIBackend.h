#pragma once
#include "AIAggregator.h"
#include <unordered_map>

namespace atlas::ai {

/// Offline template-based AI backend (Layer 1).
/// Provides deterministic, pre-authored responses for common queries
/// without requiring any external LLM service.
/// See docs/16_ATLAS_AI.md for the three-layer AI architecture.
class TemplateAIBackend : public AIBackend {
public:
    TemplateAIBackend();

    AggregatorResponse Query(
        const std::string& prompt,
        const AIContext& context) override;

    /// Register a custom template pattern and response
    void AddTemplate(const std::string& pattern, const std::string& response);

    /// Returns the number of registered templates
    size_t TemplateCount() const;

    /// Clear all templates
    void ClearTemplates();

private:
    struct TemplateEntry {
        std::string pattern;
        std::string response;
        AIRequestType associatedType;
    };

    std::vector<TemplateEntry> m_templates;

    void RegisterDefaults();
    AggregatorResponse MatchTemplate(const std::string& prompt, const AIContext& context) const;
    std::string ExpandVariables(const std::string& tmpl, const AIContext& context) const;
    float ComputeMatchScore(const std::string& prompt, const std::string& pattern) const;
};

} // namespace atlas::ai
