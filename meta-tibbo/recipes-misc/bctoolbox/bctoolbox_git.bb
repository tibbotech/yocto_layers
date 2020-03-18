require bctoolbox.inc

inherit gitpkgv

PR = "${INC_PR}.0"
SRCREV = "019cd491db7d394c96432959f3b837e72cc86770"
SRC_URI = "git://git.linphone.org/bctoolbox.git;commit=${SRCREV}"
PV = "git_${SRCREV}"
PKGV = "${GITPKGVTAG}"

LIC_FILES_CHKSUM = "file://COPYING;md5=9f9938e31db89d55a796e86808c96848;rev={SRCREV}"
