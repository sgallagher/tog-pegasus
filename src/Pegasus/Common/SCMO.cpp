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

#include "SCMO.h"
#include <Pegasus/Common/CharSet.h>
#include <Pegasus/Common/CIMDateTimeRep.h>
#include <Pegasus/Common/CIMPropertyRep.h>
#include <Pegasus/Common/CommonUTF.h>
#include <Pegasus/Common/StrLit.h>
#include <Pegasus/Common/XmlWriter.h>



PEGASUS_USING_STD;

#define SCMB_INITIAL_MEMORY_CHUNK_SIZE 4096


PEGASUS_NAMESPACE_BEGIN

static StrLit _qualifierNameStrLit[] =
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

SCMOClass::SCMOClass(CIMClass& theCIMClass)
{
    PEGASUS_ASSERT(SCMB_INITIAL_MEMORY_CHUNK_SIZE
        - sizeof(SCMBClass_Main)>0);

    cls.base = (unsigned char*)malloc(SCMB_INITIAL_MEMORY_CHUNK_SIZE);
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
   _setString(theObjectPath.getNameSpace().getString(),
              cls.hdr->nameSpace,
              &cls.mem );



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

    for (Uint32 i = 0 ; i < cls.hdr->propertySet.number; i++)
    {
        // Append the key property name.
        // The length has to be reduces by 1 not to copy the trailing '\0'
        keyNames.append(
            String((const char*)_getCharString(nodeArray[i].name,cls.base),
                   nodeArray[i].name.length-1));

    }
}

SCMO_RC SCMOClass::_getKeyBindingNodeIndex(Uint32& node, const char* name) const
{

    Uint32 tag,len,hashIdx;

    len = strlen(name);
    tag = _generateStringTag((const unsigned char*)name, len);
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
    tag = _generateStringTag((const unsigned char*)name, len);
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
    scmoKeyBindNode->type = _cimTypeToKeyBindType(propRep->_value.getType());
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
        (unsigned char*)&scmoPropNode->theProperty.defaultValue - cls.base;

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

    valueStart = (unsigned char*)&scmoQual->value - cls.base;

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

    valueStart = (unsigned char*)&scmoValue->value - cls.base;

    if (rep->isNull)
    {
        return;
    }

    if (scmoValue->flags.isArray)
    {
        scmoValue->valueArraySize = theCIMValue.getArraySize();
        _setArrayValue(valueStart,rep->type, rep->u);
    }
    else
    {
        _setUnionValue(valueStart, rep->type, rep->u);
    }
}

void SCMOClass::_setArrayValue(Uint64 start, CIMType type, Union& u)
{
    SCMBUnion* scmoUnion = (SCMBUnion*)&(cls.base[start]);
    Uint64 arrayStart;
    Uint32 n;

    switch (type)
    {
    case CIMTYPE_BOOLEAN:
        {
            Array<Boolean> *x = reinterpret_cast<Array<Boolean>*>(&u);
            n = x->size();
            arrayStart = _getFreeSpace(
                scmoUnion->_arrayValue,
                n*sizeof(Boolean),
                &cls.mem);
            memcpy(&cls.base[arrayStart],x->getData(),n * sizeof(Boolean));
            break;
        }

    case CIMTYPE_UINT8:
        {
            Array<Uint8> *x = reinterpret_cast<Array<Uint8>*>(&u);
            n = x->size();
            arrayStart = _getFreeSpace(
                scmoUnion->_arrayValue,
                n*sizeof(Uint8),
                &cls.mem);
            memcpy(&cls.base[arrayStart],x->getData(),n * sizeof(Uint8));
            break;
        }

    case CIMTYPE_SINT8:
        {
            Array<Sint8> *x = reinterpret_cast<Array<Sint8>*>(&u);
            n = x->size();
            arrayStart = _getFreeSpace(
                scmoUnion->_arrayValue,
                n*sizeof(Sint8),
                &cls.mem);
            memcpy(&cls.base[arrayStart],x->getData(),n * sizeof(Sint8));
            break;
        }

    case CIMTYPE_UINT16:
        {
            Array<Uint16> *x = reinterpret_cast<Array<Uint16>*>(&u);
            n = x->size();
            arrayStart = _getFreeSpace(
                scmoUnion->_arrayValue,
                n*sizeof(Uint16),
                &cls.mem);
            memcpy(&cls.base[arrayStart],x->getData(),n * sizeof(Uint16));

            break;
        }

    case CIMTYPE_SINT16:
        {
            Array<Sint16> *x = reinterpret_cast<Array<Sint16>*>(&u);
            n = x->size();
            arrayStart = _getFreeSpace(
                scmoUnion->_arrayValue,
                n*sizeof(Sint16),
                &cls.mem);
            memcpy(&cls.base[arrayStart],x->getData(),n * sizeof(Sint16));
            break;
        }

    case CIMTYPE_UINT32:
        {
            Array<Uint32> *x = reinterpret_cast<Array<Uint32>*>(&u);
            n = x->size();
            arrayStart = _getFreeSpace(
                scmoUnion->_arrayValue,
                n*sizeof(Uint32),
                &cls.mem);
            memcpy(&cls.base[arrayStart],x->getData(),n * sizeof(Uint32));
            break;
        }

    case CIMTYPE_SINT32:
        {
            Array<Sint32> *x = reinterpret_cast<Array<Sint32>*>(&u);
            n = x->size();
            arrayStart = _getFreeSpace(
                scmoUnion->_arrayValue,
                n*sizeof(Sint32),
                &cls.mem);
            memcpy(&cls.base[arrayStart],x->getData(),n * sizeof(Sint32));
            break;
        }

    case CIMTYPE_UINT64:
        {
            Array<Uint64> *x = reinterpret_cast<Array<Uint64>*>(&u);
            n = x->size();
            arrayStart = _getFreeSpace(
                scmoUnion->_arrayValue,
                n*sizeof(Uint64),
                &cls.mem);
            memcpy(&cls.base[arrayStart],x->getData(),n * sizeof(Uint64));
            break;
        }

    case CIMTYPE_SINT64:
        {
            Array<Sint64> *x = reinterpret_cast<Array<Sint64>*>(&u);
            n = x->size();
            arrayStart = _getFreeSpace(
                scmoUnion->_arrayValue,
                n*sizeof(Sint64),
                &cls.mem);
            memcpy(&cls.base[arrayStart],x->getData(),n * sizeof(Sint64));
            break;
        }

    case CIMTYPE_REAL32:
        {
            Array<Real32> *x = reinterpret_cast<Array<Real32>*>(&u);
            n = x->size();
            arrayStart = _getFreeSpace(
                scmoUnion->_arrayValue,
                n*sizeof(Real32),
                &cls.mem);
            memcpy(&cls.base[arrayStart],x->getData(),n * sizeof(Real32));
            break;
        }

    case CIMTYPE_REAL64:
        {
            Array<Real64> *x = reinterpret_cast<Array<Real64>*>(&u);
            n = x->size();
            arrayStart = _getFreeSpace(
                scmoUnion->_arrayValue,
                n*sizeof(Real64),
                &cls.mem);
            memcpy(&cls.base[arrayStart],x->getData(),n * sizeof(Real64));
            break;
        }

    case CIMTYPE_CHAR16:
        {
            Array<Char16> *x = reinterpret_cast<Array<Char16>*>(&u);
            n = x->size();
            arrayStart = _getFreeSpace(
                scmoUnion->_arrayValue,
                n*sizeof(Char16),
                &cls.mem);
            memcpy(&cls.base[arrayStart],x->getData(),n * sizeof(Char16));
            break;
        }

    case CIMTYPE_STRING:
        {
            SCMBDataPtr *ptr;

            Array<String> *x = reinterpret_cast<Array<String>*>(&u);

            n = x->size();
            arrayStart = _getFreeSpace(
                scmoUnion->_arrayValue,
                n*sizeof(SCMBDataPtr),
                &cls.mem);

            for (Uint32 i = 0; i < n ; i++)
            {
                // the pointer has to be set eache loop,
                // because a reallocation may take place.
                ptr = (SCMBDataPtr*)(&cls.base[arrayStart]);
                _setString( (*x)[i],ptr[i], &cls.mem );
            }

            break;
        }

    case CIMTYPE_DATETIME:
        {
            SCMBDateTime *ptr;
            Array<CIMDateTime> *x = reinterpret_cast<Array<CIMDateTime>*>(&u);
            n = x->size();
            arrayStart = _getFreeSpace(
                scmoUnion->_arrayValue,
                n*sizeof(SCMBDateTime),
                &cls.mem);

            ptr=(SCMBDateTime*)(&cls.base[arrayStart]);

            for (Uint32 i = 0; i < n ; i++)
            {
                memcpy(&(ptr[i]),(*x)[i]._rep,sizeof(SCMBDateTime));
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


void SCMOClass::_setUnionValue(Uint64 start, CIMType type, Union& u)
{
    SCMBUnion* scmoUnion = (SCMBUnion*)&(cls.base[start]);

    switch (type)
    {
    case CIMTYPE_BOOLEAN:
        {
            scmoUnion->_booleanValue = u._booleanValue;
            break;
        }

    case CIMTYPE_UINT8:
        {
            scmoUnion->_uint8Value = u._uint8Value;
            break;
        }

    case CIMTYPE_SINT8:
        {
            scmoUnion->_sint8Value = u._sint8Value;
            break;
        }

    case CIMTYPE_UINT16:
        {
            scmoUnion->_uint16Value = u._uint16Value;
            break;
        }

    case CIMTYPE_SINT16:
        {
            scmoUnion->_sint16Value = u._sint16Value;
            break;
        }

    case CIMTYPE_UINT32:
        {
            scmoUnion->_uint32Value = u._uint32Value;
            break;
        }

    case CIMTYPE_SINT32:
        {
            scmoUnion->_sint32Value = u._sint32Value;
            break;
        }

    case CIMTYPE_UINT64:
        {
            scmoUnion->_uint64Value = u._uint64Value;
            break;
        }

    case CIMTYPE_SINT64:
        {
            scmoUnion->_sint64Value = u._sint64Value;
            break;
        }

    case CIMTYPE_REAL32:
        {
            scmoUnion->_real32Value = u._real32Value;
            break;
        }

    case CIMTYPE_REAL64:
        {
            scmoUnion->_real64Value = u._real64Value;
            break;
        }

    case CIMTYPE_CHAR16:
        {
            scmoUnion->_char16Value = u._char16Value;
            break;
        }

    case CIMTYPE_STRING:
        {
            _setString(*((String*)((void*)&u)),
                       scmoUnion->_stringValue,
                       &cls.mem );
            break;
        }

    case CIMTYPE_DATETIME:
        {
            memcpy(
                &scmoUnion->_dateTimeValue,
                (*((CIMDateTime*)((void*)&u)))._rep,
                sizeof(SCMBDateTime));
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
        if (_qualifierNameStrLit[i].size == length)
        {
            // TBD: Make it more efficent...
            if(String::equalNoCase(
                theCIMName.getString(),
                _qualifierNameStrLit[i].str))
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

SCMO_RC SCMOClass::_isNodeSameType(
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

Boolean SCMOInstance::isSame(SCMOInstance& theInstance) const
{
    return inst.base == theInstance.inst.base;
}

const unsigned char* SCMOInstance::getHostName() const
{
  return _getCharString(inst.hdr->hostName,inst.base);
}

void SCMOInstance::setHostName(const unsigned char* hostName)
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

const unsigned char* SCMOInstance::getClassName() const
{
    return _getCharString(
        inst.hdr->theClass->cls.hdr->className,
        inst.hdr->theClass->cls.base);        
}

const unsigned char* SCMOInstance::getNameSpace() const
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


    inst.base = (unsigned char*)malloc(SCMB_INITIAL_MEMORY_CHUNK_SIZE);
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
          sizeof(SCMBDataPtr)*inst.hdr->numberKeyBindings,
          &inst.mem,
          true);

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

    // Allocate the SCMBInstancePropertyArray
    _getFreeSpace(
        inst.hdr->propertyArray,
        sizeof(SCMBValue)*inst.hdr->numberProperties,
        &inst.mem,
        true);

}

SCMO_RC SCMOInstance::getProperty(
    const char* name,
    CIMType& type,
    const void** pvalue,
    Boolean& isArray,
    Uint32& size ) const
{
    Uint32 node;
    const unsigned char** pname;
    SCMO_RC rc = SCMO_OK;

    *pvalue = NULL;
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

    return  _getPropertyAtNodeIndex(node,pname,type,pvalue,isArray,size);
}

SCMO_RC SCMOInstance::getPropertyAt(
        Uint32 idx,
        const unsigned char** pname,
        CIMType& type,
        const void** pvalue,
        Boolean& isArray,
        Uint32& size ) const
{
    *pname = NULL;
    *pvalue = NULL;
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

    return  _getPropertyAtNodeIndex(node,pname,type,pvalue,isArray,size);
}

SCMO_RC SCMOInstance::getPropertyNodeIndex(const char* name, Uint32& node) const
{
    if(name==NULL)
    {
        return SCMO_INVALID_PARAMETER;
    }

    return (inst.hdr->theClass->_getProperyNodeIndex(node,name));
}
SCMO_RC SCMOInstance::setPropertyWithOrigin(
    const char* name,
    CIMType type,
    void* value,
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

    
    _setPropertyAtNodeIndex(node,type,value,isArray,size);

    return SCMO_OK;
}

 SCMO_RC SCMOInstance::setPropertyWithNodeIndex(
     Uint32 node,
     CIMType type,
     void* value,
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
             // The named propery is not part of this instance
             // due to filtering.
             return SCMO_NOT_FOUND;
         }
     }

     // Is the traget type OK ?
     rc = inst.hdr->theClass->_isNodeSameType(node,type,isArray);
     if (rc != SCMO_OK)
     {
         return rc;
     }    

     _setPropertyAtNodeIndex(node,type,value,isArray,size);

     return SCMO_OK;
 }

void SCMOInstance::_setPropertyAtNodeIndex(
    Uint32 node,
    CIMType type,
    void* value,
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

    if (value==NULL)
    {
        theInstPropNodeArray[node].flags.isNull=true;
    }
    else
    {
        Uint64 start =
            (unsigned const char*)&(theInstPropNodeArray[node].value)-inst.base;

        _setSCMBUnion(value,type,isArray,size,start);
    }
}

void SCMOInstance::_setSCMBUnion(
    void* value,
    CIMType type,
    Boolean isArray,
    Uint32 size,
    Uint64 start)
{
    SCMBUnion* u = (SCMBUnion*)&(inst.base[start]);

    switch (type)
    {
    case CIMTYPE_BOOLEAN:
        {
            if (isArray)
            {
                _setBinary(value,size*sizeof(Boolean),
                           u->_arrayValue,
                           &inst.mem );
            }
            else
            {
                u->_booleanValue = *((Boolean*)value);
            }
            break;
        }

    case CIMTYPE_UINT8:
        {
            if (isArray)
            {
                _setBinary(value,size*sizeof(Uint8),
                           u->_arrayValue,
                           &inst.mem );
            }
            else
            {
                u->_uint8Value = *((Uint8*)value);
            }
            break;
        }

    case CIMTYPE_SINT8:
        {
            if (isArray)
            {
                _setBinary(value,size*sizeof(Sint8),
                           u->_arrayValue,
                           &inst.mem );
            }
            else
            {
                u->_sint8Value = *((Sint8*)value);
            }
            break;
        }

    case CIMTYPE_UINT16:
        {
            if (isArray)
            {
                _setBinary(value,size*sizeof(Uint16),
                           u->_arrayValue,
                           &inst.mem );
            }
            else
            {
                u->_uint16Value = *((Uint16*)value);
            }
            break;
        }

    case CIMTYPE_SINT16:
        {
            if (isArray)
            {
                _setBinary(value,size*sizeof(Sint16),
                           u->_arrayValue,
                           &inst.mem );
            }
            else
            {
                u->_sint16Value = *((Sint16*)value);
            }
            break;
        }

    case CIMTYPE_UINT32:
        {
            if (isArray)
            {
                _setBinary(value,size*sizeof(Uint32),
                           u->_arrayValue,
                           &inst.mem );
            }
            else
            {
                u->_uint32Value = *((Uint32*)value);
            }
            break;
        }

    case CIMTYPE_SINT32:
        {
            if (isArray)
            {
                _setBinary(value,size*sizeof(Sint32),
                           u->_arrayValue,
                           &inst.mem );
            }
            else
            {
                u->_sint32Value = *((Sint32*)value);
            }
            break;
        }

    case CIMTYPE_UINT64:
        {
            if (isArray)
            {
                _setBinary(value,size*sizeof(Uint64),
                           u->_arrayValue,
                           &inst.mem );
            }
            else
            {
                u->_uint64Value = *((Uint64*)value);
            }
            break;
        }

    case CIMTYPE_SINT64:
        {
            if (isArray)
            {
                _setBinary(value,size*sizeof(Sint64),
                           u->_arrayValue,
                           &inst.mem );
            }
            else
            {
                u->_sint64Value = *((Sint64*)value);
            }
            break;
        }

    case CIMTYPE_REAL32:
        {
            if (isArray)
            {
                _setBinary(value,size*sizeof(Real32),
                           u->_arrayValue,
                           &inst.mem );
            }
            else
            {
                u->_real32Value = *((Real32*)value);
            }
            break;
        }

    case CIMTYPE_REAL64:
        {
            if (isArray)
            {
                _setBinary(value,size*sizeof(Real64),
                           u->_arrayValue,
                           &inst.mem );
            }
            else
            {
                u->_real64Value = *((Real64*)value);
            }
            break;
        }

    case CIMTYPE_CHAR16:
        {
            if (isArray)
            {
                _setBinary(value,size*sizeof(Char16),
                           u->_arrayValue,
                           &inst.mem );
            }
            else
            {
                u->_char16Value = *((Char16*)value);
            }
            break;
        }

    case CIMTYPE_STRING:
        {
            if (isArray)
            {
                SCMBDataPtr* ptr;
                char** tmp;
                Uint64 startPtr;

                startPtr = _getFreeSpace(
                    u->_arrayValue,
                    size*sizeof(SCMBDataPtr),
                    &inst.mem,false);
                // the value is pointer to an array of char*
                tmp = (char**)value;

                for (Uint32 i = 0; i < size; i++)
                {
                    ptr = (SCMBDataPtr*)&(inst.base[startPtr]);
                    // Copy the sting including the trailing '\0'
                    _setBinary(tmp[i],strlen(tmp[i])+1,ptr[i],&inst.mem );
                }
            }
            else
            {
                // Copy the sting including the trailing '\0'
                _setBinary(
                    value,
                    strlen((char*)value)+1,
                    u->_stringValue,
                    &inst.mem );
            }
            break;
        }

    case CIMTYPE_DATETIME:
        {
            if (isArray)
            {
                _setBinary(value,size*sizeof(SCMBDateTime),
                           u->_arrayValue,
                           &inst.mem );
            }
            else
            {
                memcpy(&u->_dateTimeValue,value,sizeof(SCMBDateTime));
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

SCMO_RC SCMOInstance::_getPropertyAtNodeIndex(
        Uint32 node,
        const unsigned char** pname,
        CIMType& type,
        const void** pvalue,
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
            (unsigned const char*)&(theInstPropNodeArray[node].value) -
            inst.base;

        // the caller has to copy the value !
        *pvalue = _getSCMBUnion(type,isArray,size,start,inst.base);

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
        (unsigned const char*)
               &(theClassPropNodeArray[node].theProperty.defaultValue.value) -
        (inst.hdr->theClass->cls.base);

    *pvalue = _getSCMBUnion(
        type,
        isArray,
        size,
        start,
        (inst.hdr->theClass->cls.base)
        );

    return SCMO_OK;

}

SCMOInstance SCMOInstance::clone() const
{
    SCMOInstance newInst;
    newInst.inst.base = (unsigned char*)malloc(this->inst.mem->totalSize);
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

Uint32 SCMOInstance::getPropertyCount() const
{
    return(inst.hdr->numberProperties);
}

const void* SCMOInstance::_getSCMBUnion(
    CIMType type,
    Boolean isArray,
    Uint32 size,
    Uint64 start,
    unsigned char* base) const
{

    SCMBUnion* u = (SCMBUnion*)&(base[start]);

    void* av = NULL;
    if (isArray)
    {
        av = (void*)&base[u->_arrayValue.start];
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
                return ((void*)av);
            }
            else
            {
                return((void*)u);
            }
            break;
        }

    case CIMTYPE_STRING:
        {
            SCMBDataPtr *ptr;
            unsigned char** tmp;

            if (isArray)
            {
                // allocate an array of char* pointers.
                *tmp = (unsigned char*)malloc(size*sizeof(unsigned char*));
                if (*tmp == NULL )
                {
                    throw PEGASUS_STD(bad_alloc)();
                }

                // use temporary variables to avoid casting
                ptr = (SCMBDataPtr*)av;

                for(Uint32 i = 0; i < size; i++)
                {
                    // resolv relative pointer to absolute pointer
                    tmp[i] = (unsigned char*)_getCharString(ptr[i],base);
                }

                return((void*)*tmp);
            }
            else
            {
                return(_getCharString(u->_stringValue,base));
            }


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

Uint32 SCMOInstance::getKeyBindingCount()
{
    return(inst.hdr->numberKeyBindings);
}


SCMO_RC SCMOInstance::getKeyBindingAt(
        Uint32 node,
        const unsigned char** pname,
        CIMKeyBinding::Type& type,
        const unsigned char** pvalue) const
{
    *pname = NULL;
    *pvalue = NULL;

    if (node >= inst.hdr->numberKeyBindings)
    {
        return SCMO_INDEX_OUT_OF_BOUND;
    }

    return _getKeyBindingAtNodeIndex(node,pname,type,pvalue);
}

SCMO_RC SCMOInstance::getKeyBinding(
    const char* name,
    CIMKeyBinding::Type& type,
    const unsigned char** pvalue) const
{
    pvalue = NULL;
    SCMO_RC rc;
    Uint32 node;
    const unsigned char** pname;

    rc = inst.hdr->theClass->_getKeyBindingNodeIndex(node,name);
    if (rc != SCMO_OK)
    {
        return rc;
    }

    return _getKeyBindingAtNodeIndex(node,pname,type,pvalue);

}

SCMO_RC SCMOInstance::_getKeyBindingAtNodeIndex(
    Uint32 node,
    const unsigned char** pname,
    CIMKeyBinding::Type& type,
    const unsigned char** pvalue) const
{

    SCMBDataPtr* theInstKeyBindNodeArray =
        (SCMBDataPtr*)&inst.base[inst.hdr->keyBindingArray.start];

    // create a pointer to keybinding node array of the class.
    Uint64 idx = inst.hdr->theClass->cls.hdr->keyBindingSet.nodeArray.start;
    SCMBKeyBindingNode* theClassKeyBindNodeArray =
        (SCMBKeyBindingNode*)&(inst.hdr->theClass->cls.base)[idx];

    type = theClassKeyBindNodeArray->type;
    *pname = _getCharString(
        theClassKeyBindNodeArray->name,
        inst.hdr->theClass->cls.base);

        // There is no value set in the instance
    // if the relative pointer has no start value.
    if (theInstKeyBindNodeArray[node].start==0)
    {
        return SCMO_NULL_VALUE;
    }

    // Set the absolut pointer to the key binding value
    *pvalue = _getCharString(theInstKeyBindNodeArray[node],inst.base);

    return SCMO_OK;

}

SCMO_RC SCMOInstance::setKeyBinding(
    const char* name,
    CIMKeyBinding::Type type,
    const char* pvalue)
{
    SCMO_RC rc;
    Uint32 node;

    rc = inst.hdr->theClass->_getKeyBindingNodeIndex(node,name);
    if (rc != SCMO_OK)
    {
        return rc;
    }

   // create a pointer to keybinding node array of the class.
    Uint64 idx = inst.hdr->theClass->cls.hdr->keyBindingSet.nodeArray.start;
    SCMBKeyBindingNode* theClassKeyBindNodeArray =
        (SCMBKeyBindingNode*)&(inst.hdr->theClass->cls.base)[idx];

    if (theClassKeyBindNodeArray[node].type != type)
    {
        return SCMO_TYPE_MISSMATCH;
    }

    SCMBDataPtr* theInstKeyBindNodeArray =
        (SCMBDataPtr*)&inst.base[inst.hdr->keyBindingArray.start];

    // copy the value including trailing '\0'
    _setBinary(pvalue,strlen(pvalue)+1,theInstKeyBindNodeArray[node],&inst.mem);

    return SCMO_OK;

}

void SCMOInstance::setPropertyFilter(const char **propertyList)
{
    SCMO_RC rc;
    Uint32 node,i = 0;

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
            sizeof(Uint32)*inst.hdr->filterProperties);

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

        if (rc == SCMO_OK)
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

void SCMODump::dumpSCMOClass(SCMOClass& testCls) const
{
    SCMBClass_Main* clshdr = testCls.cls.hdr;
    unsigned char* clsbase = testCls.cls.base;

    printf("\n\nDump of SCMOClass\n");
    // The magic number for SCMO class
    printf("\nheader.magic=%08X",clshdr->header.magic);
    // Total size of the instance memory block( # bytes )
    printf("\nheader.totalSize=%llu",clshdr->header.totalSize);
    // The # of bytes free
    printf("\nheader.freeBytes=%llu",clshdr->header.freeBytes);
    // Index to the start of the free space in this insance
    printf("\nheader.StartOfFreeSpace=%llu",clshdr->header.startOfFreeSpace);
    // The reference counter for this class
    printf("\nrefCount=%i",clshdr->refCount.get());

    printf("\n\nsuperClassName: \'%s\'",
           _getCharString(clshdr->superClassName,clsbase));
    printf("\nnameSpace: \'%s\'",_getCharString(clshdr->nameSpace,clsbase));
    printf("\nclassName: \'%s\'",_getCharString(clshdr->className,clsbase));
    printf("\n\nTheClass qualfiers:");
    _dumpQualifierArray(
        clshdr->qualifierArray.start,
        clshdr->numberOfQualifiers,
        clsbase);
    printf("\n");
    dumpKeyPropertyMask(testCls);
    printf("\n");
    dumpKeyIndexList(testCls);
    printf("\n");
    dumpClassProperties(testCls);
    printf("\n");
    dumpKeyBindingSet(testCls);
    printf("\n");
    /*
    */
    printf("\n");

}

void SCMODump::dumpSCMOClassQualifiers(SCMOClass& testCls) const
{
    SCMBClass_Main* clshdr = testCls.cls.hdr;
    unsigned char* clsbase = testCls.cls.base;

    printf("\n\nTheClass qualfiers:");
    _dumpQualifierArray(
        clshdr->qualifierArray.start,
        clshdr->numberOfQualifiers,
        clsbase);
    printf("\n");

}

void SCMODump::hexDumpSCMOClass(SCMOClass& testCls) const
{
    unsigned char* tmp;

    SCMBClass_Main* clshdr = testCls.cls.hdr;
    unsigned char* clsbase = testCls.cls.base;

    printf("\n\nHex dump of a SCMBClass:");
    printf("\n========================");
    printf("\n\n Size of SCMBClass: %llu",clshdr->header.totalSize);
    printf("\n cls.base = %p\n\n",clsbase);

    _hexDump(clsbase,clshdr->header.totalSize);

}
void SCMODump::dumpKeyIndexList(SCMOClass& testCls) const
{
    SCMBClass_Main* clshdr = testCls.cls.hdr;
    unsigned char* clsbase = testCls.cls.base;

    printf("\n\nKey Index List:");
    printf("\n===============\n");

    // Get absolut pointer to key index list of the class
    Uint32* keyIndex = (Uint32*)&(clsbase)[clshdr->keyIndexList.start];
    Uint32 line,j,i;
    for (j = 0; j < clshdr->propertySet.number; j = j + line)
    {
        if ((clshdr->propertySet.number-j)/16)
        {
            line = 16 ;
        }
        else
        {
            line = clshdr->propertySet.number%16;
        }


        printf("Index :");
        for (i = 0; i < line; i++)
        {
            printf(" %3u",j+i);
        }

        printf("\nNode  :");
        for (i = 0; i < line; i++)
        {
            printf(" %3u",keyIndex[j+i]);
        }

        printf("\n\n");

    }

}

void SCMODump::dumpKeyBindingSet(SCMOClass& testCls) const
{
    SCMBClass_Main* clshdr = testCls.cls.hdr;
    unsigned char* clsbase = testCls.cls.base;

    printf("\n\nKey Binding Set:");
    printf("\n=================\n");
    printf("\nNumber of key bindings: %3u",clshdr->keyBindingSet.number);
    dumpHashTable(
        clshdr->keyBindingSet.hashTable,
        PEGASUS_KEYBINDIG_SCMB_HASHSIZE);

    dumpClassKeyBindingNodeArray(testCls);

}

void SCMODump::dumpClassKeyBindingNodeArray(SCMOClass& testCls) const
{
    SCMBClass_Main* clshdr = testCls.cls.hdr;
    unsigned char* clsbase = testCls.cls.base;

    SCMBKeyBindingNode* nodeArray =
        (SCMBKeyBindingNode*)
             &(clsbase[clshdr->keyBindingSet.nodeArray.start]);

    for (Uint32 i = 0; i <  clshdr->keyBindingSet.number; i++)
    {
        printf("\n\n===================");
        printf("\nKey Binding #%3u",i);
        printf("\n===================");

        printf("\nHas next: %s",(nodeArray[i].hasNext?"TRUE":"FALSE"));
        if (nodeArray[i].hasNext)
        {
            printf("\nNext Node: %3u",nodeArray[i].nextNode);
        }
        else
        {
            printf("\nNext Node: N/A");
        }

        printf("\nKey Property name: %s",
               _getCharString(nodeArray[i].name,clsbase));

        printf("\nHash Tag %3u Hash Index %3u",
               nodeArray[i].nameHashTag,
               nodeArray[i].nameHashTag%PEGASUS_KEYBINDIG_SCMB_HASHSIZE);

        printf("\nKey binding type: %s",
               XmlWriter::keyBindingTypeToString(nodeArray[i].type).str);

    }

}

void SCMODump::dumpClassProperties(SCMOClass& testCls) const
{
    SCMBClass_Main* clshdr = testCls.cls.hdr;
    unsigned char* clsbase = testCls.cls.base;

    printf("\n\nClass Properties:");
    printf("\n=================\n");
    printf("\nNumber of properties: %3u",clshdr->propertySet.number);
    dumpHashTable(
        clshdr->propertySet.hashTable,
        PEGASUS_PROPERTY_SCMB_HASHSIZE);
    dumpClassPropertyNodeArray(testCls);

}

void SCMODump::dumpClassPropertyNodeArray(SCMOClass& testCls) const
{
    SCMBClass_Main* clshdr = testCls.cls.hdr;
    unsigned char* clsbase = testCls.cls.base;

    SCMBClassPropertyNode* nodeArray =
        (SCMBClassPropertyNode*)
            &(clsbase[clshdr->propertySet.nodeArray.start]);

    for (Uint32 i = 0; i <  clshdr->propertySet.number; i++)
    {

        printf("\n\n===================");
        printf("\nClass property #%3u",i);
        printf("\n===================");

        printf("\nHas next: %s",(nodeArray[i].hasNext?"TRUE":"FALSE"));
        if (nodeArray[i].hasNext)
        {
            printf("\nNext Node: %3u",nodeArray[i].nextNode);
        }
        else
        {
            printf("\nNext Node: N/A");
        }

        _dumpClassProperty(nodeArray[i].theProperty,clsbase);
    }
}

void SCMODump::_dumpClassProperty(
    const SCMBClassProperty& prop,
    unsigned char* clsbase) const
{
    printf("\nProperty name: %s",_getCharString(prop.name,clsbase));

    printf("\nHash Tag %3u Hash Index %3u",
           prop.nameHashTag,
           prop.nameHashTag%PEGASUS_PROPERTY_SCMB_HASHSIZE);
    printf("\nPropagated: %s isKey: %s",
           (prop.flags.propagated?"TRUE":"FALSE"),
           (prop.flags.isKey?"TRUE":"FALSE")
           );

    printf("\nOrigin class name: %s",
           _getCharString(prop.originClassName,clsbase));
    printf("\nReference class name: %s",
           _getCharString(prop.refClassName,clsbase));

    printSCMOValue(prop.defaultValue,clsbase);

    _dumpQualifierArray(
        prop.qualifierArray.start,
        prop.numberOfQualifiers,
        clsbase);

}

void SCMODump::dumpHashTable(Uint32* hashTable,Uint32 size) const
{
    Uint32 i,j,line;
    printf("\n\nHash table:\n");


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


        printf("Index    :");
        for (i = 0; i < line; i++)
        {
            printf(" %3u",j+i);
        }

        printf("\nHashTable:");
        for (i = 0; i < line; i++)
        {
            printf(" %3u",hashTable[j+i]);
        }

        printf("\n\n");

    }


}

void SCMODump::_dumpQualifierArray(
    Uint64 start, 
    Uint32 size,
    unsigned char* clsbase
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
    unsigned char* clsbase
    ) const
{
     if(theQualifier.name == QUALNAME_USERDEFINED)
     {
         printf("\n\nQualifier user defined name: \'%s\'",
                _getCharString(theQualifier.userDefName,clsbase));
     }
     else
     {
         printf("\n\nQualifier DMTF defined name: \'%s\'",
                _qualifierNameStrLit[theQualifier.name].str);
     }

     printf("\nPropagated : %s",
            (theQualifier.propagated ? "True" : "False"));
     printf("\nFlavor : %s",
         (const char*)(CIMFlavor(theQualifier.flavor).toString().getCString()));

     printSCMOValue(theQualifier.value,clsbase);

}

void SCMODump::printSCMOValue(
    const SCMBValue& theValue,
    unsigned char* base) const
{
   printf("\nValueType : %s",cimTypeToString(theValue.valueType));
   printf("\nValue was set by the provider: %s",
       (theValue.flags.isSet ? "True" : "False"));
   if (theValue.flags.isNull)
   {
       printf("\nIt's a NULL value.");
       return;
   }
   if (theValue.flags.isArray)
   {
       printf("\nThe value is an Array of size: %u",theValue.valueArraySize);
       printf("\nThe values are: '%s'",
              (const char*)printArrayValue(
                  theValue.valueType,
                  theValue.valueArraySize,
                  theValue.value,
                  base).getCString());
   }
   else
   {
      printf("\nThe Value is: '%s'",
          (const char*)
             printUnionValue(theValue.valueType,theValue.value,base)
             .getCString());
   }

   return;

}

String SCMODump::printArrayValue(
    CIMType type,
    Uint32 size, 
    SCMBUnion u,
    unsigned char* base) const
{
    Buffer out;

    switch (type)
    {
    case CIMTYPE_BOOLEAN:
        {
            Boolean* p=(Boolean*)&(base[u._arrayValue.start]);
            for (Uint32 i = 0; i < size; i++)
            {
                _toString(out,p[i]);
                out.append(' ');
            }
            break;
        }

    case CIMTYPE_UINT8:
        {
            Uint8* p=(Uint8*)&(base[u._arrayValue.start]);
            _toString(out,p,size);
            break;
        }

    case CIMTYPE_SINT8:
        {
            Sint8* p=(Sint8*)&(base[u._arrayValue.start]);

            break;
        }

    case CIMTYPE_UINT16:
        {
            Uint16* p=(Uint16*)&(base[u._arrayValue.start]);
            _toString(out,p,size);
            break;
        }

    case CIMTYPE_SINT16:
        {
            Sint16* p=(Sint16*)&(base[u._arrayValue.start]);
            _toString(out,p,size);
            break;
        }

    case CIMTYPE_UINT32:
        {
            Uint32* p=(Uint32*)&(base[u._arrayValue.start]);
            _toString(out,p,size);
            break;
        }

    case CIMTYPE_SINT32:
        {
            Sint32* p=(Sint32*)&(base[u._arrayValue.start]);
            _toString(out,p,size);
            break;
        }

    case CIMTYPE_UINT64:
        {
            Uint64* p=(Uint64*)&(base[u._arrayValue.start]);
            _toString(out,p,size);
            break;
        }

    case CIMTYPE_SINT64:
        {
            Sint64* p=(Sint64*)&(base[u._arrayValue.start]);
            _toString(out,p,size);
            break;
        }

    case CIMTYPE_REAL32:
        {
            Real32* p=(Real32*)&(base[u._arrayValue.start]);
            _toString(out,p,size);
            break;
        }

    case CIMTYPE_REAL64:
        {
            Real64* p=(Real64*)&(base[u._arrayValue.start]);
            _toString(out,p,size);
            break;
        }

    case CIMTYPE_CHAR16:
        {
            Char16* p=(Char16*)&(base[u._arrayValue.start]);
            _toString(out,p,size);
            break;
        }

    case CIMTYPE_STRING:
        {
            SCMBDataPtr* p = (SCMBDataPtr*)&(base[u._arrayValue.start]);
            for (Uint32 i = 0; i < size; i++)
            {
                out.append((const char*)_getCharString(p[i],base),
                           p[i].length-1);
                out.append(' ');
            }
            break;
        }

    case CIMTYPE_DATETIME:
        {
            SCMBDateTime* p = (SCMBDateTime*)&(base[u._arrayValue.start]);
            CIMDateTime x;
            for (Uint32 i = 0; i < size; i++)
            {
                memcpy(x._rep,&(p[i]),sizeof(SCMBDateTime));
                _toString(out,x);
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
    unsigned char* base) const
{
    Buffer out;

    switch (type)
    {
    case CIMTYPE_BOOLEAN:
        {
            _toString(out,u._booleanValue);
            break;
        }

    case CIMTYPE_UINT8:
        {
            _toString(out,u._uint8Value);
            break;
        }

    case CIMTYPE_SINT8:
        {
            _toString(out,u._sint8Value);
            break;
        }

    case CIMTYPE_UINT16:
        {
            _toString(out,(Uint32)u._uint16Value);
            break;
        }

    case CIMTYPE_SINT16:
        {
            _toString(out,u._sint16Value);
            break;
        }

    case CIMTYPE_UINT32:
        {
            _toString(out,u._uint32Value);
            break;
        }

    case CIMTYPE_SINT32:
        {
            _toString(out,u._sint32Value);
            break;
        }

    case CIMTYPE_UINT64:
        {
            _toString(out,u._uint64Value);
            break;
        }

    case CIMTYPE_SINT64:
        {
            _toString(out,u._sint64Value);
            break;
        }

    case CIMTYPE_REAL32:
        {
            _toString(out,u._real32Value);
            break;
        }

    case CIMTYPE_REAL64:
        {
            _toString(out,u._real32Value);
            break;
        }

    case CIMTYPE_CHAR16:
        {
            _toString(out,u._char16Value);
            break;
        }

    case CIMTYPE_STRING:
        {
            out.append((const char*)_getCharString(u._stringValue,base),
                       u._stringValue.length-1);
            break;
        }

    case CIMTYPE_DATETIME:
        {
            CIMDateTime x;
            memcpy(x._rep,&(u._dateTimeValue),sizeof(SCMBDateTime));
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

void SCMODump::dumpKeyPropertyMask(SCMOClass& testCls ) const
{

    SCMBClass_Main* clshdr = testCls.cls.hdr;
    unsigned char* clsbase = testCls.cls.base;

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

         printf("\nkeyPropertyMask[%02u]= ",i);

         for (Uint32 j = 0; j < end; j++)
         {
             if (j > 0 && !(j%8))
             {
                 printf(" ");
             }

             if (theKeyMask[i] & printMask)
             {
                 printf("1");
             }
             else
             {
                 printf("0");
             }

             printMask = printMask << 1;
         }
         printf("\n");
     }
}

void SCMODump::_hexDump(unsigned char* buffer,int length) const
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
                        printf("%c",printLine[y][x]);
                    }
                    else
                    {
                        printf("%1X",printLine[y][x]);
                    }
                }
                printf("\n");
            }
            printf("\n");
        }

        item = buffer[i];

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

/*****************************************************************************
 * The constant functions
 *****************************************************************************/

static const void* _resolveDataPtr(
    const SCMBDataPtr& ptr,
    unsigned char* base)
{
    return ((ptr.start==(Uint64)0 ? NULL : (void*)&(base[ptr.start])));
}

PEGASUS_COMMON_LINKAGE const unsigned char* _getCharString(
    const SCMBDataPtr& ptr,
    unsigned char* base)
{
    return ((ptr.start==(Uint64)0 ? NULL : &(base[ptr.start])));
}


static Uint32 _generateSCMOStringTag(
    const SCMBDataPtr& ptr,
    unsigned char* base)
{
    // The lenght of a SCMBDataPtr to a UTF8 string includs the trailing '\0'.
    return _generateStringTag(_getCharString(ptr,base),ptr.length-1);
}

static Uint32 _generateStringTag(const unsigned char* str, Uint32 len)
{
    if (len == 0)
    {
        return 0;
    }
    return
        (Uint32(CharSet::toUpperHash(str[0]) << 1) |
        Uint32(CharSet::toUpperHash(str[len-1])));
}

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

static Boolean _equalUTF8Strings(
    const SCMBDataPtr& ptr_a,
    unsigned char* base,
    const char* name,
    Uint32 len)

{
    // size without trailing '\0' !!
    if (ptr_a.length-1 != len)
    {
        return false;
    }

    const char* a = (const char*)_getCharString(ptr_a,base);

    // ToDo: Here an UTF8 complinet comparison should take place
    return ( strncmp(a,name,len )== 0 );

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
        (*pmem)->freeBytes= (*pmem)->freeBytes + oldSize;
        (*pmem)->totalSize= (*pmem)->totalSize + oldSize;
    }

    (*pmem)->freeBytes = (*pmem)->freeBytes - size;
    (*pmem)->startOfFreeSpace = (*pmem)->startOfFreeSpace + size;

    if (clear)
    {
        // If requested, set memory to 0.
        memset(&((unsigned char*)(*pmem))[start],0,size);
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
       memcpy(&((unsigned char*)(*pmem))[start],(const char*)theCString,length);
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
    if (bufferSize != 1)
    {

        Uint64 start;
        // Attention ! In this function a reallocation may take place.
        // The reference ptr may be unusable after the call to _getFreeSpace
        // --> use the returned start index.
        start = _getFreeSpace(ptr , bufferSize, pmem);
        // Copy buffer into SCMB
        memcpy(
            &((unsigned char*)(*pmem))[start],
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
