## Install Latest GNU Radio on CentOS 7
There's an RPM package for GNU Radio in EPEL now, but if we want the latest then need to do the following.

	sudo yum install cmake3 doxygen swig boost-devel log4cpp-devel gmp-devel orc-devel PyYAML pygobject2-devel gtk3-devel cairo-devel pycairo-devel numpy python-mako python-sphinx python-six uhd* codec2-devel gsm-devel zeromq-devel

Even if you have `python-mako` which appears to have a high enough version, gnuradio complains so I installed with `pip`.

	sudo pip install mako

Qt5 is an issue to get gnuradio-companion.


## Install on CentOS 7

The only real gotchas were that it requires a slightly newer version of `gflags` and `glog` because of a namespace change. Also, you'll need cmake3 for `glog` but you can get that normally in CentOS.

	wget https://github.com/gflags/gflags/archive/v2.2.1.tar.gz
	tar -zxf gflags-2.2.1.tar.gz
	cd gflags-2.2.1
	mkdir build
	cd build
	cmake -DCMAKE_INSTALL_PREFIX=/usr -DBUILD_SHARED_LIBS=ON ..
	make
	sudo make install
	sudo ldconfig


	wget https://github.com/google/glog/archive/v0.3.5.tar.gz
	tar -zxf glog-0.3.5.tar.gz
	cd glog-0.3.5
	mkdir build
	cmake3 -DCMAKE_INSTALL_PREFIX=/usr -DBUILD_SHARED_LIBS=ON ..
	make
	sudo make install
	sudo ldconfig

The other wierd thing was these got installed to `/usr/lib` and gnss-sdr wanted them in `/usr/lib64` so I linked them there. Not sure if you can get them installed there during the build....

	sudo ln -s /usr/lib/libgflags.so /usr/lib64/libgflags.so
	sudo ln -s /usr/lib/libglog.so /usr/lib64/libglog.so


If you want PlutoSDR and other support there were alot of errors for:

	error: ‘runtime_error’ is not a member of ‘std’

Not sure why since that should be a member of std in C98 but if you upgrade to C++11 this goes away.

	sudo yum install centos-release-scl
	sudo yum install devtoolset-4
	scl enable devtoolset-4 bash

Then rebuild. You may need to always enable that toolset in your bash sessions to run gnss-sdr if it's also a runtime problem and not just a buildtime problem.

The other buildtime options are explain [here](https://gnss-sdr.org/docs/tutorials/configuration-options-building-time/), you need to enable each radio type that you want it to support.

	cmake -DENABLE_PLUTOSDR=ON -DCMAKE_BUILD_TYPE=Debug ..


## Available Receivers
The signal sources that work with gnss-sdr are shown in the [source code](https://github.com/gnss-sdr/gnss-sdr/tree/master/src/algorithms/Signal_Source/adapters). The following strings are accepted as values for the `SignalSource.implementation` argument in the gnss-sdr conf file:

	* Ad9361_Fpga_Signal_Source
	* Custom_Udp_Signal_Source
	* File_Signal_Source
	* Flexiband_Signal_Source
	* Fmcomms2_Signal_Source
	* Gen_Signal_Source
	* Gn3s_Signal_Source
	* Labsat_Signal_Source
	* Multichannel_File_Signal_Source
	* Nsr_File_Signal_Source
	* Osmosdr_Signal_Source
	* Plutosdr_Signal_Source
	* Raw_Array_Signal_Source
	* Rtl_Tcp_Signal_Source
	* Spir_File_Signal_Source
	* Spir_Gss6450_File_Signal_Source
	* Two_Bit_Cpx_File_Signal_Source
	* Two_Bit_Packed_File_Signal_Source
	* Uhd_Signal_Source




