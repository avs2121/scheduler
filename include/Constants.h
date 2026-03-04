#pragma once
#include <string>

// Default Scheduler Setup
static constexpr int DEFAULT_TIME_QUANTUM = 4;
static constexpr int DEFAULT_AGING_THRESHOLD = 5;
static constexpr int DEFAULT_MAX_PRIORITY = 3;
static constexpr int DEFAULT_CONTEXT_SWITCH_TIME = 1;
static constexpr std::string DEFAULT_ALGORITHM = "rr";

// Max amount of processes
static constexpr int MAX_PROCESS_SIZE = 100;
