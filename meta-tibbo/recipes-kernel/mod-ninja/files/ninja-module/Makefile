EXTRA_CFLAGS += -DLINUX -DNINJA_DEBUG -Idrivers/usb/gadget/
#EXTRA_CFLAGS += -DLINUX
obj-m := ninja.o
ninja-objs := ninja_main.o ninja_io.o ninja_th.o ninja_cmd.o ninja_hndl.o
ninja-objs += ninja_io_net.o ninja_io_usb.o
ninja-objs += ninja_procfs.o ninja_netlink.o
ninja-objs += g_ninja/g_ninja.o g_ninja/g_ninja_d.o
ninja-objs += g_ninja/g_ninja_req.o g_ninja/g_ninja_rw.o g_ninja/g_ninja_conf.o
