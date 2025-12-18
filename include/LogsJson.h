#pragma once
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <utility>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

void appendToJSON(const std::string &filename, const json &entry);
void createJSON(const std::string &filename);