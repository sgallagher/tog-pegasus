//%2004////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN
// ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
// "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//==============================================================================
//
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <cassert>
#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/Client/ClientOpPerformanceDataHandler.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

const String NAMESPACE = "root/cimv2";


class CliStat : public ClientOpPerformanceDataHandler
{
public:

    virtual void handleClientOpPerformanceData (const ClientOpPerformanceData & item){
            if (!(0 <= item.operationType) || !(39 >= item.operationType)){
               cerr << "roundTripTime is incorrect in ClientOpPerformanceData " << endl;
               cerr << "error in Pegasus/Client/test/ClientStatistics" << endl;
               exit(1);            }

            if (item.roundTripTime == 0){
               cerr << "roundTripTime is incorrect in ClientOpPerformanceData " << endl;
               cerr << "error in Pegasus/Client/test/ClientStatistics" << endl;
               // ATTN: Temporarily disable this failure.  See Bugzilla 3211.
               //exit(1);
            }

            if (item.requestSize == 0){
                cerr << "requestSize is incorrect in ClientOpPerformanceData " << endl;
                cerr << "error in Pegasus/Client/test/ClientStatistics" << endl;
                exit(1);
            }

            if (item.responseSize == 0){
                cerr << "responseSize is incorrect in ClientOpPerformanceData " << endl;
                cerr << "error in Pegasus/Client/test/ClientStatistics" << endl;
                exit(1);
            }

            if (item.serverTimeKnown) {
                if (item.serverTime == 0){
                   cerr << "serverTime is incorrect in ClientOpPerformanceData " << endl;
                   cerr << "error in Pegasus/Client/test/ClientStatistics" << endl;
                   exit(1);
                }
            }  
   }
};




int main(int argc, char** argv)
{
   cout << "++++++testing Client Performance Statistics " << endl;
    try{
	   CIMClient client;
	   client.connect("localhost", 5988, String::EMPTY, String::EMPTY);

	 CliStat stat = CliStat();
         client.registerClientOpPerformanceDataHandler(stat);
         String className = "PG_ComputerSystem";
         String nameSpace = "root/cimv2";
         Array<CIMObjectPath> instances = client.enumerateInstanceNames(nameSpace,
                                                                        className); 
    }

    catch(Exception& e){
	   PEGASUS_STD(cerr) << "Error: " << e.getMessage() << PEGASUS_STD(endl);
	   exit(1);
    }





    PEGASUS_STD(cout) << "+++++ passed all tests" << PEGASUS_STD(endl);

    return 0;
}
