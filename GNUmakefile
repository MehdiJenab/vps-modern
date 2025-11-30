# ==============================================================================
# Vlasov-Poisson Solver - Development Makefile
# ==============================================================================
# This Makefile provides convenient shortcuts for common development tasks.
# It wraps CMake commands and ensures builds happen out-of-source.
#
# Usage:
#   make              - Build debug version
#   make release      - Build release version
#   make test         - Run all tests
#   make clean        - Remove build artifacts
#   make distclean    - Remove ALL generated files (full reset)
#   make help         - Show all available targets
# ==============================================================================

.PHONY: all debug release test clean distclean help
.PHONY: configure-debug configure-release build-debug build-release
.PHONY: run format lint docs install

# Default target
all: debug

# ==============================================================================
# Configuration
# ==============================================================================
BUILD_DIR := build
DEBUG_DIR := $(BUILD_DIR)/debug
RELEASE_DIR := $(BUILD_DIR)/release
NPROC := $(shell nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

# Colors for output
CYAN := \033[36m
GREEN := \033[32m
YELLOW := \033[33m
RED := \033[31m
RESET := \033[0m

# ==============================================================================
# Build Targets
# ==============================================================================

## debug: Build debug version (default)
debug: configure-debug build-debug
	@echo "$(GREEN)Debug build complete!$(RESET)"
	@echo "Executable: $(DEBUG_DIR)/src/app/vps_solver"

## release: Build optimized release version
release: configure-release build-release
	@echo "$(GREEN)Release build complete!$(RESET)"
	@echo "Executable: $(RELEASE_DIR)/src/app/vps_solver"

configure-debug:
	@echo "$(CYAN)Configuring debug build...$(RESET)"
	@cmake --preset debug

configure-release:
	@echo "$(CYAN)Configuring release build...$(RESET)"
	@cmake --preset release-with-tests

build-debug:
	@echo "$(CYAN)Building debug...$(RESET)"
	@cmake --build $(DEBUG_DIR) -j$(NPROC)

build-release:
	@echo "$(CYAN)Building release...$(RESET)"
	@cmake --build $(RELEASE_DIR) -j$(NPROC)

# ==============================================================================
# Test Targets
# ==============================================================================

## test: Run all tests (debug build)
test: debug
	@echo "$(CYAN)Running tests...$(RESET)"
	@cd $(DEBUG_DIR) && ctest --output-on-failure

## test-release: Run all tests (release build)
test-release: release
	@echo "$(CYAN)Running tests (release)...$(RESET)"
	@cd $(RELEASE_DIR) && ctest --output-on-failure

## test-verbose: Run tests with verbose output
test-verbose: debug
	@echo "$(CYAN)Running tests (verbose)...$(RESET)"
	@cd $(DEBUG_DIR) && ctest --output-on-failure --verbose

# ==============================================================================
# Run Targets
# ==============================================================================

## run: Build and run the solver (debug)
run: debug
	@echo "$(CYAN)Running solver...$(RESET)"
	@$(DEBUG_DIR)/src/app/vps_solver

## run-release: Build and run the solver (release)
run-release: release
	@echo "$(CYAN)Running solver (release)...$(RESET)"
	@$(RELEASE_DIR)/src/app/vps_solver

# ==============================================================================
# Clean Targets
# ==============================================================================

## clean: Remove build directories
clean:
	@echo "$(YELLOW)Cleaning build directories...$(RESET)"
	@rm -rf $(BUILD_DIR)
	@echo "$(GREEN)Clean complete.$(RESET)"

## distclean: Full clean - remove ALL generated files
distclean: clean
	@echo "$(YELLOW)Removing all generated files...$(RESET)"
	@# Remove any in-source CMake artifacts (from accidental in-source builds)
	@rm -rf CMakeCache.txt CMakeFiles/ cmake_install.cmake Makefile
	@rm -rf compile_commands.json CTestTestfile.cmake Testing/
	@rm -rf _deps/ lib/ bin/
	@rm -rf install/
	@# Clean source directories of any build artifacts
	@find src -name 'CMakeFiles' -type d -exec rm -rf {} + 2>/dev/null || true
	@find src -name 'cmake_install.cmake' -delete 2>/dev/null || true
	@find src -name 'Makefile' -delete 2>/dev/null || true
	@find src -name 'CTestTestfile.cmake' -delete 2>/dev/null || true
	@find src -name '*.a' -delete 2>/dev/null || true
	@find src -name '*.o' -delete 2>/dev/null || true
	@find src -name '*.o.d' -delete 2>/dev/null || true
	@find src -name '*_include.cmake' -delete 2>/dev/null || true
	@find src -name '*_tests.cmake' -delete 2>/dev/null || true
	@# Remove test executables
	@find src -name 'test_*' -type f -executable -delete 2>/dev/null || true
	@find src -name 'vps_solver' -type f -executable -delete 2>/dev/null || true
	@echo "$(GREEN)Full clean complete. Source tree restored.$(RESET)"

# ==============================================================================
# Code Quality Targets
# ==============================================================================

## format: Format all source files with clang-format
format:
	@echo "$(CYAN)Formatting source files...$(RESET)"
	@find src -name '*.cpp' -o -name '*.h' | xargs clang-format -i
	@echo "$(GREEN)Formatting complete.$(RESET)"

## format-check: Check formatting without modifying files
format-check:
	@echo "$(CYAN)Checking source formatting...$(RESET)"
	@find src -name '*.cpp' -o -name '*.h' | xargs clang-format --dry-run --Werror

## lint: Run static analysis (requires clang-tidy)
lint: configure-debug
	@echo "$(CYAN)Running static analysis...$(RESET)"
	@find src -name '*.cpp' | xargs clang-tidy -p $(DEBUG_DIR)

# ==============================================================================
# Documentation Targets
# ==============================================================================

## docs: Generate documentation (requires Doxygen)
docs:
	@echo "$(CYAN)Generating documentation...$(RESET)"
	@if command -v doxygen &> /dev/null; then \
		doxygen Doxyfile; \
		echo "$(GREEN)Documentation generated in docs/html/$(RESET)"; \
	else \
		echo "$(RED)Doxygen not found. Please install doxygen.$(RESET)"; \
	fi

# ==============================================================================
# Installation
# ==============================================================================

## install: Install release build to install/ directory
install: release
	@echo "$(CYAN)Installing...$(RESET)"
	@cmake --install $(RELEASE_DIR) --prefix install/
	@echo "$(GREEN)Installed to install/$(RESET)"

# ==============================================================================
# Help
# ==============================================================================

## help: Show this help message
help:
	@echo "$(CYAN)Vlasov-Poisson Solver - Available targets:$(RESET)"
	@echo ""
	@echo "$(GREEN)Build:$(RESET)"
	@echo "  make              Build debug version (default)"
	@echo "  make debug        Build debug version"
	@echo "  make release      Build optimized release version"
	@echo ""
	@echo "$(GREEN)Test:$(RESET)"
	@echo "  make test         Run all tests (debug build)"
	@echo "  make test-release Run all tests (release build)"
	@echo "  make test-verbose Run tests with verbose output"
	@echo ""
	@echo "$(GREEN)Run:$(RESET)"
	@echo "  make run          Build and run solver (debug)"
	@echo "  make run-release  Build and run solver (release)"
	@echo ""
	@echo "$(GREEN)Clean:$(RESET)"
	@echo "  make clean        Remove build directories"
	@echo "  make distclean    Full clean - restore source tree"
	@echo ""
	@echo "$(GREEN)Code Quality:$(RESET)"
	@echo "  make format       Format source with clang-format"
	@echo "  make format-check Check formatting without changes"
	@echo "  make lint         Run clang-tidy static analysis"
	@echo ""
	@echo "$(GREEN)Other:$(RESET)"
	@echo "  make docs         Generate Doxygen documentation"
	@echo "  make install      Install to install/ directory"
	@echo "  make help         Show this help message"
	@echo ""
	@echo "$(YELLOW)Note: Always use this Makefile or CMake presets to ensure"
	@echo "out-of-source builds. Never run cmake directly in the source tree.$(RESET)"
