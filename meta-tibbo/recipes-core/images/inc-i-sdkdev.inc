
# GUI
inherit populate_sdk_qt5
#TOOLCHAIN_TARGET_TASK_append += " fltk"
#TOOLCHAIN_TARGET_TASK_append += " fltk-staticdev"

# NodeJS
#TOOLCHAIN_TARGET_TASK_append += " nodejs"
TOOLCHAIN_HOST_TASK_append += " nativesdk-swig"
# better to install NodeJS from https://nodejs.org/en/download/releases/
#TOOLCHAIN_HOST_TASK_append += " nativesdk-nodejs nativesdk-nodejs-npm"

TOOLCHAIN_HOST_TASK_append += " nativesdk-rpm-build"
TOOLCHAIN_HOST_TASK_append += " nativesdk-make"

TOOLCHAIN_TARGET_TASK_append += " kernel-devsrc"

TOOLCHAIN_TARGET_TASK_append += " boost-dev"
TOOLCHAIN_TARGET_TASK_append += " boost-staticdev"

TOOLCHAIN_TARGET_TASK_append += " log4cpp-dev"
#TOOLCHAIN_TARGET_TASK_append += " dlib-dev"

# OpenAMP
#TOOLCHAIN_TARGET_TASK_append += "${@bb.utils.contains("DISTRO_FEATURES", "openamp", "libopen-amp-dev libopen-amp-staticdev", "",d)}"
TOOLCHAIN_TARGET_TASK_append += "${@bb.utils.contains("DISTRO_FEATURES", "openamp", "libmetal open-amp", "",d)}"
# OpenAMP /

TOOLCHAIN_TARGET_TASK_append += " soem-dev soem-staticdev"
