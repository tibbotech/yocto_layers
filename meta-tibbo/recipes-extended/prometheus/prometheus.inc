DESCRIPTION = "Monitoring framework and TSDB"
HOMEPAGE = "https://prometheus.io/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "Apache-2.0"
SECTION = "console/utils"
SRCREV="${AUTOREV}"

inherit bin_package

S = "${WORKDIR}/git"

LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/Apache-2.0;md5=89aea4e17d99a7cacdbeed46a0096b10"
