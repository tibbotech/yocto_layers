DESCRIPTION = "Wireless Central Regulatory Domain Agent Database"
HOMEPAGE = "http://wireless.kernel.org/en/developers/Regulatory/CRDA"
SECTION = "base"
PRIORITY = "optional"
LICENSE = "ISC"
PR = "${AUTOREV}"

SRCREV="${AUTOREV}"

SRC_URI  = "git://git.kernel.org/pub/scm/linux/kernel/git/linville/wireless-regdb.git;branch=master"
SRC_URI += "file://2-countries.txt"
SRC_URI += "file://2lc.sh"

S="${WORKDIR}/git"

do_install() {
    oe_runmake PREFIX=${D} install
}
do_install_append() {
    ${WORKDIR}/2lc.sh ${S}/db.txt ${WORKDIR}/2-countries.txt > ${D}/lib/crda/2lc.txt
}

FILES_${PN} = "/lib/crda/*"
FILES_${PN}-doc = "/share/man/*"

SRC_URI[md5sum] = "ded010f200090a2f337e2ccd20e9de6b"
SRC_URI[sha256sum] = "5e69b5ad429dba0ad381e364b4b1a3439513b7fab99deacdef088787612cdbce"
LIC_FILES_CHKSUM = "file://LICENSE;md5=07c4f6dea3845b02a18dc00c8c87699c"

# NOTE: requires urpmi python-m2crypto

DEPENDS += "openssl-native"
