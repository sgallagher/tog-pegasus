
all:
	$(MAKE) -f jmpi.mak
	$(MAKE) -C org/pegasus/jmpi
	$(MAKE) -C org/pegasus/jmpi/tests/JMPI_TestPropertyTypes
	$(MAKE) -C org/pegasus/jmpi/tests/ProcessIndication
	$(MAKE) -C org/pegasus/jmpi/tests/Client

clean:
	$(MAKE) -f jmpi.mak clean
	$(MAKE) -C org/pegasus/jmpi clean
	$(MAKE) -C org/pegasus/jmpi/tests/ProcessIndication clean
	$(MAKE) -C org/pegasus/jmpi/tests/JMPI_TestPropertyTypes clean
	$(MAKE) -C org/pegasus/jmpi/tests/Client clean

messages:
