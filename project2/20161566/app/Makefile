CC := arm-none-linux-gnueabi-gcc

TARGET = app
OBJECT = app.c

all: $(TARGET)

$(TARGET):
	$(CC) -static -o $(TARGET) $(OBJECT)

send:
	adb push app /data/local/tmp

clean : 
	rm -rf *.o
	rm -rf app
