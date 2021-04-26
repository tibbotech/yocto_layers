# Tibbo/SunPlus Yocto Layers
Tibbo/SunPlus Linux Yocto layers for sp7021, am335x -based boards

Plus1 (sp7021): [A Linux Chip for IoT and Industrial Control Applications](https://tibbo.com/store/plus1.html)

Read [Tibbo Yocto Layers Howto](https://tibbotech.github.io/plus1_layers/) for more details.

## 1. Preparing the ENV

### 1.1 Clonning layers
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

### 1.2 Pulling the updates from the GitHub
```
cd ./poky.x/
git pull
cd ..
rsync -a --exclude=.git ./poky.x/ ./poky/
```

### 1.3 disk2
```
mkdir /disk2
chmod 0777 /disk2
```

## 2. Builds

### 2.1 Building two parts separately

Build small image without any additional dependencies (4300 recipes):
```
$ bitbake mc:tpp-tppg2:img-sp-tiny
```
or build complete open-source test image with some additional software (8000 recipes):
```
$ bitbake mc:tpp-tppg2:img-tps-free
```

Optionally: XBoot + arm926 test firmware (2900 recipes)
```
$ bitbake mc:tpp-tppg2-arm5:img-xboot
```

### 2.2 Building two parts at once (11000 recipes)
```
$ bitbake img-spmn
```

<sup>*</sup> First bitbake run is time-consuming. All subsequent builds 
are incremental

<sup>*</sup> Xboot + arm926 firmware is not required for ISPBOOOT assembly.
Script will download this parts from Tibbo website.

After build /disk2/build.26/tmp/deploy/images/tppg2/ contains all required image components: bootloaders, kernel, rootfs images.

## 3. Assembling ISPBOOOT.BIN from built parts

Deploy dir /disk2/build.26/tmp/deploy/images/ contains image components:

bootloaders, kernel, rootfs images.
Pack the final image:
```
$ cd /disk2/build.26/tmp/deploy/images/tppg2/
$ make -f ./sp_make.mk
```

ISPBOOOT.BIN will be placed at ./sp_out/

## How to reduce build time and resources

1) build "img-spmo" - main CPU code only. XBoot and B-Chip firmware parts will be downloaded from Tibbo website
2) use RPM or DEB only. Configured at build.tppg2/conf/local.conf#L116: https://github.com/tibbotech/yocto_layers/blob/a5bf3f0376836a1f83fd7cf74d232585c98534ed/build.tppg2/conf/local.conf#L116

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
