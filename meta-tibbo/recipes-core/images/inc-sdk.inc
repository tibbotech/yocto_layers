
inherit populate_sdk_qt5

#TOOLCHAIN_TARGET_TASK_append += " fltk"
#TOOLCHAIN_TARGET_TASK_append += " fltk-staticdev"

# better to install NodeJS from https://nodejs.org/en/download/releases/
TOOLCHAIN_HOST_TASK_append += " nativesdk-swig"

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
#TOOLCHAIN_HOST_TASK_append += " nativesdk-soem"

# for QT to build with SDK
TOOLCHAIN_HOST_TASK_append += " nativesdk-perl-module-english"
TOOLCHAIN_HOST_TASK_append += " nativesdk-perl-module-tie-hash-namedcapture"
TOOLCHAIN_HOST_TASK_append += " nativesdk-libstdc++-dev"
#TOOLCHAIN_HOST_TASK_append += " nativesdk-libstdc++-staticdev"
TOOLCHAIN_TARGET_TASK_append += " libstdc++-staticdev"

# for autogen: need it in ..86-tpssdk-linux (crosstools)
# it is a bug in meta/meta-devtools autogen dependencies
TOOLCHAIN_HOST_TASK_append += " nativesdk-perl-modules"

#TOOLCHAIN_TARGET_TASK_append += " pjproject-dev pjproject-staticdev"

TOOLCHAIN_TARGET_TASK_append += " openssl-dev"

#DEPENDS += "packagegroup-self-hosted-sdk"
DEPENDS += "packagegroup-core-sdk"

#DEPENDS += "bluez-alsa"
## for Vitaly bluez-alsa compiling with SDK (auto dependencies)
#TOOLCHAIN_TARGET_TASK_append += " bluez-alsa"
