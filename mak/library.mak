INCLUDES = -I$(ROOT)/src $(EXTRA_INCLUDES)

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
else
include $(ROOT)/mak/library-unix.mak
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

# include $(ROOT)/mak/headers.mak

include $(ROOT)/mak/depend.mak

include $(ROOT)/mak/build.mak

-include $(OBJ_DIR)/depend.mak

include $(ROOT)/mak/misc.mak

tests: $(ERROR)
