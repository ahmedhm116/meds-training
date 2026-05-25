.PHONY: all test report clean help setup

# Variables
SHELL := /bin/bash
SCRIPT := ./scripts/analyze.sh
LOG_DIR := test_data
OUT_DIR := output
LOGS := $(wildcard $(LOG_DIR)/*.log)

# Default target
.DEFAULT_GOAL := all

## all: Run the analyzer on all test log files
all:
	@echo "Running analysis on all logs in $(LOG_DIR)..."
	@mkdir -p $(OUT_DIR)
	@for log in $(LOGS); do \
		$(SCRIPT) $$log; \
	done

## test: Run the analyzer and verify exit codes (0 for pass, 1 for fail)
test:
	@echo "Starting verification tests..."
	@echo "Testing sample_pass.log (Expected: PASS/0)..."
	@$(SCRIPT) $(LOG_DIR)/sample_pass.log > /dev/null && echo "SUCCESS: sample_pass exited with 0"
	@echo "Testing sample_fail.log (Expected: FAIL/1)..."
	@! $(SCRIPT) $(LOG_DIR)/sample_fail.log > /dev/null && echo "SUCCESS: sample_fail exited with 1"
	@echo "All verification tests completed."

## report: Generate a summary report for each log in output/
report:
	@echo "Generating reports in $(OUT_DIR)..."
	@mkdir -p $(OUT_DIR)
	@for log in $(LOGS); do \
		base=$$(basename $$log .log); \
		$(SCRIPT) $$log --output $(OUT_DIR)/$${base}_report.txt; \
		echo "Generated: $(OUT_DIR)/$${base}_report.txt"; \
	done

## clean: Remove all generated output files
clean:
	@echo "Cleaning up $(OUT_DIR)..."
	@rm -rf $(OUT_DIR)/*.txt
	@echo "Cleanup complete."

## help: Print all available targets with descriptions
help:
	@echo "Available targets:"
	@grep -E '^##' Makefile | sed -e 's/## //' -e 's/:/ -/'


## setup: Check that all required tools are installed
setup:
	@echo "Checking the required tools"
	@command -v bash >/dev/null 2>&1 || { echo >&2 "Bash is required but it's not installed. Aborting."; exit 1; }
	@command -v awk >/dev/null 2>&1 || { echo >&2 "Awk is required but it's not installed. Aborting."; exit 1; }
	@command -v grep >/dev/null 2>&1 || { echo >&2 "Grep is required but it's not installed. Aborting."; exit 1; }
	@chmod +x scripts/setup_env.sh
	@echo "Environment setup complete. You can now run the analysis scripts."