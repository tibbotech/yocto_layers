NAPPS_PFX="/opt/node-apps"
NAPPS_U = "node-apps"
NAPPS_G = "www-data"

USERADD_PACKAGES = "${PN}"
USERADD_PARAM:${PN} = " \
    --system --create-home --home-dir ${NAPPS_PFX} -s /bin/false \
    --groups ${NAPPS_G} --user-group ${NAPPS_U}"
