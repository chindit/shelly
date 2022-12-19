//
// Created by david on 19/12/22.
//

#include "DeviceInfo.h"

#include <utility>

DeviceInfo::DeviceInfo(DeviceType type, std::string id, float power, float temperature, int total, std::string data) {
    this->type = type;
    this->stringType = type == SENSOR ? "sensor" : "plug";
    this->id = std::move(id);
    this->power = power;
    this->temperature = temperature;
    this->total = total;
    this->data = std::move(data);
}
