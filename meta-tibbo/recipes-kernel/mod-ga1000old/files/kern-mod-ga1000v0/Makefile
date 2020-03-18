EXTRA_CFLAGS += -DLINUX
obj-m := tpsga1000v0.o
tpsga1000v0-objs := ga1000v0_main.o

SRC := $(shell pwd)

all:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC)

modules_install:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) modules_install

clean:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) clean
