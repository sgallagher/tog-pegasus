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
// Author: Konrad Rzeszutek <konradr@us.ibm.com>
//
//
//%/////////////////////////////////////////////////////////////////////////////


#include <Pegasus/Provider/CMPI/cmpidt.h>
#include <Pegasus/Provider/CMPI/cmpift.h>
#include <Pegasus/Provider/CMPI/cmpimacs.h>
#include <Pegasus/Provider/CMPI/cmpi_cql.h>

/* ---------------------------------------------------------------------------*/
/*                              Provider Factory                              */
/* ---------------------------------------------------------------------------*/



CMPI_EXTERN_C CMPIInstanceMI* 
TestCMPIFail_1Provider_Create_InstanceMI(const CMPIBroker* brkr,const CMPIContext *ctx, CMPIStatus *rc) 
{ 
	return NULL;
}

CMPI_EXTERN_C CMPIAssociationMI* 
TestCMPIFail_1Provider_Create_AssociationMI(const CMPIBroker* brkr,const CMPIContext *ctx,  CMPIStatus *rc) 
{
	return NULL;
}

CMPI_EXTERN_C CMPIMethodMI* 
TestCMPIFail_1Provider_Create_MethodMI(const CMPIBroker* brkr, const CMPIContext *ctx,  CMPIStatus *rc)
{
	return NULL;
}

CMPI_EXTERN_C CMPIPropertyMI* 
TestCMPIFail_1Provider_Create_PropertyMI(const CMPIBroker* brkr,const CMPIContext *ctx,  CMPIStatus *rc) 
{
	return NULL;
}

CMPI_EXTERN_C CMPIIndicationMI* 
TestCMPIFail_1Provider_Create_IndicationMI(const CMPIBroker* brkr,const CMPIContext *ctx,CMPIStatus *rc) 
{
	return NULL;
}
/* ---------------------------------------------------------------------------*/
/*             end of TestCMPIProvider                      */
/* ---------------------------------------------------------------------------*/
