ROOT = $(PEGASUS_ROOT)

include $(ROOT)/mak/config.mak
include $(ROOT)/mak/commands.mak


ifndef PEGASUS_HTML_DIR
   PEGASUS_HTML_DIR = $(PEGASUS_HOME)/html
endif

FORCE:

buildDoc_ContinueOnError:
	@if $(DOCXX) --version > /dev/null 2>&1; \
then $(MAKE) --directory=$(ROOT)/doc/SDK -f Makefile PEGASUS_HTML_DIR=$(PEGASUS_HTML_DIR) buildDoc;\
else $(ECHO) "Warning: API documentation not generated.  $(DOCXX) command not found."; fi

buildDoc:
	@$(RMDIRHIER) $(PEGASUS_HTML_DIR)
	@$(MKDIRHIER) $(PEGASUS_HTML_DIR)
	@$(COPY) index.html $(PEGASUS_HTML_DIR)/index.html
	@$(MAKE) --directory=$(ROOT)/doc/SDK -f Makefile PEGASUS_HTML_DIR=$(PEGASUS_HTML_DIR) buildDefaultC++Doc
	@$(MAKE) --directory=$(ROOT)/doc/SDK -f Makefile PEGASUS_HTML_DIR=$(PEGASUS_HTML_DIR) buildCMPIDoc

buildDefaultC++Doc:
	@$(RMDIRHIER) $(PEGASUS_HTML_DIR)/DefaultC++
	@$(MKDIRHIER) $(PEGASUS_HTML_DIR)/DefaultC++
	@$(DOCXX) --config doc++.conf --dir $(PEGASUS_HTML_DIR)/DefaultC++ --verbose -H DefaultC++/ClientInterface.dxx
	@sed -e "s/Hierarchy of Classes/Hierarchy of CIM Client Classes/" $(PEGASUS_HTML_DIR)/DefaultC++/HIER.html >  $(PEGASUS_HTML_DIR)/DefaultC++/ClientClassHier.html
	@$(DOCXX) --config doc++.conf --dir $(PEGASUS_HTML_DIR)/DefaultC++ --verbose -H DefaultC++/ProviderInterface.dxx
	@sed -e "s/Hierarchy of Classes/Hierarchy of CIM Provider Classes/" $(PEGASUS_HTML_DIR)/DefaultC++/HIER.html >  $(PEGASUS_HTML_DIR)/DefaultC++/ProviderClassHier.html
	@$(DOCXX) --config doc++.conf --dir $(PEGASUS_HTML_DIR)/DefaultC++ --verbose -H DefaultC++/IndicationConsumerInterface.dxx
	@sed -e "s/Hierarchy of Classes/Hierarchy of CIM Indication Consumer Classes/" $(PEGASUS_HTML_DIR)/DefaultC++/HIER.html >  $(PEGASUS_HTML_DIR)/DefaultC++/IndicationConsumerClassHier.html
	@$(DOCXX) --config doc++.conf --dir $(PEGASUS_HTML_DIR)/DefaultC++ --verbose -H DefaultC++/CommonInterface.dxx
	@sed -e "s/Hierarchy of Classes/Hierarchy of CIM Common Classes/" $(PEGASUS_HTML_DIR)/DefaultC++/HIER.html >  $(PEGASUS_HTML_DIR)/DefaultC++/CommonClassHier.html
	@$(COPY) DefaultC++/ClassHier.html $(PEGASUS_HTML_DIR)/DefaultC++/ClassHier.html


buildCMPIDoc:
	@$(RMDIRHIER) $(PEGASUS_HTML_DIR)/CMPI
	@$(MKDIRHIER) $(PEGASUS_HTML_DIR)/CMPI
	@$(MAKE) --directory=$(PEGASUS_ROOT)/src/Pegasus/Provider/CMPI/docs -f Makefile doc
	$(DOCXX) --config doc++.conf --dir $(PEGASUS_HTML_DIR)/CMPI -H -v CMPI/Interface.dxx
	@sed -e "s/Hierarchy of Classes/Hierarchy of CMPI Classes/" $(PEGASUS_HTML_DIR)/CMPI/HIER.html >  $(PEGASUS_HTML_DIR)/CMPI/Interface.html
	@$(COPY) CMPI/ClassHier.html $(PEGASUS_HTML_DIR)/CMPI/ClassHier.html

clean:
	@$(RMDIRHIER) $(PEGASUS_HTML_DIR)
	@$(MAKE) --directory=$(PEGASUS_ROOT)/src/Pegasus/Provider/CMPI/docs -f Makefile clean
depend:

tests:
