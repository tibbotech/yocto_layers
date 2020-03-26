
include img-bai_15.0.0.inc

SRCREV_tibbo = "1f1390d82265c2b66dfa0613f32ad901f62bc387"
SRC_URI += "git://github.com/tibbotech/yocto_layers.git;branch=master;name=tibbo;destsuffix=xxx;"
SRCREV_oe = "9e60d30669a2ad0598e9abf0cd15ee06b523986b"
SRC_URI += "git://git.openembedded.org/meta-openembedded;branch=zeus;name=oe;destsuffix=xx0;"
SRCREV_qt5 = "6a9c0acbad3b9ac6b1e129c7c3c7b1648f4e4855"
SRC_URI += "git://github.com/meta-qt5/meta-qt5.git;branch=zeus;name=qt5;destsuffix=xx1;"
SRC_URI += "file://builder_session.sh.patch"

SRC_URI[qt5.md5sum] = "091caa442535d8d6b2a262007609a452"
SRC_URI[qt5.sha256sum] = "84c06b72504023f2754f835c477dd4db6b99f651244db24775da9305126eafd6"
SRC_URI[oe.md5sum] = "e54633a37bbfb78ba6e8be48adef17f5"
SRC_URI[oe.sha256sum] = "c2fc65d5878a75eea098365dde03359fe90a8aeee61cccffb3675dce09c97fbf"

do_copy_tibbo_layers () {
 rm -rf ${WORKDIR}/xxx/.git
 rm -rf ${WORKDIR}/xx0/.git
 rm -rf ${WORKDIR}/xx1/.git
 install -d ${WORKDIR}/git/meta-openembedded
 install -d ${WORKDIR}/git/meta-qt5
 cp -R ${WORKDIR}/xxx/* ${WORKDIR}/git/
 cp -R ${WORKDIR}/xx0/* ${WORKDIR}/git/meta-openembedded/
 cp -R ${WORKDIR}/xx1/* ${WORKDIR}/git/meta-qt5/
}

IMAGE_INSTALL += "pstree mariadb-client mariadb-leftovers libmysqlclient-dev"

fakeroot do_xxx () {
 install -d -o builder -g builder ${IMAGE_ROOTFS}/disk2
 sed -i -e "s/oe-init-build-env/oe-init-build-env c.tppg2/" ${IMAGE_ROOTFS}/etc/mini_x/session.d/builder_session.sh
}

IMAGE_PREPROCESS_COMMAND += "do_xxx; "

addtask copy_tibbo_layers after do_unpack before do_prepare_recipe_sysroot

DEPENDS += "rsync-native"
