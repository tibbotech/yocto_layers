
require inc-lic.inc
require inc-gen-tppg2.inc

do_install[mcdepends] += "mc::tpp-${MACHINE}:img-tst-tini:do_image_complete"
do_install[mcdepends] += "mc::tpp-${MACHINE}:img-tps-base:do_image_complete"

# Build Appliance
#do_install[mcdepends] += "mc::qemux86-64:img-bai:do_image_complete"
