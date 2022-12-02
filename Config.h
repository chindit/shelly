//
// Created by david on 1/12/22.
//

#ifndef SHELLY_CONFIG_H
#define SHELLY_CONFIG_H

#include <fstream>
#include <iostream>
#include <string>
#include <vector>


class Config {
public:
    Config();
    std::string get(const char* name);
private:
    std::vector<std::pair<std::string, std::string>> *settings;
};


#endif //SHELLY_CONFIG_H
