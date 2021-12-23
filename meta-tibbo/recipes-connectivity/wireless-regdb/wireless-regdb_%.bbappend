FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

SRC_URI += "file://2-countries.txt"
SRC_URI += "file://2lc.sh"

do_install:append() {
 install -d ${D}/lib/crda/
 ${WORKDIR}/2lc.sh ${S}/db.txt ${WORKDIR}/2-countries.txt > ${D}/lib/crda/2lc.txt
}

FILES:${PN} += "/lib/crda/2lc.txt"
FILES:${PN}-static += "/lib/crda/2lc.txt"
