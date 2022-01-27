
RDEPENDS:${PN} += "iproute2"

do_install:append() {
 if [ -f "${D}${systemd_unitdir}/wg-quick\@.service" ]; then
   install -d ${D}${systemd_unitdir}/system
   mv ${D}${systemd_unitdir}/wg-quick\@.service ${D}${systemd_unitdir}/system/wg-quick\@.service
 fi;
}
