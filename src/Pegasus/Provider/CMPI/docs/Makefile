ROOT = ../../../../../

DIR = Pegasus/Provider/CMPI/docs

include $(ROOT)/mak/config.mak

all:

doc: ../cmpimacs.h
	$(COPY) ../cmpimacs.h ./macs.h
	$(CPP) -I../ -DDOC_ONLY -P -C macs.h > cmpimacs.h
	$(RM) macs.h
clean:
	$(RM) *.h
