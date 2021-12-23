inherit pypi setuptools3

SUMMARY = "Python client for the Prometheus monitoring system"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://PKG-INFO;beginline=8;endline=8;md5=590a3fe69dec1ae6d5f4e74171c4ff0f"

RDEPENDS_${PN} += " \
        ${PYTHON_PN}-netserver \
        "

PYPI_PACKAGE = "prometheus_client"

SRC_URI[sha256sum] = "1b12ba48cee33b9b0b9de64a1047cbd3c5f2d0ab6ebcead7ddda613a750ec3c5"
