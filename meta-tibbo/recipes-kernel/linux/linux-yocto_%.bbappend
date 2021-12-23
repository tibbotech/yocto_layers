FILESEXTRAPATHS:prepend := "${THISDIR}/linux-yocto:"

KBRANCH:tppg1 = "standard/beaglebone"
KBRANCH:tppg2 = "standard/base"
# later: add pentagram patches to the Yocto kernel tree and set
#KBRANCH:tppg2 = "standard/pentagram"

# before we did not submit machine to kernel tree
COMPATIBLE_MACHINE_tppg1 = "tppg1"
COMPATIBLE_MACHINE_tppg2 = "tppg2"

# 4.12.14 !
SRCREV_tppg1 ?= "9cc6b0ae1aad7312e85ac4134398f81c0140de33"
SRCREV_tppg2 ?= "1348b764f8ca8803b6b833c69f5cb795fddf24b8"
SRCREV_meta ?= "cebe198870d781829bd997a188cc34d9f7a61023"
LINUX_VERSION_tppg1 = "4.12.28"

## 4.12.14 !
#SRCREV_tppg2 ?= "1348b764f8ca8803b6b833c69f5cb795fddf24b8"
#SRCREV_meta ?= "cebe198870d781829bd997a188cc34d9f7a61023"
#LINUX_VERSION = "4.12.28"

SRC_URI:append:tppg1 += "\
 file://cfg/from_beagle.cfg \
 file://cfg/firmware_loadable.cfg \
 file://cfg/dts.cfg \
 file://cfg/timestamps.cfg \
 file://cfg/wireless_ext.cfg \
 file://cfg/preempt.cfg \
 file://cfg/atheros.cfg \
 file://cfg/bpf.cfg \
 file://cfg/w1.cfg \
"
# rtc: tps65910, twl<X>0<Y>0, tps659<x>10
# eeprom: generic for i2c
# phy: no smsc, use icplus
# gpio: I2C_GPIO, GPIO_TPS65910
SRC_URI:append:tppg1 += "\
 file://cfg/tpp_rtc.cfg \
 file://cfg/tpp_eeprom.cfg \
 file://cfg/tpp_netdev.cfg \
 file://cfg/tpp_usb.cfg \
 file://cfg/tpp_phy.cfg \
 file://cfg/tpp_gpio.cfg \
 file://cfg/tpp_misc.cfg \
 file://cfg/tpp_leds.cfg \
 file://cfg/tpp_pwm.cfg \
"
# filesystems on/off
SRC_URI:append:tppg1 += "\
 file://cfg/tpp_fs.cfg \
 file://cfg/can.cfg \
 file://cfg/can_dbg.cfg \
"
# sound
SRC_URI:append:tppg1 += "\
 file://cfg/tpp_sound.cfg \
"
# trying to optimize
SRC_URI:append:tppg1 += "\
 file://cfg/tpp_fast.cfg \
 file://cfg/tpp_cpufreq.cfg \
 file://cfg/odd_settings.cfg \
"
SRC_URI:append:tppg1 += "\
 file://cfg/irq_dbg.cfg \
"
# file://cfg/dbg_locks.cfg

# usb gadget debug
# file://dbg/musb_gadget.c.patch
# file://dbg/musb_gadget_ep0.c.patch
# file://dbg/0-epautoconf.c.patch
# file://dbg/1-epautoconf.c.patch

# test
#SRC_URI:append:tppg1 += "\
# file://dbg-3.17/dtimer.c.patch \
# file://dbg-3.17/dtimer.h.patch \
# file://dbg-3.17/timer.dbg.patch \
#"

# dtsi definition for TPPs3 platform
SRC_URI:append:tppg1 += "\
 file://dts-patches-4.1/am33xx.tpp.dtsi.patch \
 file://dts-patches-4.1/am335x-TPP.dtsi.patch \
 file://dts-patches-4.1/am335x-TPPA.dtsi.patch \
 file://dts-patches-4.1/am335x-TPPA-t-usb.dts.patch \
 file://dts-patches-4.1/am335x-TPPs2.dts.patch \
 file://dts-patches-4.1/am335x-TPPs3.dtsi.patch \
 file://dts-patches-4.1/am335x-TPPs3.dts.patch \
 file://dts-patches-4.1/am335x-TPPs3-t-gpio.dts.patch \
 file://dts-patches-4.1/am335x-TPPs3-t-buzzer.dts.patch \
 file://dts-patches-4.1/am335x-TPPs3-t-can.dts.patch \
 file://dts-patches-4.1/am335x-TPPs3-t-i2c.dts.patch \
 file://dts-patches-4.1/am335x-TPPs3-t-uart.dts.patch \
 file://dts-patches-4.1/am335x-TPPs3-t-usb.dts.patch \
 file://dts-patches-4.1/am335x-TPPs3-t-usb-host.dts.patch \
 file://dts-patches-4.1/am335x-TPPs3-t-usb-peri.dts.patch \
 file://dts-patches-4.1/am335x-TPPs3-t-mmc.dts.patch \
 file://dts-patches-4.1/am335x-TPPs3-t-sound.dts.patch \
 file://dts-patches-4.1/am335x-TPPs3-RLP.dts.patch \
 file://dts-patches-4.1/am335x-TPPs3-fwuploader.dts.patch \
 file://dts-patches-4.1/am335x-KLM601.dts.patch \
 file://dts-patches-4.1/am335x-KLM601.revB.dts.patch \
 file://dts-patches-4.1/am335x-KLM601.revC.dts.patch \
"
# file://dts-patches/am33xx.dtsi.pwmss.patch
# file://dts-patches/am33xx-clocks.dtsi.patch
# file://dts-patches-tests/dtsi.timer4.patch

# i2c-gpio patch
#SRC_URI:append:tppg1 += "\
# file://dbg/i2c-gpio.c.patch \
#"

# i2c-algo-bit debug
#SRC_URI:append:tppg1 += "\
# file://dbg/i2c-algo-bit.c.patch \
# file://dbg/rtc-tps65910.c.dbg.patch \
# file://dbg/tps65910.c.dbg.patch \
#"

#SRC_URI:append:tppg1 += "\
# file://dbg-timers-4.1/timer.dbg.patch \
# file://dbg-timers-4.1/dtimer.c.patch \
# file://dbg-timers-4.1/dtimer.h.patch \
#"

# MMC over SPI
SRC_URI:append:tppg1 += "\
 file://mmcspi.Kconfig.patch \
"

# TPS ga10000 hw SPI
SRC_URI:append:tppg1 += "\
 file://cfg/libertas.cfg \
"
# Keytroller LCD
SRC_URI:append:tppg1 += "\
 file://cfg/klm_lcd.cfg \
"

# Tibbit 02 (232/422/485)
SRC_URI:append:tppg1 += "\
 file://tibbits/omap-serial.c.patch \
"
# file://tibbits/omap-serial.c.dbg.patch
# file://tibbits/omap-serial.c.dbg2.patch
# file://tibbits/omap-serial.c.nocts.patch

# SPI Tibbits user-space spidev
SRC_URI:append:tppg1 += "\
 file://tibbits/spidev.c.patch \
"
# USB rx/tx mess patch for < 4.4.21
#SRC_URI:append:tppg1 += "\
# file://usb-patch/musb_host.c.rxtx.patch \
#"

# liber debug
#SRC_URI:append:tppg1 += "\
# file://dbg-liber/main.c.dbg.patch \
#"
# file://dbg-liber/if_spi.c.patch 
# file://dbg-liber/if_spi.c.2.patch 
# file://dbg-liber/main.c.patch 
# file://dbg-liber/main.c.2.patch 
# file://dbg-liber/main.c.dbg.patch 

# Mic Bias debugging (not needed now)
# file://dbg-sound/tlv320aic3x.c.patch

# tmp
SRC_URI:append:tppg1 += "\
 file://cfg/wifi.cfg \
"
# tmp /

# ade7758 power meter patch to show the freq
SRC_URI:append:tppg1 += "\
 file://ade7758_core.c.patch \
 file://ade7758_core.c.comp.patch \
 file://ade7854.c.patch \
"
# file://spi-omap2-mcspi.c.patch

#SRC_URI:append:tppg1 += "file://omap_fbdev.c.patch"

# SRC_URI:append:tppg1 += "file://tilcdc.1.dbg.patch"
# SRC_URI:append:tppg1 += "file://tilcdc.2.dbg.patch"
# SRC_URI:append:tppg1 += "file://tilcdc.3.dbg.patch"
# SRC_URI:append:tppg1 += "file://tilcdc.4.dbg.patch"
# SRC_URI:append:tppg1 += "file://tilcdc.5.dbg.patch"
# SRC_URI:append:tppg1 += "file://tilcdc.6.dbg.patch"

SRC_URI:append:tppg1 += "file://tilcdc.reset.patch"

# SunPlus
#SRC_URI:append:tppg2 += "file://SP/*"

# Functionality flags
#KERNEL_FEATURES:append:tppg1 += "cape-3.19/cape.scc"
#KERNEL_FEATURES:append:tppg1 += "cfg/ovls/ovls.scc"
KERNEL_FEATURES:append:tppg1 += "features/media/media-usb-webcams.scc"
KERNEL_FEATURES:append:tppg2 += "features/media/media-usb-webcams.scc"
KERNEL_FEATURES:append:tppg1 += "features/netfilter/netfilter.cfg"
KERNEL_FEATURES:append:tppg2 += "features/netfilter/netfilter.cfg"
KERNEL_FEATURES:append:tppg1 += "features/usb/serial-all.scc"
KERNEL_FEATURES:append:tppg2 += "features/usb/serial-all.scc"
KERNEL_FEATURES:append:tppg1 += "cfg/rpi-ovls/ovls.scc"
#KERNEL_FEATURES:append:tppg2 += "cfg/rpi-ovls/ovls.scc"

#KBUILD_DEFCONFIG_tppg2 = "pentagram_sc7021_achip_emu_defconfig"
