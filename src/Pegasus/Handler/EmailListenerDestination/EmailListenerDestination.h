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

PEGASUS_NAMESPACE_BEGIN

PEGASUS_USING_STD;

#if defined(PEGASUS_OS_HPUX) || \
    defined(PEGASUS_OS_LINUX)
# define SENDMAIL_CMD "/usr/sbin/sendmail"
# define SENDMAIL_CMD_OPTS "-t -i <"
# define TEMP_MAIL_FILE "tmpMailFile"
# define MAX_SENDMAIL_CMD_LEN 100
# define SH_EXECUTE_FAILED 127
# define TEMP_NAME_LEN L_tmpnam
# include <unistd.h>
#endif

class PEGASUS_HANDLER_LINKAGE EmailListenerDestination: public CIMHandler
{
public:

    EmailListenerDestination()
    {
    }

    virtual ~EmailListenerDestination()
    {
    }

    void initialize(CIMRepository* repository);

    void terminate()
    {
    }

    void handleIndication(
	const OperationContext& context,
	const String nameSpace,
	CIMInstance& indication, 
	CIMInstance& handler, 
	CIMInstance& subscription, 
	ContentLanguages& contentLanguages);

private:

    /**
        Sends the formatted indication to the specified recipients. 
	Utility "sendmail" is the default tool to be used. The platform 
	maintainer can also choose different tools.

        @param  mailTo         the addresses of the To: field 
        @param  mailCc         the addresses of the Cc: field 
        @param  mailSubject    the Subject: field of the mail message 
        @param  formattedText  the formatted indication 
    */

    void _sendViaEmail(
	const Array<String> & mailTo,
	const Array<String> & mailCc,
	const String & mailSubject,
	const String & formattedText);

    /**
        Build the header of the mail message which includes To:, Cc:,
	From:, and Subject: fields

        @param  mailTo         the addresses of the To: field 
        @param  mailCc         the addresses of the Cc: field 
        @param  mailSubject    the Subject: field of the mail message 
        @param  filePtr        the pointer to the temporary file 
    */

    void _buildMailHeader(
	const Array<String> & mailTo,
	const Array<String> & mailCc,
	const String & mailSubject,
	FILE * filePtr);

    /**
        Build the mail address string from address array 

        @param  mailAddr       the array of the mail addresses 

        @return the string of the mail addresses 
    */
    String _buildMailAddrStr(
	const Array<String> & mailAddr);

    /**
        Write the mail header string to file 

        @param  mailHdrStr     the header string to be written to the file
        @param  filePtr        the pointer to the temporary file 
    */
    void _writeStrToFile(
	const String & mailHdrStr,
	FILE * filePtr);

    /**
        Sends the indication via e-mail by opening a pipe to sendmail() 

        @param  mailFile       The temporary file name 
    */
    void _sendMsg(char * mailFile);

    /**
	Opens a temporary file to hold the indication mail message

        @param  filePtr     the address of the pointer to the open file 
        @param  mailFile    the temporary file name 
    */
    void _openFile(
	FILE **filePtr,
	char * mailFile);

};

PEGASUS_NAMESPACE_END
