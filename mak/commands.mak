ifndef OS
 ifndef ROOT
  ifdef PEGASUS_ROOT
     ROOT =  $(subst \,/,$(PEGASUS_ROOT))
  else
     ROOT = .
  endif
 endif
 include $(ROOT)/mak/config.mak
endif

ifeq ($(OS),win32)
  STRIPCRS = stripcrs $(RESULTFILE) $(MASTERRESULTFILE)
  DIFF = mu compare
  REDIRECTERROR = 2>&1
  CIMSERVER_START_SERVICE = cimserver -start $(CIMSERVER_CONFIG_OPTIONS)
  CIMSERVER_STOP_SERVICE = cimserver -stop
  SLEEP =
  REMOVE_PEGASUS_DIRECTORY = mu rmdirhier pegasus
  MUEXE = mu.exe
  MKDIRHIER = $(MUEXE) mkdirhier
  RMDIRHIER = $(MUEXE) rmdirhier
  ECHO = mu echo
  COPY = mu copy
  CHMOD =
  CHOWN =
  CHGRP =
endif

ifeq ($(OS),HPUX)
  STRIPCRS = 
  DIFF = diff
  REDIRECTERROR = 2>&1
  CIMSERVER_START_SERVICE = cimserver $(CIMSERVER_CONFIG_OPTIONS)
  CIMSERVER_STOP_SERVICE = cimserver -s
  SLEEP = sleep
  REMOVE_PEGASUS_DIRECTORY = rm -Rf pegasus.old; mv pegasus pegasus.old
  MUEXE = mu
  MKDIRHIER = mkdir -p
  RMDIRHIER = rm -rf
  CPDIRHIER = cp -R
  ECHO = echo
  COPY = cp
  TOUCH = touch
  CAT = cat
  DOCXX = doc++

  GENERATE_RANDSEED = randseed
  OPENSSL_COMMAND = openssl
  GET_HOSTNAME = `hostname`

  ifeq ($(PEGASUS_PLATFORM), HPUX_PARISC_ACC)
    LIB_LINK_SUFFIX = .sl
  else
    LIB_LINK_SUFFIX = .so
  endif

  Pdr_xr_xr_x = 555
  P_rwxr_xr_x = 755
  P_r_xr__r__ = 744
  P_r__r__r__ = 444
  P_r________ = 400
  P_r_xr_xr_x = 555
  P_rw_r__r__ = 644
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
endif

ifeq ($(OS),solaris)
  STRIPCRS =
  DIFF = diff
  REDIRECTERROR = 2>&1
  CIMSERVER_START_SERVICE = cimserver $(CIMSERVER_CONFIG_OPTIONS)
  CIMSERVER_STOP_SERVICE = /usr/bin/ps -ef | /usr/bin/grep cimserver | /usr/bin/grep -v grep | /usr/bin/awk '{print "kill -9 "$$2 |"/usr/bin/ksh"}'
  SLEEP = sleep
  REMOVE_PEGASUS_DIRECTORY = rm -Rf pegasus.old; mv pegasus pegasus.old
  MUEXE = mu
  MKDIRHIER = $(MUEXE) mkdirhier
  RMDIRHIER = $(MUEXE) rmdirhier
  ECHO = echo
  COPY = cp
  CHMOD =
  CHOWN =
  CHGRP =
endif

ifeq ($(OS),linux)
  STRIPCRS =
  DIFF = diff
  REDIRECTERROR = 2>&1
  CIMSERVER_START_SERVICE = cimserver $(CIMSERVER_CONFIG_OPTIONS)
  CIMSERVER_STOP_SERVICE = cimserver -s
  SLEEP = sleep
  REMOVE_PEGASUS_DIRECTORY = rm -Rf pegasus.old; mv pegasus pegasus.old
  MUEXE = mu
  MKDIRHIER = mkdir -p
  RMDIRHIER = rm -rf
  CPDIRHIER = cp -R
  ECHO = echo
  COPY = cp
  TOUCH = touch
  CAT = cat
  DOCXX = doc++

  OPENSSL_COMMAND = openssl
  GET_HOSTNAME = `hostname`

  LIB_LINK_SUFFIX = .so

  Pdr_xr_xr_x = 555
  P_rwxr_xr_x = 755
  P_r_xr__r__ = 744
  P_r__r__r__ = 444
  P_r________ = 400
  P_r_xr_xr_x = 555
  P_rw_r__r__ = 644
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
endif

ifeq ($(OS),zos)
  STRIPCRS =
  DIFF =
  REDIRECTERROR =
  CIMSERVER_START_SERVICE = cimserver $(CIMSERVER_CONFIG_OPTIONS)
  CIMSERVER_STOP_SERVICE = cimserver -s
  SLEEP = sleep
  REMOVE_PEGASUS_DIRECTORY = rm -Rf pegasus.old; mv pegasus pegasus.old
  MUEXE = mu
  MKDIRHIER = mkdir -p
  RMDIRHIER = rm -rf
  ECHO =
  COPY = cp
  CHMOD =
  CHOWN =
  CHGRP =
endif

ifeq ($(OS),VMS)
  STRIPCRS =
  DIFF = 
  REDIRECTERROR =
  CIMSERVER_START_SERVICE = cimserver $(CIMSERVER_CONFIG_OPTIONS)
  CIMSERVER_STOP_SERVICE = cimserver -s
  SLEEP = wait
  REMOVE_PEGASUS_DIRECTORY = mu rmdirhier pegasus
  MUEXE = mu
  MKDIRHIER = $(MUEXE) mkdirhier
  RMDIRHIER = $(MUEXE) rmdirhier
  ECHO =
  CHMOD =
  CHOWN =
  CHGRP =
endif

ifeq ($(OS),aix)
  STRIPCRS =
  DIFF = 
  REDIRECTERROR =
  CIMSERVER_START_SERVICE = cimserver $(CIMSERVER_CONFIG_OPTIONS)
  CIMSERVER_STOP_SERVICE = cimserver -s
  SLEEP = sleep
  REMOVE_PEGASUS_DIRECTORY = rm -Rf pegasus.old; mv pegasus pegasus.old
  MUEXE = mu
  MKDIRHIER = mkdir -p
  RMDIRHIER = rm -rf
  ECHO = echo
  COPY = cp
  CHMOD =
  CHOWN =
  CHGRP =
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

cimstop_IgnoreError: CMDSFORCE
	@make -f $(ROOT)/mak/commands.mak -i cimstop

cimstart: CMDSFORCE
	$(CIMSERVER_START_SERVICE)

sleep: CMDSFORCE
	$(SLEEP) $(TIME)

mkdirhier: CMDSFORCE
	$(MKDIRHIER) $(DIRNAME)

mkdirhier_IgnoreError: CMDSFORCE
	@make -f $(ROOT)/mak/commands.mak -i mkdirhier

rmdirhier: CMDSFORCE
	$(RMDIRHIER) $(DIRNAME)

rmdirhier_IgnoreError: CMDSFORCE
	@make -f $(ROOT)/mak/commands.mak -i rmdirhier

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

testCommands: CMDSFORCE
	$(MAKE) -f $(ROOT)/mak/commands.mak sleep TIME=10
	$(MAKE) -f $(ROOT)/mak/commands.mak rmdirhier_IgnoreError DIRNAME=$(TMP_DIR)/PegasusTestDirA/PegasusTestDirB
	$(MAKE) -f $(ROOT)/mak/commands.mak mkdirhier DIRNAME=$(TMP_DIR)/PegasusTestDirA/PegasusTestDirB
	$(MAKE) -f $(ROOT)/mak/commands.mak mkdirhier_IgnoreError DIRNAME=$(TMP_DIR)/PegasusTestDirA/PegasusTestDirB
	$(MAKE) -f $(ROOT)/mak/commands.mak rmdirhier DIRNAME=$(TMP_DIR)/PegasusTestDirA
	$(MAKE) -f $(ROOT)/mak/commands.mak rmdirhier_IgnoreError DIRNAME=$(TMP_DIR)/PegasusTestDirA/PegasusTestDirB
	$(MAKE) -f $(ROOT)/mak/commands.mak cimstop_IgnoreError
	$(MAKE) -f $(ROOT)/mak/commands.mak cimstart
	$(MAKE) -f $(ROOT)/mak/commands.mak cimstop
	$(MAKE) -f $(ROOT)/mak/commands.mak cimstop_IgnoreError
	$(MAKE) -f $(ROOT)/mak/commands.mak cimstart CIMSERVER_CONFIG_OPTIONS="traceLevel=1 traceComponents=XmlIO"
	cimconfig -g traceLevel -c
	cimconfig -g traceComponents -c
	$(MAKE) -f $(ROOT)/mak/commands.mak cimstop
