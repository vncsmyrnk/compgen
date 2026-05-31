SRCDIR = .

VERSION := $(shell git describe --tags --always --dirty 2>/dev/null || cat VERSION 2>/dev/null || echo "unknown")

CC = gcc
CFLAGS = $(shell cat $(SRCDIR)/compile_flags.txt) -DVERSION=\"$(VERSION)\"
CFLAGS_RELEASE = -O2
CFLAGS_DEBUG = -g -O0 -fsanitize=address -fno-omit-frame-pointer
LDLIBS = -lm

SRCS = $(wildcard $(SRCDIR)/src/*.c) $(wildcard $(SRCDIR)/vendor/ckdl/src/*.c)
ALL_SRCS = $(wildcard $(SRCDIR)/src/*.c) $(wildcard $(SRCDIR)/vendor/ckdl/src/*.c) $(wildcard $(SRCDIR)/test/*.c)
OBJS = $(SRCS:.c=.o)

OUTPUT = $(SRCDIR)/build
TARGET = $(OUTPUT)/cg
ZSH_COMPLETION = $(OUTPUT)/_cg

PREFIX ?= /usr
DESTDIR ?=

DATAROOTDIR = $(PREFIX)/share
DATADIR = $(DATAROOTDIR)
BINDIR = $(PREFIX)/bin
MANDIR = $(DATAROOTDIR)/man
MANDIR1 = $(MANDIR)/man1
ZSHDIR = $(DATAROOTDIR)/zsh

INSTALL ?= install
INSTALL_PROGRAM = $(INSTALL)
INSTALL_DATA = $(INSTALL) -m 644

TEST_BIN = $(OUTPUT)/test_runner
TEST_SRCS = $(wildcard $(SRCDIR)/test/*.c) $(filter-out $(SRCDIR)/src/main.c, $(SRCS))
TEST_OBJS = $(TEST_SRCS:.c=.o)

all: CFLAGS += $(CFLAGS_RELEASE)
all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(OUTPUT)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDLIBS)

$(ZSH_COMPLETION): completions.kdl $(TARGET)
	$(TARGET) --shell zsh $< > $@

.PHONY: install
install: all $(ZSH_COMPLETION)
	$(INSTALL) -d $(DESTDIR)$(BINDIR)
	$(INSTALL) -d $(DESTDIR)$(ZSHDIR)/site-functions
	$(INSTALL_DATA) $(ZSH_COMPLETION) $(DESTDIR)$(ZSHDIR)/site-functions
	$(INSTALL_PROGRAM) $(TARGET) $(DESTDIR)$(BINDIR)

.PHONY: uninstall
uninstall:
	rm -f $(DESTDIR)$(BINDIR)/cg
	rm -f $(DESTDIR)$(ZSHDIR)/site-functions/_cg

.PHONY: dist
dist:
	@echo "Packaging source for $(VERSION)..."
	@mkdir -p compgen-$(VERSION)
	git archive HEAD | tar -x -C compgen-$(VERSION)
	@echo $(VERSION) > compgen-$(VERSION)/VERSION
	tar -czvf compgen-$(VERSION).tar.gz compgen-$(VERSION)
	@rm -rf compgen-$(VERSION)

$(TEST_BIN): CFLAGS += $(CFLAGS_DEBUG)
$(TEST_BIN): $(TEST_OBJS)
	@mkdir -p $(OUTPUT)
	$(CC) $(CFLAGS) -o $@ $(TEST_OBJS) $(LDLIBS)

.PHONY: debug
debug: CFLAGS += $(CFLAGS_DEBUG)
debug: $(TARGET)

.PHONY: check
check: all $(TEST_BIN)
	LSAN_OPTIONS=suppressions=lsan_suppressions.txt ./$(TEST_BIN)

.PHONY: lint
lint:
	clang-format --dry-run --Werror $(ALL_SRCS)

.PHONY: format
format:
	clang-format -i $(ALL_SRCS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(OBJS) $(TARGET) $(TEST_OBJS)
	rm -rf $(OUTPUT)
