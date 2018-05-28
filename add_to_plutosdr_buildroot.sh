#!/bin/bash

mkdir ../plutosdr-fw/buildroot/package/testPlutoSDR

echo -ne "TESTPLUTOSDR_SITE = \$(TOPDIR)/../../testPlutoSDR.tar.gz
TESTPLUTOSDR_SITE_METHOD = file
TESTPLUTOSDR_DEPENDENCIES = libiio

define TESTPLUTOSDR_EXTRACT_CMDS
	cp -a \$(TOPDIR)/../../testPlutoSDR/* \$(@D)/
endef

define TESTPLUTOSDR_BUILD_CMDS
	\$(TARGET_CC) \$(TARGET_CFLAGS) \$(TARGET_LDFLAGS) \$(@D)/testPlutoSDR.c -o \$(@D)/testPlutoSDR -liio
endef

define TESTPLUTOSDR_INSTALL_TARGET_CMDS
	\$(INSTALL) -D -m 0755 \$(@D)/testPlutoSDR \$(TARGET_DIR)/usr/bin/testPlutoSDR
endef

\$(eval \$(generic-package))" > ../plutosdr-fw/buildroot/package/testPlutoSDR/testPlutoSDR.mk

echo -ne "config BR2_PACKAGE_TESTPLUTOSDR
    bool \"testplutosdr\"
    help
        this is some message
		blah blah

comment \"blah blah testPlutoSDR\"" > ../plutosdr-fw/buildroot/package/testPlutoSDR/Config.in


# strip last 'endmenu' and append the following
sed -i '$ d' ../plutosdr-fw/buildroot/package/Config.in
printf "menu \"testPlutoSDR\"\n\tsource \"package/testPlutoSDR/Config.in\"\nendmenu\nendmenu" >> ../plutosdr-fw/buildroot/package/Config.in

# this syntax is slightly specific to use with the PlutoSDR firmware, other hardware would have a different config file
echo "BR2_PACKAGE_TESTPLUTOSDR=y" >> ../plutosdr-fw/buildroot/configs/zynq_pluto_defconfig

# Buildroot needs the tarball of the source for some reason...
tar -czf ../testPlutoSDR.tar.gz ../testPlutoSDR/
