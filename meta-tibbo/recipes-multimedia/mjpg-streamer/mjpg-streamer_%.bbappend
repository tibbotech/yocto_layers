
inherit systemd

SRC_URI += "file://etc/udev/rules.d/cams.rules"
SRC_URI += "file://lib/systemd/system/cam-0.service"

do_install:append () {
 install -d ${D}/etc/udev/rules.d/
 install ${WORKDIR}/etc/udev/rules.d/cams.rules ${D}/etc/udev/rules.d/
 install -d ${D}/lib/systemd/system/
 install -m 0644 ${WORKDIR}/lib/systemd/system/cam-0.service ${D}/lib/systemd/system/
}

SYSTEMD_SERVICE:${PN} = "cam-0.service"
