CC = gcc
CFLAGS = -Wall -O2
LDFLAGS =

SRC = src/main.c src/request_handle.c src/list_file.c src/fna2json.c src/decoder.c src/file_buff.c src/dynamic_insert.c
OBJ = $(SRC:.c=.o)

TARGET = server

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm $(OBJ)

clean_all:
	rm $(OBJ) $(TARGET)
