
include img-bai_15.0.0.inc

IMAGE_ROOTFS_EXTRA_SPACE = "104857600"

SRC_URI += "file://npm.dunfell.patch"

SRCREV = "795339092f87672e4f68e4d3bc4cfd0e252d1831"

SRCREV_tibbo = "4d9d2f4c4f153211598cfae69e4d49ecc0750fa4"
SRC_URI += "git://github.com/tibbotech/yocto_layers.git;protocol=https;branch=dunfell;name=tibbo;destsuffix=xxx;"
SRCREV_oe = "69f94af4d91215e7d4e225bab54bf3bcfee42f1c"
SRC_URI += "git://git.openembedded.org/meta-openembedded;branch=dunfell;name=oe;destsuffix=xx0;"
SRCREV_qt5 = "b4d24d70aca75791902df5cd59a4f4a54aa4a125"
SRC_URI += "git://github.com/meta-qt5/meta-qt5.git;protocol=https;branch=dunfell;name=qt5;destsuffix=xx1;"
SRCREV_amp = "bdbdb0b02af4800a0c7fc70edef33cfd55babbdd"
SRC_URI += "git://github.com/OpenAMP/meta-openamp.git;protocol=https;branch=master;name=amp;destsuffix=xx2;"
SRC_URI += "file://builder_session.sh.patch"
SRC_URI += "file://clear_sock.sh"

SRC_URI[qt5.md5sum] = "091caa442535d8d6b2a262007609a452"
SRC_URI[qt5.sha256sum] = "84c06b72504023f2754f835c477dd4db6b99f651244db24775da9305126eafd6"
SRC_URI[oe.md5sum] = "e54633a37bbfb78ba6e8be48adef17f5"
SRC_URI[oe.sha256sum] = "c2fc65d5878a75eea098365dde03359fe90a8aeee61cccffb3675dce09c97fbf"

do_copy_tibbo_layers () {
 rm -rf ${WORKDIR}/xxx/.git
 rm -rf ${WORKDIR}/xx0/.git
 rm -rf ${WORKDIR}/xx1/.git
 rm -rf ${WORKDIR}/xx2/.git
 install -d ${WORKDIR}/git/meta-openembedded
 install -d ${WORKDIR}/git/meta-qt5
 install -d ${WORKDIR}/git/meta-openamp
 cp -R ${WORKDIR}/xxx/* ${WORKDIR}/git/
 cp -R ${WORKDIR}/xx0/* ${WORKDIR}/git/meta-openembedded/
 cp -R ${WORKDIR}/xx1/* ${WORKDIR}/git/meta-qt5/
 cp -R ${WORKDIR}/xx2/* ${WORKDIR}/git/meta-openamp/
 sed -i 's/"gatesgarth"/"rocko sumo thud zeus dunfell gatesgarth"/g' ${WORKDIR}/git/meta-openamp/conf/layer.conf
 patch -d ${WORKDIR}/git/ -p1 < ${WORKDIR}/npm.dunfell.patch
}

IMAGE_INSTALL += "pstree mariadb-client mariadb-leftovers libmysqlclient-dev"

fakeroot do_xxx () {
 install -d -m 0777 -o builder -g builder ${IMAGE_ROOTFS}/disk2
 sed -i -e "s/oe-init-build-env/oe-init-build-env build.tppg2/" ${IMAGE_ROOTFS}/etc/mini_x/session.d/builder_session.sh
 cp -R ${WORKDIR}/clear_sock.sh ${IMAGE_ROOTFS}/home/builder/
}

IMAGE_PREPROCESS_COMMAND += "do_xxx; "

addtask copy_tibbo_layers after do_unpack before do_prepare_recipe_sysroot

DEPENDS += "rsync-native"
