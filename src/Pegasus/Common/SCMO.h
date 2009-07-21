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
#include <Pegasus/Common/AtomicInt.h>
#include <Pegasus/Common/CIMType.h>
#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/CIMQualifierList.h>
#include <Pegasus/Common/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

//  Constants defining the size of the hash table used in the PropertySet
//  implementation.

#define PEGASUS_PROPERTY_SCMB_HASHSIZE 64
#define PEGASUS_KEYBINDIG_SCMB_HASHSIZE 32

#define PEGASUS_SCMB_CLASS_MAGIC 0xF00FABCD
#define PEGASUS_SCMB_INSTANCE_MAGIC 0xD00D1234

class SCMOClass;
class SCMODump;

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

struct SCMBDateTime
{
    // Number of microseconds elapsed since January 1, 1 BCE.
    Uint64 usec;

    // UTC offset
    Uint32 utcOffset;

    // ':' for intervals. '-' or '+' for time stamps.
    Uint16 sign;

    // Number of wild characters ('*') used to initialize this object.
    Uint16 numWildcards;
};

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
    SCMO_RC _isNodeSameType(Uint32 node,CIMType type, Boolean isArray) const;

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

class PEGASUS_COMMON_LINKAGE SCMOInstance
{
public:

    /**
     * Creating a SCMOInstance using a SCMOClass.
     * @param baseClass A SCMOClass.
     */
    SCMOInstance(SCMOClass baseClass);

    /**
     * Copy constructor for the SCMO instance, used to implement refcounting.
     * @param theSCMOClass The instance for which to create a copy
     * @return
     */
    SCMOInstance(const SCMOInstance& theSCMOInstance )
    {
        inst.hdr = theSCMOInstance.inst.hdr;
        Ref();
    }

    /**
     * Destructor is decrementing the refcount. If refcount is zero, the
     * singele chunk memory object is deallocated.
     */
    ~SCMOInstance()
    {
        Unref();
    }

    /**
     * Builds a SCMOInstance based on this SCMOClass.
     * The method arguments determine whether qualifiers are included,
     * the class origin attributes are included,
     * and which properties are included in the new instance.
     * @param baseClass The SCMOClass of this instance.
     * @param includeQualifiers A Boolean indicating whether qualifiers in
     * the class definition (and its properties) are to be added to the
     * instance.  The TOINSTANCE flavor is ignored.
     * @param includeClassOrigin A Boolean indicating whether ClassOrigin
     * attributes are to be added to the instance.
     * @param propertyList Is an NULL terminated array of char* to property
     * names defining the properties that are included in the created instance.
     * If the propertyList is NULL, all properties are included to the instance.
     * If the propertyList is empty, no properties are added.
     *
     * Note that this function does NOT generate an error if a property name
     * is supplied that is NOT in the class;
     * it simply does not add that property to the instance.
     *
     */
    SCMOInstance(
        SCMOClass baseClass,
        Boolean includeQualifiers,
        Boolean includeClassOrigin,
        const char** propertyList);

    /**
     * Builds a SCMOInstance from the given SCMOClass and copies all
     * CIMInstance data into the new SCMOInstance.
     * @param baseClass The SCMOClass of this instance.
     * @param cimInstance A CIMInstace of the same class.
     */
    SCMOInstance(SCMOClass baseClass, const CIMInstance& cimInstance);

    /**
     * Converts the SCMOInstance into a CIMInstance.
     * It is a deep copy of the SCMOInstance into the CIMInstance.
     * @param cimInstance An empty CIMInstance.
     */
    void getCIMInstance(CIMInstance& cimInstance) const;

    /**
     * Makes a deep copy of the instance.
     * This creates a new copy of the instance.
     * @return A new copy of the SCMOInstance object.
     */
    SCMOInstance clone() const;

    /**
     * Returns the number of properties of the instance.
     * @param Number of properties
     */
    Uint32 getPropertyCount() const;

    /**
     * Gets the property name, type, and value addressed by a positional index.
     * The property name and value has to be copied by the caller !
     * @param pos The positional index of the property
     * @param pname Returns the property name as '\0' terminated string.
     *              Has to be copied by caller.
     *              It is set to NULL if rc != SCMO_OK.
     * @param pvalue Returns a pointer to the value of property.
     *               The value has to be copied by the caller !
     *               It returns NULL if rc != SCMO_OK.
     *               If the value is an array, the
     *               value array is stored in continuous memory.
     *               e.g. If the CIMType is CIMTYPE_UINT32:
     *               value = (void*)Uint32[0 to size-1]
     *               If it is an array of CIMTYPE_STRING, an array
     *               of char* to the string values is returned.
     *               This array has to be freed by the caller !
     * @param type Returns the CIMType of the property
     *             It is invalid if rc == SCMO_INDEX_OUT_OF_BOUND.
     * @param isArray Returns if the value is an array.
     *             It is invalid if rc == SCMO_INDEX_OUT_OF_BOUND.
     * @param size Returns the size of the array.
     *             If it is not an array, 0 is returned.
     *             It is invalid if rc == SCMO_INDEX_OUT_OF_BOUND.
     *
     * @return     SCMO_OK
     *             SCMO_NULL_VALUE : The value is a null value.
     *             SCMO_INDEX_OUT_OF_BOUND : Given index not found
     *
     */
    SCMO_RC getPropertyAt(
        Uint32 pos,
        const char** pname,
        CIMType& type,
        const void** pvalue,
        Boolean& isArray,
        Uint32& size ) const;

    /**
     * Gets the type and value of the named property.
     * The value has to be copied by the caller !
     * @param name The property name
     * @param pvalue Returns a pointer to the value of property.
     *               The value has to be copied by the caller !
     *               It returns NULL if rc != SCMO_OK.
     *               If the value is an array, the
     *               value array is stored in continuous memory.
     *               e.g. If the CIMType is CIMTYPE_UINT32:
     *               value = (void*)Uint32[0 to size-1]
     *               If it is an array of CIMTYPE_STRING, an array
     *               of char* to the string values is returned.
     *               This array has to be freed by the caller !
     * @param type Returns the CIMType of the property
     *             It is invalid if rc == SCMO_NOT_FOUND.
     * @param isArray Returns if the value is an array.
     *             It is invalid if rc == SCMO_NOT_FOUND.
     * @param size Returns the size of the array.
     *             If it is not an array, 0 is returned.
     *             It is invalid if rc == SCMO_NOT_FOUND.
     *
     * @return     SCMO_OK
     *             SCMO_NULL_VALUE : The value is a null value.
     *             SCMO_NOT_FOUND : Given property name not found.
     */
    SCMO_RC getProperty(
        const char* name,
        CIMType& type,
        const void** pvalue,
        Boolean& isArray,
        Uint32& size ) const;

    /**
     * Set/replace a property in the instance.
     * If the class origin is specified, it is honored at identifying
     * the property within the instance.
     * Note: Only properties which are already part of the instance/class can
     * be set/replaced.
     * @param name The name of the property to be set.
     * @param type The CIMType of the property
     * @param value A pointer to property  value.
     *         The value is copied into the instance
     *         If the value == NULL, a null value is assumed.
     *         If the value is an array, the
     *         value array must be stored in continuous memory.
     *         e.g. If the CIMType is CIMTYPE_UINT32:
     *         value = (void*)Uint32[0 to size-1]
     * @param isArray Indicate that the value is an array. Default false.
     * @param size Returns the size of the array. If not an array this
     *         this parameter is ignorer. Default 0.
     * @param origin The class originality of the property.
     *               If NULL, then it is ignorred. Default NULL.
     * @return     SCMO_OK
     *             SCMO_NOT_SAME_ORIGIN : The property name was found, but
     *                                    the origin was not the same.
     *             SCMO_NOT_FOUND : Given property name not found.
     *             SCMO_WRONG_TYPE : Named property has the wrong type.
     *             SCMO_NOT_AN_ARRAY : Named property is not an array.
     *             SCMO_IS_AN_ARRAY  : Named property is an array.
     */
    SCMO_RC setPropertyWithOrigin(
        const char* name,
        CIMType type,
        void* value,
        Boolean isArray=false,
        Uint32 size = 0,
        const char* origin = NULL);

    /**
     * Rebuilds the key bindings off the property values
     * if no or incomplete key properties are set on the instance.
     */
    void rebuildKeyProperties();

    /**
     * Set/replace a property filter on an instance.
     * The filter is a white list of property names.
     * A property part of the list can be accessed by name or index and
     * is eligible to be returned to requester.
     * Key properties can not be filtered. They are always a part of the
     * instance. If a key property is not part of the property list,
     * it will not be filtered out.
     * @param propertyList Is an NULL terminated array of char* to
     * property names
     */
    void setPropertyFilter(const char **propertyList);

    /**
     * Gets the hash index for the named property.
     * @param theName The property name
     * @param pos Returns the hash index.
     * @return     SCMO_OK
     *             SCMO_INVALID_PARAMETER: name was a NULL pointer.
     *             SCMO_NOT_FOUND : Given property name not found.
     */
    SCMO_RC getPropertyNodeIndex(const char* name, Uint32& pos) const;

    /**
     * Set/replace a property in the instance at node index.
     * @param index The node index.
     * @param type The CIMType of the property
     * @param value A pointer to property  value.
     *         The value is copied into the instance
     *         If the value is an array, the
     *         value array must be stored in continuous memory.
     *         e.g. If the CIMType is CIMTYPE_UINT32:
     *         value = (void*)Uint32[0 to size-1]
     * @param isArray Indicate that the value is an array. Default false.
     * @param size The size of the array. If not an array this
     *         this parameter is ignorer. Default 0.
     * @return     SCMO_OK
     *             SCMO_INDEX_OUT_OF_BOUND : Given index not found
     *             SCMO_NOT_FOUND : The property at given node index
     *                              is not set due to filtering.
     *             SCMO_WRONG_TYPE : The property at given node index 
     *                               has the wrong type.
     *             SCMO_NOT_AN_ARRAY : The property at given node index 
     *                                 is not an array.
     *             SCMO_IS_AN_ARRAY  : The property at given node index 
     *                                 is an array.
     */
    SCMO_RC setPropertyWithNodeIndex(
        Uint32 node,
        CIMType type,
        void* value,
        Boolean isArray=false,
        Uint32 size = 0);

    /**
     * Set/replace the named key binding
     * @param name The key binding name.
     * @param type The type as CIMKeyBinding::Type.
     * @parma value The value as string.
     * @return     SCMO_OK
     *             SCMO_TYPE_MISSMATCH : Given type does not
     *                                   match to key binding type
     *             SCMO_NOT_FOUND : Given property name not found.
     */
    SCMO_RC setKeyBinding(
        const char* name,
        CIMKeyBinding::Type type,
        const char* pvalue);

    /**
     * Gets the key binding count.
     * @return the number of key bindings set.
     */
    Uint32 getKeyBindingCount();

    /**
     * Get the indexed key binding.
     * @parm idx The key bining index
     * @parm pname Returns the name.
     *             Has to be copied by caller.
     *             It is invalid if rc == SCMO_INDEX_OUT_OF_BOUND.
     * @param type Returns the type as CIMKeyBinding::Type.
     *             It is invalid if rc == SCMO_INDEX_OUT_OF_BOUND.
     * @parma pvalue Returns the value as string.
     *             Has to be copied by caller.
     *             It is only valid if rc == SCMO_OK.
     * @return     SCMO_OK
     *             SCMO_NULL_VALUE : The key binding is not set.
     *             SCMO_INDEX_OUT_OF_BOUND : Given index not found
     *
     */
    SCMO_RC getKeyBindingAt(
        Uint32 idx,
        const char** pname,
        CIMKeyBinding::Type& type,
        const char** pvalue) const;

    /**
     * Get the named key binding.
     * @parm name The name of the key binding.
     * @param type Returns the type as CIMKeyBinding::Type.
     *             It is invalid if rc == SCMO_INDEX_OUT_OF_BOUND.
     * @parma value Returns the value as string.
     *             Has to be copied by caller.
     *             It is only valid if rc == SCMO_OK.
     * @return     SCMO_OK
     *             SCMO_NULL_VALUE : The key binding is not set.
     *             SCMO_NOT_FOUND : Given property name not found.
     */
    SCMO_RC getKeyBinding(
        const char* name,
        CIMKeyBinding::Type& ptype,
        const char** pvalue) const;

    /**
     * Determines whether the object has been initialized.
     * @return True if the object has not been initialized, false otherwise.
     */
    Boolean isUninitialized( ) const {return (inst.base == NULL); };

    /**
     * Determies if two objects are referencing to the same instance
     * @return True if the objects are referencing to the some instance.
     */
    Boolean isSame(SCMOInstance& theInstance) const;

    /**
     * Get the host name of the instance. The caller has to make a copy !
     * @return The host name as UTF8.
     */
    const char* getHostName() const;

    /**
     * Sets the provided host name at the instance.
     * @param hostName The host name as UTF8.
     */
    void setHostName(const char* hostName);

    /**
     * Get the class name of the instance. The cabler has to make a copy !
     * @return The class name as UTF8.
     */
    const char* getClassName() const;

    /**
     * Get the name space of the instance. The caller has to make a copy !
     * @return The name space as UTF8.
     */
    const char* getNameSpace() const;


private:

    void Ref()
    {
        inst.hdr->refCount++;
        // printf("\ninst.hdr->refCount=%u\n",inst.hdr->refCount.get());
    };

    void Unref()
    {
        if (inst.hdr->refCount.decAndTestIfZero())
        {
            // printf("\ninst.hdr->refCount=%u\n",inst.hdr->refCount.get());
            // The class has also be dereferenced.
            delete inst.hdr->theClass;
            free(inst.base);
            inst.base=NULL;
        }
        else
        {
            // printf("\ninst.hdr->refCount=%u\n",inst.hdr->refCount.get());
        }

    };
    /**
     * A SCMOInstance can only be created by a SCMOClass
     */
    SCMOInstance();

    void _initSCMOInstance(
        SCMOClass* pClass,
        Boolean inclQual,
        Boolean inclOrigin);


    SCMO_RC _getPropertyAtNodeIndex(
            Uint32 pos,
            const char** pname,
            CIMType& type,
            const void** pvalue,
            Boolean& isArray,
            Uint32& size ) const;

    void _setPropertyAtNodeIndex(
        Uint32 pos,
        CIMType type,
        void* value,
        Boolean isArray,
        Uint32 size);

    void* _getSCMBUnion(
        CIMType type,
        Boolean isArray,
        Uint32 size,
        Uint64 start,
        char* base) const;

    void _setSCMBUnion(
        void* value,
        CIMType type,
        Boolean isArray,
        Uint32 size,
        Uint64 start);

    SCMO_RC _getKeyBindingAtNodeIndex(
        Uint32 pos,
        const char** pname,
        CIMKeyBinding::Type& ptype,
        const char** pvalue) const;

    Uint32 _initPropFilterWithKeys();

    void _setPropertyInPropertyFilter(Uint32 i);

    Boolean _isPropertyInFilter(Uint32 i) const;

    void _clearPropertyFilter();

    union{
        // To access the instance main structure
        SCMBInstance_Main *hdr;
        // To access the memory management header
        SCMBMgmt_Header     *mem;
        // Generic access pointer
        char *base;
    }inst;

    friend class SCMOClass;
    friend class SCMODump;
};

// The definiton of a dump class for SCMO 
class PEGASUS_COMMON_LINKAGE SCMODump
{

public:

    // Methods for SCMOClass only
    void hexDumpSCMOClass(SCMOClass& testCls) const;
    void dumpSCMOClass(SCMOClass& testCls) const;
    void dumpSCMOClassQualifiers(SCMOClass& testCls) const;
    void dumpKeyPropertyMask(SCMOClass& testCls) const;
    void dumpClassProperties(SCMOClass& testCls) const;
    void dumpKeyIndexList(SCMOClass& testCls) const;
    void dumpClassPropertyNodeArray(SCMOClass& testCls) const;
    void dumpKeyBindingSet(SCMOClass& testCls) const;
    void dumpClassKeyBindingNodeArray(SCMOClass& testCls) const;

    // Methods for SCMOClass and SCMOInstance
    void dumpHashTable(Uint32* hashTable,Uint32 size)const;

    void printSCMOValue(
        const SCMBValue& theValue,
        char* base) const;

    String printArrayValue(
        CIMType type,
        Uint32 size,
        SCMBUnion u,
        char* base) const;

    String printUnionValue(
        CIMType type, 
        SCMBUnion u,
        char* base) const;

private:

    // context sensitive methods
    void _dumpQualifierArray(
        Uint64 start, 
        Uint32 size,
        char* clsbase) const;

    void _dumpQualifier(
        const SCMBQualifier& theQualifier,
        char* clsbase) const;

    void _dumpClassProperty(
        const SCMBClassProperty& prop,
        char* clsbase) const;

   void _hexDump(char* buffer,int length) const;
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

PEGASUS_NAMESPACE_END


#endif
