CC=g++
CFLAGS=-lscrypt -O2
OUT=./bin/a
OBJECTS-BASE=base64.o fortuna.o
OBJECTS-NI=AES.o AES-NI.o $(OBJECTS-BASE)
OBJECTS-NO-NI=AES-NO-NI.o $(OBJECTS-BASE)
NASM-ARCH=elf64
WIN_DEF=
ifeq ($(OS),Windows_NT)
	WIN_DEF=-DWINDOWS
endif

make: $(OBJECTS-NI)
	$(CC) -o $(OUT) main.cpp $(OBJECTS-NI) $(CFLAGS)

no-ni: $(OBJECTS-NO-NI)
	$(CC) -o $(OUT) main.cpp $(OBJECTS-NO-NI) $(CFLAGS)

clean:
	rm *.o
	rm $(OUT)

./base64.o: ./base64.cpp
	$(CC) -c -o ./base64.o -O2 ./base64.cpp

./fortuna.o: ./fortuna.cpp
	$(CC) -c -o ./fortuna.o -O2 ./fortuna.cpp

./AES-NI.o: ./AES-NI.asm
	nasm -f $(NASM-ARCH) -o ./AES-NI.o ./AES-NI.asm

./AES.o: ./AES.cpp
	$(CC) -c -o ./AES.o ./AES.cpp $(WIN_DEF)

./AES-NO-NI.o: ./AES.cpp
	$(CC) -c -o ./AES-NO-NI.o -O2 ./AES.cpp -DNO_NI
