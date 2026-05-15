#pragma once
#include <curl/curl.h>
#include <nlohmann/json.hpp>

#include <string>

namespace landing {


struct Parameter {
    std::string key;
    std::string value;
};


struct ApiEndpoint {
    std::string provider;
    std::string endpoint;
    Parameter* parameter = nullptr;
};


nlohmann::json json_from_file(std::string file_path);

std::string request(CURL* curl, const landing::ApiEndpoint& endpoint);


class Request {
    private:
        landing::ApiEndpoint endpoint;
        std::string url;
        nlohmann::json config;


        std::string create_url();


    public:
        Request(landing::ApiEndpoint endpoint);

        std::string get_url();

};



}