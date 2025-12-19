#pragma once
#include <fstream>
#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <unordered_map>
#include <utility>

#include "nlohmann/json.hpp"
using json = nlohmann::json;

void appendToJSON(const std::string &filename, const json &entry);
void createJSON(const std::string &filename);