//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_OperationResponseHandler_h
#define Pegasus_OperationResponseHandler_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/CIMIndication.h>

#include <Pegasus/Provider/ResponseHandler.h>
#include <Pegasus/Provider/SimpleResponseHandler.h>

PEGASUS_NAMESPACE_BEGIN

template<class T>
class OperationResponseHandler : public SimpleResponseHandler<T>
{
public:
    OperationResponseHandler(CIMRequestMessage * request, CIMResponseMessage * response);
    virtual ~OperationResponseHandler(void);

    CIMRequestMessage * getRequest(void) const;
    CIMResponseMessage * getResponse(void) const;

    virtual void complete(void);
    virtual void complete(const OperationContext & context);

    virtual void setStatus(const Uint32 code, const String & message = String::EMPTY);

protected:
    CIMRequestMessage * _request;
    CIMResponseMessage * _response;

};

template<class T>
inline OperationResponseHandler<T>::OperationResponseHandler(CIMRequestMessage * request, CIMResponseMessage * response)
    : _request(request), _response(response)
{
}

template<class T>
inline OperationResponseHandler<T>::~OperationResponseHandler(void)
{
}

template<class T>
inline CIMRequestMessage * OperationResponseHandler<T>::getRequest(void) const
{
    return(_request);
}

template<class T>
inline CIMResponseMessage * OperationResponseHandler<T>::getResponse(void) const
{
    return(_response);
}

template<class T>
inline void OperationResponseHandler<T>::complete(void)
{
    OperationContext context;

    complete(context);
}

template<class T>
inline void OperationResponseHandler<T>::complete(const OperationContext & context)
{
}

template<class T>
inline void OperationResponseHandler<T>::setStatus(const Uint32 code, const String & message)
{
    _response->cimException = PEGASUS_CIM_EXCEPTION(CIMStatusCode(code), message);
}

/* ------------------------------------------------------------------------- */
/* operation specific response handlers                                      */
/* ------------------------------------------------------------------------- */

class GetInstanceResponseHandler:  public OperationResponseHandler<CIMInstance>
{
    public:
        GetInstanceResponseHandler(
            CIMGetInstanceRequestMessage * request,
            CIMGetInstanceResponseMessage * response)
        : OperationResponseHandler<CIMInstance>(request, response)
        {
        }

        virtual void complete(const OperationContext & context)
        {
            if(getObjects().size() > 0)
            {
                ((CIMGetInstanceResponseMessage *)getResponse())->cimInstance =
                     getObjects()[0];
            }
            else
            {
                // error? provider claims success,
                // but did not deliver an instance.
                setStatus(CIM_ERR_NOT_FOUND);
            }
        }
};

    class EnumerateInstancesResponseHandler : public OperationResponseHandler<CIMInstance>
    {
    public:
        EnumerateInstancesResponseHandler(
            CIMEnumerateInstancesRequestMessage * request,
            CIMEnumerateInstancesResponseMessage * response)
        : OperationResponseHandler<CIMInstance>(request, response)
        {
        }

        virtual void complete(const OperationContext & context)
        {
            Array<CIMNamedInstance> cimInstances;

            // ATTN: can be removed once CIMNamedInstance is removed
            for(Uint32 i = 0, n = getObjects().size(); i < n; i++)
            {
                CIMInstance cimInstance(getObjects()[i]);

                cimInstances.append(CIMNamedInstance(cimInstance.getPath(),
                                    cimInstance));
            }

            ((CIMEnumerateInstancesResponseMessage *)
                getResponse())->cimNamedInstances = cimInstances;
        }

    };

    class EnumerateInstanceNamesResponseHandler : public OperationResponseHandler<CIMReference>
    {
    public:
        EnumerateInstanceNamesResponseHandler(
            CIMEnumerateInstanceNamesRequestMessage * request,
            CIMEnumerateInstanceNamesResponseMessage * response)
        : OperationResponseHandler<CIMReference>(request, response)
        {
        }

        virtual void complete(const OperationContext & context)
        {
            ((CIMEnumerateInstanceNamesResponseMessage *)
                getResponse())->instanceNames = getObjects();
        }

    };

    class CreateInstanceResponseHandler : public OperationResponseHandler<CIMReference>
    {
    public:
        CreateInstanceResponseHandler(
            CIMCreateInstanceRequestMessage * request,
            CIMCreateInstanceResponseMessage * response)
        : OperationResponseHandler<CIMReference>(request, response)
        {
        }

        virtual void complete(const OperationContext & context)
        {
            if (getObjects().size() > 0)
            {
                ((CIMCreateInstanceResponseMessage *)
                    getResponse())->instanceName = getObjects()[0];
            }

            // ATTN: is it an error to not return instance name?
        }

    };

    class ModifyInstanceResponseHandler : public OperationResponseHandler<CIMInstance>
    {
    public:
        ModifyInstanceResponseHandler(
            CIMModifyInstanceRequestMessage * request,
            CIMModifyInstanceResponseMessage * response)
        : OperationResponseHandler<CIMInstance>(request, response)
        {
        }

    };

    class DeleteInstanceResponseHandler : public OperationResponseHandler<CIMInstance>
    {
    public:
        DeleteInstanceResponseHandler(
            CIMDeleteInstanceRequestMessage * request,
            CIMDeleteInstanceResponseMessage * response)
        : OperationResponseHandler<CIMInstance>(request, response)
        {
        }

    };


    class AssociatorNamesResponseHandler : public OperationResponseHandler<CIMReference>
    {
    public:
        AssociatorNamesResponseHandler(
            CIMAssociatorNamesRequestMessage * request,
            CIMAssociatorNamesResponseMessage * response)
        : OperationResponseHandler<CIMReference>(request, response)
        {
        }

        virtual void complete(const OperationContext & context)
        {
            ((CIMAssociatorNamesResponseMessage *)
                getResponse())->objectNames.appendArray(getObjects());
        }

    };


    class ReferencesResponseHandler : public OperationResponseHandler<CIMObject>    {
    public:
        ReferencesResponseHandler(
            CIMReferencesRequestMessage * request,
            CIMReferencesResponseMessage * response)
        : OperationResponseHandler<CIMObject>(request, response)
        {
        }

        virtual void complete(const OperationContext & context)
        {
            Array<CIMObjectWithPath> cimObjects;

            // ATTN: can be removed once CIMNamedInstance is removed
            for(Uint32 i = 0, n = getObjects().size(); i < n; i++)
            {
                CIMObject cimObject(getObjects()[i]);

                cimObjects.append(
                    CIMObjectWithPath(cimObject.getPath(), cimObject));
            }
            ((CIMReferencesResponseMessage *)
                getResponse())->cimObjects.appendArray(cimObjects);
        }

    };


    class ReferenceNamesResponseHandler : public OperationResponseHandler<CIMReference>
    {
    public:
        ReferenceNamesResponseHandler(
            CIMReferenceNamesRequestMessage * request,
            CIMReferenceNamesResponseMessage * response)
        : OperationResponseHandler<CIMReference>(request, response)
        {
        }

        virtual void complete(const OperationContext & context)
        {
            ((CIMReferenceNamesResponseMessage *)
                getResponse())->objectNames.appendArray(getObjects());
        }

    };


    class InvokeMethodResponseHandler : public OperationResponseHandler<CIMValue>
    {
    public:
        InvokeMethodResponseHandler(
            CIMInvokeMethodRequestMessage * request,
            CIMInvokeMethodResponseMessage * response)
        : OperationResponseHandler<CIMValue>(request, response)
        {
        }

        virtual void complete(const OperationContext & context)
        {
            // error? provider claims success, but did not deliver a CIMValue.
            if(getObjects().size() != 0)
            {
                ((CIMInvokeMethodResponseMessage *)
                    getResponse())->retValue = getObjects()[0];
            }
        }
    };

PEGASUS_NAMESPACE_END

#endif
