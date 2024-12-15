#include <iostream>
#include <regex>
#include "configLoader.h"
#include "../../constants.h"
#include "../../log.h"
#include <nlohmann/json.hpp>

using Json = nlohmann::json;

Endpoint ConfigLoader::loadConfig(const std::string &config_path) {

    std::string ip = DEFAULT_IP;
    int port = DEFAULT_PORT;

    logServerProgress<ConfigLoader>( __func__, "Loading config file...");
    std::ifstream file(config_path);

    if (!file.is_open()) {
        logServerResult(false, false, false);
        logServerProgress<ConfigLoader>( __func__, "Using default values: " + ip + ":" + std::to_string(port));
        logServerResult(true);
        return Endpoint(ip, port);
    }
    logServerResult(true);

    Json root;
    try {
        logServerProgress<ConfigLoader>( __func__, "Parsing config file...");
        file >> root;
        logServerResult(true);
    } catch (const Json::parse_error& e) {
        logServerResult(false, false);
        logServerProgress<ConfigLoader>( __func__, "Using default values: " + ip + ":" + std::to_string(port));
        logServerResult(true);
        return Endpoint(ip, port);
    }

    logServerProgress<ConfigLoader>( __func__, "Validating ip address...");
    if (root["ip"].is_string() && isValidIP(root["ip"].get<std::string>()))
    {
        ip = root["ip"].get<std::string>();
        logServerResult(true);
    }
    else {
        logServerResult(false, false);
        logServerProgress<ConfigLoader>( __func__, "Using default ip: " + ip);
        logServerResult(true);
    }

    logServerProgress<ConfigLoader>( __func__, "Validating port number...");
    if (root["port"].is_number_integer() && isValidPort(root["port"].get<int>())) {
        port = root["port"].get<int>();
        logServerResult(true);
    }
    else {
        logServerResult(false, false);
        logServerProgress<ConfigLoader>( __func__, "Using default port: " + std::to_string(port));
        logServerResult(true);
    }

    file.close();

    return Endpoint(ip, port);
}

bool ConfigLoader::isValidIP(const std::string &ip) {
    const std::regex ip_regex(R"(^((25[0-5]|(2[0-4]|1\d|[1-9]|)\d)\.?\b){4}$)");
    return std::regex_match(ip, ip_regex);
}

bool ConfigLoader::isValidPort(const int &port) {
    return port > 0 && port < 65535;
}