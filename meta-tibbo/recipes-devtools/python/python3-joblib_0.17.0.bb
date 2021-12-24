
SRC_URI = "git://github.com/joblib/joblib.git;protocol=https;branch=master;tag=${PV}"

SRC_URI[sha256sum] = "9e284edd6be6b71883a63c9b7f124738a3c16195513ad940eae7e3438de885d5"

require python-joblib.inc
