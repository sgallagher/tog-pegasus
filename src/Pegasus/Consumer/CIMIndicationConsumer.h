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
