//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett-Packard Company, IBM, 
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
//         Nag Boranna, Hewlett-Packard Company ( nagaraja_boranna@hp.com )
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


// debug flag
#define SSLCONTEXT_DEBUG(X) // X

// switch on 'server needs certified client'
//#define CLIENT_CERTIFY


#ifdef PEGASUS_HAS_SSL
//
// use the following definitions only if SSL is available
// 

//
// certificate handling routine
//

VERIFY_CERTIFICATE verify_certificate;

static int cert_verify(SSL_CTX *ctx, char *cert_file, char *key_file)
{

   if (cert_file != NULL)
   {
       if (SSL_CTX_use_certificate_file(ctx,cert_file,SSL_FILETYPE_PEM) <=0)
       {
           SSLCONTEXT_DEBUG(cerr << "no certificate found in " << cert_file << endl;)
           return 0;
       }
       if (key_file == NULL) key_file=cert_file;
       if (SSL_CTX_use_PrivateKey_file(ctx,key_file,SSL_FILETYPE_PEM) <= 0)
       {
           SSLCONTEXT_DEBUG(cerr << "no private key found in " << key_file << endl;)
           return 0;
       }

       if (!SSL_CTX_check_private_key(ctx)) 
       {
           SSLCONTEXT_DEBUG(cerr << "Private and public key do not match\n";)
           return 0;
       }
   }
   return -1;
}

static int prepareForCallback(int preverifyOk, X509_STORE_CTX *ctx)
{
    PEG_METHOD_ENTER(TRC_SSL, "CertificateManager::prepareForCallback()");

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
        SSLCONTEXT_DEBUG(cerr << "---> verify error: num = " << err << ", " 
            << X509_verify_cert_error_string(err) << ", " << depth 
            << ", " << buf <<  endl;)
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
    CertificateInfo certInfo(subjectName, issuerName, depth, err);

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

    SSLCONTEXT_DEBUG(cerr << "verify return: " << preverifyOk << endl;)

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
                       VERIFY_CERTIFICATE verifyCert,
                       const String& randomFile,
                       Boolean isCIMClient)
    throw(SSL_Exception)
{
    SSLCONTEXT_DEBUG(cout << "Entering SSLContextRep::SSLContextRep()\n";)

    _certPath = certPath.allocateCString();

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
          ArrayDestroyer<char> pRandomFile(randomFile.allocateCString());
          char* randFilename = pRandomFile.getPointer();
   
          SSLCONTEXT_DEBUG( cout << "load Rand file: name=" << randFilename << endl; )
          int ret = RAND_load_file(randFilename, -1);
          if ( ret < 0 )
          {
            SSLCONTEXT_DEBUG( cerr << " RAND_load_file failed, Status="<< ret << endl;)
            throw( SSL_Exception("RAND_load_file - failed"));
          }
          else
          {
            SSLCONTEXT_DEBUG( cerr << " RAND_load_file Status="<< ret << endl;)
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
            SSLCONTEXT_DEBUG( cerr << " Not enough data , Rand Status="<< seedRet << endl;)
            throw( SSL_Exception("RAND_seed - Not enough seed data "));
          }
       }
       else
       {
           throw( SSL_Exception("Random seed file required"));
       }
 
     }

#endif // end of PEGASUS_SSL_RANDOMFILE

    //
    // create SSL Context Area
    //

    if (!( _SSLContext = SSL_CTX_new(SSLv23_method()) ))
        throw( SSL_Exception("Could not get SSL CTX"));

    //
    // set overall SSL Context flags
    //

    SSL_CTX_set_quiet_shutdown(_SSLContext, 1);
    SSL_CTX_set_mode(_SSLContext, SSL_MODE_AUTO_RETRY);
    SSL_CTX_set_options(_SSLContext,SSL_OP_ALL);

#ifdef CLIENT_CERTIFY
   SSL_CTX_set_verify(_SSLContext, SSL_VERIFY_PEER | SSL_VERIFY_CLIENT_ONCE, 
       prepareForCallback);
#else   
    if (verifyCert != NULL)
    {
        SSL_CTX_set_verify(_SSLContext, 
            SSL_VERIFY_PEER | SSL_VERIFY_CLIENT_ONCE, prepareForCallback);
    }
#endif // end of  CLIENT_CERTIFY

    //
    // check certificate given to me
    //

    if (!cert_verify(_SSLContext, _certPath, _certPath))
        throw( SSL_Exception("Could not get certificate and/or private key"));

    SSLCONTEXT_DEBUG(cout << "Leaving SSLContextRep::SSLContextRep()\n";)
}

  
//
// Destructor
//

SSLContextRep::~SSLContextRep()
{
    SSLCONTEXT_DEBUG(cout << "Entering SSLContextRep::~SSLContextRep()\n";)

    free(_certPath);
    SSL_CTX_free(_SSLContext);

    SSLCONTEXT_DEBUG(cout << "Leaving SSLContextRep::~SSLContextRep()\n";)
}

SSL_CTX * SSLContextRep::getContext() const 
{
    return _SSLContext;
}
#else 

//
// these definitions are used if ssl is not availabel
//

SSLContextRep::SSLContextRep(const String& certPath,
                       VERIFY_CERTIFICATE verifyCert,
                       const String& randomFile,
                       Boolean isCIMClient) throw(SSL_Exception) {}

SSLContextRep::~SSLContextRep() {}

SSL_CTX * SSLContextRep::getContext() const { return NULL; }

#endif // end of PEGASUS_HAS_SSL

///////////////////////////////////////////////////////////////////////////////
//
// SSLContext
//
///////////////////////////////////////////////////////////////////////////////


SSLContext::SSLContext(
    const String& certPath,
    VERIFY_CERTIFICATE verifyCert,
    const String& randomFile,
    Boolean isCIMClient) throw(SSL_Exception) 
{
    _rep = new SSLContextRep(certPath, verifyCert, randomFile, isCIMClient);
}

SSLContext::~SSLContext() 
{
    delete _rep;
}


///////////////////////////////////////////////////////////////////////////////
//
// CertificateInfo
//
///////////////////////////////////////////////////////////////////////////////

CertificateInfo::CertificateInfo(
    const String subjectName,
    const String issuerName,
    const int errorDepth,
    const int errorCode)
    :
    _subjectName(subjectName),
    _issuerName(issuerName),
    _errorDepth(errorDepth),
    _errorCode(errorCode)
{
    _respCode = 0;
}

CertificateInfo::~CertificateInfo()
{

}

String CertificateInfo::getSubjectName() const
{
    return (_subjectName);
}

String CertificateInfo::getIssuerName() const
{
    return (_issuerName);
}

int CertificateInfo::getErrorDepth() const
{
    return (_errorDepth);
}

int CertificateInfo::getErrorCode() const
{
    return (_errorCode);
}

void CertificateInfo::setResponseCode(const int respCode)
{
    _respCode = respCode;
}

PEGASUS_NAMESPACE_END

