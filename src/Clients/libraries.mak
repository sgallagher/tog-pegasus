
LIBRARIES = \
    pegcommon \
    pegconfig \
    pegrepository \
	pegCLIClientLib \
    pegclient

ifneq ($(PEGASUS_PLATFORM),ZOS_ZSERIES_IBM)
  ifneq ($(OS),HPUX)
    LIBRARIES += slp
  endif
endif
