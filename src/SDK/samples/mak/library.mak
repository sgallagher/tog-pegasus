RM = rm -f

include $(ROOT)/mak/common.mak

.SUFFIXES: .xml .rsp

TMP_OBJECTS = $(foreach i,$(SOURCES),$i)

ifeq ($(OS_TYPE),windows)
CPP_OBJECTS = $(TMP_OBJECTS:.cpp=.obj)
OBJECTS = $(CPP_OBJECTS:.c=.obj)
else
CPP_OBJECTS = $(TMP_OBJECTS:.cpp=.o)
OBJECTS = $(CPP_OBJECTS:.c=.o)
endif

LIB = lib$(LIBRARY).$(PLATFORM_SUFFIX)

.c.o:
	$(COMPILE_C_COMMAND) -c -o $@ $(LIBRARY_COMPILE_OPTIONS) -I $(PEGASUS_INCLUDE_DIR) $(DEFINES) $*.c

.cpp.o:
	$(COMPILE_COMMAND) -c -o $@  $(LIBRARY_COMPILE_OPTIONS) -I $(PEGASUS_INCLUDE_DIR) $(DEFINES) $*.cpp

.xml.rsp:
	@ wbemexec $*.xml > $*.rsp | cd .

$(LIB): $(OBJECTS) Makefile $(ROOT)/mak/library.mak
	$(LIBRARY_LINK_COMMAND) $(LIBRARY_LINK_OPTIONS) $(DEFINES) $(LINK_OUT)$@ $(OBJECTS) $(SYS_LIBS) $(DYNAMICLIBRARIES)
	$(MAKE) -i unlink
	ln -f -s $(PEGASUS_SAMPLES_DIR)/$(SOURCE_DIR)/$(LIB) $(SYM_LINK_LIB).$(PLATFORM_SUFFIX)

rebuild:
	$(MAKE) clean
	$(MAKE)

clean:
	@$(foreach i, $(OBJECTS), $(RM) $(i);)
	$(MAKE) -i unlink
	@$(foreach i, $(LIB), $(RM) $(i);)
	@$(foreach i, $(XMLRESPONSES), $(RM) $(i);)

XMLRESPONSES = $(XMLSCRIPTS:.xml=.rsp)
tests: $(XMLRESPONSES)

unlink:
	$(RM) $(SYM_LINK_LIB).$(PLATFORM_SUFFIX)
