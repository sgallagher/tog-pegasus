ROOT = ../../..
include $(ROOT)/mak/config.mak

LIBRARY = FilesAndDirectories

SOURCE_DIR = Providers/CMPI/FilesAndDirectories

# Remove this flag if you do not want to run the CMPI provider in simulated mode.
DEFINES += -DSIMULATED

LIBRARIES = \
  pegcommon \
  pegprovider

SOURCES = \
   CWS_PlainFile.c \
   CWS_FileUtils.c \
   CWS_Directory.c \
   CWS_DirectoryContainsFile.c \
   cwsutil.c

include $(ROOT)/mak/library.mak

tests:
