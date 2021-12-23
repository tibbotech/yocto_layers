#PACKAGECONFIG  = "python3 eigen jpeg png tiff v4l libv4l gstreamer samples tbb gphoto2 \
#    ${@bb.utils.contains("DISTRO_FEATURES", "x11", "gtk", "", d)} \
#    ${@bb.utils.contains("LICENSE_FLAGS_WHITELIST", "commercial", "libav", "", d)}"
#PACKAGECONFIG += " python3"

PACKAGECONFIG += "dnn"
PACKAGECONFIG += "python3"

EXTRA_OECMAKE += "-DENABLE_NEON=ON"
