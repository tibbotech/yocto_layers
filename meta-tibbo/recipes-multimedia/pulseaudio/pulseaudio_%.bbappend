
FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

# KLM601 needs it
RDEPENDS:pulseaudio-server += " \
    pulseaudio-module-switch-on-connect \
    pulseaudio-module-bluetooth-policy \
    pulseaudio-module-bluetooth-discover \
    pulseaudio-module-bluez5-device \
    pulseaudio-module-bluez5-discover \
    pulseaudio-module-match \
    pulseaudio-module-combine \
    pulseaudio-module-remap-sink \
    pulseaudio-module-remap-source \
    pulseaudio-module-dbus-protocol \
"

SRC_URI += "file://etc/tmpfiles.d/pulse.conf"

CONFFILES:${PN}-bin += "${sysconfdir}/tmpfiles.d/pulse.conf"

do_install:append() {
 install -d ${D}${sysconfdir}/tmpfiles.d
 install -m 0644 ${WORKDIR}/etc/tmpfiles.d/pulse.conf ${D}${sysconfdir}/tmpfiles.d/pulse.conf
}

SRC_URI += "file://etc/systemd/system/pulseaudio.service"
SRC_URI += "file://etc/pulse/system.pa.klmbt"
SRC_URI += "file://etc/pulse/system.pa"
FILES:${PN}-server += "${sysconfdir}/systemd/system/pulseaudio.service"
FILES:${PN}-server += "${sysconfdir}/pulse/system.pa.klmbt"
FILES:${PN}-server += "${sysconfdir}/pulse/system.pa"

do_install:append() {
 install -d ${D}${sysconfdir}/systemd/system/
 install -m 0644 ${WORKDIR}/etc/systemd/system/pulseaudio.service ${D}${sysconfdir}/systemd/system/pulseaudio.service
 install -d ${D}${sysconfdir}/pulse/
 install -m 0644 ${WORKDIR}/etc/pulse/system.pa.klmbt ${D}${sysconfdir}/pulse/system.pa.klmbt
 install -m 0644 ${WORKDIR}/etc/pulse/system.pa ${D}${sysconfdir}/pulse/system.pa
}
