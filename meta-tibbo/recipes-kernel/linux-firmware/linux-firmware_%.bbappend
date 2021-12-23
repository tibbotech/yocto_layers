
#do_install:append() {
# rm -r ${D}${nonarch_base_libdir}/firmware/amdgpu
# rm -r ${D}${nonarch_base_libdir}/firmware/i915
# rm -r ${D}${nonarch_base_libdir}/firmware/intel/*.sfi
# rm -r ${D}${nonarch_base_libdir}/firmware/mellanox
# rm -r ${D}${nonarch_base_libdir}/firmware/moxa
# rm -r ${D}${nonarch_base_libdir}/firmware/radeon
# rm -r ${D}${nonarch_base_libdir}/firmware/vxge
# rm -r ${D}${nonarch_base_libdir}/firmware/qed
## rm -r ${D}${nonarch_base_libdir}/firmware/ar3k
## rm -r ${D}${nonarch_base_libdir}/firmware/ath10k
## rm -r ${D}${nonarch_base_libdir}/firmware/ath6k
## rm -r ${D}${nonarch_base_libdir}/firmware/ath9k_htc
# rm -r ${D}${nonarch_base_libdir}/firmware/bnx2
# rm -r ${D}${nonarch_base_libdir}/firmware/bnx2x
# rm -r ${D}${nonarch_base_libdir}/firmware/cxgb3
# rm -r ${D}${nonarch_base_libdir}/firmware/cxgb4
# rm -r ${D}${nonarch_base_libdir}/firmware/dvb*
## rm -r ${D}${nonarch_base_libdir}/firmware/iwlwifi*
# rm -r ${D}${nonarch_base_libdir}/firmware/myri10ge*
#}
