
ifeq ($(COMPILER),xlc)
  LINK_COMMAND = makeC++SharedLib
  LINK_ARGUMENTS = -p 0
  LINK_OUT = -o
endif

ifeq ($(COMPILER),acc)
  LINK_COMMAND = aCC -b
  ifeq ($(PEGASUS_SUPPORTS_DYNLIB),yes)
    LINK_COMMAND += -Wl,+b/usr/lib -Wl,+s
  endif
  ifdef PEGASUS_DEBUG
    LINK_COMMAND += -g
  endif
  LINK_ARGUMENTS =
  LINK_OUT = -o
endif

ifeq ($(COMPILER),gnu)
  LINK_COMMAND = g++ -shared
  LINK_ARGUMENTS =
  LINK_OUT = -o
endif

ifeq ($(COMPILER),deccxx)
  LINK_COMMAND = cxx -shared
  LINK_ARGUMENTS =
  LINK_OUT = -o
endif

FULL_LIB=$(LIB_DIR)/lib$(LIBRARY)$(LIB_SUFFIX)

## Rule for all UNIX library builds
$(FULL_LIB): $(LIB_DIR)/target $(OBJ_DIR)/target $(OBJECTS) $(LIBRARIES) \
    $(ERROR)
ifneq ($(COMPILER),xlc)
  ## Actions for all UNIX compilers except xlc
  ifeq ($(PEGASUS_SUPPORTS_DYNLIB),yes)
    ## To generate shared libraries which will cause dynamic
    ## search of other shared libraries which they reference,
    ## must specify the referenced shared libraries as "-l<name>"
    ## DYNAMIC_LIBRARIES must be defined appropriately in the
    ## libraries.mak file that includes this file
    ##
	$(LINK_COMMAND) $(LINK_ARGUMENTS) -L$(LIB_DIR) $(LINK_OUT)$(FULL_LIB) $(OBJECTS) $(DYNAMIC_LIBRARIES)
  else
	$(LINK_COMMAND) $(LINK_ARGUMENTS) $(LINK_OUT) $(FULL_LIB) $(OBJECTS) $(LIBRARIES)
	$(TOUCH) $(FULL_LIB)
  endif
else
	ar crv $(PEGASUS_PLATFORM).lib $(OBJECTS) $(LIBRARIES)
	$(LINK_COMMAND) $(LINK_ARGUMENTS) $(LINK_OUT)$(FULL_LIB) $(PEGASUS_PLATFORM).lib
	rm -f $(PEGASUS_PLATFORM).lib
	$(TOUCH) $(FULL_LIB)
endif
	@ $(ECHO)

clean-lib: $(ERROR)
	rm -f $(FULL_LIB)

FILES_TO_CLEAN = $(OBJECTS) $(FULL_LIB)
