TARGET  = patch_uboot
CC      = arm-linux-gnueabihf-gcc
CFLAGS  = 
LDFLAGS = -static
 
all:
	$(CC) main.c -o $(TARGET) $(CFLAGS) $(LDFLAGS)

clean:
	rm -rf $(TARGET)
