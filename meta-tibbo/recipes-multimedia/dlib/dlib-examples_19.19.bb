require dlib.inc

S = "${WORKDIR}/git/examples"

SRC_URI[md5sum] = "11a38f3cb14e56775ff440131c714504"
SRC_URI[sha256sum] = "147817ce228c276ced807f5c3b11f1e39aad242ccacf1486754d204e68fd3af1"

DEPENDS += "python"

do_install() {
 install -d ${D}/opt/dlib-examples
 install ${B}/*_ex ${D}/opt/dlib-examples
}

FILES_${PN} += "/opt/dlib-examples/"