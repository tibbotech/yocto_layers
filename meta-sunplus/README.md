# meta-sunplus

Yocto private layer for SunPlus (<http://sunplus.com/>) boards.

## Quick links

* Git repo: ...
* Mailing list (yocto mailing list): <yocto@yoctoproject.org>
* Issues management (Github Issues): ...
* Documentation: ...

## Description

This is the ....

## Dependencies

This layer depends on:

* URI: git://git.yoctoproject.org/poky
  * branch: master
  * revision: HEAD

* URI: git://git.openembedded.org/meta-openembedded
  * layers: meta-oe, meta-multimedia, meta-networking, meta-python
  * branch: master
  * revision: HEAD

## Quick Start

1. source poky/oe-init-build-env build-tpp
2. Add this layer to bblayers.conf and the dependencies above
3. Set MACHINE in local.conf to one of the supported boards
4. bitbake tps-image
5. ...
6. Boot your board.

## Maintainers

* Dvorkin Dmitry `<dvorkin at tibbo.com>`
