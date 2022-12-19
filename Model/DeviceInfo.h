//
// Created by david on 19/12/22.
//

#ifndef SHELLY_DEVICEINFO_H
#define SHELLY_DEVICEINFO_H


#include <string>
#include "../enums.h"

class DeviceInfo {
public:
    DeviceInfo(DeviceType type, std::string id, float power = 0.0, float temperature = 0.0, int total = 0, std::string data = "{}");

    DeviceType type;
    std::string stringType;
    std::string id;
    float power{};
    float temperature{};
    int total{};
    std::string data;
};


#endif //SHELLY_DEVICEINFO_H
