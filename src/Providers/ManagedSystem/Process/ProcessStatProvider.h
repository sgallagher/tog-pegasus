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
// Author: Christopher Neufeld <neufeld@linuxcare.com>
//         David Kennedy       <dkennedy@linuxcare.com>
//
// Modified By:
//         David Kennedy       <dkennedy@linuxcare.com>
//         Christopher Neufeld <neufeld@linuxcare.com>
//         Al Stone, Hewlett-Packard Company <ahs3@fc.hp.com>
//         Jim Metcalfe, Hewlett-Packard Company
//         Carlos Bonilla, Hewlett-Packard Company
//         Mike Glantz, Hewlett-Packard Company <michael_glantz@hp.com>
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//
//%////////////////////////////////////////////////////////////////////////////

#ifndef PG_PROCESS_STAT_PROVIDER_H
#define PG_PROCESS_STAT_PROVIDER_H

/* ==========================================================================
   Includes.
   ========================================================================== */

#include "ProcessPlatform.h"

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;


class ProcessStatProvider : public CIMInstanceProvider
{

public:

  ProcessStatProvider();

  ~ProcessStatProvider();

  void createInstance(const OperationContext       &context,
                    const CIMObjectPath           &instanceName,
                    const CIMInstance            &instanceObject,
                    ObjectPathResponseHandler &handler);

  void deleteInstance(const OperationContext       &context,
                    const CIMObjectPath           &instanceReference,
                    ResponseHandler &handler);

  void enumerateInstances(
	const OperationContext & context,
	const CIMObjectPath & classReference,
	const Boolean includeQualifiers,
	const Boolean includeClassOrigin,
	const CIMPropertyList & propertyList,
	InstanceResponseHandler & handler);

  void enumerateInstanceNames(const OperationContext        &context,
                            const CIMObjectPath            &ref,
                            ObjectPathResponseHandler &handler);

  void getInstance(const OperationContext       &context,
                 const CIMObjectPath           &instanceName,
                 const Boolean                 includeQualifiers,
                 const Boolean                 includeClassOrigin,
                 const CIMPropertyList        &propertyList,
                 InstanceResponseHandler &handler);

  void modifyInstance(const OperationContext       &context,
                    const CIMObjectPath           &instanceName,
                    const CIMInstance            &instanceObject,
                    const Boolean                includeQualifiers,
		    const CIMPropertyList        &propertyList,
                    ResponseHandler &handler);

  void initialize(CIMOMHandle&);

  void terminate(void);

private:

  // Please see comments in ProcessProvider.h for information
  // on these private functions and data members

  CIMOMHandle _cimomHandle;

  Array<CIMKeyBinding> _constructKeyBindings(const Process&);

  CIMInstance _constructInstance(const CIMName&, const CIMNamespaceName&, 
      const Process&);

  void _checkClass(CIMName&);

  String &_getCSName(void);

  String &_getOSName(void);

  String _hostName;

  String _osName;
};


#endif  /* #ifndef PG_PROCESS_STAT_PROVIDER_H */
