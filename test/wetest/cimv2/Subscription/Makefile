ROOT = ../../../..
include $(ROOT)/mak/config.mak
include $(ROOT)/test/config.mak

XMLSCRIPTS = \
    CreateModule01 CreateProvider01 CreateCapability01 \
    CreateModule02 CreateProvider02 CreateCapability02 \
    CreateFilter01 CreateFilter02 CreateFilter03 CreateFilter04 \
    CreateFilter05 CreateFilter06 \
    CreateHandler01 CreateHandler02 CreateHandler03 \
    CreateSubscription01 CreateSubscription02 CreateSubscription03 \
    CreateSubscription04 CreateSubscription05 CreateSubscription06 \
    CreateSubscription07 CreateSubscription08 CreateSubscription09 \
    EnumerateFilters01 \
    EnumerateHandlers01 \
    EnumerateSubscriptions01 \
    ModifyFilter01 \
    ModifyHandler01 \
    ModifySubscription01 ModifySubscription02 \
    ModifySubscription03 ModifySubscription04 ModifySubscription05 \
    ModifySubscription07 \
    CreateProvider03 CreateCapability03 \
    ModifyCapability01 ModifyCapability02 \
    DeleteCapability03 DeleteProvider03 \
    DeleteFilter07 DeleteHandler04 \
    DeleteSubscription01 DeleteSubscription03 \
    DeleteSubscription04 DeleteSubscription05 DeleteSubscription06 \
    DeleteSubscription07 DeleteSubscription08 DeleteSubscription09 \
    DeleteFilter02 DeleteFilter03 DeleteFilter04 \
    DeleteFilter05 DeleteFilter06 \
    DeleteHandler01 DeleteHandler02 DeleteHandler03 \
    CreateHandler09 CreateSubscription14 EnumerateSubscriptions03 \
    DeleteHandler09 EnumerateSubscriptions04 \
    DeleteFilter01 \
    CreateFilter07 CreateFilter08 CreateFilter09 CreateFilter10 \
    CreateFilter11 \
    CreateHandler04 CreateHandler05 CreateHandler06 CreateHandler07 \
    CreateHandler08 \
    CreateHandler10 CreateHandler11 CreateHandler12 CreateHandler13 \
    CreateHandler14 \
    CreateSubscription10 CreateSubscription11 CreateSubscription12 \
    CreateSubscription13 \
    CreateSubscription15 CreateSubscription16 CreateSubscription17 \
    CreateSubscription18 CreateSubscription19 CreateSubscription20 \
    CreateSubscription21 CreateSubscription22 CreateSubscription23 \
    CreateSubscription24 CreateSubscription25 \
    DeleteCapability02 DeleteProvider02 DeleteModule02 \
    DeleteCapability01 DeleteProvider01 DeleteModule01
#   EnumerateFilters01 EnumerateFilters02 \
#   EnumerateHandlers01 EnumerateHandlers02 \
#   EnumerateSubscriptions01 EnumerateSubscriptions02 \
#   GetFilter01 GetFilter02 GetFilter03 GetFilter04 GetFilter05 GetFilter06 \
#   GetHandler01 GetHandler02 GetHandler03 \
#   GetSubscription01 GetSubscription02 GetSubscription03 \
#   GetSubscription04 GetSubscription05 GetSubscription06 \
#   GetSubscription07 GetSubscription08 GetSubscription09 \
#   CreateSubscription26 ModifySubscription06 \
#   CreateFilter13 CreateSubscription15 \
#   DeleteSubscription15 DeleteFilter13 \
#   CreateFilter11 CreateFilter12 \

testcleanup:
	$(MAKE) -i removeproviders
	$(MAKE) -i removeclasses
	$(MAKE) clean

removeproviders:
	cimprovider -r -m ProcessIndicationProviderModule
	cimprovider -r -m AlertIndicationProviderModule
	cimprovider -r -m PG_ProviderModule

removeclasses:
	@wbemexec $(WBEMEXECOPTIONS) DeleteHandler04.xml
	@wbemexec $(WBEMEXECOPTIONS) DeleteSubscription01.xml
	@wbemexec $(WBEMEXECOPTIONS) DeleteSubscription03.xml
	@wbemexec $(WBEMEXECOPTIONS) DeleteSubscription04.xml
	@wbemexec $(WBEMEXECOPTIONS) DeleteSubscription05.xml
	@wbemexec $(WBEMEXECOPTIONS) DeleteSubscription06.xml
	@wbemexec $(WBEMEXECOPTIONS) DeleteSubscription07.xml
	@wbemexec $(WBEMEXECOPTIONS) DeleteSubscription08.xml
	@wbemexec $(WBEMEXECOPTIONS) DeleteSubscription09.xml
	@wbemexec $(WBEMEXECOPTIONS) DeleteFilter01.xml
	@wbemexec $(WBEMEXECOPTIONS) DeleteFilter02.xml
	@wbemexec $(WBEMEXECOPTIONS) DeleteFilter03.xml
	@wbemexec $(WBEMEXECOPTIONS) DeleteFilter04.xml
	@wbemexec $(WBEMEXECOPTIONS) DeleteFilter05.xml
	@wbemexec $(WBEMEXECOPTIONS) DeleteFilter06.xml
	@wbemexec $(WBEMEXECOPTIONS) DeleteFilter07.xml
	@wbemexec $(WBEMEXECOPTIONS) DeleteHandler01.xml
	@wbemexec $(WBEMEXECOPTIONS) DeleteHandler02.xml
	@wbemexec $(WBEMEXECOPTIONS) DeleteHandler03.xml
	@wbemexec $(WBEMEXECOPTIONS) DeleteHandler04.xml
	@wbemexec $(WBEMEXECOPTIONS) DeleteHandler09.xml

include $(ROOT)/test/configend.mak
