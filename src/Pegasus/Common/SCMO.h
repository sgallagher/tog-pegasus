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

#ifndef _SCMO_H_
#define _SCMO_H_


#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Linkage.h>
#include <Pegasus/Common/CIMDateTimeRep.h>
#include <Pegasus/Common/CIMType.h>
#include <Pegasus/Common/AtomicInt.h>
#include <Pegasus/Common/CIMQualifierList.h>
#include <Pegasus/Common/CIMObjectPath.h>

PEGASUS_NAMESPACE_BEGIN

//  Constants defining the size of the hash table used in the PropertySet
//  implementation.

#define PEGASUS_PROPERTY_SCMB_HASHSIZE 64
#define PEGASUS_KEYBINDIG_SCMB_HASHSIZE 32

class SCMOClass;

// The enum of return values for SCMO functions.
enum SCMO_RC
{
    SCMO_OK = 0,
    SCMO_NULL_VALUE,
    SCMO_NOT_FOUND,
    SCMO_INDEX_OUT_OF_BOUND,
    SCMO_NOT_SAME_ORIGIN,
    SCMO_INVALID_PARAMETER,
    SCMO_TYPE_MISSMATCH,
    SCMO_WRONG_TYPE,
    SCMO_NOT_AN_ARRAY,
    SCMO_IS_AN_ARRAY
};

// This the definition of a relative pointer for objects stored in
// the single chunk memory. The memory is handled as array of chars.
struct SCMBDataPtr
{
    // start index of the data area
    Uint64      start;
    // length of data area
    Uint64      length;
};

//
// This enumeration and string array defines the qualifier names
// defined by the DMTF (standardized and optional ).
//
// The value QUALNAME_USERDEFINED indicates that the qualifier is
// user defined and the qualifier name must be specified.
//
enum QualifierNameEnum
{
    QUALNAME_USERDEFINED=0,
    QUALNAME_ABSTRACT,
    QUALNAME_AGGREGATE,
    QUALNAME_AGGREGATION,
    QUALNAME_ALIAS,
    QUALNAME_ARRAYTYPE,
    QUALNAME_ASSOCIATION,
    QUALNAME_BITMAP,
    QUALNAME_BITVALUES,
    QUALNAME_CLASSCONSTRAINT,
    QUALNAME_COMPOSITION,
    QUALNAME_CORRELATABLE,
    QUALNAME_COUNTER,
    QUALNAME_DELETE,
    QUALNAME_DEPRECATED,
    QUALNAME_DESCRIPTION,
    QUALNAME_DISPLAYDESCRIPTION,
    QUALNAME_DISPLAYNAME,
    QUALNAME_DN,
    QUALNAME_EMBEDDEDINSTANCE,
    QUALNAME_EMBEDDEDOBJECT,
    QUALNAME_EXCEPTION,
    QUALNAME_EXPENSIVE,
    QUALNAME_EXPERIMENTAL,
    QUALNAME_GAUGE,
    QUALNAME_IFDELETE,
    QUALNAME_IN,
    QUALNAME_INDICATION,
    QUALNAME_INVISIBLE,
    QUALNAME_ISPUNIT,
    QUALNAME_KEY,
    QUALNAME_LARGE,
    QUALNAME_MAPPINGSTRINGS,
    QUALNAME_MAX,
    QUALNAME_MAXLEN,
    QUALNAME_MAXVALUE,
    QUALNAME_METHODCONSTRAINT,
    QUALNAME_MIN,
    QUALNAME_MINLEN,
    QUALNAME_MINVALUE,
    QUALNAME_MODELCORRESPONDENCE,
    QUALNAME_NONLOCAL,
    QUALNAME_NONLOCALTYPE,
    QUALNAME_NULLVALUE,
    QUALNAME_OCTETSTRING,
    QUALNAME_OUT,
    QUALNAME_OVERRIDE,
    QUALNAME_PROPAGATED,
    QUALNAME_PROPERTYCONSTRAINT,
    QUALNAME_PROPERTYUSAGE,
    QUALNAME_PROVIDER,
    QUALNAME_PUNIT,
    QUALNAME_READ,
    QUALNAME_REQUIRED,
    QUALNAME_REVISION,
    QUALNAME_SCHEMA,
    QUALNAME_SOURCE,
    QUALNAME_SOURCETYPE,
    QUALNAME_STATIC,
    QUALNAME_SYNTAX,
    QUALNAME_SYNTAXTYPE,
    QUALNAME_TERMINAL,
    QUALNAME_TRIGGERTYPE,
    QUALNAME_UMLPACKAGEPATH,
    QUALNAME_UNITS,
    QUALNAME_UNKNOWNVALUES,
    QUALNAME_UNSUPPORTEDVALUES,
    QUALNAME_VALUEMAP,
    QUALNAME_VALUES,
    QUALNAME_VERSION,
    QUALNAME_WEEK,
    QUALNAME_WRITE
};

typedef CIMDateTimeRep SCMBDateTime;

//
// This union is used to represent the values of properties, qualifiers,
// method return values, and method arguments.
//
// This union is only used for simple CIMTypes.
// If the CIMType is an Array or String the union contains the start index of
// this value with in the SCMB memory block.
//
// For EmbeddedObjects and EmbeddedInstances the union
// contains an external pointer.
//
union SCMBUnion
{
    Boolean     _booleanValue;
    Uint8       _uint8Value;
    Sint8       _sint8Value;
    Uint16      _uint16Value;
    Sint16      _sint16Value;
    Uint32      _uint32Value;
    Sint32      _sint32Value;
    Uint64      _uint64Value;
    Sint64      _sint64Value;
    Real32      _real32Value;
    Real64      _real64Value;
    Uint16      _char16Value;
    // A relative pointer to SCMBInstance_Main
    SCMBDataPtr _referenceValue;
    SCMBDataPtr _arrayValue;
    SCMBDataPtr _stringValue;
    SCMBDateTime _dateTimeValue;
    // used for embedded instances and objects.
    void*       _voidPtr;
};

struct SCMBValue
{
    // The CIMType of the value
    CIMType         valueType;

    struct{
        // If the value not set
        unsigned isNull:1;
        // If value is a type array
        unsigned isArray:1;
        // If value is set by the provider ( valid for SCMOInstance )
        unsigned isSet:1;
    } flags;

    // The number of elements if the value is a type array.
    Uint32          valueArraySize;

    SCMBUnion       value;
};

struct SCMBQualifier
{
    //Boolean flag
    Sint32              propagated;
    QualifierNameEnum   name;
    // The same value as CIMFlavor.
    Uint32              flavor;
    // if name == QUALNAME_USERDEFINED
    // the ralative pointer to the user defined name
    SCMBDataPtr     userDefName;
    // Qualifier Value
    SCMBValue       value;
};

struct SCMBMgmt_Header
{
    // The magic number for a SCMB memory object
    Uint32          magic;
    // Total size of the SCMB memory block( # bytes )
    Uint64          totalSize;
    // The # of bytes avaialable in the dynamic area of SCMB memory block.
    Uint64          freeBytes;
    // Index to the start of the free space in this SCMB memory block.
    Uint64          startOfFreeSpace;
};

//
// The SCMB design needs the capability to access the properties via property
// name and index.
//
// Therefore the OrderedSet implementation is mapped into SCMB class properties
// and key bindigs.
//

struct SCMBClassProperty
{
    // Relative pointer to property name
    SCMBDataPtr     name;
    Uint32          nameHashTag;
    // Flags
    struct{
        unsigned propagated:1;
        unsigned isKey:1;
    } flags;
    // Relative pointer to the origin class name
    SCMBDataPtr     originClassName;
    // Relative pointer to the reference class name
    SCMBDataPtr     refClassName;
    // Default value
    SCMBValue       defaultValue;
    // Number of qualifiers in the array
    Uint32          numberOfQualifiers;
    // Relative pointer to SCMBQualifierArray
    SCMBDataPtr     qualifierArray;
};

struct SCMBClassPropertySet_Header
{
    // Number of property nodes in the nodeArray
    Uint32          number;
    // Used for name based lookups based on the name tags.
    // A tag is generated by entangling the bit values of a first
    // and last letter of a CIMName.
    // PEGASUS_PROPERTY_SCMB_HASHSIZE is the hash size.
    // The index of the hashTable is calculated doing a remainder operator
    // with the name tag and hash size PEGASUS_PROPERTY_SCMB_HASHSIZE
    // (NameTag % PEGASUS_PROPERTY_SCMB_HASHSIZE)
    // The hashTable contains the index of the SCMBClassPropertyNode
    // in the nodeArray.
    Uint32  hashTable[PEGASUS_PROPERTY_SCMB_HASHSIZE];
    // Relative pointer to the ClassPropertyNodeArray;
    SCMBDataPtr     nodeArray;
};

struct SCMBClassPropertyNode
{
    // Is there a next node in the hash chain?
    Sint32          hasNext;
    // Array index of next property in hash chain.
    Uint32          nextNode;
    // The class property
    SCMBClassProperty theProperty;
};

struct SCMBKeyBindingNode
{
    // Is there a next node in the hash chain?
    Sint32          hasNext;
    // Array index of next property in hash chain.
    Uint32          nextNode;
    // Relativer pointer to the key property name.
    SCMBDataPtr     name;
    Uint32          nameHashTag;
    // Key binding type.
    CIMKeyBinding::Type type;
};

struct SCMBKeyBindingSet_Header
{

    // Number of keybindings in the keyBindingNodeArray.
    Uint32          number;
    // Used for name based lookups based on the name tags
    // A tag is generated by entangling the bit values of a first
    // and last letter of a CIMName.
    // PEGASUS_KEYBINDIG_SCMB_HASHSIZE is the hash size.
    // The index of the hashTable is calculated doing a remainder operator
    // with the name tag and hash size PEGASUS_KEYBINDIG_SCMB_HASHSIZE
    // (NameTag % PEGASUS_KEYBINDIG_SCMB_HASHSIZE)
    // The hashTable contains the index of the SCMBKeyBindingNode
    // in the keyBindingNodeArray.
    Uint32          hashTable[PEGASUS_KEYBINDIG_SCMB_HASHSIZE];
    // Relative pointer to an array of SCMBKeyBindingNode.
    SCMBDataPtr     nodeArray;
};

struct SCMBClass_Main
{
    // The SCMB management header
    SCMBMgmt_Header     header;
    // The reference counter for this class
    AtomicInt       refCount;
    // SuperClassName
    SCMBDataPtr     superClassName;
    // Relative pointer to classname
    SCMBDataPtr     className;
    // Relative pointer to namespace
    SCMBDataPtr     nameSpace;
    // The key properties of this class are identified
    // by a SCMBKeyPropertyMask
    SCMBDataPtr     keyPropertyMask;
    // A list of index to the key propertis in the property set node array.
    SCMBDataPtr     keyIndexList;
    // Keybinding orderd set
    SCMBKeyBindingSet_Header keyBindingSet;
    // A set containing the class properties.
    SCMBClassPropertySet_Header    propertySet;
    // Relative pointer to SCMBQualifierArray
    Uint32          numberOfQualifiers;
    SCMBDataPtr     qualifierArray;

};

//
// This is the static header information of a SCMB instance.
// It also holds a reference to the SCMB representation of the class describing
// the instance.
//

struct SCMBInstance_Main
{
    // The SCMB management header
    SCMBMgmt_Header     header;
    // The reference counter for this instance
    AtomicInt           refCount;
    // A absolute pointer/reference to the class SCMB for this instance
    SCMOClass*          theClass;
    // Instance flags
    struct{
      unsigned includeQualifiers  :1;
      unsigned includeClassOrigin :1;
      unsigned isFiltered:1;
    }flags;
    // For a full object path is build using the name space and class name
    // of the referenced class.
    // Relative pointer to hostname
    SCMBDataPtr     hostName;
    // Number of key bindings of the instance
    Uint32          numberKeyBindings;
    // Relative pointer to SCMOInstanceKeyBindingArray
    SCMBDataPtr     keyBindingArray;
    // Relative pointer to SCMBPropertyFilter
    SCMBDataPtr     propertyFilter;
    // Relative pointer to SCMBPropertyFilterIndexMap
    SCMBDataPtr     propertyFilterIndexMap;
    // Number of properties of the class.
    Uint32          numberProperties;
    // Number of filter properties of the instance.
    Uint32          filterProperties;
    // Relative pointer to SCMBInstancePropertyArray
    SCMBDataPtr     propertyArray;
};

// The index of an instance key binding is the same as the class.
// If a key binding has to be find by name, the class key binding ordered set
// has to be used to find the right index.
// The key bindings of an instances contains only the value as strings.
typedef SCMBDataPtr SCMOInstanceKeyBindingArray[];

// The index of an instance property is the same as the class property.
// If a property has to be find by name, the class property ordered set
// has to be used to find the right index.
// The properties of an instances contains only the values provider.
// If a instance property does not contain a value
// the default value of the class has to be used.
typedef SCMBValue   SCMBInstancePropertyArray[];

// It is a bit mask to filter properties from the instance.
// The index of a property in the property set is eqal to the index with in
// the property filter.
// If a bit is not set (0), the property is filtered out then it cannot be set
// and is not eligible to be returned to requester.
// If a bit is set (1) then a property is eligible to be set and returned
// to the requester.
// The array size of the property filter is (numberOfProperties / 64)
typedef Uint64        SCMBPropertyFilter[];

// It is a bit mask to identify key properties of an instance or class.
// The index of a key property in the property set is eqal to the index with in
// the key property mask.
// If a bit is not set (0) then it is not a key property.
// If a bit is set (1) then it is a key property.
// The key property maks is overruling the property filter. That means if a
// property is a key property it cannot be filtered out.
// The array size of the key property mask is (numberOfProperties / 64)
typedef Uint64        SCMBKeyPropertyMask[];

// If a filer is applied to this the instance (flag.isFiltered=1)
// the SCMBPropertyFilterIndexMap array is used to ensure that one
// can iterate streight  from 0 to filterProperties.
typedef Uint32      SCMBPropertyFilterIndexMap[];

// This is the definition of a detached instance property.
struct SCMBProperty_Main
{
    // The SCMB management header
    SCMBMgmt_Header     header;
    // The property name
    SCMBDataPtr     name;
    // The value of this property
    SCMBValue       value;
};

// The static definition of the common SCMO memory functions

static Uint64 _getFreeSpace(
    SCMBDataPtr& ptr,
    Uint64 size,
    SCMBMgmt_Header** pmem,
    Boolean clear = false);

static void _setString(
    const String& theString,
    SCMBDataPtr& ptr,
    SCMBMgmt_Header** pmem);

static void _setBinary(
    const void* theBuffer,
    Uint64 bufferSize,
    SCMBDataPtr& ptr,
    SCMBMgmt_Header** pmem);

static const void* _resolveDataPtr(
    const SCMBDataPtr& ptr,
    char* base);

static const char* _getCharString(
    const SCMBDataPtr& ptr,
    char* base);

static Uint32 _generateStringTag(const char* str, Uint32 len);


static Uint32 _generateSCMOStringTag(
    const SCMBDataPtr& ptr,
    char* base);

static CIMKeyBinding::Type _cimTypeToKeyBindType(CIMType cimType);

static Boolean _equalUTF8Strings(
    const SCMBDataPtr& ptr_a,
    char* base,
    const char* name,
    Uint32 len);


static Boolean _equalNoCaseUTF8Strings(
    const SCMBDataPtr& ptr_a,
    char* base,
    const char* name,
    Uint32 len);

PEGASUS_NAMESPACE_END


#endif
