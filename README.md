# PlutoSDR Development
Just my notes/files on using the PlutoSDR with CentOS 7. For more info see documentation at:
 * https://wiki.analog.com/university/tools/pluto/developers
 * https://analogdevicesinc.github.io/libiio/
 * https://buildroot.org/downloads/manual/manual.html
 * https://elinux.org/images/2/2a/Using-buildroot-real-project.pdf

### Setup libiio driver
Upgrade a header file:

	wget https://raw.githubusercontent.com/torvalds/linux/master/include/uapi/linux/usb/functionfs.h
	sudo mv functionfs.h /usr/include/linux/usb/

Install some deps:

	sudo yum install xml2 libxml2-devel bison flex cmake

Build and install the library:

	git clone https://github.com/analogdevicesinc/libiio
	cd libiio
	mkdir build
	cd build
	cmake ../
	make
	sudo make install
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib64

Copy a rules file to ensure you have permissions to access the PlutoSDR device. You'll want to edit it before reloading udev to remove the `GROUP="plugdev"` and change `MODE="0664"` to `MODE="0666"`.

	wget https://raw.githubusercontent.com/analogdevicesinc/plutosdr-fw/master/scripts/53-adi-plutosdr-usb.rules
	sudo mv 53-adi-plutosdr-usb.rules /etc/udev/rules.d/
	udevadm control --reload-rules

You may also need to be part of dialout group, I already was so can't confirm if it really matters or not. You have to log out and back in for this to take effect.

	sudo usermod -a -G dialout <username>

Finally not entirely sure the following is required but was indicated:

	wget http://swdownloads.analog.com/cse/share/iiod-usb.conf
	sudo cp iiod-usb.conf /etc/init.d/

Check that everything works, plug in your PlutoSDR and run iio_info -s. If you're lucky you'll see something like the following.

	$ iio_info -s
	Library version: 0.15 (git tag: 6ecff5d)
	Compiled with backends: local xml ip usb
	Available contexts:
		0: Local devices [local:]
		1: 0456:b673 (Analog Devices Inc. PlutoSDR (ADALM-PLUTO)), serial=104473ce69910006eeff1e006dad89b158 [usb:2.4.

So what's cool is that you can actual use the same iio interface calls from both the host (laptop/PC) and target (embedded device/PlutoSDR) so the same testPlutoSDR source code will work on either platform, just with a re-compile. To use this program on the host just run `make`.


### Setup Zynq/ARM build environment
Install some dependencies:

	sudo yum install dtc glibc.i686 libstdc++.i686 git ccache fakeroot help2man mtools rsync openssl-devel ncurses-devel

	wget http://dfu-util.sourceforge.net/releases/dfu-util-0.9.tar.gz
	tar -zxf dfu-util-0.9.tar.gz
	cd dfu-util-0.9/
	./configure
	make
	sudo make install

Download and install the 2016.4 version of Xilinx Vivado and SDK to `/opt/Xilinx/` (or wherever). Then setup some environment variables:

	export CROSS_COMPILE=arm-xilinx-linux-gnueabi-
	export PATH=$PATH:/opt/Xilinx/SDK/2016.4/bin:/opt/Xilinx/SDK/2016.4/gnu/arm/lin/bin
	export VIVADO_SETTINGS=/opt/Xilinx/Vivado/2016.4/settings64.sh

Grab the core source code:

	git clone --recursive https://github.com/analogdevicesinc/plutosdr-fw.git
	git clone https://github.com/timcardenuto/testPlutoSDR.git 

### Add to buildroot image
If you copied/cloned this project into same directory as the `plutosdr-fw` project, then you can run the script `add_to_plutosdr_buildroot.sh`. This should create all the files needed to add this project to the build process.

### Build/flash image

Run build:

    cd plutosdr-fw
    make
    
This will take a while the first time. If it errors out on a missing /bin/tar then you make have to cd into plutosdr-fw/buildroot and run make, before going back to the top level to run make.


When you get a succesful build, put PlutoSDR into DFU mode (using SSH or serial):

	$ ssh plutosdr
	Warning: Permanently added 'plutosdr' (ECDSA) to the list of known hosts.
	root@plutosdr's password: 
	Welcome to:
	______ _       _        _________________
	| ___ \ |     | |      /  ___|  _  \ ___ \
	| |_/ / |_   _| |_ ___ \ `--.| | | | |_/ /
	|  __/| | | | | __/ _ \ `--. \ | | |    /
	| |   | | |_| | || (_) /\__/ / |/ /| |\ \
	\_|   |_|\__,_|\__\___/\____/|___/ \_| \_|

	v0.26
	http://wiki.analog.com/university/tools/pluto
	# device_reboot sf
	#

When the LED stops blinking run the DFU reflash command:

	$ dfu-util -a firmware.dfu -D build/pluto.dfu
	dfu-util 0.9
	Copyright 2005-2009 Weston Schmidt, Harald Welte and OpenMoko Inc.
	Copyright 2010-2016 Tormod Volden and Stefan Schmidt
	This program is Free Software and has ABSOLUTELY NO WARRANTY
	Please report bugs to http://sourceforge.net/p/dfu-util/tickets/

	Match vendor ID from file: 0456
	Match product ID from file: b673
	Opening DFU capable USB device...
	ID 0456:b674
	Run-time device DFU version 0110
	Claiming USB DFU Interface...
	Setting Alternate Setting #1 ...
	Determining device status: state = dfuIDLE, status = 0
	dfuIDLE, continuing
	DFU mode device DFU version 0110
	Device returned transfer size 4096
	Copying data from PC to DFU device
	Download	[=========================] 100%      9539799 bytes
	Download done.
	state(7) = dfuMANIFEST, status(0) = No error condition is present
	state(2) = dfuIDLE, status(0) = No error condition is present
	Done!

When that's done, unplug and reattach the PlutoSDR. Now when you ssh/serial in you'll find `/usr/bin/testPlutoSDR` which will use the IIO library to tune and receive samples from the AD936x.


