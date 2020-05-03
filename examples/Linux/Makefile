CC       = gcc
OBJ      = ULWOS2_demo1.o ULWOS2.o ULWOS2_HAL.o
LINKOBJ  = obj/ULWOS2_demo1.o obj/ULWOS2.o obj/ULWOS2_HAL.o
BIN      = ULWOS2_demo1
CFLAGS   = -g3 -D ULWOS2_TARGET=ULWOS2_TARGET_LINUX
RM       = rm -f

.PHONY: all clean clean-custom

all: makedir $(BIN) 

clean: clean-custom
	${RM} $(LINKOBJ) $(BIN)

$(BIN): $(OBJ)
	$(CC) $(LINKOBJ) -o $(BIN) $(LIBS)

ULWOS2_demo1.o: ULWOS2_demo1.c
	$(CC) -c ULWOS2_demo1.c -o obj/ULWOS2_demo1.o $(CFLAGS)

ULWOS2.o: ../../src/ULWOS2.c
	$(CC) -c ../../src/ULWOS2.c -o obj/ULWOS2.o $(CFLAGS)

ULWOS2_HAL.o: ../../src/ULWOS2_HAL.c
	$(CC) -c ../../src/ULWOS2_HAL.c -o obj/ULWOS2_HAL.o $(CFLAGS)

makedir:
	mkdir -p obj
