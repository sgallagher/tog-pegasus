#//%LICENSE////////////////////////////////////////////////////////////////
#//
#// Licensed to The Open Group (TOG) under one or more contributor license
#// agreements.  Refer to the OpenPegasusNOTICE.txt file distributed with
#// this work for additional information regarding copyright ownership.
#// Each contributor licenses this file to you under the OpenPegasus Open
#// Source License; you may not use this file except in compliance with the
#// License.
#//
#// Permission is hereby granted, free of charge, to any person obtaining a
#// copy of this software and associated documentation files (the "Software"),
#// to deal in the Software without restriction, including without limitation
#// the rights to use, copy, modify, merge, publish, distribute, sublicense,
#// and/or sell copies of the Software, and to permit persons to whom the
#// Software is furnished to do so, subject to the following conditions:
#//
#// The above copyright notice and this permission notice shall be included
#// in all copies or substantial portions of the Software.
#//
#// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
#// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
#// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
#// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
#// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
#// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
#// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#//
#//////////////////////////////////////////////////////////////////////////
##
##  cimcli tests are executed complete from Makefiles.   Today they are
##  divided between this Makefile and a Makefile in
##  src/Providers/TestProviders/CLITestProvider/tests
##  All cimcli tests will be integrated into this Makefile in the future.
##
ROOT = ../../../..

include $(ROOT)/mak/config.mak

DIR = Clients/cimcli/tests
SRC_DIR = $(ROOT)/src/$(DIR)

CIMCLI = @cimcli

QUOTE=\"

ifeq ($(OS_TYPE),vms)
QUOTE=""
endif

include $(ROOT)/mak/config.mak
include $(ROOT)/mak/configschema.mak

ifdef PEGASUS_TMP
    TMP_DIR = $(subst \,/,$(PEGASUS_TMP))
else
    TMP_DIR = $(SRC_DIR)
endif

RESULTFILE = $(TMP_DIR)/result
MASTERRESULTFILE = $(SRC_DIR)/result.master

PROVIDERNS=test/TestProvider

PROVNSOPT = -n $(PROVIDERNS)


# single definition for the class name and namespace used for the
# embedded instance tests.
EMBED_CLASS_DEF = -n $(PROVIDERNS) Test_CLITestEmbeddedClass

depend:

sub:

misc:

tests:

messages:

general:


poststarttests::
	@$(RM) $(RESULTFILE)
	@$(ECHO) > ${RESULTFILE} 
	$(MAKE) createEmbeddedInstanceTests
	$(MAKE) invokeEmbeddedInstMethodTests
	$(MAKE) EmbeddedInstanceErrorTests
	@$(ECHO) Sort and compare result file
	@ $(call DIFFSORT,$(RESULTFILE),$(MASTERRESULTFILE))
	@$(ECHO) +++++ Remove Result files since test successful
	-@$(RM) $(RESULTFILE)
	@$(ECHO) +++++ passed tests

### Clean up an created/modified/or deleted instances that may exist
### in the server (i.e. the provider) and remove the result files.
### Note that the normal path also removes result files so that the
### only reason for this is to redo a failed test before the provider is
### reinitialized (server restarted or provider reloaded)
clean: 
	-$(MAKE) d1

## Create instance tests
createEmbeddedInstanceTests: ct1 ct2 ct3 ct4 ct5 ct6 ct7 ct8 ct9 ct10 ct11

##############################################################################
# simple create without embedded property
c1:
	$(CIMCLI) ci $(EMBED_CLASS_DEF) Id=111 \
	    comment="test with no embedded instance"   >> $(RESULTFILE)

t1:
	$(CIMCLI) ti $(EMBED_CLASS_DEF) Id=111	\
	    comment="test with no embedded instance"   >> $(RESULTFILE)

d1:
	$(CIMCLI) di $(EMBED_CLASS_DEF) Id=111 \
		>> $(RESULTFILE)
g1:
	$(CIMCLI) gi $(EMBED_CLASS_DEF) Id=111 \
		>> $(RESULTFILE)

ct1: c1 g1 t1 d1
	@$(ECHO) +++++ passed test ct1

##############################################################################
## create instance operation with a single embedded property
c2:
	$(CIMCLI) ci $(EMBED_CLASS_DEF) id=201 \
	    comment="test with embedded instance" \
	    embeddedInst{Test_CLITestEmbedded1 Id=202 \
		    comment="Embedded Instance comment property" } \
		>> $(RESULTFILE)

t2:
	$(CIMCLI) ti $(EMBED_CLASS_DEF) id=201 \
	    comment="test with embedded instance" \
	    embeddedInst{Test_CLITestEmbedded1 Id=202 \
		    comment="Embedded Instance comment property" } \
		>> $(RESULTFILE)
d2:
	$(CIMCLI) di $(EMBED_CLASS_DEF) Id=201 \
		>> $(RESULTFILE)
g2:
	$(CIMCLI) gi $(EMBED_CLASS_DEF) Id=201 \
		>> $(RESULTFILE)

## Build an instance with embedded instance, test it and delete it.  Combines
## The previous 3 tests into a single target.
ct2: c2 g2 t2 d2
	@$(ECHO) +++++ passed test ct2

############################################################################
## create instance operation with multiple embedded properties
c3:
	$(CIMCLI) ci $(EMBED_CLASS_DEF) id=301 \
	    comment="test with multiple embedded instances" \
	    embeddedInst{Test_CLITestEmbedded1 Id=302 \
		comment="First property with embedded instance" } \
	    embeddedInst2{Test_CLITestEmbedded2 Id=303 name=fred \
		comment="Second property with embedded instance" } \
		>> $(RESULTFILE)

t3:
	$(CIMCLI) ti $(EMBED_CLASS_DEF) id=301 \
	    comment="test with multiple embedded instances" \
	    embeddedInst{Test_CLITestEmbedded1 Id=302 \
		comment="First property with embedded instance" } \
	    embeddedInst2{Test_CLITestEmbedded2 Id=303 name=fred \
		comment="Second property with embedded instance" } \
		>> $(RESULTFILE)

d3:
	$(CIMCLI) di $(EMBED_CLASS_DEF) Id=301 \
		>> $(RESULTFILE)

g3:
	$(CIMCLI) gi $(EMBED_CLASS_DEF) Id=301 \
		>> $(RESULTFILE)

## Build an instance with embedded instance, test it and delete it.  Combines
## The previous 3 tests into a single target.
ct3: c3 g3 t3 d3
	@$(ECHO) +++++ passed test ct3
	
############################################################################
## create instance operation with embedded object
c4:
	$(CIMCLI) ci $(EMBED_CLASS_DEF) id=401 \
	    comment="test with single embedded object" \
	    embeddedObj{Test_CLITestEmbedded1 Id=402 \
		    comment="Property is embedded object" } \
		>> $(RESULTFILE)

t4:
	$(CIMCLI) ti $(EMBED_CLASS_DEF) id=401 \
	    comment="test with single embedded object" \
	    embeddedObj{Test_CLITestEmbedded1 Id=402 \
		    comment="Property is embedded object" } \
		>> $(RESULTFILE)

d4:
	$(CIMCLI) di $(EMBED_CLASS_DEF) Id=401 \
		>> $(RESULTFILE)

g4:
	$(CIMCLI) gi $(EMBED_CLASS_DEF) Id=401 \
		>> $(RESULTFILE)

## Build an instance with embedded object, test it and delete it.  Combines
## The previous 3 tests into a single target.
ct4: c4 g4 t4 d4
	@$(ECHO) +++++ passed test ct4
	
############################################################################
## create instance operation with embedded instance array property
c5:
	$(CIMCLI) ci $(EMBED_CLASS_DEF) id=501 \
	    comment="test with array of embedded instances" \
	    embeddedInstArray{Test_CLITestEmbedded1 Id=502 \
		    comment="First embedded Instance" } \
	    embeddedInstArray{Test_CLITestEmbedded1 Id=503 \
		    comment="Second embedded Instance" } >> $(RESULTFILE)

t5:
	$(CIMCLI) ti $(EMBED_CLASS_DEF) id=501 \
	    comment="test with array of embedded instances" \
	    embeddedInstArray{Test_CLITestEmbedded1 Id=502 \
		    comment="First embedded Instance" } \
	    embeddedInstArray{Test_CLITestEmbedded1 Id=503 \
		    comment="Second embedded Instance" } >> $(RESULTFILE) \
		>> $(RESULTFILE)

d5:
	$(CIMCLI) di $(EMBED_CLASS_DEF) Id=501 \
		>> $(RESULTFILE)

g5:
	$(CIMCLI) gi $(EMBED_CLASS_DEF) Id=501 \
		>> $(RESULTFILE)

## Build an instance with embedded instance, test it and delete it.  Combines
## The previous 3 tests into a single target.
ct5: c5 g5 t5 d5
	@$(ECHO) +++++ passed test ct5

############################################################################
## create instance operation with embedded instance array property and with
## the alternate form of array definition (}'{. The test uses the complete
## copy form so that this also tests if they generate the same code.
c6:
	$(CIMCLI) ci $(EMBED_CLASS_DEF) id=601 \
	    comment="test with array of embedded instances" \
	    embeddedInstArray{Test_CLITestEmbedded1 Id=602 \
		              comment="First embedded Instance" }.{  \
	                  Id=603 \
		              comment="Second embedded Instance" } \
		>> $(RESULTFILE)

t6:
	$(CIMCLI) ti $(EMBED_CLASS_DEF) id=601 \
	    comment="test with array of embedded instances" \
	    embeddedInstArray{Test_CLITestEmbedded1 Id=602 \
		    comment="First embedded Instance" } \
	    embeddedInstArray{Test_CLITestEmbedded1 Id=603 \
		    comment="Second embedded Instance" } \
		>> $(RESULTFILE)

g6:
	$(CIMCLI) gi $(EMBED_CLASS_DEF) Id=601 \
		>> $(RESULTFILE)

d6:
	$(CIMCLI) di $(EMBED_CLASS_DEF) Id=601 \
		>> $(RESULTFILE)

## Build an instance with embedded instance, test it and delete it.  Combines
## The previous 3 tests into a single target.
ct6: c6 g6 t6 d6
	@$(ECHO) +++++ passed test ct6

#############################################################################
## Create a recursive embedded instance. This is an instance of embedd3 
## within embedded 2 which is embedded within EmbeddedClass
c7:
	$(CIMCLI) ci $(EMBED_CLASS_DEF) id=701 \
	    comment="test with recursive embedded instance" \
	    embeddedInst{Test_CLITestEmbedded2 Id=702 \
		comment="Property with embedded instance" \
		    recursiveEmbeddedInst{Test_CLITestEmbedded3 Id=702 \
			comment="recursive embedded instance" } \
            } >> $(RESULTFILE)

t7:
	$(CIMCLI) ti $(EMBED_CLASS_DEF) id=701 \
	    comment="test with recursive embedded instance" \
	    embeddedInst{Test_CLITestEmbedded2 Id=702 \
		comment="Property with embedded instance" \
		    recursiveEmbeddedInst{Test_CLITestEmbedded3 Id=702 \
			comment="recursive embedded instance" } \
            } >> $(RESULTFILE)
d7:
	$(CIMCLI) di $(EMBED_CLASS_DEF) Id=701 \
		>> $(RESULTFILE)

g7:
	$(CIMCLI) gi $(EMBED_CLASS_DEF) Id=701 \
		>> $(RESULTFILE)

ct7: c7 g7 t7 d7
	@$(ECHO) +++++ passed test ct7

############################################################################	
## create instance with ={ as token for embedded instance
c8:
	$(CIMCLI) ci $(EMBED_CLASS_DEF) id=801 \
	    comment="test with ={ as embed token" \
	    embeddedInst={Test_CLITestEmbedded2 Id=802 \
		comment="Property with embedded instance" \
		    recursiveEmbeddedInst{Test_CLITestEmbedded3 Id=803 \
			comment="recursive embedded instance" } \
            } >> $(RESULTFILE)

t8:
	$(CIMCLI) ti $(EMBED_CLASS_DEF) id=801 \
	    comment="test with ={ as embed token" \
	    embeddedInst={Test_CLITestEmbedded2 Id=802 \
		comment="Property with embedded instance" \
		    recursiveEmbeddedInst{Test_CLITestEmbedded3 Id=803 \
			comment="recursive embedded instance" } \
	    } >> $(RESULTFILE)

d8:
	$(CIMCLI) di $(EMBED_CLASS_DEF) Id=801 \
		>> $(RESULTFILE)

g8:
	$(CIMCLI) gi $(EMBED_CLASS_DEF) Id=801 \
		>> $(RESULTFILE)

ct8: c8 g8 t8 d8
	@$(ECHO) +++++ passed test ct8

############################################################################	
## create instance with last input NULL. Not required. part of the standard
## existing tests.
c9:
	$(CIMCLI) ci $(EMBED_CLASS_DEF) id=901 \
	    comment= \
	    >> $(RESULTFILE)
t9:
	$(CIMCLI) ti $(EMBED_CLASS_DEF) Id=901 \
	   comment= \
		>> $(RESULTFILE)

d9:
	$(CIMCLI) di $(EMBED_CLASS_DEF) Id=901 \
		>> $(RESULTFILE)

g9:
	$(CIMCLI) gi $(EMBED_CLASS_DEF) Id=901 \
		>> $(RESULTFILE)

ct9: c9 g9 t9 d9
	@$(ECHO) +++++ passed test ct9

############################################################################	
## create instance with recursive embedded instance that itself contains
## an array.
c10:
	$(CIMCLI) ci -n test/TestProvider Test_CLITestEmbeddedClass id=1001 \
	    comment="test with ={ as embed token" \
	    embeddedInst={Test_CLITestEmbedded2 Id=1002 \
		comment="Property with embedded instance" \
		    recursiveEmbeddedInstArray{Test_CLITestEmbedded3 Id=1002 \
			comment="recursive embedded instance" } \
		    recursiveEmbeddedInstArray{Test_CLITestEmbedded3 Id=1003 \
			comment="recursive embedded instance" } \
            } >> $(RESULTFILE)

t10:
	$(CIMCLI) ti -n test/TestProvider Test_CLITestEmbeddedClass id=1001 \
	    comment="test with ={ as embed token" \
	    embeddedInst={Test_CLITestEmbedded2 Id=1002 \
		comment="Property with embedded instance" \
		    recursiveEmbeddedInstArray={Test_CLITestEmbedded3 Id=1002 \
			comment="recursive embedded instance" } \
		    recursiveEmbeddedInstArray={Test_CLITestEmbedded3 Id=1003 \
			comment="recursive embedded instance" } \
            } >> $(RESULTFILE)

g10:
	$(CIMCLI) ti -n test/TestProvider Test_CLITestEmbeddedClass Id=1001 \
		>> $(RESULTFILE)

d10:
	$(CIMCLI) di -n test/TestProvider Test_CLITestEmbeddedClass Id=1001 \
	    >> $(RESULTFILE)

ct10: c10 g10 t10 d10
	@$(ECHO) +++++ passed test ct10
############################################################################	
## create instance with space after embed start marker 

c11:
	$(CIMCLI) ci $(EMBED_CLASS_DEF) id=1101 \
	    comment="test with embedded instance with space after {" \
	    embeddedInst{ Test_CLITestEmbedded1 Id=1102 } \
		>> $(RESULTFILE)

g11:
	$(CIMCLI) gi $(EMBED_CLASS_DEF) Id=1101 \
		>> $(RESULTFILE)

t11:
	$(CIMCLI) ti $(EMBED_CLASS_DEF) id=1101 \
	    comment="test with embedded instance with space after {" \
	    embeddedInst{ Test_CLITestEmbedded1 Id=1102 } \
		>> $(RESULTFILE)

d11:
	$(CIMCLI) di $(EMBED_CLASS_DEF) Id=1101 \
		>> $(RESULTFILE)

ct11: c11 g11 t11 d11
	@$(ECHO) +++++ passed test ct11
##############################################################################
## Embedded instance method tests

invokeEmbeddedInstMethodTests: im1 im2 im3 im4 im5 im6 im7

## control test. im without embedded instance
im1:
	$(CIMCLI) im $(EMBED_CLASS_DEF) \
	    embeddedInstParamMethod firstArg="test string" \
		>> $(RESULTFILE)

## test for multiple entry of scalar argument.  Code added this release.
im1err:
      o  $(CIMCLI) im $(EMBED_CLASS_DEF) \
	    embeddedInstParamMethod firstArg="test simple im" arg1=blah \
		>> $(RESULTFILE)

## im with single embedded instance.  The method embeddedInstParamMethod
## returns exactly what was input.
im2:
	$(CIMCLI) im $(EMBED_CLASS_DEF) \
	    embeddedInstParamMethod firstArg="test single embedded instance parameter" \
	    embedInstArg{Test_CLITestEmbedded1 Id=501 } \
	    lastArg="last parameter" \
		>> $(RESULTFILE)
		
## im with single embedded object.
im3:
	$(CIMCLI) im $(EMBED_CLASS_DEF) \
	    embeddedInstParamMethod firstArg="test single embedded object parameter" \
	    embedObjArg{Test_CLITestEmbedded1 Id=501 } \
	    lastArg="Last Parameter" \
		>> $(RESULTFILE)

## im with array of embedded instances. 
im4:
	$(CIMCLI) im $(EMBED_CLASS_DEF) \
	    embeddedInstParamMethod firstArg="test embedded instance array parameter" \
	    embedInstArrayArg{Test_CLITestEmbedded1 Id=501 } \
	    embedInstArrayArg{Test_CLITestEmbedded1 Id=502 } \
	    lastArg="last parameter" \
		>> $(RESULTFILE)

## im with array of embedded objects.
im5:
	$(CIMCLI) im $(EMBED_CLASS_DEF) \
	    embeddedInstParamMethod firstArg="test embedded object array parameter" \
	    embedObjArrayArg{Test_CLITestEmbedded1 Id=501 } \
	    embedObjArrayArg{Test_CLITestEmbedded1 Id=502 } \
            lastArg="last parameter" \
		>> $(RESULTFILE)

## test parameter creation to allow arrays to be created by
## multiple definitions of a property name.
im6:
	$(CIMCLI) im Test_CLITestProviderClass -n test/TestProvider \
	    InOutParamTest \
	    arraySint32=99,100,110,120 \
	    arrayString=one,two,three \
		>> $(RESULTFILE)

## test combination of ways to create arrays of parameters
im7:
	$(CIMCLI) im Test_CLITestProviderClass -n test/TestProvider \
	    InOutParamTest \
	    arraySint32=99,100,110 arraySint32=120 \
	    arrayString=one,two,three arrayString=four \
		>> $(RESULTFILE)

###############################################################################
## Error tests

EmbeddedInstanceErrorTests:
	@$(ECHO) Following tests generate ERROR messages but return exit code 0.
	make createEmbeddedInstanceErrors
	make invokeEmbeddedInstanceMethodErrors
	@$(ECHO) +++++ passed test errors tests \
		>> $(RESULTFILE)

createEmbeddedInstanceErrors: cier1 cier2 cier3 cier5 cier6 cier7 cier8

###########################################################################
## Error tests for embedded instances
## test for error no embedded end mark
cier1:
	$(CIMCLI) ci $(EMBED_CLASS_DEF) id=301 \
	    comment="test error, no embedded instance end mark" \
	    embeddedInst{Test_CLITestEmbedded1 Id=401  --expExit 53 \
		>> $(RESULTFILE) 2>&1

## test for error, to many end embedded marks
cier2:
	$(CIMCLI) ci $(EMBED_CLASS_DEF) id=301 \
	    comment="test with embedded instance. To many embedded end tags." \
	    embeddedInst{Test_CLITestEmbedded1 Id=401 } } --expExit 53 \
		>> $(RESULTFILE) 2>&1

## test for error, embedded end marks with no embedded property
cier3:
	$(CIMCLI) ci $(EMBED_CLASS_DEF) id=301 \
	    comment="test with embedded instance. Missing embedded end tag." } \
	    --expExit 53 >> $(RESULTFILE) 2>&1

## test for error, embedded end marks with no embedded property
## ERROR here in that we should not be able to create instance with empty
## key parameter, Should return a system error but actually creates the
## class.  This is probably a provider error so lets not worry about it.
cier4:
	$(CIMCLI) ci $(EMBED_CLASS_DEF) id! \
	    --expExit 53 >> $(RESULTFILE) 2>&1

## Test to insure that the ! only works with String parameters.
cier5:
	$(CIMCLI) ci -n test/TestProvider Test_CLITestProviderClass id=1000 \
	    scalBool! --expExit 53 >> $(RESULTFILE) 2>&1

## test for invalid input of parameter, no property name
cier6:
	$(CIMCLI) ci $(EMBED_CLASS_DEF) =301 \
	    comment="test error, input with no property name" \
	    embeddedInst{Test_CLITestEmbedded1 Id=401  --expExit 53 \
		>> $(RESULTFILE) 2>&1

## test for error no name on embedded instance mark
cier7:
	$(CIMCLI) ci $(EMBED_CLASS_DEF) id=301 \
	    comment="test error, no embedded instance property name" \
	    {Test_CLITestEmbedded1 Id=401  --expExit 53 \
		>> $(RESULTFILE) 2>&1

## test creating instance with no properties defined. In this case, cimcli
## builds and sends and lets server decide.  In our provider, we return
## failed with message.
cier8:
	$(CIMCLI) ci -n test/TestProvider Test_CLITestEmbeddedClass \
	  --expExit 1 >> $(RESULTFILE) 2>&1

## test creating array input on non-array property TODO
cier9:

############################################################################
## Invoke method error tests

invokeEmbeddedInstanceMethodErrors: imer1 imer2 imer3

## error input. Attempt to input multiple scalar properties with same 
## name. 
imer1:
	$(CIMCLI) im Test_CLITestProviderClass -n test/TestProvider \
	    InOutParamTest \
	    scalSint32=110 scalSint32=120 --expExit 53 \
	    >> $(RESULTFILE) 2>&1

## Error input: Attempt to input scalar with array value.
imer2:
	$(CIMCLI) im Test_CLITestProviderClass -n test/TestProvider \
	    InOutParamTest \
	    scalSint32=110,120 --expExit 53 \
	    >> $(RESULTFILE) 2>&1

## not really an error, boundary condition.  Build with no parameters on
## input. Our provider returns rtn code = 0
imer3:
	$(CIMCLI) im Test_CLITestProviderClass -n test/TestProvider \
	    InOutParamTest  >> $(RESULTFILE) 2>&1


