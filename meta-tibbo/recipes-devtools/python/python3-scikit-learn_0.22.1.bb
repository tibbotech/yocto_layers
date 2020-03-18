
SRC_URI = "git://github.com/scikit-learn/scikit-learn.git;branch=master;tag=${PV}"

SRC_URI[md5sum] = "4c8d2ab712bd03e01bc55291e1f7bc6e"
SRC_URI[sha256sum] = "57538d138ba54407d21e27c306735cbd42a6aae0df6a5a30c7a6edde46b0017d"

require python-scikit-learn.inc
