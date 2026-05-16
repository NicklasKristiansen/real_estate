#include <landing/requests.hpp>

#include <curl/curl.h>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <nlohmann/json.hpp>
#include <vector>
#include <memory>
#include <utility>
#include <optional>

using json = nlohmann::json;


namespace {
// =============================================================================
// Curl helpers
// =============================================================================


size_t write_callback(char* ptr, size_t size, size_t nmemb, void* userdata) {
    auto* output = static_cast<std::string*>(userdata);
    output->append(ptr, size * nmemb);
    return size * nmemb;
}

void substitute_parameter(std::string& api_endpoint, const std::optional<landing::Parameter>& param) {
    std::string key = "{" + (*param).key + "}";
    std::size_t pos = api_endpoint.find(key);

    if (pos == std::string::npos) {
        throw std::runtime_error(std::string("The api endpoint '" + api_endpoint + "'" + "does not contain the parameter key: '" + key + "'"));
    }

    api_endpoint.replace(pos, key.length(), (*param).value);
}

std::unique_ptr<curl_slist, landing::CurlSlistDeleter>
add_headers(CURL* curl, const std::vector<std::string>& header_strings) {
    std::size_t length = header_strings.size();

    struct curl_slist* curl_headers = nullptr;
    for (std::size_t i = 0; i < length; i++) {
        curl_headers = curl_slist_append(curl_headers, header_strings[i].c_str());
    }

    auto curl_slist_ptr = std::unique_ptr<curl_slist, landing::CurlSlistDeleter>(curl_headers);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curl_slist_ptr.get());

    return curl_slist_ptr;
}

// creates the complete url from a ApiEndpoint
std::string create_url(const landing::ApiEndpoint& endpoint, std::shared_ptr<const json> config) {
    std::string base_url = config->at(endpoint.provider).at("base_url").get<std::string>();
    std::string api_endpoint = 
        config->at(endpoint.provider)
        .at("endpoints")
        .at(endpoint.endpoint)
        .at("path")
        .get<std::string>();

    if (endpoint.parameter) {
        substitute_parameter(api_endpoint, endpoint.parameter);
    }

    return base_url + api_endpoint;
}

// prepares curl by giving it headers, full url, callback function and setting up impersonate.
//
// the function takes a curl pointer, ApiEndpoint, config and the refrence to a response string
// that curl can write the response to the function returns the complete url to the endpoint.
std::string setup_curl(CURL* curl, const landing::ApiEndpoint& endpoint, std::shared_ptr<const json> config, std::string& response) {
    std::string url = create_url(endpoint, config);
    
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    std::string impersonate = config->at(endpoint.provider).at("impersonate").get<std::string>();
    CURLcode impersonate_result = curl_easy_impersonate(curl, impersonate.c_str(), 1);

    if (impersonate_result != CURLE_OK) {
        throw std::runtime_error(
            std::string("curl_easy_impersonate failed: ") + curl_easy_strerror(impersonate_result)
        );
    }

    return url;
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

json load_config() {
    return json_from_file("config/requests.json");
}


// =============================================================================
// Request
// =============================================================================

Request::Request(landing::ApiEndpoint endpoint, std::shared_ptr<const json> config)
    : curl(curl_easy_init()), endpoint(std::move(endpoint)), cfg(config){

        if (curl == NULL) {
            throw std::runtime_error(std::string("Could not initialize a curl instance when creating Request."));
        }

        std::vector<std::string> default_headers = config->at(this->endpoint.provider).at("default_headers");
        headers = add_headers(curl.get(), default_headers);


        url_ = setup_curl(curl.get(), this->endpoint, cfg, response);
        
        CURLcode result = curl_easy_perform(curl.get());

        curl_easy_getinfo(curl.get(), CURLINFO_RESPONSE_CODE, &status_code_);


        if (result != CURLE_OK) {
           throw std::runtime_error(
                std::string("curl_easy_perform failed: ") + curl_easy_strerror(result)
            );
        }


}

std::string Request::url() const {
    return url_;
}

std::string Request::text() const {
    return response;
}

json Request::to_json() const {
    return json::parse(response);
}

long Request::status_code() const {
    return status_code_;
}

void Request::raise_for_status() const {
    if (status_code_ < 200 || status_code_ >= 300) {
        throw std::runtime_error(
            "Unexpected HTTP status " + std::to_string(status_code_) +
            ": " + text()
        );
    }
}

}