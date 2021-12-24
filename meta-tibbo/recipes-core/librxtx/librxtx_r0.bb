require librxtx.inc

SRCREV="215a4c69bf73ba00d4e60f15f391562be4f59958"

#SRC_URI += "file://x0.patch"
#SRC_URI += "file://x1.patch"
#SRC_URI += "file://x2.patch"

# host system java root
#DV_JAVA="/usr/java/jdk1.7.0_79"
DV_JAVA="/usr/java/latest"
# target java classpath
DV_JAVA_T="/opt/jdk"

EXTRA_OEMAKE += "RXTX_PATH=${D}${DV_JAVA_T}/jre/lib/${TARGET_ARCH}/"
EXTRA_OEMAKE += "JHOME=${D}${DV_JAVA_T}/jre/lib/ext/"
EXTRA_OECONF += "JPATH=${DV_JAVA}/"

do_configure() {
 ${S}/configure ${CONFIGUREOPTS} ${EXTRA_OECONF}
}

do_install:prepend() {
 install -d ${D}${DV_JAVA_T}/jre/lib/${TARGET_ARCH}/
 install -d ${D}${DV_JAVA_T}/jre/lib/ext/
}

PACKAGES = "${PN}-dbg ${PN}"

FILES:${PN}-dbg  = "${DV_JAVA_T}/jre/lib/${TARGET_ARCH}/.debug/*"
FILES:${PN}-dbg += "/usr/src/debug/librxtx/*"

FILES:${PN}  = "${DV_JAVA_T}/jre/lib/${TARGET_ARCH}/*"
FILES:${PN} += "${DV_JAVA_T}/jre/lib/ext/"

SRC_URI[sha256sum] = "f91b07e478d2210c44ae02abcd6ab17016135775ae7cf931305df2a5314882b0"
