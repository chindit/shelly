//
// Created by david on 19/12/22.
//

#ifndef SHELLY_CACHEDDEVICE_H
#define SHELLY_CACHEDDEVICE_H


#include <string>
#include "../enums.h"

class CachedDevice {
    CachedDevice(std::string id, DeviceType type, int lastSync);

    std::string id;
    DeviceType type;
    int lastSync;
};


#endif //SHELLY_CACHEDDEVICE_H
