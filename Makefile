CC:=gcc
CFLAGS:=-Wall -Werror -Wextra -ggdb
SRC:=pratt_parser.c arena.c tlex.c
HEADERS:=arena.h tlex.h
TARGET:=pratt_parser

$(TARGET): $(SRC) $(HEADERS)
	$(CC) $(CFLAGS) $(SRC) $(HEADERS) -o build/$(TARGET)
