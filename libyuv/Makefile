SOURCES=scale_argb.c planar_functions.c convert_argb.c convert_from_argb.c cpu_id.c row_any.c row_gcc.c row_common.c scale_any.c scale_gcc.c scale_common.c convert_from.c
LIB=libyuv_reduced.o

CPPFLAGS=-Wall -O3 -msse2 -I.
#LDFLAGS=

# CPPFLAGS+=-ffunction-sections

OBJECTS=$(patsubst %.c,$(PREFIX)%$(SUFFIX).o,\
        $(patsubst %.cpp,$(PREFIX)%$(SUFFIX).o,\
$(SOURCES)))
DEPENDS=$(patsubst %.c,$(PREFIX)%$(SUFFIX).d,\
        $(patsubst %.cpp,$(PREFIX)%$(SUFFIX).d,\
        $(filter-out %.o,""\
$(SOURCES))))

.PHONY: all clean
all: $(LIB)
ifneq "$(MAKECMDGOALS)" "clean"
  -include $(DEPENDS)
endif

clean:
	$(RM) $(OBJECTS) $(DEPENDS) $(LIB)

%.d: %.c
	@$(CC) $(CPPFLAGS) -MM -MT"$@" -MT"$*.o" -o $@ $<  2> /dev/null

%.d: %.cpp
	@$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MM -MT"$@" -MT"$*.o" -o $@ $<  2> /dev/null

$(LIB): $(OBJECTS)
	$(LD) -r -o $@ $^

