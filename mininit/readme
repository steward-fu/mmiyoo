Mininit
=======

This program enables you to use a SquashFS image as the root filesystem of an embedded Linux system. If you want to use a different file system instead of SquashFS, change the `ROOTFS_TYPE` macro in `mininit.c`.

The file system containing the rootfs image is expected to be a FAT file system with long file names (vfat). If you want to use a different file system, change the `BOOTFS_TYPE` macro in `mininit.c`.

Mininit will mount a `devtmpfs` file system to get the device nodes it needs to mount other partitions, so make sure you enable this in your kernel config.

In the kernel configuration you should enable the file system used for the rootfs image and the file system used to host that image. Also you should enable loop device support. No pre-created loop devices are required, since Mininit will allocate them on demand via `/dev/loop-control`.

Mininit does not perform any module loading, so all options it needs will have to be built-in (`=y`) rather than built as modules (`=m`). Your rootfs can of course load kernel modules for functionality that is used later in the boot process, after Mininit has done its job.

A typical kernel config would contain the following lines:
```
CONFIG_BLK_DEV_LOOP=y
CONFIG_BLK_DEV_LOOP_MIN_COUNT=0
CONFIG_DEVTMPFS=y
CONFIG_SQUASHFS=y
CONFIG_VFAT_FS=y
```

There are two possible locations where Mininit can be loaded from: initramfs (`mininit-initramfs` binary) or a system partition (`mininit-syspart` binary). The following two sections describe these two deployment options.

Mininit in Initramfs
--------------------

Initramfs is a ramdisk that is populated from a "cpio" archive that is built into the kernel itself. Mininit should be run as the `/init` process in initramfs. It can run without any other files present in the initramfs, but it is recommended to have a working `/dev/console` so you can read the logging. An example initramfs manifest would look like this:
```
dir     /dev                                    0755    0 0
nod     /dev/console                            0644    0 0 c 5 1
file    /init           mininit-initramfs       0755    0 0
```
Read `Documentation/filesystems/ramfs-rootfs-initramfs.txt` for more information on populating initramfs.

You must add a `boot=X` parameter to the kernel command line, where `X` is a comma-separated list of the possible device nodes for the filesystem on which the rootfs image is stored.

A typical kernel config would contain the following lines:
```
CONFIG_BLK_DEV_INITRD=y
CONFIG_INITRAMFS_SOURCE="initramfs-tree.txt"
CONFIG_CMDLINE_BOOL=y
CONFIG_CMDLINE="boot=/dev/mmcblk0p1"
```

Initramfs is the most flexible approach to booting Linux, but it does require the manifest text file and the Mininit binary to be present in your kernel source tree. This can make it more difficult to set up the build environment. Also you will have to do an incremental kernel build every time you change Mininit.

Mininit in Boot Partition
-------------------------

Instead of using initramfs, Mininit can be stored on the same partition that hosts the rootfs image. Typically this will be a "system partition" which also hosts the kernel (`zImage`/`vmlinuz.bin`), optionally an image containing kernel modules (`modules.squashfs`) and possibly the later stages of the bootloader.

When run in this way, Mininit requires mount points (directories, typically empty) named `dev` and `root` in the boot partition.

A typical kernel config would contain the following lines:
```
CONFIG_DEVTMPFS_MOUNT=y
CONFIG_CMDLINE_BOOL=y
CONFIG_CMDLINE="root=/dev/mmcblk0p1 rootfstype=vfat ro rootwait init=/mininit-syspart"
```

Boot Partition Layout
---------------------

The following file names are used for the rootfs image on the boot partition:
```
rootfs.squashfs          : root file system
rootfs.squashfs.bak      : fallback root file system
update_r.bin             : replacement root file system
```

If a replacement rootfs image is present, the following rename sequence is performed:
> update_r.bin -> rootfs.squashfs -> rootfs.squashfs.bak -> (deleted)

For every image file in the list, there can be a text file with a `.sha1` suffix that contains the checksum of the image file. Note that Mininit doesn't perform checksum checks, but it does apply the same rename sequence to checksum files on updates, such that they are kept in sync with their respective image files.

If the argument `rootfs_bak` appears on the kernel command line, the fallback root file system is mounted instead of the main one. Also the update behavior is changed to preserve the fallback image. The intended use is for the boot loader to add the `rootfs_bak` argument to the kernel command line if for example specific button is pressed during boot, to allow the user to recover from a bad or corrupted rootfs image.

Optionally, the boot partition can also hold images containing kernel modules:
```
modules.squashfs         : kernel modules file system
modules.squashfs.bak     : fallback kernel modules file system
update_m.bin             : replacement kernel modules file system
```
Mininit takes care of renaming these images on updates, similar to the rootfs update mechanism, but it doesn't mount the modules image.

If you want to customize or remove the update mechanism, edit the `perform_updates()` function in `mininit.c`.

RootFS Image Layout
-------------------

Inside the rootfs image, mount points are expected at `/dev` and `/boot`, where devtmpfs and the boot partition will end up respectively. The boot partition will be mounted read-only when control is transferred to the `/sbin/init` of the rootfs.

Building
--------

To compile, pass your cross compiler to Make via the CC variable, like this:

> $ make CC=mipsel-gcw0-linux-uclibc-gcc

If you want additional debug logging, see `LOG_LEVEL` in `debug.h`.

Credits
-------

Developed for OpenDingux (https://github.com/opendingux/) by Paul Cercueil <paul@crapouillou.net> and Maarten ter Huurne <maarten@treewalker.org>.

Based on Dingux mininit (http://code.google.com/p/dingoo-linux/) by Ignacio Garcia Perez <iggarpe@gmail.com>.

License
-------

Mininit is licensed under the 2-clause BSD license. See the file `license.txt` for details.
