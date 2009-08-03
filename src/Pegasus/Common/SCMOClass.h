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

#ifndef _SCMOCLASS_H_
#define _SCMOCLASS_H_


#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Linkage.h>
#include <Pegasus/Common/SCMO.h>
#include <Pegasus/Common/CIMClass.h>

PEGASUS_NAMESPACE_BEGIN


#define PEGASUS_SCMB_CLASS_MAGIC 0xF00FABCD

class PEGASUS_COMMON_LINKAGE SCMOClass
{
public:

    /**
     * Constructs a SCMOClass out of a CIMClass.
     * @param theCIMClass The source the SCMOClass is constucted off.
     * @return
     */
    SCMOClass(CIMClass& theCIMClass );

    /**
     * Copy constructor for the SCMO class, used to implement refcounting.
     * @param theSCMOClass The class for which to create a copy
     * @return
     */
    SCMOClass(const SCMOClass& theSCMOClass )
    {
        cls.hdr = theSCMOClass.cls.hdr;
        Ref();
    }

    /**
     * Destructor is decrementing the refcount. If refcount is zero, the
     * singele chunk memory object is deallocated.
     */
    ~SCMOClass()
    {
        Unref();
    }

    /**
     * Gets the key property names as a string array
     * @return An Array of String objects containing the names of the key
     * properties.
     */
    void getKeyNamesAsString(Array<String>& keyNames) const;

    /**
     * Determines whether the object has been initialized.
     * @return True if the object has not been initialized, false otherwise.
     */
    Boolean isUninitialized( ) const {return (cls.base == NULL); };

private:

    void Ref()
    {
        cls.hdr->refCount++;
        // printf("\ncls.hdr->refCount=%u\n",cls.hdr->refCount.get());
    };

    void Unref()
    {
        if (cls.hdr->refCount.decAndTestIfZero())
        {
            // printf("\ncls.hdr->refCount=%u\n",cls.hdr->refCount.get());
            free(cls.base);
            cls.base=NULL;
        }
        else
        {
            // printf("\ncls.hdr->refCount=%u\n",cls.hdr->refCount.get());
        }

    };

    /**
     * Constructs an uninitialized SCMOClass object.
     */
    SCMOClass();

    SCMO_RC _getProperyNodeIndex(Uint32& node, const char* name) const;
    SCMO_RC _getKeyBindingNodeIndex(Uint32& node, const char* name) const;

    void _setClassQualifers(CIMClass& theCIMClass);
    QualifierNameEnum  _setQualifier(
        Uint64 start,
        const CIMQualifier& theCIMQualifier);

    void _setClassProperties(CIMClass& theCIMClass);
    void _setProperty(Uint64 start,
                         Boolean* isKey,
                         const CIMProperty& theCIMProperty);
    Boolean _setPropertyQualifiers(
        Uint64 start,
        const CIMQualifierList& theQualifierList);

    void _setClassKeyBinding(Uint64 start, const CIMProperty& theCIMProperty);
    void _insertPropertyIntoOrderedSet(Uint64 start, Uint32 newIndex);
    void _insertKeyBindingIntoOrderedSet(Uint64 start, Uint32 newIndex);
    void _clearKeyPropertyMask();
    void _setPropertyAsKeyInMask(Uint32 i);
    Boolean _isPropertyKey(Uint32 i);

    void _setValue(Uint64 start, const CIMValue& theCIMValue);
    void _setUnionValue(Uint64 start, CIMType type, Union& u);
    void _setArrayValue(Uint64 start, CIMType type, Union& u);
    QualifierNameEnum _getSCMOQualifierNameEnum(const CIMName& theCIMname);
    Boolean _isSamePropOrigin(Uint32 node, const char* origin) const;

    inline SCMO_RC _isNodeSameType(
        Uint32 node,
        CIMType type, 
        Boolean isArray) const;

    union{
        // To access the class main structure
        SCMBClass_Main *hdr;
        // To access the memory management header
        SCMBMgmt_Header  *mem;
        // Generic access pointer
        char *base;
    }cls;

    friend class SCMOInstance;
    friend class SCMODump;

};


PEGASUS_NAMESPACE_END

#endif
