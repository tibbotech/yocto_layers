# Update DTC to latest git and apply DT overlay patch

FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI:append:class-native += "\
 file://upd.mk \
 file://sync.sh \
 file://upd_bad.its \
 file://upd_b_KLM601.its \
 file://upd_b_tpp.its \
 file://upd_fwuploader.its \
 file://upd_KLM601-revB-b.its \
 file://upd_KLM601-revB.its \
 file://upd_KLM601-revC.its \
 file://upd_KLM601-revT.its \
 file://upd_TPS3-agent.its \
 file://upd_TPS3-db.its \
 file://upd_TPS3-node.its \
 file://upd_TPS3-rlp.its \
"

inherit deploy

do_deploy() {
 echo "DV1"
}
do_deploy:append:class-native() {
# echo "DV2: s:${S}/dtc D:${DEPLOYDIR}"
 install ${S}/dtc ${DEPLOYDIR}/dtc
 install ${WORKDIR}/upd.mk ${DEPLOYDIR}/
 install ${WORKDIR}/sync.sh ${DEPLOYDIR}/
 install -m 644 ${WORKDIR}/*.its ${DEPLOYDIR}/
}

addtask deploy before do_package_stage after do_compile

RDEPENDS:${PN} += "mcpp"
