DESCRIPTION = "ncnn is a high-performance neural network inference framework optimized for the mobile platform"
LICENSE = "GPLv2"
SECTION = "libs/multimedia"

INC_PR = "r0"

#DEPENDS  = "protobuf (= 3.7.1) opencv"
DEPENDS  = "protobuf protobuf-native opencv"
S = "${WORKDIR}/git"

#inherit cmake pkgconfig
inherit cmake

#EXTRA_OECMAKE += "-DENABLE_TESTS_COMPONENT=OFF -DCMAKE_SKIP_INSTALL_RPATH=ON"
