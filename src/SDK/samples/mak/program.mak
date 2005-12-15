include $(ROOT)/mak/common.mak

OUTPUT = $(PROGRAM)

OBJECTS = $(SOURCES:.cpp=.o)
.cpp.o:
	$(COMPILE_COMMAND) $(PROGRAM_COMPILE_OPTIONS) -c -o $@ -I $(PEGASUS_INCLUDE_DIR) $(DEFINES) $*.cpp

$(OUTPUT): $(OBJECTS)
	$(PROGRAM_LINK_COMMAND) $(PROGRAM_LINK_OPTIONS) $(LINK_OUT)$@ $(OBJECTS) $(DYNAMIC_LIBRARIES) $(SYS_LIBS)

clean:
	@$(foreach i, $(OBJECTS), $(RM) $(i);)
	@$(foreach i, $(PROGRAM), $(RM) $(i);)
	@$(foreach i, $(EXTRA_CLEAN_FILES), $(RM) $(i);)
