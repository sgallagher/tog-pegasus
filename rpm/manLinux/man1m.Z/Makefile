ROOT = ../../..

include $(ROOT)/mak/config.mak
clean:

install:

ifeq ($(OS_TYPE),unix)
	$(MKDIRHIER) $(DEST_MAN_DIR)/man8
	$(COPY) cimauth.1m  $(DEST_MAN_DIR)/man8/cimauth.8
	$(COPY) cimconfig.1m $(DEST_MAN_DIR)/man8/cimconfig.8 
	$(COPY) cimprovider.1m $(DEST_MAN_DIR)/man8/cimprovider.8
	$(COPY) cimserver.1m $(DEST_MAN_DIR)/man8/cimserver.8
endif

prestarttests:

poststarttests:

tests:
