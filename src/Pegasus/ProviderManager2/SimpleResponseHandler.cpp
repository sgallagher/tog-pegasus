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
//         Brian G. Campbell, EMC (campbell_brian@emc.com) - PEP140/phase2
//
//%/////////////////////////////////////////////////////////////////////////////

#include "SimpleResponseHandler.h"

#include <Pegasus/ProviderManager2/OperationResponseHandler.h>

#include <Pegasus/Common/Logger.h>

PEGASUS_NAMESPACE_BEGIN

//
// SimpleResponseHandler
//

SimpleResponseHandler::SimpleResponseHandler(void)
{
}

SimpleResponseHandler::~SimpleResponseHandler(void)
{
}

void SimpleResponseHandler::processing(void)
{
    Logger::put(
        Logger::STANDARD_LOG,
        System::CIMSERVER,
        Logger::TRACE,
        "SimpleResponseHandler::processing()");

    // do nothing
}

void SimpleResponseHandler::complete(void)
{
    Logger::put(
        Logger::STANDARD_LOG,
        System::CIMSERVER,
        Logger::TRACE,
        "SimpleResponseHandler::complete()");

    send(true);
}

// return the number of objects in this handler
Uint32 SimpleResponseHandler::size(void) const
{
    return(0);
}

// clear any objects in this handler
void SimpleResponseHandler::clear(void)
{
}

ContentLanguages SimpleResponseHandler::getLanguages(void)
{
	Logger::put(
        Logger::STANDARD_LOG,
        System::CIMSERVER,
        Logger::TRACE,
        "SimpleResponseHandler: getLanguages()");

	ContentLanguages langs;

    try
	{
		// Try to get the ContentLanguages out of the OperationContext
		// in the base ResponseHandler.
		OperationContext context = getContext();

        ContentLanguageListContainer cntr = context.get(
            ContentLanguageListContainer::NAME);

        langs = cntr.getLanguages();
	}
	catch (const Exception &)
	{
		// The content language container must not exist.
		// Return the empty ContentLanguages.
	}

	return(langs);
}

void SimpleResponseHandler::send(Boolean isComplete)
{
	// If this was NOT instantiated as a derived OperationResponseHandle class,
	// then this will be null but is NOT an error. In this case, there is no
	// response attached, hence no data,so there is nothing to send. else we have
	// a valid "cross-cast" to the operation side

	OperationResponseHandler *operation =
		dynamic_cast<OperationResponseHandler*>(this);

	if (operation)
    {
        operation->send(isComplete);
    }
}

//
// SimpleInstanceResponseHandler
//

SimpleInstanceResponseHandler::SimpleInstanceResponseHandler(void)
{
}

void SimpleInstanceResponseHandler::processing(void)
{
    SimpleResponseHandler::processing();
}

void SimpleInstanceResponseHandler::complete(void)
{
    SimpleResponseHandler::complete();
}

Uint32 SimpleInstanceResponseHandler::size(void) const
{
    return(_objects.size());
}

void SimpleInstanceResponseHandler::clear(void)
{
    _objects.clear();
}

void SimpleInstanceResponseHandler::deliver(const CIMInstance & instance)
{
    Logger::put(
        Logger::STANDARD_LOG,
        System::CIMSERVER,
        Logger::TRACE,
        "SimpleInstanceResponseHandler::deliver()");

    _objects.append(instance);

    send(false);
}

void SimpleInstanceResponseHandler::deliver(const Array<CIMInstance> & instances)
{
    // call deliver for each object in the array
    for(Uint32 i = 0, n = instances.size(); i < n; i++)
    {
        deliver(instances[i]);
    }
}

const Array<CIMInstance> SimpleInstanceResponseHandler::getObjects(void) const
{
    return(_objects);
}

//
// SimpleObjectPathResponseHandler
//

SimpleObjectPathResponseHandler::SimpleObjectPathResponseHandler(void)
{
}

void SimpleObjectPathResponseHandler::processing(void)
{
    SimpleResponseHandler::processing();
}

void SimpleObjectPathResponseHandler::complete(void)
{
    SimpleResponseHandler::complete();
}

Uint32 SimpleObjectPathResponseHandler::size(void) const
{
    return(_objects.size());
}

void SimpleObjectPathResponseHandler::clear(void)
{
    _objects.clear();
}

void SimpleObjectPathResponseHandler::deliver(const CIMObjectPath & objectPath)
{
    Logger::put(
        Logger::STANDARD_LOG,
        System::CIMSERVER,
        Logger::TRACE,
        "SimpleObjectPathResponseHandler::deliver()");

    _objects.append(objectPath);

    send(false);
}

void SimpleObjectPathResponseHandler::deliver(const Array<CIMObjectPath> & objectPaths)
{
    // call deliver for each object in the array
    for(Uint32 i = 0, n = objectPaths.size(); i < n; i++)
    {
        deliver(objectPaths[i]);
    }
}

const Array<CIMObjectPath> SimpleObjectPathResponseHandler::getObjects(void) const
{
    return(_objects);
}

//
// SimpleMethodResultResponseHandler
//

SimpleMethodResultResponseHandler::SimpleMethodResultResponseHandler(void)
{
}

void SimpleMethodResultResponseHandler::processing(void)
{
    SimpleResponseHandler::processing();
}

void SimpleMethodResultResponseHandler::complete(void)
{
    SimpleResponseHandler::complete();
}

Uint32 SimpleMethodResultResponseHandler::size(void) const
{
    return(_objects.size());
}

void SimpleMethodResultResponseHandler::clear(void)
{
    _objects.clear();

    _returnValue.clear();
}

void SimpleMethodResultResponseHandler::deliverParamValue(const CIMParamValue & outParamValue)
{
    _objects.append(outParamValue);

    // async delivers not yet supported for parameters
    //send(false);
}

void SimpleMethodResultResponseHandler::deliverParamValue(const Array<CIMParamValue> & outParamValues)
{
    // call deliver for each object in the array
    for(Uint32 i = 0, n = outParamValues.size(); i < n; i++)
    {
        deliverParamValue(outParamValues[i]);
    }
}

void SimpleMethodResultResponseHandler::deliver(const CIMValue & returnValue)
{
    Logger::put(
        Logger::STANDARD_LOG,
        System::CIMSERVER,
        Logger::TRACE,
        "SimpleMethodResultResponseHandler::deliver()");

    _returnValue = returnValue;

    send(false);
}

const Array<CIMParamValue> SimpleMethodResultResponseHandler::getParamValues(void) const
{
    return(_objects);
}

const CIMValue SimpleMethodResultResponseHandler::getReturnValue(void) const
{
    return(_returnValue);
}

//
// SimpleIndicationResponseHandler
//

SimpleIndicationResponseHandler::SimpleIndicationResponseHandler(void)
{
}

void SimpleIndicationResponseHandler::processing(void)
{
    SimpleResponseHandler::processing();
}

void SimpleIndicationResponseHandler::complete(void)
{
    SimpleResponseHandler::complete();
}

Uint32 SimpleIndicationResponseHandler::size(void) const
{
    return(_objects.size());
}

void SimpleIndicationResponseHandler::clear(void)
{
    _objects.clear();
}

void SimpleIndicationResponseHandler::deliver(const CIMIndication & indication)
{
    Logger::put(
        Logger::STANDARD_LOG,
        System::CIMSERVER,
        Logger::TRACE,
        "SimpleIndicationResponseHandler::deliver()");

    _objects.append(indication);

    send(false);
}

void SimpleIndicationResponseHandler::deliver(const Array<CIMIndication> & indications)
{
    // call deliver for each object in the array
    for(Uint32 i = 0, n = indications.size(); i < n; i++)
    {
        deliver(indications[i]);
    }
}

void SimpleIndicationResponseHandler::deliver(
    const OperationContext & context,
    const CIMIndication & indication)
{
    Logger::put(
        Logger::STANDARD_LOG,
        System::CIMSERVER,
        Logger::TRACE,
        "SimpleIndicationResponseHandler::deliver()");

    _objects.append(indication);
}

void SimpleIndicationResponseHandler::deliver(
    const OperationContext & context,
    const Array<CIMIndication> & indications)
{
    // call deliver for each object in the array
    for(Uint32 i = 0, n = indications.size(); i < n; i++)
    {
        deliver(indications[i]);
    }
}

const Array<CIMIndication> SimpleIndicationResponseHandler::getObjects(void) const
{
    return(_objects);
}


//
// SimpleObjectResponseHandler
//

SimpleObjectResponseHandler::SimpleObjectResponseHandler(void)
{
}

void SimpleObjectResponseHandler::processing(void)
{
    SimpleResponseHandler::processing();
}

void SimpleObjectResponseHandler::complete(void)
{
    SimpleResponseHandler::complete();
}

Uint32 SimpleObjectResponseHandler::size(void) const
{
    return(_objects.size());
}

void SimpleObjectResponseHandler::clear(void)
{
    _objects.clear();
}

void SimpleObjectResponseHandler::deliver(const CIMObject & object)
{
    Logger::put(
        Logger::STANDARD_LOG,
        System::CIMSERVER,
        Logger::TRACE,
        "SimpleObjectResponseHandler::deliver()");

    _objects.append(object);

    send(false);
}

void SimpleObjectResponseHandler::deliver(const Array<CIMObject> & objects)
{
    // call deliver for each object in the array
    for(Uint32 i = 0, n = objects.size(); i < n; i++)
    {
        deliver(objects[i]);
    }
}

const Array<CIMObject> SimpleObjectResponseHandler::getObjects(void) const
{
    return(_objects);
}

//
// SimpleInstance2ObjectResponseHandler
//

SimpleInstance2ObjectResponseHandler::SimpleInstance2ObjectResponseHandler(void)
{
}

void SimpleInstance2ObjectResponseHandler::processing(void)
{
    SimpleResponseHandler::processing();
}

void SimpleInstance2ObjectResponseHandler::complete(void)
{
    SimpleResponseHandler::complete();
}

Uint32 SimpleInstance2ObjectResponseHandler::size(void) const
{
    return(_objects.size());
}

void SimpleInstance2ObjectResponseHandler::clear(void)
{
    _objects.clear();
}

void SimpleInstance2ObjectResponseHandler::deliver(const CIMInstance & object)
{
    Logger::put(
        Logger::STANDARD_LOG,
        System::CIMSERVER,
        Logger::TRACE,
        "SimpleInstance2ObjectResponseHandler::deliver()");

    _objects.append(CIMObject(object));

    // async delivers not yet supported
    //send(false);
}

void SimpleInstance2ObjectResponseHandler::deliver(const Array<CIMInstance> & objects)
{
    // call deliver for each object in the array
    for(Uint32 i = 0, n = objects.size(); i < n; i++)
    {
        deliver(objects[i]);
    }
}

const Array<CIMObject> SimpleInstance2ObjectResponseHandler::getObjects(void) const
{
    return(_objects);
}

//
// SimpleValueResponseHandler
//

SimpleValueResponseHandler::SimpleValueResponseHandler(void)
{
}

void SimpleValueResponseHandler::processing(void)
{
    SimpleResponseHandler::processing();
}

void SimpleValueResponseHandler::complete(void)
{
    SimpleResponseHandler::complete();
}

Uint32 SimpleValueResponseHandler::size(void) const
{
    return(_objects.size());
}

void SimpleValueResponseHandler::clear(void)
{
    _objects.clear();
}

void SimpleValueResponseHandler::deliver(const CIMValue & value)
{
    Logger::put(
        Logger::STANDARD_LOG,
        System::CIMSERVER,
        Logger::TRACE,
        "SimpleValueResponseHandler::deliver()");

    _objects.append(value);

    send(false);
}

void SimpleValueResponseHandler::deliver(const Array<CIMValue> & values)
{
    // call deliver for each object in the array
    for(Uint32 i = 0, n = values.size(); i < n; i++)
    {
        deliver(values[i]);
    }
}

const Array<CIMValue> SimpleValueResponseHandler::getObjects(void) const
{
    return(_objects);
}

PEGASUS_NAMESPACE_END
