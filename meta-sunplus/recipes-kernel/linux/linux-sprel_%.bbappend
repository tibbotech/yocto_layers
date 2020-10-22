FILESEXTRAPATHS_prepend := "${THISDIR}/linux-spdev:"

KBRANCH_tppg2 = "master"
#KBRANCH_tppg2 = "tibbo/muxZero"
#KBRANCH_tppg2 = "kernel_4.19"
KBRANCH_tppg2 = "tibbo/spi_exp"
# mainline
COMPATIBLE_MACHINE_tppg2 = "tppg2"

SRC_URI = "git://github.com/tibbotech/plus1_kernel.git;branch=${KBRANCH}"
# SRC_URI += "git://git.yoctoproject.org/yocto-kernel-cache;type=kmeta;name=meta;branch=yocto-4.12;destsuffix=${KMETA}"
SRC_URI += "file://kernel-meta/*;type=kmeta;name=meta;destsuffix=${KMETA}"

# 5.4 master
SRCREV = "e58c1790eb68099d5cbf08e368be84b7007bd076"
# 5.4 tibbo/spi_exp
SRCREV = "9a797a8dfef2bcbebb2bbc4ad266b08ddaabc923"
# 4.19
SRCREV_machine_tppg2 = "eee6766b2540822a9b229f64549299793a903f41"
# 5.4 master
SRCREV_machine_tppg2 = "e58c1790eb68099d5cbf08e368be84b7007bd076"
# 5.4 tibbo/spi_exp
SRCREV_machine_tppg2 = "9a797a8dfef2bcbebb2bbc4ad266b08ddaabc923"

# if using meta from master
#SRCREV_meta ?= "cebe198870d781829bd997a188cc34d9f7a61023"
LINUX_VERSION = "4.19.37"
LINUX_VERSION = "5.4.35"

#LINUX_KERNEL_TYPE = "debug"

MIRRORS=""
PREMIRRORS=""

KMETA="kernel-meta"

#SRC_URI += "file://defconfig"

SRC_URI += "file://dts/sp7021-ttt.dts.patch"
SRC_URI += "file://dts/sp7021-ublox.dts.patch"
#SRC_URI += "file://pinctrl_dbg/sppctl.c.err.patch"
#SRC_URI += "file://pinctrl_dbg/sp7021_gpio_ops.c.Fdbg.patch"
SRC_URI += "file://dts/sp7021-tpsgpio.dts.patch"
# for gpio-driven rts/cts
#SRC_URI += "file://uart_gpio_rtscts/sp_uart.c.dbg.patch"
#SRC_URI += "file://uart_gpio_rtscts/sp7021-tpstest.dts.4ctsrts.patch"
#SRC_URI += "file://uart_gpio_rtscts/sp7021-tpsgpio.dts.4ugpio.patch"
# SDIO debug
#SRC_URI += "file://sdio_dbg/spsdv2.c.err.patch"
#SRC_URI += "file://sdio_dbg/spsdv2.c.inf.patch"
# FIXME
#SRC_URI += "file://485/sp_uart.c.0.patch"
#SRC_URI += "file://485/serial_core.c.dbg0.patch"

# test
#SRC_URI += "file://dts/sp7021-common.dtsi.nodma.patch"

# tmp: spi debug
#SRC_URI += "file://dbg_spi/spi-sunplus-sp7021.c.dbg0.patch"
#SRC_URI += "file://dbg_spi/spi-sunplus-sp7021.c.dbg1.patch"
#SRC_URI += "file://dbg_spi/spi-sunplus-sp7021.c.csdeb.patch"
#SRC_URI += "file://dbg_spi/spi-sunplus-sp7021.c.cstst.patch"
#SRC_URI += "file://dbg_spi/spi-sunplus-sp7021.c.numcsdbg.patch"
#SRC_URI += "file://dbg_spi/spi-sunplus-sp7021.c.enpol.patch"

#SRC_URI += "file://dbg_spi/spi.c.dbg0.patch"
SRC_URI += "file://dbg_can/mcp251x.c.dbg0.patch"
#SRC_URI += "file://dbg_can/mcp251x.c.dbg1.patch"
#SRC_URI += "file://dbg_can/mcp251x.c.dbg2.patch"
SRC_URI += "file://dbg_can/mcp251x.c.clk0.patch"

# set GPIO to 83 for ltpp3g2
SRC_URI += "file://bcmdhd/dhd_gpio.c.gpio.patch"

# mcp25xxfd driver
#SRC_URI += "git://github.com/msperl/linux-rpi.git;branch=upstream-5.2-master-80f23-mcp25xxfd-v8.2;subpath=linux-rpi/drivers/net/can/spi/mcp25xxfd;destsuffix=drivers/net/can/spi/"
SRC_URI += "file://mcp25xxfd/mcp25xxfd.tar.gz"
SRC_URI += "file://mcp25xxfd/Kconfig.mcp25xxfd.patch"
SRC_URI += "file://mcp25xxfd/Makefile.mcp25xxfd.patch"

# mcp25xxfd driver
do_patch_append() {
 cp -r ${WORKDIR}/mcp25xxfd ${S}/drivers/net/can/spi/
}

#RDEPENDS_kernel-module-bcmdhd += "${@bb.utils.contains('DISTRO_FEATURES', 'wifi', 'bcmdhd-firmware', '', d)}"
RDEPENDS_kernel-module-bcmdhd += "bcmdhd-firmware"

#KBUILD_DEFCONFIG="pentagram_sc7021_achip_emu_defconfig"
#KERNEL_CONFIG_COMMAND = "oe_runmake_call -C ${S} O=${B} pentagram_sc7021_achip_emu_defconfig"
#KERNEL_CONFIG_COMMAND = "oe_runmake_call -C ${S} O=${B} defconfig"

#KERNEL_EXTRA_FEATURES += "features/initramfs/initramfs.scc"

#do_install_append() {
# install -m 0755 -d ${D}${base_libdir}/modules/${KERNEL_VERSION}/kernel/3rdparty
# install -m 0644 ${S}/tpsga1000v0${KERNEL_OBJECT_SUFFIX} ${D}${base_libdir}/modules/${KERNEL_VERSION}/kernel/3rdparty
# # to force load ASAP for non-builtin module: applications start too fast
## install -d ${D}${sysconfdir}/modules-load.d
## install -m 0644 ${S}/tpsleds.conf ${D}${sysconfdir}/modules-load.d/
#}

#KERNEL_FEATURES_append += "cfg/rpi-ovls/ovls-4.12.scc"
KERNEL_FEATURES_append += "cfg/rpi-ovls/ovls-4.19.scc"

LIC_FILES_CHKSUM = "file://COPYING;md5=bbea815ee2795b2f4230826c0c6b8814"
