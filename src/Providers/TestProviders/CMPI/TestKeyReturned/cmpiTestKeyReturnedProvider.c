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
//
//%/////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

#include <Pegasus/Provider/CMPI/cmpidt.h>
#include <Pegasus/Provider/CMPI/cmpift.h>
#include <Pegasus/Provider/CMPI/cmpimacs.h>

static const CMPIBroker *_broker;

/* ---------------------------------------------------------------------------*/
/*                      Instance Provider Interface                           */
/* ---------------------------------------------------------------------------*/

CMPIStatus TestCMPIKeyReturnedProviderCleanup(
    CMPIInstanceMI * mi,
    const CMPIContext * ctx,
    CMPIBoolean term)
{
  CMReturn(CMPI_RC_OK);
}

CMPIStatus TestCMPIKeyReturnedProviderEnumInstanceNames(
    CMPIInstanceMI * mi,
    const CMPIContext * ctx,
    const CMPIResult * rslt,
    const CMPIObjectPath * ref)
{
    CMReturn(CMPI_RC_ERR_NOT_SUPPORTED);
}

CMPIStatus TestCMPIKeyReturnedProviderEnumInstances(
    CMPIInstanceMI * mi,
    const CMPIContext * ctx,
    const CMPIResult * rslt,
    const CMPIObjectPath * ref,
    const char **properties)
{
    CMPIValue num;
    CMPIValue flag;

    num.uint32=42;
    flag.boolean=1;

    CMPIInstance * myInst=CMNewInstance(_broker,ref,0);
    
    CMSetProperty(myInst,"Name", "TestInstance", CMPI_chars);    
    CMSetProperty(myInst,"Number", &num, CMPI_uint32);
    CMSetProperty(myInst,"Flag", &flag, CMPI_boolean);

    CMReturnInstance(rslt, myInst);
    CMReturnDone(rslt);
    CMReturn(CMPI_RC_OK);
}

CMPIStatus TestCMPIKeyReturnedProviderGetInstance(
    CMPIInstanceMI * mi,
    const CMPIContext * ctx,
    const CMPIResult * rslt,
    const CMPIObjectPath * cop,
    const char **properties)
{
    CMReturn(CMPI_RC_ERR_NOT_SUPPORTED);
}

CMPIStatus TestCMPIKeyReturnedProviderCreateInstance(
    CMPIInstanceMI * mi,
    const CMPIContext * ctx,
    const CMPIResult * rslt,
    const CMPIObjectPath * cop,
    const CMPIInstance * ci)
{
    CMReturn(CMPI_RC_ERR_NOT_SUPPORTED);
}

CMPIStatus TestCMPIKeyReturnedProviderModifyInstance(
    CMPIInstanceMI * mi,
    const CMPIContext * ctx,
    const CMPIResult * rslt,
    const CMPIObjectPath * cop,
    const CMPIInstance * ci,
    const char **properties)
{
    CMReturn(CMPI_RC_ERR_NOT_SUPPORTED);
}

CMPIStatus TestCMPIKeyReturnedProviderDeleteInstance(
    CMPIInstanceMI * mi,
    const CMPIContext * ctx,
    const CMPIResult * rslt,
    const CMPIObjectPath * cop)
{
    CMReturn(CMPI_RC_ERR_NOT_SUPPORTED);
}

CMPIStatus TestCMPIKeyReturnedProviderExecQuery(
    CMPIInstanceMI * mi,
    const CMPIContext * ctx,
    const CMPIResult * rslt,
    const CMPIObjectPath * ref,
    const char *lang,
    const char *query)
{
    CMReturn(CMPI_RC_ERR_NOT_SUPPORTED);
}


/* ---------------------------------------------------------------------------*/
/*                              Provider Factory                              */
/* ---------------------------------------------------------------------------*/
CMInstanceMIStub(
    TestCMPIKeyReturnedProvider,
    TestCMPIKeyReturnedProvider,
    _broker,
    CMNoHook);

/* ---------------------------------------------------------------------------*/
/*             end of TestCMPIKeyReturnedProvider                             */
/* ---------------------------------------------------------------------------*/
