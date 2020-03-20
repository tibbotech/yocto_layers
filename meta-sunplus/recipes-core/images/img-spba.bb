
require img-lic.inc

# Build Appliance
do_install[mcdepends] += "multiconfig::qemux86-64:img-bai:do_image_complete"
