DESCRIPTION:append = " +MD press -> SD"

require xboot-emmc_sp.bb

do_configure:append() {
 echo "CONFIG_CUSTOM_BOOT_BTN=3" >> ${S}/.config
 echo "CONFIG_CUSTOM_BTN_DEV=SDCARD_ISP" >> ${S}/.config
}
