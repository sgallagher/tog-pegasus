//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett Packard, IBM, The Open Group,
// Tivoli Systems
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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/PegasusVersion.h>

#include <iostream>
#include <Pegasus/Handler/CIMHandler.h>

#ifdef PEGASUS_OS_HPUX
# include <unistd.h>
#endif

PEGASUS_NAMESPACE_BEGIN

PEGASUS_USING_STD;

#define _MAX_COMMAND_SIZE 1000
#define _MAIL_FILE_NAME "mail_file"
 
//#define DDD(X) X
#define DDD(X) // X

DDD(static const char* _SENDMAILINDICATIONHANDLER = "sendmailIndicationHandler::";)

class PEGASUS_HANDLER_LINKAGE sendmailIndicationHandler: public CIMHandler
{
public:

    sendmailIndicationHandler()
    {
        DDD(cout << _SENDMAILINDICATIONHANDLER << "sendmailIndicationHandler()" << endl;)
    }

    virtual ~sendmailIndicationHandler()
    {
        DDD(cout << _SENDMAILINDICATIONHANDLER << "~sendmailIndicationHandler()" << endl;)
    }

    void initialize(CIMRepository* repository)
    {
        DDD(cout << _SENDMAILINDICATIONHANDLER << "initialize()" << endl;)
    }

    void terminate()
    {
        DDD(cout << _SENDMAILINDICATIONHANDLER << "terminate()" << endl;)
    }

    void handleIndication(
	CIMInstance& indicationHandlerInstance, 
	CIMInstance& indicationInstance, 
	String nameSpace);
};

void sendmailIndicationHandler::handleIndication(
    CIMInstance& indicationHandlerInstance,
    CIMInstance& indicationInstance,
    String nameSpace)
{
#ifdef PEGASUS_OS_HPUX
    //get destination for the indication
    Uint32 pos = indicationHandlerInstance.findProperty("destination");
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
    catch (TypeMismatch& e)
    {
        // ATTN: Deal with a malformed handler instance
    }

    FILE* sendmailFile;
    sendmailFile = fopen(_MAIL_FILE_NAME, "a");
    if (!sendmailFile)
        throw CIMException(CIM_ERR_FAILED); 

    Uint32 n = indicationInstance.getPropertyCount();
    for (Uint32 i = 0; i < n; i++)
    {
        CIMConstProperty instanceProp = indicationInstance.getProperty(i);
        fprintf(sendmailFile,
                "%s\t%s\n",
                (const char *)_CString(instanceProp.getName()),
                (const char *)_CString(instanceProp.getValue().toString()));
    }
    fclose(sendmailFile);

    FILE* sendmailptr;
    char  sendcmd[_MAX_COMMAND_SIZE];

    sprintf(sendcmd,
            "%s %s %s %s",
            "/usr/sbin/sendmail",
            (const char *)_CString(dest),
            "<",
            _MAIL_FILE_NAME);

    if ((sendmailptr = popen(sendcmd, "r")) == NULL)
        throw CIMException(CIM_ERR_FAILED);
    else
        pclose(sendmailptr);
    unlink(_MAIL_FILE_NAME);
#else
    throw CIMException(CIM_ERR_NOT_SUPPORTED);
#endif
}

// This is the dynamic entry point into this dynamic module. The name of
// this handler is "CIMxmlIndicationHandler" which is appened to "PegasusCreateHandler_"
// to form a symbol name. This function is called by the HandlerTable
// to load this handler.

extern "C" PEGASUS_EXPORT CIMHandler* 
    PegasusCreateHandler_sendmailIndicationHandler() 
{
    return new sendmailIndicationHandler;
}

PEGASUS_NAMESPACE_END
