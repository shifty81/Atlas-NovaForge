#pragma once
// ============================================================
// Atlas LLM Backend — Offline-safe language model interface
// ============================================================
//
// Provides a structured interface for wiring an LLM backend
// (local or remote) into AtlasAI. The default implementation
// is an offline stub that returns deterministic responses
// without network access. A real backend subclass can be
// swapped in at runtime via SetBackend().
//
// See: docs/11_ATLAS_AI.md
//      docs/ATLAS_CORE_CONTRACT.md

#include <cstdint>
#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <unordered_map>

#include "../assets/HttpClient.h"

namespace atlas::ai {

/// Configuration for an LLM request.
struct LLMRequest {
    std::string prompt;
    std::string systemPrompt;
    float temperature = 0.7f;    ///< 0 = deterministic, 1 = creative
    uint32_t maxTokens = 256;
    uint64_t requestId = 0;
};

/// Response from the LLM backend.
struct LLMResponse {
    uint64_t requestId = 0;
    std::string text;
    bool success = false;
    std::string errorMessage;
    uint32_t tokensUsed = 0;
    float latencyMs = 0.0f;
};

/// Backend capability flags.
enum class LLMCapability : uint8_t {
    None          = 0,
    TextGeneration = 1,
    Embeddings     = 2,
    FunctionCall   = 4,
    Streaming      = 8,
};

/// Abstract LLM backend interface.
class ILLMBackend {
public:
    virtual ~ILLMBackend() = default;

    /// Process a single prompt request.
    virtual LLMResponse Complete(const LLMRequest& request) = 0;

    /// Whether the backend is available/healthy.
    virtual bool IsAvailable() const = 0;

    /// Human-readable name of the backend.
    virtual std::string Name() const = 0;

    /// Capability bitfield for this backend.
    virtual uint8_t Capabilities() const = 0;
};

/// Offline stub — returns canned responses keyed by prompt prefix.
/// Used in headless CI and environments with no network access.
class OfflineLLMBackend : public ILLMBackend {
public:
    LLMResponse Complete(const LLMRequest& request) override;
    bool IsAvailable() const override;
    std::string Name() const override;
    uint8_t Capabilities() const override;

    /// Register a canned response for prompts that start with `prefix`.
    void RegisterResponse(const std::string& prefix, const std::string& response);

    /// Number of registered canned responses.
    size_t ResponseCount() const;

    /// Clear all canned responses.
    void ClearResponses();

    /// Total number of calls since construction.
    uint64_t CallCount() const;

private:
    std::unordered_map<std::string, std::string> m_responses;
    uint64_t m_callCount = 0;
    uint64_t m_nextRequestId = 1;
};

/// HTTP-based LLM backend that connects to an OpenAI-compatible API.
/// Uses the atlas::asset::IHttpClient interface for network communication.
/// Supports configurable endpoint URL, model name, and API key.
class HttpLLMBackend : public ILLMBackend {
public:
    /// Construct with an HTTP client, API endpoint, and model name.
    /// @param httpClient  Non-owning pointer to an HTTP client (must outlive this backend).
    /// @param endpoint    API endpoint URL (e.g. "https://api.openai.com/v1/chat/completions")
    /// @param model       Model name (e.g. "gpt-4", "gpt-3.5-turbo")
    HttpLLMBackend(atlas::asset::IHttpClient* httpClient,
                   const std::string& endpoint,
                   const std::string& model);

    LLMResponse Complete(const LLMRequest& request) override;
    bool IsAvailable() const override;
    std::string Name() const override;
    uint8_t Capabilities() const override;

    /// Set the API key for authentication.
    void SetApiKey(const std::string& apiKey);

    /// Whether an API key has been configured.
    bool HasApiKey() const;

    /// Returns the configured endpoint URL.
    const std::string& GetEndpoint() const;

    /// Returns the configured model name.
    const std::string& GetModel() const;

    /// Set the request timeout in milliseconds.
    void SetTimeoutMs(uint32_t timeoutMs);

    /// Returns the request timeout in milliseconds.
    uint32_t GetTimeoutMs() const;

    /// Total number of successful API calls.
    uint64_t SuccessCount() const;

    /// Total number of failed API calls.
    uint64_t FailureCount() const;

private:
    atlas::asset::IHttpClient* m_httpClient = nullptr;
    std::string m_endpoint;
    std::string m_model;
    std::string m_apiKey;
    uint32_t m_timeoutMs = 30000;
    uint64_t m_successCount = 0;
    uint64_t m_failureCount = 0;
    uint64_t m_nextRequestId = 1;

    /// Build the JSON request body for the API call.
    std::string BuildRequestBody(const LLMRequest& request) const;

    /// Parse the JSON response from the API.
    LLMResponse ParseResponse(const std::string& responseBody, uint64_t requestId) const;
};

/// Forwards requests to whichever ILLMBackend is currently registered.
/// Falls back to OfflineLLMBackend when no backend is set.
class LLMBackendRegistry {
public:
    /// Replace the active backend. Pass nullptr to revert to offline stub.
    void SetBackend(std::shared_ptr<ILLMBackend> backend);

    /// Active backend (never nullptr — falls back to offline stub).
    ILLMBackend& GetBackend();
    const ILLMBackend& GetBackend() const;

    /// Whether a non-offline backend is currently registered.
    bool HasRealBackend() const;

    /// Send a prompt and return a response via the active backend.
    LLMResponse Complete(const LLMRequest& request);

    /// Convenience: fire-and-forget with a simple string prompt.
    LLMResponse Complete(const std::string& prompt);

    /// History of all completed requests.
    const std::vector<LLMResponse>& ResponseHistory() const;

    /// Number of requests processed since construction.
    size_t RequestCount() const;

    /// Clear response history.
    void ClearHistory();

private:
    std::shared_ptr<ILLMBackend> m_backend;
    OfflineLLMBackend m_offlineStub;
    std::vector<LLMResponse> m_history;
    uint64_t m_nextRequestId = 1;
};

/// Factory that creates and configures an HttpLLMBackend from environment
/// variables or explicit parameters.
///
/// Environment variables (used when Create() is called without arguments):
///   ATLAS_LLM_ENDPOINT  — API endpoint (e.g. "https://api.openai.com/v1/chat/completions")
///   ATLAS_LLM_MODEL     — Model name (e.g. "gpt-4")
///   ATLAS_LLM_API_KEY   — API key
///   ATLAS_LLM_TIMEOUT   — Timeout in milliseconds (optional, default 30000)
class LLMBackendFactory {
public:
    /// Create an HttpLLMBackend from environment variables.
    /// Returns nullptr if required variables are missing.
    static std::shared_ptr<HttpLLMBackend> CreateFromEnv(atlas::asset::IHttpClient* httpClient);

    /// Create an HttpLLMBackend with explicit parameters.
    static std::shared_ptr<HttpLLMBackend> Create(
        atlas::asset::IHttpClient* httpClient,
        const std::string& endpoint,
        const std::string& model,
        const std::string& apiKey,
        uint32_t timeoutMs = 30000);

    /// Check if the required environment variables are set.
    static bool HasEnvConfig();
};

} // namespace atlas::ai
