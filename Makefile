CC = gcc
CFLAGS = -std=c23 -Wall -Wextra -Wpedantic -Werror
CPPFLAGS = $(INCS) $(DEPFLAGS)
DEPFLAGS = -MMD -MP

SRC = src
BIN = bin
OBJ = obj

INCS = -Iinclude
SRCS = $(shell find $(SRC) -type f -name '*.c')
STATIC_OBJS = $(patsubst $(SRC)/%.c, $(OBJ)/$(BUILD_TYPE)/static/%.o, $(SRCS))
SHARED_OBJS = $(patsubst $(SRC)/%.c, $(OBJ)/$(BUILD_TYPE)/shared/%.o, $(SRCS))
STATIC_DEPS = $(patsubst %.o, %.d, $(STATIC_OBJS))
SHARED_DEPS = $(patsubst %.o, %.d, $(SHARED_OBJS))

STATIC_LIB = $(BIN)/$(BUILD_TYPE)/static/libscunit$(LIB_SUFFIX).a
SHARED_LIB = $(BIN)/$(BUILD_TYPE)/shared/libscunit$(LIB_SUFFIX).so

COLORED_OUTPUT ?= enabled
ifeq ($(COLORED_OUTPUT), enabled)
	RESET = \033[0m
	GREEN = \033[32m
	CYAN = \033[36m
endif

BUILD_TYPE ?= release
ifeq ($(BUILD_TYPE), debug)
    CFLAGS += -g3 -O0
    LIB_SUFFIX = d
else ifeq ($(BUILD_TYPE), release)
    CFLAGS += -g0 -O3
endif

.PHONY: all static shared clean help

all: static shared

static: $(STATIC_LIB)

shared: $(SHARED_LIB)

clean:
	@rm -rf $(BIN) $(OBJ)

help:
	@echo -e "Usage: make [$(GREEN)TARGET$(RESET)]... [$(CYAN)VARIABLE$(RESET)]..."
	@echo ""
	@echo "Targets:"
	@echo -e "  $(GREEN)all$(RESET)     Build both a static and a shared library (default)."
	@echo -e "  $(GREEN)static$(RESET)  Build only a static library."
	@echo -e "  $(GREEN)shared$(RESET)  Build only a shared library."
	@echo -e "  $(GREEN)clean$(RESET)   Remove all build artifacts."
	@echo -e "  $(GREEN)help$(RESET)    Print this help message."
	@echo ""
	@echo "Variables:"
	@echo -e "  $(CYAN)COLORED_OUTPUT$(RESET)={disabled|enabled}  Enable or disable colored output (default = enabled)."
	@echo -e "  $(CYAN)BUILD_TYPE$(RESET)={debug|release}         Set the build type (default = release)."

$(STATIC_LIB): $(STATIC_OBJS)
	@mkdir -p $(dir $@)
	@ar -rcs $@ $^

$(SHARED_LIB): $(SHARED_OBJS)
	@mkdir -p $(dir $@)
	@$(CC) -shared $^ -o $@

$(OBJ)/$(BUILD_TYPE)/static/%.o: $(SRC)/%.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(OBJ)/$(BUILD_TYPE)/shared/%.o: $(SRC)/%.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -fPIC $(CPPFLAGS) -c $< -o $@

-include $(STATIC_DEPS) $(SHARED_DEPS)