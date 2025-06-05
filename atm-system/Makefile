CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -D_GNU_SOURCE -I. $(shell pkg-config --cflags openssl)
SRCDIR = src
OBJDIR = src
SRCS = $(SRCDIR)/auth.c $(SRCDIR)/main.c $(SRCDIR)/system_input.c $(SRCDIR)/system_db.c $(SRCDIR)/system_menu.c $(SRCDIR)/system.c
OBJS = $(SRCS:.c=.o)
TARGET = atm

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    CRYPTLIB =
else
    CRYPTLIB = -lcrypt
endif

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(CRYPTLIB) -lsqlite3 $(shell pkg-config --libs openssl)

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(SRCDIR)/header.h
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
