ROOT = ../../..

DIR = CommExample/Comm

include $(ROOT)/mak/config.mak

EXTRA_INCLUDES = $(ACE_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_COMM_INTERNAL 

LIBRARIES = $(ACE_LIB)

LIBRARY = comm

SOURCES = \
    ACEComm.cpp \
    Comm.cpp 

include $(ROOT)/mak/library.mak
