ROOT = ../../../..
DIR = Pegasus/DynListener/Service
include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY -DPEGASUS_CONSUMER_INTERNAL

PROGRAM = cimlistener

LIBRARIES= \
    pegcommon \
    pegexportserver \
    pegdynlistener \
    pegprovider \
    pegservice

SOURCES = cimlistener.cpp

include $(ROOT)/mak/program.mak

tests:

poststarttests:

run:

