ROOT = $(PEGASUS_ROOT)

ifeq ($(PEGASUS_PLATFORM),LINUX_IX86_GNU)
   DOCXX = doc++
endif

ifeq ($(PEGASUS_PLATFORM),LINUX_IA64_GNU)
   DOCXX = doc++
endif

ifeq ($(PEGASUS_PLATFORM),HPUX_PARISC_ACC)
   DOCXX = docxx
endif

ifeq ($(PEGASUS_PLATFORM),HPUX_IA64_ACC)
   DOCXX = docxx
endif

DIR = DevManual
include $(ROOT)/mak/config.mak


ifdef HTML_DESTPATH
INTERFACE_OUTPUT = $(HTML_DESTPATH)
else
INTERFACE_OUTPUT = $(PEGASUS_HOME)/html
endif

CLIENT_INTERFACE_HEAD = ClientInterface.dxx
PROVIDER_INTERFACE_HEAD = ProviderInterface.dxx
CONSUMER_INTERFACE_HEAD = IndicationConsumerInterface.dxx
COMMON_INTERFACE_HEAD = CommonInterface.dxx
CMPI_INTERFACE_HEAD = CMPIInterface.dxx
CMPI_C_INTERFACE_HEAD = CMPI_C_Interface.dxx

buildDoc:
	@$(RMDIRHIER) $(INTERFACE_OUTPUT)
	@$(MKDIRHIER) $(INTERFACE_OUTPUT)
	@$(DOCXX) --config doc++.conf --dir $(INTERFACE_OUTPUT) --verbose -H $(CLIENT_INTERFACE_HEAD)
	@sed -e "s/Hierarchy of Classes/Hierarchy of CIM Client Classes/" $(INTERFACE_OUTPUT)/HIER.html >  $(INTERFACE_OUTPUT)/ClientClassHier.html
	@$(DOCXX) --config doc++.conf --dir $(INTERFACE_OUTPUT) --verbose -H $(PROVIDER_INTERFACE_HEAD)
	@sed -e "s/Hierarchy of Classes/Hierarchy of CIM Provider Classes/" $(INTERFACE_OUTPUT)/HIER.html >  $(INTERFACE_OUTPUT)/ProviderClassHier.html
	@$(DOCXX) --config doc++.conf --dir $(INTERFACE_OUTPUT) --verbose -H $(CONSUMER_INTERFACE_HEAD)
	@sed -e "s/Hierarchy of Classes/Hierarchy of CIM Indication Consumer Classes/" $(INTERFACE_OUTPUT)/HIER.html >  $(INTERFACE_OUTPUT)/IndicationConsumerClassHier.html
	@$(DOCXX) --config doc++.conf --dir $(INTERFACE_OUTPUT) --verbose -H $(COMMON_INTERFACE_HEAD)
	@sed -e "s/Hierarchy of Classes/Hierarchy of CIM Comon Classes/" $(INTERFACE_OUTPUT)/HIER.html >  $(INTERFACE_OUTPUT)/CommonClassHier.html


	@$(DOCXX) --config doc++.conf --dir $(INTERFACE_OUTPUT) --verbose -H $(CMPI_INTERFACE_HEAD)
	@sed -e "s/Hierarchy of Classes/Hierarchy of CMPI Classes/" $(INTERFACE_OUTPUT)/HIER.html >  $(INTERFACE_OUTPUT)/CMPIClassHier.html

	@$(MAKE) --directory=$(PEGASUS_ROOT)/src/Pegasus/Provider/CMPI/docs -f Makefile doc
	@$(DOCXX) --config doc++.conf --dir $(INTERFACE_OUTPUT) --verbose -H $(CMPI_C_INTERFACE_HEAD)
	@sed -e "s/Hierarchy of Classes/Hierarchy of CMPI C macros, types and definitions/" $(INTERFACE_OUTPUT)/HIER.html >  $(INTERFACE_OUTPUT)/CMPI_C_Hier.html

	@$(DOCXX) --config doc++.conf --dir $(INTERFACE_OUTPUT) --verbose -H Interface.dxx
	@$(COPY) CIMClassHier.html $(INTERFACE_OUTPUT)/CIMClassHier.html

clean:
	$(RMDIRHIER) $(INTERFACE_OUTPUT)
	@$(MAKE) --directory=$(PEGASUS_ROOT)/src/Pegasus/Provider/CMPI/docs -f Makefile clean
depend:

tests:
