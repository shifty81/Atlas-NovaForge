#include "AIAssetDecisionFramework.h"
#include <algorithm>
#include <sstream>

namespace atlas::ai {

// Confidence adjustment constants
constexpr double HIGH_PRIORITY_CONFIDENCE_BOOST = 0.1;
constexpr double UNAVAILABLE_MARKETPLACE_PENALTY = 0.5;

AIAssetDecisionFramework::AIAssetDecisionFramework() {}

AIAssetDecisionFramework::~AIAssetDecisionFramework() {}

// --- Configuration ---

void AIAssetDecisionFramework::SetConfig(const AssetDecisionConfig& config) {
    m_config = config;
}

const AssetDecisionConfig& AIAssetDecisionFramework::GetConfig() const {
    return m_config;
}

// --- Context Management ---

void AIAssetDecisionFramework::SetProjectContext(std::shared_ptr<ProjectContext> context) {
    m_context = std::move(context);
}

std::shared_ptr<ProjectContext> AIAssetDecisionFramework::GetProjectContext() const {
    return m_context;
}

// --- Marketplace Integration ---

void AIAssetDecisionFramework::SetMarketplaceRegistry(asset::MarketplaceImportRegistry* registry) {
    m_marketplaceRegistry = registry;
}

// --- AI Core Integration ---

void AIAssetDecisionFramework::RegisterWithAICore(AtlasAICore& core) {
    // Register analyze intent
    AIIntentHandler analyzeHandler;
    analyzeHandler.name = "asset.analyze";
    analyzeHandler.description = "Analyze project and suggest missing assets";
    analyzeHandler.requiredPermissions = static_cast<uint8_t>(AIPermission::ReadState);
    analyzeHandler.handler = [this](const AIRequest& req) {
        return HandleAnalyzeIntent(req);
    };
    core.RegisterIntent(analyzeHandler);

    // Register suggest intent
    AIIntentHandler suggestHandler;
    suggestHandler.name = "asset.suggest";
    suggestHandler.description = "Get asset suggestions for a category";
    suggestHandler.requiredPermissions = static_cast<uint8_t>(AIPermission::ReadState);
    suggestHandler.handler = [this](const AIRequest& req) {
        return HandleSuggestIntent(req);
    };
    core.RegisterIntent(suggestHandler);

    // Register fetch intent
    AIIntentHandler fetchHandler;
    fetchHandler.name = "asset.fetch";
    fetchHandler.description = "Fetch an asset from marketplace";
    fetchHandler.requiredPermissions = static_cast<uint8_t>(AIPermission::ExecuteCommands);
    fetchHandler.handler = [this](const AIRequest& req) {
        return HandleFetchIntent(req);
    };
    core.RegisterIntent(fetchHandler);
}

void AIAssetDecisionFramework::UnregisterFromAICore(AtlasAICore& core) {
    core.UnregisterIntent("asset.analyze");
    core.UnregisterIntent("asset.suggest");
    core.UnregisterIntent("asset.fetch");
}

// --- Decision Making ---

std::vector<AssetDecision> AIAssetDecisionFramework::AnalyzeProject() {
    std::vector<AssetDecision> decisions;
    
    if (!m_context) {
        return decisions;
    }
    
    // Get suggestions from project context
    auto suggestions = m_context->GetPrioritySuggestions(m_config.maxSuggestionsPerRun);
    
    for (const auto& suggestion : suggestions) {
        decisions.push_back(CreateDecision(suggestion));
        m_decisionCount++;
    }
    
    return decisions;
}

std::vector<AssetDecision> AIAssetDecisionFramework::AnalyzeCategory(AssetCategory category) {
    std::vector<AssetDecision> decisions;
    
    if (!m_context) {
        return decisions;
    }
    
    // Get all suggestions and filter by category
    auto allSuggestions = m_context->GenerateSuggestions();
    
    for (const auto& suggestion : allSuggestions) {
        if (suggestion.category == category) {
            decisions.push_back(CreateDecision(suggestion));
            m_decisionCount++;
        }
    }
    
    return decisions;
}

bool AIAssetDecisionFramework::ExecuteDecision(const AssetDecision& decision) {
    if (!decision.shouldGenerate && !decision.shouldFetch) {
        return false;
    }
    
    if (decision.shouldGenerate) {
        // In a full implementation, this would invoke procedural generation
        // For now, we just mark it as executed
        m_executedCount++;
        return true;
    }
    
    if (decision.shouldFetch && m_marketplaceRegistry) {
        // Determine marketplace type from decision
        asset::MarketplaceType marketplaceType = asset::MarketplaceType::ItchIO;
        if (decision.marketplace == "unreal") {
            marketplaceType = asset::MarketplaceType::UnrealEngine;
        } else if (decision.marketplace == "unity") {
            marketplaceType = asset::MarketplaceType::UnityAssetStore;
        }
        
        asset::MarketplaceImportOptions options;
        options.downloadCache = m_config.cacheDirectory;
        
        auto result = m_marketplaceRegistry->FetchAndImport(
            marketplaceType, decision.assetId, options);
        
        if (result.success) {
            // Add imported asset to context - derive ID from output path
            if (m_context && !result.outputPath.empty()) {
                // Extract filename as asset ID
                size_t lastSlash = result.outputPath.find_last_of("/\\");
                std::string assetId = (lastSlash != std::string::npos) 
                    ? result.outputPath.substr(lastSlash + 1) 
                    : result.outputPath;
                m_context->AddAsset(assetId, decision.suggestion.category);
            }
            m_executedCount++;
            return true;
        }
    }
    
    return false;
}

uint32_t AIAssetDecisionFramework::ExecuteApprovedDecisions(
    const std::vector<AssetDecision>& decisions,
    DecisionApprovalCallback approvalCallback) {
    
    uint32_t executedCount = 0;
    
    for (const auto& decision : decisions) {
        DecisionAction action = DecisionAction::None;
        
        // Check for auto-approval
        if (m_config.autoApprove && decision.confidence >= m_config.autoApproveThreshold) {
            if (decision.shouldGenerate) {
                action = DecisionAction::Generate;
            } else if (decision.shouldFetch) {
                action = DecisionAction::Fetch;
            }
        } else if (approvalCallback) {
            // Ask for approval
            action = approvalCallback(decision);
        }
        
        // Execute based on action
        if (action == DecisionAction::Generate || action == DecisionAction::Fetch) {
            if (ExecuteDecision(decision)) {
                executedCount++;
            }
        }
    }
    
    return executedCount;
}

// --- Suggestion Queries ---

std::vector<AssetSuggestion> AIAssetDecisionFramework::GetTopMissingAssets(uint32_t count) const {
    if (!m_context) {
        return {};
    }
    return m_context->GetPrioritySuggestions(count);
}

bool AIAssetDecisionFramework::IsAssetNeeded(AssetCategory category) const {
    if (!m_context) {
        return false;
    }
    return !m_context->HasAssetCategory(category);
}

std::vector<std::string> AIAssetDecisionFramework::GetRecommendedSources(AssetCategory category) const {
    std::vector<std::string> sources;
    
    // Add procedural if available
    if (CanGenerateProcedurally(category)) {
        sources.push_back("procedural");
    }
    
    // Add marketplace recommendations
    std::string marketplace = DetermineMarketplace(category);
    if (!marketplace.empty()) {
        sources.push_back("marketplace:" + marketplace);
    }
    
    return sources;
}

// --- Intent Handlers ---

AIResponse AIAssetDecisionFramework::HandleAnalyzeIntent(const AIRequest& request) {
    AIResponse response;
    response.requestID = request.requestID;
    
    auto decisions = AnalyzeProject();
    
    if (decisions.empty()) {
        response.success = true;
        response.result = "No missing assets detected";
        response.confidence = 1.0;
    } else {
        response.success = true;
        
        std::ostringstream oss;
        oss << "Found " << decisions.size() << " asset suggestions:\n";
        for (const auto& decision : decisions) {
            oss << "- " << decision.suggestion.name 
                << " (" << ProjectContext::CategoryToString(decision.suggestion.category) << ")"
                << " via " << decision.suggestion.source << "\n";
            response.suggestions.push_back(decision.suggestion.name);
        }
        response.result = oss.str();
        response.confidence = 0.85;
    }
    
    return response;
}

AIResponse AIAssetDecisionFramework::HandleSuggestIntent(const AIRequest& request) {
    AIResponse response;
    response.requestID = request.requestID;
    
    // Check for category parameter
    auto it = request.parameters.find("category");
    if (it == request.parameters.end()) {
        response.success = false;
        response.errorMessage = "Missing 'category' parameter";
        return response;
    }
    
    AssetCategory category = ProjectContext::StringToCategory(it->second);
    auto decisions = AnalyzeCategory(category);
    
    if (decisions.empty()) {
        response.success = true;
        response.result = "No suggestions for category: " + it->second;
    } else {
        response.success = true;
        std::ostringstream oss;
        oss << "Suggestions for " << it->second << ":\n";
        for (const auto& decision : decisions) {
            oss << "- " << decision.suggestion.name 
                << " (confidence: " << (decision.confidence * 100) << "%)\n";
            response.suggestions.push_back(decision.suggestion.name);
        }
        response.result = oss.str();
        response.confidence = decisions.empty() ? 0.5 : decisions[0].confidence;
    }
    
    return response;
}

AIResponse AIAssetDecisionFramework::HandleFetchIntent(const AIRequest& request) {
    AIResponse response;
    response.requestID = request.requestID;
    
    // Check for required parameters
    auto marketplaceIt = request.parameters.find("marketplace");
    auto assetIdIt = request.parameters.find("assetId");
    
    if (marketplaceIt == request.parameters.end() || assetIdIt == request.parameters.end()) {
        response.success = false;
        response.errorMessage = "Missing 'marketplace' or 'assetId' parameter";
        return response;
    }
    
    if (!m_marketplaceRegistry) {
        response.success = false;
        response.errorMessage = "Marketplace registry not configured";
        return response;
    }
    
    // Create a decision for the fetch
    AssetDecision decision;
    decision.shouldFetch = true;
    decision.marketplace = marketplaceIt->second;
    decision.assetId = assetIdIt->second;
    
    if (ExecuteDecision(decision)) {
        response.success = true;
        response.result = "Asset fetched successfully: " + assetIdIt->second;
        response.confidence = 1.0;
    } else {
        response.success = false;
        response.errorMessage = "Failed to fetch asset: " + assetIdIt->second;
    }
    
    return response;
}

// --- Statistics ---

uint32_t AIAssetDecisionFramework::GetDecisionCount() const {
    return m_decisionCount;
}

uint32_t AIAssetDecisionFramework::GetExecutedCount() const {
    return m_executedCount;
}

void AIAssetDecisionFramework::ClearStatistics() {
    m_decisionCount = 0;
    m_executedCount = 0;
}

// --- Private Methods ---

AssetDecision AIAssetDecisionFramework::CreateDecision(const AssetSuggestion& suggestion) const {
    AssetDecision decision;
    decision.suggestion = suggestion;
    decision.confidence = CalculateConfidence(suggestion);
    
    // Determine action based on source and config
    if (suggestion.source == "procedural" || 
        (m_config.preferProcedural && CanGenerateProcedurally(suggestion.category))) {
        decision.shouldGenerate = true;
        decision.reasoning = "Procedural generation available for " + 
                            std::string(ProjectContext::CategoryToString(suggestion.category));
    } else if (suggestion.source.find("marketplace:") == 0) {
        decision.shouldFetch = true;
        decision.marketplace = suggestion.source.substr(12); // Remove "marketplace:" prefix
        decision.reasoning = "Fetching from " + decision.marketplace + " marketplace";
    } else {
        decision.shouldGenerate = true;
        decision.reasoning = "Default to procedural generation";
    }
    
    return decision;
}

std::string AIAssetDecisionFramework::DetermineMarketplace(AssetCategory category) const {
    switch (category) {
        case AssetCategory::Mesh3D:
        case AssetCategory::Character:
        case AssetCategory::NPC:
        case AssetCategory::Environment:
            return "sketchfab";
            
        case AssetCategory::Mesh2D:
        case AssetCategory::Texture:
        case AssetCategory::Audio:
        case AssetCategory::Music:
            return "itch.io";
            
        default:
            return "itch.io";
    }
}

bool AIAssetDecisionFramework::CanGenerateProcedurally(AssetCategory category) const {
    switch (category) {
        case AssetCategory::Terrain:
        case AssetCategory::Texture:
        case AssetCategory::VFX:
            return true;
            
        case AssetCategory::Mesh3D:
        case AssetCategory::Mesh2D:
        case AssetCategory::Character:
        case AssetCategory::NPC:
        case AssetCategory::Environment:
        case AssetCategory::Prop:
        case AssetCategory::Audio:
        case AssetCategory::Music:
        case AssetCategory::Animation:
            return false;
            
        default:
            return false;
    }
}

double AIAssetDecisionFramework::CalculateConfidence(const AssetSuggestion& suggestion) const {
    double confidence = suggestion.confidence;
    
    // Boost confidence for high-priority items
    if (suggestion.priority >= 3) {
        confidence = std::min(1.0, confidence + HIGH_PRIORITY_CONFIDENCE_BOOST);
    }
    
    // Reduce confidence if marketplace is not available
    if (suggestion.source.find("marketplace:") == 0 && !m_marketplaceRegistry) {
        confidence *= UNAVAILABLE_MARKETPLACE_PENALTY;
    }
    
    return confidence;
}

} // namespace atlas::ai
