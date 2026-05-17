CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2
INCLUDES = -Ivendor/ckdl/include
LDLIBS = -lm

SRCDIR = .
SRCS = $(wildcard $(SRCDIR)/src/*.c) $(wildcard $(SRCDIR)/vendor/ckdl/src/*.c)
OBJS = $(SRCS:.c=.o)

OUTPUT = $(SRCDIR)/.out
TARGET = $(OUTPUT)/compgen

PREFIX ?= /usr
DESTDIR ?=

BINDIR = $(PREFIX)/bin

INSTALL ?= install
INSTALL_PROGRAM = $(INSTALL)

all: $(TARGET)

$(TARGET): $(OBJS) | out
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

.PHONY: install
install: all
	$(INSTALL) -d $(DESTDIR)$(BINDIR)
	$(INSTALL_PROGRAM) $(TARGET) $(DESTDIR)$(BINDIR)

.PHONY:
check:
	clang-format --dry-run --Werror $(SRCS)

.PHONY: out
out:
	@mkdir -p $(OUTPUT)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(OBJS) $(TARGET)
	rm -rf $(OUTPUT)
