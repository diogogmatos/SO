CC = gcc
CFLAGS = -Wall -Wextra -pedantic-errors -O3 -I include $(shell pkg-config --cflags glib-2.0)
LIBS = -lm $(shell pkg-config --libs glib-2.0)

LOG = log/*

PATH_S = src/server
PATH_C = src/client

OBJ_PATH_S = obj/server
OBJ_PATH_C = obj/client

SRC_S = $(wildcard $(PATH_S)/*.c)
OBJS_S = ${SRC_S:$(PATH_S)/%.c=$(OBJ_PATH_S)/%.o}

SRC_C = $(wildcard $(PATH_C)/*.c)
OBJS_C = ${SRC_C:$(PATH_C)/%.c=$(OBJ_PATH_C)/%.o}

TARGET_S = server
TARGET_C = client

LEAKS ?= 0 # check leaks

ifeq ($(SANITIZE), 1)
	CFLAGS += -fsanitize=leak
endif

all: $(TARGET_S) $(TARGET_C)

$(TARGET_S): $(OBJS_S)
	@$(CC) $(CFLAGS) $(LIBS) -o monitor $^ ; echo "[COMPILED] $@"

$(TARGET_C): $(OBJS_C)
	@$(CC) $(CFLAGS) $(LIBS) -o tracer $^ ; echo "[COMPILED] $@"

$(OBJ_PATH_S)/%.o: $(PATH_S)/%.c
	@if [ ! -e $(OBJ_PATH_S) ]; then\
		mkdir -p $(OBJ_PATH_S) ; echo "[CREATED] $(OBJ_PATH_S)/";\
	fi
	@$(CC) $(CFLAGS) $(LIBS) -c -o $@ $^ ; echo "[LINKED] $@"

$(OBJ_PATH_C)/%.o: $(PATH_C)/%.c
	@if [ ! -e $(OBJ_PATH_C) ]; then\
		mkdir -p $(OBJ_PATH_C) ; echo "[CREATED] $(OBJ_PATH_C)/";\
	fi
	@$(CC) $(CFLAGS) $(LIBS) -c -o $@ $^ ; echo "[LINKED] $@"

run-server: $(TARGET_S)
	./monitor "log"

run-client: $(TARGET_C)
	./tracer

clean: # Clean the output files
	@rm -f monitor tracer $(OBJS_C) $(OBJS_S) $(LOG) ; echo "[CLEANED]"
