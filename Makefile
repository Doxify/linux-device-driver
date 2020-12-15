KERNELDIR=/lib/modules/`uname -r`/build


#Change the names here to your file name
MODULES = driver_module.ko 
obj-m += driver_module.o 

all:
	make -C $(KERNELDIR) M=$(PWD) modules
	$(CC) driver_test.c -o driver_test

clean:
	make -C $(KERNELDIR) M=$(PWD) clean
	rm driver_test

install:	
	make -C $(KERNELDIR) M=$(PWD) modules_install

quickInstall:
	cp $(MODULES) /lib/modules/`uname -r`/extra