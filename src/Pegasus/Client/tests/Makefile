ROOT = ../../../../

include $(ROOT)/mak/config.mak

DIRS = Client GetInstance EnumInstNames InvokeMethod InvokeMethod2 EnumInstances DeleteNamespace

DIRS_SLP = \
    slp

ifdef PEGASUS_ENABLE_SLP
  DIRS += $(DIRS_SLP)
endif

include $(ROOT)/mak/recurse.mak
