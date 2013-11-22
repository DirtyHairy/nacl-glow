# The MIT License (MIT)
# 
# Copyright (c) 2013 Christian Speckner <cnspeckn@googlemail.com>
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

RELEASE = 1

TOOLCHAIN_x86 = $(NACL_SDK_ROOT)/toolchain/linux_x86_newlib
TOOLCHAIN_arm = $(NACL_SDK_ROOT)/toolchain/linux_arm_newlib

INCLUDE = -I$(NACL_SDK_ROOT)/include
LIBS = -lppapi_cpp -lppapi
SOURCE = glow.cc logger.cc renderer.cc surface.cc settings.cc
CXXFLAGS = -O2 -Wall

LIB_FLAVOR = $(if $(RELEASE),Release,Debug)

PREFIX_64 = $(TOOLCHAIN_x86)/bin/x86_64-nacl
CXX_64 = $(PREFIX_64)-g++
STRIP_64 = $(PREFIX_64)-strip
LDFLAGS_64 = -L$(NACL_SDK_ROOT)/lib/newlib_x86_64/$(LIB_FLAVOR) $(LIBS)

PREFIX_32 = $(TOOLCHAIN_x86)/bin/i686-nacl
CXX_32 = $(PREFIX_32)-g++
STRIP_32 = $(PREFIX_32)-strip
LDFLAGS_32 = -L$(NACL_SDK_ROOT)/lib/newlib_x86_32/$(LIB_FLAVOR) $(LIBS)

PREFIX_arm = $(TOOLCHAIN_arm)/bin/arm-nacl
CXX_arm = $(PREFIX_arm)-g++
STRIP_arm = $(PREFIX_arm)-strip
LDFLAGS_arm = -L$(NACL_SDK_ROOT)/lib/newlib_arm/$(LIB_FLAVOR) $(LIBS)

BIN_64 = glow_64.pexe
BIN_32 = glow_32.pexe
BIN_arm = glow_arm.pexe
BIN = $(BIN_64) $(BIN_32) $(BIN_arm)

OBJECTS_64 = $(patsubst %.cc,obj_64/%.o,$(SOURCE))
OBJECTS_32 = $(patsubst %.cc,obj_32/%.o,$(SOURCE))
OBJECTS_arm = $(patsubst %.cc,obj_arm/%.o,$(SOURCE))
OBJECTS = $(OBJECTS_64) $(OBJECTS_32) $(OBJECTS_arm)

GARBAGE = $(OBJECTS) obj_64 obj_32 obj_arm Makefile.depend
SEMIGARBAGE = $(BIN)

all: $(BIN)

$(BIN_64) : $(OBJECTS_64)
	$(CXX_64) -o $@ $^ $(LDFLAGS_64)
	[ -n "$(RELEASE)" ] && $(STRIP_64) $@ || true

$(BIN_32) : $(OBJECTS_32)
	$(CXX_32) -o $@ $^ $(LDFLAGS_32)
	[ -n "$(RELEASE)" ] && $(STRIP_32) $@ || true

$(BIN_arm) : $(OBJECTS_arm)
	$(CXX_arm) -o $@ $^ $(LDFLAGS_arm)
	[ -n "$(RELEASE)" ] && $(STRIP_arm) $@ || true

$(OBJECTS_64) : obj_64/%.o : %.cc
	-test -d obj_64 || mkdir obj_64
	$(CXX_64) $(CXXFLAGS) $(INCLUDE) -o $@ -c $<

$(OBJECTS_32) : obj_32/%.o : %.cc
	-test -d obj_32 || mkdir obj_32
	$(CXX_32) $(CXXFLAGS) $(INCLUDE) -o $@ -c $<

$(OBJECTS_arm) : obj_arm/%.o : %.cc
	-test -d obj_arm || mkdir obj_arm
	$(CXX_arm) $(CXXFLAGS) $(INCLUDE) -o $@ -c $<

clean:
	-rm -fr $(GARBAGE)

realclean: clean
	-rm -fr $(SEMIGARBAGE)

Makefile.depend: Makefile
	$(CXX_64) $(CXXFLAGS) $(INCLUDE) -MM $(SOURCE) | sed -e 's/^\(.*\.o:\)/obj_64\/\1/' > $@
	$(CXX_32) $(CXXFLAGS) $(INCLUDE) -MM $(SOURCE) | sed -e 's/^\(.*\.o:\)/obj_32\/\1/' >> $@
	$(CXX_arm) $(CXXFLAGS) $(INCLUDE) -MM $(SOURCE) | sed -e 's/^\(.*\.o:\)/obj_arm\/\1/' >> $@

include Makefile.depend
