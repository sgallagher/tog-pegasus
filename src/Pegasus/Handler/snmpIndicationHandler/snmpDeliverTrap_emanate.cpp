//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett Packard, IBM, The Open Group,
// Tivoli Systems
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
// Author: Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "snmpDeliverTrap_emanate.h"

PEGASUS_NAMESPACE_BEGIN

PEGASUS_USING_STD;

void snmpDeliverTrap_emanate::deliverTrap(const String& trapOid,
    const String& enterprise,
    const String& destination,
    const String& trapType,
    Array<String>& vbOids,
    Array<String>& vbTypes,
    Array<String>& vbValues)
{
    int            fifo_fd;
    struct stat    sbuf;

    trapHeader myTrapHeader;
    trapHeader myTrapData;

    cout << "NITIN: calling deliverTrap()" << endl;

    if (stat(_SNMP_FIFO, &sbuf) == -1) 
        cout << "The fifo is not opened by EMANATE subagent" << endl;
    else
    {
        if ((fifo_fd = open (_SNMP_FIFO, O_WRONLY|O_NONBLOCK)) == -1) 
            cout << "The fifo is not opened for reading. Subagent is not running" << endl;
        else
        {
            strcpy(myTrapHeader.destination, _CString(destination));
            strcpy(myTrapHeader.snmpType, "SNMPV2");
            strcpy(myTrapHeader.enterprise, _CString(enterprise));
            strcpy(myTrapHeader.trapOid, _CString(trapOid));
            myTrapHeader.variable_packets =  vbOids.size();

            if (write(fifo_fd, &myTrapHeader, sizeof(myTrapHeader)) <=0)
                cout << "Error in writing" << endl;
            else
            {
                for(int packets = 0; packets<vbOids.size(); packets++)
                {
                    trapData myTrapData;
                    strcpy(myTrapData.vbOid, _CString(vbOids[packets]));
                    strcpy(myTrapData.vbType, _CString(vbTypes[packets]));
                    strcpy(myTrapData.vbValue, _CString(vbValues[packets]));
                    write(fifo_fd, &myTrapData, sizeof(myTrapData));
                }
            }
        }
        close (fifo_fd);
    }
}

PEGASUS_NAMESPACE_END
