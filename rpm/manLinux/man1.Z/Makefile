ROOT = ../../..

include $(ROOT)/mak/config.mak
clean:

install:

ifeq ($(OS_TYPE),unix)
	$(MKDIRHIER) $(DEST_MAN_DIR)/man1
	$(COPY) cimmof.1  $(DEST_MAN_DIR)/man1/cimmof.1
	$(COPY) cimprovider.1 $(DEST_MAN_DIR)/man1/cimprovider.1
	$(COPY) osinfo.1 $(DEST_MAN_DIR)/man1/osinfo.1
	$(COPY) wbemexec.1 $(DEST_MAN_DIR)/man1/wbemexec.1
endif

prestarttests:

poststarttests:

tests:
