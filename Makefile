CC = gcc
CFLAGS = -Wall -Wextra -pedantic-errors -O3

SRC_S = $(wildcard src/server/*.c)
OBJS_S = ${SRC_S:src/server/%.c=obj/server/%.o}

SRC_C = $(wildcard src/client/*.c)
OBJS_C = ${SRC_C:src/client/%.c=obj/client/%.o}

TARGET_S = server
TARGET_C = client

LEAKS ?= 0 # check leaks

ifeq ($(SANITIZE), 1)
	CFLAGS += -fsanitize=leak
endif

all: $(TARGET_S) $(TARGET_C)

server: $(OBJS_S)
	@$(CC) $(CFLAGS) -o $@ $^ ; echo "[COMPILED] $@"

client: $(OBJS_C)
	@$(CC) $(CFLAGS) -o $@ $^ ; echo "[COMPILED] $@"

obj/server/%.o: src/server/%.c
	@if [ ! -e obj/server ]; then\
		mkdir -p obj/server ; echo "[CREATED] obj/server/";\
	fi
	@$(CC) $(CFLAGS) -c -o $@ $^ ; echo "[LINKED] $@"

obj/client/%.o: src/client/%.c
	@if [ ! -e obj/client ]; then\
		mkdir -p obj/client ; echo "[CREATED] obj/client/";\
	fi
	@$(CC) $(CFLAGS) -c -o $@ $^ ; echo "[LINKED] $@"

run-server: $(TARGET_S)
	./server

run-client: $(TARGET_C)
	./client

clean: # Clean the output files
	@rm -f $(TARGET_S) $(TARGET_C) $(OBJS_S) $(OBJS_C) ; echo "[CLEANED]"
