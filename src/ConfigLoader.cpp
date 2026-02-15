#include "ConfigLoader.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <set>

#include "LogsJson.h"

static std::filesystem::path LOG_DIR = "logs";

ConfigLoader::ConfigLoader(const std::string& config_file) : config_file(config_file)
{
    loadFromFile();
    validate();
}

void ConfigLoader::loadFromFile()
{
    std::ifstream file(LOG_DIR / extensionJSON(config_file));
    if (!file.is_open())
    {
        throw std::runtime_error("Error opening file: " + config_file);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    config_data = json::parse(buffer.str());
}

std::vector<ProcessConfig> ConfigLoader::getProcessConfig() const
{
    std::vector<ProcessConfig> vector_conf;
    auto sched = config_data["processes"];
    for (const auto& i : sched)
    {
        ProcessConfig proc;
        proc.pid = i["pid"];
        proc.priority = i["priority"];
        proc.burst = i["burst_time"];
        proc.io_bound = i["io_bound"];
        proc.io_interval = i["io_interval"];
        vector_conf.push_back(proc);
    }

    return vector_conf;
}

const SchedulerConfig& ConfigLoader::getSchedulerConfig() const
{
    return sched_conf;
}

void ConfigLoader::validate()
{
    validateSchedulerConfig();
    validateProcessConfig();
}

// validate the required scheduler config numbers are inside the demands
void ConfigLoader::validateSchedulerConfig()
{
    if (!config_data.contains("scheduler_config"))
    {
        throw std::runtime_error("Error in scheduler config - missing scheduler_config");
    }

    auto sched = config_data["scheduler_config"];

    if (sched.contains("time_quantum"))
    {
        int val = sched["time_quantum"].get<int>();
        if (val <= 0)
        {
            throw std::runtime_error("Error in time_quantum in scheduler config");
        }
        sched_conf.time_quantum = val;
    }
    else
    {
        sched_conf.time_quantum = DEFAULT_TIME_QUANTUM;
    }

    if (sched.contains("max_priority"))
    {
        int val = sched["max_priority"].get<int>();
        if (val > 10 || val <= 0)
        {
            throw std::runtime_error("Error in max_priority in scheduler config");
        }
        sched_conf.max_priority = val;
    }
    else
    {
        sched_conf.max_priority = DEFAULT_MAX_PRIORITY;
    }

    if (sched.contains("aging_threshold"))
    {
        int val = sched["aging_threshold"].get<int>();
        if (val <= 0)
        {
            throw std::runtime_error("Error in aging_threshold in scheduler config");
        }
        sched_conf.aging_threshold = val;
    }
    else
    {
        sched_conf.aging_threshold = DEFAULT_AGING_THRESHOLD;
    }
}

// validate process config for e.g no duplicate pids
void ConfigLoader::validateProcessConfig() const
{
    if (!config_data.contains("processes"))
    {
        throw std::runtime_error("Error in process config - missing process array");
    }

    if (config_data["processes"].empty())
    {
        throw std::runtime_error("Error in process config - empty process array");
    }

    std::vector<ProcessConfig> vector_conf = getProcessConfig();
    auto sched = config_data["processes"];

    std::set<int> processSet;

    /* lidt lambda teknik, for sjov.

    - I realiteten behøves kun nedenstående, fordi set ikke kan have duplicates, og dernæst kan size
    tjekkes: for(auto& p : vector_conf){ processSet.insert(p.pid);
    }

    */
    auto it = std::find_if(std::begin(vector_conf),
                           std::end(vector_conf),
                           [&processSet](const ProcessConfig& p)
                           {
                               if (processSet.find(p.pid) != processSet.end())
                                   return true;

                               else
                                   processSet.insert(p.pid);
                               return false;
                           });

    if (it != vector_conf.end())
    {
        throw std::runtime_error("Duplicate PID found: " + std::to_string(it->pid));
    }
}