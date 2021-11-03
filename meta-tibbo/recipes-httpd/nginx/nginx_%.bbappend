
inherit systemd

SRC_URI += "file://all.crt"
SRC_URI += "file://all.key"
SRC_URI += "file://all.conf"
# test for ssl forwarding
#SRC_URI += "file://ktr_default_server"

#EXTRA_OECONF += " --error-log-path=/dev/stderr"
EXTRA_OECONF += " --error-log-path=/var/log/nginx.err.log"

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

do_install_append() {
 install -d ${D}${sysconfdir}/nginx/ssl/
 install -m 0644 ${WORKDIR}/all.crt ${D}${sysconfdir}/nginx/ssl/
 install -m 0644 ${WORKDIR}/all.key ${D}${sysconfdir}/nginx/ssl/
 install -m 0644 ${WORKDIR}/all.conf ${D}${sysconfdir}/nginx/ssl/
}

# test for ssl forwarding
#do_install_append_tppg2() {
# install -m 0644 ${WORKDIR}/ktr_default_server ${D}${sysconfdir}/nginx/sites-available/default_server
# install -m 0644 ${WORKDIR}/ktr_default_server ${D}${sysconfdir}/nginx/sites-enabled/default_server.conf
#}

SYSTEMD_AUTO_ENABLE_${PN} = "enable"

# dlna module
#SRC_URI += "https://github.com/arut/nginx-dlna-module.git;rev=master;destsuffix=nginx-dlna-module"
##SRC_URI += "file://nginx-dlna-module/ngx_http_dlna.c.patch"
#EXTRA_OECONF += " --add-module=${WORKDIR}/nginx-dlna-module"
##EXTRA_OECONF += " --with-compat --add-dynamic-module=${WORKDIR}/nginx-dlna-module"
#
#do_compile_append() {
# make modules
#}
#

# rtmp module
SRC_URI += "https://github.com/arut/nginx-rtmp-module.git;rev=master;destsuffix=nginx-rtmp-module"
#EXTRA_OECONF += " --add-module=${WORKDIR}/nginx-dlna-module"
EXTRA_OECONF += " --with-compat --add-dynamic-module=${WORKDIR}/nginx-rtmp-module"

FILES_${PN} += "/usr/modules/ngx_rtmp_module.so"

INSANE_SKIP_${PN} = "ldflags"

# add WebDav module to build
EXTRA_OECONF += " --with-http_dav_module"

# add WebDav extended commands module
SRC_URI += "https://github.com/arut/nginx-dav-ext-module.git;rev=v2.0.0;destsuffix=ngx_http_dav_ext_module"
FILES_${PN} += "/usr/modules/ngx_http_dav_ext_module.so"
EXTRA_OECONF += " --with-compat --add-dynamic-module=${WORKDIR}/ngx_http_dav_ext_module"
DEPENDS += "expat"

DEPENDS += "libxslt"
