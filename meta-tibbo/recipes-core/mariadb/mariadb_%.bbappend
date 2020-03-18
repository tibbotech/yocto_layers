
do_install_append() {
 install -d ${D}/var/lib/mysql
 chown mysql:mysql ${D}/var/lib/mysql
}

FILES_${PN}-server += "/var/lib/mysql"

DEPENDS += "libpcre"
