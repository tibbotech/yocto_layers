SUMMARY = "Dynamic DNS Update Client"
PR = "r1"

inherit systemd

LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYRIGHT;md5=ed4b1320404827af6b24b60c0c53656f \
                    file://COPYING;md5=0636e73ff0215e8d672dc4c32c317bb3"

SRC_URI = "https://sourceforge.net/projects/ddclient/files/ddclient/ddclient-${PV}/ddclient-${PV}.tar.bz2 \
	   file://persistent \
	   file://ddclient.service \
	"
SRC_URI[sha256sum] = "d40e2f1fd3f4bff386d27bbdf4b8645199b1995d27605a886b8c71e44d819591"

RDEPENDS:${PN} += "perl"
RDEPENDS:${PN} += "perl-module-getopt-long"
RDEPENDS:${PN} += "perl-module-sys-hostname"
RDEPENDS:${PN} += "perl-module-io-socket"

# NOTE: no Makefile found, unable to determine what needs to be done

do_configure () {
	# Specify any needed configure commands here
	:
}

do_compile () {
	# Specify compilation commands here
	:
}

do_install () {
	# Specify install commands here
	install -d ${D}${sbindir}
	install -d ${D}${sysconfdir}/default/persistent
	install -m 0755 ${S}/ddclient ${D}${sbindir}
	install -m 0644 ${S}/sample-etc_ddclient.conf ${D}${sysconfdir}/ddclient.conf.sample
	install -m 0644 ${D}${sysconfdir}/ddclient.conf.sample ${D}${sysconfdir}/ddclient.conf
	install -m 0644 ${WORKDIR}/persistent ${D}/${sysconfdir}/default/persistent/50_ddclient
 install -d ${D}${systemd_unitdir}/system
 install -m 0644 ${WORKDIR}/ddclient.service ${D}${systemd_unitdir}/system
}

SYSTEMD_SERVICE:${PN} = "ddclient.service"
