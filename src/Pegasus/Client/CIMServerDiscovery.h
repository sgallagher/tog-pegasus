//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2003 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
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
// Author: Tony Fiorentino (fiorentino_tony@emc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////
#ifndef Pegasus_CIMServerDiscovery_h
#define Pegasus_CIMServerDiscovery_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Client/Attribute.h>
#include <Pegasus/Client/CIMServerDescription.h>
#include <Pegasus/Client/Linkage.h>

PEGASUS_NAMESPACE_BEGIN


///////////////////////////////////////////////////////////////////////////////
// CIMServerDiscoveryRep
///////////////////////////////////////////////////////////////////////////////
class CIMServerDiscoveryRep
{
public:

  CIMServerDiscoveryRep();
  ~CIMServerDiscoveryRep();

  Array<CIMServerDescription> lookup(const Array<Attribute> & criteria);

};

/** This class provides the interface that a client uses to discover
    CIM Servers
*/
class PEGASUS_CLIENT_LINKAGE CIMServerDiscovery
{
public:

  /** Constructor for a CIMServerDiscovery object.
  */
  CIMServerDiscovery();

  /** Destructor for a CIMServerDiscovery object
  */
  ~CIMServerDiscovery();

  /** Lookup all WBEM servers
      @return  the return value is an array of connection descriptions.
  */
  Array<CIMServerDescription> lookup();

  /** Lookup specific WBEM servers
      @param   criteria   Criteria for specifing which CIM Servers to lookup.
      @return  the return value is array of connection descriptions.
  */
  Array<CIMServerDescription> lookup(const Array<Attribute> & criteria);

private:

  CIMServerDiscoveryRep* _rep;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_CIMServerDiscovery_h */



