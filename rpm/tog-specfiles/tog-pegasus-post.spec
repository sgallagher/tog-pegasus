# Start of section pegasus/rpm/tog-specfiles/tog-pegasus-post.spec
#
#           install   remove   upgrade  reinstall
# %post        1        -         2         2
#
   export PEGASUS_ARCH_LIB=%PEGASUS_ARCH_LIB

   if [ -d %PEGASUS_PREV_REPOSITORY_DIR ]; then
       # Running Repository Upgrade utility
       %PEGASUS_SBIN_DIR/repupgrade %PEGASUS_PREV_REPOSITORY_DIR \
           %PEGASUS_REPOSITORY_DIR 2>>%PEGASUS_INSTALL_LOG
       /bin/tar -C %PEGASUS_REPOSITORY_PARENT_DIR -cf \
           %PEGASUS_PREV_REPOSITORY_DIR`date '+%Y-%m-%d-%s.%N'`.tar \
           %PEGASUS_PREV_REPOSITORY_DIR_NAME
       rm -rf %PEGASUS_PREV_REPOSITORY_DIR
   fi
   if [ $1 -eq 1 ]; then
%if %{AUTOSTART}
       /sbin/chkconfig --add tog-pegasus
%endif
   :;
   elif [ $1 -gt 0 ]; then
       /etc/init.d/tog-pegasus condrestart
   :;
   fi
#
# End of section pegasus/rpm/tog-specfiles/tog-pegasus-post.spec
