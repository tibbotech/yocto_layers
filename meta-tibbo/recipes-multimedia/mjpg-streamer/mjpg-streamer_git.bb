SUMMARY = "MJPG-streamer takes JPGs from Linux-UVC compatible webcams, filesystem or other input plugins and streams them as M-JPEG via HTTP to webbrowsers, VLC and other software. It is the successor of uvc-streamer, a Linux-UVC streaming application with Pan/Tilt"
SECTION = "libs/multimedia"

LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://LICENSE;md5=751419260aa954499f7abaabaa882bbe"

PV = "0.5+git${SRCPV}"
SRCREV = "5f6adeefa0d5a78833cc809f2bfa76131f2b9ff8"
SRCREV = "501f6362c5afddcfb41055f97ae484252c85c912"
SRC_URI = "git://github.com/jacksonliam/mjpg-streamer.git;protocol=https \
           file://0001-Makefile-don-t-overwrite-C-LDFLAGS.patch;striplevel=2 \
          "

DEPENDS = "libgphoto2 v4l-utils"

S = "${WORKDIR}/git/mjpg-streamer-experimental"

CFLAGS =+ "-DLINUX -D_GNU_SOURCE "

do_configure() {
    # disable some stuff
    sed -i -e '/PLUGINS += input_raspicam.so/d' ${S}/Makefile
    sed -i -e 's/# PLUGINS += input_ptp2.so/PLUGINS += input_ptp2.so/' ${S}/Makefile
}

EXTRA_OEMAKE = "USE_LIBV4L2=1"

# oe_runmake seems to reset MAKEFLAGS, so just call plain make
do_compile() {
    make -e ${EXTRA_OEMAKE}
}

do_install() {
    install -d ${D}${bindir}
    install -d ${D}${libdir}

    oe_runmake install DESTDIR=${D}${prefix}

    install -d ${D}${datadir}/mjpg-streamer/
    mv ${D}${prefix}/www ${D}${datadir}/mjpg-streamer/
}

FILES_${PN} += "${libdir}/*.so"

INSANE_SKIP_${PN} = "ldflags"
