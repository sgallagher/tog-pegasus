ifeq ($(PLATFORM_VERSION_SUPPORTED), yes)
  ifdef PLATFORM_COMPONENT_NAME
     DEFINES += -DPLATFORM_COMPONENT_NAME=\"$(PLATFORM_COMPONENT_NAME)\"
  else
     DEFINES += -DPLATFORM_COMPONENT_NAME=\"$(LIBRARY)\"
  endif
endif

INCLUDES = -I$(ROOT)/src $(EXTRA_INCLUDES)

include $(ROOT)/mak/common.mak

################################################################################
##
## Build list of object names.
##
################################################################################

TMP_OBJECTS = $(foreach i,$(SOURCES),$(OBJ_DIR)/$i)

ifeq ($(OS_TYPE),windows)
OBJECTS = $(TMP_OBJECTS:.cpp=.obj)
else
OBJECTS = $(TMP_OBJECTS:.cpp=.o)
endif

################################################################################
##
## Library rule:
##
################################################################################

ifeq ($(OS_TYPE),windows)
include $(ROOT)/mak/library-windows.mak
endif
ifeq ($(OS_TYPE),unix)
include $(ROOT)/mak/library-unix.mak
endif
ifeq ($(OS_TYPE),nsk)
include $(ROOT)/mak/library-nsk.mak
endif

################################################################################
##
## Clean rules:
##
################################################################################

include $(ROOT)/mak/clean.mak

################################################################################
##
## Build list of object names:
##
################################################################################

include $(ROOT)/mak/objects.mak

include $(ROOT)/mak/depend.mak

include $(ROOT)/mak/build.mak

include $(ROOT)/mak/docxx.mak

include $(ROOT)/mak/sub.mak

-include $(OBJ_DIR)/depend.mak

include $(ROOT)/mak/misc.mak

tests: $(ERROR)

poststarttests: $(ERROR)
