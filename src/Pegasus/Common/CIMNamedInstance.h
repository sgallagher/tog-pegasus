//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett-Packard Company, IBM,
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
// Author: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_CIMNamedInstance_h
#define Pegasus_CIMNamedInstance_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMObjectPath.h>
#include <Pegasus/Common/CIMInstance.h>

PEGASUS_NAMESPACE_BEGIN

class CIMNamedInstanceRep;

/** The CIMNamedInstance encapsulates a CIMObjectPath and CIMInstance.
    Accessors are provided for getting the two parts. Constructors are
    provided for initializing it from a CIMNamedInstance and from the
    two individual parts.
*/
class PEGASUS_COMMON_LINKAGE CIMNamedInstance
{
public:

    /**	Constructor
    */
    CIMNamedInstance();

    /**
        Constructs a CIMNamedInstance object from an instance name and an
        instance.

        @param instanceName The CIMObjectPath object containing the instance
                            name.
        @param instance The CIMInstance object containing the instance.
    */
    CIMNamedInstance(
        const CIMObjectPath& instanceName,
        const CIMInstance& instance
    );

    /**
        Constructs a CIMNamedInstance object from another CIMNamedInstance.

        @param namedInstance The CIMNamedInstance to copy.
    */
    CIMNamedInstance(const CIMNamedInstance& namedInstance);

    /**
        Destructor
    */
    ~CIMNamedInstance();

    /**
        Resets the value of the CIMNamedInstance object to contain the
        specified named instance.

        @param namedInstance The CIMNamedInstance object containing the
                             named instance to set.
    */
    CIMNamedInstance& operator=(const CIMNamedInstance& namedInstance);

    /**
        Resets the value of the CIMNamedInstance object to contain the
        specified instance name and instance.

        @param instanceName The CIMObjectPath object containing the instance
                            name to set.
        @param instance The CIMInstance object containing the instance to set.
    */
    void set(const CIMObjectPath& instanceName, const CIMInstance& instance);

    /**
        Accessor for the instance name portion of the CIMNamedInstance object.

        @return A CIMObjectPath object containing the instance name.
    */
    const CIMObjectPath& getInstanceName() const;

    /**
        Accessor for the instance portion of the CIMNamedInstance object.

        @return A CIMInstance object containing the instance.
    */
    const CIMInstance& getInstance() const;

    /**
        Accessor for the instance name portion of the CIMNamedInstance object.

        @return A CIMObjectPath object containing the instance name.
    */
    CIMObjectPath& getInstanceName();

    /**
        Accessor for the instance portion of the CIMNamedInstance object.

        @return A CIMInstance object containing the instance.
    */
    CIMInstance& getInstance();

    /** Makes a deep copy (clone) of the given object. */
    CIMNamedInstance clone() const;

private:

    CIMNamedInstanceRep* _rep;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_CIMNamedInstance_h */
