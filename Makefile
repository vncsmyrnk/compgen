CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g -O0 -fsanitize=address -fno-omit-frame-pointer
INCLUDES = -Ivendor/ckdl/include
LDLIBS = -lm

SRCDIR = .
SRCS = $(wildcard $(SRCDIR)/src/*.c) $(wildcard $(SRCDIR)/vendor/ckdl/src/*.c)
ALL_SRCS = $(wildcard $(SRCDIR)/src/*.c) $(wildcard $(SRCDIR)/vendor/ckdl/src/*.c) $(wildcard $(SRCDIR)/test/*.c)
OBJS = $(SRCS:.c=.o)

OUTPUT = $(SRCDIR)/.out
TARGET = $(OUTPUT)/compgen

PREFIX ?= /usr
DESTDIR ?=

BINDIR = $(PREFIX)/bin

INSTALL ?= install
INSTALL_PROGRAM = $(INSTALL)

TEST_BIN = $(OUTPUT)/test_runner
TEST_SRCS = $(wildcard $(SRCDIR)/test/*.c) $(filter-out $(SRCDIR)/src/main.c, $(SRCS))
TEST_OBJS = $(TEST_SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(OUTPUT)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDLIBS)

.PHONY: install
install: all
	$(INSTALL) -d $(DESTDIR)$(BINDIR)
	$(INSTALL_PROGRAM) $(TARGET) $(DESTDIR)$(BINDIR)

$(TEST_BIN): $(TEST_OBJS)
	@mkdir -p $(OUTPUT)
	$(CC) $(CFLAGS) -o $@ $(TEST_OBJS) $(LDLIBS)

.PHONY: test
check: all $(TEST_BIN)
	LSAN_OPTIONS=suppressions=lsan_suppressions.txt ./$(TEST_BIN)

.PHONY: lint
lint:
	clang-format --dry-run --Werror $(ALL_SRCS)

.PHONY: format
format:
	clang-format -i $(ALL_SRCS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(OBJS) $(TARGET) $(TEST_OBJS)
	rm -rf $(OUTPUT)
