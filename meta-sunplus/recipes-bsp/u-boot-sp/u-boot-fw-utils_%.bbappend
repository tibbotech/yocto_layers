
SRC_URI += "file://fw_env.emmc.config"

do_install_append () {
 install -m 0644 ${WORKDIR}/fw_env.emmc.config ${D}${sysconfdir}/fw_env.config
}
