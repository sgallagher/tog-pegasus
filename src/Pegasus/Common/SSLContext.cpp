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
//              Sushma Fernandes,
//                  Hewlett-Packard Company (sushma_fernandes@hp.com)
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
#include <Pegasus/Common/FileSystem.h>

#include "SSLContext.h"
#include "SSLContextRep.h"

PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

//
// use the following definitions only if SSL is available
// 
#ifdef PEGASUS_HAS_SSL

//
// certificate handling routine
//

// ATTN-RK-20020905: This global variable is unsafe with multiple SSL contexts
SSLCertificateVerifyFunction* verify_certificate;

// Mutex for SSL locks.
Mutex* SSLContextRep::_sslLocks = 0;

// Mutex for _countRep.
Mutex SSLContextRep::_countRepMutex;

// Initialise _count for SSLContextRep objects.
int SSLContextRep::_countRep = 0;

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
    SSLCertificateInfo certInfo(subjectName, issuerName, depth, err, preverifyOk);

    if (verify_certificate(certInfo))
    {
        preverifyOk = 1;
    }

    PEG_METHOD_EXIT();
    return(preverifyOk);
}

//
// Implement OpenSSL locking callback.
//
void pegasus_locking_callback( int 		mode, 
                               int 		type, 
                               const char* 	file, 
                               int 		line)
{
    // Check whether the mode is lock or unlock.

    if ( mode & CRYPTO_LOCK )
    {
        Tracer::trace(TRC_SSL, Tracer::LEVEL4,
                "Now locking for %d", pegasus_thread_self());
        SSLContextRep::_sslLocks[type].lock( pegasus_thread_self() );
    }
    else
    {
        Tracer::trace(TRC_SSL, Tracer::LEVEL4,
                "Now unlocking for %d", pegasus_thread_self());
        SSLContextRep::_sslLocks[type].unlock( );
    }
}

//
// Initialize OpenSSL Locking and id callbacks.
//
void SSLContextRep::init_ssl()
{
     // Allocate Memory for _sslLocks. SSL locks needs to be able to handle
     // up to CRYPTO_num_locks() different mutex locks.
     PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL4,
           "Initialized SSL callback.");

     _sslLocks= new Mutex[CRYPTO_num_locks()];

     // Set the ID callback. The ID callback returns a thread ID.

     CRYPTO_set_id_callback((unsigned long (*)())pegasus_thread_self);

     // Set the locking callback to pegasus_locking_callback.

     CRYPTO_set_locking_callback((void (*)(int,int,const char *,int))pegasus_locking_callback);

}

// Free OpenSSL Locking and id callbacks.
void SSLContextRep::free_ssl()
{
    // Cleanup _sslLocks and set locking & id callback to NULL.

    CRYPTO_set_locking_callback(NULL);
    CRYPTO_set_id_callback     (NULL);
    PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL4,
             "Freed SSL callback.");

    delete []_sslLocks;
}


//
// SSL context area
//
// For the OSs that don't have /dev/random device file,
// must enable PEGASUS_SSL_RANDOMFILE flag.
//
SSLContextRep::SSLContextRep(const String& certPath,
                       const String& certKeyPath,
                       SSLCertificateVerifyFunction* verifyCert,
                       const String& randomFile)
{
    PEG_METHOD_ENTER(TRC_SSL, "SSLContextRep::SSLContextRep()");

    _certPath = certPath.getCString();

    _certKeyPath = certKeyPath.getCString();

    verify_certificate = verifyCert;


    // Initialiaze SSL callbacks and increment the SSLContextRep object _counter.
    _countRepMutex.lock(pegasus_thread_self());

    try
    {
        Tracer::trace(TRC_SSL, Tracer::LEVEL4,
                "Value of Countrep in constructor %d", _countRep);
        if ( _countRep == 0 )
        {
            init_ssl();

            //
            // load SSL library
            //
            Tracer::trace(TRC_SSL, Tracer::LEVEL4,
                "Before calling SSL_load_error_strings %d", pegasus_thread_self());

            SSL_load_error_strings();

            Tracer::trace(TRC_SSL, Tracer::LEVEL4,
                "After calling SSL_load_error_strings %d", pegasus_thread_self());

            Tracer::trace(TRC_SSL, Tracer::LEVEL4,
                "Before calling SSL_library_init %d", pegasus_thread_self());

            SSL_library_init();

            Tracer::trace(TRC_SSL, Tracer::LEVEL4,
                "After calling SSL_library_init %d", pegasus_thread_self());

        } 
    }
    catch(...)
    {
        _countRepMutex.unlock();
        throw;
    }
    _countRep++;
    _countRepMutex.unlock();

    _randomInit(randomFile);

    _sslContext = _makeSSLContext();

    PEG_METHOD_EXIT();
}

SSLContextRep::SSLContextRep(const SSLContextRep& sslContextRep)
{
    PEG_METHOD_ENTER(TRC_SSL, "SSLContextRep::SSLContextRep()");

    _certPath = sslContextRep._certPath;
    _certKeyPath = sslContextRep._certKeyPath;
    // ATTN: verify_certificate is set implicitly in global variable
    _randomFile = sslContextRep._randomFile;

    // Initialiaze SSL callbacks and increment the SSLContextRep object _counter.
    _countRepMutex.lock(pegasus_thread_self());
    try
    {
        Tracer::trace(TRC_SSL, Tracer::LEVEL4,
             "Value of Countrep in copy constructor %d", _countRep);
        if ( _countRep == 0 )
        {
            init_ssl();
        } 
    }
    catch(...)
    {
        _countRepMutex.unlock();
        throw;
    }
    _countRep++;
    _countRepMutex.unlock();

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

    // Decrement the SSLContextRep object _counter.
    _countRepMutex.lock(pegasus_thread_self());
    _countRep--;
    // Free SSL locks if no instances of SSLContextRep exist.
    try
    {
        Tracer::trace(TRC_SSL, Tracer::LEVEL4,
                "Value of Countrep in destructor %d", _countRep);
        if ( _countRep == 0 )
        {
            free_ssl();
        }
    }
    catch(...)
    {
        _countRepMutex.unlock();
        throw;
    }
    _countRepMutex.unlock();
    PEG_METHOD_EXIT();
}

//
// initialize OpenSSL's PRNG
//
void SSLContextRep::_randomInit(const String& randomFile)
{
    PEG_METHOD_ENTER(TRC_SSL, "SSLContextRep::_randomInit()");

    Boolean ret;
    int retVal = 0;

    const int DEV_RANDOM_BYTES = 64;	        /* how many bytes to read */
    const String devRandom = "/dev/random";     /* random device name */
    const String devUrandom = "/dev/urandom";   /* pseudo-random device name */

#ifdef PEGASUS_SSL_DEVRANDOM

    if ( FileSystem::exists(devRandom) )
    {
        while ( RAND_status() == 0 )
        {
            //
            // Always attempt to seed from good entropy sources, first
            // try /dev/random
            //
            retVal = RAND_load_file(devRandom.getCString(), DEV_RANDOM_BYTES);
            if (retVal <= 0)
            {
                break;
            }
        }
    }

    if ( FileSystem::exists(devUrandom) )
    {
        while ( RAND_status() == 0 )
        {
            //
            // If there isn't /dev/random try /dev/urandom
            //
            retVal = RAND_load_file(devUrandom.getCString(), DEV_RANDOM_BYTES);
            if (retVal <= 0)
            {
                break;
            }
        }
    }
#endif  /* PEGASUS_SSL_DEVRANDOM */


#ifdef PEGASUS_SSL_RANDOMFILE
    if ( RAND_status() == 0 )
    {
        //
        // Initialise OpenSSL random number generator.
        //
        if ( randomFile == String::EMPTY )
        {
            PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL4,
                "Random seed file is required.");
            PEG_METHOD_EXIT();
            throw( SSLException("Random seed file required"));
        }

        //
        // Try the given random seed file
        //
        ret = FileSystem::exists(randomFile);
        if( ret )
        {
            retVal = RAND_load_file(randomFile.getCString(), -1); 
            if ( retVal < 0 )
            {
                PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL4,
                    "Not enough seed data in seed file: " + randomFile);
                PEG_METHOD_EXIT();
                throw( SSLException("Not enough seed data in random seed file."));
            }
        }

        if ( RAND_status() == 0 )
        {
            //
            // Try to do more seeding
            //
            long seedNumber;
            srandom((unsigned int)time(NULL)); // Initialize
            seedNumber = random();
            RAND_seed((unsigned char *) &seedNumber, sizeof(seedNumber));

            int  seedRet = RAND_status();
            if ( seedRet == 0 )
            {
                PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL4,
                    "Not enough seed data in random seed file, RAND_status = " +
                    seedRet);
                PEG_METHOD_EXIT();
                throw( SSLException("Not enough seed data in random seed file."));
            }
        }
    }
#endif  /* PEGASUS_SSL_RANDOMFILE */

    int  seedRet = RAND_status();
    if ( seedRet == 0 )
    {
        PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL4,
            "Not enough seed data , RAND_status = " + seedRet );
        PEG_METHOD_EXIT();
        throw( SSLException("Not enough seed data."));
    }

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

#ifdef PEGASUS_SSL_WEAKENCRYPTION
    if (!(SSL_CTX_set_cipher_list(sslContext, SSL_TXT_EXP40)))
        throw( SSLException("Could not set the cipher list"));
#endif

    //
    // set overall SSL Context flags
    //

    SSL_CTX_set_quiet_shutdown(sslContext, 1);
    SSL_CTX_set_mode(sslContext, SSL_MODE_AUTO_RETRY);
    SSL_CTX_set_options(sslContext,SSL_OP_ALL);
    SSL_CTX_set_session_cache_mode(sslContext, SSL_SESS_CACHE_OFF);

    if (verify_certificate != NULL)
    {
        SSL_CTX_set_verify(sslContext, 
            SSL_VERIFY_PEER | SSL_VERIFY_CLIENT_ONCE, prepareForCallback);
    }
    else
    {
	SSL_CTX_set_verify(sslContext, SSL_VERIFY_NONE, NULL);
    }

    //
    // Check if there is CA certificate file specified. If specified,
    // load the certificates from the file in to the CA trust store.
    //
    if (strncmp(_certPath, "", 1) != 0)
    {
        //
        // load certificates in to trust store
        //

        if ((!SSL_CTX_load_verify_locations(sslContext, _certPath, NULL)) ||
            (!SSL_CTX_set_default_verify_paths(sslContext)))
        {
            PEG_METHOD_EXIT();
            throw( SSLException("Could not load certificates in to trust store."));
        }
    }

    //
    // Check if there is a certificate key file (file containing server 
    // certificate and private key) specified. If specified, validate the 
    // certificate and the private key, and load them.
    //
    if (strncmp(_certKeyPath, "", 1) != 0)
    {
        //
        // load the specified server certificates
        //

        if (SSL_CTX_use_certificate_file(sslContext,
            _certKeyPath, SSL_FILETYPE_PEM) <=0)
        {
            PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL4,
                "---> SSL: no certificate found in " + String(_certKeyPath));
            PEG_METHOD_EXIT();
            throw( SSLException("Could not get server certificate."));
        }

        //
        // load given private key and check for validity
        //

        if (!_verifyPrivateKey(sslContext, _certKeyPath))
        {
            PEG_METHOD_EXIT();
            throw( SSLException("Could not get private key."));
        }
    }

    PEG_METHOD_EXIT();
    return sslContext;
}

Boolean SSLContextRep::_verifyPrivateKey(SSL_CTX *ctx, const char *keyFilePath)
{
    PEG_METHOD_ENTER(TRC_SSL, "_verifyPrivateKey()");

    if (SSL_CTX_use_PrivateKey_file(ctx, keyFilePath, SSL_FILETYPE_PEM) <= 0)
    {
        PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL4,
            "---> SSL: no private key found in " + String(keyFilePath));
        PEG_METHOD_EXIT();
        return false;
    }

    if (!SSL_CTX_check_private_key(ctx))
    {
        PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL4,
            "---> SSL: Private and public key do not match");
        PEG_METHOD_EXIT();
        return false;
    }

    PEG_METHOD_EXIT();
    return true;
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
                       const String& certKeyPath,
                       SSLCertificateVerifyFunction* verifyCert,
                       const String& randomFile) {}

SSLContextRep::SSLContextRep(const SSLContextRep& sslContextRep) {}

SSLContextRep::~SSLContextRep() {}

SSL_CTX * SSLContextRep::_makeSSLContext() { return 0; }

Boolean SSLContextRep::_verifyPrivateKey(SSL_CTX *ctx,
                                         const char *keyFilePath) { return false; }

SSL_CTX * SSLContextRep::getContext() const { return 0; }

void SSLContextRep::init_ssl() {}

void SSLContextRep::free_ssl() {}

#endif // end of PEGASUS_HAS_SSL

///////////////////////////////////////////////////////////////////////////////
//
// SSLContext
//
///////////////////////////////////////////////////////////////////////////////


SSLContext::SSLContext(
    const String& certPath,
    SSLCertificateVerifyFunction* verifyCert,
    const String& randomFile)
{
    _rep = new SSLContextRep(certPath, String::EMPTY, verifyCert, randomFile);
}

#ifndef PEGASUS_REMOVE_DEPRECATED
SSLContext::SSLContext(
    const String& certPath,
    SSLCertificateVerifyFunction* verifyCert,
    const String& randomFile,
    Boolean isCIMClient)
{
    _rep = new SSLContextRep(certPath, String::EMPTY, verifyCert, randomFile);
}
#endif

SSLContext::SSLContext(
    const String& certPath,
    const String& certKeyPath,
    SSLCertificateVerifyFunction* verifyCert,
    const String& randomFile)
{
    _rep = new SSLContextRep(certPath, certKeyPath, verifyCert, randomFile);
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
    const int errorCode,
    const int respCode)
{
    _rep = new SSLCertificateInfoRep();
    _rep->subjectName = subjectName;
    _rep->issuerName = issuerName;
    _rep->errorDepth = errorDepth;
    _rep->errorCode = errorCode;
    _rep->respCode = respCode;
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

int SSLCertificateInfo::getResponseCode() const
{
    return (_rep->respCode);
}

void SSLCertificateInfo::setResponseCode(const int respCode)
{
    _rep->respCode = respCode;
}

PEGASUS_NAMESPACE_END

