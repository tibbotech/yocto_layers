
SRC_URI = "git://github.com/scikit-learn/scikit-learn.git;protocol=https;branch=1.0.X;tag=${PV}"

SRC_URI[sha256sum] = "20766f515e6cd6f954554387dfae705d93c7b544ec0e6c6a5d8e006f6f7ef480"

require python-scikit-learn.inc
