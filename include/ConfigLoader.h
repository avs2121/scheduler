#pragma once
#include <filesystem>
#include <string>
#include <vector>

#include "PCB.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

// ScheduleConfig struct

struct SchedulerConfig
{
    int time_quantum;
    int max_priority;
    int aging_threshold;
};

// Process config struct

struct ProcessConfig
{
    int pid;
    int priority;
    int burst;
    bool io_bound;
    int io_interval;
};

class ConfigLoader
{
   public:
    ConfigLoader(std::string& config_file);

    std::vector<ProcessConfig> getProcessConfig() const;
    SchedulerConfig getSchedulerConfig() const;
    bool validate() const;

   private:
    json config_data;
    std::string config_file;

    void loadFromFile();
    void validateSchedulerConfig() const;
    void validateProcessConfig() const;
};