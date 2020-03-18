SUMMARY = "Fundamental Python library for scientific computing"
HOMEPAGE = "https://www.scipy.org/scipylib/index.html"
SECTION = "devel/python"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://PKG-INFO;md5=d2f14edb4c43b600e876a0e647ac05b2"

inherit pypi setuptools3

RDEPENDS_${PN} += "${PYTHON_PN}-numpy ${PYTHON_PN}-pybind11"
#DEPENDS += "${PYTHON_PN}-numpy"
DEPENDS += "python3-numpy"

BBCLASSEXTEND = "native nativesdk"

do_configure[noexec] = "1"
