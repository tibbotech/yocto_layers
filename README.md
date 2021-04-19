# Tibbo/SunPlus Yocto Layers
Tibbo/SunPlus Linux Yocto layers for sp7021, am335x -based boards

Plus1 (sp7021): [A Linux Chip for IoT and Industrial Control Applications](https://tibbo.com/store/plus1.html)

Read [Tibbo Yocto Layers Howto](https://tibbotech.github.io/plus1_layers/) for more details.

## Clonning layers
```
rel="dunfell"
git clone git://git.yoctoproject.org/poky.git
cd poky; git checkout origin/${rel};
git clone https://github.com/meta-qt5/meta-qt5.git
cd meta-qt5; git checkout origin/${rel}; cd ..
git clone git://git.openembedded.org/meta-openembedded
cd meta-openembedded; git checkout origin/${rel}; cd ..
cd ..
git clone https://github.com/tibbotech/yocto_layers.git ./poky.x
cd poky.x; git checkout ${rel}; cd ..
rsync -a --exclude=.git ./poky.x/ ./poky/
patch -p0 < ./poky/npm.${rel}.patch
```

## Pulling the updates from the GitHub
```
cd ./poky.x/
git pull
cd ..
rsync -a --exclude=.git ./poky.x/ ./poky/
```

## Prepare for builds
```
mkdir /disk2
chmod 0777 /disk2
```

## Build
```
cd ./poky/
. oe-init-build-env build.tppg2
bitbake img-spmn
// builds main CPU code + arm926 code or
bitbake img-spmo
// builds main CPU code only, arm926 code is taken from Tibbo website
```

After build /disk2/build.26/tmp/deploy/images/tppg2/ contains all required image components: bootloaders, kernel, rootfs images.
Pack the final image:
```
$ cd /disk2/build.26/tmp/deploy/images/tppg2/
$ make -f ./sp_make.mk
```

ISPBOOOT.BIN is in the ./sp_out/

## Customizations

### Bootloader and kernel source 
(SunPlus private repo or GitHub public) are defined at
```
meta-tibbo/conf/machine/include/tppg2-all-prefs.inc
```
### ISPBOOOT.BIN components
are defined at
```
/disk2/build.26/tmp/deploy/images/tppg2/sp_make.mk
```
Components to choose:
* xboot and U-boot: for nand or emmc,
* DTB version,
* rootfs: there are several, packed to ext4, UBI and squashfs.

### Preconfigured RPM repo URL
is defined (PACKAGE_FEED_URIS variable) at
```
meta-tibbo/conf/distro/tps.conf
```
