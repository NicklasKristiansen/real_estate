#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <fstream>

#include <curl/curl.h>
#include "landing/requests.hpp"

using json = nlohmann::json;





int main() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    CURL* curl = curl_easy_init();


    landing::ApiEndpoint endpoint = {"boliga", "sales_history"};

    std::string response = landing::request(curl, endpoint);

    std::cout << response << '\n';

    return 0;
}