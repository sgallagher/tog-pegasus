ifeq ($(COMPILER),xlc)
  LINK_COMMAND = xlC_r
  LINK_ARGUMENTS = -qmkshrobj=$(AIX_LIB_PRIORITY)
  LINK_OUT = -o
  ifeq ($(PEGASUS_SUPPORTS_DYNLIB), yes)
    LINK_COMMAND += -G
  endif
endif

ifeq ($(COMPILER),acc)
  LINK_COMMAND = $(CXX) -b -Wl,+hlib$(LIBRARY)$(LIB_SUFFIX)
  ifeq ($(HPUX_IA64_VERSION), yes)
    LINK_COMMAND += +DD64 -mt
  endif
  ifeq ($(PEGASUS_SUPPORTS_DYNLIB),yes)
    LINK_COMMAND += -Wl,+s -Wl,+b/opt/wbem/lib
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
	$(LINK_COMMAND) $(LINK_ARGUMENTS) -L$(LIB_DIR) $(LINK_OUT) $(FULL_LIB) $(OBJECTS) $(DYNAMIC_LIBRARIES) $(EXTRA_LIBRARIES)

    ifeq ($(PEGASUS_PLATFORM),ZOS_ZSERIES_IBM)
      ## z/OS needs side definition files to link executables to
      ## dynamic libraries, so we have to copy them into the lib_dir
	touch $(ROOT)/src/$(DIR)/lib$(LIBRARY).x
	cp $(ROOT)/src/$(DIR)/lib$(LIBRARY).x $(LIB_DIR)
    endif
    ifeq ($(OS),HPUX)
	$(MAKE) --directory=$(LIB_DIR) -f $(PEGASUS_ROOT)/mak/library-unix.mak ln LIBRARY=lib$(LIBRARY) SUFFIX=$(LIB_SUFFIX)
    endif
  else
	$(LINK_COMMAND) $(LINK_ARGUMENTS) $(LINK_OUT) $(FULL_LIB) $(OBJECTS) $(FULL_LIBRARIES) $(EXTRA_LIBRARIES)
  endif
	$(TOUCH) $(FULL_LIB)
	@ $(ECHO)

clean-lib: $(ERROR)
	rm -f $(FULL_LIB)

ln:
	ln -f -s $(LIBRARY)$(SUFFIX) $(LIBRARY).sl

FILES_TO_CLEAN = $(OBJECTS) $(FULL_LIB)
