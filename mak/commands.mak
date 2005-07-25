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
# commands.mak is a helper Makefile that is intended to be included in an upper level Makefile.

# Ensure that config.mak is included (so that the ROOT variable is set correctly)

ifndef ROOT
    ifdef PEGASUS_ROOT
        ROOT =  $(subst \,/,$(PEGASUS_ROOT))
    else
        ROOT = .
    endif
    include $(ROOT)/mak/config.mak
endif

ifndef OPENSSL_BIN
    OPENSSL_BIN = $(OPENSSL_HOME)/bin
endif

    OPENSSL_COMMAND = $(OPENSSL_BIN)/openssl

ifeq ($(OS),win32)
    STRIPCRS = stripcrs $(RESULTFILE) $(MASTERRESULTFILE)
    DIFF = mu compare
    SORT = mu sort
    REDIRECTERROR = 2>&1
    CIMSERVER_START_SERVICE = $(CIMSERVER_PATH)cimserver $(CIMSERVER_CONFIG_OPTIONS) -start
    CIMSERVER_STOP_SERVICE = $(CIMSERVER_PATH)cimserver -stop
    SLEEP = mu sleep
    WINDOWS_ONLY_SLEEP = mu sleep 10
    REMOVE_PEGASUS_DIRECTORY = mu rmdirhier pegasus
    MUEXE = mu.exe
    RM = mu rm
    MKDIRHIER = $(MUEXE) mkdirhier
    RMDIRHIER = $(MUEXE) rmdirhier
    ECHO = mu echo
    ECHO-E = mu echo-e
    COPY = mu copy
    CHMOD =
    CHOWN =
    CHGRP =
    CURRENT_USER=Administrator
else
    WINDOWS_ONLY_SLEEP =
endif

ifeq ($(OS),HPUX)
    STRIPCRS = 
    DIFF = diff
    SORT = sort
    REDIRECTERROR = 2>&1
    CIMSERVER_START_SERVICE = $(CIMSERVER_PATH)cimserver $(CIMSERVER_CONFIG_OPTIONS)
    CIMSERVER_STOP_SERVICE = $(CIMSERVER_PATH)cimserver -s
    SLEEP = sleep
    REMOVE_PEGASUS_DIRECTORY = rm -Rf pegasus.old; mv pegasus pegasus.old
    MUEXE = mu
    MKDIRHIER = mkdir -p
    RMDIRHIER = rm -rf
    CPDIRHIER = cp -R
    ECHO = echo
    ECHO-E = echo
    COPY = cp
    TOUCH = touch
    CAT = cat
    DOCXX = doc++

    GENERATE_RANDSEED = randseed
    GET_HOSTNAME = `nslookup \`hostname\` | grep "Name:" | sed 's/Name:[ ]*//'`

    ifeq ($(PEGASUS_PLATFORM), HPUX_PARISC_ACC)
        LIB_LINK_SUFFIX = .sl
    else
        LIB_LINK_SUFFIX = .so
    endif

    Prwxr_xr_x = 755
    Prwxrwxrwx = 777
    Prwxrwxrwt = 1777
    Pr_xr__r__ = 744
    Pr__r__r__ = 444
    Pr________ = 400
    Pr_xr_xr_x = 555
    Prw_r__r__ = 644
    CHMODDIRHIER = chmod -R

    INSTALL_USR = bin
    INSTALL_GRP = bin
    CIMSERVER_USR = root
    CIMSERVER_GRP = sys 
    CHMOD = chmod
    CHOWN = chown
    CHGRP = chgrp
    CHOWNDIRHIER = chown -R
    CHGRPDIRHIER = chgrp -R

    SYMBOLIC_LINK_CMD = ln -f -s

    CURRENT_USER=`whoami`
endif

ifeq ($(OS),solaris)
    STRIPCRS =
    DIFF = diff
    SORT = sort
    REDIRECTERROR = 2>&1
    CIMSERVER_START_SERVICE = $(CIMSERVER_PATH)cimserver $(CIMSERVER_CONFIG_OPTIONS)
    CIMSERVER_STOP_SERVICE = /usr/bin/ps -ef | /usr/bin/grep cimserver | /usr/bin/grep -v grep | /usr/bin/awk '{print "kill -9 "$$2 |"/usr/bin/ksh"}'
    SLEEP = sleep
    REMOVE_PEGASUS_DIRECTORY = rm -Rf pegasus.old; mv pegasus pegasus.old
    MUEXE = mu
    RM = rm -f
    MKDIRHIER = mkdir -p
    RMDIRHIER = rm -rf
    ECHO = echo
    ECHO-E = mu echo-e 
    COPY = cp
    CHMOD =
    CHOWN =
    CHGRP =
endif

ifeq ($(OS),linux)
    STRIPCRS =
    DIFF = diff
    SORT = sort
    REDIRECTERROR = 2>&1
    CIMSERVER_START_SERVICE = $(CIMSERVER_PATH)cimserver $(CIMSERVER_CONFIG_OPTIONS)
    CIMSERVER_STOP_SERVICE = $(CIMSERVER_PATH)cimserver -s
    SLEEP = sleep
    REMOVE_PEGASUS_DIRECTORY = rm -Rf pegasus.old; mv pegasus pegasus.old
    MUEXE = mu
    MKDIRHIER = mkdir -p
    RMDIRHIER = rm -rf
    CPDIRHIER = cp -R
    ECHO = echo
    ECHO-E = echo -e
    COPY = cp
    TOUCH = touch
    CAT = cat
    DOCXX = doc++

    GET_HOSTNAME = `host \`hostname\`|cut -d" " -f1`

    LIB_LINK_SUFFIX = .so

    Prwxrwxrwx = 777
    Prwxrwxrwt = 1777
    Prwxr_xr_x = 755
    Pr_xr__r__ = 744
    Pr__r__r__ = 444
    Pr________ = 400
    Pr_xr_xr_x = 555
    Pr_x______ = 500
    Prw_r__r__ = 644
    CHMODDIRHIER = chmod -R

    INSTALL_USR = root
    INSTALL_GRP = root
    CIMSERVER_USR = root
    CIMSERVER_GRP = root
    CHMOD = chmod
    CHOWN = chown
    CHGRP = chgrp
    CHOWNDIRHIER = chown -R
    CHGRPDIRHIER = chgrp -R

    SYMBOLIC_LINK_CMD = ln -f -s

    CURRENT_USER=`whoami`
endif

ifeq ($(OS),zos)
    STRIPCRS =
    DIFF = diff
    SORT = sort
    REDIRECTERROR =
    CIMSERVER_START_SERVICE = $(CIMSERVER_PATH)cimserver $(CIMSERVER_CONFIG_OPTIONS)
    CIMSERVER_STOP_SERVICE = $(CIMSERVER_PATH)cimserver -s
    SLEEP = sleep
    REMOVE_PEGASUS_DIRECTORY = rm -Rf pegasus.old; mv pegasus pegasus.old
    MUEXE = mu
    RM = rm -f
    MKDIRHIER = mkdir -p
    RMDIRHIER = rm -rf
    ECHO =
    ECHO-E =
    COPY = cp
    CHMOD =
    CHOWN =
    CHGRP =
endif

ifeq ($(OS),VMS)
    STRIPCRS =$(MUEXE) echo "STRIPCRS not defined in commands.mak"
    DIFF = mu compare
    SORT = mu sort
    REDIRECTERROR = $(MUEXE) echo "REDIRECTERROR not defined in commands.mak"
    CIMSERVER_START_SERVICE = $(CIMSERVER_PATH)cimserver $(CIMSERVER_CONFIG_OPTIONS)
    CIMSERVER_STOP_SERVICE = $(CIMSERVER_PATH)cimserver -s
    SLEEP = wait
    REMOVE_PEGASUS_DIRECTORY = mu rmdirhier pegasus
    MUEXE = mu
    MKDIRHIER = $(MUEXE) mkdirhier
    RMDIRHIER = $(MUEXE) rmdirhier
    ECHO = $(MUEXE) echo 
    ECHO-E = $(MUEXE) echo "ECHO-E not defined in commands.mak"
    CHMOD = $(MUEXE) echo "CHMOD not defined in commands.mak"
    CHOWN = $(MUEXE) echo "CHOWN not defined in commands.mak"
    CHGRP = $(MUEXE) echo  "CHGRP not defined in commands.mak"
endif

ifeq ($(OS),aix)
    STRIPCRS =
    DIFF = diff
    SORT = sort
    REDIRECTERROR =
    CIMSERVER_START_SERVICE = $(CIMSERVER_PATH)cimserver $(CIMSERVER_CONFIG_OPTIONS)
    CIMSERVER_STOP_SERVICE = $(CIMSERVER_PATH)cimserver -s
    SLEEP = sleep
    REMOVE_PEGASUS_DIRECTORY = rm -Rf pegasus.old; mv pegasus pegasus.old
    MUEXE = mu
    RM = rm -f
    MKDIRHIER = mkdir -p
    RMDIRHIER = rm -rf
    ECHO = echo
    ECHO-E = 
    COPY = cp
    CHMOD =
    CHOWN =
    CHGRP =
endif

ifeq ($(OS),darwin)
    STRIPCRS =
    DIFF = diff
    SORT = sort
    REDIRECTERROR = 2>&1
    CIMSERVER_START_SERVICE = $(CIMSERVER_PATH)cimserver $(CIMSERVER_CONFIG_OPTIONS)
    CIMSERVER_STOP_SERVICE = $(CIMSERVER_PATH)cimserver -s 
    SLEEP = sleep
    REMOVE_PEGASUS_DIRECTORY = rm -Rf pegasus.old; mv pegasus pegasus.old
    MUEXE = mu
    RM = rm -f
    MKDIRHIER = mkdir -p
    RMDIRHIER = rm -rf
    ECHO = echo
    ECHO-E = mu echo-e
    COPY = cp
    TOUCH = touch
    CAT = cat 	
    
    CHMOD = chmod
    CHOWN = chown
    CHGRP = chgrp

    CHMODDIRHIER = chmod -R 	
    CHOWNDIRHIER = chown -R
    CHGRPDIRHIER = chgrp -R

    SYMBOLIC_LINK_CMD = ln -f -s

    CURRENT_USER=`whoami`		
endif

ifndef TMP_DIR
    ifdef PEGASUS_TMP
        TMP_DIR = $(subst \,/,$(PEGASUS_TMP))
    else
        TMP_DIR = .
    endif
endif

CMDSFORCE: 

cimstop: CMDSFORCE
	$(CIMSERVER_STOP_SERVICE)

cimstart: CMDSFORCE
	$(CIMSERVER_START_SERVICE)

sleep: CMDSFORCE
	$(SLEEP) $(TIME)

mkdirhier: CMDSFORCE
	$(MKDIRHIER) $(DIRNAME)

rmdirhier: CMDSFORCE
	$(RMDIRHIER) $(DIRNAME)

setpermissions: CMDSFORCE
	$(CHMOD) $(PERMISSIONS) $(OBJECT)
	$(CHOWN) $(OWNER) $(OBJECT) 
	$(CHGRP) $(GROUP) $(OBJECT) 

sethierpermissions: CMDSFORCE
	$(CHMODDIRHIER) $(PERMISSIONS) $(OBJECT)
	$(CHOWNDIRHIER) $(OWNER) $(OBJECT) 
	$(CHGRPDIRHIER) $(GROUP) $(OBJECT) 

createlink: CMDSFORCE
	$(SYMBOLIC_LINK_CMD) $(OBJECT) $(LINKNAME)

createrandomseed: CMDSFORCE
	$(GENERATE_RANDSEED) $(FILENAME)

# Because commands.mak is intended to be used as
# helper Makefile, embedded use of calls to "make" are
# problematic because the name of toplevel is not known.
# To workaround this problem, the MAKEOPTIONS define 
# has been added to the following commands to
# allow the name of the toplevel Makefile to be included. 
# E.g.,
#
# make MAKEOPTION="-f TestMakefile" cimstop_IgnoreError
#
# However, a better alternative would be to call the 
# the command directly from the toplevel makefile.
# E.g.,
#
# make -f TestMakefile -i cimstop

cimstop_IgnoreError: CMDSFORCE
	@$(MAKE) $(MAKEOPTIONS) -i cimstop

rmdirhier_IgnoreError: CMDSFORCE
	@$(MAKE) $(MAKEOPTIONS) -i rmdirhier

mkdirhier_IgnoreError: CMDSFORCE
	@$(MAKE) $(MAKEOPTIONS) -i mkdirhier


# The runTestSuite option starts the CIM Server
# with a designated set of configuration options (i.e.,
# CIMSERVER_CONFIG_OPTIONS) and then runs a specified
# set of tests (i.e., TESTSUITE_CMDS). After the tests
# have been executed, the CIM Server is stopped.
#
# To call runTestSuite, you simply (1) define the
# configuration options, (2) define the set of
# tests and (3) call commands.mak with the appropriate
# parameters.  Note: when specifying the set of tests
# @@ is used as a replacement for blank. E.g., the
# following sequence can be used to enable the
# HTTP connection, disable the HTTPS connection and
# run an osinfo, TestClient, OSTestClient,
# and InvokeMethod2 test.
#
#runTestSuiteTest_CONFIG_OPTIONS = enableHttpConnection=true enableHttpsConnection=false
#runTestSuiteTest_TEST_CMDS = \
#   osinfo@@-hlocalhost@@-p5988@@-uguest@@-wguest\
#   TestClient@@-local \
#   OSTestClient \
#   $(MAKE)@@--directory=$(PEGASUS_ROOT)/src/Pegasus/Client/tests/InvokeMethod2@@poststarttests
#
#runTestSuiteTest: CMDSFORCE
#	$(MAKE) $(MAKEOPTIONS) runTestSuite CIMSERVER_CONFIG_OPTIONS="$(runTestSuiteTest_CONFIG_OPTIONS)" TESTSUITE_CMDS="$(runTestSuiteTest_TEST_CMDS)"

runTestSuite: CMDSFORCE
	$(CIMSERVER_START_SERVICE)
	$(WINDOWS_ONLY_SLEEP)
	$(foreach i, $(TESTSUITE_CMDS), $(subst @@, ,$(i)))
	$(CIMSERVER_STOP_SERVICE)

ifndef PEGASUS_SSLCNF_FULLY_QUALIFIED_DSN
  PEGASUS_SSLCNF_FULLY_QUALIFIED_DSN=$(GET_HOSTNAME)
endif

createSSLCnfFile: CMDSFORCE
	@$(RM) $(PEGASUS_SSLCERT_CNFFILE)
	@$(ECHO) "[ req ]" >> $(PEGASUS_SSLCERT_CNFFILE)
	@$(ECHO) "distinguished_name     = req_distinguished_name" >> $(PEGASUS_SSLCERT_CNFFILE)
	@$(ECHO) "prompt                 = no"  >> $(PEGASUS_SSLCERT_CNFFILE)
	@$(ECHO) "[ req_distinguished_name ]" >>  $(PEGASUS_SSLCERT_CNFFILE)
	@$(ECHO) "C                      = $(PEGASUS_SSLCNF_COUNTRY_CODE)" >> $(PEGASUS_SSLCERT_CNFFILE)
	@$(ECHO) "ST                     = $(PEGASUS_SSLCNF_STATE)" >> $(PEGASUS_SSLCERT_CNFFILE)
	@$(ECHO) "L                      = $(PEGASUS_SSLCNF_LOCALITY)" >> $(PEGASUS_SSLCERT_CNFFILE)
	@$(ECHO) "O                      = $(PEGASUS_SSLCNF_ORGANIZATION)" >> $(PEGASUS_SSLCERT_CNFFILE)
	@$(ECHO) "OU                     = $(PEGASUS_SSLCNF_ORGANIZATION_UNIT)" >> $(PEGASUS_SSLCERT_CNFFILE)
	@$(ECHO) "CN                     = $(PEGASUS_SSLCNF_FULLY_QUALIFIED_DSN)" >> $(PEGASUS_SSLCERT_CNFFILE)

createSSLCertificate: CMDSFORCE
ifdef PEGASUS_SSL_RANDOMFILE
	@$(OPENSSL_COMMAND) req -x509 -days $(PEGASUS_SSLCERT_DAYS) -newkey rsa:2048 -rand $(PEGASUS_SSLCERT_RANDOMFILE) -nodes -config $(PEGASUS_SSLCERT_CNFFILE) -keyout $(PEGASUS_SSLCERT_KEYFILE) -out 
$(PEGASUS_SSLCERT_CERTFILE)
else
	@$(OPENSSL_COMMAND) req -x509 -days $(PEGASUS_SSLCERT_DAYS) -newkey rsa:2048 -nodes -config $(PEGASUS_SSLCERT_CNFFILE) -keyout $(PEGASUS_SSLCERT_KEYFILE) -out $(PEGASUS_SSLCERT_CERTFILE)
endif

displayCertificate: CMDSFORCE
	@$(OPENSSL_COMMAND) x509 -in $(PEGASUS_SSLCERT_CERTFILE) -text
