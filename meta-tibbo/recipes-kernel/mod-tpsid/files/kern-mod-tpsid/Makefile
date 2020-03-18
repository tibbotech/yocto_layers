EXTRA_CFLAGS += -DLINUX
obj-m := tpsid.o
tpsid-objs := tpsid_main.o tpsid_procfs.o

SRC := $(shell pwd)
#KERNEL_SRC ?= /lib/modules/$(shell uname -r)/build/

all:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC)

modules_install:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) modules_install

clean:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) clean
