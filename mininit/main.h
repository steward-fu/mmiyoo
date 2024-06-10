#ifndef __DEBUG_H__
#define __DEBUG_H__

  #ifndef MS_MOVE
    #define MS_MOVE 8192
  #endif
  
  #define ROOTFS_TYPE "squashfs"
  #define ROOTFS_FILE "rootfs.squashfs"

  #define LOG(LEVEL, ...) do { \
	  snprintf(logbuf, sizeof(logbuf), LEVEL "mininit: " __VA_ARGS__); \
	  fprintf(logfd, logbuf); \
	} while (0)

  #define dbg(...) LOG("<14>", __VA_ARGS__)
  //#define dbg(...)

  int logetfree(void);
  int switch_root(void);
  int open_dir_to_clean(void);
  int create_mount_point(const char*);
  int losetup(const char*, const char*);
  
  const char *mount_boot(void);

#endif
