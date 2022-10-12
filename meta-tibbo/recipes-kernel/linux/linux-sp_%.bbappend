FILESEXTRAPATHS:prepend := "${THISDIR}/linux-sp:"

COMPATIBLE_MACHINE:append = "|tppg2"

SRC_URI:append = " file://kmeta-tppg2;type=kmeta;name=kmeta-tppg2;destsuffix=kmeta-tppg2"

# additional DTSes
SRC_URI += "file://dts/sp7021-ttt.dts.patch"
SRC_URI += "file://dts/sp7021-ltpp3g2-empty.dts.patch"
SRC_URI += "file://dts/sp7021-dsx.dts.patch"
SRC_URI += "file://dts/sp7021-dsx.dtsi.patch"
SRC_URI += "file://dts/sp7021-icognize.dts.patch"
# tppg2 no BT (uart4) by default
SRC_URI += "file://dts/sp7021-ltpp3g2revD.dts.wifionly.patch"

# for gpio-driven rts/cts
#SRC_URI += "file://uart_gpio_rtscts/sp_uart.c.dbg.patch"
#SRC_URI += "file://uart_gpio_rtscts/sp7021-tpstest.dts.4ctsrts.patch"
#SRC_URI += "file://uart_gpio_rtscts/sp7021-tpsgpio.dts.4ugpio.patch"
# FIXME
#SRC_URI += "file://485/sp_uart.c.0.patch"
#SRC_URI += "file://485/serial_core.c.dbg0.patch"

#SRC_URI += "file://dbg_spi/spi.c.dbg0.patch"
SRC_URI += "file://mcp251x-dbg/mcp251x.c.dbg0.patch"
SRC_URI += "file://mcp251x-dbg/mcp251x.c.clk0.patch"

# set GPIO to 83 for ltpp3g2
SRC_URI += "file://bcmdhd-gpio/dhd_gpio.c.gpio.patch"
#SRC_URI += "file://bcmdhd-gpio/dhd_gpio.c.of.patch"

# mcp251xfd mainline backport
SRC_URI += "file://mcp251xfd-backport/mcp251xfd.tar.gz"
SRC_URI += "file://mcp251xfd-backport/Kconfig.mcp251xfd.patch"
SRC_URI += "file://mcp251xfd-backport/Makefile.mcp251xfd.patch"
#SRC_URI += "git://github.com/tibbotech/mcp251xfd.git;protocol=https;branch=master"

# Goodix dbg
SRC_URI += "file://goodix-dbg/goodix.c.dbg.patch"

# mcp251xfd driver
do_patch:append() {
 cp -r ${WORKDIR}/mcp251xfd ${S}/drivers/net/can/spi/
}

#RDEPENDS:kernel-module-bcmdhd += "${@bb.utils.contains('DISTRO_FEATURES', 'wifi', 'bcmdhd-firmware', '', d)}"
RDEPENDS:kernel-module-bcmdhd += "bcmdhd-firmware"

#KERNEL_EXTRA_FEATURES += "features/initramfs/initramfs.scc"

KERNEL_FEATURES:append = " cfg/rpi-ovls/ovls-4.19.scc"

KERNEL_FEATURES:append = "${@bb.utils.contains("MACHINE_FEATURES", "touchscreen", " custom/touchscreen/all.scc", "" ,d)}"
KERNEL_FEATURES:append = "${@bb.utils.contains("MACHINE_FEATURES", "usbgadget", " features/usb/usb-gadgets.scc", "" ,d)}"
KERNEL_FEATURES:append = "${@bb.utils.contains("MACHINE_FEATURES", "3g", " custom/modems/protos.scc", "" ,d)}"
KERNEL_FEATURES:append = " ${@bb.utils.contains('MACHINE_FEATURES', 'vfat', 'cfg/fs/vfat.scc', '', d)}"
KERNEL_FEATURES:append = " custom/tunnel/udp.scc"
KERNEL_FEATURES:append = " cgl/net/l2tp.scc"
KERNEL_FEATURES:append = " custom/netfilter/addrtype.scc"

KERNEL_DEVICETREE:append:tppg2 = " sp7021-ltpp3g2-empty.dtb"
