# ==============================================================================
# DSP Project Makefile
# ==============================================================================

# Compiler and flags
CC        := gcc
CFLAGS    := -Wall -Wextra -Wpedantic -std=c11
LDFLAGS   := -lm

# Directories
SRC_DIRS  := core

# Output
TARGET    := dsp

# Root-level source files (e.g. main.c)
ROOT_SRCS := $(wildcard *.c)

# Automatically find all .c source files and derive .o files
SRCS      := $(ROOT_SRCS) $(shell find $(SRC_DIRS) -name '*.c')
OBJS      := $(SRCS:.c=.o)

# Automatically collect all unique include directories (wherever .h files live)
INC_DIRS  := $(sort $(dir $(shell find $(SRC_DIRS) -name '*.h')))
CFLAGS    += $(addprefix -I, $(INC_DIRS))

# ==============================================================================
# Build rules
# ==============================================================================

.PHONY: all clean rebuild format

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@find $(SRC_DIRS) -name '*.o' -delete 2>/dev/null || true
	@rm -f $(TARGET)
	@echo "Clean complete."

rebuild: clean all

format:
	@find . -name '*.c' -o -name '*.h' | xargs clang-format -i
	@echo "Formatting complete."
