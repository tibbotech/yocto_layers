
# copy of /meta/recipes-core/images/build-appliance-image_15.0.0.bb
#SRCREV = "12a4c177bb541b3187c7a54d5804f30c35f22d8c"

include img-bai_15.0.0.inc

#SRC_URI = "..."

IMAGE_INSTALL += "pstree mariadb-client mariadb-leftovers libmysqlclient-dev"

fakeroot do_xxx () {
 install -d -o builder -g builder ${IMAGE_ROOTFS}/disk2
}

IMAGE_PREPROCESS_COMMAND += "do_xxx; "
