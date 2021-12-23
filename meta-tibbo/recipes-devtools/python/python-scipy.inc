SUMMARY = "SciPy: Scientific Library for Python"
HOMEPAGE = "https://www.scipy.org"
LICENSE = "BSD"

S = "${WORKDIR}/scipy-${PV}"

RDEPENDS:${PN} += "${PYTHON_PN}-numpy ${PYTHON_PN}-multiprocessing lapack"
DEPENDS += "${PYTHON_PN}-numpy ${PYTHON_PN}-numpy-native lapack"

CLEANBROKEN = "1"

inherit setuptools3

export LAPACK = "${STAGING_LIBDIR}"
export BLAS = "${STAGING_LIBDIR}"

export F90 = "${TARGET_PREFIX}gfortran"
export FARCH = "${TUNE_CCARGS}"
# Numpy expects the LDSHARED env variable to point to a single
# executable, but OE sets it to include some flags as well. So we split
# the existing LDSHARED variable into the base executable and flags, and
# prepend the flags into LDFLAGS
LDFLAGS:prepend := "${@" ".join(d.getVar('LDSHARED', True).split()[1:])} "
export LDSHARED := "${@d.getVar('LDSHARED', True).split()[0]}"

# Tell Numpy to look in target sysroot site-packages directory for libraries
LDFLAGS:append = " -L${STAGING_LIBDIR}/${PYTHON_DIR}/site-packages/numpy/core/lib"

LIC_FILES_CHKSUM = "file://LICENSE.txt;md5=eb7262aea2504e4c0dfd16f5079e14dd"
