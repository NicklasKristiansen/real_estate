#include <landing/requests.hpp>

#include <curl/curl.h>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <nlohmann/json.hpp>
#include <vector>

using json = nlohmann::json;


namespace {

size_t write_callback(char* ptr, size_t size, size_t nmemb, void* userdata) {
    auto* output = static_cast<std::string*>(userdata);
    output->append(ptr, size * nmemb);
    return size * nmemb;
}




void add_headers(CURL* curl, const json &config) {
    std::vector<std::string> default_headers = config["default_headers"];

    std::size_t headers_length = default_headers.size();

    struct curl_slist* headers = nullptr;
    for (std::size_t i = 0; i < headers_length; i++) {
        headers = curl_slist_append(headers, default_headers[i].c_str());
    }

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
}

void setup_curl(CURL* curl, const landing::ApiEndpoint& endpoint, const json& config) {

    
    std::string base_url = config["base_url"].get<std::string>();
    std::string url = base_url.append(config["endpoints"][endpoint.endpoint]["path"].get<std::string>());

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");
    
    add_headers(curl, config);
}



}


namespace landing {

json json_from_file(std::string file_path) {
    std::ifstream file(file_path);

    if (!file) {
        throw std::runtime_error(std::string("Could not open json file: ") + file_path);
    }

    json file_content;

    try {
        file >> file_content;
    } catch (const json::parse_error& e) {
        throw std::runtime_error(
            std::string("JSON parse error in ") + file_path + ": " + e.what() + '\n'
        );
    }

    return file_content;
}


std::string request(CURL* curl, const landing::ApiEndpoint& endpoint) {
    json config = landing::json_from_file("config/requests.json")[endpoint.provider];

    setup_curl(curl, endpoint, config);

    std::string response;

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    std::string impersonate = config["impersonate"].get<std::string>();
    CURLcode impersonate_result = curl_easy_impersonate(curl, impersonate.c_str(), 1);

    if (impersonate_result != CURLE_OK) {
        throw std::runtime_error(
            std::string("curl_easy_impersonate failed: ") +
            curl_easy_strerror(impersonate_result)
        );
    }

    CURLcode result = curl_easy_perform(curl);

    if (result != CURLE_OK) {
        throw std::runtime_error(
            std::string("curl_easy_perform failed: ") +
            curl_easy_strerror(result)
        );
    }

    return response;
}

}