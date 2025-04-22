CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = riscv-assembler
SRC = FileManager/fileManager.c Assembler/assembler.c Main/main.c
OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJ) $(TARGET)

test: $(TARGET)
	./$(TARGET) test.asm test.bin

.PHONY: all clean test