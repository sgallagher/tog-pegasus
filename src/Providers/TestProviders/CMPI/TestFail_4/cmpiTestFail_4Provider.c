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
static const CMPIBroker *_broker;

const char *_msg =  "&quot;&lt;MSGS&gt;&lt;MSG COMP=&quot;CIMVM&quot; RC=&quot;400&quot; RS=&quot;65&quot;" \
					" MSGID=&quot;DNZZMC465E&quot; COMM AND=&quot;Transaction: CimVm::CbCProvisioningService::createUser; Subtask:" \
					" Process disk settings&quot;&gt;Fehler beim Erstellen eines virtuellen Computersystems "\
					" (Aktion wurde rückgängig gemacht).&#13;&lt;/MSG&gt;&lt;MSG COMP=&quot;CIMVM&quot; "\
					" RC=&quot;200&quot; RS=&quot;5&quot; MSGID=&quot;DNZZMC205E&quot; "\
					" ROOTCAUSE=&quot;true&quot;&gt;&amp;quot;22500&amp;quot; ist keine gültige "\
					" Einheitennummer.&#13;&lt;/MSG&gt;&lt;/MSGS&gt;&quot;";

CMPIStatus
TestCMPIFail_4ProviderCleanup (CMPIInstanceMI * mi, const CMPIContext * ctx,
                                 CMPIBoolean  term)
{

  CMReturn (CMPI_RC_OK);
}

CMPIStatus
TestCMPIFail_4ProviderEnumInstanceNames (CMPIInstanceMI * mi,
                                           const CMPIContext * ctx,
                                           const CMPIResult * rslt,
                                           const CMPIObjectPath * ref)
{
  CMReturnWithChars(_broker, CMPI_RC_ERR_NOT_FOUND, _msg);
}

CMPIStatus
TestCMPIFail_4ProviderEnumInstances (CMPIInstanceMI * mi,
                                       const CMPIContext * ctx,
                                       const CMPIResult * rslt,
                                       const CMPIObjectPath * ref,
                                       const char **properties)
{
  CMReturnWithChars(_broker, CMPI_RC_ERR_NOT_FOUND, _msg);
}

CMPIStatus
TestCMPIFail_4ProviderGetInstance (CMPIInstanceMI * mi,
                                     const CMPIContext * ctx,
                                     const CMPIResult * rslt,
                                     const CMPIObjectPath * cop,
                                     const char **properties)
{
  CMReturnWithChars(_broker, CMPI_RC_ERR_NOT_FOUND, _msg);
}

CMPIStatus
TestCMPIFail_4ProviderCreateInstance (CMPIInstanceMI * mi,
                                        const CMPIContext * ctx,
                                        const CMPIResult * rslt,
                                        const CMPIObjectPath * cop,
                                        const CMPIInstance * ci)
{
  CMReturnWithChars(_broker, CMPI_RC_ERR_NOT_FOUND, _msg);
}

CMPIStatus
TestCMPIFail_4ProviderModifyInstance (CMPIInstanceMI * mi,
                                        const CMPIContext * ctx,
                                        const CMPIResult * rslt,
                                        const CMPIObjectPath * cop,
                                        const CMPIInstance * ci,
                                        const char **properties)
{
  CMReturnWithChars(_broker, CMPI_RC_ERR_NOT_FOUND, _msg);
}

CMPIStatus
TestCMPIFail_4ProviderDeleteInstance (CMPIInstanceMI * mi,
                                        const CMPIContext * ctx,
                                        const CMPIResult * rslt,
                                        const CMPIObjectPath * cop)
{
  CMReturnWithChars(_broker, CMPI_RC_ERR_NOT_FOUND, _msg);
}

CMPIStatus
TestCMPIFail_4ProviderExecQuery (CMPIInstanceMI * mi,
                                   const CMPIContext * ctx,
                                   const CMPIResult * rslt,
                                   const CMPIObjectPath * ref,
                                   const char *lang, const char *query)
{
  CMReturnWithChars(_broker, CMPI_RC_ERR_NOT_FOUND, _msg);
}

/* ---------------------------------------------------------------------------*/
/*                              Provider Factory                              */
/* ---------------------------------------------------------------------------*/

CMInstanceMIStub (TestCMPIFail_4Provider,
                  TestCMPIFail_4Provider, _broker, CMNoHook);


/* ---------------------------------------------------------------------------*/
/*             end of TestCMPIProvider                      */
/* ---------------------------------------------------------------------------*/

