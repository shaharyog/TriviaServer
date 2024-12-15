#pragma once

#include <string>
#include <fstream>
#include <kissnet.hpp>
#include "../communicator/endpoint.h"

class ConfigLoader {
public:
    static Endpoint loadConfig(const std::string &config_path);

private:
    static bool isValidIP(const std::string &ip);

    static bool isValidPort(const int &port);
};
