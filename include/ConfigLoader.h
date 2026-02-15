#pragma once
#include <string>
#include <vector>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

static constexpr int DEFAULT_TIME_QUANTUM = 4;
static constexpr int DEFAULT_AGING_THRESHOLD = 5;
static constexpr int DEFAULT_MAX_PRIORITY = 3;

/* ScheduleConfig struct
    Values can be provided for specific implementation. If not provided, will default to pre-set
   values.
    -> Therefore not requirement to provide these.
*/
struct SchedulerConfig
{
    int max_priority;
    int aging_threshold;
    int time_quantum;
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
    const SchedulerConfig& getSchedulerConfig() const;

   private:
    json config_data;
    std::string config_file;
    SchedulerConfig sched_conf;

    void validate();
    void loadFromFile();
    void validateSchedulerConfig();
    void validateProcessConfig() const;
};