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
// Modified By: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//              Dave Rosckes (rosckes@us.ibm.com)
//              Brian G. Campbell, EMC (campbell_brian@emc.com) - PEP140/phase2
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_SimpleResponseHandler_h
#define Pegasus_SimpleResponseHandler_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/ResponseHandler.h>
#include <Pegasus/Common/Logger.h>

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
    SimpleResponseHandler(void)
    {
    }

    virtual ~SimpleResponseHandler(void)
    {
    }

    virtual void processing(void)
    {
	Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
		    "SimpleResponseHandler: processing()");
        // do nothing
    }

    virtual void complete(void)
    {
	Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
		    "SimpleResponseHandler: complete()");

	send(true);
    }

		// return the number of objects in this handler
   	virtual Uint32 size() const { return 0;	}

		// clear any objects in this handler
	  virtual void clear() {}

// l10n
    ContentLanguages getLanguages(void);

protected:
	// send (deliver) asynchronously with restrictions
	virtual void send(Boolean isComplete);

};

class PEGASUS_PPM_LINKAGE SimpleInstanceResponseHandler : public SimpleResponseHandler, public InstanceResponseHandler
{
public:
    SimpleInstanceResponseHandler(void)
    {
    }

    virtual void processing(void) { SimpleResponseHandler::processing(); }
    virtual void complete(void) { SimpleResponseHandler::complete(); }
		virtual Uint32 size() const { return _objects.size(); }
		virtual void clear() { _objects.clear(); }

    virtual void deliver(const CIMInstance & instance)
    {
	Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
		    "SimpleResponseHandler: deliver()");

        _objects.append(instance);
				send(false);
    }

    virtual void deliver(const Array<CIMInstance> & instances)
    {
        // call deliver for each object in the array
        for(Uint32 i = 0, n = instances.size(); i < n; i++)
        {
            deliver(instances[i]);
        }
    }

    const Array<CIMInstance> getObjects(void) const
    {
        return _objects;
    }

private:
    Array<CIMInstance> _objects;

};

class PEGASUS_PPM_LINKAGE SimpleObjectPathResponseHandler : public SimpleResponseHandler, public ObjectPathResponseHandler
{
public:
    SimpleObjectPathResponseHandler(void)
    {
    }

    virtual void processing(void) { SimpleResponseHandler::processing(); }
    virtual void complete(void) { SimpleResponseHandler::complete(); }
	  virtual Uint32 size() const { return _objects.size(); }
		virtual void clear() { _objects.clear(); }

    virtual void deliver(const CIMObjectPath & objectPath)
    {
	Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
		    "SimpleResponseHandler: deliver()");
        _objects.append(objectPath);
				send(false);
    }

    virtual void deliver(const Array<CIMObjectPath> & objectPaths)
    {
        // call deliver for each object in the array
        for(Uint32 i = 0, n = objectPaths.size(); i < n; i++)
        {
            deliver(objectPaths[i]);
        }
    }

    const Array<CIMObjectPath> getObjects(void) const
    {
        return _objects;
    }

private:
    Array<CIMObjectPath> _objects;

};

class PEGASUS_PPM_LINKAGE SimpleMethodResultResponseHandler : public SimpleResponseHandler, public MethodResultResponseHandler
{
public:
    SimpleMethodResultResponseHandler(void)
    {
    }

    virtual void processing(void) { SimpleResponseHandler::processing(); }
    virtual void complete(void) { SimpleResponseHandler::complete(); }
	  // NOTE: this is the total size (count) of ALL parameters!
		virtual Uint32 size() const { return _objects.size(); }
		virtual void clear() { _objects.clear(); _returnValue.clear(); }

    virtual void deliverParamValue(const CIMParamValue & outParamValue)
    {
        _objects.append(outParamValue);
				// async delivers not yet supported for parameters
				//send(false);
    }

    virtual void deliverParamValue(const Array<CIMParamValue> & outParamValues)
    {
        // call deliver for each object in the array
        for(Uint32 i = 0, n = outParamValues.size(); i < n; i++)
        {
            deliverParamValue(outParamValues[i]);
        }
    }

    virtual void deliver(const CIMValue & returnValue)
    {
	Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
		    "SimpleResponseHandler: deliver()");
        _returnValue = returnValue;
				send(false);
    }

    const Array<CIMParamValue> getParamValues(void) const
    {
        return _objects;
    }

    const CIMValue getReturnValue(void) const
    {
        return _returnValue;
    }

private:
    Array<CIMParamValue> _objects;

    CIMValue _returnValue;

};

class PEGASUS_PPM_LINKAGE SimpleIndicationResponseHandler : public SimpleResponseHandler, public IndicationResponseHandler
{
public:
    SimpleIndicationResponseHandler(void)
    {
    }

    virtual void processing(void) { SimpleResponseHandler::processing(); }
    virtual void complete(void) { SimpleResponseHandler::complete(); }
	  virtual Uint32 size() const { return _objects.size(); }
		virtual void clear() { _objects.clear(); }

    virtual void deliver(const CIMIndication & indication)
    {
	Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
		    "SimpleResponseHandler: deliver()");

        _objects.append(indication);
				send(false);
    }

    virtual void deliver(const Array<CIMIndication> & indications)
    {
        // call deliver for each object in the array
        for(Uint32 i = 0, n = indications.size(); i < n; i++)
        {
            deliver(indications[i]);
        }
    }

    virtual void deliver(
        const OperationContext & context,
        const CIMIndication & indication)
    {
	Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
		    "SimpleResponseHandler: deliver()");

        _objects.append(indication);
    }

    virtual void deliver(
        const OperationContext & context,
        const Array<CIMIndication> & indications)
    {
        // call deliver for each object in the array
        for(Uint32 i = 0, n = indications.size(); i < n; i++)
        {
            deliver(indications[i]);
        }
    }

    const Array<CIMIndication> getObjects(void) const
    {
        return _objects;
    }

    CIMInstance _provider;

private:
    Array<CIMIndication> _objects;

};

class PEGASUS_PPM_LINKAGE SimpleObjectResponseHandler : public SimpleResponseHandler, public ObjectResponseHandler
{
public:
    SimpleObjectResponseHandler(void)
    {
    }

    virtual void processing(void) { SimpleResponseHandler::processing(); }
    virtual void complete(void) { SimpleResponseHandler::complete(); }
	  virtual Uint32 size() const { return _objects.size(); }
		virtual void clear() { _objects.clear(); }

    virtual void deliver(const CIMObject & object)
    {
	Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
		    "SimpleResponseHandler: deliver()");

        _objects.append(object);
				send(false);
    }

    virtual void deliver(const Array<CIMObject> & objects)
    {
        // call deliver for each object in the array
        for(Uint32 i = 0, n = objects.size(); i < n; i++)
        {
            deliver(objects[i]);
        }
    }

    const Array<CIMObject> getObjects(void) const
    {
        return _objects;
    }

private:
    Array<CIMObject> _objects;

};

class PEGASUS_PPM_LINKAGE SimpleInstance2ObjectResponseHandler : public SimpleResponseHandler, public InstanceResponseHandler
{
public:
    SimpleInstance2ObjectResponseHandler(void)
    {
    }

    virtual void processing(void) { SimpleResponseHandler::processing(); }
    virtual void complete(void) { SimpleResponseHandler::complete(); }
		virtual Uint32 size() const { return _objects.size(); }
		virtual void clear() { _objects.clear(); }

    virtual void deliver(const CIMInstance & object)
    {
	Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
		    "SimpleResponseHandler: deliver()");

        _objects.append(CIMObject(object));
				// async delivers not yet supported
				//send(false);
    }

    virtual void deliver(const Array<CIMInstance> & objects)
    {
        // call deliver for each object in the array
        for(Uint32 i = 0, n = objects.size(); i < n; i++)
        {
            deliver(objects[i]);
        }
    }

    const Array<CIMObject> getObjects(void) const
    {
        return _objects;
    }

private:
    Array<CIMObject> _objects;

};

class PEGASUS_PPM_LINKAGE SimpleValueResponseHandler : public SimpleResponseHandler, public ValueResponseHandler
{
public:
    SimpleValueResponseHandler(void)
    {
    }

    virtual void processing(void) { SimpleResponseHandler::processing(); }
    virtual void complete(void) { SimpleResponseHandler::complete(); }
		virtual Uint32 size() const { return _objects.size(); }
		virtual void clear() { _objects.clear(); }

    virtual void deliver(const CIMValue & value)
    {
	Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
		    "SimpleResponseHandler: deliver()");

        _objects.append(value);
				send(false);
    }

    virtual void deliver(const Array<CIMValue> & values)
    {
        // call deliver for each object in the array
        for(Uint32 i = 0, n = values.size(); i < n; i++)
        {
            deliver(values[i]);
        }
    }

    const Array<CIMValue> getObjects(void) const
    {
        return _objects;
    }

private:
    Array<CIMValue> _objects;

};

PEGASUS_NAMESPACE_END

#endif
