
#D0=${TMPDIR}/deploy/images/${MACHINE}/
#D1=${TMPDIR}/deploy/images/${MACHINE}-arm5/
D0=.
D1=../tppg2-arm5

OUTD=sp_out

F_KRN=uImage

#F_XBT=xboot-nand.bin
F_XBT=xboot-emmc.bin

#F_UBT=u-boot-a7021_emmc-sp-r0.img
#F_UBT=u-boot.img-a7021_nand
F_UBT=u-boot.img-a7021_ppg2

#F_DTB=pentagram-sp7021-achip-emu.dtb
F_DTB=sp7021-ltpp3g2revD.dtb

#F_ROO=img-spt-tppg2.squashfs
F_ROO=img-sp-tinl-tppg2.ext4

all: ${D0}/${OUTD}/ISPBOOOT.BIN

${D0}/${OUTD}/xboot.img: ${D1}/${F_XBT}
	install -d ${D0}/${OUTD}/
	install ${D1}/${F_XBT} ${D0}/${OUTD}/xboot.bin
	${D0}/sp_tools/secure_sign/gen_signature.sh ${D0}/${OUTD} xboot.bin 0
	cd ${D1}/sp_tools/; ./add_xhdr.sh ../../tppg2/${OUTD}/xboot.bin ../../tppg2/${OUTD}/xboot.img 1
	install ${D0}/${OUTD}/xboot.img ${D0}/${OUTD}/xboot0
	install ${D0}/${OUTD}/xboot.img ${D0}/${OUTD}/xboot1

${D0}/${OUTD}/${F_UBT}: ${D0}/${F_UBT}
	install -d ${D0}/${OUTD}/
	install $< $@
	${D0}/sp_tools/secure_sign/gen_signature.sh ${D0}/${OUTD}/ ${F_UBT} 1
	install ${D0}/${OUTD}/${F_UBT} ${D0}/${OUTD}/uboot0
	install ${D0}/${OUTD}/${F_UBT} ${D0}/${OUTD}/uboot1
	install ${D0}/${OUTD}/${F_UBT} ${D0}/${OUTD}/uboot2

${D0}/${OUTD}/${F_KRN}: ${D0}/${F_KRN}
	install -d ${D0}/${OUTD}/
	install $< $@
	${D0}/sp_tools/secure_sign/gen_signature.sh ${D0}/${OUTD}/ ${F_KRN} 1
	install ${D0}/${OUTD}/${F_KRN} ${D0}/${OUTD}/kernel

${D0}/${OUTD}/dtb: ${D0}/${F_DTB}
	install -d ${D0}/${OUTD}/
	ln -f -s `pwd`/${F_DTB} `pwd`/${OUTD}/dtb

${D0}/${OUTD}/rootfs: ${D0}/${F_ROO}
	install -d ${D0}/${OUTD}/
	ln -f -s `pwd`/${F_ROO} `pwd`/${OUTD}/rootfs

#if [ "$(USE_QK_BOOT)" = "1" ]; then 
# implement for vmlinux
# implement for dtb
#fi;

${D0}/${OUTD}/ISPBOOOT.BIN: ${D0}/${OUTD}/xboot.img ${D0}/${OUTD}/${F_UBT} ${D0}/${OUTD}/${F_KRN} ${D0}/${OUTD}/dtb ${D0}/${OUTD}/rootfs
	which mkimage
	${D0}/sp_tools/isp pack_image ${D0}/${OUTD}/ISPBOOOT.BIN \
	${D0}/${OUTD}/xboot0 \
	${D0}/${OUTD}/uboot0 \
	${D0}/${OUTD}/xboot1 0x100000 \
	${D0}/${OUTD}/uboot1 0x100000 \
	${D0}/${OUTD}/uboot2 0x100000 \
	${D0}/${OUTD}/env 0x80000 \
	${D0}/${OUTD}/env_redund 0x80000 \
	${D0}/${OUTD}/dtb 0x40000 \
	${D0}/${OUTD}/kernel 0xf00000 \
	${D0}/${OUTD}/rootfs 0x1E000000
	
	install -d ${D0}/${OUTD}/boot2linux
	install -d ${D0}/${OUTD}/boot2linux_SDcard

	${D0}/sp_tools/isp extract4boot2linux \
	${D0}/${OUTD}/ISPBOOOT.BIN \
	${D0}/${OUTD}/boot2linux/ISPBOOOT.BIN

	${D0}/sp_tools/isp extract4boot2linux_sdcardboot \
	${D0}/${OUTD}/ISPBOOOT.BIN \
	${D0}/${OUTD}/boot2linux_SDcard/ISPBOOOT.BIN

.PHONY: all

${D0}/${OUTD}/ISPBOOOT.BIN: sp_make.mk ${D0}/${OUTD}/xboot.img ${D0}/${OUTD}/${F_UBT} ${D0}/${OUTD}/${F_KRN} ${D0}/${OUTD}/dtb ${D0}/${OUTD}/rootfs
${D0}/${OUTD}/xboot.img ${D0}/${OUTD}/${F_UBT} ${D0}/${OUTD}/${F_KRN} ${D0}/${OUTD}/dtb ${D0}/${OUTD}/rootfs: sp_make.mk
