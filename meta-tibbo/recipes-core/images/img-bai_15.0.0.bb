
include img-bai_15.0.0.inc

SRCREV_tibbo = "5697c448fb5c360a032088e714563750015a0785"
SRC_URI += "git://github.com/tibbotech/yocto_layers.git;branch=master;name=tibbo;destsuffix=xxx;"
SRC_URI += "file://builder_session.sh.patch"

do_copy_tibbo_layers () {
 rm -rf ${WORKDIR}/xxx/.git
 cp -R ${WORKDIR}/xxx/* ${WORKDIR}/git/
}

IMAGE_INSTALL += "pstree mariadb-client mariadb-leftovers libmysqlclient-dev"

fakeroot do_xxx () {
 install -d -o builder -g builder ${IMAGE_ROOTFS}/disk2
 sed -i -e "s/oe-init-build-env/oe-init-build-env c.tppg2/" ${IMAGE_ROOTFS}/etc/mini_x/session.d/builder_session.sh
}

IMAGE_PREPROCESS_COMMAND += "do_xxx; "

addtask copy_tibbo_layers after do_unpack before do_prepare_recipe_sysroot

DEPENDS += "rsync-native"
