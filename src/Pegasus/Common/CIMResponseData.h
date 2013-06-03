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
// Class CIMResponseData encapsulates the possible types of response data
// representations and supplies conversion methods between these types.
// PEP#348 - The CMPI infrastructure using SCMO (Single Chunk Memory Objects)
// describes its usage in the server flow.
// The design document can be found on the OpenPegasus website openpegasus.org
// at https://collaboration.opengroup.org/pegasus/pp/documents/21210/PEP_348.pdf
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
#include <Pegasus/Common/Magic.h>

PEGASUS_NAMESPACE_BEGIN
PEGASUS_USING_STD;

// The following is a Trace tool that traces ALL cimResponseData calls
// when enabled
// KS_TODO _ Delete this
//#define TRACELINE cout << __FILE__ << ":" << __LINE__ << endl;
#define TRACELINE

typedef Array<Sint8> ArraySint8;
#define PEGASUS_ARRAY_T ArraySint8
# include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T

//// KS_TODO REMOVE THIS _Temp test for size validity DELETE when done
#define PSVALID PEGASUS_ASSERT(sizeValid());
/*
#define PSVALID {printf("PSVALID Error line %u\n",__LINE__); \
 PEGASUS_ASSERT(sizeValid());}
#define PSVALID {if(!sizeValid(){printf("PSVALID Error line %u\n",__LINE__);
PEGASUS_ASSERT(false);}}
*/
class PEGASUS_COMMON_LINKAGE CIMResponseData
{
public:

    enum ResponseDataEncoding {
        RESP_ENC_CIM = 1,
        RESP_ENC_BINARY = 2,
        RESP_ENC_XML = 4,
        RESP_ENC_SCMO = 8
    };

    enum ResponseDataContent {
        RESP_INSTNAMES = 1,
        RESP_INSTANCES = 2,
        RESP_INSTANCE = 3,
        RESP_OBJECTS = 4,
        RESP_OBJECTPATHS = 5
    };
    //includeClassOrigin & _includeQualifiers are set to true by default.
    //_propertyList is initialized to an empty propertylist to enable
    // sending all properties by default. _isClassOperation set false and
    // only reset by selected operations (ex. associator response builder)
    CIMResponseData(ResponseDataContent content):
        _encoding(0),_dataType(content),_size(0), _includeQualifiers(true),
        _includeClassOrigin(true),
        _isClassOperation(false),
        _propertyList(CIMPropertyList())
    {
        TRACELINE;
        PEGASUS_ASSERT(valid()); // KS_TEMP KS_TODO DELETE THIS
        size();
    }

    CIMResponseData(const CIMResponseData & x):
        _encoding(x._encoding),
        _mapObjectsToIntances(x._mapObjectsToIntances),
        _dataType(x._dataType),
        _size(x._size),
        _referencesData(x._referencesData),
        _instanceData(x._instanceData),
        _hostsData(x._hostsData),
        _nameSpacesData(x._nameSpacesData),
        _binaryData(x._binaryData),
        _defaultNamespace(x._defaultNamespace),
        _defaultHostname(x._defaultHostname),
        _instanceNames(x._instanceNames),
        _instances(x._instances),
        _objects(x._objects),
        _scmoInstances(x._scmoInstances),
        _includeQualifiers(x._includeQualifiers),
        _includeClassOrigin(x._includeClassOrigin),
        _isClassOperation(x._isClassOperation),
        _propertyList(x._propertyList),
        _magic(x._magic)
    {
        TRACELINE;
        /////PEGASUS_ASSERT(x.valid());     // KS_TEMP
        PEGASUS_ASSERT(valid());            // KS_TEMP
        size();
    }

    // Construct an empty object.  Issue here in that we would like to
    // assure that this is invalid but if we add the _dataType parameter
    // it must be a valid one.  The alternative would be to define an
    // invalid enum but that would cost us in all case/if statements.
    // Therefore up to the user to create and then use correctly.
    // KS_TODO fix this so that the empty state is somehow protected.
    CIMResponseData():
        _encoding(0),_mapObjectsToIntances(false), _size(0),
        _includeQualifiers(true), _includeClassOrigin(true),
        _propertyList(CIMPropertyList())
    {
        TRACELINE;
        size();   //// TODOD remove temp
    }

    /**
     * Move CIM objects from another CIMResponse data object to this
     * CIMResponseData object. Moves the number of objects defined
     * in the count parameter from one to another. They are removed
     * from the from object and inserted into the to object.
     *
     * @param CIMResponseData from which the objects are moved
     * @param count Uint32 count of objects to move
     * @return - Actual number of objects moved.
     */
    Uint32 moveObjects(CIMResponseData & x, Uint32 count);

    /**
     * Return count of the number of CIM objects in the
     * CIMResponseData object
     * @return Uint32 The count of the number of CIM objects
     * (instances, paths, or objects)in the CIMResponsedata object
     */
    Uint32 size();

    ~CIMResponseData()
    {
    }

    // Issue with pull and other operations
    // in that the other assoc operations return objects and objectPaths
    // and the pulls return instances and instancePaths. The pull operation
    // must be able to handle either so we use this to reset the datatype
    // KS_TODO -- This should check size and only allow if nothing in the
    // object.
    Boolean setDataType(ResponseDataContent content)
    {
        TRACELINE;
        PEGASUS_ASSERT(valid());
        PEGASUS_ASSERT(_size == 0);      // KS_TODO_TEMP or debug mode.
        _dataType = content;
        return true;
    }

    // get the datatype property
    ResponseDataContent getResponseDataContent()
    {
        TRACELINE;
        PEGASUS_ASSERT(valid());            // KS_TEMP KS_TODO
        return _dataType;
    }
    // C++ objects interface handling

    // Instance Names handling
    Array<CIMObjectPath>& getInstanceNames();

    void setInstanceNames(const Array<CIMObjectPath>& x)
    {
        TRACELINE;
        _instanceNames=x;
        _encoding |= RESP_ENC_CIM;
        _size += x.size();
    }

    // Instance handling
    CIMInstance& getInstance();

    void setInstance(const CIMInstance& x)
    {
        TRACELINE;
        PSVALID;
        _instances.clear();
        _instances.append(x);
        _size++;
        _encoding |= RESP_ENC_CIM;

        PSVALID;
    }

    // Instances handling
    Array<CIMInstance>& getInstances();

    // Get an array of CIMInstances from the CIMResponseData converting from
    // any of the internal forms to the C++ format.  This will also convert
    // CIMObjects to CIMInstances if there are any CIMObjects.
    // NOTE: This is a temporary solution to satisfy the BinaryCodec passing
    // of data to the client where the data could be either instances or
    // objects.  The correct solution is to convert back when the provider, etc.
    // returns the data to the server.  We must convert to that solution but
    // this keeps the BinaryCodec working for the moment.
    // Expect that this will be used only in CIMCLient.cpp
    Array<CIMInstance>& getInstancesFromInstancesOrObjects();

    void setInstances(const Array<CIMInstance>& x)
    {
        TRACELINE;
        PSVALID;
        _instances=x;
        _encoding |= RESP_ENC_CIM;
        _size += x.size();
        PSVALID;
    }

    void appendInstance(const CIMInstance& x)
    {
        TRACELINE;
        PEGASUS_ASSERT(valid());
        _instances.append(x);
        _encoding |= RESP_ENC_CIM;
        _size += 1;
    }

    void appendInstances(const Array<CIMInstance>& x)
    {
        TRACELINE;
        PEGASUS_ASSERT(valid());
        _instances.appendArray(x);
        _encoding |= RESP_ENC_CIM;
        _size += x.size();
    }

    // Objects handling
    Array<CIMObject>& getObjects();
    void setObjects(const Array<CIMObject>& x)
    {
        TRACELINE;
        PEGASUS_ASSERT(valid());
        _objects=x;
        _encoding |= RESP_ENC_CIM;
        _size += x.size();
    }
    void appendObject(const CIMObject& x)
    {
        TRACELINE;
        PEGASUS_ASSERT(valid());
        _objects.append(x);
        _encoding |= RESP_ENC_CIM;
        _size += 1;
    }

    // SCMO representation, single instance stored as one element array
    // object paths are represented as SCMOInstance
    Array<SCMOInstance>& getSCMO();

    void setSCMO(const Array<SCMOInstance>& x);

    void appendSCMO(const Array<SCMOInstance>& x)
    {
        TRACELINE;
        PEGASUS_ASSERT(valid());
        _scmoInstances.appendArray(x);
        _encoding |= RESP_ENC_SCMO;
        _size += x.size();
    }

    Array<Uint8>& getBinary();

    // Set the binary stream in the CIMBuffer into the CIMResponseData
    bool setBinary(CIMBuffer& in);

    // Set any data from the CIMBuffer remaining after the current CIMBuffer
    // position pointer into the CIMResponseData Object.
    bool setRemainingBinaryData(CIMBuffer& in);

    // Xml data is unformatted, no need to differentiate between instance
    // instances and object paths or objects
    bool setXml(CIMBuffer& in);

    // function used by OperationAggregator to aggregate response data in a
    // single ResponseData object
    void appendResponseData(const CIMResponseData & x);

    // Function used by CMPI layer to complete the namespace on all data held
    // Input (x) has to have a valid namespace
    void completeNamespace(const SCMOInstance * x);

    // Function primarily used by CIMOperationRequestDispatcher to complete
    // namespace and hostname on a,an,r and rn operations in the
    // OperationAggregator
    void completeHostNameAndNamespace(
        const String & hn,
        const CIMNamespaceName & ns);

    // Encoding responses

    // Encode the CIMResponse data into binary format used with Provider Agents
    // and OP Clients in the provider CIMBuffer
    void encodeBinaryResponse(CIMBuffer& out);

    // Encode the CIMResponse data into Xml format used with Provider Agents
    //  only in the provided CIMBuffer
    void encodeInternalXmlResponse(CIMBuffer& out);

    // Encode the CIMResponse data into official Xml format(CIM over Http)
    // used to communicate to clients in the provided CIMBuffer.
    //Note that the pull responses requires a flag.
    void encodeXmlResponse(Buffer& out, Boolean isPull);

    // diagnostic tests magic number in context to see if valid object
    Boolean valid();

    // official Xml format(CIM over Http) used to communicate to clients
    void encodeXmlResponse(Buffer& out);

    //This function is called from buildResponce to set CIMResponcedata
    //with respective values of IncludeQualifiers,IncludeClassOrigin and
    //propertyFilter.
    void setRequestProperties(
        const Boolean includeQualifiers,
        const Boolean includeClassOrigin,
        const CIMPropertyList& propertyList);

    // Used with association and reference operations (i.e. operations that
    // return CIMObject or CIMObjectPath to set a parameter to define whether
    // responseData is for operation on a class or instance.
    // Allows building the correct path (classPath or instancePath) and
    // object type (Class or Instance) on response.
    void setIsClassOperation(Boolean b);

    void setPropertyList(const CIMPropertyList& propertyList)
    {
        TRACELINE;
        _propertyList = propertyList;
    }
    CIMPropertyList & getPropertyList()
    {
        TRACELINE;
        return _propertyList;
    }

    bool sizeValid();                   //KS_TEMP KS_TODO REMOVE

private:

    // helper functions to transform different formats into one-another
    // functions work on the internal data and calling of them should be
    // avoided

    void _resolveToCIM();
    void _resolveToSCMO();

    void _resolveBinary();

    void _resolveXmlToSCMO();
    void _resolveXmlToCIM();

    void _resolveSCMOToCIM();
    void _resolveCIMToSCMO();

    // Helper functions for this class only, do NOT externalize
    SCMOInstance _getSCMOFromCIMInstance(const CIMInstance& cimInst);
    SCMOInstance _getSCMOFromCIMObject(const CIMObject& cimObj);
    SCMOInstance _getSCMOFromCIMObjectPath(const CIMObjectPath& cimPath);
    SCMOClass* _getSCMOClass(const char* ns,const char* cls);
    void _deserializeInstance(Uint32 idx,CIMInstance& cimInstance);
    void _deserializeObject(Uint32 idx,CIMObject& cimObject);
    Boolean _deserializeReference(Uint32 idx,CIMObjectPath& cimObjectPath);
    Boolean _deserializeInstanceName(Uint32 idx,CIMObjectPath& cimObjectPath);

    // Bitflags in this integer will reflect what data representation types
    // are currently stored in this CIMResponseData object
    Uint32 _encoding;

    // Special flag to handle the case where binary data in passed through the
    // system but must be mapped to instances in the getInstances.  This
    // accounts for only one case today, binary data in the BinaryCodec
    Boolean _mapObjectsToIntances;

    // Storing type of data in this enumeration
    ResponseDataContent _dataType;

    // Count of objects stored in this CIMResponseData object.  This is the
    // accumulated count of objects stored in all of the data
    // representations
    Uint32 _size;

    // unused arrays are represented by ArrayRepBase _empty_rep
    // which is a 16 byte large static shared across all of them
    // so, even though this object looks large, it holds just
    // 2 integer and 9 pointers

    // For XML encoding.
    Array<ArraySint8> _referencesData;
    Array<ArraySint8> _instanceData;
    Array<String> _hostsData;
    Array<CIMNamespaceName> _nameSpacesData;

    // For binary encoding.
    Array<Uint8> _binaryData;
    CIMNamespaceName _defaultNamespace;
    String _defaultHostname;

    // Default C++ encoding
    Array<CIMObjectPath> _instanceNames;
    Array<CIMInstance> _instances;
    Array<CIMObject> _objects;

    // SCMO encoding
    Array<SCMOInstance> _scmoInstances;

    // Request characteristics that are carried through operation for
    // modification of response generation.
    Boolean _includeQualifiers;
    Boolean _includeClassOrigin;
    // Defines whether response CIMObjects or ObjectPaths are class or instance.
    // because associators, etc. operations provide both class and instance
    // responses. Default is false and should only be set to true by
    // operation requests such as associators (which could return either
    // instances or classes) when the operation is to return class information.
    Boolean _isClassOperation;
    CIMPropertyList _propertyList;

    // magic number to use with valid function to confirm validity
    // of response data.
    Magic<0x57D11323> _magic;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_CIMResponseData_h */
