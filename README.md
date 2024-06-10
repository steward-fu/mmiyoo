# Linux OS for Miyoo Mini (Plus)
TBD

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

&nbsp;

## How to flash kernel to MicroSD
```
$  sudo dd if=arch/arm/boot/uImage.xz of=/dev/sdX bs=1K seek=8
```

&nbsp;

## How to prepare MicroSD
```
1. Reserve 8MB in head
2. Create Partition 1 with FAT32 format
3. Create 'root' and 'dev' folders in root of Partition 1
4. Copy 'mininit' to root of Partition 1
5. Copy 'rootfs' to root of Partition 1
```

&nbsp;

## How to boot into Linux OS
```
Press SELECT button and then power on
```

&nbsp;

## How to boot into Stock OS
```
Power on
```
