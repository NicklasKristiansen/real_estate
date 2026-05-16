#pragma once
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <memory>
#include <string>
#include <optional>

namespace landing {


struct Parameter {
    std::string key;
    std::string value;
};


struct ApiEndpoint {
    std::string provider;
    std::string endpoint;
    std::optional<Parameter> parameter = std::nullopt;
};

struct CurlSlistDeleter {
    void operator()(curl_slist* list) const {
        if (list) {
            curl_slist_free_all(list);
        }
    }
};


struct CurlDeleter {
    void operator()(CURL* curl) const {
        if (curl) {
            curl_easy_cleanup(curl);
        }
    }
};

nlohmann::json json_from_file(std::string file_path);
nlohmann::json load_config();

class Request {
    private:
        std::unique_ptr<CURL, CurlDeleter> curl;
        landing::ApiEndpoint endpoint;
        std::shared_ptr<const nlohmann::json> cfg;
        std::unique_ptr<curl_slist, CurlSlistDeleter> headers;
        std::string url_;
        std::string response;
        long status_code_ = 0;


    public:
        Request(landing::ApiEndpoint endpoint, std::shared_ptr<const nlohmann::json> config);

        std::string url() const;
        std::string text() const;
        nlohmann::json to_json() const;
        long status_code() const;
        void raise_for_status() const;
};



}