require dlib.inc

S = "${WORKDIR}/git/examples"

DEPENDS += "python"

do_install() {
 install -d ${D}/opt/dlib-examples
 install ${B}/*_ex ${D}/opt/dlib-examples
}

FILES:${PN} += "/opt/dlib-examples/"
