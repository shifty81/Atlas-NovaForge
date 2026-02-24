#include "HttpClient.h"

namespace atlas::asset {

HttpResponse NullHttpClient::Get(
    const std::string& url,
    const std::vector<std::pair<std::string, std::string>>& headers) {
    
    HttpResponse response;
    response.statusCode = 0;
    response.errorMessage = "No HTTP backend configured (NullHttpClient)";
    return response;
}

HttpResponse NullHttpClient::Post(
    const std::string& url,
    const std::string& body,
    const std::vector<std::pair<std::string, std::string>>& headers) {

    HttpResponse response;
    response.statusCode = 0;
    response.errorMessage = "No HTTP backend configured (NullHttpClient)";
    return response;
}

HttpResponse NullHttpClient::DownloadFile(
    const std::string& url,
    const std::string& outputPath,
    const std::vector<std::pair<std::string, std::string>>& headers) {
    
    HttpResponse response;
    response.statusCode = 0;
    response.errorMessage = "No HTTP backend configured (NullHttpClient)";
    return response;
}

}  // namespace atlas::asset
