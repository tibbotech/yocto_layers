
inherit populate_sdk populate_sdk_qt5

#TOOLCHAIN_TARGET_TASK += "fltk"
#TOOLCHAIN_TARGET_TASK += "fltk-staticdev"

# better to install NodeJS from https://nodejs.org/en/download/releases/
TOOLCHAIN_HOST_TASK += "nativesdk-swig"

TOOLCHAIN_HOST_TASK += "nativesdk-rpm-build"
TOOLCHAIN_HOST_TASK += "nativesdk-make"

TOOLCHAIN_TARGET_TASK += "kernel-devsrc"

TOOLCHAIN_TARGET_TASK += "boost-dev"
TOOLCHAIN_TARGET_TASK += "boost-staticdev"

TOOLCHAIN_TARGET_TASK += "log4cpp-dev"
#TOOLCHAIN_TARGET_TASK += "dlib-dev"

# OpenAMP
#TOOLCHAIN_TARGET_TASK += "${@bb.utils.contains("DISTRO_FEATURES", "openamp", "libopen-amp-dev libopen-amp-staticdev", "",d)}"
TOOLCHAIN_TARGET_TASK += "${@bb.utils.contains("DISTRO_FEATURES", "openamp", "libmetal open-amp", "",d)}"
# OpenAMP /

TOOLCHAIN_TARGET_TASK += "soem-dev soem-staticdev"
#TOOLCHAIN_HOST_TASK += "nativesdk-soem"

# for QT to build with SDK
TOOLCHAIN_HOST_TASK += "nativesdk-perl-module-english"
TOOLCHAIN_HOST_TASK += "nativesdk-perl-module-tie-hash-namedcapture"
TOOLCHAIN_HOST_TASK += "nativesdk-libstdc++-dev"
#TOOLCHAIN_HOST_TASK += "nativesdk-libstdc++-staticdev"
TOOLCHAIN_TARGET_TASK += "libstdc++-staticdev"

# for autogen: need it in ..86-tpssdk-linux (crosstools)
# it is a bug in meta/meta-devtools autogen dependencies
TOOLCHAIN_HOST_TASK += "nativesdk-perl-modules"

#TOOLCHAIN_TARGET_TASK += "pjproject-dev pjproject-staticdev"

TOOLCHAIN_TARGET_TASK += "openssl-dev"

#DEPENDS += "packagegroup-self-hosted-sdk"
DEPENDS += "packagegroup-core-sdk"

#DEPENDS += "bluez-alsa"
## for Vitaly bluez-alsa compiling with SDK (auto dependencies)
#TOOLCHAIN_TARGET_TASK += "bluez-alsa"
