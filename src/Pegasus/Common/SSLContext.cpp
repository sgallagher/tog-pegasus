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
// Modified By: Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifdef PEGASUS_HAS_SSL
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <openssl/rand.h>
#else
#define SSL_CTX void
#endif // end of PEGASUS_HAS_SSL
#include <Pegasus/Common/Destroyer.h>
#include <Pegasus/Common/Socket.h>
#include <Pegasus/Common/Tracer.h>

#include "SSLContext.h"
#include "SSLContextRep.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

// switch on if 'server needs certified client'
//#define CLIENT_CERTIFY

//
// use the following definitions only if SSL is available
// 
#ifdef PEGASUS_HAS_SSL

//
// certificate handling routine
//

// ATTN-RK-20020905: This global variable is unsafe with multiple SSL contexts
SSLCertificateVerifyFunction* verify_certificate;

static int cert_verify(SSL_CTX *ctx, const char *cert_file, const char *key_file)
{
    PEG_METHOD_ENTER(TRC_SSL, "cert_verify()");

    if (cert_file != NULL)
    {
        if (SSL_CTX_use_certificate_file(ctx,cert_file,SSL_FILETYPE_PEM) <=0)
        {
            PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL4, 
                "---> SSL: no certificate found in " + String(cert_file));
            PEG_METHOD_EXIT();
            return 0;
        }
        if (key_file == NULL) key_file=cert_file;
        if (SSL_CTX_use_PrivateKey_file(ctx,key_file,SSL_FILETYPE_PEM) <= 0)
        {
            PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL4, 
                "---> SSL: no private key found in " + String(key_file));
            PEG_METHOD_EXIT();
            return 0;
        }

        if (!SSL_CTX_check_private_key(ctx)) 
        {
            PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL4, 
                "---> SSL: Private and public key do not match");
            PEG_METHOD_EXIT();
            return 0;
        }
    }
    PEG_METHOD_EXIT();
    return -1;
}

static int prepareForCallback(int preverifyOk, X509_STORE_CTX *ctx)
{
    PEG_METHOD_ENTER(TRC_SSL, "prepareForCallback()");

    char   buf[256];
    X509   *err_cert;
    int    err; 
    int    depth;
    String subjectName;
    String issuerName;
    int    verify_depth = 0;
    int    verify_error = X509_V_OK;

    err_cert = X509_STORE_CTX_get_current_cert(ctx);
    err = X509_STORE_CTX_get_error(ctx);

    depth = X509_STORE_CTX_get_error_depth(ctx);

    //FUTURE: Not sure what to do with these...?
    //ssl = X509_STORE_CTX_get_ex_data(ctx, SSL_get_ex_data_X509_STORE_CTX_idx());
    //mydata = SSL_get_ex_data(ssl, mydata_index);

    X509_NAME_oneline(X509_get_subject_name(err_cert), buf, 256);
    subjectName = String(buf);

    if (verify_depth >= depth)
    {
        preverifyOk = 1;
        verify_error = X509_V_OK;
    }
    else
    {
        preverifyOk = 0;
        verify_error = X509_V_ERR_CERT_CHAIN_TOO_LONG;
        X509_STORE_CTX_set_error(ctx, verify_error);
    }

    if (!preverifyOk)
    {
        PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL4, 
            "---> SSL: verify error: " + String(X509_verify_cert_error_string(err)));
    }

    if (!preverifyOk && (err == X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT))
    {
        X509_NAME_oneline(X509_get_issuer_name(ctx->current_cert), buf, 256);
        issuerName = String(buf);
    }
    else
    {
        X509_NAME_oneline(X509_get_issuer_name(err_cert), buf, 256);
        issuerName = String(buf);
    }

    //
    // Call the verify_certificate() callback
    //
    SSLCertificateInfo certInfo(subjectName, issuerName, depth, err);

    if (verify_certificate(certInfo))
    {
        preverifyOk = 1;
    }

    //delete certInfo;

    // ATTN-NB-03-05102002: Overriding the default verification result, may
    // need to be changed to make it more generic.
    if (preverifyOk)
    {
        X509_STORE_CTX_set_error(ctx, verify_error);
    }

    PEG_METHOD_EXIT();
    return(preverifyOk);
}


//
// SSL context area
//
// For the OSs that don't have /dev/random device file,
// must enable PEGASUS_SSL_RANDOMFILE flag.
//
// CIM clients must specify a SSL random file and also
// set isCIMClient to true. However, CIMserver does not
// seem to care the Random seed and /dev/random. 
//
//
SSLContextRep::SSLContextRep(const String& certPath,
                       SSLCertificateVerifyFunction* verifyCert,
                       const String& randomFile,
                       Boolean isCIMClient)
{
    PEG_METHOD_ENTER(TRC_SSL, "SSLContextRep::SSLContextRep()");

    _certPath = certPath.getCString();

    verify_certificate = verifyCert;

    //
    // load SSL library
    //
    SSL_load_error_strings();
    SSL_library_init();

#ifdef PEGASUS_SSL_RANDOMFILE
    
    //
    // We will only need SSL Random Seed for CIM Clients
    // 
    if (isCIMClient) 
    {
       long  seedNumber;
       //
       // Initialise OpenSSL 0.9.5 random number generator.
       //
       if ( randomFile != String::EMPTY )
       {
          int ret = RAND_load_file(randomFile.getCString(), -1);
          if ( ret < 0 )
          {
            PEG_METHOD_EXIT();
            throw( SSLException("RAND_load_file - failed"));
          }

          //
          // Will do more seeding
          //
          srandom((unsigned int)time(NULL)); // Initialize
          seedNumber = random();
          RAND_seed((unsigned char *) &seedNumber, sizeof(seedNumber));

          int  seedRet = RAND_status();
          if ( seedRet == 0 )
          {
              PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL4, 
                  "Not enough data , RAND_status = " + seedRet );
              PEG_METHOD_EXIT();
              throw( SSLException("RAND_seed - Not enough seed data "));
          }
       }
       else
       {
           PEG_METHOD_EXIT();
           throw( SSLException("Random seed file required"));
       }
 
     }

#endif // end of PEGASUS_SSL_RANDOMFILE

    _sslContext = _makeSSLContext();

    PEG_METHOD_EXIT();
}

SSLContextRep::SSLContextRep(const SSLContextRep& sslContextRep)
{
    PEG_METHOD_ENTER(TRC_SSL, "SSLContextRep::SSLContextRep()");

    _certPath = sslContextRep._certPath;
    // ATTN: verify_certificate is set implicitly in global variable
    _randomFile = sslContextRep._randomFile;
    _isCIMClient = sslContextRep._isCIMClient;
    _sslContext = _makeSSLContext();

    PEG_METHOD_EXIT();
}

//
// Destructor
//

SSLContextRep::~SSLContextRep()
{
    PEG_METHOD_ENTER(TRC_SSL, "SSLContextRep::~SSLContextRep()");

    SSL_CTX_free(_sslContext);

    PEG_METHOD_EXIT();
}

SSL_CTX * SSLContextRep::_makeSSLContext()
{
    PEG_METHOD_ENTER(TRC_SSL, "SSLContextRep::_makeSSLContext()");

    SSL_CTX * sslContext = 0;

    //
    // create SSL Context Area
    //

    if (!( sslContext = SSL_CTX_new(SSLv23_method()) ))
    {
        PEG_METHOD_EXIT();
        throw( SSLException("Could not get SSL CTX"));
    }

#ifdef PEGASUS_OS_HPUX
    if (!(SSL_CTX_set_cipher_list(sslContext, SSL_TXT_EXP40)))
        throw( SSLException("Could not set the cipher list"));
#endif

    //
    // set overall SSL Context flags
    //

    SSL_CTX_set_quiet_shutdown(sslContext, 1);
    SSL_CTX_set_mode(sslContext, SSL_MODE_AUTO_RETRY);
    SSL_CTX_set_options(sslContext,SSL_OP_ALL);

#ifdef CLIENT_CERTIFY
    SSL_CTX_set_verify(sslContext, SSL_VERIFY_PEER | SSL_VERIFY_CLIENT_ONCE, 
        prepareForCallback);
#else
    if (verify_certificate != NULL)
    {
        SSL_CTX_set_verify(sslContext, 
            SSL_VERIFY_PEER | SSL_VERIFY_CLIENT_ONCE, prepareForCallback);
    }
#endif

    //
    // check certificate given to me
    //

    if (!cert_verify(sslContext, _certPath, _certPath))
    {
        PEG_METHOD_EXIT();
        throw( SSLException("Could not get certificate and/or private key"));
    }

    PEG_METHOD_EXIT();
    return sslContext;
}

SSL_CTX * SSLContextRep::getContext() const 
{
    return _sslContext;
}
#else 

//
// these definitions are used if ssl is not available
//

SSLContextRep::SSLContextRep(const String& certPath,
                       SSLCertificateVerifyFunction* verifyCert,
                       const String& randomFile,
                       Boolean isCIMClient) {}

SSLContextRep::SSLContextRep(const SSLContextRep& sslContextRep) {}

SSLContextRep::~SSLContextRep() {}

SSL_CTX * SSLContextRep::_makeSSLContext() { return 0; }

SSL_CTX * SSLContextRep::getContext() const { return 0; }

#endif // end of PEGASUS_HAS_SSL

///////////////////////////////////////////////////////////////////////////////
//
// SSLContext
//
///////////////////////////////////////////////////////////////////////////////


SSLContext::SSLContext(
    const String& certPath,
    SSLCertificateVerifyFunction* verifyCert,
    const String& randomFile,
    Boolean isCIMClient)
{
    _rep = new SSLContextRep(certPath, verifyCert, randomFile, isCIMClient);
}

SSLContext::SSLContext(const SSLContext& sslContext)
{
    _rep = new SSLContextRep(*sslContext._rep);
}

// Dummy constructor made private to disallow default construction
SSLContext::SSLContext()
{
}

SSLContext::~SSLContext() 
{
    delete _rep;
}


///////////////////////////////////////////////////////////////////////////////
//
// SSLCertificateInfo
//
///////////////////////////////////////////////////////////////////////////////

class SSLCertificateInfoRep
{
public:
    String subjectName;
    String issuerName;
    int    errorDepth;
    int    errorCode;
    int    respCode;
};


SSLCertificateInfo::SSLCertificateInfo(
    const String subjectName,
    const String issuerName,
    const int errorDepth,
    const int errorCode)
{
    _rep = new SSLCertificateInfoRep();
    _rep->subjectName = subjectName;
    _rep->issuerName = issuerName;
    _rep->errorDepth = errorDepth;
    _rep->errorCode = errorCode;
    _rep->respCode = 0;
}

SSLCertificateInfo::SSLCertificateInfo(
    const SSLCertificateInfo& certificateInfo)
{
    _rep = new SSLCertificateInfoRep();
    _rep->subjectName = certificateInfo._rep->subjectName;
    _rep->issuerName = certificateInfo._rep->issuerName;
    _rep->errorDepth = certificateInfo._rep->errorDepth;
    _rep->errorCode = certificateInfo._rep->errorCode;
    _rep->respCode = certificateInfo._rep->respCode;
}

// Dummy constructor made private to disallow default construction
SSLCertificateInfo::SSLCertificateInfo()
{
}

SSLCertificateInfo::~SSLCertificateInfo()
{
    delete _rep;
}

String SSLCertificateInfo::getSubjectName() const
{
    return (_rep->subjectName);
}

String SSLCertificateInfo::getIssuerName() const
{
    return (_rep->issuerName);
}

int SSLCertificateInfo::getErrorDepth() const
{
    return (_rep->errorDepth);
}

int SSLCertificateInfo::getErrorCode() const
{
    return (_rep->errorCode);
}

void SSLCertificateInfo::setResponseCode(const int respCode)
{
    _rep->respCode = respCode;
}

PEGASUS_NAMESPACE_END

