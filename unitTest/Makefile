CC       = gcc
OBJ      = ULWOS2_unitTest.o ULWOS2.o ULWOS2_HAL.o
LINKOBJ  = obj/ULWOS2_unitTest.o obj/ULWOS2.o obj/ULWOS2_HAL.o
BIN      = ULWOS2_unitTest
CFLAGS   = -g3 -D ULWOS2_TARGET=ULWOS2_TARGET_UNITTEST -D ULWOS2_SIGNALS
RM       = rm -f

.PHONY: all clean clean-custom

all: makedir $(BIN) 

clean: clean-custom
	${RM} $(LINKOBJ) $(BIN)

$(BIN): $(OBJ)
	$(CC) $(LINKOBJ) -o $(BIN) $(LIBS)

ULWOS2_unitTest.o: ULWOS2_unitTest.c
	$(CC) -c ULWOS2_unitTest.c -o obj/ULWOS2_unitTest.o $(CFLAGS)

ULWOS2.o: ../src/ULWOS2.c
	$(CC) -c ../src/ULWOS2.c -o obj/ULWOS2.o $(CFLAGS)

ULWOS2_HAL.o: ../src/ULWOS2_HAL.c
	$(CC) -c ../src/ULWOS2_HAL.c -o obj/ULWOS2_HAL.o $(CFLAGS)

makedir:
	mkdir -p obj
