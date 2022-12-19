//
// Created by david on 19/12/22.
//

#ifndef SHELLY_CACHE_H
#define SHELLY_CACHE_H

#include <filesystem>
#include <fstream>
#include <string>
#include <jsoncpp/json/json.h>
#include "Model/DeviceInfo.h"

class Cache {
public:
    explicit Cache(const std::string& path);
    bool exists(const std::string& id);
    bool needSync(const std::string& id);
    bool wasUpdated(const std::string& id, int timestamp);
    void put(std::string deviceId, const DeviceInfo& device);
    void write();

private:
    static std::string tolower(std::string input);

    Json::Value cache;
    std::string path;
};


#endif //SHELLY_CACHE_H
