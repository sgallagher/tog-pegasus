
LIBRARIES = \
    pegcommon \
    pegconfig \
    pegrepository \
	pegCLIClientLib \
    pegclient

ifneq ($(PEGASUS_PLATFORM),ZOS_ZSERIES_IBM)
ifneq ($(PEGASUS_PLATFORM),HPUX_PARISC_ACC)
    LIBRARIES += slp
endif
endif
