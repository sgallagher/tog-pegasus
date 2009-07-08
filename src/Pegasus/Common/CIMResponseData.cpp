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
#include "XmlWriter.h"
#include "XmlReader.h"
#include "Tracer.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

//-----------------------------------------------------------------------------
//
//  CIMInstanceResponseData
//
//-----------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Takes a binary stream representing an instances from a CIMBuffer and stores 
// it in the responsedata.           
// @param hasLen Indicates if the binary instance stream is prepended with an
//               Uint32 value indicating the number of instances in the stream.
//------------------------------------------------------------------------------
bool CIMInstanceResponseData::setBinaryCimInstance(CIMBuffer& in, bool hasLen)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMInstanceResponseData::setBinaryCimInstance");

    if (hasLen)
    {
        if (!in.getUint8A(_binaryData))
        {
            PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
                "Failed to get binary instance data!");
            PEG_METHOD_EXIT();
            return false;
        }
    }
    else
    {
        size_t remainingDataLength = in.capacity() - in.size();

        _binaryData.append((Uint8*)in.getPtr(), remainingDataLength);
    }

    _resolveCallback = _resolveBinaryInstance;
    _binaryEncoding = true;

    PEG_METHOD_EXIT();
    return true;
};

bool CIMInstanceResponseData::setXmlCimInstance(CIMBuffer& in)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMInstanceResponseData::setXmlCimInstance");

    if (!in.getSint8A(_instanceData))
    {
        PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
            "Failed to get XML instance data!");
        PEG_METHOD_EXIT();
        return false;
    }

    if (!in.getSint8A(_referenceData))
    {
        PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
            "Failed to get XML instance data (reference)!");
        PEG_METHOD_EXIT();
        return false;
    }

    if (!in.getString(_hostData))
    {
        PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
            "Failed to get XML instance data (host)!");
        PEG_METHOD_EXIT();
        return false;
    }

    if (!in.getNamespaceName(_nameSpaceData))
    {
        PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
            "Failed to get XML instance data (namespace)!");
        PEG_METHOD_EXIT();
        return false;
    }

    _resolveCallback = _resolveXMLInstance;
    _binaryEncoding = false;

    PEG_METHOD_EXIT();
    return true;
};

//------------------------------------------------------------------------------
// Encodes the CIMInstance representation contained in the current 
// CIMResponseData object in binary response message format.
// This code corresponds to method _resolveBinaryInstance, which is used
// revert a binary instance representation back into a CIMInstance
//------------------------------------------------------------------------------
void CIMInstanceResponseData::encodeBinaryResponse(CIMBuffer& out) const
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMInstanceResponseData::encodeBinaryResponse");

    if (_resolveCallback && _binaryEncoding)
    {
        const Array<Uint8>& data = _binaryData;
        out.putBytes(data.getData(), data.size());
    }
    else
    {
        out.putInstance(_cimInstance, false, false);
    }
    PEG_METHOD_EXIT();
}

//------------------------------------------------------------------------------
// Encodes the CIMInstanc representation contained in the current 
// CIMResponseData object in xml response message format.
// This code corresponds to method _resolveXmlInstance, which is used
// revert a CIM-XML instance representation back into a CIMInstance.
//------------------------------------------------------------------------------
void CIMInstanceResponseData::encodeXmlResponse(Buffer& out) const
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMInstanceResponseData::encodeXmlResponse");

    if (_resolveCallback && !_binaryEncoding)
    {
        out.append( (char*)_instanceData.getData(),_instanceData.size()-1);
    }
    else
    {
        XmlWriter::appendInstanceElement(out, _cimInstance);
    }
    PEG_METHOD_EXIT();
}

//------------------------------------------------------------------------------
// Instantiates a CIMInstance from a binary representation created by
// the CIMBinMessageSerializer.
// Returns true on success.
//------------------------------------------------------------------------------
Boolean CIMInstanceResponseData::_resolveBinaryInstance(
    CIMInstanceResponseData* data,
    CIMInstance& instance)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMInstanceResponseData::_resolveBinaryInstance");

    CIMBuffer in((char*)data->_binaryData.getData(), data->_binaryData.size());

    if (!in.getInstance(instance))
    {
        instance = CIMInstance();
        in.release();
        PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
            "Failed to resolve binary instance!");
        PEG_METHOD_EXIT();
        return false;
    }

    in.release();
    PEG_METHOD_EXIT();
    return true;
}

//------------------------------------------------------------------------------
// Instantiates a CIMInstance from an xml representation created by
// the CIMBinMessageSerializer.
// Returns true on success.
//------------------------------------------------------------------------------
Boolean CIMInstanceResponseData::_resolveXMLInstance(
    CIMInstanceResponseData* data,
    CIMInstance& cimInstance)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMInstanceResponseData::_resolveXMLInstance");

    // Deserialize instance:
    {
        XmlParser parser((char*)data->_instanceData.getData());

        if (!XmlReader::getInstanceElement(parser, cimInstance))
        {
            cimInstance = CIMInstance();
            PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
                "Failed to resolve XML instance, parser error!");
            PEG_METHOD_EXIT();
            return false;
        }
    }

    // Deserialize path:
    {
        XmlParser parser((char*)data->_referenceData.getData());
        CIMObjectPath cimObjectPath;

        if (XmlReader::getValueReferenceElement(parser, cimObjectPath))
        {
            if (data->_hostData.size())
                cimObjectPath.setHost(data->_hostData);

            if (!data->_nameSpaceData.isNull())
                cimObjectPath.setNameSpace(data->_nameSpaceData);

            cimInstance.setPath(cimObjectPath);
        }
    }

    PEG_METHOD_EXIT();
    return true;
}


//-----------------------------------------------------------------------------
//
//  CIMInstancesResponseData
//
//-----------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Takes a binary stream of instances from a CIMBuffer and stores it in the
// responsedata.           
// @param hasLen Indicates if the binary instance stream is prepended with an
//               Uint32 value indicating the number of instances in the stream.
//------------------------------------------------------------------------------
bool CIMInstancesResponseData::setBinaryCimInstances(CIMBuffer& in, bool hasLen)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMInstancesResponseData::setBinaryCimInstances");

    if (hasLen)
    {
        if (!in.getUint8A(_binaryData))
        {
            PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
                "Failed to get binary instance data!");
            PEG_METHOD_EXIT();
            return false;
        }
    }
    else
    {
        size_t remainingDataLength = in.capacity() - in.size();
        _binaryData.append((Uint8*)in.getPtr(), remainingDataLength);
    }

    _resolveCallback = _resolveBinaryInstances;
    _binaryEncoding = true;

    PEG_METHOD_EXIT();
    return true;
};

bool CIMInstancesResponseData::setXmlCimInstances(CIMBuffer& in)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMInstancesResponseData::setXmlCimInstances");

    Uint32 count;

    if (!in.getUint32(count))
    {
        PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
            "Failed to get XML instance data (number of instance)!");
        PEG_METHOD_EXIT();
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
            PEG_METHOD_EXIT();
            return false;
        }

        if (!in.getSint8A(ref))
        {
            PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
                "Failed to get XML instance data (references)!");
            PEG_METHOD_EXIT();
            return false;
        }

        if (!in.getString(host))
        {
            PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
                "Failed to get XML instance data (host)!");
            PEG_METHOD_EXIT();
            return false;
        }

        if (!in.getNamespaceName(ns))
        {
            PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
                "Failed to get XML instance data (namespace)!");
            PEG_METHOD_EXIT();
            return false;
        }

        _instancesData.append(inst);
        _referencesData.append(ref);
        _hostsData.append(host);
        _nameSpacesData.append(ns);
    }

    _resolveCallback = _resolveXMLInstances;
    _binaryEncoding = false;

    PEG_METHOD_EXIT();
    return true;
};




//------------------------------------------------------------------------------
// Encodes the array of CIMInstance representation contained in the current 
// CIMResponseData object in binary response message format.
// This code corresponds to method _resolveBinaryInstances, which is used
// revert a binary instance array representation back into an array of
// CIMInstance
//------------------------------------------------------------------------------
void CIMInstancesResponseData::encodeBinaryResponse(CIMBuffer& out) const
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMInstancesResponseData::encodeBinaryResponse");

    if (_resolveCallback && _binaryEncoding)
    {
        const Array<Uint8>& data = _binaryData;
        out.putBytes(data.getData(), data.size());
    }
    else
    {
        out.putInstanceA(_namedInstances, false);
    }
    PEG_METHOD_EXIT();
}

//------------------------------------------------------------------------------
// Encodes the array of CIMInstance representation contained in the current 
// CIMResponseData object in xml response message format.
// This code corresponds to method _resolveXmlInstances, which is used
// revert a CIM-XML object array representation back into an array of
// CIMInstance.
//------------------------------------------------------------------------------
void CIMInstancesResponseData::encodeXmlResponse(Buffer& out) const
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMInstancesResponseData::encodeXmlResponse");

    if (_resolveCallback && !_binaryEncoding)
    {
        const Array<ArraySint8>& a = _instancesData;
        const Array<ArraySint8>& b = _referencesData;

        for (Uint32 i = 0, n = a.size(); i < n; i++)
        {
            out << STRLIT("<VALUE.NAMEDINSTANCE>\n");
            out.append((char*)b[i].getData(), b[i].size() - 1);
            out.append((char*)a[i].getData(), a[i].size() - 1);
            out << STRLIT("</VALUE.NAMEDINSTANCE>\n");
        }
    }
    else
    {
        for (Uint32 i = 0, n = _namedInstances.size(); i < n; i++)
            XmlWriter::appendValueNamedInstanceElement(
                out, _namedInstances[i]);
    }
    PEG_METHOD_EXIT();
}

//------------------------------------------------------------------------------
// Instantiates an array of CIMInstances from a binary representation created by
// the CIMBinMessageSerializer.
// Returns true on success.
//------------------------------------------------------------------------------
Boolean CIMInstancesResponseData::_resolveBinaryInstances(
    CIMInstancesResponseData* data,
    Array<CIMInstance>& instances)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMInstancesResponseData::_resolveBinaryInstances");

    instances.clear();

    CIMBuffer in((char*)data->_binaryData.getData(), data->_binaryData.size());

    while (in.more())
    {
        if (!in.getInstanceA(instances))
        {
            in.release();
            PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
                "Failed to remove binary instance!");
            PEG_METHOD_EXIT();
            return false;
        }
    }

    in.release();
    PEG_METHOD_EXIT();
    return true;
}



//------------------------------------------------------------------------------
// Instantiates an array of CIMInstances from an xml representation created by
// the CIMBinMessageSerializer.
// Returns true on success.
//------------------------------------------------------------------------------
Boolean CIMInstancesResponseData::_resolveXMLInstances(
    CIMInstancesResponseData* data,
    Array<CIMInstance>& instances)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMInstancesResponseData::_resolveXMLInstances");

    instances.clear();

    for (Uint32 i = 0; i < data->_instancesData.size(); i++)
    {
        CIMInstance cimInstance;

        // Deserialize instance:
        {
            XmlParser parser((char*)data->_instancesData[i].getData());

            if (!XmlReader::getInstanceElement(parser, cimInstance))
            {
                PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
                    "Failed to resolve XML instance. Creating empty instance!");
                cimInstance = CIMInstance();
            }
        }

        // Deserialize path:
        {
            XmlParser parser((char*)data->_referencesData[i].getData());
            CIMObjectPath cimObjectPath;

            if (XmlReader::getInstanceNameElement(parser, cimObjectPath))
            {
                if (!data->_nameSpacesData[i].isNull())
                    cimObjectPath.setNameSpace(data->_nameSpacesData[i]);

                if (data->_hostsData[i].size())
                    cimObjectPath.setHost(data->_hostsData[i]);

                cimInstance.setPath(cimObjectPath);
            }
        }

        instances.append(cimInstance);
    }

    PEG_METHOD_EXIT();
    return true;
}


//-----------------------------------------------------------------------------
//
//  CIMObjectsResponseData
//
//-----------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Takes a binary stream of objects from a CIMBuffer and stores 
// it in the responsedata.           
// @param hasLen Indicates if the binary object stream is prepended with an
//               Uint32 value indicating the number of objects in the stream.
//------------------------------------------------------------------------------
bool CIMObjectsResponseData::setBinaryCimObjects(CIMBuffer& in, bool hasLen)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMObjectsResponseData::setBinaryCimObjects");

    if (hasLen)
    {
        if (!in.getUint8A(_binaryData))
        {
            PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
                "Failed to get binary object data!");
            PEG_METHOD_EXIT();
            return false;
        }
    }
    else
    {
        size_t remainingDataLength = in.capacity() - in.size();
        _binaryData.append((Uint8*)in.getPtr(), remainingDataLength);
    }

    _resolveCallback = _resolveBinaryObjects;
    _binaryEncoding = true;

    PEG_METHOD_EXIT();
    return true;
};

bool CIMObjectsResponseData::setXmlCimObjects(CIMBuffer& in)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMObjectsResponseData::setXmlCimObjects");

    Uint32 count;

    if (!in.getUint32(count))
    {
        PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
            "Failed to get XML object data (number of objects)!");
        PEG_METHOD_EXIT();
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
            PEG_METHOD_EXIT();
            return false;
        }

        if (!in.getSint8A(ref))
        {
            PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
                "Failed to get XML object data (reference)!");
            PEG_METHOD_EXIT();
            return false;
        }

        if (!in.getString(host))
        {
            PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
                "Failed to get XML object data (host)!");
            PEG_METHOD_EXIT();
            return false;
        }

        if (!in.getNamespaceName(ns))
        {
            PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
                "Failed to get XML object data (namespace)!");
            PEG_METHOD_EXIT();
            return false;
        }

        _cimObjectsData.append(obj);
        _referencesData.append(ref);
        _hostsData.append(host);
        _nameSpacesData.append(ns);
    }

    _resolveCallback = _resolveXMLObjects;
    _binaryEncoding = false;

    PEG_METHOD_EXIT();
    return true;
}

//------------------------------------------------------------------------------
// Encodes the array of CIMObject representation contained in the current 
// CIMResponseData object in binary response message format.
// This code corresponds to method _resolveBinaryObjects, which is used
// revert a binary object array representation back into an array of
// CIMObject.
//------------------------------------------------------------------------------
void CIMObjectsResponseData::encodeBinaryResponse(CIMBuffer& out) const
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMObjectsResponseData::encodeBinaryResponse");

    if (_resolveCallback && _binaryEncoding)
    {
        const Array<Uint8>& data = _binaryData;
        out.putBytes(data.getData(), data.size());
    }
    else
    {
        out.putObjectA(_cimObjects);
    }
    PEG_METHOD_EXIT();
}

//------------------------------------------------------------------------------
// Encodes the array of CIMObject representation contained in the current 
// CIMResponseData object in xml response message format.
// This code corresponds to method _resolveXmlObjects, which is used
// revert a CIM-XML object array representation back into an array of
// CIMObject.
//------------------------------------------------------------------------------
void CIMObjectsResponseData::encodeXmlResponse(Buffer& out) const
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMObjectsResponseData::encodeXmlResponse");

    if (_resolveCallback && !_binaryEncoding)
    {
        const Array<ArraySint8>& a = _cimObjectsData;
        const Array<ArraySint8>& b = _referencesData;

        for (Uint32 i = 0, n = a.size(); i < n; i++)
        {
            out << STRLIT("<VALUE.OBJECTWITHPATH>\n");
            out.append((char*)b[i].getData(), b[i].size() - 1);
            out.append((char*)a[i].getData(), a[i].size() - 1);
            out << STRLIT("</VALUE.OBJECTWITHPATH>\n");
        }
    }
    else
    {
        for (Uint32 i = 0; i < _cimObjects.size(); i++)
        {
            XmlWriter::appendValueObjectWithPathElement(out, _cimObjects[i]);
        }
     }
    PEG_METHOD_EXIT();
}

//------------------------------------------------------------------------------
// Instantiates an array of CIMObjects from a binary representation created by
// the CIMBinMessageSerializer.
// Returns true on success.
//------------------------------------------------------------------------------
Boolean CIMObjectsResponseData::_resolveBinaryObjects(
    CIMObjectsResponseData* data,
    Array<CIMObject>& cimObjects)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMObjectsResponseData::_resolveBinaryObjects");

    cimObjects.clear();

    CIMBuffer in((char*)data->_binaryData.getData(), data->_binaryData.size());

    while (in.more())
    {
        if (!in.getObjectA(cimObjects))
        {
            in.release();

            PEG_TRACE_CSTRING(TRC_DISCARDED_DATA, Tracer::LEVEL1,
                "Failed to resolve binary data!");
            PEG_METHOD_EXIT();
            return false;
        }
    }

    in.release();
    PEG_METHOD_EXIT();
    return true;
}

//------------------------------------------------------------------------------
// Instantiates an array of CIMObjects from an xml representation created by
// the CIMBinMessageSerializer.
// Returns true on success.
//------------------------------------------------------------------------------
Boolean CIMObjectsResponseData::_resolveXMLObjects(
    CIMObjectsResponseData* data,
    Array<CIMObject>& cimObjects)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMObjectsResponseData::_resolveXMLObjects");

    cimObjects.clear();

    for (Uint32 i=0, n=data->_cimObjectsData.size(); i<n; i++)
    {
        CIMObject cimObject;

        // Deserialize Objects:
        {
            XmlParser parser((char*)data->_cimObjectsData[i].getData());

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
            XmlParser parser((char*)data->_referencesData[i].getData());
            CIMObjectPath cimObjectPath;

            if (XmlReader::getValueReferenceElement(parser, cimObjectPath))
            {
                if (!data->_nameSpacesData[i].isNull())
                    cimObjectPath.setNameSpace(data->_nameSpacesData[i]);

                if (data->_hostsData[i].size())
                    cimObjectPath.setHost(data->_hostsData[i]);

                cimObject.setPath(cimObjectPath);
            }
        }

        cimObjects.append(cimObject);
    }

    PEG_METHOD_EXIT();
    return true;
}


PEGASUS_NAMESPACE_END
