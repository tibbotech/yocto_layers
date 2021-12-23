# copy of meta/recipes-bsp/u-boot/u-boot_2021.07.bb with
# -src-gh instead of -common

require u-boot-src-gh.inc
require u-boot.inc

DEPENDS += "bc-native dtc-native python3-setuptools-native"
