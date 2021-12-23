DESCRIPTION = "Java"
HOMEPAGE = "http://tibbo.com/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "Proprietary"
SECTION = "base"
PACKAGES = "${PN}-doc ${PN}-dbg ${PN}"

inherit bin_package

# I_N - external, "jdk7u60" for example
S = "${WORKDIR}/${I_N}"

MIRRORS=""
PREMIRRORS=""

SRC_URI = "http://tibbo.com/downloads/LTPS_tmp/jdk-${PV}.tar.gz;subdir=${I_N}/opt"

FILES:${PN}  = "/opt/jdk*"
FILES:${PN} += "/opt/tps*"
FILES:${PN} += "/etc/profile.d/*"
FILES:${PN}-doc  = "/opt/jdk*/man/*"
FILES:${PN}-dbg  = "/opt/jdk*/bin/.debug/*"
FILES:${PN}-dbg += "/opt/jdk*/jre/bin/.debug/*"
FILES:${PN}-dbg += "/opt/jdk*/lib/.debug/*"
FILES:${PN}-dbg += "/opt/jdk*/lib/arm/jli/.debug/*"
FILES:${PN}-dbg += "/opt/jdk*/jre/lib/.debug/*"
FILES:${PN}-dbg += "/opt/jdk*/jre/lib/arm/.debug/*"
FILES:${PN}-dbg += "/opt/jdk*/jre/lib/arm/*/.debug/*"

do_install() {
 install -d ${D}/opt/${I_N}
 cp -r ${S}/opt/${I_O}/jre/* ${D}/opt/${I_N}
}

do_install:append() {
# install -d ${D}/etc/profile.d/
# echo "export JAVA_HOME=/opt/${I_O}" > ${D}/etc/profile.d/jdk.sh
 find ${D}/ -name "src.zip" -delete
 find ${D}/ -name ".debug" -delete
 # depends on libX
 find ${D}/ -name "policytool" -delete
 # there are two libmawt.so. Cannot remove libmawt.so from /headless/
# find ${D}/ -name "libmawt.so" -delete
# rm -rf ${D}/opt/${I_O}/jre/lib/arm/xawt/libmawt.so
 find ${D}/ -name "libjawt.so" -delete
 find ${D}/ -name "libawt_xawt.so" -delete
 find ${D}/ -name "libsplashscreen.so" -delete
# find ${D}/ -name "libfontmanager.so" -delete
 cd ${D}/opt/
 ln -s ${I_N} ${PN}
}

INSANE_SKIP:${PN}  = "ldflags dev-so"
INSANE_SKIP:${PN} += "build-deps"

PRIVATE_LIBS += "libjvm.so"

LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/${LICENSE};md5=0557f9d92cf58f2ccdd50f62f8ac0b28"
