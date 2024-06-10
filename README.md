# Linux OS for Miyoo Mini (Plus)
TBD

&nbsp;

# Installation
1. Patch U-Boot  
  You can patch U-Boot on Stock OS by [rel_u-boot_patch_20230719.zip](https://github.com/steward-fu/website/releases/download/miyoo-mini-plus/rel_u-boot_patch_20230719.zip)
2. Make bootable MicroSD for Linux OS
```
a). Reserve 8MB in head for kernel image
b). Create Partition 1 with FAT32 format
c). Create 'root' and 'dev' folders in root of Partition 1
d). Copy 'mininit' to root of Partition 1
e). Copy 'rootfs' to root of Partition 1
```
3. Flash kernel to MicroSD
```
$  sudo dd if=arch/arm/boot/uImage.xz of=/dev/sdX bs=1K seek=8
```

&nbsp;

# Linux and Stock OS
When press SELECT button and then power on, it goes to Linux OS. If user doesn't press the SELECT button, it boots into Stock OS by default.

&nbsp;

# Building

&nbsp;

## How to prepare the build environment (Docker)
```
$ sudo docker build -t rpatch .
```

&nbsp;

## How to delete the build environment (Docker)
```
$ sudo docker image rm rpatch
```

&nbsp;

## How to build kernel
```
$ cd kernel
$ sudo docker run -it --rm -u $(id -u ${USER}):$(id -g ${USER}) -v $(pwd):/kernel rpatch /bin/bash

Docker $ export PATH=/opt/prebuilt/bin/:$PATH
Docker $ cd /kernel
Docker $ ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- make miyoo_mini_defconfig
Docker $ ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- LOADADDR=0x20008000 make all -j4
```

&nbsp;

## How to build mininit
```
$ cd mininit
$ make
```

