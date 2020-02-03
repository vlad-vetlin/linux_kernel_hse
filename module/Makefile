obj-m += test.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

install: 
	insmod ./test.ko
	mknod /dev/test c 44 0

uninstall:
	rmmod test
	rm /dev/test

.PHONY: all options clean install uninstall
