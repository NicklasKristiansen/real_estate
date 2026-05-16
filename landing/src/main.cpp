#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <fstream>

#include <curl/curl.h>
#include "landing/requests.hpp"

using json = nlohmann::json;





int main() {
    curl_global_init(CURL_GLOBAL_DEFAULT);

    landing::ApiEndpoint endpoint{"boliga", "one_url", landing::Parameter{"id", "1541123521"}};

    auto cfg = std::make_shared<const json>(landing::load_config());

    landing::Request request(endpoint, cfg);

    std::cout << request.url() << "\n\n\n";

    std::cout << request.to_json().at("bbr").at("unitId").get<std::string>() << "\n";

    curl_global_cleanup();
    return 0;
}