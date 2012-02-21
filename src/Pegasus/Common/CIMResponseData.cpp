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

#include "CIMResponseData.h"
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/SCMOXmlWriter.h>
#include <Pegasus/Common/XmlReader.h>
#include <Pegasus/Common/CIMInternalXmlEncoder.h>
#include <Pegasus/Common/SCMOInternalXmlEncoder.h>

// KS_TODO_DELETE
#include <Pegasus/Common/Print.h>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

#define LOCAL_MIN(a, b) ((a < b) ? a : b)
// C++ objects interface handling

// KS_TODO Remove this completely.
bool CIMResponseData::sizeValid()
{
    TRACELINE; 
    //////cout << _size << endl;
    PEGASUS_ASSERT(valid());
    if (_size > 1000000)
    {
        TRACELINE;
        PEG_TRACE((TRC_XML, Tracer::LEVEL4,
                   "CIMResponseData::PSVALID _size too big %u",_size ));
        return false;
    }
    PEG_TRACE((TRC_XML, Tracer::LEVEL4,
        "CIMResponseData Size _size=%u", _size));
    return true;
}
// Instance Names handling
Array<CIMObjectPath>& CIMResponseData::getInstanceNames()
{
    TRACELINE;
    PSVALID;
    PEGASUS_DEBUG_ASSERT(
    (_dataType==RESP_INSTNAMES || _dataType==RESP_OBJECTPATHS));
    _resolveToCIM();
    PEGASUS_DEBUG_ASSERT(_encoding==RESP_ENC_CIM || _encoding == 0);
    return _instanceNames;
}

// Get a single instance as a CIM instance.
// This converts all of the objects in the response data to
// CIM form as part of the conversion.
// If there are no instances in the object, returns CIMInstance(),
// an empty instance.
CIMInstance& CIMResponseData::getInstance()
{
    TRACELINE;
    PEGASUS_DEBUG_ASSERT(_dataType == RESP_INSTANCE);
    _resolveToCIM();
    if (0 == _instances.size())
    {
        _instances.append(CIMInstance());
    }
    return _instances[0];
}

// Instances handling
Array<CIMInstance>& CIMResponseData::getInstances()
{
    TRACELINE;
    PEGASUS_DEBUG_ASSERT(_dataType == RESP_INSTANCES);
    _resolveToCIM();
    return _instances;
}

// Instances handling specifically for the client where the call may
// get either instances or objects and must convert them to instances
// NOTE: This is a temporary solution to satisfy the BinaryCodec passing
// of data to the client where the data could be either instances or
// objects.  The correct solution is to convert back when the provider, etc.
// returns the data to the server.  We must convert to that solution but
// this keeps it working for the moment.
Array<CIMInstance>& CIMResponseData::getInstancesFromInstancesOrObjects()
{
    TRACELINE;
    if (_dataType == RESP_INSTANCES)
    {
        _resolveToCIM();
        return _instances;
    }
    else if (_dataType == RESP_OBJECTS)
    {
        _resolveToCIM();
        for (Uint32 i = 0 ; i < _objects.size() ; i++)
        {
            _instances.append((CIMInstance)_objects[i]);
        }
        return _instances;

    }
    PEGASUS_DEBUG_ASSERT(false);
}

// Objects handling
Array<CIMObject>& CIMResponseData::getObjects()
{
    TRACELINE;
    PEGASUS_DEBUG_ASSERT(_dataType == RESP_OBJECTS);
    _resolveToCIM();
    return _objects;
}

// SCMO representation, single instance stored as one element array
// object paths are represented as SCMOInstance
Array<SCMOInstance>& CIMResponseData::getSCMO()
{
    TRACELINE;
    _resolveToSCMO();
    return _scmoInstances;
}

// set an array of SCMOInstances into the response data object
void CIMResponseData::setSCMO(const Array<SCMOInstance>& x)
{
    TRACELINE;
    PSVALID;
    _scmoInstances=x;
    _encoding |= RESP_ENC_SCMO;
    _size += x.size();
}

// Binary data is just a data stream
Array<Uint8>& CIMResponseData::getBinary()
{
    TRACELINE;
    PEGASUS_DEBUG_ASSERT(_encoding == RESP_ENC_BINARY || _encoding == 0);
    return _binaryData;
}

bool CIMResponseData::setBinary(CIMBuffer& in)
{
    TRACELINE;
    PEG_METHOD_ENTER(TRC_DISPATCHER, "CIMResponseData::setBinary");

    // Append all serial data from the CIMBuffer to the local data store.
    // Returns error if input not a serialized Uint8A
    if (!in.getUint8A(_binaryData))
    {
        PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
            "Failed to get binary input data!");
        PEG_METHOD_EXIT();
        return false;
    }
    _encoding |= RESP_ENC_BINARY;
    PEG_METHOD_EXIT();
    return true;
}

bool CIMResponseData::setRemainingBinaryData(CIMBuffer& in)
{
    TRACELINE;
    PEG_METHOD_ENTER(TRC_DISPATCHER, "CIMResponseData::setRemainingBinaryData");

    // Append any data that has not been deserialized already from
    // the CIMBuffer.
    size_t remainingDataLength = in.remainingDataLength();
    _binaryData.append((Uint8*)in.getPtr(), remainingDataLength);

    _encoding |= RESP_ENC_BINARY;
    PEG_METHOD_EXIT();
    return true;
}

bool CIMResponseData::setXml(CIMBuffer& in)
{
    TRACELINE;
    PSVALID;
    switch (_dataType)
    {
        case RESP_INSTANCE:
        {
            Array<Sint8> inst;
            Array<Sint8> ref;
            CIMNamespaceName ns;
            String host;
            if (!in.getSint8A(inst))
            {
                PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
                    "Failed to get XML instance data!");
                return false;
            }
            _instanceData.insert(0,inst);
            if (!in.getSint8A(ref))
            {
                PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
                    "Failed to get XML instance data (reference)!");
                return false;
            }
            _referencesData.insert(0,ref);
            if (!in.getString(host))
            {
                PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
                    "Failed to get XML instance data (host)!");
                return false;
            }
            _hostsData.insert(0,host);
            if (!in.getNamespaceName(ns))
            {
                PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
                    "Failed to get XML instance data (namespace)!");
                return false;
            }
            _nameSpacesData.insert(0,ns);
            _size++;
            break;
        }
        case RESP_INSTANCES:
        {
            Uint32 count;
            if (!in.getUint32(count))
            {
                PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
                    "Failed to get XML instance data (number of instance)!");
                return false;
            }
            for (Uint32 i = 0; i < count; i++)
            {
                Array<Sint8> inst;
                Array<Sint8> ref;
                CIMNamespaceName ns;
                String host;
                if (!in.getSint8A(inst))
                {
                    PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
                        "Failed to get XML instance data (instances)!");
                    return false;
                }
                if (!in.getSint8A(ref))
                {
                    PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
                        "Failed to get XML instance data (references)!");
                    return false;
                }
                if (!in.getString(host))
                {
                    PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
                        "Failed to get XML instance data (host)!");
                    return false;
                }
                if (!in.getNamespaceName(ns))
                {
                    PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
                        "Failed to get XML instance data (namespace)!");
                    return false;
                }
                _instanceData.append(inst);
                _referencesData.append(ref);
                _hostsData.append(host);
                _nameSpacesData.append(ns);
            }
            _size += count;
            break;
        }
        case RESP_OBJECTS:
        {
            Uint32 count;
            if (!in.getUint32(count))
            {
                PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
                    "Failed to get XML object data (number of objects)!");
                return false;
            }
            for (Uint32 i = 0; i < count; i++)
            {
                Array<Sint8> obj;
                Array<Sint8> ref;
                CIMNamespaceName ns;
                String host;
                if (!in.getSint8A(obj))
                {
                    PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
                        "Failed to get XML object data (object)!");
                    return false;
                }
                if (!in.getSint8A(ref))
                {
                    PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
                        "Failed to get XML object data (reference)!");
                    return false;
                }
                if (!in.getString(host))
                {
                    PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
                        "Failed to get XML object data (host)!");
                    return false;
                }
                if (!in.getNamespaceName(ns))
                {
                    PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
                        "Failed to get XML object data (namespace)!");
                    return false;
                }
                _instanceData.append(obj);
                _referencesData.append(ref);
                _hostsData.append(host);
                _nameSpacesData.append(ns);
            }
            _size += count;
            break;
        }
        // internal xml encoding of instance names and object paths not
        // done today
        case RESP_INSTNAMES:
        case RESP_OBJECTPATHS:
        default:
        {
            PEGASUS_DEBUG_ASSERT(false);
        }
    }
    _encoding |= RESP_ENC_XML;
    return true;
}

// Move the number of objects defined by the input parameter from
// one CIMResponse Object to another CIMResponse Object.
Uint32 CIMResponseData::moveObjects(CIMResponseData & from, Uint32 count)
{
    TRACELINE;
    PEG_TRACE((TRC_XML, Tracer::LEVEL3,
        "CIMResponseData::move(%u)", count));

    PEGASUS_ASSERT(valid());                 // KS_TEMP
    if (_dataType != from._dataType)         // KS_TEMP
    {
        printf("ERROR moveObjects _dataType %u. from._dataType %u\n",
        _dataType, from._dataType);
    }
    PEGASUS_DEBUG_ASSERT(_dataType == from._dataType);
    Uint32 rtnSize = 0;
    Uint32 toMove = count;
//  printf("count to move = %u encoding %u from.size %u to.size %u\n",
//          count, from._encoding, from._size, _size);

    if (RESP_ENC_XML == (from._encoding & RESP_ENC_XML))
    {
        switch (_dataType)
        {
            case RESP_OBJECTPATHS:
            case RESP_INSTNAMES:
                break;
            case RESP_INSTANCE:
                {
                    Uint32 moveCount = toMove;
                    if (from._instanceData.size() > 0)
                    {
                        // temp test to assure all sizes are the same.
                        PEGASUS_ASSERT(from._hostsData.size() ==
                                        from._instanceData.size());
                        PEGASUS_ASSERT(from._referencesData.size() ==
                                        from._instanceData.size());
                        PEGASUS_ASSERT(from._nameSpacesData.size() ==
                                        from._instanceData.size());
                        _instanceData.append(from._instanceData.getData(),1);
                        from._instanceData.remove(0, 1);
                        _referencesData.append(
                            from._referencesData.getData(),1);
                        from._referencesData.remove(0, 1);
                        if (_hostsData.size())
                        {
                            _hostsData.append(from._hostsData.getData(),1);
                            from._hostsData.remove(0, 1);
                        }
                        if (_nameSpacesData.size())
                        {
                            _nameSpacesData.append(
                                from._nameSpacesData.getData(),1);
                            from._nameSpacesData.remove(0, 1);
                        }
                        rtnSize += 1;
                        toMove--;
                        _encoding |= RESP_ENC_XML;
                    }
                }
                break;

            // The above should probably be folded into the following.
            // Need something like an assert if there is ever more than
            // one instance in _instanceData for type RESP_INSTANCE
            case RESP_INSTANCES:
            case RESP_OBJECTS:
                {
                    Uint32 moveCount = LOCAL_MIN(toMove,
                                                 from._instanceData.size());

                    PEGASUS_ASSERT(from._referencesData.size() ==
                                    from._instanceData.size());
                    _instanceData.append(from._instanceData.getData(),
                                         moveCount);
                    from._instanceData.remove(0, moveCount);
                    _referencesData.append(from._referencesData.getData(),
                                           moveCount);
                    from._referencesData.remove(0, moveCount);
                    rtnSize += moveCount;
                    toMove -= moveCount;
                    _encoding |= RESP_ENC_XML;
                }
                break;
        }
    }
    if (RESP_ENC_BINARY == (from._encoding & RESP_ENC_BINARY))
    {
        // KS_PULL TBD Add binary move function
        // Cannot resolve this one without actually processing
        // the data since it is a stream.
        rtnSize += 0;
        PEGASUS_ASSERT(false);
    }

    if (RESP_ENC_SCMO == (from._encoding & RESP_ENC_SCMO))
    {
        Uint32 moveCount = LOCAL_MIN(toMove, from._scmoInstances.size());

        _scmoInstances.append(from._scmoInstances.getData(), moveCount);
        from._scmoInstances.remove(0, moveCount);
        rtnSize += moveCount;
        toMove -= moveCount;
        _encoding |= RESP_ENC_SCMO;
    }

    if (RESP_ENC_CIM == (from._encoding & RESP_ENC_CIM))
    {
        switch (_dataType)
        {
            case RESP_OBJECTPATHS:
            case RESP_INSTNAMES:
                {
                    Uint32 moveCount = LOCAL_MIN(toMove,
                                                 from._instanceNames.size());

                    _instanceNames.append(
                        from._instanceNames.getData(), moveCount);
                    from._instanceNames.remove(0, moveCount);
                    rtnSize += moveCount;
                    toMove -= moveCount;
                    _encoding |= RESP_ENC_CIM;
                }
                break;
            case RESP_INSTANCE:
            case RESP_INSTANCES:
                {

                    Uint32 moveCount = LOCAL_MIN(toMove,
                                                 from._instances.size());

                    _instances.append(from._instances.getData(), moveCount);
                    from._instances.remove(0, moveCount);
                    rtnSize += moveCount;
                    toMove -= moveCount;
                    _encoding |= RESP_ENC_CIM;
                }
                break;
            case RESP_OBJECTS:
                {
                    Uint32 moveCount = LOCAL_MIN(toMove,
                                                 from._objects.size());
                    _objects.append(from._objects.getData(), moveCount);
                    from._objects.remove(0, moveCount);
                    rtnSize += moveCount;
                    toMove -= moveCount;
                    _encoding |= RESP_ENC_CIM;
                }
                break;
        }
    }
    PEGASUS_ASSERT(rtnSize == (count - toMove));

    _size += rtnSize;
    from._size -= rtnSize;

    if (rtnSize != _size)
    {
        PEG_TRACE((TRC_XML, Tracer::LEVEL1,
            "Size calc error _size %u rtnSWize = %u", _size, rtnSize));
    }
    //PEGASUS_ASSERT(rtnSize == _size);

    return rtnSize;
}

// Return the number of CIM objects in the CIM Response data object
//
#define TEMPLOG PEG_TRACE((TRC_XML, Tracer::LEVEL4, \
 "rtnSize %u size %u", rtnSize, _size))

Uint32 CIMResponseData::size()
{
    TRACELINE;
    PEG_METHOD_ENTER(TRC_XML,"CIMResponseData::size()");
    PSVALID;
// If debug mode, add up all the individual size components to
// determine overall size of this object.  Then compare this with
// the _size variable.  this is a good check on the completeness of the
// size computations.  We should be able to remove this at some point
// but there are many sources of size info and we need to be sure we
// have covered them all.
#ifdef PEGASUS_DEBUG
    PEGASUS_ASSERT(valid());            //KS_TEMP

    Uint32 rtnSize = 0;
    TEMPLOG;
    if (RESP_ENC_XML == (_encoding & RESP_ENC_XML))
    {
        TEMPLOG;
        switch (_dataType)
        {
            case RESP_OBJECTPATHS:
            case RESP_INSTNAMES:
                break;
            case RESP_INSTANCE:
                rtnSize +=1;
                break;
            case RESP_INSTANCES:
            case RESP_OBJECTS:
                rtnSize += _instanceData.size();
                break;
        }
        PSVALID;
        TEMPLOG;
    }
    if (RESP_ENC_BINARY == (_encoding & RESP_ENC_BINARY))
    {
        TEMPLOG;
        // KS_PULL_TODO
        // Cannot resolve this one without actually processing
        // the data since it is a stream.
        rtnSize += 0;
        //PEGASUS_ASSERT(false);
        TEMPLOG;
    }

    if (RESP_ENC_SCMO == (_encoding & RESP_ENC_SCMO))
    {
        PSVALID;
        TEMPLOG;
        rtnSize += _scmoInstances.size();
        TEMPLOG;
    }

    if (RESP_ENC_CIM == (_encoding & RESP_ENC_CIM))
    {
        PSVALID;
        TEMPLOG;
        switch (_dataType)
        {
            case RESP_OBJECTPATHS:
            case RESP_INSTNAMES:
                rtnSize += _instanceNames.size();
                break;
            case RESP_INSTANCE:
            case RESP_INSTANCES:
                rtnSize += _instances.size();
                break;
            case RESP_OBJECTS:
                rtnSize += _objects.size();
                break;
        }
        PSVALID;
        TEMPLOG;
    }
    // Test of actual count against _size variable.
    if (rtnSize != _size)
    {
        PSVALID;
        TEMPLOG;
        PEG_TRACE((TRC_XML, Tracer::LEVEL1,
        "CIMResponseData::size ERROR. debug size mismatch."
            "Computed = %u. variable = %u",rtnSize, _size ));
        // KS_TEMP
        cout << "Size err " << rtnSize << " " << _size << endl;
        TEMPLOG;
    }
    //PEGASUS_TEST_ASSERT(rtnSize == _size);
#endif
    PEG_METHOD_EXIT();
    return _size;
}

// function used by OperationAggregator to aggregate response data in a
// single ResponseData object. Adds all data in the from ResponseData object
// input variable to the target ResponseData object
// target array
void CIMResponseData::appendResponseData(const CIMResponseData & x)
{
    TRACELINE;
    // Confirm that the CIMResponseData type matches the type
    // of the data being appended

    PEGASUS_ASSERT(valid());            // KS_TEMP
    PEGASUS_DEBUG_ASSERT(_dataType == x._dataType);
    _encoding |= x._encoding;

    // add all binary data
    _binaryData.appendArray(x._binaryData);
    // KS_TBD TODO PULL Add the counter incrementer for binary

    // add all the C++ stuff
    _instanceNames.appendArray(x._instanceNames);
    _size += x._instanceNames.size();
    _instances.appendArray(x._instances);
    _size += x._instances.size();
    _objects.appendArray(x._objects);
    _size += x._objects.size();

    // add the SCMO instances
    _scmoInstances.appendArray(x._scmoInstances);
    _size += x._scmoInstances.size();

    // add Xml encodings
    // KS_TBD - FIX _Size stuff here also.
    _referencesData.appendArray(x._referencesData);
    _instanceData.appendArray(x._instanceData);
    _hostsData.appendArray(x._hostsData);
    _nameSpacesData.appendArray(x._nameSpacesData);

    // transfer property list
    _propertyList = x._propertyList;
}

// Encoding responses into output format
void CIMResponseData::encodeBinaryResponse(CIMBuffer& out)
{
    TRACELINE;
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMResponseData::encodeBinaryResponse");

    PSVALID;

    // Need to do a complete job here by transferring all contained data
    // into binary format and handing it out in the CIMBuffer
    // KS_TODO
    if (RESP_ENC_BINARY == (_encoding & RESP_ENC_BINARY))
    {
        PEGASUS_ASSERT(false);   // KS_TEMP

        // Binary does NOT need a marker as it consists of C++ and SCMO
        const Array<Uint8>& data = _binaryData;
        out.putBytes(data.getData(), data.size());
    }

    if (RESP_ENC_CIM == (_encoding & RESP_ENC_CIM))
    {
        out.putTypeMarker(BIN_TYPE_MARKER_CPPD);
        switch (_dataType)
        {
            case RESP_INSTNAMES:
            {
                out.putObjectPathA(_instanceNames);
                break;
            }
            case RESP_INSTANCE:
            {
                if (0 == _instances.size())
                {
                    _instances.append(CIMInstance());
                }
                out.putInstance(_instances[0], true, true);
                break;
            }
            case RESP_INSTANCES:
            {
                out.putInstanceA(_instances);
                break;
            }
            case RESP_OBJECTS:
            {
                out.putObjectA(_objects);
                break;
            }
            case RESP_OBJECTPATHS:
            {
                out.putObjectPathA(_instanceNames);
                break;
            }
            default:
            {
                PEGASUS_DEBUG_ASSERT(false);
            }
        }
    }
    if (RESP_ENC_SCMO == (_encoding & RESP_ENC_SCMO))
    {
        out.putTypeMarker(BIN_TYPE_MARKER_SCMO);
        out.putSCMOInstanceA(_scmoInstances);
    }
    if (RESP_ENC_XML == (_encoding & RESP_ENC_XML))
    {
        // This actually should not happen following general code logic
        PEGASUS_DEBUG_ASSERT(false);
    }

    PEG_METHOD_EXIT();
}

void CIMResponseData::completeNamespace(const SCMOInstance * x)
{
    TRACELINE;
    const char * ns;
    Uint32 len;
    ns = x->getNameSpace_l(len);
    // Both internal XML as well as binary always contain a namespace
    // don't have to do anything for those two encodings
    if ((RESP_ENC_BINARY == (_encoding&RESP_ENC_BINARY)) && (len != 0))
    {
        _defaultNamespace = CIMNamespaceName(ns);
    }
    if (RESP_ENC_CIM == (_encoding & RESP_ENC_CIM))
    {
        CIMNamespaceName nsName(ns);
        switch (_dataType)
        {
            case RESP_INSTANCE:
            {
                if (_instances.size() > 0)
                {
                    const CIMInstance& inst = _instances[0];
                    CIMObjectPath& p =
                        const_cast<CIMObjectPath&>(inst.getPath());
                    if (p.getNameSpace().isNull())
                    {
                        p.setNameSpace(nsName);
                    }
                }
                break;
            }
            case RESP_INSTANCES:
            {
                for (Uint32 j = 0, n = _instances.size(); j < n; j++)
                {
                    const CIMInstance& inst = _instances[j];
                    CIMObjectPath& p =
                        const_cast<CIMObjectPath&>(inst.getPath());
                    if (p.getNameSpace().isNull())
                    {
                        p.setNameSpace(nsName);
                    }
                }
                break;
            }
            case RESP_OBJECTS:
            {
                for (Uint32 j = 0, n = _objects.size(); j < n; j++)
                {
                    const CIMObject& object = _objects[j];
                    CIMObjectPath& p =
                        const_cast<CIMObjectPath&>(object.getPath());
                    if (p.getNameSpace().isNull())
                    {
                        p.setNameSpace(nsName);
                    }
                }
                break;
            }
            case RESP_INSTNAMES:
            case RESP_OBJECTPATHS:
            {
                for (Uint32 j = 0, n = _instanceNames.size(); j < n; j++)
                {
                    CIMObjectPath& p = _instanceNames[j];
                    if (p.getNameSpace().isNull())
                    {
                        p.setNameSpace(nsName);
                    }
                }
                break;
            }
            default:
            {
                PEGASUS_DEBUG_ASSERT(false);
            }
        }
    }
    if (RESP_ENC_SCMO == (_encoding & RESP_ENC_SCMO))
    {
        for (Uint32 j = 0, n = _scmoInstances.size(); j < n; j++)
        {
            SCMOInstance & scmoInst=_scmoInstances[j];
            if (0 == scmoInst.getNameSpace())
            {
                scmoInst.setNameSpace_l(ns,len);
            }
        }
    }
}

void CIMResponseData::completeHostNameAndNamespace(
    const String & hn,
    const CIMNamespaceName & ns)
{
    TRACELINE;
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMResponseData::completeHostNameAndNamespace");

    PEGASUS_ASSERT(valid());            // KS_TEMP

    if (RESP_ENC_BINARY == (_encoding & RESP_ENC_BINARY))
    {
        // On binary need remember hostname and namespace in case someone
        // builds C++ default objects or Xml types from it later on
        // -> usage: See resolveBinary()
        _defaultNamespace=ns;
        _defaultHostname=hn;
    }
    // InternalXml does not support objectPath calls
    if ((RESP_ENC_XML == (_encoding & RESP_ENC_XML)) &&
            (RESP_OBJECTS == _dataType))
    {
        for (Uint32 j = 0, n = _referencesData.size(); j < n; j++)
        {
            if (0 == _hostsData[j].size())
            {
                _hostsData[j]=hn;
            }
            if (_nameSpacesData[j].isNull())
            {
                _nameSpacesData[j]=ns;
            }
        }
    }
    if (RESP_ENC_CIM == (_encoding & RESP_ENC_CIM))
    {
        switch (_dataType)
        {
            // Instances added to account for namedInstance in Pull operations.
            case RESP_INSTANCES:

                for (Uint32 j = 0, n = _instances.size(); j < n; j++)
                {
                    const CIMInstance& instance = _instances[j];
                    CIMObjectPath& p =
                        const_cast<CIMObjectPath&>(instance.getPath());
                    if (p.getHost().size()==0)
                    {
                        p.setHost(hn);
                    }
                    if (p.getNameSpace().isNull())
                    {
                        p.setNameSpace(ns);
                    }
                }
            case RESP_OBJECTS:
            {
                for (Uint32 j = 0, n = _objects.size(); j < n; j++)
                {
                    const CIMObject& object = _objects[j];
                    CIMObjectPath& p =
                        const_cast<CIMObjectPath&>(object.getPath());
                    if (p.getHost().size()==0)
                    {
                        p.setHost(hn);
                    }
                    if (p.getNameSpace().isNull())
                    {
                        p.setNameSpace(ns);
                    }
                }
                break;
            }
            // INSTNAMES added to account for instance paths in pull name
            // operations
            case RESP_INSTNAMES:
            case RESP_OBJECTPATHS:
            {
                for (Uint32 j = 0, n = _instanceNames.size(); j < n; j++)
                {
                    CIMObjectPath& p = _instanceNames[j];
                    if (p.getHost().size() == 0)
                        p.setHost(hn);
                    if (p.getNameSpace().isNull())
                        p.setNameSpace(ns);
                }
                break;
            }
            default:
            {
                PEGASUS_DEBUG_ASSERT(false);
            }
        }
    }
    if (RESP_ENC_SCMO == (_encoding & RESP_ENC_SCMO))
    {
        CString hnCString=hn.getCString();
        const char* hnChars = hnCString;
        Uint32 hnLen = strlen(hnChars);
        CString nsCString=ns.getString().getCString();
        const char* nsChars=nsCString;
        Uint32 nsLen = strlen(nsChars);
        switch (_dataType)
        {
            // KS_PULL add Instances and InstNames to cover pull operations
            // KS_PULL - Confirm that this OK.
            case RESP_INSTNAMES:
            case RESP_INSTANCES:
            case RESP_OBJECTS:
            case RESP_OBJECTPATHS:
            {
                for (Uint32 j = 0, n = _scmoInstances.size(); j < n; j++)
                {
                    SCMOInstance & scmoInst=_scmoInstances[j];
                    if (0 == scmoInst.getHostName())
                    {
                        scmoInst.setHostName_l(hnChars,hnLen);
                    }
                    if (0 == scmoInst.getNameSpace())
                    {
                        scmoInst.setNameSpace_l(nsChars,nsLen);
                    }
                }
                break;
            }
            default:
            {
                PEGASUS_DEBUG_ASSERT(false);
            }
        }
    }
    PEG_METHOD_EXIT();
}

// NOTE: The reason for the isPullResponse variable is that there are
// some variations in ouput to Xml depending on whether the responses
// are one of the pull responses or the original responsed
void CIMResponseData::encodeXmlResponse(Buffer& out, Boolean isPullResponse)
{
    TRACELINE;

    PEG_TRACE((TRC_XML, Tracer::LEVEL3,
        "CIMResponseData::encodeXmlResponse(encoding=%X,dataType=%X)",
        _encoding,
        _dataType));
    
    // already existing Internal XML does not need to be encoded further
    // binary input is not actually impossible here, but we have an established
    // fallback
    if (RESP_ENC_BINARY == (_encoding & RESP_ENC_BINARY))
    {
        _resolveBinary();
    }
    if (RESP_ENC_XML == (_encoding & RESP_ENC_XML))
    {
        switch (_dataType)
        {
            case RESP_INSTANCE:
            {
                const Array<ArraySint8>& a = _instanceData;
                out.append((char*)a[0].getData(), a[0].size() - 1);
                break;
            }
            case RESP_INSTANCES:
            {
                const Array<ArraySint8>& a = _instanceData;
                const Array<ArraySint8>& b = _referencesData;

                for (Uint32 i = 0, n = a.size(); i < n; i++)
                {
                    if (isPullResponse)
                    {
                        out << STRLIT("<VALUE.INSTANCEWITHPATH>\n");
                    }
                    else
                    {
                        out << STRLIT("<VALUE.NAMEDINSTANCE>\n");
                    }
                    out.append((char*)b[i].getData(), b[i].size() - 1);
                    out.append((char*)a[i].getData(), a[i].size() - 1);
                    if (isPullResponse)
                    {
                        out << STRLIT("</VALUE.INSTANCEWITHPATH>\n");
                    }
                    else
                    {
                        out << STRLIT("</VALUE.NAMEDINSTANCE>\n");
                    }
                }
                break;
            }
            case RESP_OBJECTS:
            {
                const Array<ArraySint8>& a = _instanceData;
                const Array<ArraySint8>& b = _referencesData;
                for (Uint32 i = 0, n = a.size(); i < n; i++)
                {
                    out << STRLIT("<VALUE.OBJECTWITHPATH>\n");
                    out << STRLIT("<INSTANCEPATH>\n");
                    XmlWriter::appendNameSpacePathElement(
                            out,
                            _hostsData[i],
                            _nameSpacesData[i]);
                    // Leave out the surrounding tags "<VALUE.REFERENCE>\n"
                    // and "</VALUE.REFERENCE>\n" which are 18 and 19 characters
                    // long
                    out.append(
                        ((char*)b[i].getData())+18,
                        b[i].size() - 1 - 18 -19);
                    out << STRLIT("</INSTANCEPATH>\n");
                    // append instance body
                    out.append((char*)a[i].getData(), a[i].size() - 1);
                    out << STRLIT("</VALUE.OBJECTWITHPATH>\n");
                }
                break;
            }
            // internal xml encoding of instance names and object paths not
            // done today
            case RESP_INSTNAMES:
            case RESP_OBJECTPATHS:
            default:
            {
                PEGASUS_DEBUG_ASSERT(false);
            }
        }
    }

    if (RESP_ENC_CIM == (_encoding & RESP_ENC_CIM))
    {
        switch (_dataType)
        {
            case RESP_INSTNAMES:
            {
                for (Uint32 i = 0, n = _instanceNames.size(); i < n; i++)
                {
                    // Element type is different for Pull responses
                    if (isPullResponse)
                    {
                        XmlWriter::appendInstancePathElement(out,
                            _instanceNames[i]);
                    }
                    else
                    {
                        XmlWriter::appendInstanceNameElement(out,
                            _instanceNames[i]);
                    }
                }
                break;
            }
            case RESP_INSTANCE:
            {
                if (_instances.size() > 0)
                {
                    XmlWriter::appendInstanceElement(
                        out, 
                        _instances[0],
                        _includeQualifiers,
                        _includeClassOrigin,
                        _propertyList);
                }
                break;
            }
            case RESP_INSTANCES:
            {
                for (Uint32 i = 0, n = _instances.size(); i < n; i++)
                {
                    /// KS_TODO_DELETE
                    ////PrintInstance(cout, _instances[i]);
                    if (isPullResponse)
                    {
                        XmlWriter::appendValueInstanceWithPathElement(
                            out,
                            _instances[i],
                            _includeQualifiers,
                            _includeClassOrigin,
                            _propertyList);
                    }
                    else
                    {
                        XmlWriter::appendValueNamedInstanceElement(
                            out,
                            _instances[i],
                            _includeQualifiers,
                            _includeClassOrigin,
                            _propertyList);
                    }
                }
                break;
            }
            case RESP_OBJECTS:
            {
                for (Uint32 i = 0; i < _objects.size(); i++)
                {
                    // If pull, map to instances
                    if (isPullResponse)
                    {
                        CIMInstance x = (CIMInstance)_objects[i];
                        XmlWriter::appendValueInstanceWithPathElement(
                            out, x,
                            _includeQualifiers,
                            _includeClassOrigin,
                            _propertyList);
                    }
                    else
                    {
                        XmlWriter::appendValueObjectWithPathElement(
                            out,
                            _objects[i],
                            _includeQualifiers,
                            _includeClassOrigin,
                            _propertyList);
                    }
                }
                break;
            }
            case RESP_OBJECTPATHS:
            {
                for (Uint32 i = 0, n = _instanceNames.size(); i < n; i++)
                {
                    // ObjectPaths come from providers for pull operations
                    // but are encoded as instancePathElements
                    if (isPullResponse)

                    {
                        XmlWriter::appendInstancePathElement(out,
                           _instanceNames[i]);
                    }
                    else
                    {
                        out << "<OBJECTPATH>\n";
                        XmlWriter::appendValueReferenceElement(
                            out,
                            _instanceNames[i],
                            false);
                        out << "</OBJECTPATH>\n";
                    }
                }
                break;
            }
            default:
            {
                PEGASUS_DEBUG_ASSERT(false);
            }
        }
    }
    if (RESP_ENC_SCMO == (_encoding & RESP_ENC_SCMO))
    {
        switch (_dataType)
        {
            case RESP_INSTNAMES:
            {
                if (isPullResponse)
                {
                    for (Uint32 i = 0, n = _scmoInstances.size(); i < n; i++)
                    {
                        SCMOXmlWriter::appendInstancePathElement(
                            out,
                            _scmoInstances[i]);

                    }
                }
                else
                {
                    for (Uint32 i = 0, n = _scmoInstances.size(); i < n; i++)
                    {
                        SCMOXmlWriter::appendInstanceNameElement(
                            out,
                            _scmoInstances[i]);

                    }
                }
                break;
            }
            case RESP_INSTANCE:
            {
                if (_scmoInstances.size() > 0)
                {
                    if(_propertyList.isNull())
                    {
                        Array<Uint32> emptyNodes; 
                        SCMOXmlWriter::appendInstanceElement(
                            out,
                            _scmoInstances[0],
                            false,
                            emptyNodes);
                    }
                    else
                    {
                        Array<propertyFilterNodesArray_t> propFilterNodesArrays;
                        // This searches for an already created array of nodes, 
                        //if not found, creates it inside propFilterNodesArrays 
                        const Array<Uint32> & nodes= 
                            SCMOXmlWriter::getFilteredNodesArray( 
                                propFilterNodesArrays, 
                                _scmoInstances[0], 
                                _propertyList);
                        SCMOXmlWriter::appendInstanceElement(
                            out,
                            _scmoInstances[0],
                            true,
                            nodes); 
                    }  
                }
                break;
            }
            case RESP_INSTANCES:
            {
                if (isPullResponse)
                {
                    SCMOXmlWriter::appendValueSCMOInstanceWithPathElements(
                        out, _scmoInstances, _propertyList);
                }
                else
                {
                    SCMOXmlWriter::appendValueSCMOInstanceElements(
                        out, _scmoInstances, _propertyList);
                }
                break;
            }
            case RESP_OBJECTS:
            {
                if (isPullResponse)
                {
                    SCMOXmlWriter::appendValueSCMOInstanceWithPathElements(
                        out,_scmoInstances, _propertyList);
                }
                else
                {
                    // KS_TODO why is this one named element rather than
                    // elements
                    SCMOXmlWriter::appendValueObjectWithPathElement(
                        out, _scmoInstances, _propertyList);
                }
                break;
            }
            case RESP_OBJECTPATHS:
            {
                for (Uint32 i = 0, n = _scmoInstances.size(); i < n; i++)
                {
                    if (isPullResponse)
                    {
                        SCMOXmlWriter::appendInstancePathElement(out,
                            _scmoInstances[i]);
                    }
                    else
                    {
                        out << "<OBJECTPATH>\n";
                        SCMOXmlWriter::appendValueReferenceElement(
                            out, _scmoInstances[i],
                            false);
                        out << "</OBJECTPATH>\n";
                    }
                }
                break;
            }
            default:
            {
                PEGASUS_DEBUG_ASSERT(false);
            }
        }
    }
}

// contrary to encodeXmlResponse this function encodes the Xml in a format
// not usable by clients
void CIMResponseData::encodeInternalXmlResponse(CIMBuffer& out)
{
    TRACELINE;
    PEG_TRACE((TRC_XML, Tracer::LEVEL3,
        "CIMResponseData::encodeInternalXmlResponse(encoding=%X,content=%X)",
        _encoding,
        _dataType));
    // For mixed (CIM+SCMO) responses, we need to tell the receiver the
    // total number of instances. The totalSize variable is used to keep track
    // of this.
    Uint32 totalSize = 0;

    // already existing Internal XML does not need to be encoded further
    // binary input is not actually impossible here, but we have an established
    // fallback
    if (RESP_ENC_BINARY == (_encoding & RESP_ENC_BINARY))
    {
        _resolveBinary();
    }
    if ((0 == _encoding) ||
        (RESP_ENC_CIM == (_encoding & RESP_ENC_CIM)))
    {
        switch (_dataType)
        {
            case RESP_INSTANCE:
            {
                if (0 == _instances.size())
                {
                    _instances.append(CIMInstance());
                    CIMInternalXmlEncoder::_putXMLInstance(
                        out,
                        _instances[0]);
                    break;
                }
                CIMInternalXmlEncoder::_putXMLInstance(
                    out,
                    _instances[0],
                    _includeQualifiers,
                    _includeClassOrigin,
                    _propertyList);
                break;
            }
            case RESP_INSTANCES:
            {
                Uint32 n = _instances.size();
                totalSize = n + _scmoInstances.size();
                out.putUint32(totalSize);
                for (Uint32 i = 0; i < n; i++)
                {
                    CIMInternalXmlEncoder::_putXMLNamedInstance(
                        out,
                        _instances[i],
                        _includeQualifiers,
                        _includeClassOrigin,
                        _propertyList);
                }
                break;
            }
            case RESP_OBJECTS:
            {
                Uint32 n = _objects.size();
                totalSize = n + _scmoInstances.size();
                out.putUint32(totalSize);
                for (Uint32 i = 0; i < n; i++)
                {
                    CIMInternalXmlEncoder::_putXMLObject(
                        out,
                        _objects[i],
                        _includeQualifiers,
                        _includeClassOrigin,
                        _propertyList);
                }
                break;
            }
            // internal xml encoding of instance names and object paths not
            // done today
            case RESP_INSTNAMES:
            case RESP_OBJECTPATHS:
            default:
            {
                PEGASUS_DEBUG_ASSERT(false);
            }
        }
    }
    if (RESP_ENC_SCMO == (_encoding & RESP_ENC_SCMO))
    {
        switch (_dataType)
        {
            case RESP_INSTANCE:
            {
                if (0 == _scmoInstances.size())
                {
                    _scmoInstances.append(SCMOInstance());
                }
                SCMOInternalXmlEncoder::_putXMLInstance(
                    out, 
                    _scmoInstances[0],
                    _propertyList);
                break;
            }
            case RESP_INSTANCES:
            {
                Uint32 n = _scmoInstances.size();
                // Only put the size when not already done above
                if (0==totalSize)
                {
                    out.putUint32(n);
                }
                SCMOInternalXmlEncoder::_putXMLNamedInstance(
                    out,
                    _scmoInstances,
                    _propertyList);
                break;
            }
            case RESP_OBJECTS:
            {
                Uint32 n = _scmoInstances.size();
                // Only put the size when not already done above
                if (0==totalSize)
                {
                    out.putUint32(n);
                }
                SCMOInternalXmlEncoder::_putXMLObject(
                    out,
                    _scmoInstances,
                    _propertyList);
                break;
            }
            // internal xml encoding of instance names and object paths not
            // done today
            case RESP_INSTNAMES:
            case RESP_OBJECTPATHS:
            default:
            {
                PEGASUS_DEBUG_ASSERT(false);
            }
        }
    }

}

void CIMResponseData::_resolveToCIM()
{
    TRACELINE;
    PEG_TRACE((TRC_XML, Tracer::LEVEL3,
        "CIMResponseData::_resolveToCIM(encoding=%X,content=%X)",
        _encoding,
        _dataType));

    if (RESP_ENC_XML == (_encoding & RESP_ENC_XML))
    {
        _resolveXmlToCIM();
    }
    if (RESP_ENC_BINARY == (_encoding & RESP_ENC_BINARY))
    {
        _resolveBinary();
    }
    if (RESP_ENC_SCMO == (_encoding & RESP_ENC_SCMO))
    {
        _resolveSCMOToCIM();
    }

    PEGASUS_DEBUG_ASSERT(_encoding == RESP_ENC_CIM || _encoding == 0);
}

void CIMResponseData::_resolveToSCMO()
{
    TRACELINE;
    PEG_TRACE((TRC_XML, Tracer::LEVEL3,
        "CIMResponseData::_resolveToSCMO(encoding=%X,content=%X)",
        _encoding,
        _dataType));

    if (RESP_ENC_XML == (_encoding & RESP_ENC_XML))
    {
        _resolveXmlToSCMO();
    }
    if (RESP_ENC_BINARY == (_encoding & RESP_ENC_BINARY))
    {
        _resolveBinary();
    }
    if (RESP_ENC_CIM == (_encoding & RESP_ENC_CIM))
    {
        _resolveCIMToSCMO();
    }
    PEGASUS_DEBUG_ASSERT(_encoding == RESP_ENC_SCMO || _encoding == 0);
}

// helper functions to transform different formats into one-another
// functions work on the internal data and calling of them should be
// avoided whenever possible
void CIMResponseData::_resolveBinary()
{
    TRACELINE;
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMResponseData::_resolveBinary");

    CIMBuffer in((char*)_binaryData.getData(), _binaryData.size());

    while (in.more())
    {
        Uint32 binaryTypeMarker=0;
        if(!in.getTypeMarker(binaryTypeMarker))
        {
            PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
                "Failed to get type marker for binary objects!");
            PEG_METHOD_EXIT();
            in.release();
            return;
        }

        if (BIN_TYPE_MARKER_SCMO==binaryTypeMarker)
        {
            if (!in.getSCMOInstanceA(_scmoInstances))
            {
                _encoding &=(~RESP_ENC_BINARY);
                in.release();
                PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
                    "Failed to resolve binary SCMOInstances!");
                PEG_METHOD_EXIT();
                return;
            }

            _encoding |= RESP_ENC_SCMO;
        }
        else
        {
            switch (_dataType)
            {
                case RESP_INSTNAMES:
                case RESP_OBJECTPATHS:
                {
                    if (!in.getObjectPathA(_instanceNames))
                    {
                        _encoding &=(~RESP_ENC_BINARY);
                        in.release();
                        PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
                            "Failed to resolve binary CIMObjectPaths!");
                        PEG_METHOD_EXIT();
                        return;
                    }
                    break;
                }
                case RESP_INSTANCE:
                {
                    CIMInstance instance;
                    if (!in.getInstance(instance))
                    {
                        _encoding &=(~RESP_ENC_BINARY);
                        _encoding |= RESP_ENC_CIM;
                        _instances.append(instance);
                        in.release();
                        PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
                            "Failed to resolve binary instance!");
                        PEG_METHOD_EXIT();
                        return;
                    }

                    _instances.append(instance);
                    break;
                }
                case RESP_INSTANCES:
                {
                    if (!in.getInstanceA(_instances))
                    {
                        _encoding &=(~RESP_ENC_BINARY);
                        in.release();
                        PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
                            "Failed to resolve binary CIMInstances!");
                        PEG_METHOD_EXIT();
                        return;
                    }
                    break;
                }
                case RESP_OBJECTS:
                {
                    if (!in.getObjectA(_objects))
                    {
                        in.release();
                        _encoding &=(~RESP_ENC_BINARY);
                        PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
                            "Failed to resolve binary CIMObjects!");
                        PEG_METHOD_EXIT();
                        return;
                    }
                    break;
                }
                default:
                {
                    PEGASUS_DEBUG_ASSERT(false);
                }
            } // switch
            _encoding |= RESP_ENC_CIM;
        } // else SCMO
    }
    _encoding &=(~RESP_ENC_BINARY);
    // fix up the hostname and namespace for objects if defaults
    // were set
    if (_defaultHostname.size() > 0 && !_defaultNamespace.isNull())
    {
        completeHostNameAndNamespace(_defaultHostname, _defaultNamespace);
    }
    in.release();
    PEG_METHOD_EXIT();
}

void CIMResponseData::_resolveXmlToCIM()
{
    TRACELINE;
    switch (_dataType)
    {
        // Xml encoding for instance names and object paths not used
        case RESP_OBJECTPATHS:
        case RESP_INSTNAMES:
        {
            break;
        }
        case RESP_INSTANCE:
        {
            CIMInstance cimInstance;
            // Deserialize instance:
            {
                XmlParser parser((char*)_instanceData[0].getData());

                if (!XmlReader::getInstanceElement(parser, cimInstance))
                {
                    cimInstance = CIMInstance();
                    PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
                        "Failed to resolve XML instance, parser error!");
                }
            }
            // Deserialize path:
            {
                XmlParser parser((char*)_referencesData[0].getData());
                CIMObjectPath cimObjectPath;

                if (XmlReader::getValueReferenceElement(parser, cimObjectPath))
                {
                    if (_hostsData.size())
                    {
                        cimObjectPath.setHost(_hostsData[0]);
                    }
                    if (!_nameSpacesData[0].isNull())
                    {
                        cimObjectPath.setNameSpace(_nameSpacesData[0]);
                    }
                    cimInstance.setPath(cimObjectPath);
                    // only if everything works we add the CIMInstance to the
                    // array
                    _instances.append(cimInstance);
                }
            }
            break;
        }
        case RESP_INSTANCES:
        {
            for (Uint32 i = 0; i < _instanceData.size(); i++)
            {
                CIMInstance cimInstance;
                // Deserialize instance:
                {
                    XmlParser parser((char*)_instanceData[i].getData());

                    if (!XmlReader::getInstanceElement(parser, cimInstance))
                    {
                        PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
                            "Failed to resolve XML instance."
                                " Creating empty instance!");
                        cimInstance = CIMInstance();
                    }
                }

                // Deserialize path:
                {
                    XmlParser parser((char*)_referencesData[i].getData());
                    CIMObjectPath cimObjectPath;

                    if (XmlReader::getInstanceNameElement(parser,cimObjectPath))
                    {
                        if (!_nameSpacesData[i].isNull())
                            cimObjectPath.setNameSpace(_nameSpacesData[i]);

                        if (_hostsData[i].size())
                            cimObjectPath.setHost(_hostsData[i]);

                        cimInstance.setPath(cimObjectPath);
                    }
                }

                _instances.append(cimInstance);
            }
            break;
        }
        case RESP_OBJECTS:
        {
            for (Uint32 i=0, n=_instanceData.size(); i<n; i++)
            {
                CIMObject cimObject;

                // Deserialize Objects:
                {
                    XmlParser parser((char*)_instanceData[i].getData());

                    CIMInstance cimInstance;
                    CIMClass cimClass;

                    if (XmlReader::getInstanceElement(parser, cimInstance))
                    {
                        cimObject = CIMObject(cimInstance);
                    }
                    else if (XmlReader::getClassElement(parser, cimClass))
                    {
                        cimObject = CIMObject(cimClass);
                    }
                    else
                    {
                        PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
                            "Failed to get XML object data!");
                    }
                }

                // Deserialize paths:
                {
                    XmlParser parser((char*)_referencesData[i].getData());
                    CIMObjectPath cimObjectPath;

                    if (XmlReader::getValueReferenceElement(
                            parser,
                            cimObjectPath))
                    {
                        if (!_nameSpacesData[i].isNull())
                            cimObjectPath.setNameSpace(_nameSpacesData[i]);

                        if (_hostsData[i].size())
                            cimObjectPath.setHost(_hostsData[i]);

                        cimObject.setPath(cimObjectPath);
                    }
                }
                _objects.append(cimObject);
            }
            break;
        }
        default:
        {
            PEGASUS_DEBUG_ASSERT(false);
        }
    }
    // Xml was resolved, release Xml content now
    _referencesData.clear();
    _hostsData.clear();
    _nameSpacesData.clear();
    _instanceData.clear();
    // remove Xml Encoding flag
    _encoding &=(~RESP_ENC_XML);
    // add CIM Encoding flag
    _encoding |=RESP_ENC_CIM;
}

void CIMResponseData::_resolveXmlToSCMO()
{
    TRACELINE;
    // Not optimal, can probably be improved
    // but on the other hand, since using the binary format this case should
    // actually not ever happen.
    _resolveXmlToCIM();
    _resolveCIMToSCMO();
}

void CIMResponseData::_resolveSCMOToCIM()
{
    TRACELINE;
    switch(_dataType)
    {
        case RESP_INSTNAMES:
        case RESP_OBJECTPATHS:
        {
            for (Uint32 x=0, n=_scmoInstances.size(); x < n; x++)
            {
                CIMObjectPath newObjectPath;
                _scmoInstances[x].getCIMObjectPath(newObjectPath);
                _instanceNames.append(newObjectPath);
            }
            break;
        }
        case RESP_INSTANCE:
        {
            if (_scmoInstances.size() > 0)
            {
                CIMInstance newInstance;
                _scmoInstances[0].getCIMInstance(newInstance);
                _instances.append(newInstance);
            }
            break;
        }
        case RESP_INSTANCES:
        {
            for (Uint32 x=0, n=_scmoInstances.size(); x < n; x++)
            {
                CIMInstance newInstance;
                _scmoInstances[x].getCIMInstance(newInstance);
                _instances.append(newInstance);
            }
            break;
        }
        case RESP_OBJECTS:
        {
            for (Uint32 x=0, n=_scmoInstances.size(); x < n; x++)
            {
                CIMInstance newInstance;
                _scmoInstances[x].getCIMInstance(newInstance);
                _objects.append(CIMObject(newInstance));
            }
            break;
        }
        default:
        {
            PEGASUS_DEBUG_ASSERT(false);
        }
    }
    _scmoInstances.clear();
    // remove CIM Encoding flag
    _encoding &=(~RESP_ENC_SCMO);
    // add SCMO Encoding flag
    _encoding |=RESP_ENC_CIM;
}

void CIMResponseData::_resolveCIMToSCMO()
{
    TRACELINE;
    CString nsCString=_defaultNamespace.getString().getCString();
    const char* _defNamespace = nsCString;
    Uint32 _defNamespaceLen;
    if (_defaultNamespace.isNull())
    {
        _defNamespaceLen=0;
    }
    else
    {
        _defNamespaceLen=strlen(_defNamespace);
    }
    switch (_dataType)
    {
        case RESP_INSTNAMES:
        {
            for (Uint32 i=0,n=_instanceNames.size();i<n;i++)
            {
                SCMOInstance addme(
                    _instanceNames[i],
                    _defNamespace,
                    _defNamespaceLen);
                _scmoInstances.append(addme);
            }
            _instanceNames.clear();
            break;
        }
        case RESP_INSTANCE:
        {
            if (_instances.size() > 0)
            {
                SCMOInstance addme(
                    _instances[0],
                    _defNamespace,
                    _defNamespaceLen);
                _scmoInstances.clear();
                _scmoInstances.append(addme);
                _instances.clear();
            }
            break;
        }
        case RESP_INSTANCES:
        {
            for (Uint32 i=0,n=_instances.size();i<n;i++)
            {
                SCMOInstance addme(
                    _instances[i],
                    _defNamespace,
                    _defNamespaceLen);
                _scmoInstances.append(addme);
            }
            _instances.clear();
            break;
        }
        case RESP_OBJECTS:
        {
            for (Uint32 i=0,n=_objects.size();i<n;i++)
            {
                SCMOInstance addme(
                    _objects[i],
                    _defNamespace,
                    _defNamespaceLen);
                _scmoInstances.append(addme);
            }
            _objects.clear();
            break;
        }
        case RESP_OBJECTPATHS:
        {
            for (Uint32 i=0,n=_instanceNames.size();i<n;i++)
            {
                SCMOInstance addme(
                    _instanceNames[i],
                    _defNamespace,
                    _defNamespaceLen);
                // TODO: More description about this.
                if (0 == _instanceNames[i].getKeyBindings().size())
                {
                    // if there is no keybinding, this is a class
                    addme.setIsClassOnly(true);
                }
                _scmoInstances.append(addme);
            }
            _instanceNames.clear();
            break;
        }
        default:
        {
            PEGASUS_DEBUG_ASSERT(false);
        }
    }

    // remove CIM Encoding flag
    _encoding &=(~RESP_ENC_CIM);
    // add SCMO Encoding flag
    _encoding |=RESP_ENC_SCMO;
}

/**
 * Validate the magic object for this CIMResponseData. This
 * compiles only in debug mode and can be use to validate the
 * CIMResponseData object
 *
 * @return Boolean True if valid object.
 */
Boolean CIMResponseData::valid()
{
    return _magic;
}

void CIMResponseData::setRequestProperties(
    const Boolean includeQualifiers,
    const Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    TRACELINE;
    _includeQualifiers = includeQualifiers;
    _includeClassOrigin = includeClassOrigin;
    _propertyList = propertyList; 
}

PEGASUS_NAMESPACE_END
