# Update DTC to latest git and apply DT overlay patch

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI_append_class-native += "\
 file://upd.mk \
 file://sync.sh \
 file://*.its \
"

inherit deploy

do_deploy() {
 echo "DV1"
}
do_deploy_append_class-native() {
# echo "DV2: s:${S}/dtc D:${DEPLOYDIR}"
 install ${S}/dtc ${DEPLOYDIR}/dtc
 install ${WORKDIR}/upd.mk ${DEPLOYDIR}/
 install ${WORKDIR}/sync.sh ${DEPLOYDIR}/
 install -m 644 ${WORKDIR}/*.its ${DEPLOYDIR}/
}

addtask deploy before do_package_stage after do_compile

RDEPENDS_${PN} += "mcpp"
