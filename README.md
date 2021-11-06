# Tibbo/SunPlus Yocto Layers
Tibbo/SunPlus Linux Yocto layers for sp7021, am335x -based boards

Plus1 (sp7021): [A Linux Chip for IoT and Industrial Control Applications](https://tibbo.com/store/plus1.html)

Read [Tibbo Yocto Layers Howto](https://tibbotech.github.io/plus1_layers/) for more details.

## 1. Preparing the ENV

### 1.1 Clonning layers
```
rel="dunfell"
git clone -b ${rel} git://git.yoctoproject.org/poky.git
git clone -b ${rel} https://github.com/meta-qt5/meta-qt5.git ./poky/meta-qt5
git clone https://github.com/OpenAMP/meta-openamp.git ./poky/meta-openamp
git clone -b ${rel} git://git.openembedded.org/meta-openembedded ./poky/meta-openembedded
git clone -b ${rel} https://github.com/tibbotech/yocto_layers.git ./poky.x
rsync -a --exclude=.git ./poky.x/ ./poky/
patch -p0 < ./poky/npm.${rel}.patch
sed -i 's/"gatesgarth"/"rocko sumo thud zeus dunfell gatesgarth"/g' ./poky/meta-openamp/conf/layer.conf
```

### 1.2 Pulling the updates from the GitHub
```
cd ./poky.x/;  git pull ; cd ..
rsync -a --exclude=.git ./poky.x/ ./poky/
```

### 1.3 Creating separate disk directory for build (under root account)
```
# install -d -m 0777 /disk2
```

## 2.0 Before builds (env initialization)
```
cd <subdir>/poky
source oe-init-build-env build.tppg2
```

## 2.1 Build the image
'Make sure you did 2.0 step in this console. Then:
```
bitbake <imagename>
```
Final images and parts will be placed at
/disk2/build.26/tmp/deploy/images/<machine>/

## machine, distro, imagename table for IMG

| machine        | distro  | imagename    | command line                         | tasks / DL / HDD       | comment                                      |
| -------------- | ------- | ------------ | ------------------------------------ | ---------------------- | ---------------------------------------------|
| tpp-tppg2-arm5 | tps     | imgf-xboot   | bitbake mc:tpp-tppg2-arm5:imgf-xboot |  ~2800 /  6 GB / 29 GB | Builds several versions of XBoot and A926 FW |
| tpp-tppg2      | tps     | img-tst-tini | bitbake mc::img-tst-tini             |  ~4800 /  7 GB / 30 GB | Small CLI systemd-only image                 |
| tpp-tppg2      | tps     | img-tps-free | bitbake mc::img-tps-free             |  ~4950 /  7 GB / 31 GB | + package management, gdb, tcf-agent         |
| tpp-tppg2      | tps     | img-tps-base | bitbake mc::img-tps-base             |  ~5400 /  7 GB / 32 GB | + TiOS, TPS-WAN, other closed-source apps    |
| tpp-tppg2      | tps     | img-tps-repo | bitbake mc::img-tps-repo             |  ~9900 / 12 GB / 40 GB | Packages free + private collection           |
| tppg2, -arm5   | tps     | imgm-spmn    | bitbake imgm-spmn                     |  ~7700 / 7 GB / 60 GB | img-tst-tini + img-tps-free + imgf-xboot     |
| tppg2, -arm5   | tps     | imgm-spmt    | bitbake imgm-spmt                     |  ~8100 / 7 GB / 61 GB | img-tst-tini + img-tps-base + imgf-xboot     |
| qemux86-64     | tps     | imgm-spba    | bitbake imgm-spba                     |  ~7000 / ? GB / ?? GB | BuildApplience image                         |

## 2.2 Build the SDK
'Make sure you did 2.0 step in this console. Then:
```
bitbake -c populate_sdk <imagename>
```
SDK self-unpack image will be placed into
/disk2/build.26/tmp/deploy/sdk/

## cross-target, distro, imagename table for SDK

| cross-target   | distro  | imagename    | command line                                         | comment                                      |
| -------------- | ------- | ------------ | ---------------------------------------------------- | ---------------------------------------------|
| tpp-tppg2-arm5 | tps     | imgf-xboot   | bitbake mc:tpp-tppg2-arm5:imgf-xboot -c populate_sdk | A926 SDK (cross-tools + libs)                |
| tpp-tppg2      | tps     | img-tst-tini | bitbake mc::img-tst-tini -c populate_sdk             | A7 SDK                                       |
| tpp-tppg2      | tps     | img-tps-free | bitbake mc::img-tps-free -c populate_sdk             | A7 SDK                                       |
| tpp-tppg2      | tps     | img-tps-base | bitbake mc::img-tps-base -c populate_sdk             | A7 SDK                                       |
| tpp-tppg2      | tps     | img-tps-repo | bitbake mc::img-tps-repo -c populate_sdk             | A7 SDK                                       |


First bitbake run is time-consuming. All subsequent builds are incremental.

Xboot + arm926 firmware is not required for ISPBOOOT assembly.
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
