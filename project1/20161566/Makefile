CC= arm-none-linux-gnueabi-gcc
CFLAGS= -static -pthread
OBJECTS = main.o input_process.o output_process.o
TARGET = 20161566

all: $(TARGET)

$(TARGET) : $(OBJECTS)
			  $(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

main.o: main.c global.h
	$(CC) $(CLFAGS) -c main.c

input_process.o: input_process.c global.h input_process.h
	$(CC) $(CFLAGS) -c input_process.c

output_process.o: output_process.c global.h output_process.h 
	$(CC) $(CFLAGS) -c output_process.c 


clean:
	rm -rf $(OBJECTS) $(TARGET) *.h.gch

