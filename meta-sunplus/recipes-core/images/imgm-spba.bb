# macro "image" to build Build Applience

require inc-lic.inc

# Build Appliance
do_install[mcdepends] += "mc::qemux86-64:img-bai:do_image_complete"
