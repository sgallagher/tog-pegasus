ROOT = $(PEGASUS_ROOT)

DIR = Providers/sample/CMPI/FilesAndDirectories

include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DSIMULATED

LIBRARY = FilesAndDirectories
	
SOURCES = \
   CWS_PlainFile.c \
   CWS_FileUtils.c \
   CWS_Directory.c \
   CWS_DirectoryContainsFile.c \
   cwsutil.c

include $(ROOT)/mak/library.mak

tests:
