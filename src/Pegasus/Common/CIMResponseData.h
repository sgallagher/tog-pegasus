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

#ifndef Pegasus_CIMResponseData_h
#define Pegasus_CIMResponseData_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/Linkage.h>
#include <Pegasus/Common/CIMBuffer.h>
#include <Pegasus/Common/SCMOClass.h>
#include <Pegasus/Common/SCMOInstance.h>
#include <Pegasus/Common/SCMODump.h>

PEGASUS_NAMESPACE_BEGIN

typedef Array<Sint8> ArraySint8;
#define PEGASUS_ARRAY_T ArraySint8
# include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T


//-----------------------------------------------------------------------------
//
//  CIMInstanceNamesResponseData
//
//-----------------------------------------------------------------------------

class PEGASUS_COMMON_LINKAGE CIMInstanceNamesResponseData
{
public:

    enum ResponseDataEncoding {
        RESP_ENC_CIM,
        RESP_ENC_BINARY,
        RESP_ENC_XML,
        RESP_ENC_SCMO
    };

    CIMInstanceNamesResponseData():
        _resolveCallback(0),
        _encoding(RESP_ENC_CIM)
    {
    }

    Array<CIMObjectPath>& getInstanceNames()
    {
        _resolve();
        return _instanceNames;
    }

    /*const Array<CIMConstObjectPath>& getInstanceNames() const
    {
        // Resolve the instances before returning them.  The resolve step
        // requires non-const access, but does not fundamentally change the
        // message contents.
        const_cast<CIMInstanceNamesResponseData*>(this)->_resolve();

        // The Array<CIMInstance> is masqueraded as an Array<CIMConstInstance>
        // for expedience, since the internal representations are the same.
        return *((Array<CIMConstObjectPath>*)(void*)&_instanceNames);
    }*/

    void setInstanceNames(const Array<CIMObjectPath>& x)
    {
        _resolveCallback = 0;
        _instanceNames = x;
    }

    void appendInstanceName(const CIMObjectPath& x)
    {
        _resolveCallback = 0;
        _instanceNames.append(x);
    }

    void setSCMOInstanceNames(const Array<SCMOInstance>& x)
    {
        _resolveCallback = _resolveSCMOInstanceNames;
        fprintf(
            stderr,
            "CIMResponseData::setSCMOInstanceNames(cb=%p)\n",
            _resolveCallback);
        _scmoInstanceNames = x;
    }

    void appendSCMOInstanceName(const SCMOInstance& x)
    {
        _resolveCallback = _resolveSCMOInstanceNames;
        fprintf(
            stderr,
            "CIMResponseData::appendSCMOInstanceName(cb=%p)\n",
            _resolveCallback);
        _scmoInstanceNames.append(x);
    }

    Array<Uint8>& getBinaryCimInstanceNames()
    {
        return _binaryData;
    }

    bool setBinaryCimInstanceNames(CIMBuffer& in, bool hasLen=true);
    bool setXmlCimInstanceNames(CIMBuffer& in);

    void encodeBinaryResponse(CIMBuffer& out);
    void encodeXmlResponse(Buffer& out);

private:

    CIMInstanceNamesResponseData(const CIMInstanceNamesResponseData&);
    CIMInstanceNamesResponseData& operator=(
        const CIMInstanceNamesResponseData&);

    static Boolean _resolveXMLInstanceNames(
        CIMInstanceNamesResponseData* data,
        Array<CIMObjectPath>& instanceNames);

    static Boolean _resolveBinaryInstanceNames(
        CIMInstanceNamesResponseData* data,
        Array<CIMObjectPath>& instanceNames);

    static Boolean _resolveSCMOInstanceNames(
        CIMInstanceNamesResponseData* data,
        Array<CIMObjectPath>& instanceNames);

    Boolean (*_resolveCallback)(
        CIMInstanceNamesResponseData* data,
        Array<CIMObjectPath>& cimInstance);

    void _resolve()
    {
        if (_resolveCallback)
        {
            (*_resolveCallback)(this, _instanceNames);
            _resolveCallback = 0;
        }
    }

    ResponseDataEncoding _encoding;

    // For XML encoding.
    Array<ArraySint8> _referencesData;
    Array<String> _hostsData;
    Array<CIMNamespaceName> _nameSpacesData;

    // For binary encoding.
    Array<Uint8> _binaryData;

    Array<CIMObjectPath> _instanceNames;
    Array<SCMOInstance> _scmoInstanceNames;
};


//-----------------------------------------------------------------------------
//
//  CIMInstanceResponseData
//
//-----------------------------------------------------------------------------

class PEGASUS_COMMON_LINKAGE CIMInstanceResponseData
{
public:

    CIMInstanceResponseData():
        _resolveCallback(0),
        _binaryEncoding(false)
    {
    }

    CIMInstance& getCimInstance()
    {
        _resolve();
        return _cimInstance;
    }

    const CIMConstInstance& getCimInstance() const
    {
        // Resolve the instance before returning it.  The resolve step requires
        // non-const access, but does not fundamentally change the response
        const_cast<CIMInstanceResponseData*>(this)->_resolve();

        // The CIMInstance is masqueraded as a CIMConstInstance for expedience,
        // since the internal representations are the same.
        return *((CIMConstInstance*)(void*)&_cimInstance);
    }

    void setCimInstance(const CIMInstance& x)
    {
        _resolveCallback = 0;
        _cimInstance = x;
    }

    Array<Uint8>& getBinaryCimInstance()
    {
        return _binaryData;
    }

    bool setBinaryCimInstance(CIMBuffer& in, bool hasLen=true);
    bool setXmlCimInstance(CIMBuffer& in);

    void encodeBinaryResponse(CIMBuffer& out) const;
    void encodeXmlResponse(Buffer& out) const;

private:

    CIMInstanceResponseData(const CIMInstanceResponseData&);
    CIMInstanceResponseData& operator=(
        const CIMInstanceResponseData&);

    static Boolean _resolveXMLInstance(
        CIMInstanceResponseData* data,
        CIMInstance& instance);

    static Boolean _resolveBinaryInstance(
        CIMInstanceResponseData* data,
        CIMInstance& instance);

    Boolean (*_resolveCallback)(
        CIMInstanceResponseData* msg,
        CIMInstance& cimInstance);

    void _resolve()
    {
        if (_resolveCallback)
        {
            (*_resolveCallback)(this, _cimInstance);
            _resolveCallback = 0;
        }
    }

    Boolean _binaryEncoding;

    // For XML encoding:
    Array<Sint8> _instanceData;
    Array<Sint8> _referenceData;
    CIMNamespaceName _nameSpaceData;
    String _hostData;

    // For Binary encoding:
    Array<Uint8> _binaryData;

    CIMInstance _cimInstance;

};


//-----------------------------------------------------------------------------
//
//  CIMInstancesResponseData
//
//-----------------------------------------------------------------------------

class PEGASUS_COMMON_LINKAGE CIMInstancesResponseData
{
public:

    enum ResponseDataEncoding {
        RESP_ENC_CIM,
        RESP_ENC_BINARY,
        RESP_ENC_XML,
        RESP_ENC_SCMO
    };

    CIMInstancesResponseData():
        _resolveCallback(0),
        _encoding(RESP_ENC_CIM)
    {
    }

    Array<CIMInstance>& getNamedInstances()
    {
        _resolve();
        return _namedInstances;
    }

    const Array<CIMConstInstance>& getNamedInstances() const
    {
        // Resolve the instances before returning them.  The resolve step
        // requires non-const access, but does not fundamentally change the
        // message contents.
        const_cast<CIMInstancesResponseData*>(this)->_resolve();

        // The Array<CIMInstance> is masqueraded as an Array<CIMConstInstance>
        // for expedience, since the internal representations are the same.
        return *((Array<CIMConstInstance>*)(void*)&_namedInstances);
    }

    void setNamedInstances(const Array<CIMInstance>& x)
    {
        _resolveCallback = 0;
        _namedInstances = x;
    }

    void appendNamedInstance(const CIMInstance& x)
    {
        _resolveCallback = 0;
        _namedInstances.append(x);
    }

    void setSCMOInstances(const Array<SCMOInstance>& x)
    {
        _resolveCallback = _resolveSCMOInstances;
        fprintf(
            stderr,
            "CIMResponseData::setSCMOInstances(cb=%p)\n",
            _resolveCallback);
        _scmoInstances = x;
    }

    void appendSCMOInstance(const SCMOInstance& x)
    {
        _resolveCallback = _resolveSCMOInstances;
        fprintf(
            stderr,
            "CIMResponseData::appendSCMOInstance(cb=%p)\n",
            _resolveCallback);
        _scmoInstances.append(x);
    }

    Array<Uint8>& getBinaryCimInstances()
    {
        return _binaryData;
    }

    bool setBinaryCimInstances(CIMBuffer& in, bool hasLen=true);
    bool setXmlCimInstances(CIMBuffer& in);

    void encodeBinaryResponse(CIMBuffer& out);
    void encodeXmlResponse(Buffer& out);

private:

    CIMInstancesResponseData(const CIMInstancesResponseData&);
    CIMInstancesResponseData& operator=(
        const CIMInstancesResponseData&);

    static Boolean _resolveXMLInstances(
        CIMInstancesResponseData* data,
        Array<CIMInstance>& instances);

    static Boolean _resolveBinaryInstances(
        CIMInstancesResponseData* data,
        Array<CIMInstance>& instances);

    static Boolean _resolveSCMOInstances(
        CIMInstancesResponseData* data,
        Array<CIMInstance>& instances);

    Boolean (*_resolveCallback)(
        CIMInstancesResponseData* data,
        Array<CIMInstance>& cimInstance);

    void _resolve()
    {
        if (_resolveCallback)
        {
            (*_resolveCallback)(this, _namedInstances);
            _resolveCallback = 0;
        }
    }

    ResponseDataEncoding _encoding;

    // For XML encoding.
    Array<ArraySint8> _instancesData;
    Array<ArraySint8> _referencesData;
    Array<String> _hostsData;
    Array<CIMNamespaceName> _nameSpacesData;

    // For binary encoding.
    Array<Uint8> _binaryData;

    Array<CIMInstance> _namedInstances;
    Array<SCMOInstance> _scmoInstances;
};


//-----------------------------------------------------------------------------
//
//  CIMObjectsResponseData
//
//-----------------------------------------------------------------------------

class PEGASUS_COMMON_LINKAGE CIMObjectsResponseData
{
public:

    CIMObjectsResponseData():
        _resolveCallback(0),
        _binaryEncoding(false)
    {
    }

    Array<CIMObject>& getCIMObjects()
    {
        _resolve();
        return _cimObjects;
    }

    const Array<CIMConstObject>& getCIMObjects() const
    {
        // Resolve the instances before returning them.  The resolve step
        // requires non-const access, but does not fundamentally change the
        // message contents.
        const_cast<CIMObjectsResponseData*>(this)->_resolve();

        // The Array<CIMInstance> is masqueraded as an Array<CIMConstInstance>
        // for expedience, since the internal representations are the same.
        return *((Array<CIMConstObject>*)(void*)&_cimObjects);
    }

    void setCIMObjects(const Array<CIMObject>& x)
    {
        _resolveCallback = 0;
        _cimObjects = x;
    }

    Array<Uint8>& getBinaryCimObjects()
    {
        return _binaryData;
    }

    bool setBinaryCimObjects(CIMBuffer& in, bool hasLen=true);
    bool setXmlCimObjects(CIMBuffer& in);

    void encodeBinaryResponse(CIMBuffer& out) const;
    void encodeXmlResponse(Buffer& out) const;

private:

    CIMObjectsResponseData(const CIMObjectsResponseData&);
    CIMObjectsResponseData& operator=(
        const CIMObjectsResponseData&);

    static Boolean _resolveXMLObjects(
        CIMObjectsResponseData* data,
        Array<CIMObject>& objects);

    static Boolean _resolveBinaryObjects(
        CIMObjectsResponseData* data,
        Array<CIMObject>& objects);

    Boolean (*_resolveCallback)(
        CIMObjectsResponseData* data,
        Array<CIMObject>& cimObjects);

    void _resolve()
    {
        if (_resolveCallback)
        {
            (*_resolveCallback)(this, _cimObjects);
            _resolveCallback = 0;
        }
    }

    Boolean _binaryEncoding;

    // For XML encoding.
    // For XML encoding.
    Array<ArraySint8> _cimObjectsData;
    Array<ArraySint8> _referencesData;
    Array<String> _hostsData;
    Array<CIMNamespaceName> _nameSpacesData;

    // For binary encoding.
    Array<Uint8> _binaryData;

    Array<CIMObject> _cimObjects;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_CIMResponseData_h */
