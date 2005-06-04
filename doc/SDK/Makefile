#//%2005////////////////////////////////////////////////////////////////////////
#//
#// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
#// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
#// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
#// IBM Corp.; EMC Corporation, The Open Group.
#// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
#// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
#// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
#// EMC Corporation; VERITAS Software Corporation; The Open Group.
#//
#// Permission is hereby granted, free of charge, to any person obtaining a copy
#// of this software and associated documentation files (the "Software"), to
#// deal in the Software without restriction, including without limitation the
#// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
#// sell copies of the Software, and to permit persons to whom the Software is
#// furnished to do so, subject to the following conditions:
#// 
#// THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN
#// ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
#// "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
#// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
#// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
#// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
#// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
#// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#//
#//==============================================================================
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

2.6Deferred:
	@$(MAKE) --directory=$(ROOT)/doc/SDK -f Makefile PEGASUS_HTML_DIR=$(PEGASUS_HTML_DIR) buildCMPIC++Doc

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

buildCMPIC++Doc:
	@$(RMDIRHIER) $(PEGASUS_HTML_DIR)/CMPIC++
	@$(MKDIRHIER) $(PEGASUS_HTML_DIR)/CMPIC++
	@$(MAKE) --directory=$(PEGASUS_ROOT)/src/Pegasus/Provider/CMPI/docs -f Makefile doc
	$(DOCXX) --config doc++.conf --dir $(PEGASUS_HTML_DIR)/CMPIC++ -H -v CMPIC++/Interface.dxx
	@sed -e "s/Hierarchy of Classes/Hierarchy of CMPI Classes/" $(PEGASUS_HTML_DIR)/CMPIC++/HIER.html >  $(PEGASUS_HTML_DIR)/CMPIC++/Interface.html
	@$(COPY) CMPIC++/ClassHier.html $(PEGASUS_HTML_DIR)/CMPIC++/ClassHier.html

clean:
	@$(RMDIRHIER) $(PEGASUS_HTML_DIR)
	@$(MAKE) --directory=$(PEGASUS_ROOT)/src/Pegasus/Provider/CMPI/docs -f Makefile clean
depend:

tests:
