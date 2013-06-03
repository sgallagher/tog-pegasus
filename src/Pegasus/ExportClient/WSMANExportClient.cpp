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

#include <Pegasus/Common/Constants.h>
#include <Pegasus/WsmServer/WsmWriter.h>
#include <Pegasus/WsmServer/WsmRequest.h>
#include <Pegasus/WsmServer/CimToWsmResponseMapper.h>
#include <Pegasus/ExportClient/WSMANExportClient.h>

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

WSMANExportClient::WSMANExportClient(
    HTTPConnector* httpConnector,
    Uint32 timeoutMilliseconds)
    :
    ExportClient(PEGASUS_QUEUENAME_WSMANEXPORTCLIENT,
        httpConnector,
        timeoutMilliseconds)
{
    PEG_METHOD_ENTER (TRC_EXPORT_CLIENT, 
        "WSMANExportClient::WSMANExportClient()");
    PEG_METHOD_EXIT();
}

WSMANExportClient::~WSMANExportClient()
{
    PEG_METHOD_ENTER (TRC_EXPORT_CLIENT, 
        "WSMANExportClient::~WSMANExportClient()");
    disconnect();
    PEG_METHOD_EXIT();
}

void WSMANExportClient::exportIndication(
    const String& url,
    const CIMInstance& instanceName,
    const ContentLanguageList& contentLanguages,
    const String& toPath)    
{
    PEG_METHOD_ENTER (TRC_EXPORT_CLIENT, 
        "WSMANExportClient::exportIndication()");
    try
    {
        CimToWsmResponseMapper wsmMapper;
        WsmInstance wsmInstance;
        wsmMapper.convertCimToWsmInstance(url, instanceName, 
            wsmInstance, PEGASUS_INSTANCE_NS);
        WsmRequest * request = new WsExportIndicationRequest(
            WsmUtils::getMessageId(),
            url,
            toPath,
            wsmInstance);

        request->contentLanguages = contentLanguages;
        PEG_TRACE ((TRC_INDICATION_GENERATION, Tracer::LEVEL4,
            "Exporting %s Indication for destination %s:%d%s",
            (const char*)(instanceName.getClassName().getString().
            getCString()),
            (const char*)(_connectHost.getCString()), _connectPortNumber,
            (const char*)(url.getCString())));


        _doRequest(request);
    }
    catch (const Exception& e)
    {
        PEG_TRACE((TRC_DISCARDED_DATA, Tracer::LEVEL1,
            "Failed to export indication: %s",
            (const char*)e.getMessage().getCString()));
        throw;
    }
    catch (...)
    {
        PEG_TRACE((TRC_DISCARDED_DATA, Tracer::LEVEL1,
            "Failed to export indication for class %s",
            (const char*)(instanceName.getClassName().getString().
            getCString())));
        throw;
    }
    PEG_METHOD_EXIT();
}

void WSMANExportClient::_doRequest(
    WsmRequest * request)
    
{
    PEG_METHOD_ENTER (TRC_EXPORT_CLIENT, "WSMANExportClient::_doRequest()");

    AutoPtr<WsmRequest> indicationRequest(request);
    
    if (!_connected )
    {
        PEG_METHOD_EXIT();
        throw NotConnectedException();
    }

    _authenticator.setRequestMessage(0);

    //
    //  Set HTTP method in request to POST
    //
    indicationRequest->httpMethod = HTTP_METHOD__POST;

    //Current WSMAN eventing part supports only PUSH delivery mode.
    //So we dont wait for the response here,we only send the 
    //indication to consumer. 
    if ( _deliveryMode == Push )
    {
        _wsmanRequestEncoder->enqueue(indicationRequest.release());
    }
    else
    {
        PEG_TRACE((TRC_DISCARDED_DATA, Tracer::LEVEL1,
            "Failed to export indication since delivery mode %s"
            " is not supported", (const char*)_deliveryMode));
    }
    PEG_METHOD_EXIT();
}

void WSMANExportClient ::setDeliveryMode(deliveryMode &deliveryMode)
{
    _deliveryMode = deliveryMode;
}
    
PEGASUS_NAMESPACE_END      
