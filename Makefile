CC = clang
CFLAGS = -std=c99 -Wall -Wextra -Wpedantic -O2
SRCS = main.c lc3.c
OBJS = $(SRCS:.c=.o)
TARGET = lc3_emu

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

sanitize: CFLAGS += -fsanitize=address,undefined -g
sanitize: re

test: $(TARGET)
	@bash tests/run_tests.sh

clean:
	rm -f $(OBJS) $(TARGET) test.obj tests/*.obj

re: clean all

.PHONY: all sanitize test clean re
