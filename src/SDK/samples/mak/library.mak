include $(ROOT)/mak/common.mak

.SUFFIXES: .xml .rsp

OBJECTS = $(SOURCES:.cpp=.o)

LIB = lib$(LIBRARY).$(PLATFORM_SUFFIX)

.cpp.o:
	$(COMPILE_COMMAND) -c -o $@  $(LIBRARY_COMPILE_OPTIONS) $(PEGASUS_INCLUDES) $(DEFINES) $*.cpp

.xml.rsp:
	@ wbemexec $*.xml > $*.rsp | cd .

$(LIB): $(OBJECTS) Makefile $(ROOT)/mak/library.mak
	$(LIBRARY_LINK_COMMAND) $(LIBRARY_LINK_OPTIONS) $(DEFINES) $(LINK_OUT)$@ $(OBJECTS) $(SYS_LIBS) $(DYNAMICLIBRARIES)
	$(MAKE) -i unlink
	ln -f -s $(SAMPLES_DIR)/$(SOURCE_DIR)/$(LIB) $(SYM_LINK_LIB).$(PLATFORM_SUFFIX)

rebuild:
	$(MAKE) clean
	$(MAKE)

clean:
	rm -f $(OBJECTS)
	$(MAKE) -i unlink
	rm -f $(LIB)
	rm -f $(XMLRESPONSES)

XMLRESPONSES = $(XMLSCRIPTS:.xml=.rsp)
tests: $(XMLRESPONSES)

unlink:
	rm -f $(SYM_LINK_LIB).$(PLATFORM_SUFFIX)
