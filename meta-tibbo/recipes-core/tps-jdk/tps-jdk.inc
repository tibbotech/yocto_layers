DESCRIPTION = "Aggregate for LTPS JDK"
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

LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/Proprietary;md5=0557f9d92cf58f2ccdd50f62f8ac0b28"

FILES_${PN}  = "/opt/jdk*"
FILES_${PN} += "/opt/tps*"
FILES_${PN} += "/etc/profile.d/*"
FILES_${PN}-doc  = "/opt/jdk*/man/*"
FILES_${PN}-dbg  = "/opt/jdk*/bin/.debug/*"
FILES_${PN}-dbg += "/opt/jdk*/jre/bin/.debug/*"
FILES_${PN}-dbg += "/opt/jdk*/lib/.debug/*"
FILES_${PN}-dbg += "/opt/jdk*/lib/arm/jli/.debug/*"
FILES_${PN}-dbg += "/opt/jdk*/jre/lib/.debug/*"
FILES_${PN}-dbg += "/opt/jdk*/jre/lib/arm/.debug/*"
FILES_${PN}-dbg += "/opt/jdk*/jre/lib/arm/*/.debug/*"

do_install_append() {
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
 find ${D}/ -name "libsplashscreen.so" -delete
# find ${D}/ -name "libfontmanager.so" -delete
 cd ${D}/opt/
 ln -s ${I_N} ${PN}
}

INSANE_SKIP_${PN}  = "ldflags dev-so"
INSANE_SKIP_${PN} += "build-deps"

PRIVATE_LIBS += "libjvm.so"

#RDEPENDS_${PN} += "alsa-lib"
#DEPENDS += "alsa-lib"

### for SDK
#BBCLASSEXTEND += "nativesdk"
#
#FILES_${PN}_append_class-nativesdk = " ${SDKPATHNATIVE}"
#
#do_install_append_class-nativesdk() {
# mkdir -p ${D}${SDKPATHNATIVE}/environment-setup.d
# echo "export JAVA_HOME=/opt/${I_N}" > ${D}${SDKPATHNATIVE}/environment-setup.d/jdk.sh
#}
