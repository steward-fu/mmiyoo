TARGET  = mininit
CROSS   = arm-linux-gnueabi-
CC      = $(CROSS)gcc
LD      = $(CROSS)gcc
CFLAGS  = -O2 -fomit-frame-pointer -std=c99
LDFLAGS = -s -static
OBJS    = main.o

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $@

%.o: %.c 
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

