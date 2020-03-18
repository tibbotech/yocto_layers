
# possible values:
# gh - public github version
# sp - private SP version

xboot_v = "gh"
PREFERRED_VERSION_xboot = "${xboot_v}"
PREFERRED_VERSION_xboot-nand = "${xboot_v}"
PREFERRED_VERSION_xboot-emmc = "${xboot_v}"
PREFERRED_VERSION_xboot-tools = "${xboot_v}"
PREFERRED_VERSION_xboot-tools-native = "${xboot_v}"
PREFERRED_VERSION_xboot-helpers = "${xboot_v}"

PREFERRED_VERSION_isp-native = "gh"

uboot_v = "gh"
PREFERRED_VERSION_u-boot-fw-utils = "${uboot_v}"
PREFERRED_VERSION_u-boot-tools = "${uboot_v}"
PREFERRED_VERSION_u-boot = "${uboot_v}"

# possible values:
# linux-sprel - public github version
# linux-spdev - private SP version
# linux-yocto - traditional Yocto version

linux_v = "spdev"
PREFERRED_PROVIDER_virtual/kernel = "linux-${linux_v}"
