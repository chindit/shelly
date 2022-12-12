#include <iostream>
#include <ctime>
#include <jsoncpp/json/json.h>
#include <fmt/format.h>
#include <pqxx/pqxx>

#include "Request.h"
#include "Config.h"

int main() {
    Config configuration = Config();

    // List of all devices
    std::string devicesList = configuration.get("devices") + ",";
    // Delimiter for devices (CSV line
    std::string delimiter = ",";
    // Pointer for CSV device iteration
    size_t pos = 0;
    // Variables used to retrieve device infos
    std::string token, url, result, body, lines, sqlLines;
    // Headers (empty for info retrieval)
    std::vector<std::pair<std::string, std::string>> curlHeaders;
    // Wether device is IP or not
    bool isIp = true, response = false;

    std::cout << "Starting data retrieving" << std::endl;

    while ((pos = devicesList.find(delimiter)) != std::string::npos) {
        auto *reqwest = new Request();
        std::string device = devicesList.substr(0, pos);
        result = "";
        isIp = count(device.begin(), device.end(), '.') == 3;
        if (isIp) {
            url = device + "/status";
            body = "";
        } else {
            url = configuration.get("shelly_server") + "/device/status?id=" + device;
            body = "auth_key=" + configuration.get("shelly_key");
        }
        devicesList.erase(0, pos + delimiter.length());

        std::cout << "Checkin device " << device << std::endl;

        response = reqwest->call(url, curlHeaders, body, result);
        delete reqwest;

        if (!response || result.length() == 0) {
            std::cout << "Unable to retrieve data for device " << device << std::endl;
            continue;
        }

        Json::Value jsonData;
        Json::CharReaderBuilder builder;
        JSONCPP_STRING jsonErrorMessage;
        const auto rawJsonLength = static_cast<int>(result.length());
        const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
        if (!reader->parse(result.c_str(), result.c_str() + rawJsonLength, &jsonData,
                           &jsonErrorMessage)) {
            std::cout << "error" << std::endl;
            return EXIT_FAILURE;
        }

        if (!isIp) {
            lines += fmt::format("plug,id={} value={:.2f},temperature={:.2f},total={}\n", device,
                                 jsonData["data"]["device_status"]["meters"][0]["power"].asFloat(),
                                 jsonData["data"]["device_status"]["temperature"].asFloat(),
                                 jsonData["data"]["device_status"]["meters"][0]["total"].asInt());
            sqlLines += fmt::format("('plug', '{}', '{:.2f}', '{:.2f}', '{}'),", device,
                                    jsonData["data"]["device_status"]["meters"][0]["power"].asFloat(),
                                    jsonData["data"]["device_status"]["temperature"].asFloat(),
                                    jsonData["data"]["device_status"]["meters"][0]["total"].asInt());
        } else {
            lines += fmt::format("plug,id={} value={:.2f},temperature={:.2f},total={}\n", jsonData["mac"].asString(),
                                 jsonData["meters"][0]["power"].asFloat(), jsonData["temperature"].asFloat(),
                                 jsonData["meters"][0]["total"].asInt());
            sqlLines += fmt::format("('plug', '{}', '{:.2f}', '{:.2f}', '{}'),", jsonData["mac"].asString(),
                                       jsonData["meters"][0]["power"].asFloat(), jsonData["temperature"].asFloat(),
                                       jsonData["meters"][0]["total"].asInt());
        }

        std::cout << "Device checked" << std::endl;
    }

    // Sending data to PostgreSQL
    sqlLines.pop_back();
    pqxx::connection c{configuration.get("psql_uri")};
    pqxx::work txn{c};
    txn.exec0("INSERT INTO shelly (type, device_id, power, temperature, total) VALUES " + sqlLines);
    txn.commit();

    // Sending data to InfluxDB
    if (configuration.get("influx_token").length() > 0) {
        auto *reqwest = new Request();
        curlHeaders.emplace_back("Authorization", "Token " + configuration.get("influx_token"));
        curlHeaders.emplace_back("Content-Type", "text/plain; charset=utf-8");
        curlHeaders.emplace_back("Accept", "application/json");
        response = reqwest->call(
                configuration.get("influx_url") + "/api/v2/write?org=" + configuration.get("influx_org") + "&bucket=" +
                configuration.get("influx_bucket"), curlHeaders, lines, result);

        if (response) {
            std::cout << "Data successfully sent" << std::endl;
        } else {
            std::cout << "Error while sending data" << std::endl;
        }
    }

    std::cout << "Exiting, bye!" << std::endl;

    return 1;
}
