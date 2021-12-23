
do_install:append() {
 install -d ${D}/var/lib/mysql
 chown mysql:mysql ${D}/var/lib/mysql
}

FILES:${PN}-server += "/var/lib/mysql"

DEPENDS += "libpcre"
