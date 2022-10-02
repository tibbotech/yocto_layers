
FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

inherit systemd

SRC_URI += "file://all.crt"
SRC_URI += "file://all.key"
SRC_URI += "file://all.conf"
# test for ssl forwarding
#SRC_URI += "file://ktr_default_server"

#EXTRA_OECONF += " --error-log-path=/dev/stderr"
EXTRA_OECONF += " --error-log-path=/var/log/nginx.err.log"

do_install:append() {
 install -d ${D}${sysconfdir}/nginx/ssl/
 install -m 0644 ${WORKDIR}/all.crt ${D}${sysconfdir}/nginx/ssl/
 install -m 0644 ${WORKDIR}/all.key ${D}${sysconfdir}/nginx/ssl/
 install -m 0644 ${WORKDIR}/all.conf ${D}${sysconfdir}/nginx/ssl/
}

# test for ssl forwarding
#do_install:append:tppg2() {
# install -m 0644 ${WORKDIR}/ktr_default_server ${D}${sysconfdir}/nginx/sites-available/default_server
# install -m 0644 ${WORKDIR}/ktr_default_server ${D}${sysconfdir}/nginx/sites-enabled/default_server.conf
#}

SYSTEMD_AUTO_ENABLE:${PN} = "enable"

# dlna module
#SRC_URI += "git://github.com/arut/nginx-dlna-module.git;rev=master;destsuffix=nginx-dlna-module;name=dlna"
#SRC_URI += "file://nginx-dlna-module/ngx_http_dlna.c.patch"
#EXTRA_OECONF += " --add-module=${WORKDIR}/nginx-dlna-module"
##EXTRA_OECONF += " --with-compat --add-dynamic-module=${WORKDIR}/nginx-dlna-module"
##
##do_compile:append() {
## make modules
##}


# rtmp module
SRC_URI += "git://github.com/arut/nginx-rtmp-module.git;protocol=https;rev=master;destsuffix=nginx-rtmp-module;name=rtmp"
EXTRA_OECONF += " --with-compat --add-dynamic-module=${WORKDIR}/nginx-rtmp-module"

FILES:${PN} += "/usr/modules/ngx_rtmp_module.so"

INSANE_SKIP:${PN} = "ldflags"

# add WebDav module to build
EXTRA_OECONF += " --with-http_dav_module"

# add WebDav extended commands module
#SRC_URI += "git://github.com/arut/nginx-dav-ext-module.git;protocol=https;rev=v2.0.0;destsuffix=ngx_http_dav_ext_module;name=dav"
SRC_URI += "git://github.com/arut/nginx-dav-ext-module.git;protocol=https;rev=v3.0.0;destsuffix=ngx_http_dav_ext_module;name=dav"
FILES:${PN} += "/usr/modules/ngx_http_dav_ext_module.so"
EXTRA_OECONF += " --with-compat --add-dynamic-module=${WORKDIR}/ngx_http_dav_ext_module"
DEPENDS += "expat"

DEPENDS += "libxslt"

SRC_URI[dlna.sha256sum] = "092e9b7d1f0231b500e03e36ea4020eeb2f51b03ae318165acd2a5a448a2d930"
SRC_URI[rtmp.sha256sum] = "c94785d43eb2c124b34ba43eb243567f3bb995638647631150a4d5666e95c13c"
SRC_URI[dav.sha256sum] = "ab94ca8c659c8c6dbdda71069d0a45a16a63034e03b14c482b73d8c994f62d39"
