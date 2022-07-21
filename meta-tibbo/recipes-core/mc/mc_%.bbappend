
do_install:append() {
 mkdir -p ${D}${sysconfdir}/profile.d/
 ln -sf /usr/libexec/mc/mc.sh ${D}${sysconfdir}/profile.d/20mc.sh
 ln -sf /usr/libexec/mc/mc.csh ${D}${sysconfdir}/profile.d/20mc.csh
}

EXTRA_OECONF = "--with-screen=ncurses --with-gpm-mouse --without-x --disable-configure-args"

DEPENDS += "gpm"