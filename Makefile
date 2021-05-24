SOURCES=main.cpp xcb_base.cpp xcb_img.cpp xcb_ewmh.cpp myui.cpp libyuv/libyuv_reduced.o
EXEC=xndiview

CPPFLAGS+=-O3 -Wall
#CXXFLAGS+=-std=c++11
PACKAGES=xcb xcb-shm

# LDFLAGS+=-Wl,--gc-sections

TARGET=$(shell $(CC) -dumpmachine)
ifneq "$(findstring apple-darwin,$(TARGET))" ""    # e.g. x86_64-apple-darwin16.6.0
  TARGET=x64                           # TODO?
  SDK=/Library/NDI SDK for Apple
  LDFLAGS+=-L"$(SDK)/lib/$(TARGET)" -lndi
else
  SDK=../NDI SDK for Linux

  CPPFLAGS+=-I"$(SDK)/include" -msse2
  LDFLAGS+=-L"$(SDK)/lib/$(TARGET)" -lndi

  LDFLAGS+=-Wl,-rpath="$(SDK)/lib/$(TARGET)"
endif

PKG_CONFIG:=pkg-config

ifneq "$(PACKAGES)" ""
  CPPFLAGS+=$(shell $(PKG_CONFIG) --cflags $(PACKAGES))
  LDFLAGS+=$(shell $(PKG_CONFIG) --libs $(PACKAGES))
endif

OBJECTS=$(patsubst %.c,$(PREFIX)%$(SUFFIX).o,\
        $(patsubst %.cpp,$(PREFIX)%$(SUFFIX).o,\
$(SOURCES)))
DEPENDS=$(patsubst %.c,$(PREFIX)%$(SUFFIX).d,\
        $(patsubst %.cpp,$(PREFIX)%$(SUFFIX).d,\
        $(filter-out %.o,""\
$(SOURCES))))

.PHONY: all clean
all: $(EXEC)
ifneq "$(MAKECMDGOALS)" "clean"
  -include $(DEPENDS)
endif

clean:
	$(RM) $(OBJECTS) $(DEPENDS) $(EXEC)
	$(MAKE) -C libyuv/ clean

%.d: xcbcpp/%.cpp
	@$(CC) $(CPPFLAGS) -MM -MT"$@" -MT"$*.o" -o $@ $<  2> /dev/null

%.o: xcbcpp/%.cpp
	@$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ -c $<

myui.d myui.o: CPPFLAGS+=-Ilibyuv

libyuv/libyuv_reduced.o:
	$(MAKE) -C libyuv libyuv_reduced.o

%.d: %.c
	@$(CC) $(CPPFLAGS) -MM -MT"$@" -MT"$*.o" -o $@ $<  2> /dev/null

%.d: %.cpp
	@$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MM -MT"$@" -MT"$*.o" -o $@ $<  2> /dev/null

$(EXEC): $(OBJECTS)
	$(CXX) -o $@ $^ $(LDFLAGS)

