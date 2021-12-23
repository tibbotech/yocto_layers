SUMMARY = "Running Python functions as pipeline jobs"
HOMEPAGE = "https://joblib.readthedocs.io/en/latest/"
SECTION = "devel/python"
LICENSE = "BSD-3-Clause"

inherit pypi setuptools3

#RDEPENDS:${PN} += ""

#DEPENDS = "lapack"

BBCLASSEXTEND = "native nativesdk"

LIC_FILES_CHKSUM = "file://LICENSE.txt;md5=32b289008fb813a27c9025f02b59d03d"
