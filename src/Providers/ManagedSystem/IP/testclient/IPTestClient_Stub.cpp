//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
// Author: Paulo F. Borges (pfborges@wowmail.com)
//
// Modified By: 
//         Lyle Wilkinson, Hewlett-Packard Company <lyle_wilkinson@hp.com>
//
//%/////////////////////////////////////////////////////////////////////////////

// This file has the NTPProvider-specific routines that will be called to get
// a validation of the CIM information vs. the current test system

//------------------------------------------------------------------------------
// INCLUDES
//------------------------------------------------------------------------------
//used by <explain it>
#include "IPTestClient.h"

//------------------------------------------------------------------------------
PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;


//------------------------------------------------------------------------------
void IPTestClient::_check_BIPTLEp_Antecedent(String &pv, Boolean verbose)
{
    if (verbose)
    {
	cout << "  >> checking PG_BindIPToLANEndpoint Antecedent `" <<
	     pv << "' . . ." << endl;
    }

}

//------------------------------------------------------------------------------
void IPTestClient::_check_BIPTLEp_Dependent(String &pv, Boolean verbose)
{
    if (verbose)
    {
	cout << "  >> checking PG_BindIPToLANEndpoint Dependent `" <<
	     pv << "' . . ." << endl;
    }

}

//------------------------------------------------------------------------------
void IPTestClient::_check_BIPTLEp_FrameType(Uint16 &pv, Boolean verbose)
{
    if (verbose)
    {
	cout << "  >> checking PG_BindIPToLANEndpoint FrameType `" <<
	     pv << "' . . ." << endl;
    }

}

//------------------------------------------------------------------------------
void IPTestClient::_check_IPPEp_Caption(String &pv, Boolean verbose)
{
    if (verbose)
    {
	cout << "  >> checking CIM_IPProtocolEndpoint Caption `" <<
	     pv << "' . . ." << endl;
    }

}

//------------------------------------------------------------------------------
void IPTestClient::_check_IPPEp_Description(String &pv, Boolean verbose)
{
    if (verbose)
    {
	cout << "  >> checking CIM_IPProtocolEndpoint Description `" <<
	     pv << "' . . ." << endl;
    }

}

//------------------------------------------------------------------------------
void IPTestClient::_check_IPPEp_InstallDate(CIMDateTime &pv,
                                                Boolean verbose)
{
    if (verbose)
    {
	cout << "  >> checking CIM_IPProtocolEndpoint InstallDate `" <<
	     pv.toString() << "' . . ." << endl;
    }

}

//------------------------------------------------------------------------------
void IPTestClient::_check_IPPEp_Status(String &pv, Boolean verbose)
{
    if (verbose)
    {
	cout << "  >> checking CIM_IPProtocolEndpoint Status `" <<
	     pv << "' . . ." << endl;
    }

}

//------------------------------------------------------------------------------
void IPTestClient::_check_IPPEp_SystemCreationClassName(String &pv,
                                                       Boolean verbose)
{
    if (verbose)
    {
	cout << "  >> checking CIM_IPProtocolEndpoint SCCN `" <<
	     pv << "' . . ." << endl;
    }

}

//------------------------------------------------------------------------------
void IPTestClient::_check_IPPEp_SystemName(String &pv, Boolean verbose)
{
    if (verbose)
    {
	cout << "  >> checking CIM_IPProtocolEndpoint SystemName `" <<
	     pv << "' . . ." << endl;
    }

}

//------------------------------------------------------------------------------
void IPTestClient::_check_IPPEp_CreationClassName(String &pv,
                                                 Boolean verbose)
{
    if (verbose)
    {
	cout << "  >> checking CIM_IPProtocolEndpoint CCN `" <<
	     pv << "' . . ." << endl;
    }

}

//------------------------------------------------------------------------------
void IPTestClient::_check_IPPEp_Name(String &pv, Boolean verbose)
{
    if (verbose)
    {
	cout << "  >> checking CIM_IPProtocolEndpoint Name `" <<
	     pv << "' . . ." << endl;
    }

}

//------------------------------------------------------------------------------
void IPTestClient::_check_IPPEp_NameFormat(String &pv, Boolean verbose)
{
    if (verbose)
    {
	cout << "  >> checking CIM_IPProtocolEndpoint NameFormat `" <<
	     pv << "' . . ." << endl;
    }

}

//------------------------------------------------------------------------------
void IPTestClient::_check_IPPEp_ProtocolType(Uint16 &pv, Boolean verbose)
{
    if (verbose)
    {
	cout << "  >> checking CIM_IPProtocolEndpoint ProtocolType `" <<
	     pv << "' . . ." << endl;
    }

}

//------------------------------------------------------------------------------
void IPTestClient::_check_IPPEp_OtherTypeDescription(String &pv,
                                                    Boolean verbose)
{
    if (verbose)
    {
	cout << "  >> checking CIM_IPProtocolEndpoint OtherTypeDescription `" <<
	     pv << "' . . ." << endl;
    }

}

//------------------------------------------------------------------------------
void IPTestClient::_check_IPPEp_Address(String &pv, Boolean verbose)
{
    if (verbose)
    {
	cout << "  >> checking CIM_IPProtocolEndpoint Address `" <<
	     pv << "' . . ." << endl;
    }

}

//------------------------------------------------------------------------------
void IPTestClient::_check_IPPEp_SubnetMask(String &pv, Boolean verbose)
{
    if (verbose)
    {
	cout << "  >> checking CIM_IPProtocolEndpoint SubnetMask `" <<
	     pv << "' . . ." << endl;
    }

}

//------------------------------------------------------------------------------
void IPTestClient::_check_IPPEp_AddressType(Uint16 &pv, Boolean verbose)
{
    if (verbose)
    {
	cout << "  >> checking CIM_IPProtocolEndpoint AddressType `" <<
	     pv << "' . . ." << endl;
    }

}

//------------------------------------------------------------------------------
void IPTestClient::_check_IPPEp_IPVersionSupport(Uint16 &pv,
                                                Boolean verbose)
{
    if (verbose)
    {
	cout << "  >> checking CIM_IPProtocolEndpoint IPVersionSupport `" <<
	     pv << "' . . ." << endl;
    }

}

//------------------------------------------------------------------------------
void IPTestClient::_check_IPRoute_Caption(String &pv, Boolean verbose)
{
    if (verbose)
    {
	cout << "  >> checking PG_IPRoute Caption `" <<
	     pv << "' . . ." << endl;
    }

}

//------------------------------------------------------------------------------
void IPTestClient::_check_IPRoute_Description(String &pv, Boolean verbose)
{
    if (verbose)
    {
	cout << "  >> checking PG_IPRoute Description `" <<
	     pv << "' . . ." << endl;
    }

}

//------------------------------------------------------------------------------
void IPTestClient::_check_IPRoute_InstallDate(CIMDateTime &pv,
                                                  Boolean verbose)
{
    if (verbose)
    {
	cout << "  >> checking PG_IPRoute InstallDate `" <<
	     pv.toString() << "' . . ." << endl;
    }

}

//------------------------------------------------------------------------------
void IPTestClient::_check_IPRoute_Status(String &pv, Boolean verbose)
{
    if (verbose)
    {
	cout << "  >> checking PG_IPRoute Status `" <<
	     pv << "' . . ." << endl;
    }

}

//------------------------------------------------------------------------------
void IPTestClient::_check_IPRoute_Name(String &pv, Boolean verbose)
{
    if (verbose)
    {
	cout << "  >> checking PG_IPRoute Name `" <<
	     pv << "' . . ." << endl;
    }

}

//------------------------------------------------------------------------------
void IPTestClient::_check_IPRoute_NextHop(String &pv, Boolean verbose)
{
    if (verbose)
    {
	cout << "  >> checking PG_IPRoute NextHop `" <<
	     pv << "' . . ." << endl;
    }

}

//------------------------------------------------------------------------------
void IPTestClient::_check_IPRoute_IsStatic(Boolean &pv, Boolean verbose)
{
    if (verbose)
    {
	cout << "  >> checking PG_IPRoute IsStatic `" <<
	     pv << "' . . ." << endl;
    }

}

//------------------------------------------------------------------------------
void IPTestClient::_check_IPRoute_SystemCreationClassName(String &pv,
                                                         Boolean verbose)
{
    if (verbose)
    {
	cout << "  >> checking PG_IPRoute SCCN `" <<
	     pv << "' . . ." << endl;
    }

}

//------------------------------------------------------------------------------
void IPTestClient::_check_IPRoute_SystemName(String &pv, Boolean verbose)
{
    if (verbose)
    {
	cout << "  >> checking PG_IPRoute SystemName `" <<
	     pv << "' . . ." << endl;
    }

}

//------------------------------------------------------------------------------
void IPTestClient::_check_IPRoute_ServiceCreationClassName(String &pv,
                                                          Boolean verbose)
{
    if (verbose)
    {
	cout << "  >> checking PG_IPRoute ServiceCCN `" <<
	     pv << "' . . ." << endl;
    }

}

//------------------------------------------------------------------------------
void IPTestClient::_check_IPRoute_ServiceName(String &pv, Boolean verbose)
{
    if (verbose)
    {
	cout << "  >> checking PG_IPRoute ServiceName `" <<
	     pv << "' . . ." << endl;
    }

}

//------------------------------------------------------------------------------
void IPTestClient::_check_IPRoute_CreationClassName(String &pv,
                                                   Boolean verbose)
{
    if (verbose)
    {
	cout << "  >> checking PG_IPRoute CCN `" <<
	     pv << "' . . ." << endl;
    }

}

//------------------------------------------------------------------------------
void IPTestClient::_check_IPRoute_IPDestinationAddress(String &pv,
                                                      Boolean verbose)
{
    if (verbose)
    {
	cout << "  >> checking PG_IPRoute IPDestinationAddress `" <<
	     pv << "' . . ." << endl;
    }

}

//------------------------------------------------------------------------------
void IPTestClient::_check_IPRoute_IPDestinationMask(String &pv,
                                                   Boolean verbose)
{
    if (verbose)
    {
	cout << "  >> checking PG_IPRoute IPDestinationMask `" <<
	     pv << "' . . ." << endl;
    }

}

//------------------------------------------------------------------------------
void IPTestClient::_check_IPRoute_AddressType(Uint16 &pv, Boolean verbose)
{
    if (verbose)
    {
	cout << "  >> checking PG_IPRoute AddressType `" <<
	     pv << "' . . ." << endl;
    }

}

//------------------------------------------------------------------------------
void IPTestClient::_check_IPRoute_DestinationAddress(String &pv,
                                                    Boolean verbose)
{
    if (verbose)
    {
	cout << "  >> checking PG_IPRoute DestinationAddress `" <<
	     pv << "' . . ." << endl;
    }

}

//------------------------------------------------------------------------------
void IPTestClient::_check_IPRoute_DestinationMask(String &pv,
                                                 Boolean verbose)
{
    if (verbose)
    {
	cout << "  >> checking PG_IPRoute DestinationMask `" <<
	     pv << "' . . ." << endl;
    }

}

//------------------------------------------------------------------------------
void IPTestClient::_check_LEP_SystemCreationClassName(String &pv,
                                                     Boolean verbose)
{
    if (verbose)
    {
	cout << "  >> checking CIM_LANEndpoint SCCN `" <<
	     pv << "' . . ." << endl;
    }

}

//------------------------------------------------------------------------------
void IPTestClient::_check_LEP_SystemName(String &pv, Boolean verbose)
{
    if (verbose)
    {
	cout << "  >> checking CIM_LANEndpoint SystemName `" <<
	     pv << "' . . ." << endl;
    }

}

//------------------------------------------------------------------------------
void IPTestClient::_check_LEP_CreationClassName(String &pv,
                                               Boolean verbose)
{
    if (verbose)
    {
	cout << "  >> checking CIM_LANEndpoint CCN `" <<
	     pv << "' . . ." << endl;
    }

}

//------------------------------------------------------------------------------
void IPTestClient::_check_LEP_Name(String &pv, Boolean verbose)
{
    if (verbose)
    {
	cout << "  >> checking CIM_LANEndpoint Name `" <<
	     pv << "' . . ." << endl;
    }

}

