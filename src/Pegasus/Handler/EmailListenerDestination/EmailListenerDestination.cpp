//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
//
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
// Author: Yi Zhou, Hewlett-Packard Company (yi.zhou@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/PegasusVersion.h>

#include <iostream>
#include <Pegasus/Handler/CIMHandler.h>
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/MessageLoader.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/CIMType.h>
#include <Pegasus/Common/IndicationFormatter.h>
#include <Pegasus/IndicationService/IndicationConstants.h>

#if !defined(PEGASUS_OS_HPUX) && !defined(PEGASUS_OS_LINUX)
# error "Unsupported Platform" 
#endif

#include "EmailListenerDestination.h"

PEGASUS_NAMESPACE_BEGIN

PEGASUS_USING_STD;

void EmailListenerDestination::initialize(CIMRepository* repository)
{
}

void EmailListenerDestination::handleIndication(
    const OperationContext& context,
    const String nameSpace,
    CIMInstance& indication, 
    CIMInstance& handler, 
    CIMInstance& subscription, 
    ContentLanguages & contentLanguages)
{
    PEG_METHOD_ENTER (TRC_IND_HANDLER, 
        "EmailListenerDestination::handleIndication");

    String indicationText;

    try
    {
        // gets formatted indication message
	indicationText = IndicationFormatter::getFormattedIndText(
	    subscription, indication, contentLanguages);

	// get MailTo from handler instance
	Array<String> mailTo;
	handler.getProperty(handler.findProperty(
	    PEGASUS_PROPERTYNAME_LSTNRDST_MAILTO)).getValue().get(mailTo);    

	// get MailSubject from handler instance
	String mailSubject = String::EMPTY;
	handler.getProperty(handler.findProperty(
	    PEGASUS_PROPERTYNAME_LSTNRDST_MAILSUBJECT)).getValue().get(
	    mailSubject);

	// get MailCc from handler instance
	CIMValue mailCcValue;
	Array<String> mailCc;

        Uint32 posMailCc = handler.findProperty(
	    PEGASUS_PROPERTYNAME_LSTNRDST_MAILCC);  

	if (posMailCc != PEG_NOT_FOUND)
	{
	    mailCcValue = handler.getProperty(posMailCc).getValue();
	}

	if (!mailCcValue.isNull())
	{
	    if ((mailCcValue.getType() == CIMTYPE_STRING) &&
		(mailCcValue.isArray()))
            {
		mailCcValue.get(mailCc);
            }
	}

	// Sends the formatted indication to the specified recipients 
        _sendViaEmail(mailTo, mailCc, mailSubject, indicationText);

    }
    catch (CIMException & c)
    {
        PEG_TRACE_STRING(TRC_IND_HANDLER, Tracer::LEVEL4, c.getMessage());
        PEG_METHOD_EXIT();

        throw PEGASUS_CIM_EXCEPTION (CIM_ERR_FAILED, c.getMessage());
    }
    catch (Exception& e)
    {
        PEG_TRACE_STRING(TRC_IND_HANDLER, Tracer::LEVEL4, e.getMessage());
        PEG_METHOD_EXIT();

        throw PEGASUS_CIM_EXCEPTION (CIM_ERR_FAILED, e.getMessage());
    }
    catch (...)
    {
        PEG_TRACE_STRING(TRC_IND_HANDLER, Tracer::LEVEL4,
            "Failed to deliver indication via e-mail.");
        PEG_METHOD_EXIT();
   
        throw PEGASUS_CIM_EXCEPTION_L (CIM_ERR_FAILED,
            MessageLoaderParms("Handler.EmailListenerDestination."
	    "EmailListenerDestination.FAILED_TO_DELIVER_INDICATION_VIA_EMAIL",
            "Failed to deliver indication via e-mail."));
    }

    PEG_METHOD_EXIT();
}

void EmailListenerDestination::_sendViaEmail(
    const Array<String> & mailTo,
    const Array<String> & mailCc,
    const String & mailSubject,
    const String & formattedText)
{
    PEG_METHOD_ENTER (TRC_IND_HANDLER,
	"EmailListenerDestination::_sendViaEmail");

#if defined(PEGASUS_OS_HPUX) || defined(PEGASUS_OS_LINUX)

    String exceptionStr;
    FILE * mailFilePtr;
    FILE * filePtr;
    char mailFile[TEMP_NAME_LEN];

    // Check for proper execute permissions for sendmail
    if (access(SENDMAIL_CMD, X_OK) < 0)
    {
        Tracer::trace(TRC_IND_HANDLER, Tracer::LEVEL4,
            "Cannot execute %s: %s." SENDMAIL_CMD,
	    strerror(errno));

	MessageLoaderParms parms(
	    "Handler.EmailListenerDestination.EmailListenerDestination._MSG_EXECUTE_ACCESS_FAILED",
            "Cannot execute $0: $1",
	    SENDMAIL_CMD,
	    strerror(errno));
	
	 Logger::put_l (Logger::STANDARD_LOG, System::CIMSERVER, 
	     Logger::WARNING, 
	     "Handler.EmailListenerDestination.EmailListenerDestination._MSG_EXECUTE_ACCESS_FAILED",
	     MessageLoader::getMessage(parms));

        PEG_METHOD_EXIT();

	return;
    }

    // open a temporary file to hold the indication mail message
    _openFile(&filePtr, mailFile);

    try
    {
	_buildMailHeader(mailTo, mailCc, mailSubject, filePtr);

	// write indication text to the file
	_writeStrToFile(formattedText, filePtr);

	fclose(filePtr);

    }
    catch (CIMException &c)
    {
	fclose(filePtr);
	unlink(mailFile);

	PEG_METHOD_EXIT();
	return;
    }

    try
    {
	// send the message
	_sendMsg(mailFile);
    }
    catch (CIMException &c)
    {
	unlink(mailFile);

	PEG_METHOD_EXIT();
	return;
    }

    unlink(mailFile);

    PEG_METHOD_EXIT();

#else
    PEG_TRACE_STRING(TRC_IND_HANDLER, Tracer::LEVEL4,
        "sendmail is not supported.");
    PEG_METHOD_EXIT();

   
    throw PEGASUS_CIM_EXCEPTION_L (CIM_ERR_NOT_SUPPORTED,
        MessageLoaderParms("Handler.EmailListenerDestination."
        "EmailListenerDestination.UNSUPPORTED_OPERATION",
        "sendmail is not supported."));
#endif
    PEG_METHOD_EXIT();

}

void EmailListenerDestination::_buildMailHeader(
    const Array<String> & mailTo,
    const Array<String> & mailCc,
    const String & mailSubject,
    FILE * filePtr)
{
    PEG_METHOD_ENTER (TRC_IND_HANDLER,
	"EmailListenerDestination::_buildMailHeader");

    String exceptionStr;

    String mailToStr = _buildMailAddrStr(mailTo);

    if (mailToStr == String::EMPTY)
    {
	PEG_METHOD_EXIT();
	throw PEGASUS_CIM_EXCEPTION_L (CIM_ERR_FAILED,
            MessageLoaderParms("Handler.EmailListenerDestination."
	    "EmailListenerDestination.DO_NOT_HAVE_EMAIL_ADDRESS",
	    "Do not have an e-mail address."));
    }

    String mailHdrStr = String::EMPTY;

    // Write the mailToStr to file
    mailHdrStr.append("To: ");
    mailHdrStr.append(mailToStr);
    _writeStrToFile(mailHdrStr, filePtr);

    String mailCcStr = _buildMailAddrStr(mailCc); 

    // Write the mailCcStr to file
    mailHdrStr = String::EMPTY;

    mailHdrStr.append("Cc: ");
    mailHdrStr.append(mailCcStr);
    _writeStrToFile(mailHdrStr, filePtr);

    // build from string 
    String fromStr = String::EMPTY;
    fromStr.append("From: ");
    fromStr.append(System::getEffectiveUserName());
    fromStr.append("@");
    fromStr.append(System::getFullyQualifiedHostName ());

    // Write the fromStr to file
    _writeStrToFile(fromStr, filePtr);

    // Write the mailSubject string to file
    String mailSubjectStr = String::EMPTY;
    mailSubjectStr.append("Subject: ");
    mailSubjectStr.append(mailSubject);
    _writeStrToFile(mailSubjectStr, filePtr);

    PEG_METHOD_EXIT();
}

String EmailListenerDestination::_buildMailAddrStr(
    const Array<String> & mailAddr)
{
    PEG_METHOD_ENTER (TRC_IND_HANDLER,
	"EmailListenerDestination::_buildMailAddrStr");

    String mailAddrStr = String::EMPTY;
    Uint32 mailAddrSize = mailAddr.size();

    for (Uint32 i=0; i < mailAddrSize; i++)
    {
	mailAddrStr.append(mailAddr[i]);

	if (i < (mailAddrSize - 1))
	{
	    mailAddrStr.append(",");
	}
    }

    PEG_METHOD_EXIT();
    return (mailAddrStr);
}

void EmailListenerDestination::_writeStrToFile(
    const String & mailHdrStr,
    FILE * filePtr)
{
    PEG_METHOD_ENTER (TRC_IND_HANDLER,
	"EmailListenerDestination::_writeStrToFile");

    String exceptionStr;

    if (fprintf(filePtr, "%s\n", (const char *)mailHdrStr.getCString()) < 0)
    {
        Tracer::trace(TRC_IND_HANDLER, Tracer::LEVEL4,
            "Failed to write the %s to the file: %s.",
	    (const char *)mailHdrStr.getCString(),
	    strerror(errno));

	MessageLoaderParms parms(
	    "Handler.EmailListenerDestination.EmailListenerDestination._MSG_WRITE_TO_THE_FILE_FAILED",
	    "Failed to write the $0 to the file: $1.",
	    mailHdrStr,
	    strerror(errno));

        exceptionStr.append(MessageLoader::getMessage(parms));

        PEG_METHOD_EXIT();

        throw PEGASUS_CIM_EXCEPTION (CIM_ERR_FAILED, exceptionStr);
    }

    PEG_METHOD_EXIT();
}

void EmailListenerDestination::_sendMsg(
    char * mailFile)
{
    PEG_METHOD_ENTER (TRC_IND_HANDLER,
	"EmailListenerDestination::_sendMsg");

    String exceptionStr;
    char sendmailCmd[MAX_SENDMAIL_CMD_LEN];
    FILE * sendmailPtr;
    struct stat statBuf;

    // Checks the existence of the temp mail file
    if (!System::exists(mailFile))
    {
        Tracer::trace(TRC_IND_HANDLER, Tracer::LEVEL4,
            "File %s does not exist: %s.",
	    mailFile,
	    strerror(errno));

	MessageLoaderParms parms(
	    "Handler.EmailListenerDestination.EmailListenerDestination._MSG_FILE_DOES_NOT_EXIST",
	    "File $0 does not exist: $1.",
	    mailFile,
	    strerror(errno));

        exceptionStr.append(MessageLoader::getMessage(parms));

        PEG_METHOD_EXIT();

        throw PEGASUS_CIM_EXCEPTION (CIM_ERR_FAILED, exceptionStr);
    }

    // Checks the length of the file since a zero length file causes
    // problems for sendmail()
    if (stat(mailFile, &statBuf) !=0)
    {
        Tracer::trace(TRC_IND_HANDLER, Tracer::LEVEL4,
            "Can not get file %s status: %s.",
	    mailFile,
	    strerror(errno));

	MessageLoaderParms parms(
	    "Handler.EmailListenerDestination.EmailListenerDestination._MSG_CAN_NOT_GET_FILE_STATUS",
	    "Can not get file $0 status: $1.",
	    mailFile,
	    strerror(errno));

        exceptionStr.append(MessageLoader::getMessage(parms));

        PEG_METHOD_EXIT();

        throw PEGASUS_CIM_EXCEPTION (CIM_ERR_FAILED, exceptionStr);
    }

    if (statBuf.st_size == 0)
    {
        Tracer::trace(TRC_IND_HANDLER, Tracer::LEVEL4,
            "File %s does not contain any data.",
	    mailFile);

	MessageLoaderParms parms(
	    "Handler.EmailListenerDestination.EmailListenerDestination._MSG_FILE_DOES_NOT_CONTAIN_DATA",
	    "File $0 does not contain any data.",
	    mailFile);

        exceptionStr.append(MessageLoader::getMessage(parms));

        PEG_METHOD_EXIT();

        throw PEGASUS_CIM_EXCEPTION (CIM_ERR_FAILED, exceptionStr);
    }

    sprintf(sendmailCmd, "%s %s %s", SENDMAIL_CMD, 
	    SENDMAIL_CMD_OPTS, mailFile);

    // Open the pipe to send the message
    if ((sendmailPtr = popen(sendmailCmd, "r")) == NULL)
    {
        Tracer::trace(TRC_IND_HANDLER, Tracer::LEVEL4,
            "popen of sendmail failed.");

	MessageLoaderParms parms(
	    "Handler.EmailListenerDestination.EmailListenerDestination._MSG_POPEN_OF_SENDMAIL_FAILED",
	    "popen of sendmail failed.");

        exceptionStr.append(MessageLoader::getMessage(parms));

        PEG_METHOD_EXIT();

        throw PEGASUS_CIM_EXCEPTION (CIM_ERR_FAILED, exceptionStr);

    }

    // Close the pipe
    Sint32 retCode = pclose(sendmailPtr);
    if (retCode < 0)
    {
        Tracer::trace(TRC_IND_HANDLER, Tracer::LEVEL4,
            "No associated stream with this popen command.");

	MessageLoaderParms parms(
	    "Handler.EmailListenerDestination.EmailListenerDestination._MSG_NO_ASSOCIATED_STREAM",
	    "No associated stream with this popen command.");

        exceptionStr.append(MessageLoader::getMessage(parms));

        PEG_METHOD_EXIT();

        throw PEGASUS_CIM_EXCEPTION (CIM_ERR_FAILED, exceptionStr);
    }
    else if (retCode == SH_EXECUTE_FAILED)
    {
        Tracer::trace(TRC_IND_HANDLER, Tracer::LEVEL4,
            "/usr/bin/sh could not be executed.");

	MessageLoaderParms parms(
	    "Handler.EmailListenerDestination.EmailListenerDestination._MSG_SHELL_CAN_NOT_BE_EXECUTED",
	    "/usr/bin/sh could not be executed.");

        exceptionStr.append(MessageLoader::getMessage(parms));

        PEG_METHOD_EXIT();

        throw PEGASUS_CIM_EXCEPTION (CIM_ERR_FAILED, exceptionStr);
    }
    PEG_METHOD_EXIT();
}

void EmailListenerDestination::_openFile(
    FILE **filePtr,
    char * mailFile)
{
    PEG_METHOD_ENTER (TRC_IND_HANDLER,
	"EmailListenerDestination::_openFile");

    String exceptionStr;

    *filePtr = fopen(tmpnam(mailFile), "w");
    if (*filePtr == NULL)
    {
        Tracer::trace(TRC_IND_HANDLER, Tracer::LEVEL4,
            "fopen of %s failed: %s.", mailFile,
	    strerror(errno));

	MessageLoaderParms parms(
	    "Handler.EmailListenerDestination.EmailListenerDestination._MSG_FAILED_TO_OPEN_THE_FILE",
	    "fopen of $0 failed: $1.",
	    mailFile,
	    strerror(errno));

        exceptionStr.append(MessageLoader::getMessage(parms));

        PEG_METHOD_EXIT();

        throw PEGASUS_CIM_EXCEPTION (CIM_ERR_FAILED, exceptionStr);
    }

    PEG_METHOD_EXIT();
}

// This is the dynamic entry point into this dynamic module. The name of
// this handler is "EmailListenerDestination" which is appended to "PegasusCreateHandler_"
// to form a symbol name. This function is called by the HandlerTable
// to load this handler.

extern "C" PEGASUS_EXPORT CIMHandler* 
    PegasusCreateHandler_EmailListenerDestination() {
    return new EmailListenerDestination;
}

PEGASUS_NAMESPACE_END
