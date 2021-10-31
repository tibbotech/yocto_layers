FILESEXTRAPATHS_prepend := "${THISDIR}/linux-spdev:"

KBRANCH_tppg2 = "master"
#KBRANCH_tppg2 = "tibbo/muxZero"
#KBRANCH_tppg2 = "kernel_4.19"
#KBRANCH_tppg2 = "tibbo/spi_exp"
#KBRANCH_tppg2 = "tibbo/spi_exp2"
#KBRANCH_tppg2 = "tibbo/485"
#KBRANCH_tppg2 = "tibbo/spi256"
# mainline
COMPATIBLE_MACHINE_tppg2 = "tppg2"

SRC_URI = "git://github.com/tibbotech/plus1_kernel.git;branch=${KBRANCH}"
# SRC_URI += "git://git.yoctoproject.org/yocto-kernel-cache;type=kmeta;name=meta;branch=yocto-4.12;destsuffix=${KMETA}"
SRC_URI += "file://kernel-meta/*;type=kmeta;name=meta;destsuffix=${KMETA}"
#SRC_URI += "file://kernel-meta.tar.gz;type=kmeta;name=meta;destsuffix=${KMETA}"

# 5.4 master
SRCREV = "3d2a894c9d9038ab101e9b3917e8587365e689b7"
## 5.4 tibbo/spi_exp
#SRCREV = "1cc820c8fb982b0dc359513dc069bba4e097fcd1"
## 5.4 tibbo/spi_exp2
#SRCREV = "acd013f0a071284c010f25bc71b7b96d042bfcc1"
# 5.4 master
SRCREV = "b37cef136f721442d7daa36aab08079206d1d8e4"
# 4.19
SRCREV_machine_tppg2 = "e81c7196d43ee83e0c05a9ac666cfe7a5fbd2ce9"
# 5.4 master
SRCREV_machine_tppg2 = "3d2a894c9d9038ab101e9b3917e8587365e689b7"
## 5.4 tibbo/spi_exp
#SRCREV_machine_tppg2 = "1cc820c8fb982b0dc359513dc069bba4e097fcd1"
## 5.4 tibbo/spi_exp2
#SRCREV_machine_tppg2 = "acd013f0a071284c010f25bc71b7b96d042bfcc1"
# 5.4 master
SRCREV_machine_tppg2 = "b37cef136f721442d7daa36aab08079206d1d8e4"

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

SRC_URI += "file://pinctrl_dbg/gpioirq.patch"

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

# tmp: spi debug
#SRC_URI += "file://dbg_spi/spi-sunplus-sp7021.c.dbg0.patch"
#SRC_URI += "file://dbg_spi/spi-sunplus-sp7021.c.dbg1.patch"
#SRC_URI += "file://dbg_spi/spi-sunplus-sp7021.c.norxtxdbg.patch"
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
#SRC_URI += "git://github.com/tibbotech/mcp251xfd.git;branch=master"

# +dts
SRC_URI += "file://dts/sp7021-dsx.dts.patch"
SRC_URI += "file://dts/sp7021-dsx.dtsi.patch"

# FB patch
SRC_URI += "file://video/fb_sp7021_main.c.set.patch"

# 485 test
SRC_URI += "file://uart_485/sp_uart.c.sleep1.patch"

# mcp251xfd driver
do_patch_append() {
 cp -r ${WORKDIR}/mcp251xfd ${S}/drivers/net/can/spi/
}

do_deploy_append() {
 dd="${DEPLOYDIR}"
 if [ -n "${KERNEL_DEPLOYSUBDIR}" ]; then
   dd="${DEPLOYDIR}/${KERNEL_DEPLOYSUBDIR}"
 fi;
 echo "dv kern deploy:${KERNEL_IMAGE_NAME} - ${KERNEL_IMAGE_LINK_NAME} i: ${INITRAMFS_NAME} - ${INITRAMFS_LINK_NAME}"
 for imgType in ${KERNEL_IMAGETYPES} ; do
    echo "dv img type: ${imgType}"
    fn="${imgType}-${KERNEL_IMAGE_NAME}"
    if [ -f "${dd}/${fn}.bin" ]; then
      echo "dv0 : ${dd}/${fn}.bin"
      install ${dd}/${fn}.bin ${dd}/${fn}.img
      ${STAGING_DIR_NATIVE}/sp_tools/secure_sign/gen_signature.sh ${dd} ${fn}.img 1
      ln -sf ${fn}.img ${dd}/${imgType}-${KERNEL_IMAGE_LINK_NAME}.img
    fi;
    fn="${imgType}-${INITRAMFS_NAME}"
    if [ -f "${dd}/${fn}.bin" ]; then
      echo "dv1 : ${dd}/${fn}.bin"
      install ${dd}/${fn}.bin ${dd}/${fn}.img
      ${STAGING_DIR_NATIVE}/sp_tools/secure_sign/gen_signature.sh ${dd} ${fn}.img 1
      ln -sf ${fn}.img ${dd}/${imgType}-${INITRAMFS_LINK_NAME}.img
    fi;
 done;
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

KERNEL_FEATURES_append += "${@bb.utils.contains("MACHINE_FEATURES", "touchscreen", " custom/touchscreen/all.scc", "" ,d)}"
KERNEL_FEATURES_append += "${@bb.utils.contains("MACHINE_FEATURES", "usbgadget", " features/usb/usb-gadgets.scc", "" ,d)}"

LIC_FILES_CHKSUM = "file://COPYING;md5=bbea815ee2795b2f4230826c0c6b8814"

DEPENDS += "isp-native"
