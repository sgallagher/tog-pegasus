     The OpenPegasus Unix load test scripts

11 December 2014

The 3 bash scripts in this directory are the basis for common load testing
of the CIM Server starting with Pegasu 2.14.

The scripts are:

1. watchcimserver - generates a regular server information output from the
ps command while the server is running.  

To get more detailed information use the help option:
   watchcimserver --help

2. The client code execution scripts are:
   a. chosimpletest
   b. choparalleltest

The second choparalleltest is apparently a more rigerous test in that it 
Each of these scripts operates by starting a number of clients concurrently
and letting those clients execute requests against the server.

The requests continue aach in its own background process to create a number of
parallel requests to the server.

The client is stopped by:
   a. Stopping the cimserver.
   b. Reaching the time limit defined by the command line options on the
      script
   c. The operator entering ctrl-c which kills the parent and child processes

The difference between the two scripts is that the simple scripts executes
all of the background tasks until they complete and then restarts the loop
so that since they finish unevenly, some end their background tasks faster
than others.

The parallel script sets each background client to operate in a continuous
loop until loop and runs them repeatedly.

The parallel script is generally more taxing on the server than the
simple script.

Today both are included because the parallel script tends to actually overload
the server.

Typical usage might be:

1. Start the cimserver
2. open a terminal window and start the watchcimserver script. For example:

    watchcimserver -i 240 -t 30h -t 10m

This will generate output both to the console and to the file watchcimserver.txt
every 240 seconds for 30 hours and 10 minutes and then will shutdown.

3. Start the cho script to start generating requests to the server.  Generally
the command line on each is the sam with one difference, the parallel script
outputs results to either the console or the console and a log file for
each client based on the -l option

For example:
    choparalleltest -t 30h -l

Would cause the script to initiate the clients defined in the script and
let them run for 30 hours. At that point it would shut all the clients down.

STATUS:

These scripts are considered unsupported but since the provide a common basis
for load testing the CIMServer, we expect to continue to grow them and add
functionality.

Expect them to change at any time without warning since they are in the
unsupported directory.




