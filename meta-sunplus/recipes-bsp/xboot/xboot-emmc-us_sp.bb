DESCRIPTION:append = " +MD press -> USB"

require xboot-emmc_sp.bb

do_configure:append() {
 echo "CONFIG_CUSTOM_BOOT_BTN=3" >> ${S}/.config
 echo "CONFIG_CUSTOM_BTN_DEV=USB_ISP" >> ${S}/.config
}
