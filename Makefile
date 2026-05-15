CC      := gcc
CFLAGS_COMMON := -Wall -Wextra -Wpedantic -I./src -I./deps
CFLAGS_DEBUG  := -O0 -g
CFLAGS_RELEASE := -O2

CFLAGS_ASAN := -fsanitize=address -g -O0 -fno-omit-frame-pointer
LDFLAGS_ASAN := -fsanitize=address

CFLAGS  := $(CFLAGS_COMMON) $(CFLAGS_DEBUG)
CFLAGS_BENCH := $(CFLAGS_COMMON) $(CFLAGS_RELEASE) -DNDEBUG
LDFLAGS := -lm

SRC := $(shell find src -name '*.c' ! -name 'main.c' )
APP_SRC := $(SRC) src/main.c

HDR := $(shell find src -name '*.h') $(shell find tests -name '*.h' 2>/dev/null || true)

TARGET  := build/convol

.PHONY: build clean fmt help asan

help:
	@echo "Available targets:"
	@echo "  build         - Build main program"
	@echo "  asan          - Build with AddressSanitizer"
	@echo "  clean         - Remove build directory"
	@echo "  fmt           - Format code"

build: $(TARGET)

$(TARGET): $(APP_SRC) $(HDR)
	@mkdir -p build
	$(CC) $(CFLAGS) $(APP_SRC) -o $@ $(LDFLAGS)

asan: clean
	@mkdir -p build
	$(CC) $(CFLAGS) $(CFLAGS_ASAN) $(APP_SRC) -o $(TARGET)_asan $(LDFLAGS) $(LDFLAGS_ASAN)
	
clean:
	rm -rf build

fmt:
	clang-format -i $(SRC) $(APP_SRC) $(HDR)
