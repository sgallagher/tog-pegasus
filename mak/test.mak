ifeq ($(PEGASUS_PLATFORM), WIN32_IX86_MSVC)
    STRIPCRS = stripcrs $(RESULTFILE) $(MASTERRESULTFILE)
    DIFF = mu compare
    REDIRECTERROR = 2>&1
else
    STRIPCRS =
    DIFF = diff
    REDIRECTERROR = 2>&1
endif

PEGASUS_XML_ORDERING_DEFECT_ENCOUNTERED=on

ifdef DIFF
    COMPARERESULTS = @$(DIFF) $(MASTERRESULTFILE) $(RESULTFILE)
    COMPARERESULTS_DS = @$(call DIFFSORT,$(MASTERRESULTFILE),$(RESULTFILE))
else
    COMPARERESULTS = @$(ECHO) "Results not compared."
    COMPARERESULTS_DS = @$(ECHO) "Results not compared."
endif

ifdef PEGASUS_TMP
    TMP_DIR = $(subst \,/,$(PEGASUS_TMP))
else
    TMP_DIR = .
endif

default:

#l10n
messages: $(ERROR)
