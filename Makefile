CC=g++
CFLAGS=-static -lscrypt -fpermissive -w
OUT=./bin/a.out

make: ./bin/AES.o
	$(CC) -o $(OUT) ./bin/AES.o main.cpp $(CFLAGS)

./bin/AES.o: AES.asm
	nasm -f elf64 -o ./bin/AES.o AES.asm

clean:
	rm $(OUT)
	rm ./bin/AES.o
