//%//////////////////////////-*-c++-*-//////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
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
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_OperationResponseHandler_h
#define Pegasus_OperationResponseHandler_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Server/Linkage.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/MessageQueueService.h>
#include <Pegasus/Common/Constants.h>

#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMIndication.h>
#include <Pegasus/Common/CIMValue.h>

#include <Pegasus/Common/ResponseHandler.h>
#include <Pegasus/Common/IPC.h>

PEGASUS_NAMESPACE_BEGIN

class ResponseHandlerState
{
   public:
      enum OP_STATE
      {
	 UNKNOWN,
	 INITIALIZED,
	 PROCESSING,
	 COMPLETE,
	 FAILED
      };
      

      ResponseHandlerState(void)
	 :_state(INITIALIZED)
      {
      }
      
      virtual ~ResponseHandlerState(void)
      {
      }
      
      
      Uint32 get_state(void)
      {
	 return _state.value();
      }
      
      void set_state(OP_STATE state)
      {
	 _state = state;
      }
      
      AtomicInt _state;
};


/* ------------------------------------------------------------------------- */
/* operation specific response handlers                                      */
/* ------------------------------------------------------------------------- */

class SimpleOperationResponseHandler : public ResponseHandler, virtual public ResponseHandlerState
{
   public:
      
      SimpleOperationResponseHandler(void)
      {
	 
      }
      
      virtual void processing(void)
      {
	 set_state(PROCESSING);
      }
      
      virtual void processing(OperationContext & context)
      {
	 processing();
      }
      
      virtual void complete(void)
      {
	 set_state(COMPLETE);
      }
      
      virtual void complete(OperationContext & context)
      {
	 complete();
      }
      
      virtual void setStatus(Uint32 code, const String & message = String::EMPTY)
      {
	 _status = code;
	 _msg = message;
      }
      
   private:
      Uint32 _status;
      String _msg;
      
};



class OperationResponseHandler : public ResponseHandler, virtual public ResponseHandlerState
{
   public:
      OperationResponseHandler(void)
	 : _request_ptr(0),
	   _response_ptr(0)
      {
      }
      
      OperationResponseHandler(
	 CIMRequestMessage *request, 
	 CIMResponseMessage *response)
	 : _request_ptr(request),
	   _response_ptr(response)
      {
	 
      }

      
      virtual void processing(void)
      {
	 set_state(PROCESSING);
      }
      
      virtual void processing(OperationContext & context)
      {
	 processing();
      }
      
      virtual void complete(void)
      {
	 set_state(COMPLETE);
      }
      
      virtual void complete(OperationContext & context)
      {
	 complete();
      }
      
      virtual void setStatus(Uint32 code, const String & message = String::EMPTY)
      {
	 if(_response_ptr)
	    _response_ptr->cimException = PEGASUS_CIM_EXCEPTION(CIMStatusCode(code), message);
	 
	 _status = code;
	 _msg = message;
      }
      
      CIMRequestMessage *getRequest(void)
      {
	 return _request_ptr;
      }
      
      CIMResponseMessage * getResponse(void)
      {
	 return _response_ptr;
      }
      
   private:
      Uint32 _status;
      String _msg;
      
      CIMRequestMessage *_request_ptr;
      CIMResponseMessage *_response_ptr;
      
};



class SimpleInstanceResponseHandler: public InstanceResponseHandler, virtual public ResponseHandlerState
{
   public:
      SimpleInstanceResponseHandler(void)
      {
      }
      
      virtual void processing(void)
      {
	 set_state(PROCESSING);
      }
      
      virtual void processing(OperationContext & context)
      {
	 processing();
      }
      
      virtual void complete(void)
      {
	 set_state(COMPLETE);
      }
      
      virtual void complete(OperationContext & context)
      {
	 complete();
      }
      
      virtual void deliver(const CIMInstance & instance)
      {
	 _objects.append(instance);
      }
      
      virtual void deliver(const Array<CIMInstance> & instances)
      {
	 for(Uint32 i = 0, n = instances.size(); i < n; i++)
	 {
	    deliver(instances[i]);
	 }
      }
      
      const Array<CIMInstance> & getObjects(void) const
      {
	 return(_objects);
      }
      
      virtual void setStatus(Uint32 code, const String & message = String::EMPTY)
      {
	
      }
      
   private:
      Array<CIMInstance> _objects;
};



class GetInstanceResponseHandler: public InstanceResponseHandler, virtual public ResponseHandlerState
{
   public:
      GetInstanceResponseHandler(CIMGetInstanceRequestMessage *request,
				 CIMGetInstanceResponseMessage *response)
	 : _request_ptr(request),
	   _response_ptr(response)
      {
      }
      
      virtual void processing(void)
      {
	 set_state(PROCESSING);
      }
      
      virtual void processing(OperationContext & context)
      {
	 processing();
      }
      
      virtual void complete(void)
      {
	 set_state(COMPLETE);
	 if(getObjects().size() == 0)
	 {
            // error? provider claims success,
            // but did not deliver an instance.
            setStatus(CIM_ERR_NOT_FOUND);
	    
            return;
	 }

	 getResponse()->cimInstance = getObjects()[0];
	 
      }
      
      virtual void complete(OperationContext & context)
      {
	 complete();
      }
      
      virtual void deliver(const CIMInstance & instance)
      {
	 _objects.append(instance);
      }
      
      virtual void deliver(const Array<CIMInstance> & instances)
      {
	 for(Uint32 i = 0, n = instances.size(); i < n; i++)
	 {
	    deliver(instances[i]);
	 }
      }
      
      const Array<CIMInstance> & getObjects(void) const
      {
	 return(_objects);
      }
      
      virtual void setStatus(Uint32 code, const String & message = String::EMPTY)
      {
	 _response_ptr->cimException = PEGASUS_CIM_EXCEPTION(CIMStatusCode(code), message);
      }
      
      CIMGetInstanceRequestMessage *getRequest(void)
      {
	 return _request_ptr;
      }
      
      CIMGetInstanceResponseMessage * getResponse(void)
      {
	 return _response_ptr;
      }
      
   private:
      Array<CIMInstance> _objects;
      CIMGetInstanceRequestMessage * _request_ptr;
      CIMGetInstanceResponseMessage * _response_ptr;
};


class EnumerateInstancesResponseHandler: public InstanceResponseHandler, virtual public ResponseHandlerState
{
   public:
      EnumerateInstancesResponseHandler(CIMEnumerateInstancesRequestMessage * request,
					CIMEnumerateInstancesResponseMessage * response)
	 : _request_ptr(request),
	   _response_ptr(response)
      {
      }
      
      virtual void processing(void)
      {
	 set_state(PROCESSING);
      }
      
      virtual void processing(OperationContext & context)
      {
	 processing();
      }
      
      virtual void complete(void)
      {
	 set_state(COMPLETE);
	 getResponse()->cimNamedInstances = getObjects();
      }
      
      virtual void complete(OperationContext & context)
      {
	 complete();
      }
      
      virtual void deliver(const CIMInstance & instance)
      {
	 _objects.append(instance);
      }
      
      virtual void deliver(const Array<CIMInstance> & instances)
      {
	 for(Uint32 i = 0, n = instances.size(); i < n; i++)
	 {
	    deliver(instances[i]);
	 }
      }
      
      const Array<CIMInstance> & getObjects(void) const
      {
	 return(_objects);
      }
      
      virtual void setStatus(Uint32 code, const String & message = String::EMPTY)
      {
	 _response_ptr->cimException = PEGASUS_CIM_EXCEPTION(CIMStatusCode(code), message);
      }
      
      CIMEnumerateInstancesRequestMessage *getRequest(void)
      {
	 return _request_ptr;
      }

      CIMEnumerateInstancesResponseMessage * getResponse(void)
      {
	 return _response_ptr;
      }
      
   private:
      Array<CIMInstance> _objects;
      CIMEnumerateInstancesRequestMessage * _request_ptr;
      CIMEnumerateInstancesResponseMessage * _response_ptr;

};

class SimpleObjectPathResponseHandler: public ObjectPathResponseHandler, virtual public ResponseHandlerState
{

   public:
      SimpleObjectPathResponseHandler(void)
      {
      }
      
      virtual void processing(void)
      {
	 set_state(PROCESSING);
      }
      
      virtual void processing(OperationContext & context)
      {
	 processing();
      }
      
      virtual void complete(void)
      {
	 set_state(COMPLETE);
      }
      
      virtual void complete(OperationContext & context)
      {
	 complete();
      }
      
      virtual void deliver(const CIMObjectPath & instance)
      {
	 _objects.append(instance);
      }
      
      virtual void deliver(const Array<CIMObjectPath> & instances)
      {
	 _objects.appendArray(instances);
      }
      
      const Array<CIMObjectPath> & getObjects(void) const
      {
	 return(_objects);
      }
      
      virtual void setStatus(Uint32 code, const String & message = String::EMPTY)
      {
	 _status = code;
	 _msg = message;
      }
      
   private:
      Uint32 _status;
      String _msg;
      Array<CIMObjectPath> _objects;
};



class EnumerateInstanceNamesResponseHandler: public ObjectPathResponseHandler, virtual public ResponseHandlerState
{
   public:
      EnumerateInstanceNamesResponseHandler(
	 CIMEnumerateInstanceNamesRequestMessage * request,
	 CIMEnumerateInstanceNamesResponseMessage * response)
	 : _request_ptr(request),
	   _response_ptr(response)
      {
      }
      
      virtual void processing(void)
      {
	 set_state(PROCESSING);
      }
      
      virtual void processing(OperationContext & context)
      {
	 processing();
      }
      
      virtual void complete(void)
      {
	 set_state(COMPLETE);
	 getResponse()->instanceNames = getObjects();
      }
      
      virtual void complete(OperationContext & context)
      {
	 complete();
      }
      
      virtual void deliver(const CIMObjectPath & instance)
      {
	 _objects.append(instance);
      }
      
      virtual void deliver(const Array<CIMObjectPath> & instances)
      {
	 for(Uint32 i = 0, n = instances.size(); i < n; i++)
	 {
	    deliver(instances[i]);
	 }
      }
      
      const Array<CIMObjectPath> & getObjects(void) const
      {
	 return(_objects);
      }
      
      virtual void setStatus(Uint32 code, const String & message = String::EMPTY)
      {
	 _response_ptr->cimException = PEGASUS_CIM_EXCEPTION(CIMStatusCode(code), message);
      }
      
       CIMEnumerateInstanceNamesRequestMessage *getRequest(void)
      {
	 return _request_ptr;
      }
      
      CIMEnumerateInstanceNamesResponseMessage *getResponse(void)
      {
	 return _response_ptr;
      }
      
   private:
      Array<CIMObjectPath> _objects;
      CIMEnumerateInstanceNamesRequestMessage * _request_ptr;
      CIMEnumerateInstanceNamesResponseMessage * _response_ptr;
};


class CreateInstanceResponseHandler: public ObjectPathResponseHandler, virtual public ResponseHandlerState
{
   public:
      CreateInstanceResponseHandler(
	 CIMCreateInstanceRequestMessage * request,
	 CIMCreateInstanceResponseMessage * response)
	 : _request_ptr(request),
	   _response_ptr(response)
      {
      }
      
      virtual void processing(void)
      {
	 set_state(PROCESSING);
      }
      
      virtual void processing(OperationContext & context)
      {
	 processing();
      }
      
      virtual void complete(void)
      {
	 set_state(COMPLETE);
	 if (getObjects().size() == 0 )
	    return;
	 getResponse()->instanceName = getObjects()[0];
      }
      
      virtual void complete(OperationContext & context)
      {
	 complete();
      }
      
      virtual void deliver(const CIMObjectPath & instance)
      {
	 _objects.append(instance);
      }
      
      virtual void deliver(const Array<CIMObjectPath> & instances)
      {
	 for(Uint32 i = 0, n = instances.size(); i < n; i++)
	 {
	    deliver(instances[i]);
	 }
      }
      
      const Array<CIMObjectPath> & getObjects(void) const
      {
	 return(_objects);
      }
      
      virtual void setStatus(Uint32 code, const String & message = String::EMPTY)
      {
	 _response_ptr->cimException = PEGASUS_CIM_EXCEPTION(CIMStatusCode(code), message);
      }
      
      CIMCreateInstanceRequestMessage *getRequest(void)
      {
	 return _request_ptr;
      }
      
      CIMCreateInstanceResponseMessage *getResponse(void)
      {
	 return _response_ptr;
      }
      
   private:
      Array<CIMObjectPath> _objects;
      CIMCreateInstanceRequestMessage * _request_ptr;
      CIMCreateInstanceResponseMessage * _response_ptr;
};



class ModifyInstanceResponseHandler: virtual public ResponseHandler, virtual public ResponseHandlerState
{
   public:
      ModifyInstanceResponseHandler(
	 CIMModifyInstanceRequestMessage * request,
	 CIMModifyInstanceResponseMessage * response )
	 : _request_ptr(request),
	   _response_ptr(response)
      {
      }
      
      virtual void processing(void)
      {
	 set_state(PROCESSING);
      }
      
      virtual void processing(OperationContext & context)
      {
	 processing();
      }
      
      virtual void complete(void)
      {
	 set_state(COMPLETE);
      }
      
      virtual void complete(OperationContext & context)
      {
	 complete();
      }
      
      
      virtual void setStatus(Uint32 code, const String & message = String::EMPTY)
      {
	 _response_ptr->cimException = PEGASUS_CIM_EXCEPTION(CIMStatusCode(code), message);
      }
      
      CIMModifyInstanceRequestMessage *getRequest(void)
      {
	 return _request_ptr;
      }
      
      CIMModifyInstanceResponseMessage *getResponse(void)
      {
	 return _response_ptr;
      }
      
   private:
      CIMModifyInstanceRequestMessage * _request_ptr;
      CIMModifyInstanceResponseMessage * _response_ptr;
};


class DeleteInstanceResponseHandler: virtual public ResponseHandler, virtual public ResponseHandlerState
{
   public:
      
      DeleteInstanceResponseHandler(CIMDeleteInstanceRequestMessage * request,
				    CIMDeleteInstanceResponseMessage * response)
	 : _request_ptr(request),
	   _response_ptr(response)
      {
      }
      
      virtual void processing(void)
      {
	 set_state(PROCESSING);
      }
      
      virtual void processing(OperationContext & context)
      {
	 processing();
      }
      
      virtual void complete(void)
      {
	 set_state(COMPLETE);
      }
      
      virtual void complete(OperationContext & context)
      {
	 complete();
      }
      
      
      virtual void setStatus(Uint32 code, const String & message = String::EMPTY)
      {
	 _response_ptr->cimException = PEGASUS_CIM_EXCEPTION(CIMStatusCode(code), message);
      }
      
       CIMDeleteInstanceRequestMessage *getRequest(void)
      {
	 return _request_ptr;
      }
      
      CIMDeleteInstanceResponseMessage * getResponse(void)
      {
	 return _response_ptr;
      }
      
   private:

      CIMDeleteInstanceRequestMessage * _request_ptr;
      CIMDeleteInstanceResponseMessage * _response_ptr;
};



class GetPropertyResponseHandler: public ValueResponseHandler, virtual public ResponseHandlerState
{
   public:
      GetPropertyResponseHandler( 
	 CIMGetPropertyRequestMessage * request,
	 CIMGetPropertyResponseMessage * response)
	 : _request_ptr(request),
	   _response_ptr(response)
      {
      }
      
      virtual void processing(void)
      {
	 set_state(PROCESSING);
      }
      
      virtual void processing(OperationContext & context)
      {
	 processing();
      }
      
      virtual void complete(void)
      {
	 set_state(COMPLETE);
	 if(getObjects().size() == 0)
	 {
            // error? provider claims success,
            // but did not deliver an instance.
            setStatus(CIM_ERR_NOT_FOUND);
	    return;
	 }
	 
	 getResponse()->value = getObjects()[0];
      }
      
      virtual void complete(OperationContext & context)
      {
	 complete();
      }
      
      virtual void deliver(const CIMValue & instance)
      {
	 _objects.append(instance);
      }
      
      virtual void deliver(const Array<CIMValue> & instances)
      {
	 for(Uint32 i = 0, n = instances.size(); i < n; i++)
	 {
	    deliver(instances[i]);
	 }
      }
      
      const Array<CIMValue> & getObjects(void) const
      {
	 return(_objects);
      }
      
      virtual void setStatus(Uint32 code, const String & message = String::EMPTY)
      {
	 _response_ptr->cimException = PEGASUS_CIM_EXCEPTION(CIMStatusCode(code), message);
      }
      
      CIMGetPropertyRequestMessage *getRequest(void)
      {
	 return _request_ptr;
      }
      
      CIMGetPropertyResponseMessage * getResponse(void)
      {
	 return _response_ptr;
      }
      
   private:
      Array<CIMValue> _objects;
      CIMGetPropertyRequestMessage * _request_ptr;
      CIMGetPropertyResponseMessage * _response_ptr;
};


class SetPropertyResponseHandler: virtual public ResponseHandler, virtual public ResponseHandlerState
{
   public:
      SetPropertyResponseHandler(
	 CIMSetPropertyRequestMessage * request,
	 CIMSetPropertyResponseMessage * response)
	 : _request_ptr(request),
	   _response_ptr(response)
      {
      }
      
      virtual void processing(void)
      {
	 set_state(PROCESSING);
      }
      
      virtual void processing(OperationContext & context)
      {
	 processing();
      }
      
      virtual void complete(void)
      {
	 set_state(COMPLETE);
      }
      
      virtual void complete(OperationContext & context)
      {
	 complete();
      }
      
      virtual void setStatus(Uint32 code, const String & message = String::EMPTY)
      {
	 _response_ptr->cimException = PEGASUS_CIM_EXCEPTION(CIMStatusCode(code), message);
      }
      
      CIMSetPropertyRequestMessage *getRequest(void)
      {
	 return _request_ptr;
      }
      
      CIMSetPropertyResponseMessage * getResponse(void)
      {
	 return _response_ptr;
      }
      
   private:
      CIMSetPropertyRequestMessage *_request_ptr;
      CIMSetPropertyResponseMessage *_response_ptr;
};


class AssociatorNamesResponseHandler: public ObjectPathResponseHandler, virtual public ResponseHandlerState
{
   public:
      AssociatorNamesResponseHandler(
	 CIMAssociatorNamesRequestMessage * request,
	 CIMAssociatorNamesResponseMessage * response)
	 : _request_ptr(request),
	   _response_ptr(response)
      {
      }
      
      virtual void processing(void)
      {
	 set_state(PROCESSING);
      }
      
      virtual void processing(OperationContext & context)
      {
	 processing();
      }
      
      virtual void complete(void)
      {
	 set_state(COMPLETE);
	 getResponse()->objectNames.appendArray(getObjects());
      }
      
      virtual void complete(OperationContext & context)
      {
	 complete();
      }
      
      virtual void deliver(const CIMObjectPath & instance)
      {
	 _objects.append(instance);
      }
      
      virtual void deliver(const Array<CIMObjectPath> & instances)
      {
	 for(Uint32 i = 0, n = instances.size(); i < n; i++)
	 {
	    deliver(instances[i]);
	 }
      }
      
      const Array<CIMObjectPath> & getObjects(void) const
      {
	 return(_objects);
      }
      
      virtual void setStatus(Uint32 code, const String & message = String::EMPTY)
      {
	 _response_ptr->cimException = PEGASUS_CIM_EXCEPTION(CIMStatusCode(code), message);
      }
      
      CIMAssociatorNamesRequestMessage *getRequest(void)
      {
	 return _request_ptr;
      }
      
      CIMAssociatorNamesResponseMessage * getResponse(void)
      {
	 return _response_ptr;
      }
      
   private:
      Array<CIMObjectPath> _objects;
      CIMAssociatorNamesRequestMessage *_request_ptr;
      CIMAssociatorNamesResponseMessage *_response_ptr;
};


class ReferencesResponseHandler: public ObjectResponseHandler, virtual public ResponseHandlerState
{
   public:
      ReferencesResponseHandler(
	 CIMReferencesRequestMessage *request,
	 CIMReferencesResponseMessage *response)
	 : _request_ptr(request),
	   _response_ptr(response)
      {
      }
      
      virtual void processing(void)
      {
	 set_state(PROCESSING);
      }
      
      virtual void processing(OperationContext & context)
      {
	 processing();
      }
      
      virtual void complete(void)
      {
	 set_state(COMPLETE);
	 getResponse()->cimObjects = getObjects();
      }
      
      virtual void complete(OperationContext & context)
      {
	 complete();
      }
      
      virtual void deliver(const CIMObject & instance)
      {
	 _objects.append(instance);
      }
      
      virtual void deliver(const Array<CIMObject> & instances)
      {
	 for(Uint32 i = 0, n = instances.size(); i < n; i++)
	 {
	    deliver(instances[i]);
	 }
      }
      
      const Array<CIMObject> & getObjects(void) const
      {
	 return(_objects);
      }
      
      virtual void setStatus(Uint32 code, const String & message = String::EMPTY)
      {
	 _response_ptr->cimException = PEGASUS_CIM_EXCEPTION(CIMStatusCode(code), message);
      }
      
      CIMReferencesRequestMessage *getRequest(void)
      {
	 return _request_ptr;
      }
      
      CIMReferencesResponseMessage * getResponse(void)
      {
	 return _response_ptr;
      }
      
   private:
      Array<CIMObject> _objects;
      CIMReferencesRequestMessage *_request_ptr;
      CIMReferencesResponseMessage *_response_ptr;
};



class ReferenceNamesResponseHandler: public ObjectPathResponseHandler, virtual public ResponseHandlerState
{
   public:
      ReferenceNamesResponseHandler( 
	 CIMReferenceNamesRequestMessage *request,
	 CIMReferenceNamesResponseMessage *response)
	 : _request_ptr(request),
	   _response_ptr(response)
      {
      }
      
      virtual void processing(void)
      {
	 set_state(PROCESSING);
      }
      
      virtual void processing(OperationContext & context)
      {
	 processing();
      }
      
      virtual void complete(void)
      {
	 set_state(COMPLETE);
	 getResponse()->objectNames.appendArray(getObjects());
      }
      
      virtual void complete(OperationContext & context)
      {
	 complete();
      }
      
      virtual void deliver(const CIMObjectPath & instance)
      {
	 _objects.append(instance);
      }
      
      virtual void deliver(const Array<CIMObjectPath> & instances)
      {
	 for(Uint32 i = 0, n = instances.size(); i < n; i++)
	 {
	    deliver(instances[i]);
	 }
      }
      
      const Array<CIMObjectPath> & getObjects(void) const
      {
	 return(_objects);
      }
      
      virtual void setStatus(Uint32 code, const String & message = String::EMPTY)
      {
	 _response_ptr->cimException = PEGASUS_CIM_EXCEPTION(CIMStatusCode(code), message);
      }
      
      CIMReferenceNamesRequestMessage *getRequest(void)
      {
	 return _request_ptr;
      }
      
      CIMReferenceNamesResponseMessage *getResponse(void)
      {
	 return _response_ptr;
      }
      
   private:
      Array<CIMObjectPath> _objects;
      CIMReferenceNamesRequestMessage *_request_ptr;
      CIMReferenceNamesResponseMessage *_response_ptr;
};

class SimpleMethodResultResponseHandler: public MethodResultResponseHandler, virtual public ResponseHandlerState
{
   public:
      SimpleMethodResultResponseHandler(void) 
      {
      }
      
      virtual void processing(void)
      {
	 set_state(PROCESSING);
      }
      
      virtual void processing(OperationContext & context)
      {
	 processing();
      }
      
      virtual void complete(void)
      {
	 set_state(COMPLETE);
      }
      
      virtual void complete(OperationContext & context)
      {
	 complete();
      }

      void deliverParamValue(const CIMParamValue & outParamValue)
      {
	 _params.append(outParamValue);
      }
      

      void deliverParamValue(const Array<CIMParamValue> & outParamValues)
      {
	 _params.appendArray(outParamValues);
      }
      
      virtual void deliver(const CIMValue & instance)
      {
	 _objects.append(instance);
      }
      
      virtual void deliver(const Array<CIMValue> & instances)
      {
	 _objects.appendArray(instances);
      }
      
      const Array<CIMValue> & getObjects(void) const
      {
	 return(_objects);
      }
      
      const Array<CIMParamValue> & getParamValues(void) const
      {
	 return(_params);
      }
      
      const CIMValue & getReturnValue(void) const
      {
	 return _objects[0];
      }
      
      virtual void setStatus(Uint32 code, const String & message = String::EMPTY)
      {
	 _status = code;
	 _msg = message;
      }
   private:
      Uint32 _status;
      String _msg;
      Array<CIMParamValue> _params;
      Array<CIMValue> _objects;
};

class InvokeMethodResponseHandler: public MethodResultResponseHandler, virtual public ResponseHandlerState
{
   public:
      InvokeMethodResponseHandler( 
	 CIMInvokeMethodRequestMessage * request,
	 CIMInvokeMethodResponseMessage * response)
	 : _request_ptr(request),
	   _response_ptr(response)
      {
      }
      
      virtual void processing(void)
      {
	 set_state(PROCESSING);
      }
      
      virtual void processing(OperationContext & context)
      {
	 processing();
      }
      
      virtual void complete(void)
      {
	 set_state(COMPLETE);
	 if(getObjects().size() == 0 )
	    return;
	 
	 getResponse()->outParameters = getObjects();
      }
      
      virtual void complete(OperationContext & context)
      {
	 complete();
      }

      void deliverParamValue(const CIMParamValue & outParamValue)
      {
	 _params.append(outParamValue);
      }
      

      void deliverParamValue(const Array<CIMParamValue> & outParamValues)
      {
	 _params.appendArray(outParamValues);
      }
      
      virtual void deliver(const CIMValue & instance)
      {
	 _objects.append(instance);
      }
      
      virtual void deliver(const Array<CIMValue> & instances)
      {
	 _objects.appendArray(instances);
      }
      
      const Array<CIMParamValue> & getObjects(void) const
      {
	 return(_params);
      }
      
      virtual void setStatus(Uint32 code, const String & message = String::EMPTY)
      {
	 _response_ptr->cimException = PEGASUS_CIM_EXCEPTION(CIMStatusCode(code), message);
      }
      
      CIMInvokeMethodRequestMessage *getRequest(void)
      {
	 return _request_ptr;
      }
      
      CIMInvokeMethodResponseMessage * getResponse(void)
      {
	 return _response_ptr;
      }
      
   private:
      Array<CIMParamValue> _params;
      Array<CIMValue> _objects;
      CIMInvokeMethodRequestMessage * _request_ptr;
      CIMInvokeMethodResponseMessage * _response_ptr;
};

class HandleIndicationResponseHandler: public IndicationResponseHandler, virtual public ResponseHandlerState
{
   public:
      HandleIndicationResponseHandler(void)
      {
      }
      virtual void processing(void)
      {
	 set_state(PROCESSING);
      }
      
      virtual void processing(OperationContext & context)
      {
	 processing();
      }
      
      virtual void complete(void)
      {
	 set_state(COMPLETE);
      }
      
      virtual void complete(OperationContext & context)
      {
	 complete();
      }
      
      const Array<CIMIndication> & getObjects(void) const
      {
	 return(_objects);

      }

      virtual void deliver(const CIMIndication & indication)
      {
	 _objects.append(indication);
      }

      virtual void deliver(const Array<CIMIndication> & indications)
      {
	 _objects.appendArray(indications);
      }
      

      virtual void deliver(const OperationContext & context, 
			   const CIMIndication & indication)
      {
	 deliver(indication);
      }

      virtual void deliver(const OperationContext & context,
			   const Array<CIMIndication> & indications)
      {
	 deliver(indications);
      }

      virtual void setStatus(Uint32 code, const String & message = String::EMPTY)
      {
	 _status = code;
	 _msg = message;
      }
      
   private:

      Uint32  _status;
      String _msg;
      
      Array<CIMIndication> _objects;      
};



class EnableIndicationsResponseHandler: public IndicationResponseHandler, virtual public ResponseHandlerState
{
   public:
      EnableIndicationsResponseHandler(
	 CIMEnableIndicationsRequestMessage *request,
	 CIMEnableIndicationsResponseMessage *response,
	 MessageQueueService * source,
	 MessageQueueService * target = 0)
	 : _source(source),
	   _target(target),
	   _request_copy(*request),
	   _response_copy(*response)
      {

	 PEGASUS_ASSERT(_source != 0);
	 
	 // get indication service
	 if(_target == 0)
	 {
            Array<Uint32> serviceIds;
	    
            _source->find_services(PEGASUS_QUEUENAME_ESERVER_INDICATIONSERVICE, 0, 0, &serviceIds);
	    
            PEGASUS_ASSERT(serviceIds.size() != 0);
	    
            _target = dynamic_cast<MessageQueueService *>(MessageQueue::lookup(serviceIds[0]));
	    
            PEGASUS_ASSERT(_target != 0);
	 }
      }
      
      virtual void processing(void)
      {
	 set_state(PROCESSING);
      }
      
      virtual void processing(OperationContext & context)
      {
	 processing();
      }
      
      virtual void complete(void)
      {
	 set_state(COMPLETE);
      }
      
      virtual void complete(OperationContext & context)
      {
	 complete();
      }
      
      virtual void deliver(const CIMIndication & cimIndication)
      {
        // ATTN: temporarily convert indication to instance
        CIMInstance cimInstance(cimIndication);

        // create message
        CIMProcessIndicationRequestMessage * request =
	   new CIMProcessIndicationRequestMessage(
	      _request_copy.messageId,
	      cimInstance.getPath().getNameSpace(),
	      cimInstance,
	      QueueIdStack(_target->getQueueId(), _source->getQueueId()));

        // send message
        AsyncLegacyOperationStart * asyncRequest =
            new AsyncLegacyOperationStart(
            _source->get_next_xid(),
            0,
            _target->getQueueId(),
            request,
            _target->getQueueId());

        PEGASUS_ASSERT(asyncRequest != 0);

        _source->SendForget(asyncRequest);

      }

      virtual void deliver(const OperationContext & context,
			   const CIMIndication & cimIndication)
      {
	 deliver(cimIndication);
      }
      
      
      virtual void deliver(const Array<CIMIndication> & instances)
      {
	 for(Uint32 i = 0, n = instances.size(); i < n; i++)
	 {
	    deliver(instances[i]);
	 }
      }
      
      virtual void deliver(const OperationContext & context, 
			   const Array<CIMIndication> & instances)
      {
	 deliver(instances);
      }

      const Array<CIMIndication> & getObjects(void) const
      {
	 return(_objects);
      }
      
      virtual void setStatus(Uint32 code, const String & message = String::EMPTY)
      {
	 _response_copy.cimException = PEGASUS_CIM_EXCEPTION(CIMStatusCode(code), message);
      }
      
      CIMEnableIndicationsRequestMessage *getRequest(void)
      {
	 return new CIMEnableIndicationsRequestMessage(_request_copy);
      }
      
      CIMEnableIndicationsResponseMessage * getResponse(void)
      {
	 return new CIMEnableIndicationsResponseMessage(_response_copy);
      }

protected:
      MessageQueueService * _source;
      MessageQueueService * _target;
      
   private:
      Array<CIMIndication> _objects;
      CIMEnableIndicationsRequestMessage _request_copy;
      CIMEnableIndicationsResponseMessage _response_copy;
};



PEGASUS_NAMESPACE_END

#endif
