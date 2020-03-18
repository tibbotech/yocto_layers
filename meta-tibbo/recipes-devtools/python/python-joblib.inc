SUMMARY = "Running Python functions as pipeline jobs"
HOMEPAGE = "https://joblib.readthedocs.io/en/latest/"
SECTION = "devel/python"
LICENSE = "BSD-3-Clause"
#LIC_FILES_CHKSUM = "file://BSD-3-Clause;md5=4a2bb4f4ec2b68ec46d5d1be3371b2c7"

inherit pypi setuptools3

#RDEPENDS_${PN} += ""

#DEPENDS = "lapack"

BBCLASSEXTEND = "native nativesdk"
