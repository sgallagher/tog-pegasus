//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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
#define OPENSSL_NO_KRB5 1
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
#include <time.h>
#include <Pegasus/Common/MessageLoader.h> //l10n

#include "SSLContext.h"
#include "SSLContextRep.h"

typedef struct Timestamp 
{
    char year[4];
    char month[2];
    char day[2];
    char hour[2];
    char minutes[2];
    char seconds[2];
    char dot;
    char microSeconds[6];
    char plusOrMinus;
    char utcOffset[3];
    char padding[3];
} Timestamp_t;

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


//
// Convert ASN1_UTCTIME to CIMDateTime
//
CIMDateTime getDateTime(const ASN1_UTCTIME *utcTime)
{
    struct tm time;
    int offset;
    Timestamp_t timeStamp;
    char tempString[80];
    char plusOrMinus = '+';

    memset(&time, '\0', sizeof(time));

#define g2(p) ( ( (p)[0] - '0' ) * 10 + (p)[1] - '0' )

    time.tm_year = g2(utcTime->data);

    if(time.tm_year < 50)
    {
        time.tm_year += 100;
    }
    time.tm_mon = g2(utcTime->data + 2) - 1;
    time.tm_mday = g2(utcTime->data + 4);
    time.tm_hour = g2(utcTime->data + 6);
    time.tm_min = g2(utcTime->data + 8);
    time.tm_sec = g2(utcTime->data + 10);

    if(utcTime->data[12] == 'Z')
    {
        offset = 0;
    }
    else
    {
        offset = g2(utcTime->data + 13) * 60 + g2(utcTime->data + 15);
        if(utcTime->data[12] == '-')
        {
            plusOrMinus = '-';
        }
    }
#undef g2

    int year = 1900;
    memset((void *)&timeStamp, 0, sizeof(Timestamp_t));

    // Format the date.
    sprintf((char *) &timeStamp,"%04d%02d%02d%02d%02d%02d.%06d%04d",
            year + time.tm_year,
            time.tm_mon + 1,  
            time.tm_mday,
            time.tm_hour,
            time.tm_min,
            time.tm_sec,
            0,
            offset);

    timeStamp.plusOrMinus = plusOrMinus;

    CIMDateTime dateTime;

    dateTime.clear();
    strcpy(tempString, (char *)&timeStamp);
    dateTime.set(tempString);

    return (dateTime);
}

//
// Callback function that is called by the OpenSSL library. This function
// extracts X509 certficate information and pass that on to client application
// callback function.
//
int prepareForCallback(int preVerifyOk, X509_STORE_CTX *ctx)
{
    PEG_METHOD_ENTER(TRC_SSL, "prepareForCallback()");

    char   buf[256];
    X509   *currentCert;
    int    verifyError = X509_V_OK;

    //
    // get the current certificate
    //
    currentCert = X509_STORE_CTX_get_current_cert(ctx);

    //
    // get the default verification error code
    //
    int errorCode = X509_STORE_CTX_get_error(ctx);

    //
    // get the depth of certificate chain
    //
    int depth = X509_STORE_CTX_get_error_depth(ctx);

    //FUTURE: Not sure what to do with these...?
    //ssl = X509_STORE_CTX_get_ex_data(ctx, SSL_get_ex_data_X509_STORE_CTX_idx());
    //mydata = SSL_get_ex_data(ssl, mydata_index);

    //
    // get the version on the certificate
    //
    long version = X509_get_version(currentCert);

    //
    // get the serial number of the certificate
    //
    long serialNumber = ASN1_INTEGER_get(X509_get_serialNumber(currentCert));

    //
    // get the validity of the certificate
    //
    CIMDateTime notBefore = getDateTime(X509_get_notBefore(currentCert));

    CIMDateTime notAfter = getDateTime(X509_get_notAfter(currentCert));

    //
    // get the subject name on the certificate
    //
    X509_NAME_oneline(X509_get_subject_name(currentCert), buf, 256);
    String subjectName = String(buf);

    //
    // get the default verification error string 
    //
    String errorStr = String(X509_verify_cert_error_string(errorCode));

    //
    // log the error string if the default verification was failed
    //
    if (!preVerifyOk)
    {
        PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL4, 
            "---> SSL: certificate default verification error: " + errorStr);
    }

    //
    // get the issuer name on the certificate
    //
    if (!preVerifyOk && (errorCode == X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT))
    {
        X509_NAME_oneline(X509_get_issuer_name(ctx->current_cert), buf, 256);
    }
    else
    {
        X509_NAME_oneline(X509_get_issuer_name(currentCert), buf, 256);
    }
    String issuerName = String(buf);

    //
    // Call the verify_certificate() application callback
    //
    SSLCertificateInfo certInfo(subjectName, issuerName, version, serialNumber,
        notBefore, notAfter, depth, errorCode, errorStr, preVerifyOk);

    if (verify_certificate(certInfo))
    {
        verifyError = X509_V_OK;
        preVerifyOk = 1;
        Tracer::trace(TRC_SSL, Tracer::LEVEL4,
            "--> SSL: verify_certificate() returned X509_V_OK");
    }
    else
    {
        verifyError = certInfo.getErrorCode();
        preVerifyOk = 0;
        Tracer::trace(TRC_SSL, Tracer::LEVEL4,
            "--> SSL: verify_certificate() returned error %d", verifyError);
    }

    //
    // Reset the error. It is logically not required to reset the error code, but
    // openSSL code does not take just the return value on a failed certificate
    // verification.
    //
    X509_STORE_CTX_set_error(ctx, verifyError);

    PEG_METHOD_EXIT();

    return(preVerifyOk);
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
SSLContextRep::SSLContextRep(const String& trustPath,
                       const String& certPath,
                       const String& keyPath,
                       SSLCertificateVerifyFunction* verifyCert,
                       const String& randomFile)
{
    PEG_METHOD_ENTER(TRC_SSL, "SSLContextRep::SSLContextRep()");

    _trustPath = trustPath.getCString();

    _certPath = certPath.getCString();

    _keyPath = keyPath.getCString();

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

    _trustPath = sslContextRep._trustPath;
    _certPath = sslContextRep._certPath;
    _keyPath = sslContextRep._keyPath;
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
            //l10n
            //throw( SSLException("Random seed file required"));
            MessageLoaderParms parms("Common.SSLContext.RANDOM_SEED_FILE_REQUIRED",
            						 "Random seed file required");
            throw SSLException(parms);						 
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
                //l10n
		// do not put in $0 in default message, but pass in filename for bundle message
                //throw( SSLException("Not enough seed data in random seed file."));
                MessageLoaderParms parms("Common.SSLContext.NOT_ENOUGH_SEED_DATA_IN_FILE",
            						     "Not enough seed data in random seed file.",
            						     randomFile);
            	throw SSLException(parms);
            }
        }
        else
        {
            PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL4,
                "seed file - " + randomFile + " does not exist.");
            PEG_METHOD_EXIT();
            //l10n
            //throw( SSLException("Seed file '" + randomFile + "' does not exist."));
            MessageLoaderParms parms("Common.SSLContext.SEED_FILE_DOES_NOT_EXIST",
            						 "Seed file '$0' does not exist.",
            						 randomFile);
            throw SSLException(parms);
        }

        if ( RAND_status() == 0 )
        {
            //
            // Try to do more seeding
            //
            long seedNumber;
        #if defined(PEGASUS_PLATFORM_WIN32_IX86_MSVC)
            srand((unsigned int)time(NULL)); // Initialize
            seedNumber = rand();
        #else
            srandom((unsigned int)time(NULL)); // Initialize
            seedNumber = random();
        #endif
            RAND_seed((unsigned char *) &seedNumber, sizeof(seedNumber));

            int  seedRet = RAND_status();
            if ( seedRet == 0 )
            {
                PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL4,
                    "Not enough seed data in random seed file, RAND_status = " +
                    seedRet);
                PEG_METHOD_EXIT();
                //l10n 485
		// do not put in $0 in default message, but pass in filename for bundle message
                //throw( SSLException("Not enough seed data in random seed file."));
                MessageLoaderParms parms("Common.SSLContext.NOT_ENOUGH_SEED_DATA_IN_FILE",
            						     "Not enough seed data in random seed file.",
            						     randomFile);
            	throw SSLException(parms);
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
        //l10n
        //throw( SSLException("Not enough seed data."));
        MessageLoaderParms parms("Common.SSLContext.NOT_ENOUGH_SEED_DATA",
            					 "Not enough seed data.");
        throw SSLException(parms);
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
        //l10n
        //throw( SSLException("Could not get SSL CTX"));
        MessageLoaderParms parms("Common.SSLContext.COULD_NOT_GET",
            					 "Could not get SSL CTX");
        throw SSLException(parms);
    }

#ifdef PEGASUS_SSL_WEAKENCRYPTION
    if (!(SSL_CTX_set_cipher_list(sslContext, SSL_TXT_EXP40))){
    	//l10n
        //throw( SSLException("Could not set the cipher list"));
        MessageLoaderParms parms("Common.SSLContext.COULD_NOT_SET_CIPHER_LIST",
            					 "Could not set the cipher list");
        throw SSLException(parms);
    }
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
    if (strncmp(_trustPath, "", 1) != 0)
    {
        //
        // load certificates in to trust store
        //

        if ((!SSL_CTX_load_verify_locations(sslContext, _trustPath, NULL)) ||
            (!SSL_CTX_set_default_verify_paths(sslContext)))
        {
            PEG_METHOD_EXIT();
            //l10n
            //throw( SSLException("Could not load certificates in to trust store."));
            MessageLoaderParms parms("Common.SSLContext.COULD_NOT_LOAD_CERTIFICATES",
            					     "Could not load certificates in to trust store.");
            throw SSLException(parms);
        }
    }

    Boolean keyLoaded = false;
    //
    // Check if there is a certificate file (file containing server 
    // certificate) specified. If specified, validate and load the 
    // certificate.
    //
    if (strncmp(_certPath, "", 1) != 0)
    {
        //
        // load the specified server certificates
        //

        if (SSL_CTX_use_certificate_file(sslContext,
            _certPath, SSL_FILETYPE_PEM) <=0)
        {
            PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL4,
                "---> SSL: no certificate found in " + String(_certPath));
            PEG_METHOD_EXIT();
            //l10n
            //throw( SSLException("Could not get server certificate."));
            MessageLoaderParms parms("Common.SSLContext.COULD_NOT_GET_SERVER_CERTIFICATE",
            					     "Could not get server certificate.");
            throw SSLException(parms);
        }

        //
        // If there is no key file (file containing server
        // private key) specified or the specified file does not exist,
        // then try loading the key from the certificate file.
        //
        if ( strncmp(_keyPath, "", 1) == 0  ||
           !FileSystem::exists(String(_keyPath)) )
        {
            PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL3,
                "---> SSL: loading key from" + String(_certPath));
            //
            // load the private key and check for validity
            //
            if (!_verifyPrivateKey(sslContext, _certPath))
            {
                PEG_METHOD_EXIT();
                //l10n
                //throw( SSLException("Could not get private key."));
                MessageLoaderParms parms("Common.SSLContext.COULD_NOT_GET_PRIVATE_KEY",
            					         "Could not get private key.");
            	throw SSLException(parms);
            }
            keyLoaded = true;
        }
    }

    //
    // Check if there is a key file (file containing server
    // private key) specified and the key was not already loaded.
    // If specified, validate and load the key.
    //
    if (strncmp(_keyPath, "", 1) != 0 && !keyLoaded)
    {
        PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL3,
            "---> SSL: loading key from" + String(_keyPath));
        //
        // load given private key and check for validity
        //
        if (!_verifyPrivateKey(sslContext, _keyPath))
        {
            PEG_METHOD_EXIT();
            //l10n
            //throw( SSLException("Could not get private key."));
            MessageLoaderParms parms("Common.SSLContext.COULD_NOT_GET_PRIVATE_KEY",
            					         "Could not get private key.");
            throw SSLException(parms);
        }
        keyLoaded = true;
    }

    PEG_METHOD_EXIT();
    return sslContext;
}

Boolean SSLContextRep::_verifyPrivateKey(SSL_CTX *ctx, const char *keyPath)
{
    PEG_METHOD_ENTER(TRC_SSL, "_verifyPrivateKey()");

    if (SSL_CTX_use_PrivateKey_file(ctx, keyPath, SSL_FILETYPE_PEM) <= 0)
    {
        PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL4,
            "---> SSL: no private key found in " + String(keyPath));
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

SSLContextRep::SSLContextRep(const String& trustPath,
                       const String& certPath,
                       const String& keyPath,
                       SSLCertificateVerifyFunction* verifyCert,
                       const String& randomFile) {}

SSLContextRep::SSLContextRep(const SSLContextRep& sslContextRep) {}

SSLContextRep::~SSLContextRep() {}

SSL_CTX * SSLContextRep::_makeSSLContext() { return 0; }

Boolean SSLContextRep::_verifyPrivateKey(SSL_CTX *ctx,
                                         const char *keyPath) { return false; }

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
    const String& trustPath,
    SSLCertificateVerifyFunction* verifyCert,
    const String& randomFile)
{
    _rep = new SSLContextRep(trustPath, String::EMPTY, String::EMPTY,  verifyCert, randomFile);
}

#ifndef PEGASUS_REMOVE_DEPRECATED
SSLContext::SSLContext(
    const String& certPath,
    SSLCertificateVerifyFunction* verifyCert,
    const String& randomFile,
    Boolean isCIMClient)
{
    _rep = new SSLContextRep(certPath, String::EMPTY, String::EMPTY,  verifyCert, randomFile);
}

SSLContext::SSLContext(
    const String& certPath,
    const String& certKeyPath,
    SSLCertificateVerifyFunction* verifyCert,
    const String& randomFile)
{
    _rep = new SSLContextRep(certPath, certKeyPath, String::EMPTY, verifyCert, randomFile);
}
#endif

SSLContext::SSLContext(
    const String& trustPath,
    const String& certPath,
    const String& keyPath,
    SSLCertificateVerifyFunction* verifyCert,
    const String& randomFile)
{
    _rep = new SSLContextRep(trustPath, certPath, keyPath, verifyCert, randomFile);
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
//
// Certificate validation result codes.
//
const int    SSLCertificateInfo::V_OK                                       = 0;

const int    SSLCertificateInfo::V_ERR_UNABLE_TO_GET_ISSUER_CERT            = 2;
const int    SSLCertificateInfo::V_ERR_UNABLE_TO_GET_CRL                    = 3;
const int    SSLCertificateInfo::V_ERR_UNABLE_TO_DECRYPT_CERT_SIGNATURE     = 4;
const int    SSLCertificateInfo::V_ERR_UNABLE_TO_DECRYPT_CRL_SIGNATURE      = 5;
const int    SSLCertificateInfo::V_ERR_UNABLE_TO_DECODE_ISSUER_PUBLIC_KEY   = 6;
const int    SSLCertificateInfo::V_ERR_CERT_SIGNATURE_FAILURE               = 7;
const int    SSLCertificateInfo::V_ERR_CRL_SIGNATURE_FAILURE                = 8;
const int    SSLCertificateInfo::V_ERR_CERT_NOT_YET_VALID                   = 9;
const int    SSLCertificateInfo::V_ERR_CERT_HAS_EXPIRED                     = 10;
const int    SSLCertificateInfo::V_ERR_CRL_NOT_YET_VALID                    = 11;
const int    SSLCertificateInfo::V_ERR_CRL_HAS_EXPIRED                      = 12;
const int    SSLCertificateInfo::V_ERR_ERROR_IN_CERT_NOT_BEFORE_FIELD       = 13;
const int    SSLCertificateInfo::V_ERR_ERROR_IN_CERT_NOT_AFTER_FIELD        = 14;
const int    SSLCertificateInfo::V_ERR_ERROR_IN_CRL_LAST_UPDATE_FIELD       = 15;
const int    SSLCertificateInfo::V_ERR_ERROR_IN_CRL_NEXT_UPDATE_FIELD       = 16;
const int    SSLCertificateInfo::V_ERR_OUT_OF_MEM                           = 17;
const int    SSLCertificateInfo::V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT          = 18;
const int    SSLCertificateInfo::V_ERR_SELF_SIGNED_CERT_IN_CHAIN            = 19;
const int    SSLCertificateInfo::V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY    = 20;
const int    SSLCertificateInfo::V_ERR_UNABLE_TO_VERIFY_LEAF_SIGNATURE      = 21;
const int    SSLCertificateInfo::V_ERR_CERT_CHAIN_TOO_LONG                  = 22;
const int    SSLCertificateInfo::V_ERR_CERT_REVOKED                         = 23;
const int    SSLCertificateInfo::V_ERR_INVALID_CA                           = 24;
const int    SSLCertificateInfo::V_ERR_PATH_LENGTH_EXCEEDED                 = 25;
const int    SSLCertificateInfo::V_ERR_INVALID_PURPOSE                      = 26;
const int    SSLCertificateInfo::V_ERR_CERT_UNTRUSTED                       = 27;
const int    SSLCertificateInfo::V_ERR_CERT_REJECTED                        = 28;
const int    SSLCertificateInfo::V_ERR_SUBJECT_ISSUER_MISMATCH              = 29;
const int    SSLCertificateInfo::V_ERR_AKID_SKID_MISMATCH                   = 30;
const int    SSLCertificateInfo::V_ERR_AKID_ISSUER_SERIAL_MISMATCH          = 31;
const int    SSLCertificateInfo::V_ERR_KEYUSAGE_NO_CERTSIGN                 = 32;

const int    SSLCertificateInfo::V_ERR_APPLICATION_VERIFICATION             = 50;

class SSLCertificateInfoRep
{
public:
    String    subjectName;
    String    issuerName;
    Uint32    depth;
    Uint32    errorCode;
    Uint32    respCode;
    String    errorString;
    Uint32    versionNumber;
    long      serialNumber;
    CIMDateTime    notBefore;
    CIMDateTime    notAfter;
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
    _rep->versionNumber = 0;
    _rep->serialNumber = 0;
    _rep->notBefore = CIMDateTime(String::EMPTY);
    _rep->notAfter = CIMDateTime(String::EMPTY);
    _rep->depth = errorDepth;
    _rep->errorCode = errorCode;
    _rep->errorString = String::EMPTY;
    _rep->respCode = respCode;
}

SSLCertificateInfo::SSLCertificateInfo(
    const String subjectName,
    const String issuerName,
    const Uint32 versionNumber,
    const long serialNumber,
    const CIMDateTime notBefore,
    const CIMDateTime notAfter,
    const Uint32 depth,
    const Uint32 errorCode,
    const String errorString,
    const Uint32 respCode)
{
    _rep = new SSLCertificateInfoRep();
    _rep->subjectName = subjectName;
    _rep->issuerName = issuerName;
    _rep->versionNumber = versionNumber;
    _rep->serialNumber = serialNumber;
    _rep->notBefore = notBefore;
    _rep->notAfter = notAfter;
    _rep->depth = depth;
    _rep->errorCode = errorCode;
    _rep->errorString = errorString;
    _rep->respCode = respCode;
}

SSLCertificateInfo::SSLCertificateInfo(
    const SSLCertificateInfo& certificateInfo)
{
    _rep = new SSLCertificateInfoRep();
    _rep->subjectName = certificateInfo._rep->subjectName;
    _rep->issuerName = certificateInfo._rep->issuerName;
    _rep->versionNumber = certificateInfo._rep->versionNumber;
    _rep->serialNumber = certificateInfo._rep->serialNumber;
    _rep->notBefore = certificateInfo._rep->notBefore;
    _rep->notAfter = certificateInfo._rep->notAfter;
    _rep->depth = certificateInfo._rep->depth;
    _rep->errorCode = certificateInfo._rep->errorCode;
    _rep->errorString = certificateInfo._rep->errorString;
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

Uint32 SSLCertificateInfo::getVersionNumber() const
{
    return (_rep->versionNumber);
}

long SSLCertificateInfo::getSerialNumber() const
{
    return (_rep->serialNumber);
}

CIMDateTime SSLCertificateInfo::getNotBefore() const
{
    return (_rep->notBefore);
}

CIMDateTime SSLCertificateInfo::getNotAfter() const 
{
    return (_rep->notAfter);
}

Uint32 SSLCertificateInfo::getErrorDepth() const
{
    return (_rep->depth);
}

Uint32 SSLCertificateInfo::getErrorCode()  const
{
    return (_rep->errorCode);
}

void SSLCertificateInfo::setErrorCode(const int errorCode)
{
    _rep->errorCode = errorCode;
}

String SSLCertificateInfo::getErrorString() const
{
    return (_rep->errorString);
}

Uint32 SSLCertificateInfo::getResponseCode()  const
{
    return (_rep->respCode);
}

void SSLCertificateInfo::setResponseCode(const int respCode)
{
    _rep->respCode = respCode;
}

PEGASUS_NAMESPACE_END

