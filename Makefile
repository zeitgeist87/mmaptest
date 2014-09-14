CC      = gcc
CFLAGS  = -Wall -O2 -D_FILE_OFFSET_BITS=64

SRC = mmaptest.c
OBJ = $(SRC:%.c=%.o)

all: mmaptest

mmaptest: $(OBJ)
	$(CC) $(CFLAGS) -o mmaptest $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm mmaptest *.o -f
