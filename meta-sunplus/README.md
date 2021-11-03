# meta-sunplus

Yocto layer for SunPlus (<http://sunplus.com/>) BSPs.

## Quick links

* Git repo: https://github.com/tibbotech/yocto_layers

## Description

This is the bootloader and kernel for Plus1/SP7021

## Quick Start

1. source poky/oe-init-build-env c.tppg2
2. bitbake imgm-spmn
3. cd /disk2/build.26/tmp/deploy/images/tppg2/; make -f sp_make.mk
4. Follow instructions at https://tibbo.com/downloads/LTPS/FW/LTPPg2/

## Maintainers

* Dvorkin Dmitry `<dvorkin at tibbo.com>`
