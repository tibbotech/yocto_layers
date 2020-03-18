# yocto_layers
Tibbo Linux Yocto layers for sp7021, am335x -based boards


## Clonning layers
```
git clone git://git.yoctoproject.org/poky.git
cd poky; git checkout origin/zeus;
git clone https://github.com/meta-qt5/meta-qt5.git
cd meta-qt5; git checkout origin/zeus; cd ..
git clone git://git.openembedded.org/meta-openembedded
cd meta-openembedded; git checkout origin/zeus; cd ..
cd ..
git clone --separate-git-dir=./ https://github.com/tibbotech/yocto_layers.git ./poky
```

## Prepare for builds
```
mkdir /disk2
```

## Build
```
cd ./poky/
. oe-init-build-env c.tppg2
bitbake img-spmn
```

After build /disk2/build.24/tmp/deploy/images/tppg2/ contains all required image components: bootloaders, kernel, rootfs images.
Pack the final image:

$ cd /disk2/build.24/tmp/deploy/images/tppg2/
$ make -f ./sp_make.mk

ISPBOOOT.BIN is in the ./sp_out/ 
