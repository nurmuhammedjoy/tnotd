# Compiler and flags
CC = clang
CFLAGS = -Wall -Wextra -O2
GTK_FLAGS = $(shell pkg-config --cflags --libs gtk+-3.0 libnotify)
JSON_FLAGS = -ljson-c

# Source files and targets
NOTIF_SRC = notification.c
NOTIF_BIN = tnotd

DAEMON_SRC = daemon.c
DAEMON_BIN = daemon

# Installation directory in Termux
PREFIX = /data/data/com.termux/files/usr/bin

# Default rule
all: $(NOTIF_BIN) $(DAEMON_BIN)

# Compile notification.c with GTK and libnotify
$(NOTIF_BIN): $(NOTIF_SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(GTK_FLAGS)

# Compile daemon.c with json-c
$(DAEMON_BIN): $(DAEMON_SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(JSON_FLAGS)

# Install binaries to Termux bin
install: all
	cp $(NOTIF_BIN) $(PREFIX)/
	cp $(DAEMON_BIN) $(PREFIX)/

# Clean build files
clean:
	rm -f $(NOTIF_BIN) $(DAEMON_BIN)

.PHONY: all install clean
