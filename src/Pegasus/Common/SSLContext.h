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
// Author: Markus Mueller (sedgewick_de@yahoo.de)
//
// Modified By:
//         Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_SSLContext_h
#define Pegasus_SSLContext_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Exception.h>


PEGASUS_NAMESPACE_BEGIN

class SSLContextRep;


/** This class provides the interface that a client gets as argument
    to certificate verification call back function.
*/
class PEGASUS_EXPORT CertificateInfo
{
public:
    /** Constructor for a CertificateInfo object.
    @param subjectName subject name of the certificate
    @param issuerName  issuer name of the certificate
    @param errorDepth  depth of the certificate chain
    @param errorCode   error code from the default verification of the
    certificates by the Open SSL library.
    */
    CertificateInfo(
        const String subjectName,
        const String issuerName,
        const int errorDepth,
        const int errorCode);

    ~CertificateInfo();

    /** Gets the subject name of the certificate
    @return a string containing the subject name.
    */
    String getSubjectName() const;

    /** Gets the issuer name of the certificate
    @return a string containing the issuer name.
    */
    String getIssuerName() const;

    /** Gets the depth of the certificate chain
    @return an int containing the depth of the certificate chain
    */
    int getErrorDepth() const;

    /** Gets the preverify error code
    @return an int containing the preverification error code 
    */
    int getErrorCode() const;

    /** Sets the response code
    @param respCode response code to be set.
    */
    void setResponseCode(const int respCode);

private:

    String _subjectName;

    String _issuerName;

    int    _errorDepth;

    int    _errorCode;

    int    _respCode;
};

typedef Boolean (*VERIFY_CERTIFICATE) (CertificateInfo &certInfo);

/** This class provides the interface that a client uses to create
    SSL context.

    For the OSs that don't have /dev/random device file,
    must enable PEGASUS_SSL_RANDOMFILE flag.

    CIM clients must specify a SSL random file and also
    set isCIMClient to true. 
*/
class PEGASUS_EXPORT SSLContext
{
public:

    /** Constructor for a SSLContext object.
    @param certPath  certificate file path
    @param verifyCert  function pointer to a certificate verification
    call back function.
    @param randomFile  file path of a random file that is used as a seed 
    for random number generation by OpenSSL.
    @param isCIMClient  flag indicating that the context is created by
    the client.

    @exception SSL_Exception  exception indicating failure to create a context.
    */
    SSLContext(
        const String& certPath,
        VERIFY_CERTIFICATE verifyCert = NULL,
        const String& randomFile = String::EMPTY,
        Boolean isCIMClient = false) throw(SSL_Exception);

    ~SSLContext();

private:

    SSLContextRep* _rep;

    friend class SSLSocket;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_SSLContext_h */
