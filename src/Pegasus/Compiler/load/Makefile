ROOT = ../../../..
include $(ROOT)/mak/config.mak

MOF_PATH = $(ROOT)/Schemas/CIM25


all:

tests:
	@ echo +++++ Populating CIM repository - this will take a minute
	@ $(RMDIRHIER) $(REPOSITORY_ROOT)
	@ cimmof -I$(MOF_PATH) -nroot $(MOF_PATH)/CIM_Core25.mof
	@ echo **** CIM_Core.mof loaded
	@ cimmof -I$(MOF_PATH) $(MOF_PATH)/CIM_Schema25.mof
	@ echo *** CIM_Schema25.mof loaded

clean:

depend:

strip-headers:

prepend-headers:

sub:
