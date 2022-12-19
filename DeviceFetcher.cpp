//
// Created by david on 19/12/22.
//

#include "DeviceFetcher.h"

#include <utility>

DeviceInfo DeviceFetcher::fetch(std::string address) {
    auto *reqwest = new Request();

    std::string result, url, body = "";
    // Headers (empty for info retrieval)
    std::vector<std::pair<std::string, std::string>> curlHeaders;

    bool isIp = count(address.begin(), address.end(), '.') == 3;
    if (isIp) {
        url = address + "/status";
    } else {
        url = this->shellyUrl + "/device/status?id=" + address;
        body = "auth_key=" + this->shellyToken;
    }

    std::cout << "Checkin device " << address << std::endl;

    bool response = reqwest->call(url, curlHeaders, body, result);
    delete reqwest;

    if (!response || result.length() == 0) {
        std::cout << "Unable to retrieve data for device " << address << std::endl;
        return {FAILED, std::string("")};
    }

    Json::Value jsonData;
    Json::CharReaderBuilder builder;
    JSONCPP_STRING jsonErrorMessage;
    const auto rawJsonLength = static_cast<int>(result.length());
    const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
    if (!reader->parse(result.c_str(), result.c_str() + rawJsonLength, &jsonData,
                       &jsonErrorMessage)) {
        std::cout << "error" << std::endl;
        return {FAILED, std::string("")};
    }

    if (!isIp) {
        jsonData = jsonData["data"]["device_status"];
    }

    Json::Value rootJson;

    DeviceType type = (jsonData["meters"]) ? DeviceType::PLUG : DeviceType::SENSOR;

    if (type == SENSOR) {
        if (!this->cache->wasUpdated(address, jsonData["sys"]["unixtime"].asInt())) {
            return {SKIPPED, std::string("")};
        }
        rootJson["temperature"] = jsonData["temperature:0"]["tC"];
        rootJson["humidity"] = jsonData["humidity:0"]["rh"];
        Json::Value battery;
        battery["voltage"] = jsonData["devicepower:0"]["battery"]["V"];
        battery["percent"] = jsonData["devicepower:0"]["battery"]["percent"];

        rootJson["battery"] = battery;
    }

    return {
            type,
            (type == SENSOR ? jsonData["sys"]["mac"] : jsonData["mac"]).asString(),
            static_cast<float>(type == PLUG ? jsonData["meters"][0]["power"].asFloat() : 0.0),
            static_cast<float>(type == PLUG ? jsonData["temperature"].asFloat() : 0.0),
            type == PLUG ? jsonData["meters"][0]["total"].asInt() : 0,
            type == PLUG ? "{}" : rootJson.toStyledString()
            };
}

DeviceFetcher::DeviceFetcher(std::string serverUrl, std::string token, Cache &cache) {
    this->shellyUrl = std::move(serverUrl);
    this->shellyToken = std::move(token);
    this->cache = &cache;
}
