require ortp.inc
DEPENDS = "glib-2.0 openssl"
PR = "${INC_PR}.1"

SRC_URI += "file://0001-fix-unused-variables.patch;patch=1 \
            file://compile-fixes-newer-gcc.diff"

SRC_URI[md5sum] = "94546901d14b85f97342f4ecf39489b1"
SRC_URI[sha256sum] = "3b655a79f9122afd298e9cd702e542908bbd6ea1337c02553110c57e0b3c5835"
