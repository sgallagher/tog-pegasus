ROOT = ../../../..
include $(ROOT)/mak/config.mak

MOF_PATH = $(ROOT)/Schemas/CIM25


all:
	$(RMDIRHIER) $(REPOSITORY_ROOT)/repository
	cimmof -I$(MOF_PATH) -nroot $(MOF_PATH)/CIM_Core25.mof
	cimmof -I$(MOF_PATH) $(MOF_PATH)/CIM_Schema25.mof

clean:
tests:
depend:
