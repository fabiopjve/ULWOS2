CC       = gcc
OBJ      = main.o ULWOS2.o ULWOS2_HAL.o
LINKOBJ  = obj/main.o obj/ULWOS2.o obj/ULWOS2_HAL.o
BIN      = main
CFLAGS   = -g3
RM       = rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before $(BIN) all-after

clean: clean-custom
	${RM} $(LINKOBJ) $(BIN)

$(BIN): $(OBJ)
	$(CC) $(LINKOBJ) -o $(BIN) $(LIBS)

main.o: main.c
	$(CC) -c main.c -o obj/main.o $(CFLAGS)

ULWOS2.o: src/ULWOS2.c
	$(CC) -c src/ULWOS2.c -o obj/ULWOS2.o $(CFLAGS)

ULWOS2_HAL.o: src/ULWOS2_HAL.c
	$(CC) -c src/ULWOS2_HAL.c -o obj/ULWOS2_HAL.o $(CFLAGS)
