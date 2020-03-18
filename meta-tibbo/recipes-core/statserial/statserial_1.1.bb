require statserial.inc

BUILD_LDFLAGS = ""

SRC_URI += "\
 file://Makefile.patch \
"

FILES_${PN} = "/usr/local/bin/statserial"
FILES_${PN}-doc = "/usr/local/man/man1/*"
FILES_${PN}-dbg  = "/usr/local/bin/.debug/*"

SRC_URI[md5sum] = "bcd90fb0881c64024396bf1070de7e64"
SRC_URI[sha256sum] = "40cf3bd506852942aa98f468c90ed4ea66dbd343b93efacd40d617b55caf3967"