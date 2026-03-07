# Priority Scheduler

A configurable CPU scheduler simulator supporting multiple scheduling algorithms with I/O handling. 
Supports analytics handeling based on specific metrics. 

## Building
```bash
make              # Build everything
make test         # Run tests
make run          # Run scheduler
make clean-logs   # Remove log-files
make help         # Show list of targets
```

## Configuration

Edit `config/process_config.json` to define processes and scheduler parameters.
Scheduling algorithms: 
- Priority Round-Robin = `rr` or `RR`
- Priority Shortest Job First = `sjf` or `SJF`
- First Come First Served = `fcfs` or `FCFS`
- Multi Level Feedback Queue < Not Yet Implemented >

Scheduler Parameters: 
- Time Quantum Length 
- Max Priority Level
- Aging Threshold 
- Simulated Context Switch Time 

Process Definition: 
- Burst time
- Priority 
- I/O Specifics 
- and many more.. 


## Features

- Context Switching Overhead tracking
- Priority-based scheduling with aging
- I/O-bound process handling
- JSON configuration
- Performance metrics tracking
- Strategy Pattern for easy algortihm comparison 

## Output
Results are written to logs/:
- proces_logs.json - Event-by-event execution log
- proces_logs_metrics.json - Performance metrics summary
