obj-m+=mpu9250_pslavkin.o
KDIR := /opt/linux_kernel/
all:
	$(MAKE) -C $(KDIR) M=$$PWD modules
	$(CROSS_COMPILE)gcc test.c  -o test
clean:
	$(MAKE) -C $(KDIR) M=$$PWD clean
	rm test
