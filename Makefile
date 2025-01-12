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
	@echo "Usage: make [TARGET]... [VARIABLE]..."
	@echo ""
	@echo "Targets:"
	@echo "  all     Build both a static and shared library (default)."
	@echo "  static  Build only a static library."
	@echo "  shared  Build only a shared library."
	@echo "  clean   Remove all build artifacts."
	@echo "  help    Display this help."
	@echo ""
	@echo "Variables:"
	@echo "  BUILD_TYPE={debug|release}  Set the build type (default = release)."

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