
All you have to do to build the rpm package is

- Starting the rpmBuild script
CAVEAT: the basename of the directory should be pegasus-1.0
(example: /usr/src/pegasus-1.0)

- Waiting (a cup of coffee ?) while the package is built
(make clean; make depend; make)

-  Installing: Issue
rpm -U /usr/src/packages/RPMS/i386/pegasus-1.0-1.i386.rpm 
(the version number following the name may differ)

- Creating the repository
cimmof -I/etc/pegasus/mof -nroot /etc/pegasus/mof/CIM_Core25.mof
cimmof -I/etc/pegasus/mof -nroot/cimv20 /etc/pegasus/mof/CIM_Schema25.mof

- Starting the Server
cimserver
(The start script /etc/rc.d/pegasus is prepared for a SuSE system, I'd like
to see this adapted to RedHat.)
