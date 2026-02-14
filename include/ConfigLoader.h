#pragma once
#include <string>
#include <vector>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

/* ScheduleConfig struct
    Values can be provided for specific implementation. If not provided, will default to pre-set values.
    -> Therefore not requirement to provide these.
*/
struct SchedulerConfig
{
    std::optional<int> time_quantum;
    std::optional<int> max_priority;
    std::optional<int> aging_threshold;
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
    ConfigLoader(const std::string& config_file);

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