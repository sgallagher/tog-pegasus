ifeq ($(OS_TYPE),windows)
include $(ROOT)/mak/depend-windows.mak
else
 ifeq ($(OS_TYPE),vms)
 include $(ROOT)/mak/depend-vms.mak
 else
 include $(ROOT)/mak/depend-unix.mak
 endif
endif
