//
// Created by david on 19/12/22.
//

#ifndef SHELLY_DEVICEFETCHER_H
#define SHELLY_DEVICEFETCHER_H

#include <string>
#include <jsoncpp/json/json.h>

#include "Request.h"
#include "Model/DeviceInfo.h"
#include "Cache.h"

class DeviceFetcher {
public:
    DeviceFetcher(std::string serverUrl, std::string token, Cache& cache);
    DeviceInfo fetch(std::string address);

private:
    std::string shellyUrl;
    std::string shellyToken;
    Cache* cache;
};


#endif //SHELLY_DEVICEFETCHER_H
