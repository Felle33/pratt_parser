CC:=gcc
CFLAGS:=-Wall -Werror -Wextra -ggdb
SRC:=pratt_parser.c src/arena.c src/tlex.c
TARGET:=pratt_parser

$(TARGET): $(SRC) $(HEADERS)
	$(CC) $(CFLAGS) $(SRC) -I./include -o build/$(TARGET)
