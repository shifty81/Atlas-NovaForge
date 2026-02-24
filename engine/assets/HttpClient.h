#pragma once
// ============================================================
// Atlas HTTP Client — Platform-Agnostic Network Interface
// ============================================================
//
// Defines an abstract HTTP client interface that marketplace
// importers use to download assets from remote APIs.
//
// Implementations can use libcurl, platform APIs, or mock
// backends for testing.

#include <cstdint>
#include <string>
#include <vector>
#include <functional>

namespace atlas::asset {

/// HTTP response from a remote request
struct HttpResponse {
    int32_t statusCode = 0;
    std::string body;
    std::vector<uint8_t> binaryBody;
    std::string contentType;
    std::string errorMessage;
    
    bool IsSuccess() const { return statusCode >= 200 && statusCode < 300; }
    bool IsError() const { return statusCode == 0 || statusCode >= 400; }
};

/// Abstract HTTP client for marketplace API calls.
/// Implementations can use libcurl, platform APIs, or mock backends.
class IHttpClient {
public:
    virtual ~IHttpClient() = default;
    
    /// Perform a GET request and return the response
    virtual HttpResponse Get(
        const std::string& url,
        const std::vector<std::pair<std::string, std::string>>& headers = {}) = 0;
    
    /// Perform a POST request with a body and return the response
    virtual HttpResponse Post(
        const std::string& url,
        const std::string& body,
        const std::vector<std::pair<std::string, std::string>>& headers = {}) = 0;

    /// Download a file from a URL to a local path
    virtual HttpResponse DownloadFile(
        const std::string& url,
        const std::string& outputPath,
        const std::vector<std::pair<std::string, std::string>>& headers = {}) = 0;
};

/// Null HTTP client that always returns errors.
/// Used when no network backend is configured.
class NullHttpClient : public IHttpClient {
public:
    HttpResponse Get(
        const std::string& url,
        const std::vector<std::pair<std::string, std::string>>& headers = {}) override;

    HttpResponse Post(
        const std::string& url,
        const std::string& body,
        const std::vector<std::pair<std::string, std::string>>& headers = {}) override;

    HttpResponse DownloadFile(
        const std::string& url,
        const std::string& outputPath,
        const std::vector<std::pair<std::string, std::string>>& headers = {}) override;
};

}  // namespace atlas::asset
