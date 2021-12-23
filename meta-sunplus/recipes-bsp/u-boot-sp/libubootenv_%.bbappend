FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

SRC_URI:append:class-target = " file://fw_env.emmc.config"

do_install:append:class-target () {
 install -d ${D}${sysconfdir}/
 install -m 0644 ${WORKDIR}/fw_env.emmc.config ${D}${sysconfdir}/fw_env.config
}
