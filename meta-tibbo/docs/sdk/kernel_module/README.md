# Kernel module build with SDK example

## Preparation

To be able to build kernel modules with SDK you should run this commands ONCE
after the SDK installation:

```
$ . ~/sdkpath/environment-setup-*
$ cd ${SDKTARGETSYSROOT}/usr/src/kernel
$ make scripts;
```

## Before build:

1) (once) SDK should be installed;
2) (once) "Preparation" step is done (see above);

## Building the module

```
$ . ~/sdkpath/environment-setup-*
$ make KERNEL_SRC=${SDKTARGETSYSROOT}/usr/src/kernel
```
