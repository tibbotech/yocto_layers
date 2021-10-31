
#DEPENDS += "packagegroup-core-sdk"

#inherit populate_sdk

# XBoot fake image should be build too
#do_populate_sdk[mcdepends] += "mc:tpp-${MACHINE}:tpp-${MACHINE}-arm5:imgf-xboot:do_populate_sdk"
#do_image_complete[mcdepends] += "mc:tpp-${MACHINE}:tpp-${MACHINE}-arm5:imgf-xboot:do_image_complete"

do_install[mcdepends] += "mc::tpp-${MACHINE}-arm5:imgf-xboot:do_image_complete"
