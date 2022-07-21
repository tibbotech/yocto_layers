FILESEXTRAPATHS:prepend := "${THISDIR}/linux-spdev:"

SUMMARY = "Linux kernel (SunPlus,dev)"

# may be moved to /machine/ config
KMACHINE:tppg2 = "pentagram"

#KBRANCH:tppg2 = "kernel_4.19"
#KBRANCH:tppg2 = "tibbo/spi_exp"
#KBRANCH:tppg2 = "tibbo/spi_exp2"
#KBRANCH:tppg2 = "tibbo/485"
#KBRANCH:tppg2 = "tibbo/spi256"
KBRANCH:tppg2 = "kernel_5.4"
#KBRANCH:tppg2 = "master"

# mainline
COMPATIBLE_MACHINE:append = "|tppg2"

SRC_URI = "git://git@113.196.136.131:22/qac628/linux/kernel;protocol=ssh;name=machine;branch=${KBRANCH}"
SRC_URI += "file://kernel-meta/*;type=kmeta;name=meta;destsuffix=${KMETA}"
#SRC_URI += "file://kernel-meta.tar.gz;type=kmeta;name=meta;destsuffix=${KMETA}"
#SRC_URI += "git://git.yoctoproject.org/yocto-kernel-cache;type=kmeta;name=meta;branch=yocto-4.19;destsuffix=${KMETA}"

# 4.19
SRCREV_machine:tppg2 = "e81c7196d43ee83e0c05a9ac666cfe7a5fbd2ce9"
# 5.4 tibbo/spi_exp
SRCREV_machine:tppg2 = "1cc820c8fb982b0dc359513dc069bba4e097fcd1"
# 5.4 tibbo/spi_exp2
SRCREV_machine:tppg2 = "acd013f0a071284c010f25bc71b7b96d042bfcc1"
# 5.4 master
SRCREV_machine:tppg2 = "69009583880c562439e0a3962797a0ea8d2e55da"

# temporary it is the copy
SRCREV = "69009583880c562439e0a3962797a0ea8d2e55da"

# if using meta from master
#SRCREV_meta ?= "cebe198870d781829bd997a188cc34d9f7a61023"
LINUX_VERSION = "4.19.37"
LINUX_VERSION = "5.4.35"

#LINUX_KERNEL_TYPE = "debug"

MIRRORS=""
PREMIRRORS=""

KMETA="kernel-meta"

SRC_URI += "file://dts/sp7021-ttt.dts.patch"
#SRC_URI += "file://pinctrl_dbg/sppctl.c.err.patch"
#SRC_URI += "file://pinctrl_dbg/sp7021_gpio_ops.c.Fdbg.patch"
#SRC_URI += "file://pinctrl_dbg/pins.newdbg.patch"
#SRC_URI += "file://irq-sp-intc.c.dv1.patch"

SRC_URI += "file://dts/sp7021-tpsgpio.dts.patch"
# for gpio-driven rts/cts
#SRC_URI += "file://uart_gpio_rtscts/sp_uart.c.dbg.patch"
#SRC_URI += "file://uart_gpio_rtscts/sp7021-tpstest.dts.4ctsrts.patch"
#SRC_URI += "file://uart_gpio_rtscts/sp7021-tpsgpio.dts.4ugpio.patch"
# SDIO debug
#SRC_URI += "file://sdio_dbg/spsdv2.c.err.patch"
#SRC_URI += "file://sdio_dbg/spsdv2.c.inf.patch"

# tmp: spi debug
#SRC_URI += "file://dbg_spi/spi-sunplus-sp7021.c.dbg0.patch"
#SRC_URI += "file://dbg_spi/spi-sunplus-sp7021.c.dbg1.patch"
SRC_URI += "file://dbg_spi/spi-sunplus-sp7021.c.norxtxdbg.patch"
#SRC_URI += "file://dbg_spi/spi-sunplus-sp7021.c.csdbg.patch"
# test CAN without it
#SRC_URI += "file://dbg_spi/spi-sunplus-sp7021.c.len.patch"

#SRC_URI += "file://dbg_spi/spi.c.dbg0.patch"
SRC_URI += "file://mcp251x-dbg/mcp251x.c.dbg0.patch"
SRC_URI += "file://mcp251x-dbg/mcp251x.c.clk0.patch"

# set GPIO to 83 for ltpp3g2
SRC_URI += "file://bcmdhd/dhd_gpio.c.gpio.patch"
#SRC_URI += "file://bcmdhd/dhd_gpio.c.of.patch"

# mcp251xfd mainline backport
SRC_URI += "file://mcp251xfd-backport/mcp251xfd.tar.gz"
SRC_URI += "file://mcp251xfd-backport/Kconfig.mcp251xfd.patch"
SRC_URI += "file://mcp251xfd-backport/Makefile.mcp251xfd.patch"

# +dts
SRC_URI += "file://dts/sp7021-dsx.dts.patch"
SRC_URI += "file://dts/sp7021-dsx.dtsi.patch"
SRC_URI += "file://dts/sp7021-icognize.dts.patch"

# FB patch
SRC_URI += "file://video/fb_sp7021_main.c.set.patch"

# 485 test
SRC_URI += "file://uart_485/sp_uart.c.sleep1.patch"

# Goodix dbg
SRC_URI += "file://dbg_goodix/goodix.c.dbg.patch"

# mcp251xfd driver
do_patch:append() {
 cp -r ${WORKDIR}/mcp251xfd ${S}/drivers/net/can/spi/
}

#RDEPENDS:kernel-module-bcmdhd += "${@bb.utils.contains('DISTRO_FEATURES', 'wifi', 'bcmdhd-firmware', '', d)}"
RDEPENDS:kernel-module-bcmdhd += "bcmdhd-firmware"

#KBUILD_DEFCONFIG="pentagram_sc7021_achip_emu_defconfig"
#KERNEL_CONFIG_COMMAND = "oe_runmake_call -C ${S} O=${B} pentagram_sc7021_achip_emu_defconfig"
#KERNEL_CONFIG_COMMAND = "oe_runmake_call -C ${S} O=${B} defconfig"

#KERNEL_EXTRA_FEATURES += "features/initramfs/initramfs.scc"

KERNEL_FEATURES:append = " cfg/rpi-ovls/ovls-4.19.scc"

KERNEL_FEATURES:append = "${@bb.utils.contains("MACHINE_FEATURES", "touchscreen", " custom/touchscreen/all.scc", "" ,d)}"
KERNEL_FEATURES:append = "${@bb.utils.contains("MACHINE_FEATURES", "usbgadget", " features/usb/usb-gadgets.scc", "" ,d)}"
KERNEL_FEATURES:append = "${@bb.utils.contains("MACHINE_FEATURES", "3g", " custom/modems/protos.scc", "" ,d)}"
KERNEL_FEATURES:append = " ${@bb.utils.contains('MACHINE_FEATURES', 'vfat', 'cfg/fs/vfat.scc', '', d)}"
KERNEL_FEATURES:append = " custom/tunnel/udp.scc"
KERNEL_FEATURES:append = " cgl/net/l2tp.scc"
KERNEL_FEATURES:append = " custom/netfilter/addrtype.scc"

DEPENDS += "isp-native"
