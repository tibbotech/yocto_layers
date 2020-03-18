
SRC_URI = "https://github.com/scipy/scipy/releases/download/v${PV}/scipy-${PV}.tar.gz"

SRC_URI[md5sum] = "3a97689656f33f67614000459ec08585"
SRC_URI[sha256sum] = "dee1bbf3a6c8f73b6b218cb28eed8dd13347ea2f87d572ce19b289d6fd3fbc59"

require python-scipy.inc
