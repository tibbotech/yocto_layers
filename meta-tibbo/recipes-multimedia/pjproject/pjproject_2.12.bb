SRCREV="99660f6addf0072a1e01c3ba630e3701921add9e"

LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=b234ee4d69f5fce4486a80fdaf4a4263"

X_BR="master"

require pjproject.inc

SRC_URI += "file://Makefile.2.12.patch"

SRC_URI[sha256sum] = "2f5a1da1c174d845871c758bd80fbb580fca7799d3cfaa0d3c4e082b5161c7b4"
