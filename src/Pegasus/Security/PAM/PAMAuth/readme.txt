
This directory contains TestPAMAuth, a program that tests whether the Pegasus
PAM authentication routines are working (and whether PAM has been configured
correctly on your operating system). To use try this:

    $ TestPAMAuth bob
    Enter password for bob: _

The program attempts to authenticate and then prints the result.

You must properly configure PAM authentication for the "wbem" authentication
type. For Linux, see <PEGASUS_ROOT>/rpm/wbem. For Solaris, see 
./pam.conf.solaris.

