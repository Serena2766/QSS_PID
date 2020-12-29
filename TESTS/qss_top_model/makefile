CC=g++
CFLAGS=-std=c++17
COMPILE_TARGET=NUCLEO_F401RE
FLASH_TARGET=NODE_F401RE
EXECUTABLE_NAME=QSS_TOP

INCLUDRT_ARM_MBED=-I ../../../cadmium/include
INCLUDEDESTIMES=-I ../../../cadmium/DESTimes/include

all: qssSenderReceiver.o 
	$(CC) -g -o $(EXECUTABLE_NAME) qssSenderReceiver.o 

qssSenderReceiver.o: qssSenderReceiver.cpp
	$(CC) -g -c $(CFLAGS) $(INCLUDRT_ARM_MBED) $(INCLUDEDESTIMES) qssSenderReceiver.cpp -o qssSenderReceiver.o

clean:
	rm -f $(EXECUTABLE_NAME) *.o *~
