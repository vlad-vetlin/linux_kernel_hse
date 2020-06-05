obj-m += phone_dict.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

install: 
	insmod ./phone_dict.ko
	mknod /dev/phone_dict c 44 0

uninstall:
	rmmod phone_dict
	rm /dev/phone_dict

.PHONY: all options clean install uninstall
