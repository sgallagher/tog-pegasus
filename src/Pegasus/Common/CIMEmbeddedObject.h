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
// Author: karl Schopmeyer
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_EmbeddedObject_h
#define Pegasus_EmbeddedObject_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Linkage.h>
#include <Pegasus/Common/CIMInstance.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_COMMON_LINKAGE CIMEmbeddedObject
{

public:
/** Decodes a String containing XML that represents an embedded Object.
    This function assumes that only instances are defined in the embedded
    object XML string and therefore decodes to an instance.
    @param String defining the XML for the object
    @return CIMInstance decoded from the input String.
    @exceptions XmlValidationError - if the decoder does not validate
    an instance object.

    <p><b>Example:</b>
    <pre>
        // ... CIMProperty p - property with embedded object String
        String embeddedObjectString;
        CIMvalue v =  p.getvalue;
        String embeddedObjectString;
        if (v.getType() ==
        v.getValue(embeddedObjectString);
        try
        {
        CIMEmbeddedObject::decodeEmbeddedObject(embeddedObjectString;
        }
        catch (Exception& e)
        {
            cout << "Exception: " << e.getMessage() << endl;
            exit(1);
        }
    </pre>
*/

static CIMInstance decodeEmbeddedObject(const String& xmlStr);

/** encodes a CIMInstance into the a String representing a CIMCompliant embedded object.
    This funcition can be used by any component needing to encapsulate an embedded object
    into a String to become part of the DMTF compliant lifecycle indication.
    @param CIMInstance containing the instance to be encoded.
    @return String containing the object encoded.
    @exceptions UnitializedObjectException - If the instance was not initialized before
    the encoder is called

    <p><b>Example:</b>
    <pre>
        CIMInstance i1;
        // build instance somehow.
        String embeddedObjectString =
            CIMEmbeddedObject::encodeToEmbeddedObject(i1);
    </pre>
*/

static String encodeToEmbeddedObject (const CIMInstance& instance);
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_EmbeddedObject_h */
