EXTRA_CFLAGS += -DLINUX
obj-m := tpsleds.o
tpsleds-objs := tpsleds_main.o tpsleds_procfs.o

SRC := $(shell pwd)

all:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC)

modules_install:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) modules_install

clean:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) clean
