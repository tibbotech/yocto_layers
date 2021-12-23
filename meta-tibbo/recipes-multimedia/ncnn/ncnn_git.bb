require ncnn.inc

SRCREV = "${AUTOREV}"
SRC_URI = "git://github.com/ElegantGod/ncnn.git;protocol=https;branch=master"

SRC_URI += " file://fa0.patch \
 file://mt0.patch \
 file://sq0.patch \
 file://ss0.patch \
"

do_install:append() {
 install -d ${D}${bindir}/
 install -m 0755 ${B}/mtcnn/mtcnn ${D}${bindir}/mtcnn
}

LIC_FILES_CHKSUM = "file://LICENSE.txt;md5=8c8b5b19ef89ee9cfa47e768aa904935;rev={SRCREV}"
