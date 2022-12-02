//
// Created by david on 1/12/22.
//

#include "Config.h"

Config::Config() {
    settings = new std::vector<std::pair<std::string, std::string>>;

    std::ifstream configFile("config.conf");

    if (configFile.is_open()) {
        std::string lineContent;
        while ( configFile ) {
            std::getline (configFile, lineContent);

            auto index = lineContent.find('=');
            std::pair<std::string,std::string> keyVal;
            if (index != std::string::npos) {

                // Split around ':' character
                keyVal = std::make_pair(
                        lineContent.substr(0, index),
                        lineContent.substr(index + 1)
                );

                settings->emplace_back(keyVal);
            }
        }
    }
}

std::string Config::get(const char* name) {
    for (auto &it : *settings) {
        if (it.first ==name) {
            return it.second;
        }
    }

    return "";
}
