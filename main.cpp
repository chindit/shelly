#include <iostream>
#include <fmt/format.h>
#include <pqxx/pqxx>
#include <pwd.h>
#include <unistd.h>

#include "Config.h"
#include "Model/DeviceInfo.h"
#include "DeviceFetcher.h"
#include "Cache.h"

int main() {
    // Get home directory
    std::string homeDirectory = getpwuid(getuid())->pw_dir;
    Config configuration = Config(homeDirectory);

    // List of all devices
    std::string deviceList = configuration.get("devices") + ",";

    // Delimiter for devices (CSV line
    std::string delimiter = ",";
    // Pointer for CSV device iteration
    size_t pos = 0;
    // Variables used to retrieve device infos
    std::string token, url, result, body, lines, sqlLines;
    // Headers (empty for info retrieval)
    std::vector<std::pair<std::string, std::string>> curlHeaders;
    // Whether device is IP or not
    bool isIp = true, response = false;
    std::vector<DeviceInfo> devices;

    std::cout << "Starting data retrieving for plugs" << std::endl;

    Cache cache = Cache(homeDirectory + "/.cache/shelly/cache.json");
    DeviceFetcher fetcher = DeviceFetcher(configuration.get("shelly_server"), configuration.get("shelly_key"), cache);

    // Fetch plugs
    while ((pos = deviceList.find(delimiter)) != std::string::npos) {
        std::string device = deviceList.substr(0, pos);
        deviceList.erase(0, pos + delimiter.length());

        if (cache.exists(device) && !cache.needSync(device)) {
            std::cout << "Cache detected for device " << device << std::endl;
            continue;
        }
        auto infos = fetcher.fetch(device);
        if (infos.type == DeviceType::FAILED) {
            std::cout << "Unable to retrieve data for device " << device << std::endl;
            continue;
        }
        if (infos.type == DeviceType::SKIPPED) {
            std::cout << "Sensor " << device << " was not updated." << std::endl;
            continue;
        }

        devices.push_back(infos);
        cache.put(device, infos);

        std::cout << "DeviceFetcher checked" << std::endl;
    }

    cache.write();

    // Prepare SQL Inserts
    for(const DeviceInfo& infos : devices) {
        sqlLines += fmt::format("('{}', '{}', '{:.2f}', '{:.2f}', '{}', '{}'),",
                                infos.stringType,
                                infos.id,
                                infos.power,
                                infos.temperature,
                                infos.total,
                                infos.data
        );
    }

    // Sending data to PostgreSQL
    sqlLines.pop_back();
    pqxx::connection c{configuration.get("psql_uri")};
    pqxx::work txn{c};
    txn.exec0("INSERT INTO shelly (type, device_id, power, temperature, total, data) VALUES " + sqlLines);
    txn.commit();

    std::cout << "Exiting, bye!" << std::endl;

    return 1;
}
