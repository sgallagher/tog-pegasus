//%/////////////////////////////////////////////////////////////////////////////
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
// Author: Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//              Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/PegasusVersion.h>

#include <iostream>
#include <Pegasus/ExportClient/CIMExportClient.h>
#include <Pegasus/Handler/CIMHandler.h>
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Common/SSLContext.h>

PEGASUS_NAMESPACE_BEGIN

PEGASUS_USING_STD;

//#define DDD(X) X
#define DDD(X) // X

DDD(static const char* _CIMXMLINDICATIONHANDLER = "CIMxmlIndicationHandler::";)

static Boolean verifyListenerCertificate(SSLCertificateInfo &certInfo)
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
        DDD(cout << _CIMXMLINDICATIONHANDLER << "CIMxmlIndicationHandler()" << endl;)
    }

    virtual ~CIMxmlIndicationHandler()
    {
        DDD(cout << _CIMXMLINDICATIONHANDLER << "~CIMxmlIndicationHandler()" << endl;)
    }

    void initialize(CIMRepository* repository)
    {
        DDD(cout << _CIMXMLINDICATIONHANDLER << "initialize()" << endl;)
    }

    void terminate()
    {
        DDD(cout << _CIMXMLINDICATIONHANDLER << "terminate()" << endl;)
    }

// l10n
    void handleIndication(
	CIMInstance& indicationHandlerInstance, 
	CIMInstance& indicationInstance, 
	String nameSpace,
	ContentLanguages& contentLanguages)
    {
	//get destination for the indication
	Uint32 pos = indicationHandlerInstance.findProperty 
            (CIMName ("destination"));
        if (pos == PEG_NOT_FOUND)
        {
            // ATTN: Deal with a malformed handler instance
        }

	CIMProperty prop = indicationHandlerInstance.getProperty(pos);

        String dest;
        try
        {
            prop.getValue().get(dest);
        }
        catch (TypeMismatchException& e)
        {
            // ATTN: Deal with a malformed handler instance
        }
	
	try
        {
            static String PROPERTY_NAME__SSLCERT_FILEPATH = "sslCertificateFilePath";
            static String PROPERTY_NAME__SSLKEY_FILEPATH  = "sslKeyFilePath";
            static String PROPERTY_NAME__SSLTRUST_FILEPATH  = "sslTrustFilePath";

            //
            // Get the sslCertificateFilePath property from the Config Manager.
            //
            String certPath;
            certPath = ConfigManager::getInstance()->getCurrentValue(
                               PROPERTY_NAME__SSLCERT_FILEPATH);

            //
            // Get the sslKeyFilePath property from the Config Manager.
            //
            String keyPath;
            keyPath = ConfigManager::getInstance()->getCurrentValue(
                               PROPERTY_NAME__SSLKEY_FILEPATH);

            //
            // Get the sslKeyFilePath property from the Config Manager.
            //
            String trustPath = String::EMPTY;
            trustPath = ConfigManager::getInstance()->getCurrentValue(
                               PROPERTY_NAME__SSLTRUST_FILEPATH);

            String randFile = String::EMPTY;

#ifdef PEGASUS_SSL_RANDOMFILE
            // NOTE: It is technically not necessary to set up a random file on
            // the server side, but it is easier to use a consistent interface
            // on the client and server than to optimize out the random file on
            // the server side.
            randFile = ConfigManager::getHomedPath(PEGASUS_SSLSERVER_RANDOMFILE);
#endif

            SSLContext sslcontext(trustPath, certPath, keyPath, verifyListenerCertificate, randFile);

	    Monitor* monitor = new Monitor;
	    HTTPConnector* httpConnector = new HTTPConnector(monitor);
	    CIMExportClient exportclient(monitor, httpConnector);
            Uint32 colon = dest.find (":");
            Uint32 doubleSlash = dest.find ("//");
            Uint32 portNumber = 0;
            Boolean useHttps = false;
            String destStr = dest;

            //
            // If the URL has https (https://hostname:port/...) then use SSL 
            // for Indication delivery. If it has http (http://hostname:port/...)
            // or none (hostname:port/...) then do not use SSL.
            //
            if ((colon != PEG_NOT_FOUND) && (doubleSlash != PEG_NOT_FOUND))
            {
                String httpStr = dest.subString(0, colon); 
                if (String::equalNoCase(httpStr, "https"))
                {
                    useHttps = true;
                }
                destStr = dest.subString(doubleSlash + 2, PEG_NOT_FOUND);
            }

            colon = destStr.find (":");
            Uint32 slash = destStr.find ("/");

            if ((colon != PEG_NOT_FOUND) && (slash != PEG_NOT_FOUND))
            {
                String portStr = destStr.subString (colon + 1, slash);
                sscanf (portStr.getCString (), "%u", &portNumber);
            }

            if (useHttps)
            {
#ifdef PEGASUS_HAS_SSL
                exportclient.connect (destStr.subString (0, colon), 
                    portNumber, sslcontext);
#else
                PEGASUS_STD(cerr) << "CIMxmlIndicationHandler Error: "
                    << "Cannot do https connection." << PEGASUS_STD(endl);
#endif
            }
            else
            {
                exportclient.connect (destStr.subString (0, colon), portNumber);
            }

// l10n
	    exportclient.exportIndication(
                destStr.subString(destStr.find("/")), indicationInstance,
                contentLanguages);
	}
	catch(Exception& e)
        {
            //ATTN: Catch specific exceptions and log the error message 
            // as Indication delivery failed.

            PEGASUS_STD(cerr) << "CIMxmlIndicationHandler Error: " << e.getMessage() 
	    << PEGASUS_STD(endl);
        }
    }
};

// This is the dynamic entry point into this dynamic module. The name of
// this handler is "CIMxmlIndicationHandler" which is appened to "PegasusCreateHandler_"
// to form a symbol name. This function is called by the HandlerTable
// to load this handler.

extern "C" PEGASUS_EXPORT CIMHandler* 
    PegasusCreateHandler_CIMxmlIndicationHandler() {
    DDD(cout << "Called PegasusCreateHandler_CIMxmlIndicationHandler" << endl;)
    return new CIMxmlIndicationHandler;
}

PEGASUS_NAMESPACE_END
