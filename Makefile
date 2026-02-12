CXX = g++
CXXFLAGS = -g -std=c++20 -Wall -Iinclude

SRC_PATH = src
INC_PATH = include
TEST_PATH = tests
BUILD_DIR = build
BIN_DIR = bin
LOG_DIR = logs
BIN_PATH = $(BUILD_DIR)/$(BIN_DIR)

#Source files
SRC = $(SRC_PATH)/SchedulerClass.cpp $(SRC_PATH)/LogsJson.cpp $(SRC_PATH)/PCB.cpp $(SRC_PATH)/IOManager.cpp $(SRC_PATH)/Metrics.cpp $(SRC_PATH)/ConfigLoader.cpp
MAIN_SRC = $(SRC_PATH)/PriorityScheduler.cpp

#Test files
TEST_SRC = $(TEST_PATH)/test_main.cpp $(TEST_PATH)/test_scheduler.cpp $(TEST_PATH)/test_pcb.cpp $(TEST_PATH)/test_io.cpp $(TEST_PATH)/test_configloader.cpp $(TEST_PATH)/TestFixture.cpp

#Include files
INC = $(INC_PATH)/ReadyQueue.h $(INC_PATH)/SchedulerClass.h $(INC_PATH)/PCB.h $(INC_PATH)/IOManager.h $(INC_PATH)/Metrics.h $(INC_PATH)/ConfigLoader.h
TEST_INC = $(TEST_PATH)/TestFixture.h

#Targets
TGT_MAIN = PriorityScheduler
TGT_TEST = test_scheduler

# Default target
all: build $(TGT_MAIN) $(TGT_TEST)

# Create build directories
build:
	@echo "Generating build directory '$(BIN_PATH)'..."
	@mkdir -p $(BIN_PATH)
	@mkdir -p $(LOG_DIR)

# Link both .cpp files into one executable
$(TGT_MAIN): $(MAIN_SRC) $(SRC) $(INC)
	@echo "Building main program"
	$(CXX) $(CXXFLAGS) -I$(INC_PATH) $(MAIN_SRC) $(SRC) -o $(BIN_PATH)/$@
	@echo "Built $(BIN_PATH)/$@"

$(TGT_TEST): $(TEST_SRC) $(SRC) $(INC) $(TEST_INC)
	@echo "Building test program"
	$(CXX) $(CXXFLAGS) -I$(INC_PATH) -I$(TEST_PATH) $(TEST_SRC) $(SRC) -o $(BIN_PATH)/$@
	@echo "Built $(BIN_PATH)/$@"

#Run tests
test: $(TGT_TEST)
	@echo " "
	@echo "Runnning tests.."
	@$(BIN_PATH)/$(TGT_TEST)

#Run scheduler
run: $(TGT_MAIN)
	@echo " "
	@echo "Runnning scheduler and pcb.."
	@$(BIN_PATH)/$(TGT_MAIN)

#Build only main program
main: build $(TGT_MAIN)

#Build only tests
tests: build $(TGT_TEST)

# Clean up
clean:
	@echo "Cleaning build directory and log files..."
	rm -rf $(BUILD_DIR)
	@echo "Clean complete"

# Clean only log files
clean-logs:
	@echo "Cleaning log files..."
	rm -f $(LOG_DIR)/*.json
	rm -f $(TEST_PATH)/*.json
	@echo "Logs cleaned"

# Help target
help:
	@echo "Available targets:"
	@echo "  make            - Build everything (main program + tests)"
	@echo "  make all        - Same as 'make'"
	@echo "  make main       - Build only main program"
	@echo "  make tests      - Build only test suite"
	@echo "  make test       - Build and run tests"
	@echo "  make run        - Build and run main program"
	@echo "  make clean      - Remove all build artifacts and logs"
	@echo "  make clean-logs - Remove only log files"
	@echo "  make help       - Show this help message"

.PHONY: all build test run main tests clean clean-logs help