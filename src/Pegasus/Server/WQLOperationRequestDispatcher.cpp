//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// CIMOperationRequestDispatcher
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
// Author: Adrain Schuur (schuur@de.ibm.com)
//
// Modified By:
//     Amit K Arora, IBM (amita@in.ibm.com) for PEP#101
//	   Seema Gupta (gseema@in.ibm.com) for PEP135
//
//%/////////////////////////////////////////////////////////////////////////////

#include "WQLOperationRequestDispatcher.h"
#include <Pegasus/Common/AutoPtr.h>

PEGASUS_NAMESPACE_BEGIN

PEGASUS_USING_STD;


void WQLOperationRequestDispatcher::applyQueryToEnumeration(
           CIMResponseMessage* msg,
           QueryExpressionRep* query)
{
   CIMEnumerateInstancesResponseMessage *enr=(CIMEnumerateInstancesResponseMessage*)msg;
   WQLSelectStatement *qs=((WQLQueryExpressionRep*)query)->_stmt;

   for (int i=enr->cimNamedInstances.size()-1; i>=0; i--) {
      WQLInstancePropertySource ips(enr->cimNamedInstances[i]);
      try {
         if (qs->evaluateWhereClause(&ips)) {
	    qs->applyProjection(enr->cimNamedInstances[i]);
         }
         else enr->cimNamedInstances.remove(i);
      }
      catch (...) {
         enr->cimNamedInstances.remove(i);
      }
   }
}


void WQLOperationRequestDispatcher::handleQueryResponseAggregation(
                           OperationAggregate* poA)
{
    PEG_METHOD_ENTER(TRC_DISPATCHER,
        "CIMOperationRequestDispatcher::handleExecQueryResponse");

    CIMExecQueryResponseMessage * toResponse =
	(CIMExecQueryResponseMessage *) poA->getResponse(0);

    Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
        "CIMOperationRequestDispatcher::ExecQuery Response - Name Space: $0  Class name: $1 Response Count: poA->numberResponses",
        poA->_nameSpace.getString(),
        poA->_className.getString(),
        poA->numberResponses());

    if (poA->getResponse(0)->getType()==CIM_ENUMERATE_INSTANCES_RESPONSE_MESSAGE) {
       applyQueryToEnumeration(poA->getResponse(0),poA->_query);
    }

    // Work backward and delete each response off the end of the array
    for(Uint32 i = poA->numberResponses() - 1; i > 0; i--) {
        if (poA->getResponse(i)->getType()==CIM_ENUMERATE_INSTANCES_RESPONSE_MESSAGE) {
           applyQueryToEnumeration(poA->getResponse(i),poA->_query);
   	   CIMEnumerateInstancesResponseMessage *fromResponse =
    	       (CIMEnumerateInstancesResponseMessage *)poA->getResponse(i);
           CIMClass cimClass;
	   Boolean clsRead=false;
    	   for (Uint32 j = 0, m = fromResponse->cimNamedInstances.size(); j<m; j++) {
	      CIMObject co=CIMObject(fromResponse->cimNamedInstances[j]);
	      CIMObjectPath op=co.getPath();
    	      const Array<CIMKeyBinding>& kbs=op.getKeyBindings();
	      if (kbs.size()==0) {     // no path set why ?
		 if (clsRead==false) {
		    cimClass = _repository->getClass(
    	              poA->_nameSpace,op.getClassName(),
    	              false,true,false, CIMPropertyList());
		    clsRead=true;
		 }
		 op=fromResponse->cimNamedInstances[j].buildPath(cimClass);     
	      }
	      op.setNameSpace(poA->_nameSpace);
	      op.setHost(System::getHostName());
	      co.setPath(op);
    	      toResponse->cimObjects.append(co);
	   }
	}
	else {
   	   CIMExecQueryResponseMessage *fromResponse =
    	       (CIMExecQueryResponseMessage *)poA->getResponse(i);
           CIMClass cimClass;
	   Boolean clsRead=false;
    	   for (Uint32 j = 0, m = fromResponse->cimObjects.size(); j<m; j++) {
	      CIMObject co=fromResponse->cimObjects[j];
	      CIMObjectPath op=co.getPath();
    	      const Array<CIMKeyBinding>& kbs=op.getKeyBindings();
	      if (kbs.size()==0) {     // no path set why ?
		 if (clsRead==false) {
		    cimClass = _repository->getClass(
    	              poA->_nameSpace,op.getClassName(),
    	              false,true,false, CIMPropertyList());
		    clsRead=true;
		 }
		 op=CIMInstance(fromResponse->cimObjects[j]).buildPath(cimClass);     
	      }
	      op.setNameSpace(poA->_nameSpace);
	      op.setHost(System::getHostName());
	      co.setPath(op);
    	      toResponse->cimObjects.append(co);
    	   }
	}
    	poA->deleteResponse(i);
    }
    delete poA->_query;
    PEG_METHOD_EXIT();
}



void WQLOperationRequestDispatcher::handleQueryRequest(
   CIMExecQueryRequestMessage* request)
{
   Boolean exception=false;
   AutoPtr<WQLSelectStatement> selectStatement(new WQLSelectStatement());
   AutoPtr<WQLQueryExpressionRep> qx;
   CIMException cimException;
   CIMName className;

   //if (getenv("CMPI_DEBUG")) asm("int $3");
   PEG_METHOD_ENTER(TRC_DISPATCHER,
      "CIMOperationRequestDispatcher::handleExecQueryRequest");

    if (request->queryLanguage!="WQL") {
      cimException =
         PEGASUS_CIM_EXCEPTION(CIM_ERR_QUERY_LANGUAGE_NOT_SUPPORTED, request->queryLanguage);
      exception=true;
   }
   else {
      try {
         WQLParser::parse(request->query, *selectStatement.get());
	 className=selectStatement->getClassName();
	 qx.reset(new WQLQueryExpressionRep("WQL",selectStatement.get()));
	 selectStatement.release();
      }
      catch (ParseError& e) {
         cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_INVALID_QUERY, request->query);
         exception=true;
      }
      catch (MissingNullTerminator& e) {
         cimException =
            PEGASUS_CIM_EXCEPTION(CIM_ERR_INVALID_QUERY, request->query);
         exception=true;
      }

      if (exception==false) {
        _checkExistenceOfClass(request->nameSpace,
                           className,
                           cimException);
        if (cimException.getCode() != CIM_ERR_SUCCESS)
	   exception=true;
      }
   }

   if (exception) {
   Array<CIMObject> cimObjects;

   AutoPtr<CIMExecQueryResponseMessage> response(
      new CIMExecQueryResponseMessage(
	 request->messageId,
	 cimException,
	 request->queueIds.copyAndPop(),
	 cimObjects));

        STAT_COPYDISPATCHER_REP
        _enqueueResponse(request, response.release());
        PEG_METHOD_EXIT();
        return;
    }

    //
    // Get names of descendent classes:
    //
    Array<ProviderInfo> providerInfos;

    // This gets set by _lookupAllInstanceProviders()
    Uint32 providerCount;

    try {
        providerInfos = _lookupAllInstanceProviders(request->nameSpace,
                                                    className,
				  //                request->className,
                                                    providerCount,
						    true);
    }
    catch(CIMException& exception) {
        // Return exception response if exception from getSubClasses
        cimException = exception;
        AutoPtr<CIMExecQueryResponseMessage> response(
        new CIMExecQueryResponseMessage(request->messageId,
            cimException,
            request->queueIds.copyAndPop(),
            Array<CIMObject>()));

		STAT_COPYDISPATCHER_REP

        _enqueueResponse(request, response.release());
        PEG_METHOD_EXIT();
        return;
    }

    // Test for "enumerate too Broad" and if so, execute exception.
    // This limits the number of provider invocations, not the number
    // of instances returned.
    if(providerCount > _maximumEnumerateBreadth)
    {
        Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
                    "Request-too-broad exception.  Namespace: $0  "
                        "Class Name: $1 Limit: $2  ProviderCount: $3",
                    request->nameSpace.getString(),
                    request->className.getString(),
                    _maximumEnumerateBreadth, providerCount);

        PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
            Formatter::format("ERROR Enumerate too broad for class $0. "
                                  "Limit = $1, Request = $2",
                              request->className.getString(),
                              _maximumEnumerateBreadth,
                              providerCount));

        // l10n

        AutoPtr<CIMExecQueryResponseMessage> response(
            new CIMExecQueryResponseMessage(request->messageId,
                PEGASUS_CIM_EXCEPTION_L(CIM_ERR_NOT_SUPPORTED,
                    MessageLoaderParms("Server.CIMOperationRequestDispatcher."
                        "QUERY_REQ_TOO_BROAD", "Query request too Broad")),
                request->queueIds.copyAndPop(),
                Array<CIMObject>()));

   STAT_COPYDISPATCHER

   _enqueueResponse(request, response.release());
        PEG_METHOD_EXIT();
        return;
    }

    // If no provider is registered and the repository isn't the default,
    // return CIM_ERR_NOT_SUPPORTED
    if ((providerCount == 0) && !(_repository->isDefaultInstanceProvider()))
    {
        PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
            "CIM_ERROR_NOT_SUPPORTED for " + request->className.getString());

        AutoPtr<CIMExecQueryResponseMessage> response(
            new CIMExecQueryResponseMessage(request->messageId,
                PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, String::EMPTY),
                request->queueIds.copyAndPop(),
                Array<CIMObject>()));

   STAT_COPYDISPATCHER

   _enqueueResponse(request, response.release());
   PEG_METHOD_EXIT();
        return;
    }

    // We have instances for Providers and possibly repository.
    // Set up an aggregate object and save a copy of the original request.

    OperationAggregate *poA= new OperationAggregate(
        new CIMExecQueryRequestMessage(*request),
        request->getType(),
        request->messageId,
        request->queueIds.top(),
        className,
	qx.release(),
	"WQL");

    // Set the number of expected responses in the OperationAggregate
    Uint32 numClasses = providerInfos.size();
    poA->_aggregationSN = cimOperationAggregationSN++;
    poA->_nameSpace=request->nameSpace;

    // insert dummy CIMExecQueryResponseMessage entry
    // this ensures a CIMExecQueryResponse to be generated
    CIMExecQueryResponseMessage *qResp=new CIMExecQueryResponseMessage(
           request->messageId,
	   CIMException(CIM_ERR_FAILED,String::EMPTY), // will be removed later in
	                                       // handleOperationResponseAggregation
	   request->queueIds.copyAndPop(),
	    Array<CIMObject>()); 

    if (_repository->isDefaultInstanceProvider())
        poA->setTotalIssued(numClasses+1);
    else poA->setTotalIssued(providerCount+1);
    poA->appendResponse(qResp);

    // Loop through providerInfos, forwarding requests to providers
    for (Uint32 i = 0; i < numClasses; i++)
    {
        // If this class has a provider
        if (providerInfos[i].hasProvider)
        {
            STAT_PROVIDERSTART

            PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
                Formatter::format(
                    "Query Req. class $0 to svc \"$1\" for "
                        "control provider \"$2\", No $3 of $4, SN $5",
                    providerInfos[i].className.getString(),
                    providerInfos[i].serviceName,
                    providerInfos[i].controlProviderName,
                    i, numClasses, poA->_aggregationSN));

            if (providerInfos[i].hasNoQuery) {
	       // if (getenv("CMPI_DEBUG")) asm("int $3");
	       CIMEnumerateInstancesRequestMessage *enumReq=
	          new CIMEnumerateInstancesRequestMessage(
		     request->messageId, request->nameSpace,
		     providerInfos[i].className,
		     false,false,false,false,CIMPropertyList(),
		     request->queueIds,request->authType,
		     ((IdentityContainer)request->operationContext.get(IdentityContainer::NAME)).getUserName()); 

                _forwardRequestForAggregation(providerInfos[i].serviceName,
                      providerInfos[i].controlProviderName, enumReq, poA);
	    }

	    else {
	       // if (getenv("CMPI_DEBUG")) asm("int $3");
               AutoPtr<CIMExecQueryRequestMessage> requestCopy(
                   new CIMExecQueryRequestMessage(*request));

               requestCopy->className = providerInfos[i].className;

               _forwardRequestForAggregation(providerInfos[i].serviceName,
                   providerInfos[i].controlProviderName, requestCopy.release(), poA);
	    }

            STAT_PROVIDEREND
        }
    }
    // if (getenv("CMPI_DEBUG")) asm("int $3");
    if (_repository->isDefaultInstanceProvider())
    {
        // Loop through providerInfos, forwarding requests to repository
        for (Uint32 i = 0; i < numClasses; i++)
        {
            // If this class does not have a provider
            if (!providerInfos[i].hasProvider)
            {
                PEG_TRACE_STRING(TRC_DISPATCHER, Tracer::LEVEL4,
                    Formatter::format(
                        "ExcecQuery Req. class $0 to repository, "
                            "No $1 of $2, SN $3",
                        providerInfos[i].className.getString(),
                        i, numClasses, poA->_aggregationSN));

                CIMException cimException;
                Array<CIMInstance> cimInstances;

                STAT_PROVIDERSTART

                try
                {
                    // Enumerate instances only for this class
                    cimInstances =
                        _repository->enumerateInstancesForClass(
                            request->nameSpace,
                            providerInfos[i].className,
                            false);
                }
                catch(CIMException& exception)
                {
                    cimException = exception;
                }
                catch(Exception& exception)
                {
                    cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
                        exception.getMessage());
                }
                catch(...)
                {
                    cimException = PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED,
                        String::EMPTY);
                }

                STAT_PROVIDEREND

                AutoPtr<CIMEnumerateInstancesResponseMessage> response(
                    new CIMEnumerateInstancesResponseMessage(
                        request->messageId,
                        cimException,
                        request->queueIds.copyAndPop(),
                        cimInstances));

                STAT_COPYDISPATCHER_REP
                Boolean isDoneAggregation = poA->appendResponse(response.release());
                if (isDoneAggregation)
                {
                    handleOperationResponseAggregation(poA);
                }
            }
        }
    }
 //   abort();
   PEG_METHOD_EXIT();
    return;
}


PEGASUS_NAMESPACE_END

