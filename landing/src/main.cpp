#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <fstream>

#include <curl/curl.h>
#include "landing/requests.hpp"

using json = nlohmann::json;





int main() {
    curl_global_init(CURL_GLOBAL_DEFAULT);


    landing::Parameter param{"id", "1541123521"};
    landing::Parameter* param_ptr = &param;

    landing::ApiEndpoint endpoint{"boliga", "one_url", param_ptr};

    landing::Request request(endpoint);

    std::cout << request.get_url() << "\n";

    return 0;
}