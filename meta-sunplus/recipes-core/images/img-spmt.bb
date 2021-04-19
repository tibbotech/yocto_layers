
require img-lic.inc
require img-gen.inc

# XBoot
do_install[mcdepends] += "multiconfig::tpp-${MACHINE}-arm5:img-xboot:do_image_complete"

#do_install[mcdepends] += "multiconfig::tpp-${MACHINE}:img-sp-tiny:do_image_complete"
#do_install[mcdepends] += "multiconfig::tpp-${MACHINE}:img-sp-tini:do_image_complete"
#do_install[mcdepends] += "multiconfig::tpp-${MACHINE}:img-sp-tssz:do_image_complete"

# actual TPS image
do_install[mcdepends] += "multiconfig::tpp-${MACHINE}:img-tps-base:do_image_complete"
do_install[mcdepends] += "multiconfig::tpp-${MACHINE}:img-tps-basd:do_image_complete"
do_install[mcdepends] += "multiconfig::tpp-${MACHINE}:img-tps-tini:do_image_complete"
do_install[mcdepends] += "multiconfig::tpp-${MACHINE}:img-tps-tssz:do_image_complete"
do_install[mcdepends] += "multiconfig::tpp-${MACHINE}:img-tps-ublx:do_image_complete"

# Build Appliance
#do_install[mcdepends] += "multiconfig::qemux86-64:img-bai:do_image_complete"

# SDK
do_populate_sdk[mcdepends] += "multiconfig::tpp-${MACHINE}:img-tps-tssz:do_populate_sdk"
