
# disable mysql, sqlite and imap packages
PACKAGECONFIG = " sqlite3 \
                   ${@bb.utils.contains('DISTRO_FEATURES', 'pam', 'pam', '', d)}"

# try LDFLAGS
CFLAGS =+ " -ldl --with-curl"

RDEPENDS:${PN} += "libcurl"
