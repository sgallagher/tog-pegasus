# Start of section pegasus/rpm/tog-specfiles/tog-pegasus-preun.spec
#           install   remove   upgrade  reinstall
#  preun       -        0         1         -
if [ $1 -eq 0 ]; then
   # Check if the cimserver is running
   isRunning=`ps -el | grep cimserver |  grep -v "grep cimserver"`
   if [ "$isRunning" ]; then
      %PEGASUS_SBIN_DIR/cimserver -s
   fi 
   /sbin/chkconfig --del tog-pegasus;
   rm -f %PEGASUS_VARDATA_DIR/cimserver_current.conf;
   rm -f %PEGASUS_INSTALL_LOG;
   [ -d %PEGASUS_REPOSITORY_DIR ]  && rm -rf %PEGASUS_REPOSITORY_DIR;
   [ -d %PEGASUS_VARDATA_CACHE_DIR ]  && rm -rf %PEGASUS_VARDATA_CACHE_DIR;
   rm -f %PEGASUS_LOCAL_DOMAIN_SOCKET_PATH;
   rm -f %PEGASUS_CIMSERVER_START_FILE;
   rm -f %CIMSERVER_LOCK_FILE;
fi
#
# End of section pegasus/rpm/tog-specfiles/tog-pegasus-preun.spec
