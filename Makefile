# $Id: Makefile,v 1.5 2004/02/05 16:35:04 mathie Exp $
#
# $Log: Makefile,v $
# Revision 1.5  2004/02/05 16:35:04  mathie
# * Build kata 5 and 6.
# * Link with Boost unit test library and libcrypto (for an MD5
#   implementation).
#
# Revision 1.4  2004/02/02 12:13:22  mathie
# * Add kata4.
#
# Revision 1.3  2004/02/02 09:08:15  mathie
# * Add options to control build process (switch on warnings, optional
#   debugging symbols/optimisation).
# * Factor addition of targets out to the start of the Makefile.
# * Target to clean everything up.
#
# Revision 1.2  2004/02/02 08:32:04  mathie
# * Add trailing CR, RCS keywords.
#

TARGETS = kata2 kata4 kata5 kata6
DEBUG = yes

BOOST_HOME = $(HOME)/src/not-mine/tarballs/boost-1.30.2

# Default to linking C++ instead of C
LINK.o = $(CXX) $(LDFLAGS) $(TARGET_ARCH)

CXXFLAGS = -Wall

ifeq ($(DEBUG),)
CXXFLAGS += -O2
else
CXXFLAGS += -O0 -g
endif

CPPFLAGS = -I$(BOOST_HOME)
LDLIBS += -L$(BOOST_HOME)/libs/test/build/bin/libboost_unit_test_framework.a/darwin/debug/runtime-link-static -lboost_unit_test_framework
LDLIBS += -lcrypto

all: $(TARGETS)

clean:
	rm -f $(TARGETS) *.o *~

# Dependencies
kata2: kata2.o
kata4: kata4.o
kata5: kata5.o
kata6: kata6.o
