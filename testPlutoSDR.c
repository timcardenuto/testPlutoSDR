#include <iio.h>
#include <stdio.h>
#include <getopt.h>		// for getopt_long support
#include <string.h>		// for strcmp


static char* uri; // usb:2.2.5
static char* devicename; // ad9361-phy
static long long frequency;
static long long bandwidth;
static long long samplerate;
static char* receivefile;
static char* transmitfile;
static int printinfo;
static int verbose;

// Cmd line help message
void displayCmdUsage() {
	puts("Usage: ./testPlutoSDR [URI] [OPTIONS] \n\
  -u   --uri         Specify device URI connection info (e.g. usb:x.x.x, ip:xxx.xxx.xxx.xxx, serial:) \n\
  -d   --device      Specify the device name (e.g. ad9361-phy) \n\
  -p   --print-info  Print attributes and channel info for specified device, or all devices if none specified \n\
  -v   --verbose     Prints additional output \n\
       --help        Display this message \n\
  \n\
  e.x.  ./testPlutoSDR -u usb:2.2.5 -d ad9361-phy -p \n");
	exit(1);
}

// Boilerplate getopt code to read cmd line arguments
void parseCmdArgs(int argc, char **argv) {
    int c;
	char *ptr;
   	while (1) {
        int option_index = 0;
        static struct option long_options[] = {
			{"uri",           required_argument, 	     0, 'u'},
			{"device",        required_argument,         0, 'd'},
			{"frequency",     required_argument,         0, 'f'},
			{"bandwidth",     required_argument,         0, 'b'},
			{"sample-rate",   required_argument,         0, 's'},
			{"receive",       required_argument,         0, 'r'},
			{"transmit",      required_argument,         0, 't'},
			{"print-info",    no_argument, 	    &printinfo, 'p'},
			{"verbose",             no_argument,  &verbose, 'v'},
			{"help",                no_argument,         0, 'h'},
			{0,                               0,         0,   0},
		};

		c = getopt_long_only(argc, argv, "u:d:f:b:s:r:t:pvh", long_options, &option_index);
		if (c == -1) { break; }
       	switch (c) {
			 case 0:
				/* If this option set a flag, do nothing else now. */
				if (long_options[option_index].flag != 0) { break; }
				printf ("option %s", long_options[option_index].name);
				if (optarg) { printf (" with arg %s", optarg); }
				printf ("\n");
				break;
			case 'u':
				uri = optarg;
		        break;
			case 'd':
                devicename = optarg;
                break;
			case 'f':
				frequency = strtoul(optarg, &ptr, 10);
				if (strcmp(ptr,"")) {
					 printf("Value %s of option %s is not a number \n", ptr, long_options[option_index].name);
					 exit(1);
				 }
				break;
			case 'b':
                bandwidth = strtoul(optarg, &ptr, 10);
				if (strcmp(ptr,"")) {
					 printf("Value %s of option %s is not a number \n", ptr, long_options[option_index].name);
					 exit(1);
				 }
                break;
			case 's':
				samplerate = strtoul(optarg, &ptr, 10);
				if (strcmp(ptr,"")) {
					 printf("Value %s of option %s is not a number \n", ptr, long_options[option_index].name);
					 exit(1);
				 }
				break;
			case 'r':
				receivefile = optarg;
				break;
			case 't':
				transmitfile = optarg;
				break;
       		case 'p':
				printinfo = 1;
		        break;
       		case 'v':
				verbose = 1;
		        break;
       		default:
				displayCmdUsage();
	    }
	}
	if (optind < argc) {
      	printf ("Unrecognized options: ");
      	while (optind < argc) {
        	printf ("%s ", argv[optind++]);
      	}
		printf("\n"); //putchar ('\n');
		displayCmdUsage();
    }
	return;
}


int receive(struct iio_context *ctx, char* file)
{
	struct iio_device *dev;
	struct iio_channel *rx0_i, *rx0_q;
	struct iio_buffer *rxbuf;
	FILE *fd;
	fd = fopen(file, "w");

	// tag::code[]
	dev = iio_context_find_device(ctx, "cf-ad9361-lpc");
	// end::code[]

	rx0_i = iio_device_find_channel(dev, "voltage0", 0);
	rx0_q = iio_device_find_channel(dev, "voltage1", 0);

	iio_channel_enable(rx0_i);
	iio_channel_enable(rx0_q);

	rxbuf = iio_device_create_buffer(dev, 4096, false);
	if (!rxbuf) {
		perror("Could not create RX buffer");
		exit(-1);
	}

	while (true) {
		void *p_dat, *p_end, *t_dat;
		ptrdiff_t p_inc;

		iio_buffer_refill(rxbuf);

		p_inc = iio_buffer_step(rxbuf);
		p_end = iio_buffer_end(rxbuf);

		for (p_dat = iio_buffer_first(rxbuf, rx0_i); p_dat < p_end; p_dat += p_inc, t_dat += p_inc) {
			const int16_t i = ((int16_t*)p_dat)[0]; // Real (I)
			const int16_t q = ((int16_t*)p_dat)[1]; // Imag (Q)

			/* Process here */
			fprintf(fd, "%i,%i,", i,q);
			fflush(fd);
		}


	}

	iio_buffer_destroy(rxbuf);
}

// Get all channels for this device
void printDeviceChannels(struct iio_device* dev) {
	int num_channels = iio_device_get_channels_count(dev);
	int j = 0;
	for (j=0; j<num_channels; j++) {
		struct iio_channel* chn;
		chn =  iio_device_get_channel(dev, j);
		int num_attrs = iio_channel_get_attrs_count(chn);
		printf("\n    Channel %i, Attributes %i\n", j, num_attrs);
		const char* id = iio_channel_get_id(chn);
		const char* name = iio_channel_get_name(chn);
		bool isoutput = iio_channel_is_output(chn);
		bool isscanel = iio_channel_is_scan_element(chn);

		printf("      %-30s: %s\n", "id", id);
		printf("      %-30s: %s\n", "name", name);
		if (isoutput) { printf("      %-30s: %s\n", "I/O", "output"); }
		else { printf("      %-30s: %s\n", "I/O", "input"); }
		printf("      %-30s: %i\n", "scan_element", isscanel);

		// Get all attributes for this channel
		int k = 0;
		for (k=0; k<num_attrs; k++) {
			const char* attr = iio_channel_get_attr(chn, k);

			char buf[2048];
			int ret = iio_channel_attr_read(chn, attr, buf, sizeof(buf));
			if (ret > 0) {
				printf("      %-30s: %s\n", attr, buf);
			} else {
				iio_strerror(-ret, buf, sizeof(buf));
				fprintf(stderr, "[ERROR]  Unable to read attribute %s: %s\n", attr, buf);
			}
		}
	}
}

// Get all attributes for this device
void printDeviceAttributes(struct iio_device* dev) {
	int num_attrs = iio_device_get_attrs_count(dev);
	int j = 0;
	for (j=0; j<num_attrs; j++) {
		const char* attr = iio_device_get_attr(dev, j);
		char buf[2048];
		int ret = iio_device_attr_read(dev, attr, buf, sizeof(buf));
		if (ret > 0) {
			printf("    %-30s: %s\n", attr, buf);
		} else {
			iio_strerror(-ret, buf, sizeof(buf));
			fprintf(stderr, "[ERROR]  Unable to read attribute %s: %s\n", attr, buf);
		}
	}
}

// Get 1 devices info
void printDeviceInfo(struct iio_device* dev) {
	const char* id = iio_device_get_id(dev);
	const char* name = iio_device_get_name(dev);
	printf("    %-30s: %s\n", "id", id);
	printf("    %-30s: %s\n", "name", name);

	printDeviceAttributes(dev);
	printDeviceChannels(dev);
}

// Get info for all devices in a given context
void printDevicesInfo(struct iio_context* ctx) {
	int num_devs = iio_context_get_devices_count(ctx);
	printf("\nDevices: %i\n",num_devs);
	int i = 0;
	for (i=0; i<num_devs; i++) {
		struct iio_device* dev = iio_context_get_device(ctx, i);
		const char* id = iio_device_get_id(dev);
		const char* name = iio_device_get_name(dev);

		printf("\n  Device %i\n", i);
		printf("    %-30s: %s\n", "id", id);
		printf("    %-30s: %s\n", "name", name);

		printDeviceAttributes(dev);
		printDeviceChannels(dev);
	}
}

// Helper to print errors and old/new values of attributes
int setAttribute(struct iio_channel* chn, char* attr, long long value) {
	// read current value
	long long val;
	int ret = iio_channel_attr_read_longlong(chn, attr, &val);
	if (ret != 0) { printf("[ERROR] Failed to read %s, %i\n", attr, ret); }
	else { printf("Current %-30s : %llu\n", attr, val); }

	// set new value
	ret = iio_channel_attr_write_longlong(chn, attr, value); /* RX baseband rate 5 MSPS */
	if (ret != 0) { printf("[ERROR] Failed to write %s, %i\n", attr, ret); }
	else {
		ret = iio_channel_attr_read_longlong(chn, attr, &val);
		if (ret != 0) { printf("[ERROR] Failed to read %s, %i\n", attr, ret); }
		else { printf("New %-34s : %llu\n", attr, val); }
	}
}



int main (int argc, char **argv)
{
	//if (argc == 1) { displayCmdUsage(); }
	parseCmdArgs(argc, argv);
	printf("#### Starting PlutoSDR Test ####\n");

	if (!uri) {
		struct iio_scan_context* scan_ctx = iio_create_scan_context(NULL, 0);
		if (!scan_ctx) {
			fprintf(stderr, "Unable to create scan context\n");
			return;
		}

		struct iio_context_info **info;
		ssize_t ret = iio_scan_context_get_info_list(scan_ctx, &info);
		if (ret < 0) {
			fprintf(stderr, "Unable to scan: %li\n", (long) ret);
			iio_scan_context_destroy(scan_ctx);
		}
		if (ret == 0) {
			printf("No contexts found.\n");
			iio_context_info_list_free(info);
			iio_scan_context_destroy(scan_ctx);
		}

		printf("Available contexts:\n");
		int i = 0;
		for (i = 0; i < (size_t)ret; i++) {
			uri = iio_context_info_get_uri(info[i]);
			printf("	%s\n", uri);
		}
	}

	//ctx = iio_create_context_from_uri("ip:192.168.2.1");
	struct iio_context* ctx = iio_create_context_from_uri(uri);
	printf("Created context for uri %s\n",uri);

	if (devicename) {
		struct iio_device* dev = iio_context_find_device(ctx, devicename);
		const char* name = iio_device_get_name(dev);
		printf("Found device %s\n",name);
		if (printinfo) { printDeviceInfo(dev); }

		if (frequency != 0) {
			struct iio_channel* chn = iio_device_find_channel(dev, "altvoltage0", true);
			setAttribute(chn, "frequency", frequency);
		}

		// TODO which settings on hardware are the right ones for freq/bw/sr?
		if (bandwidth != 0) {
			printf("Not implemented\n");
		}

		if (samplerate != 0) {
			struct iio_channel* chn = iio_device_find_channel(dev, "voltage0", false);
			setAttribute(chn, "sampling_frequency", samplerate);
		}

		if (receivefile) {
			printf("Press Ctrl-C to stop receiving data\n");
			receive(ctx, receivefile);
	 		printf("Finished receive()\n");
		}

		if (transmitfile) {
			printf("Not implemented\n");
		}

	} else {
		if (printinfo) { printDevicesInfo(ctx); }
	}

	iio_context_destroy(ctx);
 	printf("#### Finished PlutoSDR Test ####\n");
	return 0;
}
