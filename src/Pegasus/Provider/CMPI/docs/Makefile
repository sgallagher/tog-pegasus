ROOT = ../../../../../

DIR = Pegasus/Provider/CMPI/docs

LOCAL_DEFINES = -DPEGASUS_USE_EXPERIMENTAL_INTERFACES -DDOC_ONLY

include $(ROOT)/mak/config.mak

all:

doc: ../cmpimacs.h
	$(COPY) ../cmpimacs.h macs.h
	$(CXX) $(LOCAL_DEFINES) -I../ -E -C macs.h > cmpimacs.h

clean:
	$(RM) macs.h
	$(RM) cmpimacs.h
