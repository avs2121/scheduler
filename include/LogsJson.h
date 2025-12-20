#pragma once

#include "nlohmann/json.hpp"
using json = nlohmann::json;

std::string extensionJSON(const std::string &filename);
void appendToJSON(const std::string &filename, const json &entry);
void createJSON(const std::string &filename);