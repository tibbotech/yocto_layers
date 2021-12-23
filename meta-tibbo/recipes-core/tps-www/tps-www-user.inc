WWW_U = "www"
WWW_G = "www-data"

USERADD_PACKAGES = "${PN}"
USERADD_PARAM:${PN} = " --system --no-create-home --groups ${WWW_G} --user-group ${WWW_U}"
