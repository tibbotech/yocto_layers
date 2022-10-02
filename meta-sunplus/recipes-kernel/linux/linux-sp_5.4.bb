KBRANCH ?= "v5.4/standard/base"

require recipes-kernel/linux/linux-sp-5.4.inc

# board specific branches
KBRANCH:qemuarm  ?= "v5.4/standard/arm-versatile-926ejs"
KBRANCH:qemuarm64 ?= "v5.4/standard/qemuarm64"
KBRANCH:qemumips ?= "v5.4/standard/mti-malta32"
KBRANCH:qemuppc  ?= "v5.4/standard/qemuppc"
KBRANCH:qemuriscv64  ?= "v5.4/standard/base"
KBRANCH:qemux86  ?= "v5.4/standard/base"
KBRANCH:qemux86-64 ?= "v5.4/standard/base"
KBRANCH:qemumips64 ?= "v5.4/standard/mti-malta64"

SRCREV_machine:qemuarm ?= "440e41f2f471ba2bb5e3c801ffc140523bdd59b1"
SRCREV_machine:qemuarm64 ?= "e378a5626133c1f46c7ee4ede3785dbb805dc5ab"
SRCREV_machine:qemumips ?= "941c329c8810e946840bd2f4ab983a94651e563f"
SRCREV_machine:qemuppc ?= "e378a5626133c1f46c7ee4ede3785dbb805dc5ab"
SRCREV_machine:qemuriscv64 ?= "e378a5626133c1f46c7ee4ede3785dbb805dc5ab"
SRCREV_machine:qemux86 ?= "e378a5626133c1f46c7ee4ede3785dbb805dc5ab"
SRCREV_machine:qemux86-64 ?= "e378a5626133c1f46c7ee4ede3785dbb805dc5ab"
SRCREV_machine:qemumips64 ?= "b73834a11da2d4327cc3cb9f33fa3540f24d56aa"
SRCREV_machine ?= "e378a5626133c1f46c7ee4ede3785dbb805dc5ab"
SRCREV_meta ?= "054d410d1a73e4729f1fe7540db058de69fe8cfe"

# remap qemuarm to qemuarma15 for the 5.4 kernel
# KMACHINE:qemuarm ?= "qemuarma15"

SRC_URI = "git://git.yoctoproject.org/linux-yocto.git;name=machine;branch=${KBRANCH}; \
           git://git.yoctoproject.org/yocto-kernel-cache;type=kmeta;name=meta;branch=yocto-5.4;destsuffix=${KMETA}"

LIC_FILES_CHKSUM = "file://COPYING;md5=bbea815ee2795b2f4230826c0c6b8814"
LINUX_VERSION ?= "5.4.35"

DEPENDS += "${@bb.utils.contains('ARCH', 'x86', 'elfutils-native', '', d)}"
DEPENDS += "openssl-native util-linux-native"

PV = "${LINUX_VERSION}+git${SRCPV}"

KMETA = "kernel-meta"
KCONF_BSP_AUDIT_LEVEL = "2"

KERNEL_DEVICETREE:qemuarmv5 = "versatile-pb.dtb"

COMPATIBLE_MACHINE = "qemuarm|qemuarmv5|qemuarm64|qemux86|qemuppc|qemumips|qemumips64|qemux86-64|qemuriscv64"

# Functionality flags
KERNEL_EXTRA_FEATURES ?= "features/netfilter/netfilter.scc"
KERNEL_FEATURES:append = " ${KERNEL_EXTRA_FEATURES}"
KERNEL_FEATURES:append:qemuall=" cfg/virtio.scc features/drm-bochs/drm-bochs.scc"
KERNEL_FEATURES:append:qemux86=" cfg/sound.scc cfg/paravirt_kvm.scc"
KERNEL_FEATURES:append:qemux86-64=" cfg/sound.scc cfg/paravirt_kvm.scc"
KERNEL_FEATURES:append = " ${@bb.utils.contains("TUNE_FEATURES", "mx32", " cfg/x32.scc", "" ,d)}"
KERNEL_FEATURES:append = " ${@bb.utils.contains("DISTRO_FEATURES", "ptest", " features/scsi/scsi-debug.scc", "" ,d)}"
