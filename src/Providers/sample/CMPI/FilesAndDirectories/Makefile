ROOT = $(PEGASUS_ROOT)

DIR = Providers/sample/CMPI/FilesAndDirectories

include $(ROOT)/mak/config.mak

ifeq ($(PEGASUS_PLATFORM),ZOS_ZSERIES_IBM)
    CXX = cc
    FLAGS += -W"c,SSCOMM"
endif

LOCAL_DEFINES = -DSIMULATED -DPEGASUS_USE_EXPERIMENTAL_INTERFACES

LIBRARY = FilesAndDirectories

SOURCES = \
   CWS_PlainFile.c \
   CWS_FileUtils.c \
   CWS_Directory.c \
   CWS_DirectoryContainsFile.c \
   cwsutil.c

include $(ROOT)/mak/library.mak

tests:
