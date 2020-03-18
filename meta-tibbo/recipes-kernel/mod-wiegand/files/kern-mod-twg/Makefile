EXTRA_CFLAGS += -DLINUX
obj-m := twg.o
twg-objs := twg_main.o twg_procfs.o twg_sysfs.o

SRC := $(shell pwd)

all:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC)

modules_install:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) modules_install

clean:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) clean
