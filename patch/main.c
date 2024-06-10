/*
 * UBoot Patch Tool for Miyoo Mini v4
 * Steward Fu
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define BOOTCMD     "bootcmd=gpio output 85 1;bootlogo 0 0 0 0 0;mw 1f001cc0 11;gpio out 8 0;sf probe 0;sf read 21000000 5f500 100;sf read 210003e8 ${sf_kernel_start} ${sf_kernel_size};mmc read 212dc6c0 10 2000;gpio out 8 1;sleep 1;gpio output 4 1;go 21000000;bootm 22000000;"

#define LDR_OFFSET  0x5f500
#define CRC_OFFSET  0x5f000
#define ENV_OFFSET  0x5f004
#define ENV_LENGTH  (0x1000 - 4)

static int env_cnt = 0;
static char env_buf[32][1024] = {0};

static int m0_len = 0;
static uint8_t m0_buf[0x80000] = {0};

static uint8_t loader[] = {
    0x3c, 0x00, 0x9f, 0xe5, 0x00, 0x00, 0x90, 0xe5,
    0x01, 0x00, 0x10, 0xe3, 0x01, 0x00, 0x00, 0x0a,
    0x30, 0x00, 0x9f, 0xe5, 0x01, 0x00, 0x00, 0xea,
    0x2c, 0x00, 0x9f, 0xe5, 0xff, 0xff, 0xff, 0xea,
    0x22, 0x14, 0xa0, 0xe3, 0x03, 0x26, 0xa0, 0xe3,
    0x00, 0x30, 0x90, 0xe5, 0x00, 0x30, 0x81, 0xe5,
    0x04, 0x00, 0x80, 0xe2, 0x04, 0x10, 0x81, 0xe2,
    0x04, 0x20, 0x52, 0xe2, 0xf9, 0xff, 0xff, 0x1a,
    0x0e, 0xf0, 0xa0, 0xe1, 0x2c, 0x78, 0x20, 0x1f,
    0xe8, 0x03, 0x00, 0x21, 0xc0, 0xc6, 0x2d, 0x21
};

static void dump_mtd(void)
{
#ifndef PC
    system("dd if=/dev/mtdblock0 of=m0.backup");
    system("dd if=/dev/mtdblock1 of=m1.backup");
    system("dd if=/dev/mtdblock2 of=m2.backup");
    system("dd if=/dev/mtdblock3 of=m3.backup");
    system("dd if=/dev/mtdblock4 of=m4.backup");
    system("dd if=/dev/mtdblock5 of=m5.backup");
    system("dd if=/dev/mtdblock6 of=m6.backup");
    system("dd if=/dev/mtdblock7 of=m7.backup");
#endif
}

static unsigned int crc32b(unsigned char *buf, int len)
{
    int i = 0;
    int j = 0;
    unsigned int crc = 0;
    unsigned int byte = 0;
    unsigned int mask = 0;
 
    crc = 0xffffffff;
    for (i = 0; i < len; i++) {
        byte = buf[i];
        crc = crc ^ byte;
        for (j = 7; j >= 0; j--) {
            mask = -(crc & 1);
            crc = (crc >> 1) ^ (0xedb88320 & mask);
        }
    }
    return ~crc;
}

static int parse_string(const char *buf)
{
    int r1 = 0;
  
    env_cnt = 0;
    memset(env_buf, 0, sizeof(env_buf));
    while (*buf) {
        if (memcmp(buf, "bootcmd=", 8)) {
            printf("env:\"%s\"\n", buf);
            strcpy(env_buf[env_cnt++], buf);
        }
        buf += strlen(buf) + 1;
    }
    return 0;
}

static int is_loader_existing(int addr)
{
    while (1) {
        if (0x5ffff < addr) {
            break;
        }
        if (m0_buf[addr]) {
            return 1;
        }
        addr += 1;
    }
    return 0;
}

int main(int argc, char **argv)
{
    int cc = 0;
    int fd = -1;
    uint8_t *pbuf = NULL;

    dump_mtd();
    fd = open("m0.backup",0);
    if (fd < 0) {
        printf("failed to open m0.backup\n");
        return -1;
    }
    m0_len = read(fd, m0_buf, sizeof(m0_buf));
    close(fd);

    parse_string(&m0_buf[ENV_OFFSET]);
    printf("loader exists: %d\n", is_loader_existing(LDR_OFFSET));

    memset(&m0_buf[ENV_OFFSET], 0, ENV_LENGTH);
    pbuf = &m0_buf[ENV_OFFSET];
    for (cc = 0; cc < env_cnt; cc++) {
        strcpy(pbuf, env_buf[cc]);
        pbuf += strlen(env_buf[cc]) + 1;
        printf("append: \"%s\"\n", env_buf[cc]);
    }
    strcpy(pbuf, BOOTCMD);
    printf("append: \"%s\"\n", BOOTCMD);

    memcpy(&m0_buf[LDR_OFFSET], loader, sizeof(loader));
    *((uint32_t *)&m0_buf[CRC_OFFSET]) = crc32b(&m0_buf[ENV_OFFSET], ENV_LENGTH);
    printf("crc32: 0x%x\n", *((uint32_t *)&m0_buf[CRC_OFFSET]));

    unlink("m0.patched");
    fd = open("m0.patched", 0x41, 0644);
    if (fd < 0) {
        printf("failed to write patch file");
        return -1;
    }
    write(fd, m0_buf, m0_len);
    close(fd);
    system("dd if=m0.patched of=/dev/mtdblock0");
    system("sync");
    return 0;
}

