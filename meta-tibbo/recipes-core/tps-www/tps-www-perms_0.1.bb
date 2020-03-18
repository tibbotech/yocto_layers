DESCRIPTION = "TPS www user"
HOMEPAGE = "http://tibbo.com/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "LGPLv2+"
SECTION = "base"

inherit useradd

require tps-www-user.inc

#USERADD_PACKAGES = "${PN}"
#USERADD_PARAM_${PN} = " \
#    --system --no-create-home \
#    --groups ${WWW_G} \
#    --user-group ${WWW_U}"
