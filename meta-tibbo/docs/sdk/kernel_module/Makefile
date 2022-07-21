obj-m += mhello.o

hello-objs := mhello.c

KERNEL_SRC ?= /lib/modules/$(shell uname -r)/build/

default:
       make -C $(KERNEL_SRC) M=$(PWD) modules

modules_install:
       make -C $(KERNEL_SRC) M=$(PWD) modules_install

clean:
       make -C $(KERNEL_SRC) M=$(PWD) clean
