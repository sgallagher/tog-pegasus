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

#include <Pegasus/Common/SCMO.h>
#include <Pegasus/Common/SCMOClass.h>
#include <Pegasus/Common/SCMOInstance.h>
#include <Pegasus/Common/SCMODump.h>
#include <Pegasus/Common/CharSet.h>
#include <Pegasus/Common/CIMDateTimeRep.h>
#include <Pegasus/Common/CIMPropertyRep.h>
#include <Pegasus/Common/CIMInstanceRep.h>
#include <Pegasus/Common/CIMObjectPathRep.h>
#include <Pegasus/Common/CIMNameCast.h>
#include <Pegasus/Common/CommonUTF.h>
#include <Pegasus/Common/StrLit.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/StringConversion.h>
#include <Pegasus/Common/ArrayIterator.h>
#include <strings.h>

#ifdef PEGASUS_OS_ZOS
  #include <Pegasus/General/SetFileDescriptorToEBCDICEncoding.h>
#endif

PEGASUS_USING_STD;

#define SCMB_INITIAL_MEMORY_CHUNK_SIZE 4096

/**
 * This macro is used at the SCMODump class
 * for generating C/C++ runtime independend output.
 * For example on Linux if fprintf got a NULL pointer
 * for a string format specification, the string "(null)" is
 * substituted. On other platforms no string "" is substituded.
 */
#define NULLSTR(x) ((x) == NULL ? "" : (x))

#define NEWCIMSTR(ptr,base) \
      ((ptr).length == 0 ?  \
      (String()) :           \
      (String(&(base)[(ptr).start],((ptr).length)-1)))

PEGASUS_NAMESPACE_BEGIN

const StrLit SCMOClass::_qualifierNameStrLit[72] =
{
    STRLIT(""),
    STRLIT("ABSTRACT"),
    STRLIT("AGGREGATE"),
    STRLIT("AGGREGATION"),
    STRLIT("ALIAS"),
    STRLIT("ARRAYTYPE"),
    STRLIT("ASSOCIATION"),
    STRLIT("BITMAP"),
    STRLIT("BITVALUES"),
    STRLIT("CLASSCONSTRAINT"),
    STRLIT("COMPOSITION"),
    STRLIT("CORRELATABLE"),
    STRLIT("COUNTER"),
    STRLIT("DELETE"),
    STRLIT("DEPRECATED"),
    STRLIT("DESCRIPTION"),
    STRLIT("DISPLAYDESCRIPTION"),
    STRLIT("DISPLAYNAME"),
    STRLIT("DN"),
    STRLIT("EMBEDDEDINSTANCE"),
    STRLIT("EMBEDDEDOBJECT"),
    STRLIT("EXCEPTION"),
    STRLIT("EXPENSIVE"),
    STRLIT("EXPERIMENTAL"),
    STRLIT("GAUGE"),
    STRLIT("IFDELETED"),
    STRLIT("IN"),
    STRLIT("INDICATION"),
    STRLIT("INVISIBLE"),
    STRLIT("ISPUNIT"),
    STRLIT("KEY"),
    STRLIT("LARGE"),
    STRLIT("MAPPINGSTRINGS"),
    STRLIT("MAX"),
    STRLIT("MAXLEN"),
    STRLIT("MAXVALUE"),
    STRLIT("METHODCONSTRAINT"),
    STRLIT("MIN"),
    STRLIT("MINLEN"),
    STRLIT("MINVALUE"),
    STRLIT("MODELCORRESPONDENCE"),
    STRLIT("NONLOCAL"),
    STRLIT("NONLOCALTYPE"),
    STRLIT("NULLVALUE"),
    STRLIT("OCTETSTRING"),
    STRLIT("OUT"),
    STRLIT("OVERRIDE"),
    STRLIT("PROPAGATED"),
    STRLIT("PROPERTYCONSTRAINT"),
    STRLIT("PROPERTYUSAGE"),
    STRLIT("PROVIDER"),
    STRLIT("PUNIT"),
    STRLIT("READ"),
    STRLIT("REQUIRED"),
    STRLIT("REVISION"),
    STRLIT("SCHEMA"),
    STRLIT("SOURCE"),
    STRLIT("SOURCETYPE"),
    STRLIT("STATIC"),
    STRLIT("SYNTAX"),
    STRLIT("SYNTAXTYPE"),
    STRLIT("TERMINAL"),
    STRLIT("TRIGGERTYPE"),
    STRLIT("UMLPACKAGEPATH"),
    STRLIT("UNITS"),
    STRLIT("UNKNOWNVALUES"),
    STRLIT("UNSUPPORTEDVALUES"),
    STRLIT("VALUEMAP"),
    STRLIT("VALUES"),
    STRLIT("VERSION"),
    STRLIT("WEAK"),
    STRLIT("WRITE")
};

#define _NUM_QUALIFIER_NAMES \
           (sizeof(_qualifierNameStrLit)/sizeof(_qualifierNameStrLit[0]))

/*****************************************************************************
 * The SCMOClass methods
 *****************************************************************************/
SCMOClass::SCMOClass()
{
    cls.mem = NULL;
}

SCMOClass::SCMOClass(
    CIMClass& theCIMClass,
    const char* nameSpaceName)
{
    PEGASUS_ASSERT(SCMB_INITIAL_MEMORY_CHUNK_SIZE
        - sizeof(SCMBClass_Main)>0);

    cls.base = (char*)malloc(SCMB_INITIAL_MEMORY_CHUNK_SIZE);
    if (cls.base == NULL)
    {
        // Not enough memory!
        throw PEGASUS_STD(bad_alloc)();
    }

    // initalize eye catcher
    cls.hdr->header.magic=PEGASUS_SCMB_CLASS_MAGIC;
    cls.hdr->header.totalSize=SCMB_INITIAL_MEMORY_CHUNK_SIZE;
    // The # of bytes free
    cls.hdr->header.freeBytes=
        SCMB_INITIAL_MEMORY_CHUNK_SIZE-sizeof(SCMBClass_Main);

    // Index to the start of the free space in this instance
    cls.hdr->header.startOfFreeSpace=sizeof(SCMBClass_Main);

    cls.hdr->refCount=1;

    try
    {
        _setString(theCIMClass.getSuperClassName().getString(),
                   cls.hdr->superClassName,
                   &cls.mem );
    }
    catch (UninitializedObjectException&)
    {
        // there is no Super ClassName
        cls.hdr->superClassName.start=0;
        cls.hdr->superClassName.length=0;
    }

    CIMObjectPath theObjectPath=theCIMClass.getPath();

    //set name space
    if (nameSpaceName)
    {
        _setBinary(nameSpaceName,
                   strlen(nameSpaceName)+1,
                   cls.hdr->nameSpace,
                   &cls.mem );
    }
    else
    {
        _setString(theObjectPath.getNameSpace().getString(),
                  cls.hdr->nameSpace,
                  &cls.mem );
    }

    //set class name
    _setString(theObjectPath.getClassName().getString(),
               cls.hdr->className,
               &cls.mem );

    //set class Qualifiers
    _setClassQualifers(theCIMClass);

    //set properties
    _setClassProperties(theCIMClass);

}

void SCMOClass::getKeyNamesAsString(Array<String>& keyNames) const
{
    SCMBKeyBindingNode* nodeArray =
    (SCMBKeyBindingNode*)&(cls.base[cls.hdr->keyBindingSet.nodeArray.start]);

    keyNames.clear();

    for (Uint32 i = 0, k = cls.hdr->propertySet.number; i < k; i++)
    {
        // Append the key property name.
        // The length has to be reduces by 1 not to copy the trailing '\0'
        keyNames.append(
            String((const char*)_getCharString(nodeArray[i].name,cls.base),
                   nodeArray[i].name.length-1));

    }
}

const char* SCMOClass::_getPropertyNameAtNode(Uint32 propNode) const
{
    SCMBClassPropertyNode* nodeArray =
        (SCMBClassPropertyNode*)
            &(cls.base[cls.hdr->propertySet.nodeArray.start]);

    return(_getCharString(nodeArray[propNode].theProperty.name,cls.base));
}

SCMO_RC SCMOClass::_getKeyBindingNodeIndex(Uint32& node, const char* name) const
{

    Uint32 tag,len,hashIdx;

    len = strlen(name);
    tag = _generateStringTag((const char*)name, len);
    // get the node index of the hash table
    hashIdx =
        cls.hdr->keyBindingSet.hashTable[tag%PEGASUS_KEYBINDIG_SCMB_HASHSIZE];
    // there is no entry in the hash table on this hash table index.
    if (hashIdx == 0)
    {
        // property name not found
        return SCMO_NOT_FOUND;
    }

    // get the propterty node array
    SCMBKeyBindingNode* nodeArray =
        (SCMBKeyBindingNode*)
            &(cls.base[cls.hdr->keyBindingSet.nodeArray.start]);

    // set property node array index.
    // The hash table index to the node arra in one larger!
    node = hashIdx - 1;

    do
    {
        // do they have the same hash tag ?
        if (nodeArray[node].nameHashTag == tag)
        {
            // Now it is worth to compare the two names
            if (_equalUTF8Strings(
                nodeArray[node].name,cls.base,name,len))
            {
                // we found the property !
                return SCMO_OK;
            }
        }
        // Are we at the end of the chain ?
        if (!nodeArray[node].hasNext)
        {
            // property name not found
            return SCMO_NOT_FOUND;
        }

        // get next node index.
        node = nodeArray[node].nextNode;

    } while ( true );

    // this should never be reached
    return SCMO_NOT_FOUND;

}

SCMO_RC SCMOClass::_getProperyNodeIndex(Uint32& node, const char* name) const
{

    Uint32 tag,len,hashIdx;

    len = strlen(name);
    tag = _generateStringTag((const char*)name, len);
    // get the node index of the hash table
    hashIdx =
        cls.hdr->propertySet.hashTable[tag%PEGASUS_PROPERTY_SCMB_HASHSIZE];
    // there is no entry in the hash table on this hash table index.
    if (hashIdx == 0)
    {
        // property name not found
        return SCMO_NOT_FOUND;
    }

    // get the propterty node array
    SCMBClassPropertyNode* nodeArray =
        (SCMBClassPropertyNode*)
            &(cls.base[cls.hdr->propertySet.nodeArray.start]);

    // set property node array index.
    // The hash table index to the node arra in one larger!
    node = hashIdx - 1;

    do
    {
        // do they have the same hash tag ?
        if (nodeArray[node].theProperty.nameHashTag == tag)
        {
            // Now it is worth to compare the two names
            if (_equalUTF8Strings(
                nodeArray[node].theProperty.name,cls.base,name,len))
            {
                // we found the property !
                return SCMO_OK;
            }
        }
        // Are we at the end of the chain ?
        if (!nodeArray[node].hasNext)
        {
            // property name not found
            return SCMO_NOT_FOUND;
        }

        // get next node index.
        node = nodeArray[node].nextNode;

    } while ( true );

    // this should never be reached
    return SCMO_NOT_FOUND;

}

void SCMOClass::_setClassProperties(CIMClass& theCIMClass)
{
    Uint32 noProps = theCIMClass.getPropertyCount();
    Uint64 start, startKeyIndexList;
    Uint32 noKeys = 0;
    Boolean isKey = false;

    Uint32 keyIndex[noProps];

    cls.hdr->propertySet.number=noProps;

    // allocate the keyIndexList
    startKeyIndexList = _getFreeSpace(
        cls.hdr->keyIndexList,
        noProps*sizeof(Uint32),
        &cls.mem,
        true);

    if(noProps != 0)
    {
        // Allocate key property mask.
        // Each property needs a bit in the mask.
        // The number of Uint64 in the key mask is :
        // Decrease the number of properties by 1 since
        // the array is starting at 0!
        // Divided with the number of bits in a Uint64.
        // e.g. number of Properties = 68
        // (68 - 1) / 64 = 1 --> The mask consists of two Uint64 values.
        _getFreeSpace(cls.hdr->keyPropertyMask,
              sizeof(Uint64)*(((noProps-1)/64)+1),
              &cls.mem);

        // allocate property array and save the start index of the array.
        start = _getFreeSpace(cls.hdr->propertySet.nodeArray,
                      sizeof(SCMBClassPropertyNode)*noProps,
                      &cls.mem);

        // clear the hash table
        memset(cls.hdr->propertySet.hashTable,
               0,
               PEGASUS_PROPERTY_SCMB_HASHSIZE*sizeof(Uint32));

        _clearKeyPropertyMask();

        for (Uint32 i = 0; i < noProps; i++)
        {

            _setProperty(start,&isKey ,theCIMClass.getProperty(i));
            if(isKey)
            {
                // if the property is a key
                // save the key index
                keyIndex[noKeys]=i;
                noKeys++;
                _setPropertyAsKeyInMask(i);
            }
            // Adjust ordered set management structures.
            _insertPropertyIntoOrderedSet(start,i);

            start = start + sizeof(SCMBClassPropertyNode);
        }

        cls.hdr->keyBindingSet.number = noKeys;

        if (noKeys != 0)
        {
            // allocate key binding array and save the start index of the array.
            start = _getFreeSpace(cls.hdr->keyBindingSet.nodeArray,
                          sizeof(SCMBKeyBindingNode)*noKeys,
                          &cls.mem);

            // clear the hash table
            memset(cls.hdr->keyBindingSet.hashTable,
                   0,
                   PEGASUS_KEYBINDIG_SCMB_HASHSIZE*sizeof(Uint32));

            // fill the key index list
            memcpy(
                &(cls.base[startKeyIndexList]),
                keyIndex,
                noKeys*sizeof(Uint32));

            for (Uint32 i = 0 ; i < noKeys; i++)
            {

                _setClassKeyBinding(start,theCIMClass.getProperty(keyIndex[i]));
                // Adjust ordered set management structures.
                _insertKeyBindingIntoOrderedSet(start,i);

                start = start + sizeof(SCMBKeyBindingNode);
            }

        }
        else
        {
            cls.hdr->keyBindingSet.nodeArray.start=0;
            cls.hdr->keyBindingSet.nodeArray.length=0;
        }
    }
    else
    {
        cls.hdr->propertySet.nodeArray.start=0;
        cls.hdr->propertySet.nodeArray.length=0;
        cls.hdr->keyPropertyMask.start=0;
        cls.hdr->keyPropertyMask.length=0;
        cls.hdr->keyBindingSet.nodeArray.start=0;
        cls.hdr->keyBindingSet.nodeArray.length=0;
    }
}

void SCMOClass::_insertKeyBindingIntoOrderedSet(Uint64 start, Uint32 newIndex)
{

    SCMBKeyBindingNode* newKeyNode =
        (SCMBKeyBindingNode*)&(cls.base[start]);

    SCMBKeyBindingNode* scmoKeyNodeArray =
        (SCMBKeyBindingNode*)
            &(cls.base[cls.hdr->keyBindingSet.nodeArray.start]);

    Uint32 *hashTable = cls.hdr->keyBindingSet.hashTable;

    if ( newIndex >= cls.hdr->keyBindingSet.number)
    {
        throw IndexOutOfBoundsException();
    }

    // calculate the new hash index of the new property.
    Uint32 hash = newKeyNode->nameHashTag % PEGASUS_KEYBINDIG_SCMB_HASHSIZE;

    // 0 is an invalid index in the hash table
    if (hashTable[hash] == 0)
    {
        hashTable[hash] = newIndex + 1;
        return;
    }

    // get the first node of the hash chain.
    Uint32 nodeIndex = hashTable[hash]-1;

    do
    {
        // is this the same note ?
        if (nodeIndex == newIndex)
        {
            // The node is already in the ordered set
            return;
        }

        // Are we at the end of the chain ?
        if (!scmoKeyNodeArray[nodeIndex].hasNext)
        {
            // link the new element at the end of the chain
            scmoKeyNodeArray[nodeIndex].nextNode = newIndex;
            scmoKeyNodeArray[nodeIndex].hasNext = true;
            return;
        }

        nodeIndex = scmoKeyNodeArray[nodeIndex].nextNode;

    } while ( true );

    // this should never be reached
}

void SCMOClass::_insertPropertyIntoOrderedSet(Uint64 start, Uint32 newIndex)
{


    SCMBClassPropertyNode* newPropNode =
        (SCMBClassPropertyNode*)&(cls.base[start]);

    SCMBClassPropertyNode* scmoPropNodeArray =
        (SCMBClassPropertyNode*)
             &(cls.base[cls.hdr->propertySet.nodeArray.start]);

    Uint32 *hashTable = cls.hdr->propertySet.hashTable;

    if ( newIndex >= cls.hdr->propertySet.number)
    {
        throw IndexOutOfBoundsException();
    }

    // calcuate the new hash index of the new property.
    Uint32 hash = newPropNode->theProperty.nameHashTag %
        PEGASUS_PROPERTY_SCMB_HASHSIZE;

    // 0 is an invalid index in the hash table
    if (hashTable[hash] == 0)
    {
        hashTable[hash] = newIndex + 1;
        return;
    }

    // get the first node of the hash chain.
    Uint32 nodeIndex = hashTable[hash]-1;

    do
    {
        // is this the same note ?
        if (nodeIndex == newIndex)
        {
            // The node is already in the ordered set
            return;
        }

        // Are we at the end of the chain ?
        if (!scmoPropNodeArray[nodeIndex].hasNext)
        {
            // link the new element at the end of the chain
            scmoPropNodeArray[nodeIndex].nextNode = newIndex;
            scmoPropNodeArray[nodeIndex].hasNext = true;
            return;
        }

        // get next node index.
        nodeIndex = scmoPropNodeArray[nodeIndex].nextNode;

    } while ( true );

    // this should never be reached
}

void SCMOClass::_setClassKeyBinding(
    Uint64 start,
    const CIMProperty& theCIMProperty)
{
    CIMPropertyRep* propRep = theCIMProperty._rep;
    Uint64 valueStart;

    // First do all _setString(). Can cause reallocation.
    _setString(propRep->_name.getString(),
        ((SCMBKeyBindingNode*)&(cls.base[start]))->name,
        &cls.mem);

    SCMBKeyBindingNode* scmoKeyBindNode =
        (SCMBKeyBindingNode*)&(cls.base[start]);

    // calculate the new hash tag
    scmoKeyBindNode->nameHashTag =
        _generateSCMOStringTag(scmoKeyBindNode->name,cls.base);
    scmoKeyBindNode->type = propRep->_value.getType();
    scmoKeyBindNode->hasNext=false;
    scmoKeyBindNode->nextNode=0;

}

void SCMOClass::_clearKeyPropertyMask()
{

    Uint64 *keyMask;

    // Calculate the real pointer to the Uint64 array
    keyMask = (Uint64*)&cls.base[cls.hdr->keyPropertyMask.start];

    // the number of Uint64 in the key mask is :
    // Decrease the number of properties by 1
    // since the array is starting at index 0!
    // Divide with the number of bits in a Uint64.
    // e.g. number of Properties = 68
    // (68 - 1) / 64 = 1 --> The mask consists of 2 Uint64

    memset(keyMask,0, sizeof(Uint64)*(((cls.hdr->propertySet.number-1)/64)+1));

}

void SCMOClass::_setPropertyAsKeyInMask(Uint32 i)
{
    Uint64 *keyMask;

    // In which Uint64 of key mask is the bit for property i ?
    // Divide with the number of bits in a Uint64.
    // 47 / 64 = 0 --> The key bit for property i is in in keyMask[0].
    Uint32 idx = i/64 ;

    // Create a filter to set the bit.
    // Modulo division with 64. Shift left a bit by the remainder.
    Uint64 filter = ( (Uint64)1 << (i%64));

    // Calculate the real pointer to the Uint64 array
    keyMask = (Uint64*)&cls.base[cls.hdr->keyPropertyMask.start];

    keyMask[idx] = keyMask[idx] | filter ;
}

Boolean SCMOClass::_isPropertyKey(Uint32 i)
{
    Uint64 *keyMask;

    // In which Uint64 of key mask is the bit for property i ?
    // Divide with the number of bits in a Uint64.
    // e.g. number of Properties = 68
    // 47 / 64 = 0 --> The key bit for property i is in in keyMask[0].
    Uint32 idx = i/64 ;

    // Create a filter to check if the bit is set:
    // Modulo division with 64. Shift left a bit by the remainder.
    Uint64 filter = ( (Uint64)1 << (i%64));

    // Calculate the real pointer to the Uint64 array
    keyMask = (Uint64*)&cls.base[cls.hdr->keyPropertyMask.start];

    return keyMask[idx] & filter ;

}


void SCMOClass::_setProperty(
    Uint64 start,
    Boolean* isKey,
    const CIMProperty& theCIMProperty)
{

   *isKey = false;

    CIMPropertyRep* propRep = theCIMProperty._rep;

    // First do all _setString(). Can cause reallocation.

    _setString(propRep->_name.getString(),
        ((SCMBClassPropertyNode*)&(cls.base[start]))->theProperty.name,
        &cls.mem);

    _setString(propRep->_classOrigin.getString(),
        ((SCMBClassPropertyNode*)
             &(cls.base[start]))->theProperty.originClassName,
        &cls.mem);

    _setString(propRep->_referenceClassName.getString(),
        ((SCMBClassPropertyNode*)&(cls.base[start]))->theProperty.refClassName,
        &cls.mem);


    SCMBClassPropertyNode* scmoPropNode =
        (SCMBClassPropertyNode*)&(cls.base[start]);

    // generate new hash tag
    scmoPropNode->theProperty.nameHashTag =
        _generateSCMOStringTag(scmoPropNode->theProperty.name,cls.base);

    scmoPropNode->theProperty.flags.propagated = propRep->_propagated;

    // just for complete intialization.
    scmoPropNode->hasNext=false;
    scmoPropNode->nextNode=0;

    // calculate the relative pointer for the default value
    Uint64 valueStart =
        (char*)&scmoPropNode->theProperty.defaultValue - cls.base;

    _setValue(valueStart,propRep->_value);

    *isKey = _setPropertyQualifiers(start,propRep->_qualifiers);
    // reset property node pointer
    scmoPropNode = (SCMBClassPropertyNode*)&(cls.base[start]);
    scmoPropNode->theProperty.flags.isKey=*isKey;
}

Boolean SCMOClass::_setPropertyQualifiers(
    Uint64 start,
    const CIMQualifierList& theQualifierList)
{
    Uint32 noQuali = theQualifierList.getCount();
    Uint64 startArray;
    QualifierNameEnum propName;
    Boolean isKey = false;

    SCMBClassPropertyNode* scmoPropNode =
        (SCMBClassPropertyNode*)&(cls.base[start]);

    scmoPropNode->theProperty.numberOfQualifiers = noQuali;


    if (noQuali != 0)
    {
        // allocate qualifier array and save the start of the array.
        startArray = _getFreeSpace(scmoPropNode->theProperty.qualifierArray,
                         sizeof(SCMBQualifier)*noQuali,
                         &cls.mem);
        for (Uint32 i = 0; i < noQuali; i++)
        {
            propName = _setQualifier(
                startArray,
                theQualifierList.getQualifier(i));
            // Only set once if the property is identified as key.
            if(!isKey)
            {
                isKey = propName==QUALNAME_KEY;
            }

            startArray = startArray + sizeof(SCMBQualifier);
        }
    }
    else
    {
        scmoPropNode->theProperty.qualifierArray.start=0;
        scmoPropNode->theProperty.qualifierArray.length=0;
    }

    return isKey;
}
void SCMOClass::_setClassQualifers(CIMClass& theCIMClass)
{

    Uint32 noQuali = theCIMClass.getQualifierCount();
    Uint64 start;

    cls.hdr->numberOfQualifiers = noQuali;

    if (noQuali != 0)
    {
        // allocate qualifier array and save the start of the array.
        start = _getFreeSpace(cls.hdr->qualifierArray,
                      sizeof(SCMBQualifier)*noQuali,
                      &cls.mem);
        for (Uint32 i = 0; i < noQuali; i++)
        {
            _setQualifier(start,theCIMClass.getQualifier(i));
            start = start + sizeof(SCMBQualifier);

        }

    }
    else
    {
        cls.hdr->qualifierArray.start=0;
        cls.hdr->qualifierArray.length=0;
    }


}

QualifierNameEnum SCMOClass::_setQualifier(
    Uint64 start,
    const CIMQualifier& theCIMQualifier)
{
    Uint64 valueStart;
    QualifierNameEnum name;

    SCMBQualifier* scmoQual = (SCMBQualifier*)&(cls.base[start]);
    name=_getSCMOQualifierNameEnum(theCIMQualifier.getName());

    scmoQual->propagated = theCIMQualifier.getPropagated();
    scmoQual->name = name;
    scmoQual->flavor = theCIMQualifier.getFlavor().cimFlavor;

    valueStart = (char*)&scmoQual->value - cls.base;

    _setValue(valueStart,theCIMQualifier.getValue());

    // reset the pointer, because the base pointer could be reallocated.
    scmoQual = (SCMBQualifier*)&(cls.base[start]);
    if (name == QUALNAME_USERDEFINED)
    {
        _setString(theCIMQualifier.getName().getString(),
                   scmoQual->userDefName,&cls.mem);
    }

    return name;
}

void SCMOClass::_setValue(Uint64 start, const CIMValue& theCIMValue)
{
    Uint64 valueStart;

    CIMValueRep* rep = *((CIMValueRep**)&theCIMValue);

    SCMBValue* scmoValue = (SCMBValue*)&(cls.base[start]);
    scmoValue->valueType = rep->type;

    scmoValue->valueArraySize = 0;
    scmoValue->flags.isNull = rep->isNull;
    scmoValue->flags.isArray = rep->isArray;
    // Only initalized by for completeness.
    scmoValue->flags.isSet = false;

    if (rep->isNull)
    {
        return;
    }

    valueStart = (char*)&scmoValue->value - cls.base;

    if (scmoValue->flags.isArray)
    {
        SCMOInstance::_setUnionArrayValue(
            valueStart,
            &cls.mem,
            rep->type,
            // Is set to the number of array members by the function.
            scmoValue->valueArraySize,
            rep->u);
    }
    else
    {
        _setUnionValue(scmoValue->value,rep->type,rep->u);
    }
}

void SCMOClass::_setUnionValue(
        SCMBUnion& scmoU,
        CIMType type,
        Union& cimU)
{
    switch (type)
    {
    case CIMTYPE_REFERENCE:
        {
            break;
        }
    case CIMTYPE_OBJECT:
        {
            break;
        }
    case CIMTYPE_INSTANCE:
        {
            break;
        }
    default:
        {
            Uint64 valueStart = (char*)&scmoU - cls.base;

            SCMOInstance::_setNonRefUnionValue(
                valueStart,
                &cls.mem,
                type,
                cimU);
            break;
        }
    }

}
QualifierNameEnum SCMOClass::_getSCMOQualifierNameEnum(
    const CIMName& theCIMName)
{
    // Get the UTF8 CString
    CString theCString=theCIMName.getString().getCString();
    // Get the real size of the UTF8 sting.
    Uint32 length = strlen((const char*)theCString);


    // The start index is 1, because the at index 0 is a place holder for
    // the user defined qualifier name which is not part of the qualifier name
    // list.
    for (Uint32 i = 1; i < _NUM_QUALIFIER_NAMES; i++)
    {
        if (qualifierNameStrLit(i).size == length)
        {
            // TBD: Make it more efficent...
            if(String::equalNoCase(
                theCIMName.getString(),
                qualifierNameStrLit(i).str))
            {
                return (QualifierNameEnum)i;
            }
        }
    }

    return QUALNAME_USERDEFINED;
}

Boolean SCMOClass::_isSamePropOrigin(Uint32 node, const char* origin) const
{
   Uint32 len = strlen(origin);

   SCMBClassPropertyNode* nodeArray =
       (SCMBClassPropertyNode*)
           &(cls.base[cls.hdr->propertySet.nodeArray.start]);

   return(_equalUTF8Strings(
       nodeArray[node].theProperty.originClassName,
       cls.base,
       origin,
       len));
}

inline SCMO_RC SCMOClass::_isNodeSameType(
    Uint32 node,
    CIMType type,
    Boolean isArray) const
{
    SCMBClassPropertyNode* nodeArray =
        (SCMBClassPropertyNode*)
            &(cls.base[cls.hdr->propertySet.nodeArray.start]);


    if(nodeArray[node].theProperty.defaultValue.valueType != type)
    {
        return SCMO_WRONG_TYPE;
    }

    if (isArray)
    {
        if (nodeArray[node].theProperty.defaultValue.flags.isArray)
        {
            return SCMO_OK;
        }
        else
        {
            return SCMO_NOT_AN_ARRAY;
        }

    }

    if (nodeArray[node].theProperty.defaultValue.flags.isArray)
    {
        return SCMO_IS_AN_ARRAY;
    }

    return SCMO_OK;

}
/*****************************************************************************
 * The SCMOInstance methods
 *****************************************************************************/

SCMOInstance::SCMOInstance()
{
    inst.base = NULL;
}

SCMOInstance::SCMOInstance(SCMOClass baseClass)
{
    _initSCMOInstance(new SCMOClass(baseClass),false,false);
}

SCMOInstance::SCMOInstance(
    SCMOClass baseClass,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const char** propertyList)
{

    _initSCMOInstance(
        new SCMOClass(baseClass),
        includeQualifiers,
        includeClassOrigin);

    setPropertyFilter(propertyList);

}

SCMOInstance::SCMOInstance(SCMOClass baseClass, const CIMObjectPath& cimObj)
{
    _initSCMOInstance(new SCMOClass(baseClass),false,false);

    _setCIMObjectPath(cimObj);

}

SCMOInstance::SCMOInstance(SCMOClass baseClass, const CIMInstance& cimInstance)
{

    CIMPropertyRep* propRep;
    Uint32 propNode;
    Uint64 valueStart;
    SCMO_RC rc;

    CIMInstanceRep* instRep = cimInstance._rep;
    Boolean hasQualifiers = (instRep->_qualifiers.getCount()>0);
    Boolean hasClassOrigin = false;

    _initSCMOInstance(new SCMOClass(baseClass),hasQualifiers,hasClassOrigin);

    _setCIMObjectPath(instRep->_reference);

    // Copy all properties
    for (Uint32 i = 0, k = instRep->_properties.size(); i < k; i++)
    {
        propRep = instRep->_properties[i]._rep;
        // if not already detected that qualifiers are specified and
        // there are qualifers at that property.
        if (!hasQualifiers && propRep->getQualifierCount() > 0)
        {
            includeQualifiers();
        }
        // if not already detected that class origins are specified and
        // there is a class origin specified at that property.
        if (!hasClassOrigin && !propRep->_classOrigin.isNull())
        {
            includeClassOrigins();
        }

        // get the property node index for the property
        rc = inst.hdr->theClass->_getProperyNodeIndex(
            propNode,
            (const char*)propRep->_name.getString().getCString());

        if (rc != SCMO_OK)
        {
            throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NO_SUCH_PROPERTY,
               propRep->_name.getString());
        }
        rc = inst.hdr->theClass->_isNodeSameType(
                 propNode,
                 propRep->_value._rep->type,
                 propRep->_value._rep->isArray);
        if (rc == SCMO_OK)
        {
            _setCIMValueAtNodeIndex(propNode, propRep->_value._rep);
        }
        else
        {
            throw PEGASUS_CIM_EXCEPTION(CIM_ERR_TYPE_MISMATCH,
               propRep->_name.getString());
        }
    }

}

SCMO_RC SCMOInstance::getCIMInstance(CIMInstance& cimInstance) const
{

    SCMO_RC rc = SCMO_OK;
    Uint32 noProps;
    CIMObjectPath objPath;

    // For better usability define pointers to SCMO Class data structures.
    SCMBClass_Main* clshdr = inst.hdr->theClass->cls.hdr;
    char* clsbase = inst.hdr->theClass->cls.base;

    getCIMObjectPath(objPath);

    cimInstance._rep =  new CIMInstanceRep(objPath);

    if (inst.hdr->flags.includeQualifiers)
    {
        SCMBQualifier* qualiArray =
            (SCMBQualifier*)&(clsbase[clshdr->qualifierArray.start]);

        CIMName qualiName;
        CIMValue theValue;

        Uint32 i, k = clshdr->numberOfQualifiers;

        for ( i = 0 ; i < k ; i++)
        {
            _getCIMValueFromSCMBValue(theValue,qualiArray[i].value,clsbase);

            if (qualiArray[i].name == QUALNAME_USERDEFINED)
            {
                qualiName = NEWCIMSTR(qualiArray[i].userDefName,clsbase);
            }
            else
            {
                qualiName = String(
                    SCMOClass::qualifierNameStrLit(qualiArray[i].name).str,
                    SCMOClass::qualifierNameStrLit(qualiArray[i].name).size);
            }

            cimInstance._rep->_qualifiers.addUnchecked(
                CIMQualifier(
                    qualiName,
                    theValue,
                    qualiArray[i].flavor,
                    qualiArray[i].propagated));
        }
    }

    if (inst.hdr->flags.isFiltered)
    {
        // Get absolut pointer to property filter index map of the instance
        Uint32* propertyFilterIndexMap =
            (Uint32*)&(inst.base[inst.hdr->propertyFilterIndexMap.start]);

        for(Uint32 i = 0, k = inst.hdr->filterProperties; i<k; i++)
        {
            // Get absolut pointer to property filter index map
            // of the instance get the real node index of the property.
            CIMProperty theProperty=_getCIMPropertyAtNodeIndex(
                propertyFilterIndexMap[i]);

            cimInstance._rep->_properties.append(theProperty);
        }

    }
    else
    {
        for(Uint32 i = 0, k = inst.hdr->numberProperties; i<k; i++)
        {
            // no filtering. Counter is node index
            CIMProperty theProperty=_getCIMPropertyAtNodeIndex(i);

            cimInstance._rep->_properties.append(theProperty);
        }

    }

    return rc;
}

void SCMOInstance::getCIMObjectPath(CIMObjectPath& cimObj) const
{
    // For better usability define pointers to SCMO Class data structures.
    SCMBClass_Main* clshdr = inst.hdr->theClass->cls.hdr;
    char* clsbase = inst.hdr->theClass->cls.base;

    // Address the class keybinding information
    SCMBKeyBindingNode* scmoClassArray =
        (SCMBKeyBindingNode*)&clsbase[clshdr->keyBindingSet.nodeArray.start];

    // Address the instance keybinding information
    SCMBKeyBindingValue* scmoInstArray =
        (SCMBKeyBindingValue*)&inst.base[inst.hdr->keyBindingArray.start];

    Uint32 numberKeyBindings = inst.hdr->numberKeyBindings;

    CIMValue theKeyBindingValue;

    for (Uint32 i = 0; i < numberKeyBindings; i ++)
    {
        if (scmoInstArray[i].isSet)
        {
            _getCIMValueFromSCMBUnion(
                theKeyBindingValue,
                scmoClassArray[i].type,
                false, // can never be a null value
                false, // can never be an array
                0,
                scmoInstArray[i].data,
                inst.base);
            cimObj._rep->_keyBindings.append(
                CIMKeyBinding(
                    CIMNameCast(NEWCIMSTR(scmoClassArray[i].name,clsbase)),
                    theKeyBindingValue
                    ));
        }
    }

    cimObj._rep->_host = NEWCIMSTR(inst.hdr->hostName,inst.base);
    cimObj._rep->_nameSpace =
        CIMNamespaceNameCast(NEWCIMSTR(clshdr->nameSpace,clsbase));
    cimObj._rep->_className=CIMNameCast(NEWCIMSTR(clshdr->className,clsbase));
}

CIMProperty SCMOInstance::_getCIMPropertyAtNodeIndex(Uint32 nodeIdx) const
{
    CIMValue theValue;
    CIMProperty retProperty;

    // For better usability define pointers to SCMO Class data structures.
    SCMBClass_Main* clshdr = inst.hdr->theClass->cls.hdr;
    char* clsbase = inst.hdr->theClass->cls.base;


    SCMBClassPropertyNode& clsProp =
        ((SCMBClassPropertyNode*)
         &(clsbase[clshdr->propertySet.nodeArray.start]))[nodeIdx];

    SCMBValue& instValue =
        ((SCMBValue*)&(inst.base[inst.hdr->propertyArray.start]))[nodeIdx];

    _getCIMValueFromSCMBValue(theValue,instValue,inst.base);


    if (inst.hdr->flags.includeClassOrigin)
    {
        retProperty = CIMProperty(
            CIMNameCast(NEWCIMSTR(clsProp.theProperty.name,clsbase)),
            theValue,
            theValue.getArraySize(),
            CIMNameCast(NEWCIMSTR(clsProp.theProperty.refClassName,clsbase)),
            CIMNameCast(NEWCIMSTR(clsProp.theProperty.originClassName,clsbase)),
            clsProp.theProperty.flags.propagated);
    }
    else
    {
         retProperty = CIMProperty(
            CIMNameCast(NEWCIMSTR(clsProp.theProperty.name,clsbase)),
            theValue,
            theValue.getArraySize(),
            CIMNameCast(NEWCIMSTR(clsProp.theProperty.refClassName,clsbase)),
            CIMName(),
            clsProp.theProperty.flags.propagated);
    }

    if (inst.hdr->flags.includeQualifiers)
    {
        SCMBQualifier* qualiArray =
            (SCMBQualifier*)
                 &(clsbase[clsProp.theProperty.qualifierArray.start]);

        CIMName qualiName;

        Uint32 i, k = clsProp.theProperty.numberOfQualifiers;
        for ( i = 0 ; i < k ; i++)
        {
            _getCIMValueFromSCMBValue(theValue,qualiArray[i].value,clsbase);

            if (qualiArray[i].name == QUALNAME_USERDEFINED)
            {
                qualiName = NEWCIMSTR(qualiArray[i].userDefName,clsbase);
            }
            else
            {
                qualiName = String(
                    SCMOClass::qualifierNameStrLit(qualiArray[i].name).str,
                    SCMOClass::qualifierNameStrLit(qualiArray[i].name).size);
            }

            retProperty._rep->_qualifiers.addUnchecked(
                CIMQualifier(
                    qualiName,
                    theValue,
                    qualiArray[i].flavor,
                    qualiArray[i].propagated));
        }
    }

    return retProperty;

}

void SCMOInstance::_getCIMValueFromSCMBUnion(
    CIMValue& cimV,
    const CIMType type,
    const Boolean isNull,
    const Boolean isArray,
    const Uint32 arraySize,
    const SCMBUnion& scmbUn,
    const char * base) const
{

    const SCMBUnion* psourceUnion = NULL;

    if (isNull)
    {
        cimV.setNullValue(type,isArray,arraySize);
        return;
    }

    if (isArray)
    {
        psourceUnion =(SCMBUnion*)&(base[scmbUn.arrayValue.start]);
    }

    switch (type)
    {

    case CIMTYPE_UINT8:
        {
            if (isArray)
            {
                Array<Uint8> x;
                for (Uint32 i = 0, k = arraySize; i < k ; i++)
                {
                    x.append(psourceUnion[i].simple.val.u8);
                }
                cimV.set(x);
            }
            else
            {
                cimV.set(scmbUn.simple.val.u8);
            }
            break;
        }

    case CIMTYPE_UINT16:
        {
            if (isArray)
            {
                Array<Uint16> x;
                for (Uint32 i = 0, k = arraySize; i < k ; i++)
                {
                    x.append(psourceUnion[i].simple.val.u16);
                }
                cimV.set(x);
            }
            else
            {
                cimV.set(scmbUn.simple.val.u16);
            }
            break;
        }

    case CIMTYPE_UINT32:
        {
            if (isArray)
            {
                Array<Uint32> x;
                for (Uint32 i = 0, k = arraySize; i < k ; i++)
                {
                    x.append(psourceUnion[i].simple.val.u32);
                }
                cimV.set(x);
            }
            else
            {
                cimV.set(scmbUn.simple.val.u32);
            }
            break;
        }

    case CIMTYPE_UINT64:
        {
            if (isArray)
            {
                Array<Uint64> x;
                for (Uint32 i = 0, k = arraySize; i < k ; i++)
                {
                    x.append(psourceUnion[i].simple.val.u64);
                }
                cimV.set(x);
            }
            else
            {
                cimV.set(scmbUn.simple.val.u64);
            }
            break;
        }

    case CIMTYPE_SINT8:
        {
            if (isArray)
            {
                Array<Sint8> x;
                for (Uint32 i = 0, k = arraySize; i < k ; i++)
                {
                    x.append(psourceUnion[i].simple.val.s8);
                }
                cimV.set(x);
            }
            else
            {
                cimV.set(scmbUn.simple.val.s8);
            }
            break;
        }

    case CIMTYPE_SINT16:
        {
            if (isArray)
            {
                Array<Sint16> x;
                for (Uint32 i = 0, k = arraySize; i < k ; i++)
                {
                    x.append(psourceUnion[i].simple.val.s16);
                }
                cimV.set(x);
            }
            else
            {
                cimV.set(scmbUn.simple.val.s16);
            }
            break;
        }

    case CIMTYPE_SINT32:
        {
            if (isArray)
            {
                Array<Sint32> x;
                for (Uint32 i = 0, k = arraySize; i < k ; i++)
                {
                    x.append(psourceUnion[i].simple.val.s32);
                }
                cimV.set(x);
            }
            else
            {
                cimV.set(scmbUn.simple.val.s32);
            }
            break;
        }

    case CIMTYPE_SINT64:
        {
            if (isArray)
            {
                Array<Sint64> x;
                for (Uint32 i = 0, k = arraySize; i < k ; i++)
                {
                    x.append(psourceUnion[i].simple.val.s64);
                }
                cimV.set(x);
            }
            else
            {
                cimV.set(scmbUn.simple.val.s64);
            }
            break;
        }

    case CIMTYPE_REAL32:
        {
            if (isArray)
            {
                Array<Real32> x;
                for (Uint32 i = 0, k = arraySize; i < k ; i++)
                {
                    x.append(psourceUnion[i].simple.val.r32);
                }
                cimV.set(x);
            }
            else
            {
                cimV.set(scmbUn.simple.val.r32);
            }
            break;
        }

    case CIMTYPE_REAL64:
        {
            if (isArray)
            {
                Array<Real64> x;
                for (Uint32 i = 0, k = arraySize; i < k ; i++)
                {
                    x.append(psourceUnion[i].simple.val.r64);
                }
                cimV.set(x);
            }
            else
            {
                cimV.set(scmbUn.simple.val.r64);
            }
            break;
        }

    case CIMTYPE_CHAR16:
        {
            if (isArray)
            {
                Array<Char16> x;
                for (Uint32 i = 0, k = arraySize; i < k ; i++)
                {
                    x.append(psourceUnion[i].simple.val.c16);
                }
                cimV.set(x);
            }
            else
            {
                cimV.set(scmbUn.simple.val.c16);
            }
            break;
        }

    case CIMTYPE_BOOLEAN:
        {
            if (isArray)
            {
                Array<Boolean> x;
                for (Uint32 i = 0, k = arraySize; i < k ; i++)
                {
                    x.append(psourceUnion[i].simple.val.bin);
                }
                cimV.set(x);
            }
            else
            {
                cimV.set(scmbUn.simple.val.bin);
            }
            break;
        }

    case CIMTYPE_STRING:
        {
            if (isArray)
            {

                Array<String> x;

                for (Uint32 i = 0, k = arraySize; i < k ; i++)
                {
                    x.append(NEWCIMSTR(psourceUnion[i].stringValue,base));
                }
                cimV.set(x);
            }
            else
            {
                cimV.set(NEWCIMSTR(scmbUn.stringValue,base));
            }
            break;
        }

    case CIMTYPE_DATETIME:
        {
            if (isArray)
            {
                Array<CIMDateTime> x;

                for (Uint32 i = 0, k = arraySize; i < k ; i++)
                {
                    x.append(CIMDateTime(&(psourceUnion[i].dateTimeValue)));
                }
                cimV.set(x);
            }
            else
            {
                cimV.set(CIMDateTime(&scmbUn.dateTimeValue));
            }
            break;

        }

        case CIMTYPE_REFERENCE:

            break;

        case CIMTYPE_OBJECT:

            break;

        case CIMTYPE_INSTANCE:

            break;
    }
}

void SCMOInstance::_getCIMValueFromSCMBValue(
    CIMValue& cimV,
    const SCMBValue& scmbV,
    const char * base) const
{
    _getCIMValueFromSCMBUnion(
        cimV,
        scmbV.valueType,
        scmbV.flags.isNull,
        scmbV.flags.isArray,
        scmbV.valueArraySize,
        scmbV.value,
        base);
}


void SCMOInstance::_setCIMObjectPath(const CIMObjectPath& cimObj)
{
    CIMObjectPathRep* objRep = cimObj._rep;
    SCMO_RC rc;

    // For better usability define pointers to SCMO Class data structures.
    SCMBClass_Main* clshdr = inst.hdr->theClass->cls.hdr;
    char* clsbase = inst.hdr->theClass->cls.base;

    CString className = objRep->_className.getString().getCString();
    CString nameSpace = objRep->_nameSpace.getString().getCString();

    // Is the instance from the same class and name space ?
    if (!(_equalNoCaseUTF8Strings(
             clshdr->className,
             clsbase,
             (const char*)className,
             strlen(className)) &&
          _equalNoCaseUTF8Strings(
             clshdr->nameSpace,
             clsbase,
             (const char*)nameSpace,
             strlen(nameSpace)))
        )
    {
        throw PEGASUS_CIM_EXCEPTION(CIM_ERR_INVALID_CLASS,
           objRep->_className.getString());
    }

    //set host name
    _setString(objRep->_host,inst.hdr->hostName,&inst.mem );

    if (inst.hdr->numberKeyBindings < objRep->_keyBindings.size())
    {
        String message("CIMObjectPath has more keybindings "
                       "than the associated class key properties.");
        throw CIMException(CIM_ERR_FAILED, message);
    }

    for (Uint32 i = 0, k = objRep->_keyBindings.size(); i < k; i++)
    {

        rc = _setKeyBindingFromString(
            (const char*)
                    objRep->_keyBindings[i].getName().getString().getCString(),
            objRep->_keyBindings[i].getValue());

        if (rc != SCMO_OK)
        {
            switch (rc)
            {
            case SCMO_NOT_FOUND:
                {
                    String message("CIMObjectPath key binding ");
                    message.append(
                        objRep->_keyBindings[i].getName().getString());
                    message.append(" not found.");
                    throw CIMException(CIM_ERR_FAILED, message);
                }
            default:
                {
                    String message("CIMObjectPath key binding ");
                    message.append(
                        objRep->_keyBindings[i].getName().getString());
                    message.append(" does not match class definition!");
                    throw CIMException(CIM_ERR_FAILED, message);
                }
            }
        }
    }

}
void SCMOInstance::_setCIMValueAtNodeIndex(Uint32 node, CIMValueRep* valRep)
{
    SCMBValue* theInstPropNodeArray =
        (SCMBValue*)&inst.base[inst.hdr->propertyArray.start];


    SCMBValue& theInstProp = theInstPropNodeArray[node];

    theInstProp.valueType=valRep->type;
    theInstProp.flags.isNull=valRep->isNull;
    theInstProp.flags.isArray=valRep->isArray;
    theInstProp.flags.isSet=true;
    theInstProp.valueArraySize=0;

    if (valRep->isNull)
    {
        return;
    }

    Uint64 start = ((const char*)&(theInstProp.value))-inst.base;

    if (valRep->isArray)
    {
        _setUnionArrayValue(
            start,
            &inst.mem,
            valRep->type,
            // Is set to the number of array members by the function.
            theInstProp.valueArraySize,
            valRep->u);
    }
    else
    {
        _setNonRefUnionValue(start,&inst.mem,valRep->type,valRep->u);
    }
}


Boolean SCMOInstance::isSame(SCMOInstance& theInstance) const
{
    return inst.base == theInstance.inst.base;
}

const char* SCMOInstance::getHostName() const
{
  return _getCharString(inst.hdr->hostName,inst.base);
}

void SCMOInstance::buildKeyBindingsFromProperties()
{

    Uint32* theClassKeyPropList =
        (Uint32*) &((inst.hdr->theClass->cls.base)
                          [(inst.hdr->theClass->cls.hdr->keyIndexList.start)]);

    SCMBKeyBindingValue* theKeyBindValueArray;
    SCMBValue* theInstPropNodeArray;

    Uint32 propNode;

    for (Uint32 i = 0, k = inst.hdr->numberKeyBindings; i < k; i++)
    {
        // the instance pointers has to be reinitialized each time,
        // because in _setKeyBindingFromSCMBUnion()
        // a reallocation can take place.
        theKeyBindValueArray =
           (SCMBKeyBindingValue*)&inst.base[inst.hdr->keyBindingArray.start];

        theInstPropNodeArray =
            (SCMBValue*)&inst.base[inst.hdr->propertyArray.start];

        // If the keybinding is not set.
        if (!theKeyBindValueArray[i].isSet)
        {
            // get the node index for this key binding form class
            propNode = theClassKeyPropList[i];

            // if property was not set by the provider or it is null.
            if (!theInstPropNodeArray[propNode].flags.isSet ||
                 theInstPropNodeArray[propNode].flags.isNull)
            {
                const char * propName =
                    inst.hdr->theClass->_getPropertyNameAtNode(propNode);
                throw NoSuchProperty(String(propName));
            }

            _setKeyBindingFromSCMBUnion(
                theInstPropNodeArray[propNode].valueType,
                theInstPropNodeArray[propNode].value,
                inst.base,
                theKeyBindValueArray[i]);
        }
    }
}

void SCMOInstance::_setKeyBindingFromSCMBUnion(
    CIMType type,
    const SCMBUnion& u,
    const char * uBase,
    SCMBKeyBindingValue& keyData)
{
    switch (type)
    {
    case CIMTYPE_UINT8:
    case CIMTYPE_UINT16:
    case CIMTYPE_UINT32:
    case CIMTYPE_UINT64:
    case CIMTYPE_SINT8:
    case CIMTYPE_SINT16:
    case CIMTYPE_SINT32:
    case CIMTYPE_SINT64:
    case CIMTYPE_REAL32:
    case CIMTYPE_REAL64:
    case CIMTYPE_CHAR16:
    case CIMTYPE_BOOLEAN:
    case CIMTYPE_DATETIME:
        {
            memcpy(&keyData.data,&u,sizeof(SCMBUnion));
            keyData.isSet=true;
            break;
        }
    case CIMTYPE_STRING:
        {
            keyData.isSet=true;
            _setBinary(
                &uBase[u.stringValue.start],
                u.stringValue.length,
                keyData.data.stringValue,
                &inst.mem);
            break;
        }
    case CIMTYPE_REFERENCE:
        {
            break;
        }
    case CIMTYPE_OBJECT:
    case CIMTYPE_INSTANCE:
        // From PEP 194: EmbeddedObjects cannot be keys.
        throw TypeMismatchException();
        break;
    }
}

void SCMOInstance::setHostName(const char* hostName)
{
    Uint32 len;

    if (hostName!=NULL)
    {

        len = strlen((const char*)hostName);
        if(len != 0)
        {

            // copy including trailing '\0'
            _setBinary(hostName,len+1,inst.hdr->hostName,&inst.mem);
            return;
        }

    }
    inst.hdr->hostName.start=0;
    inst.hdr->hostName.length=0;
}

const char* SCMOInstance::getClassName() const
{
    return _getCharString(
        inst.hdr->theClass->cls.hdr->className,
        inst.hdr->theClass->cls.base);
}

const char* SCMOInstance::getClassName_l(Uint64 & length) const
{
    SCMOClass * scmoCls = inst.hdr->theClass;
    length = scmoCls->cls.hdr->className.length;
    return _getCharString(
        scmoCls->cls.hdr->className,
        scmoCls->cls.base);

}


const char* SCMOInstance::getNameSpace() const
{
    return _getCharString(
        inst.hdr->theClass->cls.hdr->nameSpace,
        inst.hdr->theClass->cls.base);
}

void SCMOInstance::_initSCMOInstance(
    SCMOClass* pClass,
    Boolean inclQual,
    Boolean inclOrigin)
{
    PEGASUS_ASSERT(SCMB_INITIAL_MEMORY_CHUNK_SIZE
        - sizeof(SCMBInstance_Main)>0);


    inst.base = (char*)malloc(SCMB_INITIAL_MEMORY_CHUNK_SIZE);
    if (inst.base == NULL)
    {
        // Not enough memory!
        throw PEGASUS_STD(bad_alloc)();
    }

    // initalize eye catcher
    inst.hdr->header.magic=PEGASUS_SCMB_INSTANCE_MAGIC;
    inst.hdr->header.totalSize=SCMB_INITIAL_MEMORY_CHUNK_SIZE;
    // The # of bytes free
    inst.hdr->header.freeBytes=
        SCMB_INITIAL_MEMORY_CHUNK_SIZE-sizeof(SCMBInstance_Main);
    // Index to the start of the free space in this instance
    inst.hdr->header.startOfFreeSpace=sizeof(SCMBInstance_Main);

    inst.hdr->refCount=1;

    //Assign the SCMBClass structure this instance based on.
    inst.hdr->theClass = pClass;

    // Init flags
    inst.hdr->flags.includeQualifiers=inclQual;
    inst.hdr->flags.includeClassOrigin=inclOrigin;
    inst.hdr->flags.isFiltered=false;

    inst.hdr->hostName.start=0;
    inst.hdr->hostName.length=0;

    // Number of key bindings
    inst.hdr->numberKeyBindings =
        inst.hdr->theClass->cls.hdr->keyBindingSet.number;

    // Number of properties
    inst.hdr->numberProperties =
        inst.hdr->theClass->cls.hdr->propertySet.number;

    // Allocate the SCMOInstanceKeyBindingArray
    _getFreeSpace(
          inst.hdr->keyBindingArray,
          sizeof(SCMBKeyBindingValue)*inst.hdr->numberKeyBindings,
          &inst.mem,
          true);

    // Allocate the SCMBInstancePropertyArray
    _getFreeSpace(
        inst.hdr->propertyArray,
        sizeof(SCMBValue)*inst.hdr->numberProperties,
        &inst.mem,
        true);

    inst.hdr->propertyFilter.start=0;
    inst.hdr->propertyFilter.length=0;
    inst.hdr->propertyFilterIndexMap.start=0;
    inst.hdr->propertyFilterIndexMap.length=0;


}

SCMO_RC SCMOInstance::getProperty(
    const char* name,
    CIMType& type,
    const SCMBUnion** pOutVal,
    Boolean& isArray,
    Uint32& size ) const
{
    Uint32 node;
    const char* pname;
    SCMO_RC rc = SCMO_OK;

    *pOutVal = NULL;
    isArray = false;
    size = 0;

    rc = inst.hdr->theClass->_getProperyNodeIndex(node,name);
    if (rc != SCMO_OK)
    {
        return rc;
    }

    // is filtering on ?
    if (inst.hdr->flags.isFiltered)
    {
        // Is the property NOT in the property filter ?
        if(!_isPropertyInFilter(node))
        {
            // The named propery is not part of this instance
            // due to filtering.
            return SCMO_NOT_FOUND;
        }
    }

    return  _getPropertyAtNodeIndex(node,&pname,type,pOutVal,isArray,size);
}

SCMO_RC SCMOInstance::getPropertyAt(
        Uint32 idx,
        const char** pname,
        CIMType& type,
        const SCMBUnion** pOutVal,
        Boolean& isArray,
        Uint32& size ) const
{
    *pname = NULL;
    *pOutVal = NULL;
    isArray = false;
    size = 0;
    Uint32 node;

    // is filtering on ?
    if (inst.hdr->flags.isFiltered)
    {
        // check the number of properties part of the filter
        if (idx >= inst.hdr->filterProperties)
        {
            return SCMO_INDEX_OUT_OF_BOUND;
        }

        // Get absolut pointer to property filter index map of the instance
        Uint32* propertyFilterIndexMap =
        (Uint32*)&(inst.base[inst.hdr->propertyFilterIndexMap.start]);

        // get the real node index of the property.
        node = propertyFilterIndexMap[idx];
    }
    else
    {
        // the index is used as node index.
        node = idx;
        if (node >= inst.hdr->numberProperties)
        {
            return SCMO_INDEX_OUT_OF_BOUND;
        }
    }

    return  _getPropertyAtNodeIndex(node,pname,type,pOutVal,isArray,size);
}

SCMO_RC SCMOInstance::getPropertyAt(
    Uint32 pos,
    SCMBValue** value,
    const char ** valueBase,
    SCMBClassProperty ** propDef) const
{
    Uint32 node;
    // is filtering on ?
    if (inst.hdr->flags.isFiltered)
    {
        // check the number of properties part of the filter
        if (pos >= inst.hdr->filterProperties)
        {
            return SCMO_INDEX_OUT_OF_BOUND;
        }

        // Get absolut pointer to property filter index map of the instance
        Uint32* propertyFilterIndexMap =
        (Uint32*)&(inst.base[inst.hdr->propertyFilterIndexMap.start]);

        // get the real node index of the property.
        node = propertyFilterIndexMap[pos];
    }
    else
    {
        // the index is used as node index.
        node = pos;
        if (node >= inst.hdr->numberProperties)
        {
            return SCMO_INDEX_OUT_OF_BOUND;
        }
    }

    SCMBValue* theInstPropNodeArray =
        (SCMBValue*)&inst.base[inst.hdr->propertyArray.start];

    // create a pointer to property node array of the class.
    Uint64 idx = inst.hdr->theClass->cls.hdr->propertySet.nodeArray.start;
    SCMBClassPropertyNode* theClassPropNodeArray =
        (SCMBClassPropertyNode*)&(inst.hdr->theClass->cls.base)[idx];

    // return the absolute pointer to the property definition
    *propDef= &(theClassPropNodeArray[node].theProperty);

    // need check if property set or not, if not set use the default value
    if (theInstPropNodeArray[node].flags.isSet)
    {
        // return the absolute pointer to the property value in the instance
        *value = &(theInstPropNodeArray[node]);
        *valueBase = inst.base;
    }
    else
    {
        // return the absolute pointer to
        *value = &(theClassPropNodeArray[node].theProperty.defaultValue);
        *valueBase = inst.hdr->theClass->cls.base;
    }

    return SCMO_OK;
}


SCMO_RC SCMOInstance::getPropertyNodeIndex(const char* name, Uint32& node) const
{
    SCMO_RC rc;
    if(name==NULL)
    {
        return SCMO_INVALID_PARAMETER;
    }

    rc = inst.hdr->theClass->_getProperyNodeIndex(node,name);

    return rc;

}

SCMO_RC SCMOInstance::setPropertyWithOrigin(
    const char* name,
    CIMType type,
    const SCMBUnion* pInVal,
    Boolean isArray,
    Uint32 size,
    const char* origin)
{
    Uint32 node;
    SCMO_RC rc;

    rc = inst.hdr->theClass->_getProperyNodeIndex(node,name);
    if (rc != SCMO_OK)
    {
        return rc;
    }

    // Is the traget type OK ?
    rc = inst.hdr->theClass->_isNodeSameType(node,type,isArray);
    if (rc != SCMO_OK)
    {
        return rc;
    }

    // is filtering on ?
    if (inst.hdr->flags.isFiltered)
    {
        // Is the property NOT in the property filter ?
        if(!_isPropertyInFilter(node))
        {
            // The named propery is not part of this instance
            // due to filtering.
            return SCMO_NOT_FOUND;
        }
    }

    // check class origin if set.
    if (origin!= NULL)
    {
        if(!inst.hdr->theClass->_isSamePropOrigin(node,origin))
        {
            return SCMO_NOT_SAME_ORIGIN;
        }
    }


    _setPropertyAtNodeIndex(node,type,pInVal,isArray,size);

    return SCMO_OK;
}

 SCMO_RC SCMOInstance::setPropertyWithNodeIndex(
     Uint32 node,
     CIMType type,
     const SCMBUnion* pInVal,
     Boolean isArray,
     Uint32 size)
 {
     SCMO_RC rc;

     if (node >= inst.hdr->numberProperties)
     {
         return SCMO_INDEX_OUT_OF_BOUND;
     }

     // is filtering on ?
     if (inst.hdr->flags.isFiltered)
     {
         // Is the property NOT in the property filter ?
         if(!_isPropertyInFilter(node))
         {
             // The proptery of the is not set due to filtering.
             return SCMO_OK;
         }
     }

     // Is the traget type OK ?
     rc = inst.hdr->theClass->_isNodeSameType(node,type,isArray);
     if (rc != SCMO_OK)
     {
         return rc;
     }

     _setPropertyAtNodeIndex(node,type,pInVal,isArray,size);

     return SCMO_OK;
 }

void SCMOInstance::_setPropertyAtNodeIndex(
    Uint32 node,
    CIMType type,
    const SCMBUnion* pInVal,
    Boolean isArray,
    Uint32 size)
{
    SCMBValue* theInstPropNodeArray =
        (SCMBValue*)&inst.base[inst.hdr->propertyArray.start];


    theInstPropNodeArray[node].flags.isSet=true;
    theInstPropNodeArray[node].valueType=type;
    theInstPropNodeArray[node].flags.isArray=isArray;
    if (isArray)
    {
        theInstPropNodeArray[node].valueArraySize=size;
    }

    if (pInVal==NULL)
    {
        theInstPropNodeArray[node].flags.isNull=true;
    }
    else
    {
        _setSCMBUnion(
            pInVal,
            type,
            isArray,
            size,
            theInstPropNodeArray[node].value);
    }
}

void SCMOInstance::_setSCMBUnion(
    const SCMBUnion* pInVal,
    CIMType type,
    Boolean isArray,
    Uint32 size,
    SCMBUnion & u)
{

    switch (type)
    {
    case CIMTYPE_BOOLEAN:
    case CIMTYPE_UINT8:
    case CIMTYPE_SINT8:
    case CIMTYPE_UINT16:
    case CIMTYPE_SINT16:
    case CIMTYPE_UINT32:
    case CIMTYPE_SINT32:
    case CIMTYPE_UINT64:
    case CIMTYPE_SINT64:
    case CIMTYPE_REAL32:
    case CIMTYPE_REAL64:
    case CIMTYPE_CHAR16:
    case CIMTYPE_DATETIME:
        {
            if (isArray)
            {
                _setBinary(pInVal,size*sizeof(SCMBUnion),
                           u.arrayValue,
                           &inst.mem );
            }
            else
            {
                memcpy(&u,pInVal,sizeof(SCMBUnion));
            }
            break;
        }
    case CIMTYPE_STRING:
        {
            if (isArray)
            {
                SCMBUnion* ptr;
                Uint64 startPtr;

                startPtr = _getFreeSpace(
                    u.arrayValue,
                    size*sizeof(SCMBUnion),
                    &inst.mem,false);

                for (Uint32 i = 0; i < size; i++)
                {
                    ptr = (SCMBUnion*)&(inst.base[startPtr]);
                    // Copy the sting including the trailing '\0'
                    _setBinary(
                        pInVal[i].extString.pchar,
                        pInVal[i].extString.length+1,
                        ptr[i].stringValue,
                        &inst.mem );
                }
            }
            else
            {
                // Copy the sting including the trailing '\0'
                _setBinary(
                    pInVal->extString.pchar,
                    pInVal->extString.length+1,
                    u.stringValue,
                    &inst.mem );
            }
            break;
        }

        case CIMTYPE_REFERENCE:

            break;

        case CIMTYPE_OBJECT:

            break;
        case CIMTYPE_INSTANCE:

            break;
    }
}

inline void SCMOInstance::_setUnionArrayValue(
    Uint64 start,
    SCMBMgmt_Header** pmem,
    CIMType type,
    Uint32& n,
    Union& u)
{
    SCMBUnion* scmoUnion = (SCMBUnion*)&(((char*)*pmem)[start]);
    SCMBUnion* ptargetUnion;
    Uint64 arrayStart;
    Uint32 loop;

    switch (type)
    {
    case CIMTYPE_BOOLEAN:
        {
            Array<Boolean> *x = reinterpret_cast<Array<Boolean>*>(&u);
            // n can be invalid after re-allocation in _getFreeSpace !
            loop = n = x->size();
            arrayStart = _getFreeSpace(
                scmoUnion->arrayValue,
                loop*sizeof(SCMBUnion),
                pmem,
                true);

            ConstArrayIterator<Boolean> iterator(*x);

            ptargetUnion = (SCMBUnion*)&(((char*)*pmem)[arrayStart]);
            for (Uint32 i = 0; i < loop; i++)
            {
                ptargetUnion[i].simple.val.bin  = iterator[i];
                ptargetUnion[i].simple.hasValue = true;
            }
            break;
        }

    case CIMTYPE_UINT8:
        {
            Array<Uint8> *x = reinterpret_cast<Array<Uint8>*>(&u);
            // n can be invalid after re-allocation in _getFreeSpace !
            loop = n = x->size();
            arrayStart = _getFreeSpace(
                scmoUnion->arrayValue,
                loop*sizeof(SCMBUnion),
                pmem,
                true);

            ConstArrayIterator<Uint8> iterator(*x);

            ptargetUnion = (SCMBUnion*)&(((char*)*pmem)[arrayStart]);
            for (Uint32 i = 0; i < loop; i++)
            {
                ptargetUnion[i].simple.val.u8  = iterator[i];
                ptargetUnion[i].simple.hasValue = true;
            }
            break;
        }

    case CIMTYPE_SINT8:
        {
            Array<Sint8> *x = reinterpret_cast<Array<Sint8>*>(&u);
            // n can be invalid after re-allocation in _getFreeSpace !
            loop = n = x->size();
            arrayStart = _getFreeSpace(
                scmoUnion->arrayValue,
                loop*sizeof(SCMBUnion),
                pmem,
                true);

            ConstArrayIterator<Sint8> iterator(*x);

            ptargetUnion = (SCMBUnion*)&(((char*)*pmem)[arrayStart]);
            for (Uint32 i = 0; i < loop; i++)
            {
                ptargetUnion[i].simple.val.s8  = iterator[i];
                ptargetUnion[i].simple.hasValue = true;
            }
            break;
        }

    case CIMTYPE_UINT16:
        {
            Array<Uint16> *x = reinterpret_cast<Array<Uint16>*>(&u);
            // n can be invalid after re-allocation in _getFreeSpace !
            loop = n = x->size();
            arrayStart = _getFreeSpace(
                scmoUnion->arrayValue,
                loop*sizeof(SCMBUnion),
                pmem,
                true);

            ConstArrayIterator<Uint16> iterator(*x);

            ptargetUnion = (SCMBUnion*)&(((char*)*pmem)[arrayStart]);
            for (Uint32 i = 0; i < loop; i++)
            {
                ptargetUnion[i].simple.val.u16  = iterator[i];
                ptargetUnion[i].simple.hasValue = true;
            }
            break;
        }

    case CIMTYPE_SINT16:
        {
            Array<Sint16> *x = reinterpret_cast<Array<Sint16>*>(&u);
            // n can be invalid after re-allocation in _getFreeSpace !
            loop = n = x->size();
            arrayStart = _getFreeSpace(
                scmoUnion->arrayValue,
                loop*sizeof(SCMBUnion),
                pmem,
                true);

            ConstArrayIterator<Sint16> iterator(*x);

            ptargetUnion = (SCMBUnion*)&(((char*)*pmem)[arrayStart]);
            for (Uint32 i = 0; i < loop; i++)
            {
                ptargetUnion[i].simple.val.s16  = iterator[i];
                ptargetUnion[i].simple.hasValue = true;
            }
            break;
        }

    case CIMTYPE_UINT32:
        {
            Array<Uint32> *x = reinterpret_cast<Array<Uint32>*>(&u);
            // n can be invalid after re-allocation in _getFreeSpace !
            loop = n = x->size();
            arrayStart = _getFreeSpace(
                scmoUnion->arrayValue,
                loop*sizeof(SCMBUnion),
                pmem,
                true);

            ConstArrayIterator<Uint32> iterator(*x);

            ptargetUnion = (SCMBUnion*)&(((char*)*pmem)[arrayStart]);
            for (Uint32 i = 0; i < loop; i++)
            {
                ptargetUnion[i].simple.val.u32  = iterator[i];
                ptargetUnion[i].simple.hasValue = true;
            }
            break;
        }

    case CIMTYPE_SINT32:
        {
            Array<Sint32> *x = reinterpret_cast<Array<Sint32>*>(&u);
            // n can be invalid after re-allocation in _getFreeSpace !
            loop = n = x->size();
            arrayStart = _getFreeSpace(
                scmoUnion->arrayValue,
                loop*sizeof(SCMBUnion),
                pmem,
                true);

            ConstArrayIterator<Sint32> iterator(*x);

            ptargetUnion = (SCMBUnion*)&(((char*)*pmem)[arrayStart]);
            for (Uint32 i = 0; i < loop; i++)
            {
                ptargetUnion[i].simple.val.s32  = iterator[i];
                ptargetUnion[i].simple.hasValue = true;
            }
            break;
        }

    case CIMTYPE_UINT64:
        {
            Array<Uint64> *x = reinterpret_cast<Array<Uint64>*>(&u);
            // n can be invalid after re-allocation in _getFreeSpace !
            loop = n = x->size();
            arrayStart = _getFreeSpace(
                scmoUnion->arrayValue,
                loop*sizeof(SCMBUnion),
                pmem,
                true);

            ConstArrayIterator<Uint64> iterator(*x);

            ptargetUnion = (SCMBUnion*)&(((char*)*pmem)[arrayStart]);
            for (Uint32 i = 0; i < loop; i++)
            {
                ptargetUnion[i].simple.val.u64  = iterator[i];
                ptargetUnion[i].simple.hasValue = true;
            }
            break;
        }

    case CIMTYPE_SINT64:
        {
            Array<Sint64> *x = reinterpret_cast<Array<Sint64>*>(&u);
            // n can be invalid after re-allocation in _getFreeSpace !
            loop = n = x->size();
            arrayStart = _getFreeSpace(
                scmoUnion->arrayValue,
                loop*sizeof(SCMBUnion),
                pmem,
                true);

            ConstArrayIterator<Sint64> iterator(*x);

            ptargetUnion = (SCMBUnion*)&(((char*)*pmem)[arrayStart]);
            for (Uint32 i = 0; i < loop; i++)
            {
                ptargetUnion[i].simple.val.s64  = iterator[i];
                ptargetUnion[i].simple.hasValue = true;
            }
            break;
        }

    case CIMTYPE_REAL32:
        {
            Array<Real32> *x = reinterpret_cast<Array<Real32>*>(&u);
            // n can be invalid after re-allocation in _getFreeSpace !
            loop = n = x->size();
            arrayStart = _getFreeSpace(
                scmoUnion->arrayValue,
                loop*sizeof(SCMBUnion),
                pmem,
                true);

            ConstArrayIterator<Real32> iterator(*x);

            ptargetUnion = (SCMBUnion*)&(((char*)*pmem)[arrayStart]);
            for (Uint32 i = 0; i < loop; i++)
            {
                ptargetUnion[i].simple.val.r32  = iterator[i];
                ptargetUnion[i].simple.hasValue = true;
            }
            break;
        }

    case CIMTYPE_REAL64:
        {
            Array<Real64> *x = reinterpret_cast<Array<Real64>*>(&u);
            // n can be invalid after re-allocation in _getFreeSpace !
            loop = n = x->size();
            arrayStart = _getFreeSpace(
                scmoUnion->arrayValue,
                loop*sizeof(SCMBUnion),
                pmem,
                true);

            ConstArrayIterator<Real64> iterator(*x);

            ptargetUnion = (SCMBUnion*)&(((char*)*pmem)[arrayStart]);
            for (Uint32 i = 0; i < loop; i++)
            {
                ptargetUnion[i].simple.val.r64  = iterator[i];
                ptargetUnion[i].simple.hasValue = true;
            }
            break;
        }

    case CIMTYPE_CHAR16:
        {
            Array<Char16> *x = reinterpret_cast<Array<Char16>*>(&u);
            // n can be invalid after re-allocation in _getFreeSpace !
            loop = n = x->size();
            arrayStart = _getFreeSpace(
                scmoUnion->arrayValue,
                loop*sizeof(SCMBUnion),
                pmem,
                true);

            ConstArrayIterator<Char16> iterator(*x);

            ptargetUnion = (SCMBUnion*)&(((char*)*pmem)[arrayStart]);
            for (Uint32 i = 0; i < loop; i++)
            {
                ptargetUnion[i].simple.val.c16  = iterator[i];
                ptargetUnion[i].simple.hasValue = true;
            }
            break;
        }

    case CIMTYPE_STRING:
        {
            Array<String> *x = reinterpret_cast<Array<String>*>(&u);
            // n can be invalid after re-allocation in _getFreeSpace !
            loop = n = x->size();

            arrayStart = _getFreeSpace(
                scmoUnion->arrayValue,
                loop*sizeof(SCMBDataPtr),
                pmem);

            ConstArrayIterator<String> iterator(*x);

            for (Uint32 i = 0; i < loop ; i++)
            {
                // the pointer has to be set eache loop,
                // because a reallocation may take place.
                ptargetUnion = (SCMBUnion*)(&((char*)*pmem)[arrayStart]);
                _setString( iterator[i],ptargetUnion[i].stringValue, pmem );
            }

            break;
        }

    case CIMTYPE_DATETIME:
        {
            Array<CIMDateTime> *x = reinterpret_cast<Array<CIMDateTime>*>(&u);
            // n can be invalid after reallocation in _getFreeSpace !
            loop = n = x->size();

            arrayStart = _getFreeSpace(
                scmoUnion->arrayValue,
                loop*sizeof(SCMBDateTime),
                pmem);

            ConstArrayIterator<CIMDateTime> iterator(*x);

            ptargetUnion = (SCMBUnion*)(&((char*)*pmem)[arrayStart]);

            for (Uint32 i = 0; i < loop ; i++)
            {
                memcpy(
                    &(ptargetUnion[i].dateTimeValue),
                    iterator[i]._rep,
                    sizeof(SCMBDateTime));
            }
            break;
        }

        case CIMTYPE_REFERENCE:

            break;

        case CIMTYPE_OBJECT:

            break;

        case CIMTYPE_INSTANCE:

            break;
    }
}


void SCMOInstance::_setNonRefUnionValue(
    Uint64 start,
    SCMBMgmt_Header** pmem,
    CIMType type,
    Union& u)
{
    SCMBUnion* scmoUnion = (SCMBUnion*)&(((char*)*pmem)[start]);

    switch (type)
    {
    case CIMTYPE_BOOLEAN:
        {
            scmoUnion->simple.val.bin = u._booleanValue;
            scmoUnion->simple.hasValue=true;
            break;
        }

    case CIMTYPE_UINT8:
        {
            scmoUnion->simple.val.u8 = u._uint8Value;
            scmoUnion->simple.hasValue=true;
            break;
        }

    case CIMTYPE_SINT8:
        {
            scmoUnion->simple.val.s8 = u._sint8Value;
            scmoUnion->simple.hasValue=true;
            break;
        }

    case CIMTYPE_UINT16:
        {
            scmoUnion->simple.val.u16 = u._uint16Value;
            scmoUnion->simple.hasValue=true;
            break;
        }

    case CIMTYPE_SINT16:
        {
            scmoUnion->simple.val.s16 = u._sint16Value;
            scmoUnion->simple.hasValue=true;
            break;
        }

    case CIMTYPE_UINT32:
        {
            scmoUnion->simple.val.u32 = u._uint32Value;
            scmoUnion->simple.hasValue=true;
            break;
        }

    case CIMTYPE_SINT32:
        {
            scmoUnion->simple.val.s32 = u._sint32Value;
            scmoUnion->simple.hasValue=true;
            break;
        }

    case CIMTYPE_UINT64:
        {
            scmoUnion->simple.val.u64 = u._uint64Value;
            scmoUnion->simple.hasValue=true;
            break;
        }

    case CIMTYPE_SINT64:
        {
            scmoUnion->simple.val.s64 = u._sint64Value;
            scmoUnion->simple.hasValue=true;
            break;
        }

    case CIMTYPE_REAL32:
        {
            scmoUnion->simple.val.r32 = u._real32Value;
            scmoUnion->simple.hasValue=true;
            break;
        }

    case CIMTYPE_REAL64:
        {
            scmoUnion->simple.val.r64 = u._real64Value;
            scmoUnion->simple.hasValue=true;
            break;
        }

    case CIMTYPE_CHAR16:
        {
            scmoUnion->simple.val.c16 = u._char16Value;
            scmoUnion->simple.hasValue=true;
            break;
        }

    case CIMTYPE_STRING:
        {
            _setString(*((String*)((void*)&u)),
                       scmoUnion->stringValue,
                       pmem );
            break;
        }

    case CIMTYPE_DATETIME:
        {
            memcpy(
                &scmoUnion->dateTimeValue,
                (*((CIMDateTime*)((void*)&u)))._rep,
                sizeof(SCMBDateTime));
            break;
        }

    case CIMTYPE_REFERENCE:
    case CIMTYPE_OBJECT:
    case CIMTYPE_INSTANCE:
        {
            // has to be handled by SCMOClass or SCMOInstance separatly
            break;
        }
    }
}

SCMO_RC SCMOInstance::_getPropertyAtNodeIndex(
        Uint32 node,
        const char** pname,
        CIMType& type,
        const SCMBUnion** pvalue,
        Boolean& isArray,
        Uint32& size ) const
{
    SCMBValue* theInstPropNodeArray =
        (SCMBValue*)&inst.base[inst.hdr->propertyArray.start];

    // create a pointer to property node array of the class.
    Uint64 idx = inst.hdr->theClass->cls.hdr->propertySet.nodeArray.start;
    SCMBClassPropertyNode* theClassPropNodeArray =
        (SCMBClassPropertyNode*)&(inst.hdr->theClass->cls.base)[idx];

    // the property name is always from the class.
    // return the absolut pointer to the property name,
    // the caller has to copy the name!
    *pname=_getCharString(
        theClassPropNodeArray[node].theProperty.name,
        inst.hdr->theClass->cls.base);

    // the property was set by the provider.
    if (theInstPropNodeArray[node].flags.isSet)
    {

        type = theInstPropNodeArray[node].valueType;
        isArray = theInstPropNodeArray[node].flags.isArray;
        if (isArray)
        {
            size = theInstPropNodeArray[node].valueArraySize;
        }

        if (theInstPropNodeArray[node].flags.isNull)
        {
            return SCMO_NULL_VALUE;
        }

        // calculate the relative index for the value.
        Uint64 start =
            (const char*)&(theInstPropNodeArray[node].value) -
            inst.base;

        // the caller has to copy the value !
        *pvalue = _resolveSCMBUnion(type,isArray,size,start,inst.base);

        return SCMO_OK;
    }

    // the get the defaults out of the class.
    type = theClassPropNodeArray[node].theProperty.defaultValue.valueType;
    isArray =
        theClassPropNodeArray[node].theProperty.defaultValue.flags.isArray;
    if (isArray)
    {
        size = theClassPropNodeArray[node].
                   theProperty.defaultValue.valueArraySize;
    }

    if (theClassPropNodeArray[node].theProperty.defaultValue.flags.isNull)
    {
        return SCMO_NULL_VALUE;
    }

    // calcutate the relativ start address of the value
    Uint64 start =
        (const char*)
               &(theClassPropNodeArray[node].theProperty.defaultValue.value) -
        (inst.hdr->theClass->cls.base);

    *pvalue = _resolveSCMBUnion(
        type,
        isArray,
        size,
        start,
        (inst.hdr->theClass->cls.base)
        );

    return SCMO_OK;

}

SCMOInstance SCMOInstance::clone(Boolean objectPathOnly) const
{
    if (objectPathOnly)
    {
        // Create a new, empty SCMOInstance
        SCMOInstance newInst(*(this->inst.hdr->theClass));

        // Copy the host name to tha new instance-
        _setBinary(
            _resolveDataPtr(this->inst.hdr->hostName,this->inst.base),
            this->inst.hdr->hostName.length,
            newInst.inst.hdr->hostName,
            &newInst.inst.mem);

        // Copy the key bindings to that new instance.
        this->_copyKeyBindings(newInst);

        return newInst;
    }

    SCMOInstance newInst;
    newInst.inst.base = (char*)malloc(this->inst.mem->totalSize);
    if (newInst.inst.base == NULL )
    {
        throw PEGASUS_STD(bad_alloc)();
    }

    memcpy( newInst.inst.base,this->inst.base,this->inst.mem->totalSize);
    // reset the refcounter of this new instance
    newInst.inst.hdr->refCount = 1;
    // kepp the ref counter of the class correct !
    newInst.inst.hdr->theClass = new SCMOClass(*(this->inst.hdr->theClass));

    return newInst;
}

void SCMOInstance::_copyKeyBindings(SCMOInstance& targetInst) const
{
    Uint32 noBindings = inst.hdr->numberKeyBindings;

    SCMBKeyBindingValue* sourceArray =
        (SCMBKeyBindingValue*)&inst.base[inst.hdr->keyBindingArray.start];

    // Address the class keybinding information
    const SCMBClass_Main* clshdr = inst.hdr->theClass->cls.hdr;
    const char * clsbase = inst.hdr->theClass->cls.base;
    SCMBKeyBindingNode* scmoClassArray =
        (SCMBKeyBindingNode*)&clsbase[clshdr->keyBindingSet.nodeArray.start];

    SCMBKeyBindingValue* targetArray;

    for (Uint32 i = 0; i < noBindings; i++)
    {
        // hast to be set every time, because of reallocation.
        targetArray=(SCMBKeyBindingValue*)&(targetInst.inst.base)
                             [targetInst.inst.hdr->keyBindingArray.start];
        if(sourceArray[i].isSet)
        {
            // this has to be done on the target instance to keep constantness.
            targetInst._setKeyBindingFromSCMBUnion(
                scmoClassArray[i].type,
                sourceArray[i].data,
                inst.base,
                targetArray[i]);
        }
    }

}
Uint32 SCMOInstance::getPropertyCount() const
{
    if (inst.hdr->flags.isFiltered)
    {
        return(inst.hdr->filterProperties);
    }

    return(inst.hdr->numberProperties);
}

SCMBUnion * SCMOInstance::_resolveSCMBUnion(
    CIMType type,
    Boolean isArray,
    Uint32 size,
    Uint64 start,
    char* base) const
{

    SCMBUnion* u = (SCMBUnion*)&(base[start]);

    SCMBUnion* av = NULL;

    if (isArray)
    {
        if (size == 0)
        {
            return NULL;
        }
        av = (SCMBUnion*)&base[u->arrayValue.start];
    }


    switch (type)
    {
    case CIMTYPE_BOOLEAN:
    case CIMTYPE_UINT8:
    case CIMTYPE_SINT8:
    case CIMTYPE_UINT16:
    case CIMTYPE_SINT16:
    case CIMTYPE_UINT32:
    case CIMTYPE_SINT32:
    case CIMTYPE_UINT64:
    case CIMTYPE_SINT64:
    case CIMTYPE_REAL32:
    case CIMTYPE_REAL64:
    case CIMTYPE_CHAR16:
    case CIMTYPE_DATETIME:
        {
            if(isArray)
            {
                return (av);
            }
            else
            {
                return(u);
            }
            break;
        }

    case CIMTYPE_STRING:
        {
            SCMBUnion *ptr;

            if (isArray)
            {

                ptr = (SCMBUnion*)malloc(size*sizeof(SCMBUnion));
                if (ptr == NULL )
                {
                    throw PEGASUS_STD(bad_alloc)();
                }

                for(Uint32 i = 0; i < size; i++)
                {
                    // resolv relative pointer to absolute pointer
                    ptr[i].extString.pchar =
                        (char*)_getCharString(av[i].stringValue,base);
                    // lenght with out the trailing /0 !
                    ptr[i].extString.length = av[i].stringValue.length-1;
                }
            }
            else
            {
                ptr = (SCMBUnion*)malloc(sizeof(SCMBUnion));
                ptr->extString.pchar = 
                    (char*)_getCharString(u->stringValue,base);
                // lenght with out the trailing /0 !
                ptr->extString.length = u->stringValue.length-1;
            }

             return(ptr);
            break;
        }

    case CIMTYPE_REFERENCE:

        break;

    case CIMTYPE_OBJECT:

        break;

    case CIMTYPE_INSTANCE:

        break;

    default:
        PEGASUS_ASSERT(false);
        break;
    }
    return NULL;
}

Uint32 SCMOInstance::getKeyBindingCount() const
{
    return(inst.hdr->numberKeyBindings);
}


SCMO_RC SCMOInstance::getKeyBindingAt(
        Uint32 node,
        const char** pname,
        CIMType& type,
        const SCMBUnion** pvalue) const
{
    SCMO_RC rc;
    const SCMBUnion* pdata=NULL;
    Uint32 pnameLen=0;

    *pname = NULL;
    *pvalue = NULL;

    if (node >= inst.hdr->numberKeyBindings)
    {
        return SCMO_INDEX_OUT_OF_BOUND;
    }

    rc = _getKeyBindingDataAtNodeIndex(node,pname,pnameLen,type,&pdata);
    if (rc != SCMO_OK)
    {
        return rc;
    }

    *pvalue = _resolveSCMBUnion(
        type,
        false,  // A key binding can never be an array.
        0,
        (char*)pdata-inst.base,
        inst.base);

    return SCMO_OK;

}

SCMO_RC SCMOInstance::getKeyBinding(
    const char* name,
    CIMType& type,
    const SCMBUnion** pvalue) const
{
    SCMO_RC rc;
    Uint32 node;
    const char* pname=NULL;
    const SCMBUnion* pdata=NULL;
    Uint32 pnameLen=0;

    *pvalue = NULL;

    rc = inst.hdr->theClass->_getKeyBindingNodeIndex(node,name);
    if (rc != SCMO_OK)
    {
        return rc;
    }

    rc = _getKeyBindingDataAtNodeIndex(node,&pname,pnameLen,type,&pdata);
    if (rc != SCMO_OK)
    {
        return rc;
    }

    *pvalue = _resolveSCMBUnion(
        type,
        false,  // A key binding can never be an array.
        0,
        (char*)pdata-inst.base,
        inst.base);

    return SCMO_OK;
}


SCMO_RC SCMOInstance::_getKeyBindingDataAtNodeIndex(
    Uint32 node,
    const char** pname,
    Uint32 & pnameLen,
    CIMType & type,
    const SCMBUnion** pdata) const
{
    SCMBKeyBindingValue* theInstKeyBindValueArray =
        (SCMBKeyBindingValue*)&inst.base[inst.hdr->keyBindingArray.start];

    // create a pointer to keybinding node array of the class.
    Uint64 idx = inst.hdr->theClass->cls.hdr->keyBindingSet.nodeArray.start;
    SCMBKeyBindingNode* theClassKeyBindNodeArray =
        (SCMBKeyBindingNode*)&(inst.hdr->theClass->cls.base)[idx];

    type = theClassKeyBindNodeArray[node].type;

    /* First resolve pointer to the property name */
    pnameLen = theClassKeyBindNodeArray[node].name.length;
    *pname = _getCharString(
        theClassKeyBindNodeArray[node].name,
        inst.hdr->theClass->cls.base);

    // There is no value set in the instance
    if (!theInstKeyBindValueArray[node].isSet)
    {
        return SCMO_NULL_VALUE;
    }

    *pdata = &(theInstKeyBindValueArray[node].data);

    return SCMO_OK;
}

Boolean SCMOInstance::_setCimKeyBindingStringToSCMOKeyBindigValue(
    const char* v,
    Uint32 len,
    CIMType type,
    SCMBKeyBindingValue& scmoKBV
    )
{
    scmoKBV.isSet=false;

    if (v == NULL || len == 0 && type != CIMTYPE_STRING)
    {
        // The string is empty ! Do nothing.
        return true;
    }
    switch (type)
    {
    case CIMTYPE_UINT8:
        {
            Uint64 x;
            if (StringConversion::stringToUnsignedInteger(v, x) &&
                StringConversion::checkUintBounds(x, type))
            {
              scmoKBV.data.simple.val.u8 = Uint8(x);
              scmoKBV.isSet=true;
            }
            break;
        }
    case CIMTYPE_UINT16:
        {
            Uint64 x;
            if (StringConversion::stringToUnsignedInteger(v, x) &&
                StringConversion::checkUintBounds(x, type))
            {
              scmoKBV.data.simple.val.u16 = Uint16(x);
              scmoKBV.isSet=true;
            }
            break;
        }

    case CIMTYPE_UINT32:
        {
            Uint64 x;
            if (StringConversion::stringToUnsignedInteger(v, x) &&
                StringConversion::checkUintBounds(x, type))
            {
              scmoKBV.data.simple.val.u32 = Uint32(x);
              scmoKBV.isSet=true;
            }
            break;
        }

    case CIMTYPE_UINT64:
        {
            Uint64 x;
            if (StringConversion::stringToUnsignedInteger(v, x))
            {
              scmoKBV.data.simple.val.u64 = x;
              scmoKBV.isSet=true;
            }
            break;
        }

    case CIMTYPE_SINT8:
        {
            Sint64 x;
            if (StringConversion::stringToSignedInteger(v, x) &&
                StringConversion::checkSintBounds(x, type))
            {
              scmoKBV.data.simple.val.s8 = Sint8(x);
              scmoKBV.isSet=true;
            }
            break;
        }

    case CIMTYPE_SINT16:
        {
            Sint64 x;
            if (StringConversion::stringToSignedInteger(v, x) &&
                StringConversion::checkSintBounds(x, type))
            {
              scmoKBV.data.simple.val.s16 = Sint16(x);
              scmoKBV.isSet=true;
            }
            break;
        }

    case CIMTYPE_SINT32:
        {
            Sint64 x;
            if (StringConversion::stringToSignedInteger(v, x) &&
                StringConversion::checkSintBounds(x, type))
            {
              scmoKBV.data.simple.val.s32 = Sint32(x);
              scmoKBV.isSet=true;
            }
            break;
        }

    case CIMTYPE_SINT64:
        {
            Sint64 x;
            if (StringConversion::stringToSignedInteger(v, x))
            {
              scmoKBV.data.simple.val.s64 = x;
              scmoKBV.isSet=true;
            }
            break;
        }

    case CIMTYPE_DATETIME:
        {
            CIMDateTime tmp;

            try
            {
                tmp.set(v);
            }
            catch (InvalidDateTimeFormatException&)
            {
                return false;
            }

            memcpy(
                &(scmoKBV.data.dateTimeValue),
                tmp._rep,
                sizeof(SCMBDateTime));
            scmoKBV.isSet=true;
            break;
        }

    case CIMTYPE_REAL32:
        {
            Real64 x;

            if (!StringConversion::stringToReal64(v, x))
            {
              scmoKBV.data.simple.val.r32 = Real32(x);
              scmoKBV.isSet=true;
            }
            break;
        }

    case CIMTYPE_REAL64:
        {
            Real64 x;

            if (!StringConversion::stringToReal64(v, x))
            {
              scmoKBV.data.simple.val.r32 = x;
              scmoKBV.isSet=true;
            }
            break;
        }

    case CIMTYPE_CHAR16:
        {
            // Converts UTF-8 to UTF-16
            String tmp(v, len);
            if (tmp.size() == 1)
            {
                scmoKBV.data.simple.val.c16 = tmp[0];
                scmoKBV.isSet=true;
            }
            break;
        }
    case CIMTYPE_BOOLEAN:
        {
            if (strncasecmp(v, "TRUE",len) == 0)
            {
                scmoKBV.data.simple.val.bin = true;
                scmoKBV.isSet=true;
            }
            else if (strncasecmp(v, "FALSE",len) == 0)
                 {
                     scmoKBV.data.simple.val.bin = false;
                     scmoKBV.isSet=true;
                 }
            break;
        }

    case CIMTYPE_STRING:
        {
            scmoKBV.isSet=true;
            // Can cause reallocation !
            _setBinary(v,len,scmoKBV.data.stringValue,&inst.mem);
            return true;
            break;
        }
    case CIMTYPE_REFERENCE:
        {
            break;
        }
    case CIMTYPE_OBJECT:
    case CIMTYPE_INSTANCE:
        // From PEP 194: EmbeddedObjects cannot be keys.
        throw TypeMismatchException();
        break;
    }

    return scmoKBV.isSet;
}

SCMO_RC SCMOInstance::_setKeyBindingFromString(
    const char* name,
    String cimKeyBinding)
{
    SCMO_RC rc;
    Uint32 node;

    if (NULL == name)
    {
        return SCMO_INVALID_PARAMETER;
    }

    rc = inst.hdr->theClass->_getKeyBindingNodeIndex(node,name);
    if (rc != SCMO_OK)
    {
        return rc;
    }

   // create a pointer to keybinding node array of the class.
    Uint64 idx = inst.hdr->theClass->cls.hdr->keyBindingSet.nodeArray.start;
    SCMBKeyBindingNode* theClassKeyBindNodeArray =
        (SCMBKeyBindingNode*)&(inst.hdr->theClass->cls.base)[idx];

    // create a pointer to instance keybinding values
    SCMBKeyBindingValue* theInstKeyBindValueArray =
        (SCMBKeyBindingValue*)&(inst.base[inst.hdr->keyBindingArray.start]);

    CString tmp = cimKeyBinding.getCString();
    if ( _setCimKeyBindingStringToSCMOKeyBindigValue(
            (const char*)tmp,
            strlen((const char*)tmp),
            theClassKeyBindNodeArray[node].type,
            theInstKeyBindValueArray[node]))
    {
        return SCMO_TYPE_MISSMATCH;
    }

    return SCMO_OK;
}

SCMO_RC SCMOInstance::setKeyBinding(
    const char* name,
    CIMType type,
    const SCMBUnion* keyvalue)
{
    SCMO_RC rc;
    Uint32 node;

    if (NULL == name)
    {
        return SCMO_INVALID_PARAMETER;
    }

    rc = inst.hdr->theClass->_getKeyBindingNodeIndex(node,name);
    if (rc != SCMO_OK)
    {
        return rc;
    }

    return setKeyBindingAt(node, type, keyvalue);
}

SCMO_RC SCMOInstance::setKeyBindingAt(
        Uint32 node,
        CIMType type,
        const SCMBUnion* keyvalue)
{
    SCMO_RC rc;

   // create a pointer to keybinding node array of the class.
    Uint64 idx = inst.hdr->theClass->cls.hdr->keyBindingSet.nodeArray.start;
    SCMBKeyBindingNode* theClassKeyBindNodeArray =
        (SCMBKeyBindingNode*)&(inst.hdr->theClass->cls.base)[idx];

    if (NULL == keyvalue)
    {
        return SCMO_INVALID_PARAMETER;
    }

    if (node >= inst.hdr->numberKeyBindings)
    {
        return SCMO_INDEX_OUT_OF_BOUND;
    }

    if (theClassKeyBindNodeArray[node].type != type)
    {
        return SCMO_TYPE_MISSMATCH;
    }

    SCMBKeyBindingValue* theInstKeyBindValueArray =
        (SCMBKeyBindingValue*)&inst.base[inst.hdr->keyBindingArray.start];


    // Has to be set first,
    // because reallocaton can take place in _setSCMBUnion()
    theInstKeyBindValueArray[node].isSet=true;

    _setSCMBUnion(
        keyvalue,
        type,
        false, // a key binding can never be an array.
        0,
        theInstKeyBindValueArray[node].data);

    return SCMO_OK;
}


void SCMOInstance::setPropertyFilter(const char **propertyList)
{
    SCMO_RC rc;
    Uint32 node,i = 0;

    if (inst.hdr->propertyFilter.start == 0)
    {
        // Allocate the SCMBPropertyFilter
        _getFreeSpace(
            inst.hdr->propertyFilter,
            sizeof(Uint64)*(((inst.hdr->numberProperties-1)/64)+1),
            &inst.mem,
            true);

        // Allocate the SCMBPropertyFilterIndexMap
        _getFreeSpace(
            inst.hdr->propertyFilterIndexMap,
            sizeof(Uint32)*inst.hdr->numberProperties,
            &inst.mem,
            true);
    }
    // Get absolut pointer to property filter index map of the instance
    Uint32* propertyFilterIndexMap =
        (Uint32*)&(inst.base[inst.hdr->propertyFilterIndexMap.start]);

    // All properties are accepted
    if (propertyList == NULL)
    {
        // Clear filtering:
        // Switch filtering off.
        inst.hdr->flags.isFiltered = false;

        // Clear filter index map
        memset(
            propertyFilterIndexMap,
            0,
            sizeof(Uint32)*inst.hdr->numberProperties);

        //reset number filter properties to all
        inst.hdr->filterProperties = inst.hdr->numberProperties;

        return;
    }

    // Switch filtering on.
    inst.hdr->flags.isFiltered = true;

    // intit the filter with the key properties
    inst.hdr->filterProperties=_initPropFilterWithKeys();

    // add the properties to the filter.
    while (propertyList[i] != NULL)
    {
        // the hash index of the property if the property name is found
        rc = inst.hdr->theClass->_getProperyNodeIndex(node,propertyList[i]);

        // if property is already in the filter
        // ( eg. key properties ) do not add them !
        if (rc == SCMO_OK && !_isPropertyInFilter(node))
        {
            // The property name was found. Otherwise ignore this property name.
            // insert the hash index into the filter index map
            propertyFilterIndexMap[inst.hdr->filterProperties]=node;
            // increase number of properties in filter.
            inst.hdr->filterProperties++;
            // set bit in the property filter
            _setPropertyInPropertyFilter(node);
        }
        // Proceed with the next property name.
        i++;
    }

}


Uint32 SCMOInstance::_initPropFilterWithKeys()
{

    // Get absolut pointer to the key property mask of the class.
    Uint64 idx = inst.hdr->theClass->cls.hdr->keyPropertyMask.start;
    Uint64* keyMask =(Uint64*)&(inst.hdr->theClass->cls.base)[idx];

    // Get absolut pointer to property filter mask
    Uint64* propertyFilterMask =
        (Uint64*)&(inst.base[inst.hdr->propertyFilter.start]);

    // copy the key mask to the property filter mask
    memcpy(
        propertyFilterMask,
        keyMask,
        sizeof(Uint64)*(((inst.hdr->numberProperties-1)/64)+1));

    // Get absolut pointer to key index list of the class
    idx=inst.hdr->theClass->cls.hdr->keyIndexList.start;
    Uint32* keyIndex = (Uint32*)&(inst.hdr->theClass->cls.base)[idx];

    // Get absolut pointer to property filter index map of the instance
    Uint32* propertyFilterIndexMap =
        (Uint32*)&(inst.base[inst.hdr->propertyFilterIndexMap.start]);

    Uint32 noKeys = inst.hdr->theClass->cls.hdr->keyBindingSet.number;
    memcpy(propertyFilterIndexMap,keyIndex,sizeof(Uint32)*noKeys);

    // return the number of properties already in the filter index map
    return noKeys;

}

void SCMOInstance::_clearPropertyFilter()
{
    Uint64 *propertyFilter;

    // Calculate the real pointer to the Uint64 array
    propertyFilter = (Uint64*)&inst.base[inst.hdr->propertyFilter.start];

    // the number of Uint64 in the key mask is :
    // Decrease the number of properties by 1
    // since the array is starting at index 0!
    // Divide with the number of bits in a Uint64.
    // e.g. number of Properties = 68
    // (68 - 1) / 64 = 1 --> The mask consists of 2 Uint64

    memset(propertyFilter,
           0,
           sizeof(Uint64)*(((inst.hdr->numberProperties-1)/64)+1));

}
void SCMOInstance::_setPropertyInPropertyFilter(Uint32 i)
{
    Uint64 *propertyFilter;

    // In which Uint64 of key mask is the bit for property i ?
    // Divide with the number of bits in a Uint64.
    // 47 / 64 = 0 --> The key bit for property i is in in keyMask[0].
    Uint32 idx = i/64 ;

    // Create a filter to set the bit.
    // Modulo division with 64. Shift left a bit by the remainder.
    Uint64 filter = ( (Uint64)1 << (i%64));

    // Calculate the real pointer to the Uint64 array
    propertyFilter = (Uint64*)&(inst.base[inst.hdr->propertyFilter.start]);

    propertyFilter[idx] = propertyFilter[idx] | filter ;
}

Boolean SCMOInstance::_isPropertyInFilter(Uint32 i) const
{
    Uint64 *propertyFilter;

    // In which Uint64 of key mask is the bit for property i ?
    // Divide with the number of bits in a Uint64.
    // e.g. number of Properties = 68
    // 47 / 64 = 0 --> The key bit for property i is in in keyMask[0].
    Uint32 idx = i/64 ;

    // Create a filter to check if the bit is set:
    // Modulo division with 64. Shift left a bit by the remainder.
    Uint64 filter = ( (Uint64)1 << (i%64));

    // Calculate the real pointer to the Uint64 array
    propertyFilter = (Uint64*)&inst.base[inst.hdr->propertyFilter.start];

    // If the bit is set the property is NOT filtered.
    // So the result has to be negated!
    return propertyFilter[idx] & filter ;

}

/******************************************************************************
 * SCMODump Print and Dump functions
 *****************************************************************************/
SCMODump::SCMODump()
{
    _out = stdout;
    _fileOpen = false;

#ifdef PEGASUS_OS_ZOS
    setEBCDICEncoding(fileno(_out));
#endif

}

SCMODump::SCMODump(char* filename)
{
    openFile(filename);
}

void SCMODump::openFile(char* filename)
{
    const char* pegasusHomeDir = getenv("PEGASUS_HOME");

    if (pegasusHomeDir == NULL)
    {
        pegasusHomeDir = ".";
    }

    _filename = pegasusHomeDir;
    _filename.append("/");
    _filename.append(filename);

    _out = fopen((const char*)_filename.getCString(),"w+");

    _fileOpen = true;

#ifdef PEGASUS_OS_ZOS
    setEBCDICEncoding(fileno(_out));
#endif

}

void SCMODump::deleteFile()
{
    if(_fileOpen)
    {
        closeFile();
    }

    System::removeFile((const char*)_filename.getCString());

}
void SCMODump::closeFile()
{
    if (_fileOpen)
    {
        fclose(_out);
        _fileOpen=false;
        _out = stdout;
    }
}

SCMODump::~SCMODump()
{
    if (_fileOpen)
    {
        fclose(_out);
        _fileOpen=false;
    }
}

Boolean SCMODump::compareFile(String master)
{

    if (!_fileOpen)
    {
        return false;
    }

    closeFile();

    return (FileSystem::compareFiles(_filename, master));
}

void SCMODump::dumpSCMOInstance(SCMOInstance& testInst) const
{
    SCMBInstance_Main* insthdr = testInst.inst.hdr;
    char* instbase = testInst.inst.base;

    fprintf(_out,"\n\nDump of SCMOInstance\n");
    // The magic number for SCMO class
    fprintf(_out,"\nheader.magic=%08X",insthdr->header.magic);
    // Total size of the instance memory block( # bytes )
    fprintf(_out,"\nheader.totalSize=%llu",insthdr->header.totalSize);
    // The reference counter for this c++ class
    fprintf(_out,"\nrefCount=%i",insthdr->refCount.get());
    fprintf(_out,"\ntheClass: %p",insthdr->theClass);
    fprintf(_out,"\n\nThe Flags:");
    fprintf(_out,"\n   includeQualifiers: %s",
           (insthdr->flags.includeQualifiers ? "True" : "False"));
    fprintf(_out,"\n   includeClassOrigin: %s",
           (insthdr->flags.includeClassOrigin ? "True" : "False"));
    fprintf(_out,"\n   isFiltered: %s",
           (insthdr->flags.isFiltered ? "True" : "False"));
    fprintf(_out,"\n\nhostName: \'%s\'",
           NULLSTR(_getCharString(insthdr->hostName,instbase)));

    dumpSCMOInstanceKeyBindings(testInst);

    dumpSCMOInstancePropertyFilter(testInst);

    dumpInstanceProperties(testInst);
    fprintf(_out,"\n\n");

}

void SCMODump::dumpSCMOInstancePropertyFilter(SCMOInstance& testInst) const
{
    SCMBInstance_Main* insthdr = testInst.inst.hdr;
    char* instbase = testInst.inst.base;

    if (!insthdr->flags.isFiltered)
    {
        fprintf(_out,"\n\nNo propterty filter!\n\n");
        return;
    }

    fprintf(_out,"\n\nInstance Property Filter :");
    fprintf(_out,"\n==========================");
    fprintf(_out,"\n\nNumber of properties in the filter : %u\n"
        ,insthdr->filterProperties);

    dumpPropertyFilter(testInst);

    dumpPropertyFilterIndexMap(testInst);

}

void SCMODump::dumpInstanceProperties(SCMOInstance& testInst) const
{
    SCMBInstance_Main* insthdr = testInst.inst.hdr;
    char* instbase = testInst.inst.base;

    SCMBValue* val =
        (SCMBValue*)_resolveDataPtr(insthdr->propertyArray,instbase);

    fprintf(_out,"\n\nInstance Properties :");
    fprintf(_out,"\n=====================");
    fprintf(_out,"\n\nNumber of properties in instance : %u",
           insthdr->numberProperties);

    for (Uint32 i = 0, k = insthdr->numberProperties; i < k; i++)
    {
        fprintf(_out,"\n\nInstance property (#%3u) %s\n",i,
                NULLSTR(insthdr->theClass->_getPropertyNameAtNode(i)));
        if(insthdr->flags.isFiltered && !testInst._isPropertyInFilter(i))
        {
            fprintf(_out,"\nProperty is filtered out!");
        }
        else
        {
            printSCMOValue(val[i],instbase);
        }
    }

}

void SCMODump::dumpPropertyFilterIndexMap(SCMOInstance& testInst) const
{

    SCMBInstance_Main* insthdr = testInst.inst.hdr;
    char* instbase = testInst.inst.base;

    if (!insthdr->flags.isFiltered)
    {
        fprintf(_out,"\n\nNo propterty filter!\n\n");
        return;
    }

    fprintf(_out,"\n\nProperty Filter Index Max:");
    fprintf(_out,"\n==========================\n");

    // Get absolut pointer to key index list of the class
    Uint32* keyIndex =
        (Uint32*)&(instbase)[insthdr->propertyFilterIndexMap.start];

    Uint32 line,j,i,k = insthdr->filterProperties;

    for (j = 0; j < k; j = j + line)
    {
        if ((insthdr->filterProperties-j)/16)
        {
            line = 16 ;
        }
        else
        {
            line = insthdr->filterProperties%16;
        }


        fprintf(_out,"Index :");
        for (i = 0; i < line; i++)
        {
            fprintf(_out," %3u",j+i);
        }

        fprintf(_out,"\nNode  :");
        for (i = 0; i < line; i++)
        {
            fprintf(_out," %3u",keyIndex[j+i]);
        }

        fprintf(_out,"\n\n");

    }

}

void SCMODump::dumpPropertyFilter(SCMOInstance& testInst) const
{

    SCMBInstance_Main* insthdr = testInst.inst.hdr;
    char* instbase = testInst.inst.base;

    if (!insthdr->flags.isFiltered)
    {
        fprintf(_out,"\n\nNo propterty filter!");
        return;
    }

    Uint64 *thePropertyFilter =
        (Uint64*)&(instbase[insthdr->propertyFilter.start]);
     Uint32 end, noProperties = insthdr->numberProperties;
     Uint32 noMasks = (noProperties-1)/64;
     Uint64 printMask = 1;

     for (Uint32 i = 0; i <= noMasks; i++ )
     {
         printMask = 1;
         if (i < noMasks)
         {
             end = 64;
         }
         else
         {
             end = noProperties%64;
         }

         fprintf(_out,"\npropertyFilter[%02u]= ",i);

         for (Uint32 j = 0; j < end; j++)
         {
             if (j > 0 && !(j%8))
             {
                 fprintf(_out," ");
             }

             if (thePropertyFilter[i] & printMask)
             {
                 fprintf(_out,"1");
             }
             else
             {
                 fprintf(_out,"0");
             }

             printMask = printMask << 1;
         }
         fprintf(_out,"\n");
     }
}

void SCMODump::dumpSCMOInstanceKeyBindings(SCMOInstance& testInst) const
{
    SCMBInstance_Main* insthdr = testInst.inst.hdr;
    char* instbase = testInst.inst.base;

    // create a pointer to keybinding node array of the class.
    Uint64 idx = insthdr->theClass->cls.hdr->keyBindingSet.nodeArray.start;
    SCMBKeyBindingNode* theClassKeyBindNodeArray =
        (SCMBKeyBindingNode*)&(insthdr->theClass->cls.base)[idx];

    SCMBKeyBindingValue* ptr =
        (SCMBKeyBindingValue*)
             _resolveDataPtr(insthdr->keyBindingArray,instbase);

    fprintf(_out,"\n\nInstance Key Bindings :");
    fprintf(_out,"\n=======================");
    fprintf(_out,"\n\nNumber of Key Bindings : %u",insthdr->numberKeyBindings);

    for (Uint32 i = 0, k = insthdr->numberKeyBindings; i < k; i++)
    {
        if (ptr[i].isSet)
        {
            fprintf(_out,"\n\nNo %u : '%s'",i,
                (const char*)printUnionValue(
                    theClassKeyBindNodeArray[i].type,
                    ptr[i].data,
                    instbase).getCString());
        }
        else
        {
            fprintf(_out,"\n\nNo %u : Not Set",i);

        }
    }
    fprintf(_out,"\n");
}

void SCMODump::dumpSCMOClass(SCMOClass& testCls) const
{
    SCMBClass_Main* clshdr = testCls.cls.hdr;
    char* clsbase = testCls.cls.base;

    fprintf(_out,"\n\nDump of SCMOClass\n");
    // The magic number for SCMO class
    fprintf(_out,"\nheader.magic=%08X",clshdr->header.magic);
    // Total size of the instance memory block( # bytes )
    fprintf(_out,"\nheader.totalSize=%llu",clshdr->header.totalSize);
    // The reference counter for this c++ class
    fprintf(_out,"\nrefCount=%i",clshdr->refCount.get());

    fprintf(_out,"\n\nsuperClassName: \'%s\'",
           NULLSTR(_getCharString(clshdr->superClassName,clsbase)));
    fprintf(_out,"\nnameSpace: \'%s\'",
            NULLSTR(_getCharString(clshdr->nameSpace,clsbase)));
    fprintf(_out,"\nclassName: \'%s\'",
            NULLSTR(_getCharString(clshdr->className,clsbase)));
    fprintf(_out,"\n\nTheClass qualfiers:");
    _dumpQualifierArray(
        clshdr->qualifierArray.start,
        clshdr->numberOfQualifiers,
        clsbase);
    fprintf(_out,"\n");
    dumpKeyPropertyMask(testCls);
    fprintf(_out,"\n");
    dumpKeyIndexList(testCls);
    fprintf(_out,"\n");
    dumpClassProperties(testCls);
    fprintf(_out,"\n");
    dumpKeyBindingSet(testCls);
    fprintf(_out,"\n");
    /*
    */
    fprintf(_out,"\n");

}

void SCMODump::dumpSCMOClassQualifiers(SCMOClass& testCls) const
{
    SCMBClass_Main* clshdr = testCls.cls.hdr;
    char* clsbase = testCls.cls.base;

    fprintf(_out,"\n\nTheClass qualfiers:");
    _dumpQualifierArray(
        clshdr->qualifierArray.start,
        clshdr->numberOfQualifiers,
        clsbase);
    fprintf(_out,"\n\n\n");

}

void SCMODump::hexDumpSCMOClass(SCMOClass& testCls) const
{
    char* tmp;

    SCMBClass_Main* clshdr = testCls.cls.hdr;
    char* clsbase = testCls.cls.base;

    fprintf(_out,"\n\nHex dump of a SCMBClass:");
    fprintf(_out,"\n========================");
    fprintf(_out,"\n\n Size of SCMBClass: %llu\n\n",clshdr->header.totalSize);

    _hexDump(clsbase,clshdr->header.totalSize);

}
void SCMODump::dumpKeyIndexList(SCMOClass& testCls) const
{
    SCMBClass_Main* clshdr = testCls.cls.hdr;
    char* clsbase = testCls.cls.base;

    fprintf(_out,"\n\nKey Index List:");
    fprintf(_out,"\n===============\n");

    // Get absolut pointer to key index list of the class
    Uint32* keyIndex = (Uint32*)&(clsbase)[clshdr->keyIndexList.start];
    Uint32 line,j,i,k = clshdr->propertySet.number;
    for (j = 0; j < k; j = j + line)
    {
        if ((clshdr->propertySet.number-j)/16)
        {
            line = 16 ;
        }
        else
        {
            line = clshdr->propertySet.number%16;
        }


        fprintf(_out,"Index :");
        for (i = 0; i < line; i++)
        {
            fprintf(_out," %3u",j+i);
        }

        fprintf(_out,"\nNode  :");
        for (i = 0; i < line; i++)
        {
            fprintf(_out," %3u",keyIndex[j+i]);
        }

        fprintf(_out,"\n\n");

    }

}

void SCMODump::dumpKeyBindingSet(SCMOClass& testCls) const
{
    SCMBClass_Main* clshdr = testCls.cls.hdr;
    char* clsbase = testCls.cls.base;

    fprintf(_out,"\n\nKey Binding Set:");
    fprintf(_out,"\n=================\n");
    fprintf(_out,"\nNumber of key bindings: %3u",clshdr->keyBindingSet.number);
    dumpHashTable(
        clshdr->keyBindingSet.hashTable,
        PEGASUS_KEYBINDIG_SCMB_HASHSIZE);

    dumpClassKeyBindingNodeArray(testCls);

}

void SCMODump::dumpClassKeyBindingNodeArray(SCMOClass& testCls) const
{
    SCMBClass_Main* clshdr = testCls.cls.hdr;
    char* clsbase = testCls.cls.base;

    SCMBKeyBindingNode* nodeArray =
        (SCMBKeyBindingNode*)
             &(clsbase[clshdr->keyBindingSet.nodeArray.start]);

    for (Uint32 i = 0, k = clshdr->keyBindingSet.number; i < k; i++)
    {
        fprintf(_out,"\n\n===================");
        fprintf(_out,"\nKey Binding #%3u",i);
        fprintf(_out,"\n===================");

        fprintf(_out,"\nHas next: %s",(nodeArray[i].hasNext?"TRUE":"FALSE"));
        if (nodeArray[i].hasNext)
        {
            fprintf(_out,"\nNext Node: %3u",nodeArray[i].nextNode);
        }
        else
        {
            fprintf(_out,"\nNext Node: N/A");
        }

        fprintf(_out,"\nKey Property name: %s",
               NULLSTR(_getCharString(nodeArray[i].name,clsbase)));

        fprintf(_out,"\nHash Tag %3u Hash Index %3u",
               nodeArray[i].nameHashTag,
               nodeArray[i].nameHashTag%PEGASUS_KEYBINDIG_SCMB_HASHSIZE);

        fprintf(_out,"\nType: %s",cimTypeToString(nodeArray[i].type));

    }

}

void SCMODump::dumpClassProperties(SCMOClass& testCls) const
{
    SCMBClass_Main* clshdr = testCls.cls.hdr;
    char* clsbase = testCls.cls.base;

    fprintf(_out,"\n\nClass Properties:");
    fprintf(_out,"\n=================\n");
    fprintf(_out,"\nNumber of properties: %3u",clshdr->propertySet.number);
    dumpHashTable(
        clshdr->propertySet.hashTable,
        PEGASUS_PROPERTY_SCMB_HASHSIZE);
    dumpClassPropertyNodeArray(testCls);

}

void SCMODump::dumpClassPropertyNodeArray(SCMOClass& testCls) const
{
    SCMBClass_Main* clshdr = testCls.cls.hdr;
    char* clsbase = testCls.cls.base;

    SCMBClassPropertyNode* nodeArray =
        (SCMBClassPropertyNode*)
            &(clsbase[clshdr->propertySet.nodeArray.start]);

    for (Uint32 i = 0, k =  clshdr->propertySet.number; i < k; i++)
    {

        fprintf(_out,"\nClass property #%3u",i);
        fprintf(_out,"\n===================");

        fprintf(_out,"\nHas next: %s",(nodeArray[i].hasNext?"TRUE":"FALSE"));
        if (nodeArray[i].hasNext)
        {
            fprintf(_out,"\nNext Node: %3u",nodeArray[i].nextNode);
        }
        else
        {
            fprintf(_out,"\nNext Node: N/A");
        }

        _dumpClassProperty(nodeArray[i].theProperty,clsbase);
    }
}

void SCMODump::_dumpClassProperty(
    const SCMBClassProperty& prop,
    char* clsbase) const
{
    fprintf(_out,"\nProperty name: %s",
            NULLSTR(_getCharString(prop.name,clsbase)));

    fprintf(_out,"\nHash Tag %3u Hash Index %3u",
           prop.nameHashTag,
           prop.nameHashTag%PEGASUS_PROPERTY_SCMB_HASHSIZE);
    fprintf(_out,"\nPropagated: %s isKey: %s",
           (prop.flags.propagated?"TRUE":"FALSE"),
           (prop.flags.isKey?"TRUE":"FALSE")
           );

    fprintf(_out,"\nOrigin class name: %s",
           NULLSTR(_getCharString(prop.originClassName,clsbase)));
    fprintf(_out,"\nReference class name: %s",
           NULLSTR(_getCharString(prop.refClassName,clsbase)));

    printSCMOValue(prop.defaultValue,clsbase);

    _dumpQualifierArray(
        prop.qualifierArray.start,
        prop.numberOfQualifiers,
        clsbase);

}

void SCMODump::dumpHashTable(Uint32* hashTable,Uint32 size) const
{
    Uint32 i,j,line;
    fprintf(_out,"\n\nHash table:\n");


    for (j = 0; j < size; j = j + line)
    {
        if ((size-j)/16)
        {
            line = 16 ;
        }
        else
        {
            line = size%16;
        }


        fprintf(_out,"Index    :");
        for (i = 0; i < line; i++)
        {
            fprintf(_out," %3u",j+i);
        }

        fprintf(_out,"\nHashTable:");
        for (i = 0; i < line; i++)
        {
            fprintf(_out," %3u",hashTable[j+i]);
        }

        fprintf(_out,"\n\n");

    }


}

void SCMODump::_dumpQualifierArray(
    Uint64 start,
    Uint32 size,
    char* clsbase
    ) const
{

    SCMBQualifier *theArray = (SCMBQualifier*)&(clsbase[start]);

    for(Uint32 i = 0; i < size; i++)
    {
        _dumpQualifier(theArray[i],clsbase);
    }
}

void SCMODump::_dumpQualifier(
    const SCMBQualifier& theQualifier,
    char* clsbase
    ) const
{
     if(theQualifier.name == QUALNAME_USERDEFINED)
     {
         fprintf(_out,"\n\nQualifier user defined name: \'%s\'",
                NULLSTR(_getCharString(theQualifier.userDefName,clsbase)));
     }
     else
     {
         fprintf(_out,"\n\nQualifier DMTF defined name: \'%s\'",
                SCMOClass::qualifierNameStrLit(theQualifier.name).str);
     }

     fprintf(_out,"\nPropagated : %s",
            (theQualifier.propagated ? "True" : "False"));
     fprintf(_out,"\nFlavor : %s",
         (const char*)(CIMFlavor(theQualifier.flavor).toString().getCString()));

     printSCMOValue(theQualifier.value,clsbase);

}

void SCMODump::printSCMOValue(
    const SCMBValue& theValue,
    char* base) const
{
   fprintf(_out,"\nValueType : %s",cimTypeToString(theValue.valueType));
   fprintf(_out,"\nValue was set by the provider: %s",
       (theValue.flags.isSet ? "True" : "False"));
   if (theValue.flags.isNull)
   {
       fprintf(_out,"\nIt's a NULL value.");
       return;
   }
   if (theValue.flags.isArray)
   {
       fprintf(_out,
               "\nThe value is an Array of size: %u",
               theValue.valueArraySize);
       fprintf(_out,"\nThe values are: %s",
              (const char*)printArrayValue(
                  theValue.valueType,
                  theValue.valueArraySize,
                  theValue.value,
                  base).getCString());
   }
   else
   {
      fprintf(_out,"\nThe Value is: '%s'",
          (const char*)
             printUnionValue(theValue.valueType,theValue.value,base)
             .getCString());
   }

   return;

}

void SCMODump::dumpKeyPropertyMask(SCMOClass& testCls ) const
{

    SCMBClass_Main* clshdr = testCls.cls.hdr;
    char* clsbase = testCls.cls.base;

     Uint64 *theKeyMask = (Uint64*)&(clsbase[clshdr->keyPropertyMask.start]);
     Uint32 end, noProperties = clshdr->propertySet.number;
     Uint32 noMasks = (noProperties-1)/64;
     Uint64 printMask = 1;

     for (Uint32 i = 0; i <= noMasks; i++ )
     {
         printMask = 1;
         if (i < noMasks)
         {
             end = 64;
         }
         else
         {
             end = noProperties%64;
         }

         fprintf(_out,"\nkeyPropertyMask[%02u]= ",i);

         for (Uint32 j = 0; j < end; j++)
         {
             if (j > 0 && !(j%8))
             {
                 fprintf(_out," ");
             }

             if (theKeyMask[i] & printMask)
             {
                 fprintf(_out,"1");
             }
             else
             {
                 fprintf(_out,"0");
             }

             printMask = printMask << 1;
         }
         fprintf(_out,"\n");
     }
}

void SCMODump::_hexDump(char* buffer,int length) const
{

    unsigned char printLine[3][80];
    int p;
    int len;
    unsigned char item;

    for (int i = 0; i < length;i=i+1)
    {
        p = i%80;

        if ((p == 0 && i > 0) || i == length-1 )
        {
            for (int y = 0; y < 3; y=y+1)
            {
                if (p == 0)
                {
                    len = 80;
                } else
                {
                    len = p;
                }

                for (int x = 0; x < len; x=x+1)
                {
                    if (y == 0)
                    {
                        fprintf(_out,"%c",printLine[y][x]);
                    }
                    else
                    {
                        fprintf(_out,"%1X",printLine[y][x]);
                    }
                }
                fprintf(_out,"\n");
            }
            fprintf(_out,"\n");
        }

        item = (unsigned char)buffer[i];

        if (item < 32 || item > 125 )
        {
            printLine[0][p] = '.';
        } else
        {
            printLine[0][p] = item;
        }

        printLine[1][p] = item/16;
        printLine[2][p] = item%16;

    }
}

String SCMODump::printArrayValue(
    CIMType type,
    Uint32 size,
    SCMBUnion u,
    char* base) const
{
    Buffer out;

    SCMBUnion* p;
    p = (SCMBUnion*)&(base[u.arrayValue.start]);

    switch (type)
    {
    case CIMTYPE_BOOLEAN:
        {
            for (Uint32 i = 0; i < size; i++)
            {
                out.append('\'');
                _toString(out,p[i].simple.val.bin);
                out << STRLIT("\'(hasValue=");
                out << (p[i].simple.hasValue ?
                          STRLIT("TRUE)"):
                          STRLIT("FALSE)"));
                out.append(';');
            }
            break;
        }

    case CIMTYPE_UINT8:
        {
            for (Uint32 i = 0; i < size; i++)
            {
                out.append('\'');
                _toString(out,p[i].simple.val.u8);
                out << STRLIT("\'(hasValue=");
                out << (p[i].simple.hasValue ?
                          STRLIT("TRUE)"):
                          STRLIT("FALSE)"));
                out.append(';');
            }
            break;
        }

    case CIMTYPE_SINT8:
        {
            for (Uint32 i = 0; i < size; i++)
            {
                out.append('\'');
                _toString(out,p[i].simple.val.s8);
                out << STRLIT("\'(hasValue=");
                out << (p[i].simple.hasValue ?
                          STRLIT("TRUE)"):
                          STRLIT("FALSE)"));
                out.append(';');
            }
            break;
        }

    case CIMTYPE_UINT16:
        {
            for (Uint32 i = 0; i < size; i++)
            {
                out.append('\'');
                _toString(out,p[i].simple.val.u16);
                out << STRLIT("\'(hasValue=");
                out << (p[i].simple.hasValue ?
                          STRLIT("TRUE)"):
                          STRLIT("FALSE)"));
                out.append(';');
            }
            break;
        }

    case CIMTYPE_SINT16:
        {
            for (Uint32 i = 0; i < size; i++)
            {
                out.append('\'');
                _toString(out,p[i].simple.val.s16);
                out << STRLIT("\'(hasValue=");
                out << (p[i].simple.hasValue ?
                          STRLIT("TRUE)"):
                          STRLIT("FALSE)"));
                out.append(';');
            }
            break;
        }

    case CIMTYPE_UINT32:
        {
            for (Uint32 i = 0; i < size; i++)
            {
                out.append('\'');
                _toString(out,p[i].simple.val.u32);
                out << STRLIT("\'(hasValue=");
                out << (p[i].simple.hasValue ?
                          STRLIT("TRUE)"):
                          STRLIT("FALSE)"));
                out.append(';');
            }
            break;
        }

    case CIMTYPE_SINT32:
        {
            for (Uint32 i = 0; i < size; i++)
            {
                out.append('\'');
                _toString(out,p[i].simple.val.s32);
                out << STRLIT("\'(hasValue=");
                out << (p[i].simple.hasValue ?
                          STRLIT("TRUE)"):
                          STRLIT("FALSE)"));
                out.append(';');
            }
            break;
        }

    case CIMTYPE_UINT64:
        {
            for (Uint32 i = 0; i < size; i++)
            {
                out.append('\'');
                _toString(out,p[i].simple.val.u64);
                out << STRLIT("\'(hasValue=");
                out << (p[i].simple.hasValue ?
                          STRLIT("TRUE)"):
                          STRLIT("FALSE)"));
                out.append(';');
            }
            break;
        }

    case CIMTYPE_SINT64:
        {
            for (Uint32 i = 0; i < size; i++)
            {
                out.append('\'');
                _toString(out,p[i].simple.val.s64);
                out << STRLIT("\'(hasValue=");
                out << (p[i].simple.hasValue ?
                          STRLIT("TRUE)"):
                          STRLIT("FALSE)"));
                out.append(';');
            }
            break;
        }

    case CIMTYPE_REAL32:
        {
            for (Uint32 i = 0; i < size; i++)
            {
                out.append('\'');
                _toString(out,p[i].simple.val.r32);
                out << STRLIT("\'(hasValue=");
                out << (p[i].simple.hasValue ?
                          STRLIT("TRUE)"):
                          STRLIT("FALSE)"));
                out.append(';');
            }
            break;
        }

    case CIMTYPE_REAL64:
        {
            for (Uint32 i = 0; i < size; i++)
            {
                out.append('\'');
                _toString(out,p[i].simple.val.r64);
                out << STRLIT("\'(hasValue=");
                out << (p[i].simple.hasValue ?
                          STRLIT("TRUE)"):
                          STRLIT("FALSE)"));
                out.append(';');
            }
            break;
        }

    case CIMTYPE_CHAR16:
        {
            for (Uint32 i = 0; i < size; i++)
            {
                out.append('\'');
                _toString(out,p[i].simple.val.c16);
                out << STRLIT("\'(hasValue=");
                out << (p[i].simple.hasValue ?
                          STRLIT("TRUE)"):
                          STRLIT("FALSE)"));
                out.append(';');
            }
            break;
        }

    case CIMTYPE_STRING:
        {
            SCMBDataPtr* p = (SCMBDataPtr*)&(base[u.arrayValue.start]);
            for (Uint32 i = 0; i < size; i++)
            {
                if ( 0 != p[i].length)
                {
                    out.append('\'');
                    out.append((const char*)_getCharString(p[i],base),
                               p[i].length-1);
                    out.append('\'');
                }
                else
                {
                  out << STRLIT("NULL;");
                }
                out.append(';');
            }
            break;
        }

    case CIMTYPE_DATETIME:
        {
            SCMBDateTime* p = (SCMBDateTime*)&(base[u.arrayValue.start]);
            CIMDateTime x;
            for (Uint32 i = 0; i < size; i++)
            {
                memcpy(x._rep,&(p[i]),sizeof(SCMBDateTime));
                _toString(out,x);
                out.append(' ');
            }
            break;
        }

    case CIMTYPE_REFERENCE:
        {
            break;
        }

    case CIMTYPE_OBJECT:
        {
            break;
        }

    case CIMTYPE_INSTANCE:
        {
            break;
        }
    default:
        {
            PEGASUS_ASSERT(0);
        }
    }

    return out.getData();
}

String SCMODump::printUnionValue(
    CIMType type,
    SCMBUnion u,
    char* base) const
{

    Buffer out;

    switch (type)
    {
    case CIMTYPE_BOOLEAN:
        {
            _toString(out,u.simple.val.bin);
            break;
        }

    case CIMTYPE_UINT8:
        {
            _toString(out,u.simple.val.u8);
            break;
        }

    case CIMTYPE_SINT8:
        {
            _toString(out,u.simple.val.s8);
            break;
        }

    case CIMTYPE_UINT16:
        {
            _toString(out,(Uint32)u.simple.val.u16);
            break;
        }

    case CIMTYPE_SINT16:
        {
            _toString(out,u.simple.val.s16);
            break;
        }

    case CIMTYPE_UINT32:
        {
            _toString(out,u.simple.val.u32);
            break;
        }

    case CIMTYPE_SINT32:
        {
            _toString(out,u.simple.val.s32);
            break;
        }

    case CIMTYPE_UINT64:
        {
            _toString(out,u.simple.val.u64);
            break;
        }

    case CIMTYPE_SINT64:
        {
            _toString(out,u.simple.val.s64);
            break;
        }

    case CIMTYPE_REAL32:
        {
            _toString(out,u.simple.val.r32);
            break;
        }

    case CIMTYPE_REAL64:
        {
            _toString(out,u.simple.val.r32);
            break;
        }

    case CIMTYPE_CHAR16:
        {
            _toString(out,u.simple.val.c16);
            break;
        }

    case CIMTYPE_STRING:
        {
            if ( 0 != u.stringValue.length)
            {
                out.append((const char*)_getCharString(u.stringValue,base),
                           u.stringValue.length-1);
            }
            break;
        }

    case CIMTYPE_DATETIME:
        {
            CIMDateTime x;
            memcpy(x._rep,&(u.dateTimeValue),sizeof(SCMBDateTime));
            _toString(out,x);
            break;
        }

    case CIMTYPE_REFERENCE:
        {
            break;
        }

    case CIMTYPE_OBJECT:
        {
            break;
        }

    case CIMTYPE_INSTANCE:
        {
            break;
        }
    default:
        {
            PEGASUS_ASSERT(0);
        }
    }

  return out.getData();
}


/*****************************************************************************
 * The constant functions
 *****************************************************************************/

/*
static CIMKeyBinding::Type _cimTypeToKeyBindType(CIMType cimType)
{
    switch (cimType)
    {
    case CIMTYPE_BOOLEAN:
        return(CIMKeyBinding::BOOLEAN);
        break;
    case CIMTYPE_CHAR16:
    case CIMTYPE_STRING:
    case CIMTYPE_DATETIME:
        return(CIMKeyBinding::STRING);
        break;
    case CIMTYPE_REFERENCE:
        return(CIMKeyBinding::REFERENCE);
        break;
    case CIMTYPE_OBJECT:
    case CIMTYPE_INSTANCE:
        // From PEP 194: EmbeddedObjects cannot be keys.
        throw TypeMismatchException();
        break;
    default:
        return(CIMKeyBinding::NUMERIC);
        break;
    }
}
*/
/* OLD THILO VERSION
static Boolean _equalUTF8Strings(
    const SCMBDataPtr& ptr_a,
    char* base,
    const char* name,
    Uint32 len)

{
    //both are empty strings, so they are equal.
    if (ptr_a.length == 0 && len == 0)
    {
        return true;
    }

    // size without trailing '\0' !!
    if (ptr_a.length-1 != len)
    {
        return false;
    }

    const char* a = (const char*)_getCharString(ptr_a,base);

    // ToDo: Here an UTF8 complinet comparison should take place
    return ( strncmp(a,name,len )== 0 );

}
*/

static Boolean _equalUTF8Strings(
    const SCMBDataPtr& ptr_a,
    char* base,
    const char* name,
    Uint32 len)

{
    // size without trailing '\0' !!
    if (ptr_a.length-1 != len)
    {
        return false;
    }
    const char* a = (const char*)_getCharString(ptr_a,base);

    // lets do a loop-unrolling optimized compare here
    while (len >= 8)
    {
        if ((a[0] - name[0]) || (a[1] - name[1]) ||
            (a[2] - name[2]) || (a[3] - name[3]) ||
            (a[4] - name[4]) || (a[5] - name[5]) ||
            (a[6] - name[6]) || (a[7] - name[7]))
        {
            return false;
        }
        len -= 8;
        a += 8;
        name += 8;
    }
    while (len >= 4)
    {
        if ((a[0] - name[0]) || (a[1] - name[1]) ||
            (a[2] - name[2]) || (a[3] - name[3]))
        {
            return false;
        }
        len -= 4;
        a += 4;
        name += 4;
    }
    while (len--)
    {
        if (a[0] - name[0])
        {
            return false;
        }
        a++;
        name++;
    }
    return true;
}


static Boolean _equalNoCaseUTF8Strings(
    const SCMBDataPtr& ptr_a,
    char* base,
    const char* name,
    Uint32 len)

{

    //both are empty strings, so they are equal.
    if (ptr_a.length == 0 && len == 0)
    {
        return true;
    }

    // size without trailing '\0' !!
    if (ptr_a.length-1 != len)
    {
        return false;
    }

    const char* a = (const char*)_getCharString(ptr_a,base);

    // ToDo: Here an UTF8 complinet comparison should take place
    return ( strncasecmp(a,name,len )== 0 );

}


/**
 * This function calcutates a free memory slot in the single chunk memory block.
 * Warning: In this routine a reallocation may take place.
 * @param ptr A reference to a data SCMB data pointer. The values to the free
 *            block is written into this pointer. If the provided ptr is
 *            located in the single chunk memory block, this pointer may be
 *            invalid after this call. You have to recalcuate the pointer
 *            after calling this function.
 * @parm size The requested free memory slot.
 * @parm pmem A reference to the pointer of the single chunk memory block.
 *            e.g. &cls.mem
 * @return The relaive index of the free memory slot.
 */

static Uint64 _getFreeSpace(
    SCMBDataPtr& ptr,
    Uint64 size,
    SCMBMgmt_Header** pmem,
    Boolean clear)
{
    Uint64 oldSize, start;

    if (size == 0)
    {
        ptr.start = 0;
        ptr.length = 0;
        return 0;
    }

    // The SCMBDataPtr has to be set before any reallocation.
    start = (*pmem)->startOfFreeSpace;
    ptr.start = start;
    ptr.length = size;

    while ((*pmem)->freeBytes < size)
    {
        // save old size of buffer
        oldSize = (*pmem)->totalSize;
        // reallocate the buffer, double the space !
        // This is a working approach until a better algorithm is found.
        (*pmem) = (SCMBMgmt_Header*)realloc((*pmem),oldSize*2);
        if ((*pmem) == NULL)
        {
            // Not enough memory!
            throw PEGASUS_STD(bad_alloc)();
        }
        // increase the total size and free space
        (*pmem)->freeBytes+=oldSize;
        (*pmem)->totalSize+=oldSize;
    }

    (*pmem)->freeBytes -= size;
    (*pmem)->startOfFreeSpace += size;

    if (clear)
    {
        // If requested, set memory to 0.
        memset(&((char*)(*pmem))[start],0,size);
    }
    return start;
}

static void _setString(
    const String& theString,
    SCMBDataPtr& ptr,
    SCMBMgmt_Header** pmem)
{

    // Get the UTF8 CString
    CString theCString=theString.getCString();
    // Get the real size of the UTF8 sting + \0.
    // It maybe greater then the length in the String due to
    // 4 byte encoding of non ASCII chars.
    Uint64 start,length = strlen((const char*)theCString)+1;

    // If the string is not empty.
    if (length != 1)
    {

       // Attention ! In this function a reallocation may take place.
       // The reference ptr may be unusable after the call to _getFreeSpace
       // --> use the returned start index.
       start = _getFreeSpace(ptr , length, pmem);
       // Copy string including trailing \0
       memcpy(&((char*)(*pmem))[start],(const char*)theCString,length);
    }
    else
    {
        ptr.start = 0;
        ptr.length = 0;
    }
}

static void _setBinary(
    const void* theBuffer,
    Uint64 bufferSize,
    SCMBDataPtr& ptr,
    SCMBMgmt_Header** pmem)
{

    // If buffer is not empty.
    if (bufferSize != 1 && theBuffer != NULL)
    {

        Uint64 start;
        // Attention ! In this function a reallocation may take place.
        // The reference ptr may be unusable after the call to _getFreeSpace
        // --> use the returned start index.
        start = _getFreeSpace(ptr , bufferSize, pmem);
        // Copy buffer into SCMB
        memcpy(
            &((char*)(*pmem))[start],
            (const char*)theBuffer,
            bufferSize);
    }
    else
    {
        ptr.start = 0;
        ptr.length = 0;
    }
}


PEGASUS_NAMESPACE_END
