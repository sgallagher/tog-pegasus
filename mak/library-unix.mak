ifeq ($(COMPILER),xlc)
  LINK_COMMAND = makeC++SharedLib_r
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
  ifdef PEGASUS_CCOVER
    FULL_LIBRARIES += $(CCOVER_LIB)/libcov.a
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

ifeq ($(COMPILER),ibm)
  LINK_COMMAND = c++ $(FLAGS)
  LINK_ARGUMENTS =
  LINK_OUT = -o
endif

FULL_LIB=$(LIB_DIR)/lib$(LIBRARY)$(LIB_SUFFIX)

## Rule for all UNIX library builds
$(FULL_LIB): $(LIB_DIR)/target $(OBJ_DIR)/target $(OBJECTS) $(FULL_LIBRARIES) \
    $(ERROR)
  ifeq ($(PEGASUS_SUPPORTS_DYNLIB),yes)
    ## To generate shared libraries which will cause dynamic
    ## search of other shared libraries which they reference,
    ## must specify the referenced shared libraries as "-l<name>"
    ## DYNAMIC_LIBRARIES must be defined appropriately in the
    ## libraries.mak file that includes this file
    ##
	$(LINK_COMMAND) $(LINK_ARGUMENTS) -L$(LIB_DIR) $(LINK_OUT) $(FULL_LIB) $(OBJECTS) $(DYNAMIC_LIBRARIES)

    ifeq ($(PEGASUS_PLATFORM),ZOS_ZSERIES_IBM)
      ## z/OS needs side definition files to link executables to
      ## dynamic libraries, so we have to copy them into the lib_dir
	touch $(ROOT)/src/$(DIR)/lib$(LIBRARY).x
	cp $(ROOT)/src/$(DIR)/lib$(LIBRARY).x $(LIB_DIR)
    endif
  else
	$(LINK_COMMAND) $(LINK_ARGUMENTS) $(LINK_OUT) $(FULL_LIB) $(OBJECTS) $(FULL_LIBRARIES)
  endif
	$(TOUCH) $(FULL_LIB)
	@ $(ECHO)

clean-lib: $(ERROR)
	rm -f $(FULL_LIB)

FILES_TO_CLEAN = $(OBJECTS) $(FULL_LIB)
