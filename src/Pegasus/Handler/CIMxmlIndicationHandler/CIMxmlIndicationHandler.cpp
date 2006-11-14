//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/ExportClient/CIMExportClient.h>
#include <Pegasus/Handler/CIMHandler.h>
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Config/ConfigManager.h>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/PegasusVersion.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/SSLContext.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/Tracer.h>

PEGASUS_NAMESPACE_BEGIN

PEGASUS_USING_STD;


static Boolean verifyListenerCertificate(SSLCertificateInfo& certInfo)
{
    // ATTN: Add code to handle listener certificate verification.
    //
    return true;
}


class PEGASUS_HANDLER_LINKAGE CIMxmlIndicationHandler: public CIMHandler
{
public:

    CIMxmlIndicationHandler()
    {
        PEG_METHOD_ENTER(TRC_IND_HANDLER,
            "CIMxmlIndicationHandler::CIMxmlIndicationHandler");
        PEG_METHOD_EXIT();
    }

    virtual ~CIMxmlIndicationHandler()
    {
        PEG_METHOD_ENTER(TRC_IND_HANDLER,
            "CIMxmlIndicationHandler::~CIMxmlIndicationHandler");
        PEG_METHOD_EXIT();
    }

    void initialize(CIMRepository* repository)
    {

    }

    void terminate()
    {

    }

    void handleIndication(
    const OperationContext& context,
    const String nameSpace,
    CIMInstance& indicationInstance,
    CIMInstance& indicationHandlerInstance,
    CIMInstance& indicationSubscriptionInstance,
    ContentLanguageList& contentLanguages)
    {
        PEG_METHOD_ENTER(TRC_IND_HANDLER,
            "CIMxmlIndicationHandler::handleIndication()");

        //get destination for the indication
        Uint32 pos = indicationHandlerInstance.findProperty(
                CIMName ("destination"));
        if (pos == PEG_NOT_FOUND)
        {
            String msg = _getMalformedExceptionMsg();

            PEG_TRACE_STRING(TRC_IND_HANDLER, Tracer::LEVEL4, msg);

            PEG_TRACE_STRING(TRC_DISCARDED_DATA, Tracer::LEVEL2,
                "CIMxmlIndicationHandler::handleIndication failed to deliver "
                "indication: Destination property missing");

            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, msg);
        }

        CIMProperty prop = indicationHandlerInstance.getProperty(pos);

        String dest;
        try
        {
            prop.getValue().get(dest);
        }
        catch (TypeMismatchException& e)
        {
            MessageLoaderParms param(
                "Handler.CIMxmlIndicationHandler.CIMxmlIndicationHandler.ERROR",
                "CIMxmlIndicationHandler Error: ");

            String msg = MessageLoader::getMessage(param) + e.getMessage();

            PEG_TRACE_STRING(TRC_IND_HANDLER, Tracer::LEVEL4, msg);

            PEG_TRACE_STRING(TRC_DISCARDED_DATA, Tracer::LEVEL2,
                "CIMxmlIndicationHandler::handleIndication failed to deliver "
                "indication: Destination property type mismatch");

            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, msg);
        }

        try
        {
            static String PROPERTY_NAME__SSLCERT_FILEPATH =
                "sslCertificateFilePath";
            static String PROPERTY_NAME__SSLKEY_FILEPATH  = "sslKeyFilePath";

            //
            // Get the sslCertificateFilePath property from the Config Manager.
            //
            ConfigManager* configManager = ConfigManager::getInstance();

            String certPath;
            certPath = ConfigManager::getHomedPath(
                configManager->getCurrentValue(
                    PROPERTY_NAME__SSLCERT_FILEPATH));

            //
            // Get the sslKeyFilePath property from the Config Manager.
            //
            String keyPath;
            keyPath = ConfigManager::getHomedPath(
                configManager->getCurrentValue(
                    PROPERTY_NAME__SSLKEY_FILEPATH));

            String trustPath = String::EMPTY;

            String randFile = String::EMPTY;

#ifdef PEGASUS_SSL_RANDOMFILE
            randFile =
                ConfigManager::getHomedPath(PEGASUS_SSLSERVER_RANDOMFILE);
#endif

            Monitor monitor;
            HTTPConnector httpConnector(&monitor);

            CIMExportClient exportclient(&monitor, &httpConnector);
            Uint32 colon = dest.find (":");
            Uint32 portNumber = 0;
            Boolean useHttps = false;
            String destStr = dest;
            String hostStr;

            //
            // If the URL has https (https://hostname:port/... or
            // https://hostname/...) then use SSL for Indication delivery.
            // If it has http (http://hostname:port/...
            // or http://hostname/...) then do not use SSL.
            //
            if (colon != PEG_NOT_FOUND)
            {
                String httpStr = dest.subString(0, colon);
                if (String::equalNoCase(httpStr, "https"))
                {
                    useHttps = true;
                }
                else if (String::equalNoCase(httpStr, "http"))
                {
                    useHttps = false;
                }
                else
                {
                    String msg = _getMalformedExceptionMsg();

                    PEG_TRACE_STRING(TRC_IND_HANDLER, Tracer::LEVEL4, msg+dest);
                    PEG_TRACE_STRING(TRC_DISCARDED_DATA, Tracer::LEVEL2,
                        "CIMxmlIndicationHandler::handleIndication failed to "
                        "deliver indication: "
                        "missing http or https "
                        "in Destination " + dest);

                    PEG_METHOD_EXIT();
                    throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED,
                        msg + dest);
                }
            }
            else
            {
                String msg = _getMalformedExceptionMsg();

                PEG_TRACE_STRING(TRC_IND_HANDLER, Tracer::LEVEL4, msg + dest);

                PEG_TRACE_STRING(TRC_DISCARDED_DATA, Tracer::LEVEL2,
                    "CIMxmlIndicationHandler::handleIndication failed to "
                    "deliver indication: "
                    "missing colon "
                    "in Destination " + dest);

                PEG_METHOD_EXIT();
                throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, msg + dest);
            }

            String doubleSlash = dest.subString(colon + 1, 2);

            if (String::equalNoCase(doubleSlash, "//"))
            {
                destStr = dest.subString(colon + 3, PEG_NOT_FOUND);
            }
            else
            {
                String msg = _getMalformedExceptionMsg();

                PEG_TRACE_STRING(TRC_IND_HANDLER, Tracer::LEVEL4, msg + dest);

                PEG_TRACE_STRING(TRC_DISCARDED_DATA, Tracer::LEVEL2,
                    "CIMxmlIndicationHandler::handleIndication failed to "
                    "deliver indication: "
                    "missing double slash "
                    "in Destination " + dest);

                PEG_METHOD_EXIT();
                throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, msg + dest);
            }

            bool parseError = false;
            colon = destStr.find (":");
            //
            // get hostname and port number from destination string
            //
            if (colon != PEG_NOT_FOUND)
            {
                hostStr = destStr.subString (0, colon);
                destStr = destStr.subString(colon + 1, PEG_NOT_FOUND);
                String portStr = destStr.subString (0, destStr.find ("/"));

                char dummy;
                int noOfConversions = sscanf(portStr.getCString (), "%u%c",
                    &portNumber, &dummy);
                parseError = (noOfConversions != 1);
            }
            //
            // There is no port number in the destination string,
            // get port number from system
            //
            else
            {
                hostStr = destStr.subString(0, destStr.find ("/"));
                if (useHttps)
                {
                     portNumber = System::lookupPort(WBEM_HTTPS_SERVICE_NAME,
                        WBEM_DEFAULT_HTTPS_PORT);
                }
                else
                {
                    portNumber = System::lookupPort(WBEM_HTTP_SERVICE_NAME,
                        WBEM_DEFAULT_HTTP_PORT);
                }
            }

            char hostName[PEGASUS_MAXHOSTNAMELEN];
            if (!parseError)
            {
                char dummy;
                int noOfConversions = sscanf(hostStr.getCString (), "%s%c",
                    hostName, &dummy);
                parseError = (noOfConversions != 1);
            }

            if (parseError)
            {
                String msg = _getMalformedExceptionMsg();

                PEG_TRACE_STRING(TRC_IND_HANDLER, Tracer::LEVEL4, msg + dest);

                PEG_TRACE_STRING(TRC_DISCARDED_DATA, Tracer::LEVEL2,
                    "CIMxmlIndicationHandler::handleIndication failed to "
                    "deliver indication: "
                    "invalid host name or port number "
                    "in Destination " + dest);

                PEG_METHOD_EXIT();
                throw PEGASUS_CIM_EXCEPTION(CIM_ERR_NOT_SUPPORTED, msg + dest);
            }

#ifndef PEGASUS_OS_ZOS

            if (useHttps)
            {
#ifdef PEGASUS_HAS_SSL
                PEG_TRACE_STRING(TRC_IND_HANDLER, Tracer::LEVEL4,
                    "Build SSL Context...");

                SSLContext sslcontext(trustPath,
                    certPath, keyPath, verifyListenerCertificate, randFile);
                exportclient.connect (hostName, portNumber, sslcontext);
#else
                MessageLoaderParms param(
                    "Handler.CIMxmlIndicationHandler."
                        "CIMxmlIndicationHandler.ERROR",
                    "CIMxmlIndicationHandler Error: ");
                MessageLoaderParms param1(
                    "Handler.CIMxmlIndicationHandler."
                        "CIMxmlIndicationHandler.CANNOT_DO_HTTPS_CONNECTION",
                    "Cannot do https connection.");

                PEG_TRACE_STRING(TRC_IND_HANDLER, Tracer::LEVEL3,
                     MessageLoader::getMessage(param) +
                         MessageLoader::getMessage(param1));

                String msg = MessageLoader::getMessage(param) +
                    MessageLoader::getMessage(param1);

                PEG_TRACE_STRING(TRC_DISCARDED_DATA, Tracer::LEVEL2,
                    "CIMxmlIndicationHandler::handleIndication failed to "
                    "deliver indication: "
                    "https not supported "
                    "in Destination " + dest);

                PEG_METHOD_EXIT();
                throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, msg);
#endif
            }
            else
            {
                exportclient.connect (hostName, portNumber);
            }
#else
            // On zOS the ATTLS facility is using the port number(s) defined
            // of the outbound policy to decide if the indication is
            // delivered through a SSL secured socket. This is totally
            // transparent to the CIM Server.
            exportclient.connect (hostName, portNumber);

#endif
            // check destStr, if no path is specified, use "/" for the URI
            Uint32 slash = destStr.find ("/");
            if (slash != PEG_NOT_FOUND)
            {
                exportclient.exportIndication(
                    destStr.subString(slash), indicationInstance,
                    contentLanguages);
            }
            else
            {
                exportclient.exportIndication(
                    "/", indicationInstance, contentLanguages);
            }
            exportclient.disconnect();
        }
        catch(Exception& e)
        {
            //ATTN: Catch specific exceptions and log the error message
            // as Indication delivery failed.
            MessageLoaderParms param(
                "Handler.CIMxmlIndicationHandler.CIMxmlIndicationHandler.ERROR",
                "CIMxmlIndicationHandler Error: ");

            String msg = MessageLoader::getMessage(param) + e.getMessage();

            PEG_TRACE_STRING(TRC_DISCARDED_DATA, Tracer::LEVEL2,
                "CIMxmlIndicationHandler::handleIndication failed to deliver "
                "indication due to Exception: " + e.getMessage ());

            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION(CIM_ERR_FAILED, msg);
        }

        PEG_METHOD_EXIT();
    }

private:
    String _getMalformedExceptionMsg()
    {
        MessageLoaderParms param(
            "Handler.CIMxmlIndicationHandler.CIMxmlIndicationHandler.ERROR",
            "CIMxmlIndicationHandler Error: ");

        MessageLoaderParms param1(
            "Handler.CIMxmlIndicationHandler."
                "CIMxmlIndicationHandler.MALFORMED_HANDLER_INSTANCE",
            "Malformed handler instance.");

        return MessageLoader::getMessage(param) +
            MessageLoader::getMessage(param1);
    }

};

PEGASUS_NAMESPACE_END

PEGASUS_USING_PEGASUS;

// This is the entry point into this dynamic module.

extern "C" PEGASUS_EXPORT CIMHandler* PegasusCreateHandler(
    const String& handlerName)
{
    if (handlerName == "CIMxmlIndicationHandler")
    {
        return new CIMxmlIndicationHandler;
    }

    return 0;
}
