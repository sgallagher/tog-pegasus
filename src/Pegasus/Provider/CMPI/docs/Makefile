ROOT = ../../../../../

DIR = Pegasus/Provider/CMPI/docs

include $(ROOT)/mak/config.mak

all:

doc: ../cmpimacs.h
	$(COPY) ../cmpimacs.h macs.h
	$(CXX) -I../ -DDOC_ONLY -E -C macs.h > cmpimacs.h

clean:
	$(RM) macs.h
	$(RM) cmpimacs.h
