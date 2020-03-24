FILESEXTRAPATHS_prepend := "${THISDIR}/linux-spdev:"

KBRANCH_tppg2 = "master"
#KBRANCH_tppg2 = "tibbo/muxZero"
#KBRANCH_tppg2 = "tibbo/noExportOTPfunc"
#KBRANCH_tppg2 = "tibbo/otp"
#KBRANCH_tppg2 = "kernel_4.12"
# mainline
COMPATIBLE_MACHINE_tppg2 = "tppg2"

SRC_URI = "git://github.com/tibbotech/plus1_kernel.git;branch=${KBRANCH};"
# SRC_URI += "git://git.yoctoproject.org/yocto-kernel-cache;type=kmeta;name=meta;branch=yocto-4.12;destsuffix=${KMETA}"
SRC_URI += "file://kernel-meta/*;type=kmeta;name=meta;destsuffix=${KMETA}"
#SRC_URI += "file://kernel-meta/bsp/pentagram/pentagram-standard.scc"

SRCREV = "b1ebc0e0b97f06e7fbc2a14ff84d020afc72d17d"
# 4.12
SRCREV_machine_tppg2 = "2e30599bd77bcfb6102ae45cdf47c40d7be55669"
# 4.19
SRCREV_machine_tppg2 = "b1ebc0e0b97f06e7fbc2a14ff84d020afc72d17d"

# if using meta from master
#SRCREV_meta ?= "cebe198870d781829bd997a188cc34d9f7a61023"
#LINUX_VERSION = "4.12.14"
LINUX_VERSION = "4.19.37"

#LINUX_KERNEL_TYPE = "debug"

MIRRORS=""
PREMIRRORS=""

KMETA="kernel-meta"

#SRC_URI += "file://defconfig"

# temporary there: delete after kernel-meta
#SRC_URI += "file://linux-yocto-Handle-bin-awk-issues.patch"

SRC_URI += "file://pentagram-sp7021-achip-ttt.dts.patch"
#SRC_URI += "file://pinctrl_dbg/sppctl.c.err.patch"
#SRC_URI += "file://pinctrl_dbg/sp7021_gpio_ops.c.Fdbg.patch"
SRC_URI += "file://sp7021-tpsgpio.dts.patch"
SRC_URI += "file://pentagram-sp7021-common.dtsi.sdio.patch"
# for gpio-driven rts/cts
#SRC_URI += "file://uart_gpio_rtscts/sp_uart.c.dbg.patch"
#SRC_URI += "file://uart_gpio_rtscts/sp7021-tpstest.dts.4ctsrts.patch"
#SRC_URI += "file://uart_gpio_rtscts/sp7021-tpsgpio.dts.4ugpio.patch"
SRC_URI += "file://sdio_dbg/spsdv2.c.err.patch"
#SRC_URI += "file://bcmdhd.ampak.patch"

#do_patch_append() {
# cp -r ${WORKDIR}/bcmdhd ${S}/drivers/net/wireless/bcmdhd
#}
#RDEPENDS_kernel-module-bcmdhd += "${@bb.utils.contains('DISTRO_FEATURES', 'wifi', 'broadcom-bcmdhd-firmware', '', d)}"
RDEPENDS_kernel-module-bcmdhd += "broadcom-bcmdhd-firmware"

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

LIC_FILES_CHKSUM = "file://COPYING;md5=d7810fab7487fb0aad327b76f1be7cd7"
LIC_FILES_CHKSUM = "file://COPYING;md5=bbea815ee2795b2f4230826c0c6b8814"
