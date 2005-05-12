#//%2005////////////////////////////////////////////////////////////////////////
#//
#// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
#// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
#// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
#// IBM Corp.; EMC Corporation, The Open Group.
#// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
#// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
#// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
#// EMC Corporation; VERITAS Software Corporation; The Open Group.
#//
#// Permission is hereby granted, free of charge, to any person obtaining a copy
#// of this software and associated documentation files (the "Software"), to
#// deal in the Software without restriction, including without limitation the
#// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
#// sell copies of the Software, and to permit persons to whom the Software is
#// furnished to do so, subject to the following conditions:
#// 
#// THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN
#// ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
#// "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
#// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
#// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
#// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
#// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
#// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#//
#//==============================================================================
ifeq ($(COMPILER),xlc)
  LINK_COMMAND = xlC_r
  ifdef PEGASUS_USE_RELEASE_DIRS
    ifdef PEGASUS_HAS_MESSAGES
      ifdef ICU_ROOT
        ifdef ICU_INSTALL
          LINK_ARGUMENTS = -qmkshrobj=$(AIX_LIB_PRIORITY) -blibpath:/usr/linux/lib:/usr/lib:/lib:$(ICU_INSTALL)/lib -Wl,-bhalt:$(AIX_LD_HALT)
        endif
      endif
    else
      LINK_ARGUMENTS = -qmkshrobj=$(AIX_LIB_PRIORITY) -blibpath:/usr/linux/lib:/usr/lib:/lib -Wl,-bhalt:$(AIX_LD_HALT)
    endif
  else
    LINK_ARGUMENTS = -qmkshrobj=$(AIX_LIB_PRIORITY) -Wl,-bhalt:$(AIX_LD_HALT)
  endif
  LINK_OUT = -o
  ifeq ($(PEGASUS_SUPPORTS_DYNLIB), yes)
    LINK_COMMAND += -G
  endif
endif

ifeq ($(COMPILER),acc)
  LINK_COMMAND = $(CXX) -b -Wl,+hlib$(LIBRARY)$(LIB_SUFFIX)
  ifeq ($(PEGASUS_PLATFORM), HPUX_IA64_ACC)
    LINK_COMMAND += +DD64 -mt
  else
    LINK_COMMAND += -Wl,-Bsymbolic
  endif
  ifeq ($(PEGASUS_SUPPORTS_DYNLIB),yes)
    ifdef PEGASUS_USE_RELEASE_DIRS
      LINK_COMMAND += -Wl,+b$(PEGASUS_DEST_LIB_DIR):/usr/lib
      ifeq ($(PEGASUS_PLATFORM), HPUX_PARISC_ACC)
        LINK_COMMAND += -Wl,+cdp,$(PEGASUS_PLATFORM_SDKROOT)/usr/lib:/usr/lib -Wl,+cdp,$(PEGASUS_HOME)/lib:$(PEGASUS_DEST_LIB_DIR)
      endif
    else
        LINK_COMMAND += -Wl,+b$(LIB_DIR):/usr/lib
    endif
  endif
  LINK_COMMAND += -Wl,+s
  ifdef PEGASUS_DEBUG
    LINK_COMMAND += -g
  endif
  ifdef PEGASUS_CCOVER
    EXTRA_LIBRARIES += $(CCOVER_LIB)/libcov.a
  endif
  LINK_ARGUMENTS =
  LINK_OUT = -o
endif

ifeq ($(COMPILER),gnu)
  ifneq ($(PEGASUS_PLATFORM),DARWIN_PPC_GNU)
   ifdef PEGASUS_USE_RELEASE_DIRS
      LINK_COMMAND = $(CXX) -shared
      LINK_ARGUMENTS = -Wl,-hlib$(LIBRARY)$(LIB_SUFFIX)  -Xlinker -rpath -Xlinker $(PEGASUS_DEST_LIB_DIR)
   else
      LINK_COMMAND = $(CXX) -shared
      LINK_ARGUMENTS = -Wl,-hlib$(LIBRARY)$(LIB_SUFFIX)  -Xlinker -rpath -Xlinker $(LIB_DIR) $(EXTRA_LINK_ARGUMENTS)
   endif
  else
    LINK_COMMAND = $(CXX) -dynamiclib
    LINK_ARGUMENTS = --helplib$(LIBRARY)$(LIB_SUFFIX) -ldl
  endif
  ifeq ($(PEGASUS_PLATFORM), SOLARIS_SPARC_GNU)
	LINK_ARGUMENTS = -Wl,-hlib$(LIBRARY)$(LIB_SUFFIX) -Xlinker -L$(LIB_DIR) $(EXTRA_LINK_ARGUMENTS)
  endif
  LINK_OUT = -o
endif

ifeq ($(COMPILER),deccxx)
  LINK_COMMAND = cxx -shared
  LINK_ARGUMENTS =
  LINK_OUT = -o
endif

ifeq ($(COMPILER),ibm)
  LINK_COMMAND = $(CXX) $(FLAGS)
  LINK_ARGUMENTS = -W "l,XPLINK,dll"
  LINK_OUT = -o
endif

ifeq ($(COMPILER),CC)
  LINK_COMMAND = CC
  LINK_ARGUMENTS = -G -KPIC -mt -h lib$(LIBRARY).so
  LINK_OUT = -o
endif

ifdef EXTRA_LINK_FLAGS
  LINK_COMMAND += $(EXTRA_LINK_FLAGS)
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


    ifeq ($(PEGASUS_PLATFORM),AIX_RS_IBMCXX)
	rm -f $(FULL_LIB)
    endif

	$(LINK_COMMAND) $(LINK_ARGUMENTS) -L$(LIB_DIR) $(LINK_OUT) $(FULL_LIB) $(OBJECTS) $(DYNAMIC_LIBRARIES) $(EXTRA_LIBRARIES)

    ifeq ($(PEGASUS_PLATFORM),ZOS_ZSERIES_IBM)
      ## z/OS needs side definition files to link executables to
      ## dynamic libraries, so we have to copy them into the lib_dir
	touch $(ROOT)/src/$(DIR)/lib$(LIBRARY).x
	cp $(ROOT)/src/$(DIR)/lib$(LIBRARY).x $(LIB_DIR)
    endif
  else
	$(LINK_COMMAND) $(LINK_ARGUMENTS) $(LINK_OUT) $(FULL_LIB) $(OBJECTS) $(FULL_LIBRARIES) $(EXTRA_LIBRARIES)
  endif
    ifeq ($(PEGASUS_PLATFORM),HPUX_PARISC_ACC)
	$(MAKE) --directory=$(LIB_DIR) -f $(PEGASUS_ROOT)/mak/library-unix.mak ln LIBRARY=lib$(LIBRARY) SUFFIX=$(LIB_SUFFIX) PLATFORM_SUFFIX=sl
    endif
    ifeq ($(PEGASUS_PLATFORM),HPUX_IA64_ACC)
	$(MAKE) --directory=$(LIB_DIR) -f $(PEGASUS_ROOT)/mak/library-unix.mak ln LIBRARY=lib$(LIBRARY) SUFFIX=$(LIB_SUFFIX) PLATFORM_SUFFIX=so
    endif
    ifdef PEGASUS_PLATFORM_LINUX_GENERIC_GNU
	$(MAKE) --directory=$(LIB_DIR) -f $(PEGASUS_ROOT)/mak/library-unix.mak ln LIBRARY=lib$(LIBRARY) SUFFIX=$(LIB_SUFFIX) PLATFORM_SUFFIX=so
    endif
	$(TOUCH) $(FULL_LIB)
	@ $(ECHO)

clean-lib: $(ERROR)
	rm -f $(FULL_LIB)

ln:
	ln -f -s $(LIBRARY)$(SUFFIX) $(LIBRARY).$(PLATFORM_SUFFIX)

FILES_TO_CLEAN = $(OBJECTS) $(FULL_LIB)
