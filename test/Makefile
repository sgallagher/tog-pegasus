ROOT = ..

include $(ROOT)/mak/config.mak
include $(ROOT)/test/config.mak

all: clean tests

clean:
	$(MAKE) --directory=TestProviders clean
	$(MAKE) --directory=TestProviders all
	$(MAKE) --directory=wetest clean

tests:
	$(MAKE) --directory=wetest tests

# DO NOT DELETE
