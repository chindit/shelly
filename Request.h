#ifndef SHELLY_REQUEST_H
#define SHELLY_REQUEST_H

#include <curl/curl.h>
#include <iostream>
#include <string>
#include <vector>

class Request {
public:
    bool call(const std::string &url, const std::vector<std::pair<std::string, std::string>> &headers, const std::string &payload, std::string &resultBody);
private:
    static size_t write_data(void* ptr, size_t size, size_t nmemb, void* buffer);
};


#endif //SHELLY_REQUEST_H
