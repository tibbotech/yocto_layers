SRC_URI_append_tppg2x += "\
 file://sunplus0.mach.patch \
 file://sunplus0.crypto.patch \
 file://sunplus0.display.patch \
 file://sunplus0.gpio.patch \
 file://sunplus0.i2c.patch \
 file://sunplus0.net.patch \
 file://sunplus0.pwm.patch \
 file://sunplus0.scsi.patch \
 file://sunplus0.usb.patch \
 file://defconfig \
 file://initramfs.devnodes \
"

SRC_URI_append_tppg2 += "\
 file://sp0.patch \
 file://defconfig \
 file://initramfs.devnodes \
 file://cfg/nfs.cfg \
"
