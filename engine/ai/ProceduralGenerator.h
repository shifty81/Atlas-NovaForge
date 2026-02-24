#pragma once
// ============================================================
// Atlas Procedural Content Generator
// ============================================================
//
// Provides context-aware procedural generation of game content
// using deterministic, seeded template-based logic. Generates:
//
// - NPC / place / item names (culture-aware)
// - Biome-appropriate asset lists
// - Quest templates with objectives and difficulty
// - Shader suggestions per environment type
//
// An optional ILLMBackend can be wired in for richer output;
// the generator always falls back to built-in templates when
// no backend is available.
//
// See: docs/16_ATLAS_AI.md

#include "LLMBackend.h"
#include <cstdint>
#include <string>
#include <vector>

namespace atlas::ai {

// --- Name Generation ---

enum class NameCulture : uint8_t {
    Generic,
    Nordic,
    Eastern,
    Latin,
    SciFi,
    Fantasy
};

struct GeneratedName {
    std::string value;
    NameCulture culture;
    std::string category;   // "npc", "place", "item"
};

// --- Biome Asset Suggestions ---

enum class BiomeType : uint8_t {
    Forest,
    Desert,
    Mountain,
    Ocean,
    Tundra,
    Swamp,
    Plains,
    Volcanic,
    Urban
};

struct BiomeAssetList {
    BiomeType biome;
    std::vector<std::string> meshSuggestions;
    std::vector<std::string> textureSuggestions;
    std::vector<std::string> vfxSuggestions;
    std::vector<std::string> audioSuggestions;
};

// --- Quest Templates ---

enum class QuestType : uint8_t {
    Fetch,
    Combat,
    Exploration,
    Escort,
    Puzzle,
    Delivery
};

struct QuestTemplate {
    QuestType type;
    std::string title;
    std::string description;
    std::vector<std::string> objectives;
    uint32_t estimatedDifficulty = 1;   // 1-5
    std::string rewardDescription;
};

// --- Shader Suggestions ---

struct ShaderSuggestion {
    std::string name;
    std::string description;
    std::vector<std::string> parameters;
    BiomeType targetBiome;
};

// --- Main Generator ---

class ProceduralGenerator {
public:
    ProceduralGenerator();

    // --- Seed ---
    void SetSeed(uint64_t seed);
    uint64_t GetSeed() const;

    // --- LLM Backend (optional) ---
    void SetLLMBackend(ILLMBackend* backend);
    ILLMBackend* GetLLMBackend() const;

    // --- Name Generation ---
    GeneratedName GenerateName(const std::string& category, NameCulture culture);
    std::vector<GeneratedName> GenerateNames(const std::string& category,
                                             NameCulture culture,
                                             uint32_t count);

    // --- Biome Assets ---
    BiomeAssetList GenerateBiomeAssets(BiomeType biome);
    std::vector<BiomeAssetList> GenerateAllBiomeAssets();

    // --- Quest Templates ---
    QuestTemplate GenerateQuest(QuestType type, uint32_t difficulty);
    std::vector<QuestTemplate> GenerateQuestLine(uint32_t count,
                                                  uint32_t startDifficulty);

    // --- Shader Suggestions ---
    std::vector<ShaderSuggestion> SuggestShaders(BiomeType biome);
    ShaderSuggestion SuggestShader(const std::string& environmentType);

    // --- Statistics ---
    uint32_t GetGenerationCount() const;
    void ResetStatistics();

private:
    uint32_t DeterministicRand();
    std::string PickRandom(const std::vector<std::string>& options);

    uint64_t m_seed = 42;
    uint64_t m_rngState = 42;
    ILLMBackend* m_llmBackend = nullptr;
    uint32_t m_generationCount = 0;
};

} // namespace atlas::ai
