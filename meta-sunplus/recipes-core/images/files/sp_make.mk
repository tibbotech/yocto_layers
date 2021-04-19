include sp_make.inc.mk

all: ${D0}/${OUTD} ${D0}/${OUTD}/ISPBOOOT.BIN

${D0}/${OUTD}:
	install -d ${D0}/${OUTD}

${D1}/${F_XBT}.img: ${D1}/${F_XBT}.sig
	${D0}/sp_tools/add_xhdr.sh $< $@ 1

${D1}/${F_XBT}.sig: ${D1}/${F_XBT}.bin
	install ${D1}/${F_XBT}.bin ${D1}/${F_XBT}.sig
	${D0}/sp_tools/secure_sign/gen_signature.sh ${D1} ${F_XBT}.sig 0

${D0}/${OUTD}/nonos: ${D1}/${F_NON}.img
	install $< $@
	@sz=`du -sb $@ |cut -f1`; printf "size: %d (hex %x)\n" $$sz $$sz

${D1}/${F_NON}.img: ${D1}/${F_NON}.bin
	export MKIMAGE=${D0}/sp_tools/mkimage;  ${D0}/sp_tools/add_uhdr.sh nonos_B $< $@ arm 0x10040 0x10040

${D0}/${OUTD}/uboot0: ${D0}/${F_UBT}.img
	install $< $@

${D0}/${F_UBT}.img: ${D0}/${F_UBT}.uhd
	install ${D0}/${F_UBT}.uhd ${D0}/${F_UBT}.img
	${D0}/sp_tools/secure_sign/gen_signature.sh ${D0}/ ${F_UBT}.img 1

${D0}/${F_UBT}.uhd: ${D0}/${F_UBT}
	export MKIMAGE=${D0}/sp_tools/mkimage; ${D0}/sp_tools/add_uhdr.sh "uboot_pentagram_board" $< $@ arm 0x200040 0x200040
	@sz=`du -sb $@ |cut -f1`; printf "size: %d (hex %x)\n" $$sz $$sz

${D0}/${OUTD}/kernel: ${D0}/${F_KRN}.img
	install $< $@

${D0}/${F_KRN}.img: ${D0}/${F_KRN}
	install ${D0}/${F_KRN} ${D0}/${F_KRN}.img
	${D0}/sp_tools/secure_sign/gen_signature.sh ${D0}/ ${F_KRN}.img 1

${D0}/${OUTD}/dtb: ${D0}/${F_DTB}
	ln -sf `pwd`/${F_DTB} `pwd`/${OUTD}/dtb

${D0}/${OUTD}/rootfs: ${D0}/${F_ROO}
	ln -f -s `pwd`/${F_ROO} `pwd`/${OUTD}/rootfs

#if [ "$(USE_QK_BOOT)" = "1" ]; then 
# implement for vmlinux
# implement for dtb
#fi;

${D0}/${OUTD}/ISPBOOOT.BIN: ${D1}/${F_XBT}.img ${D0}/${OUTD}/nonos ${D0}/${OUTD}/uboot0 ${D0}/${OUTD}/kernel ${D0}/${OUTD}/dtb ${D0}/${OUTD}/rootfs ${D0}/sp_tools/mkimage
	install ${D1}/${F_XBT}.img ${D0}/${OUTD}/xboot0
	install ${D1}/${F_XBT}.img ${D0}/${OUTD}/xboot1
	install ${D0}/${OUTD}/uboot0 ${D0}/${OUTD}/uboot1
	install ${D0}/${OUTD}/uboot0 ${D0}/${OUTD}/uboot2
	${D0}/sp_tools/isp pack_image ${D0}/${OUTD}/ISPBOOOT.BIN \
	${D0}/${OUTD}/xboot0 \
	${D0}/${OUTD}/uboot0 \
	${D0}/${OUTD}/xboot1 0x100000 \
	${D0}/${OUTD}/uboot1 0x100000 \
	${D0}/${OUTD}/uboot2 0x100000 \
	${D0}/${OUTD}/env 0x80000 \
	${D0}/${OUTD}/env_redund 0x80000 \
	${D0}/${OUTD}/nonos 0x100000 \
	${D0}/${OUTD}/dtb 0x40000 \
	${D0}/${OUTD}/kernel 0x2000000 \
	${D0}/${OUTD}/rootfs 0x1E000000
	
# not done yet: direct SD boot
	
#	install -d ${D0}/${OUTD}/boot2linux_SDC
#	install ${D0}/${OUTD}/uboot0 ${D0}/${OUTD}/boot2linux_SDC/
#	install ${D0}/${OUTD}/kernel ${D0}/${OUTD}/boot2linux_SDC/
#	install ${D0}/${OUTD}/nonos  ${D0}/${OUTD}/boot2linux_SDC/
#	dd if=${D0}/${OUTD}/ISPBOOOT.BIN of=${D0}/${OUTD}/boot2linux_SDC/ISPBOOOT.BIN bs=1024 skip=0 count=64
#	install ${D0}/sp_tools/sdcard_boot/uEnv.txt ${D0}/${OUTD}/boot2linux_SDC/
#	${D0}/sp_tools/sdcard_boot/sdcard_boot.sh ${D0}/${OUTD}/boot2linux_SDC/

# not done yet: direct boot to Linux
#	install -d ${D0}/${OUTD}/boot2linux_USB
#	${D0}/sp_tools/isp extract4boot2linux_usbboot ${D0}/${OUTD}/ISPBOOOT.BIN ${D0}/${OUTD}/boot2linux_USB/ISPBOOOT.BIN

#	install -d ${D0}/${OUTD}/boot2linux
#
#	${D0}/sp_tools/isp extract4boot2linux \
#	${D0}/${OUTD}/ISPBOOOT.BIN \
#	${D0}/${OUTD}/boot2linux/ISPBOOOT.BIN
#
#	${D0}/sp_tools/isp extract4boot2linux_sdcardboot \
#	${D0}/${OUTD}/ISPBOOOT.BIN \
#	${D0}/${OUTD}/boot2linux_SDcard/ISPBOOOT.BIN

.PHONY: all

${D0}/${OUTD}/ISPBOOOT.BIN: sp_make.mk ${D1}/${F_XBT}.img ${D0}/${OUTD}/nonos ${D0}/${OUTD}/uboot0 ${D0}/${OUTD}/kernel ${D0}/${OUTD}/dtb ${D0}/${OUTD}/rootfs
${D0}/${OUTD}/dtb: sp_make.mk
${D0}/${OUTD}/rootfs: sp_make.mk
${D0}/${OUTD}/nonos: sp_make.mk
#${D0}/${OUTD}/xboot.img ${D0}/${OUTD}/${F_UBT} ${D0}/${OUTD}/${F_KRN} ${D0}/${OUTD}/dtb ${D0}/${OUTD}/rootfs: sp_make.mk
