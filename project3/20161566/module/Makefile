obj-m = interrupt.o 
CC := arm-none-linux-gnueabi-gcc

KDIR:=/work/achroimx_kernel
PWD := $(shell pwd)

all: driver

driver : 
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules ARCH=arm

send:
	adb push interrupt.ko /data/local/tmp

clean : 
	rm -rf *.o
	rm -rf *.ko
	rm -rf *.mod.c
	rm -rf *.order
	rm -rf *.symvers
