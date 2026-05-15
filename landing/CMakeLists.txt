cmake_minimum_required(VERSION 3.24)

project(landing LANGUAGES CXX)

find_package(nlohmann_json CONFIG REQUIRED)

set(CURL_IMPERSONATE_ROOT
    "${CMAKE_CURRENT_SOURCE_DIR}/.deps/curl-impersonate"
    CACHE PATH "Path to curl-impersonate install prefix"
)

set(CURL_IMPERSONATE_INCLUDE_DIR
    "${CURL_IMPERSONATE_ROOT}/include"
)

set(CURL_IMPERSONATE_LIBRARY
    "${CURL_IMPERSONATE_ROOT}/lib/libcurl-impersonate-chrome.so"
)

if(NOT EXISTS "${CURL_IMPERSONATE_INCLUDE_DIR}/curl/curl.h")
    message(FATAL_ERROR
        "curl-impersonate headers not found. Run ./scripts/setup.sh first."
    )
endif()

if(NOT EXISTS "${CURL_IMPERSONATE_LIBRARY}")
    message(FATAL_ERROR
        "curl-impersonate library not found. Run ./scripts/setup.sh first."
    )
endif()

add_library(curl_impersonate_chrome SHARED IMPORTED)

set_target_properties(curl_impersonate_chrome PROPERTIES
    IMPORTED_LOCATION "${CURL_IMPERSONATE_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${CURL_IMPERSONATE_INCLUDE_DIR}"
)

add_executable(landing
    src/main.cpp
    src/requests.cpp
)

target_compile_features(landing PRIVATE cxx_std_20)

target_include_directories(landing PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(landing PRIVATE
    nlohmann_json::nlohmann_json
    curl_impersonate_chrome
)