//%LICENSE////////////////////////////////////////////////////////////////
//
// Licensed to The Open Group (TOG) under one or more contributor license
// agreements.  Refer to the OpenPegasusNOTICE.txt file distributed with
// this work for additional information regarding copyright ownership.
// Each contributor licenses this file to you under the OpenPegasus Open
// Source License; you may not use this file except in compliance with the
// License.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//////////////////////////////////////////////////////////////////////////

#include <Pegasus/Config/ConfigManager.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Char16.h>
#include <Pegasus/Client/CIMClient.h>

#include <iostream>

#ifdef PEGASUS_OS_ZOS
// This is inline code. No external dependency is created.
#include <unistd.h>
#include <Pegasus/General/SetFileDescriptorToEBCDICEncoding.h>
#endif

PEGASUS_USING_PEGASUS;

PEGASUS_USING_STD;

static Boolean verbose ;
static Uint32 k;


String makeBogusIp(String &ip, const String &num1, const String &num2)
{
    //Decide if the address passed is IPV4 or IPV6
    //assuming that IPV4 contains "." and IPV6 ":"
    Uint32 idx = ip.reverseFind('.');
    Uint32 idx6 = ip.reverseFind(':');

    if(idx != PEG_NOT_FOUND)
    {
        String tmp = ip.subString(idx+1);
        ip.remove(idx+1);

        if(tmp == num1)
        {
            ip.append(num2);
        }
        else
        {
            ip.append(num1);
        }
    }
#ifdef PEGASUS_ENABLE_IPV6
    else if(idx6 != PEG_NOT_FOUND)
    {  
        String tmp = ip.subString(idx6+1);
        ip.remove(idx6+1);

        if(tmp == num1)
        {
            ip.append(num2);
        }
        else
        {
            ip.append(num1);
        }
    }
#endif
    else
    {
        cerr << "Invalid ip address detected";
    }
    return ip;
}


void _getSystemInterface(Array<String> list )
{
    for(Uint32 i =0; i < list.size(); ++i)
    {
        //Detect if ip is link-local address
        //getInterfaceAddrs() currently returns link-local addr
        //without zone index( see bug 9221)
        //This should be removed when 9221 is fixed
        String tmp = list[i].subString(0,4);
        if( !(String::equalNoCase(tmp, "fe80")))
        {
            cout << list[i] << endl;
            k = i;
            break;
        }
    }
}
void _ConnectClientAndTest(CIMClient &clnt,
    const Uint32 port, 
    const String &ip)
{
        clnt.connect(ip, port,"guest", "guest");

        CIMClass cimClass = clnt.getClass(
            CIMNamespaceName("root/cimv2"),
            CIMName ("CIM_ManagedElement"),
            true,
            false,
            false,
            CIMPropertyList());

            PEGASUS_TEST_ASSERT(String::equal("CIM_ManagedElement",
               cimClass.getClassName().getString().getCString()));

        clnt.disconnect();
}
void  _restrictionTest(String &list)
{

    CIMClient clnt;
    //This should connect
    _ConnectClientAndTest(clnt,5988,list);

    String bogusIp1 = makeBogusIp(list,"30","9");

    try
    {
        //This should fail to connect
        _ConnectClientAndTest(clnt, 5988,bogusIp1);
    }
    catch(CannotConnectException &e)
    {
        cout << "Expected exception: " << e.getMessage() << endl;
    }
}

/* 
 * Command Line arguments of this program are as
 * getSystemInterface : get the first ip address of the machine
 * RestrictionTest    : test network restriction functionality
 *
 */

int main(int argc, char** argv)
{
#ifdef PEGASUS_OS_ZOS
        // For z/OS set stdout and stderr to EBCDIC
        setEBCDICEncoding(STDOUT_FILENO);
        setEBCDICEncoding(STDERR_FILENO);
#endif

    verbose = getenv("PEGASUS_TEST_VERBOSE") ? true : false;
    if(argc != 2)
    {
        cerr << " Usage: " ;
        cerr << argv[0] <<" getSystemInterface|RestrictionTest" << endl;
        return 0;
    }

    try
    {
        Array<String> list = System::getInterfaceAddrs();
        if( list.size() > 0 )
        {
            if( !strcmp(argv[1], "getSystemInterface"))
            {
                _getSystemInterface(list);
                return 0;
            }
            else if( !strcmp(argv[1], "RestrictionTest"))
            {
                _restrictionTest(list[k]);
            }
            else
            {
                cerr << " Usage: " ;
                cerr << argv[0] <<" getSystemInterface|RestrictionTest"<< endl;
                return 0;
            }
        }
        else
        {
            cerr << "++++ No Interface Detected ++++ " << endl;
            return 0;
        }
    }
    catch(Exception& e)
    {
        cerr << "Error: " << e.getMessage() << endl;
        cerr << argv[0] << "+++++ Failed ++" << endl;

        return(1);
    }

    cout << argv[0] << " +++++ passed all tests" << endl;
    return(0);
}
