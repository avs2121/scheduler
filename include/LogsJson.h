#pragma once
#include <filesystem>

#include "nlohmann/json.hpp"
using json = nlohmann::json;

void setLogDirectory(const std::filesystem::path& dir);
void ensureLogDirExists();
std::filesystem::path makeLogPath(const std::string& filename);
std::string extensionJSON(const std::string& filename);
void appendToJSON_array(const std::string& filename, const json& entry);
void appendToJSON_object(const std::string& filename, const json& entry);
void createJSON(const std::string& filename);