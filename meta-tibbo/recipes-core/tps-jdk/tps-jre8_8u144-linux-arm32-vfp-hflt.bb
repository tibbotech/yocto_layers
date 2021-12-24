
I_N="jdk8u144"
I_O="jdk1.8.0_144"

SRC_URI[sha256sum] = "cbbd390e19ab4c473e05f60602ce2804db25e4e35be5ab95f4f1a2aeb5b72383"

require jrejdk.inc

DESCRIPTION += ": JRE8 ARM"

COMPATIBLE_HOST = "(arm.*-linux)"

RDEPENDS_${PN} += "alsa-lib"
