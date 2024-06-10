#define _DEFAULT_SOURCE
#define _GNU_SOURCE

#include <sys/mount.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <linux/loop.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "main.h"

FILE *logfd=NULL;
char logbuf[255]={0};

int switch_root(void)
{
    if(syscall(SYS_pivot_root, ".", "boot")){
        dbg("failed to pivot root: %d\n", errno);
        return -1;
    }
    return 0;
}

int logetfree(void)
{
    int fd = open("/dev/loop-control", O_RDWR);
    if (fd < 0) {
        dbg("failed to open '/dev/loop-control': %d\n", fd);
        return -1;
    }

    int devnr = ioctl(fd, LOOP_CTL_GET_FREE, NULL);
    if (devnr < 0) {
        dbg("failed to acquire free loop device: %d\n", devnr);
    } else {
        dbg("free loop device: %d\n", devnr);
    }
    close(fd);
    return devnr;
}

int losetup(const char *loop, const char *file)
{
    dbg("setting up loop: '%s' via '%s'\n", file, loop);
    int filefd = open(file, O_RDONLY);
    if (filefd < 0) {
        dbg("failed to open '%s': %d\n", file, filefd);
        return -1;
    }

    int loopfd = open(loop, O_RDONLY);
    if (loopfd < 0) {
        dbg("failed to open '%s': %d\n", loop, loopfd);
        close(filefd);
        return -1;
    }

    int res = ioctl(loopfd, LOOP_SET_FD, (void *)(intptr_t)filefd);
    if (res < 0) {
        dbg("failed to setup loop device '%s': %d\n", loop, res);
    }
    close(loopfd);
    close(filefd);
    return res;
}

int create_mount_point(const char *path)
{
    if (mkdir(path, 0755)) {
        if (errno != EEXIST) {
            dbg("failed to create '%s' mount point: %d\n", path, errno);
            return -1;
        }
    }
    return 0;
}

int main(int argc, char **argv, char **envp)
{
    FILE *kmsg = fopen("/dev/kmsg", "w");
    
    logfd = stderr;
    if(mount("devtmpfs", "/dev", "devtmpfs", 0, NULL) && errno != EBUSY){
        dbg("failed to mount devtmpfs on /dev: %d\n", errno);
    }

    if(kmsg){
        setlinebuf(kmsg);
        logfd = kmsg;
    }

    dbg("mininit 2.0.2\n");
    if(!kmsg){
        dbg("failed to open '/dev/kmsg': %d\n", errno);
    }

    if(chdir("/")){
        dbg("failed to change to '/' directory: %d\n", errno);
        return -1;
    }

    int devnr = logetfree();
    if(devnr < 0){
        devnr = 0;
    }

    char loop_dev[9 + 10 + 1]={0};
    sprintf(loop_dev, "/dev/loop%i", devnr);
    losetup(loop_dev, ROOTFS_FILE);

    dbg("mounting '%s' on '/root'\n", ROOTFS_FILE);
    if (mount(loop_dev, "/root", ROOTFS_TYPE, MS_RDONLY, NULL)) {
        dbg("failed to mount the rootfs image: %d\n", errno);
        return -1;
    }
    dbg("%s mounted on /root\n", ROOTFS_FILE);

    if (chdir("/root")) {
        dbg("failed to change to '/root' directory: %d\n", errno);
        return -1;
    }

    dbg("moving '/dev' mount\n");
    if (mount("/dev", "dev", NULL, MS_MOVE, NULL)) {
        dbg("failed to move the '/dev' mount: %d\n", errno);
        return -1;
    }

    int fd = open("dev/console", O_RDWR, 0);
  if (fd < 0) {
        dbg("failed to re-open console: %d\n", fd);
        return -1;
    }
    
  if (dup2(fd, 0) != 0 || dup2(fd, 1) != 1 || dup2(fd, 2) != 2) {
        dbg("failed to duplicate console handles\n");
        return -1;
    }

    if (fd > 2) {
    close(fd);
  }
    fd = -1;
    if (switch_root()) {
        if (fd >= 0) {
      close(fd);
    }
        return -1;
    }

    if (chroot(".")) {
        dbg("failed to chroot to new root: %d\n", errno);
        if (fd >= 0) close(fd);
        return -1;
    }

    if (chdir("/")) {
        dbg("failed to chdir to new root: %d\n", errno);
        if (fd >= 0) close(fd);
        return -1;
    }

    dbg("root switch done\n");
    if (fd >= 0) {
        dbg("removing initramfs contents\n");
        const char *executable = argv[0];
        while (*executable == '/') {
      executable+= 1;
    }
        if (unlinkat(fd, executable, 0)) {
            dbg("failed to remove '%s' executable: %d\n", executable, errno);
        }
        if (unlinkat(fd, "dev/console", 0)) {
            dbg("failed to remove '/dev/console': %d\n", errno);
        }
        if (unlinkat(fd, "dev", AT_REMOVEDIR)) {
            dbg("failed to remove '/dev' directory: %d\n", errno);
        }
        if (unlinkat(fd, "boot", AT_REMOVEDIR)) {
            dbg("failed to remove '/boot' mount point: %d\n", errno);
        }
        if (unlinkat(fd, "root", AT_REMOVEDIR)) {
            dbg("failed to remove '/root' directory: %d\n", errno);
        }
        if (close(fd)) {
            dbg("failed to close initramfs: %d\n", errno);
        }
    }

    const char *inits[] = {
        "/sbin/init",
        "/etc/init",
        "/bin/init",
        "/bin/sh",
        NULL,
    };
    for (int i = 0; ; i++) {
        if (!inits[i]) {
            dbg("failed to find the init executable\n");
            return -1;
        }
        dbg("init: %s\n", inits[i]);
        if (!access(inits[i], X_OK)) {
            argv[0] = (char *)inits[i];
            break;
        }
    }
    dbg("starting %s\n", argv[0]);
    if (kmsg) {
        logfd = stderr;
        fclose(kmsg);
    }

    execvpe(argv[0], argv, envp);
    dbg("failed to execute 'init': %d\n", errno);
    return -1;
}

