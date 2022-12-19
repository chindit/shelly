//
// Created by david on 19/12/22.
//

#include "CachedDevice.h"

#include <utility>

CachedDevice::CachedDevice(std::string id, DeviceType type, int lastSync) {
    this->id = std::move(id);
    this->type = type;
    this->lastSync = lastSync;
}
