DESCRIPTION = "System profiler and benchmark for Linux systems"
HOMEPAGE = "https://github.com/lpereira/hardinfo"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "GPL-2.0"
SECTION = "console/utils"
SRCREV="${AUTOREV}"

inherit cmake

S = "${WORKDIR}/git"

SRC_URI  = "git://github.com/lpereira/hardinfo.git;protocol=git;"

LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"
