# APR examples: cmake-built binaries, run one by one from this Makefile.
#
#   make              # configure + build
#   make run          # run every example
#   make run-01       # run a single example (01..09)

BLD   ?= bld
BIN   := $(BLD)/bin
JOBS  ?= 4

EXAMPLES := \
	01_platform \
	02_memory \
	03_file \
	04_thread \
	05_socket \
	06_time \
	07_atomic_shm \
	08_iconv \
	09_tables

.DEFAULT_GOAL := help

.PHONY: help build configure clean run $(addprefix run-,$(EXAMPLES)) \
	run-01 run-02 run-03 run-04 run-05 run-06 run-07 run-08 run-09

help:
	@echo "APR examples"
	@echo ""
	@echo "  make / make build   Configure with CMake and compile"
	@echo "  make run            Run all examples in order"
	@echo "  make run-01 .. 09   Run one example"
	@echo "  make clean          Remove $(BLD)/"
	@echo ""
	@echo "Examples: $(EXAMPLES)"

configure:
	cmake -S . -B $(BLD)

build: configure
	cmake --build $(BLD) -j$(JOBS)

run: build
	@set -e; \
	for e in $(EXAMPLES); do \
		echo ""; \
		echo "===== $$e ====="; \
		$(BIN)/$$e; \
	done

run-01_platform run-02_memory run-03_file run-04_thread run-05_socket \
run-06_time run-07_atomic_shm run-08_iconv run-09_tables: run-%: build
	@echo "===== $* ====="
	@$(BIN)/$*

run-01: run-01_platform
run-02: run-02_memory
run-03: run-03_file
run-04: run-04_thread
run-05: run-05_socket
run-06: run-06_time
run-07: run-07_atomic_shm
run-08: run-08_iconv
run-09: run-09_tables

clean:
	rm -rf $(BLD)
