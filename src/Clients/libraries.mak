
LIBRARIES = \
    pegcommon \
    pegconfig \
    pegrepository \
    pegclient

ifneq ($(PEGASUS_PLATFORM),ZOS_ZSERIES_IBM)
LIBRARIES += slp	
endif
