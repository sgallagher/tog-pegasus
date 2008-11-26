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

#ifndef Pegasus_CIMIndicationConsumer_h
#define Pegasus_CIMIndicationConsumer_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/OperationContext.h>

PEGASUS_NAMESPACE_BEGIN

/**
    This class defines the set of methods implemented by an indication
    consumer.  A provider that derives from this class must implement all
    methods. A minimal method implementation may simply throw a
    CIMNotSupportedException.
*/
class CIMIndicationConsumer
{
public:
    /**
        Constructs a default CIMIndicationConsumer object.
    */
    CIMIndicationConsumer() { };

    /**
        Destructs a CIMIndicationConsumer object.
    */
    virtual ~CIMIndicationConsumer() { };

    /**
        Delivers an indication for consumption.
        @param context contains security and locale information relevant for
            the lifetime of this operation.
        @param destinationPath A String containing the path suffix from the
            destination URI.  The path in the destination URI is expected to
            begin with "/CIMListener".  This parameter value contains the
            remainder of the path.
        @param indicationInstance A CIMInstance containing the indication
            being delivered.
    */
    virtual void consumeIndication(
        const OperationContext& context,
        const String& destinationPath,
        const CIMInstance& indicationInstance) = 0;
};

PEGASUS_NAMESPACE_END

#endif
