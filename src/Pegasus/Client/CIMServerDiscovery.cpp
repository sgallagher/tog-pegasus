//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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

#include "CIMServerDiscovery.h"

#ifdef PEGASUS_SLP_CLIENT_INTERFACE_WRAPPER
# include "PegasusSLPWrapper.cpp"
#elif OPENSLP_SLP_CLIENT_INTERFACE_WRAPPER
# include "OpenSLPWrapper.cpp"
#else
// No interface wrapper selected
PEGASUS_NAMESPACE_BEGIN
CIMServerDiscoveryRep::CIMServerDiscoveryRep()
{
}

CIMServerDiscoveryRep::~CIMServerDiscoveryRep()
{
}

Array<CIMServerDescription>
CIMServerDiscoveryRep::lookup(const Array<Attribute> & criteria)
{
  Array<CIMServerDescription> connections;
  return connections;
}

PEGASUS_NAMESPACE_END
#endif

PEGASUS_NAMESPACE_BEGIN

/////////////////////////////////////////////////////////////////////////////
// CIMServerDiscovery
/////////////////////////////////////////////////////////////////////////////
CIMServerDiscovery::CIMServerDiscovery()
:_rep(new CIMServerDiscoveryRep())
{
}

CIMServerDiscovery::~CIMServerDiscovery()
{
  if (_rep != NULL)
    delete _rep;
  _rep = NULL;
}

Array<CIMServerDescription>
CIMServerDiscovery::lookup()
{
  Array<Attribute> criteria;
  return _rep->lookup(criteria);
}

Array<CIMServerDescription>
CIMServerDiscovery::lookup(const Array<Attribute> & criteria)
{
  return _rep->lookup(criteria);
}

PEGASUS_NAMESPACE_END
