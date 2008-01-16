//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
//%/////////////////////////////////////////////////////////////////////////////

// Please be aware that the CMPI C++ API is NOT a standard currently.

#ifndef TESTCMPI_CXX_H
#define TESTCMPI_CXX_H

#include <Pegasus/Provider/CMPI/cmpidt.h>
#include <Pegasus/Provider/CMPI/CmpiArray.h>
#include <Pegasus/Provider/CMPI/CmpiBooleanData.h>
#include <Pegasus/Provider/CMPI/CmpiBroker.h>
#include <Pegasus/Provider/CMPI/CmpiData.h>
#include <Pegasus/Provider/CMPI/CmpiDateTime.h>
#include <Pegasus/Provider/CMPI/CmpiEnumeration.h>
#include <Pegasus/Provider/CMPI/CmpiInstance.h>
#include <Pegasus/Provider/CMPI/CmpiObject.h>
#include <Pegasus/Provider/CMPI/CmpiObjectPath.h>
#include <Pegasus/Provider/CMPI/CmpiString.h>
#include <Pegasus/Provider/CMPI/CmpiStatus.h>

#include <Pegasus/Provider/CMPI/CmpiInstanceMI.h>
#include <Pegasus/Provider/CMPI/CmpiMethodMI.h>

class TestCMPI_CXX : public CmpiInstanceMI
{
 public:
  
                     TestCMPI_CXX      (const CmpiBroker&     mbp,
                                        const CmpiContext&    ctx);

  virtual           ~TestCMPI_CXX      ();
    
  virtual CmpiStatus initialize        (const CmpiContext&    ctx);
  virtual int        isUnloadable      () const;
    
  virtual CmpiStatus enumInstanceNames (const CmpiContext&    ctx,
                                        CmpiResult&           rslt,
                                        const CmpiObjectPath& cop);
     
  virtual CmpiStatus enumInstances     (const CmpiContext&    ctx,
                                        CmpiResult&           rslt,
                                        const CmpiObjectPath& cop,
                                        const char*          *properties);
  
  virtual CmpiStatus getInstance       (const CmpiContext&    ctx,
                                        CmpiResult&           rslt,
                                        const CmpiObjectPath& cop,
                                        const char*          *properties);
  
  virtual CmpiStatus createInstance    (const CmpiContext&    ctx,
                                        CmpiResult&           rslt,
                                        const CmpiObjectPath& cop,
                                        const CmpiInstance&   inst);
  
  virtual CmpiStatus setInstance       (const CmpiContext&    ctx,
                                        CmpiResult&           rslt,
                                        const CmpiObjectPath& cop,
                                        const CmpiInstance&   inst,
                                        const char*          *properties);
  
  virtual CmpiStatus deleteInstance    (const CmpiContext&    ctx,
                                        CmpiResult&           rslt,
                                        const CmpiObjectPath& cop);

private:
  CmpiBroker cppBroker;
};

#endif
