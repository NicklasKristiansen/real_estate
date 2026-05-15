#pragma once
#include <curl/curl.h>
#include <nlohmann/json.hpp>

#include <string>

namespace landing {

struct ApiEndpoint {
    std::string provider;
    std::string endpoint;
};


nlohmann::json json_from_file(std::string file_path);

std::string request(CURL* curl, const landing::ApiEndpoint& endpoint);
}