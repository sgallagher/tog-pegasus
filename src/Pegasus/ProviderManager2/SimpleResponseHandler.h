//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By:
//      Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//      Dave Rosckes (rosckes@us.ibm.com)
//      Brian G. Campbell, EMC (campbell_brian@emc.com) - PEP140/phase2
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_SimpleResponseHandler_h
#define Pegasus_SimpleResponseHandler_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/ResponseHandler.h>

#include <Pegasus/ProviderManager2/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

//
// ValueResponseHandler (used internally to implement property operations)
//
class PEGASUS_PPM_LINKAGE ValueResponseHandler : virtual public ResponseHandler
{
public:
    virtual void deliver(const CIMValue & value) = 0;

    virtual void deliver(const Array<CIMValue> & values) = 0;
};


//
// SimpleResponseHandler
//
class PEGASUS_PPM_LINKAGE SimpleResponseHandler : virtual public ResponseHandler
{
public:
    SimpleResponseHandler(void);
    virtual ~SimpleResponseHandler(void);

    virtual void processing(void);

    virtual void complete(void);

    // return the number of objects in this handler
    virtual Uint32 size(void) const;

    // clear any objects in this handler
    virtual void clear(void);

    ContentLanguages getLanguages(void);

protected:
    virtual void send(Boolean isComplete);

};

class PEGASUS_PPM_LINKAGE SimpleInstanceResponseHandler : public SimpleResponseHandler, public InstanceResponseHandler
{
public:
    SimpleInstanceResponseHandler(void);

    virtual void processing(void);

    virtual void complete(void);

    virtual Uint32 size(void) const;

    virtual void clear(void);

    virtual void deliver(const CIMInstance & instance);

    virtual void deliver(const Array<CIMInstance> & instances);

    const Array<CIMInstance> getObjects(void) const;

private:
    Array<CIMInstance> _objects;

};

class PEGASUS_PPM_LINKAGE SimpleObjectPathResponseHandler : public SimpleResponseHandler, public ObjectPathResponseHandler
{
public:
    SimpleObjectPathResponseHandler(void);

    virtual void processing(void);

    virtual void complete(void);

    virtual Uint32 size(void) const;

    virtual void clear(void);

    virtual void deliver(const CIMObjectPath & objectPath);

    virtual void deliver(const Array<CIMObjectPath> & objectPaths);

    const Array<CIMObjectPath> getObjects(void) const;

private:
    Array<CIMObjectPath> _objects;

};

class PEGASUS_PPM_LINKAGE SimpleMethodResultResponseHandler : public SimpleResponseHandler, public MethodResultResponseHandler
{
public:
    SimpleMethodResultResponseHandler(void);

    virtual void processing(void);

    virtual void complete(void);

    // NOTE: this is the total size (count) of ALL parameters!
    virtual Uint32 size(void) const;

    virtual void clear(void);

    virtual void deliverParamValue(const CIMParamValue & outParamValue);

    virtual void deliverParamValue(const Array<CIMParamValue> & outParamValues);

    virtual void deliver(const CIMValue & returnValue);

    const Array<CIMParamValue> getParamValues(void) const;

    const CIMValue getReturnValue(void) const;

private:
    Array<CIMParamValue> _objects;

    CIMValue _returnValue;

};

class PEGASUS_PPM_LINKAGE SimpleIndicationResponseHandler : public SimpleResponseHandler, public IndicationResponseHandler
{
public:
    SimpleIndicationResponseHandler(void);

    virtual void processing(void);

    virtual void complete(void);

    virtual Uint32 size(void) const;

    virtual void clear(void);

    virtual void deliver(const CIMIndication & indication);

    virtual void deliver(const Array<CIMIndication> & indications);

    virtual void deliver(
        const OperationContext & context,
        const CIMIndication & indication);

    virtual void deliver(
        const OperationContext & context,
        const Array<CIMIndication> & indications);

    const Array<CIMIndication> getObjects(void) const;

    // ATTN: why is this variable public?
    CIMInstance _provider;

private:
    Array<CIMIndication> _objects;

};

class PEGASUS_PPM_LINKAGE SimpleObjectResponseHandler : public SimpleResponseHandler, public ObjectResponseHandler
{
public:
    SimpleObjectResponseHandler(void);

    virtual void processing(void);

    virtual void complete(void);

    virtual Uint32 size(void) const;

    virtual void clear(void);

    virtual void deliver(const CIMObject & object);

    virtual void deliver(const Array<CIMObject> & objects);

    const Array<CIMObject> getObjects(void) const;

private:
    Array<CIMObject> _objects;

};

class PEGASUS_PPM_LINKAGE SimpleInstance2ObjectResponseHandler : public SimpleResponseHandler, public InstanceResponseHandler
{
public:
    SimpleInstance2ObjectResponseHandler(void);

    virtual void processing(void);

    virtual void complete(void);

    virtual Uint32 size(void) const;

    virtual void clear(void);

    virtual void deliver(const CIMInstance & object);

    virtual void deliver(const Array<CIMInstance> & objects);

    const Array<CIMObject> getObjects(void) const;

private:
    Array<CIMObject> _objects;

};

class PEGASUS_PPM_LINKAGE SimpleValueResponseHandler : public SimpleResponseHandler, public ValueResponseHandler
{
public:
    SimpleValueResponseHandler(void);

    virtual void processing(void);

    virtual void complete(void);

    virtual Uint32 size(void) const;

    virtual void clear(void);

    virtual void deliver(const CIMValue & value);

    virtual void deliver(const Array<CIMValue> & values);

    const Array<CIMValue> getObjects(void) const;

private:
    Array<CIMValue> _objects;

};

PEGASUS_NAMESPACE_END

#endif
