DESCRIPTION = "Monitoring framework and TSDB"
HOMEPAGE = "https://prometheus.io/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "Apache-2.0"
SECTION = "console/utils"
SRCREV="${AUTOREV}"

inherit bin_package

S = "${WORKDIR}/git"

LIC_FILES_CHKSUM = "file://LICENSE;md5=86d3f3a95c324c9479bd8986968f4327"
