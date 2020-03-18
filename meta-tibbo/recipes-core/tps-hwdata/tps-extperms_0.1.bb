DESCRIPTION = "TPS pins.ini shared definition"
HOMEPAGE = "http://tibbo.com/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "LGPLv2+"
SECTION = "base"

inherit useradd

LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

USERADD_PACKAGES = "${PN}"
GROUPADD_PARAM_${PN} = "capi-devs"
