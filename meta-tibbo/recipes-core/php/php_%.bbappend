
# disable mysql, sqlite and imap packages
#PACKAGECONFIG = " sqlite3 \
#                   ${@bb.utils.contains('DISTRO_FEATURES', 'pam', 'pam', '', d)}"

EXTRA_OECONF =+ "--with-curl"
DEPENDS += "curl"
