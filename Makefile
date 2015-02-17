CC=g++
CFLAGS=-static -lscrypt -fpermissive -w
OUT=./bin/a

make: AES.asm
	nasm -f elf64 -o ./bin/AES.o AES.asm
	$(CC) -o $(OUT) ./bin/AES.o main.cpp $(CFLAGS)

win: AES.asm
	nasm -f win64 -o ./bin/AES.o AES.asm
	$(CC) -o $(OUT) ./bin/AES.o main.cpp $(CFLAGS) -DWINDOWS

clean:
	rm $(OUT)
	rm ./bin/AES.o
