
SRC_URI = "git://github.com/scikit-learn/scikit-learn.git;protocol=https;branch=0.23.X;tag=${PV}"

SRC_URI[md5sum] = "a03e52a3fa6988bf932db5dcdc74019c"
SRC_URI[sha256sum] = "20766f515e6cd6f954554387dfae705d93c7b544ec0e6c6a5d8e006f6f7ef480"

require python-scikit-learn.inc

LIC_FILES_CHKSUM = "file://PKG-INFO;md5=53ff369ff4563921dc4422dae6e9c0ee"
