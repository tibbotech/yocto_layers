
I_N="jdk7u75"
I_O="jdk1.7.0_75"

SRC_URI[sha256sum] = "29c53bac198ca3faf5ed370025a2a5564d43e05b92d91b6b5d2a8a503de10c01"

require jrejdk.inc

DESCRIPTION += ": JRE7 ARM"

COMPATIBLE_HOST = "(arm.*-linux)"
