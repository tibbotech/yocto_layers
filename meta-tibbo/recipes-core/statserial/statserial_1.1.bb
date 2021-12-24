require statserial.inc

BUILD_LDFLAGS = ""

SRC_URI += "file://Makefile.patch"

FILES:${PN} = "/usr/local/bin/statserial"
FILES:${PN}-doc = "/usr/local/man/man1/*"
FILES:${PN}-dbg  = "/usr/local/bin/.debug/*"

SRC_URI[sha256sum] = "40cf3bd506852942aa98f468c90ed4ea66dbd343b93efacd40d617b55caf3967"
