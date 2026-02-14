# Priority Scheduler

A multilevel feedback queue scheduler with priority aging and I/O handling.

## Building
```bash
make              # Build everything
make test         # Run tests
make run          # Run scheduler
```

## Configuration

Edit `config/process_config.json` to define processes and scheduler parameters.

## Features

- Priority-based scheduling with aging
- I/O-bound process handling
- JSON configuration
- Performance metrics tracking