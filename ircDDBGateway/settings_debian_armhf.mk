# Makefile-include
#
# Special settings for  Emdebian Compile for ARM processor, Hans-J. Barthen, DL5DI 2013-01-02
# Rename this file to settings.mk before you compile the source for the Debian armhf platform
#
	export DEB_HOST_GNU_TYPE := arm-linux-gnueabihf
	export DATADIR := "/home/opendv/data"
	export BINDIR  := "$(DESTDIR)/usr/bin"
	export LOGDIR  := "/var/log/opendv"
	export CONFDIR := "/home/opendv/ircddbgateway"
	export CC      := $(DEB_HOST_GNU_TYPE)-g++
	export LDFLAGS := -g -Xlinker -rpath-link /lib/$(DEB_HOST_GNU_TYPE) -Xlinker -rpath-link /usr/lib/$(DEB_HOST_GNU_TYPE)
	export LIBS    := -L/usr/lib/$(DEB_HOST_GNU_TYPE) -L/usr/$(DEB_HOST_GNU_TYPE)/lib -lwx_gtk2u_adv-2.8 -lwx_gtk2u_core-2.8 -lwx_baseu-2.8 -pthread
#
# Comment out the first CFLAGS entry and uncomment the second to enable the DExtra Link. For DCS Link you must comment out the first
# CFLAGS entry and uncomment the third entry. You must do a "make clean" before rebuilding the software.
#
  export CFLAGS := -g -O2 -Wall -Wno-non-virtual-dtor -Wno-strict-aliasing -Wno-psabi -DLOG_DIR='$(LOGDIR)' -DCONF_DIR='$(CONFDIR)' -DDATA_DIR='$(DATADIR)' -DBIN_DIR='$(BINDIR)' -I/usr/include/$(DEB_HOST_GNU_TYPE) -I/usr/$(DEB_HOST_GNU_TYPE)/include -I/usr/include -I/usr/$(DEB_HOST_GNU_TYPE)/lib/wx/include/gtk2-unicode-release-2.8 -I/usr/$(DEB_HOST_GNU_TYPE)/include/wx-2.8 -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -D__WXGTK__ -pthread
# export CFLAGS := -g -O2 -Wall -Wno-non-virtual-dtor -Wno-strict-aliasing -Wno-psabi -DLOG_DIR='$(LOGDIR)' -DCONF_DIR='$(CONFDIR)' -DDATA_DIR='$(DATADIR)' -DBIN_DIR='$(BINDIR)' -I/usr/include/$(DEB_HOST_GNU_TYPE) -I/usr/$(DEB_HOST_GNU_TYPE)/include -I/usr/include -I/usr/$(DEB_HOST_GNU_TYPE)/lib/wx/include/gtk2-unicode-release-2.8 -I/usr/$(DEB_HOST_GNU_TYPE)/include/wx-2.8 -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -D__WXGTK__ -pthread -DDEXTRA_LINK
# export CFLAGS := -g -O2 -Wall -Wno-non-virtual-dtor -Wno-strict-aliasing -Wno-psabi -DLOG_DIR='$(LOGDIR)' -DCONF_DIR='$(CONFDIR)' -DDATA_DIR='$(DATADIR)' -DBIN_DIR='$(BINDIR)' -I/usr/include/$(DEB_HOST_GNU_TYPE) -I/usr/$(DEB_HOST_GNU_TYPE)/include -I/usr/include -I/usr/$(DEB_HOST_GNU_TYPE)/lib/wx/include/gtk2-unicode-release-2.8 -I/usr/$(DEB_HOST_GNU_TYPE)/include/wx-2.8 -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -D__WXGTK__ -pthread -DDCS_LINK
