#!/bin/sh
install_mi_driver() {
    echo 4   > /sys/class/gpio/export
    echo 0   > /sys/class/pwm/pwmchip0/export
    echo 800 > /sys/class/pwm/pwmchip0/pwm0/period
    echo 50  > /sys/class/pwm/pwmchip0/pwm0/duty_cycle
    echo 1   > /sys/class/pwm/pwmchip0/pwm0/enable
    echo 50  > /sys/class/pwm/pwmchip0/pwm0/duty_cycle

    mount -t jffs2 mtd:appconfigs /appconfigs
    mount -t squashfs /dev/mtdblock5 /config
    mount -t squashfs /dev/mtdblock6 /customer

    insmod /config/modules/4.9.84/mhal.ko
    insmod /config/modules/4.9.84/mi_common.ko

    major=`cat /proc/devices | awk "\\$2==\""mi"\" {print \\$1}"\n`

    insmod /config/modules/4.9.84/mi_sys.ko cmdQBufSize=128 logBufSize=0
    insmod /config/modules/4.9.84/mi_gfx.ko
    insmod /config/modules/4.9.84/mi_divp.ko
    insmod /config/modules/4.9.84/mi_vdec.ko
    insmod /config/modules/4.9.84/mi_ao.ko
    insmod /config/modules/4.9.84/mi_disp.ko
    insmod /config/modules/4.9.84/mi_ipu.ko
    insmod /config/modules/4.9.84/mi_ai.ko
    insmod /config/modules/4.9.84/mi_venc.ko
    insmod /config/modules/4.9.84/mi_panel.ko
    insmod /config/modules/4.9.84/mi_alsa.ko

    mknod /dev/mi_sys   c $major 0
    mknod /dev/mi_gfx   c $major 1
    mknod /dev/mi_divp  c $major 2
    mknod /dev/mi_vdec  c $major 3
    mknod /dev/mi_ao    c $major 4
    mknod /dev/mi_disp  c $major 5
    mknod /dev/mi_ipu   c $major 6
    mknod /dev/mi_ai    c $major 7
    mknod /dev/mi_venc  c $major 8
    mknod /dev/mi_panel c $major 9
    mknod /dev/mi_alsa  c $major 10

    major=`cat /proc/devices | busybox awk "\\$2==\""mi_poll"\" {print \\$1}"`
    mknod /dev/mi_poll c $major 0

    insmod /config/modules/4.9.84/fbdev.ko
    mdev -s

    mount /dev/mtdblock4 /stock
    mount -o bind /config /stock/config
    mount -o bind /customer /stock/customer
    mount -o bind /appconfigs /stock/appconfigs
    mount -o bind /dev /stock/dev
    mount -o bind /sys /stock/sys
    mount -o bind /proc /stock/proc
}

set_pixel_clock() {
    cat /proc/kallsyms | grep HalPnlSetClkScPixel | tail -n 1 | awk '{print "echo 0x"$1" > /sys/devices/virtual/mmp/mmp/pixelclk"}' | sh
}

if [ "$1" == "init" ]; then
    install_mi_driver
    set_pixel_clock
fi
