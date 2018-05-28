CC=gcc
#/home/tc/Downloads/plutosdr/plutosdr-fw/buildroot/output/host/usr/bin/arm-xilinx-linux-gnueabi-gcc

CFLAGS=-I/usr/include
#/home/tc/Downloads/plutosdr/plutosdr-fw/buildroot/output/host/arm-buildroot-linux-gnueabi/sysroot/usr/include

LIBS=-liio

testPlutoSDR: testPlutoSDR.c
	$(CC) -o testPlutoSDR testPlutoSDR.c $(CFLAGS) $(LIBS)
