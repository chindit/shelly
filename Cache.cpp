//
// Created by david on 19/12/22.
//

#include <ctime>
#include <utility>
#include "Cache.h"

Cache::Cache(const std::string& path) {
    std::ifstream ifs;
    ifs.open(path);

    Json::CharReaderBuilder builder;
    JSONCPP_STRING errs;
    parseFromStream(builder, ifs, &this->cache, &errs);
    this->path = path;
}

bool Cache::exists(const std::string& id) {
    return (bool)(this->cache[Cache::tolower(id)]);
}

bool Cache::needSync(const std::string &id) {
    if (this->cache[Cache::tolower(id)]["type"].asInt() == SENSOR) {
        return this->cache[Cache::tolower(id)]["lastSync"].asInt() + (5*60) < std::time(nullptr);
    }

    return true;
}

bool Cache::wasUpdated(const std::string &id, int timestamp) {
    return this->cache[Cache::tolower(id)]["lastSync"].asInt() < timestamp;
}

void Cache::put(std::string deviceId, const DeviceInfo& device) {
    std::string lowecaseId = Cache::tolower(std::move(deviceId));
    this->cache[lowecaseId] = Json::Value();
    this->cache[lowecaseId]["type"] = device.type;
    this->cache[lowecaseId]["lastSync"] = std::time(nullptr);
}

void Cache::write() {
    std::filesystem::path cachePath(this->path);
    if (!std::filesystem::exists(cachePath.parent_path())) {
        std::filesystem::create_directory(cachePath.parent_path());
    }

    std::ofstream file_id;
    file_id.open(path);

    Json::StyledWriter styledWriter;
    file_id << styledWriter.write(this->cache);

    file_id.close();
}

std::string Cache::tolower(std::string data) {
    std::transform(data.begin(), data.end(), data.begin(),
                   [](unsigned char c){ return std::tolower(c); });

    return data;
}
