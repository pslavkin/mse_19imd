obj-m+=ebbchar.o
KDIR := /opt/linux_kernel/
all:
	$(MAKE) -C $(KDIR) M=$$PWD modules
#	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules
	$(CROSS_COMPILE)gcc testebbchar.c  -o test
clean:
	$(MAKE) -C $(KDIR) M=$$PWD clean
#	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean
	rm test
