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
//              Sushma Fernandes, Hewlett-Packard Company (sushma_fernandes@hp.com)
//              Heather Sterling, IBM (hsterl@us.ibm.com)
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
    SSL    *ssl;
    int    verifyError = X509_V_OK;

    //
    // get the verification callback info specific to each SSL connection
    //
    ssl = (SSL*) X509_STORE_CTX_get_ex_data(ctx, SSL_get_ex_data_X509_STORE_CTX_idx());
    SSLCallbackInfo* exData = (SSLCallbackInfo*) SSL_get_ex_data(ssl, SSL_CALLBACK_INDEX);

    //
    // If the SSLContext does not have an additional callback
    // simply return the preverification error.
    // We do not need to go through the additional steps.
    //
    if (exData->verifyCertificateCallback == NULL)
    {
        return (preVerifyOk);
    }

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

//    SSLCertificateInfo certInfo(subjectName, issuerName, version, serialNumber,
//        notBefore, notAfter, depth, errorCode, errorStr, preVerifyOk);

    exData->_peerCertificate = new SSLCertificateInfo(subjectName, issuerName, version, serialNumber,
        notBefore, notAfter, depth, errorCode, errorStr, preVerifyOk);

    //
    // Call the application callback.
    // Note that the verification result does not automatically get set to X509_V_OK if the callback is successful.
    // This is because OpenSSL retains the original default error in case we want to use it later.
    // To set the error, we could use X509_STORE_CTX_set_error(ctx, verifyError); but there is no real benefit to doing that here.
    //
    if (exData->verifyCertificateCallback(*exData->_peerCertificate))//certInfo)) 
    {
        Tracer::trace(TRC_SSL, Tracer::LEVEL4,
            "--> SSL: verifyCertificateCallback() returned X509_V_OK");

        PEG_METHOD_EXIT();
        return 1;
    }
    else // verification failed, handshake will be immediately terminated
    {
        Tracer::trace(TRC_SSL, Tracer::LEVEL4,
            "--> SSL: verifyCertificateCallback() returned error %d", exData->_peerCertificate->getErrorCode());
      
        PEG_METHOD_EXIT();
        return 0; 
    }
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
        /*Tracer::trace(TRC_SSL, Tracer::LEVEL4,
                "Now locking for %d", pegasus_thread_self());*/
        SSLContextRep::_sslLocks[type].lock( pegasus_thread_self() );
    }
    else
    {
        /*Tracer::trace(TRC_SSL, Tracer::LEVEL4,
                "Now unlocking for %d", pegasus_thread_self());*/
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
SSLContextRep::SSLContextRep(
                       const String& trustStore,
                       const String& certPath,
                       const String& keyPath,
                       SSLCertificateVerifyFunction* verifyCert,
                       Boolean trustStoreAutoUpdate,
					   String trustStoreUserName,
                       const String& randomFile)
{
    PEG_METHOD_ENTER(TRC_SSL, "SSLContextRep::SSLContextRep()");

    _trustStore = trustStore;

    _certPath = certPath;

    _keyPath = keyPath;

    _certificateVerifyFunction = verifyCert;

    _trustStoreAutoUpdate = trustStoreAutoUpdate;

	_trustStoreUserName = trustStoreUserName;

    //
    // If a truststore and/or peer verification function is specified, enable peer verification
    //
    if (trustStore != String::EMPTY || verifyCert != NULL)
    {
        _verifyPeer = true;
    } 
    else
    {
        _verifyPeer = false;
    }

    //
    // Initialiaze SSL callbacks and increment the SSLContextRep object _counter.
    //
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

    _trustStore = sslContextRep._trustStore;
    _certPath = sslContextRep._certPath;
    _keyPath = sslContextRep._keyPath;
    _verifyPeer = sslContextRep._verifyPeer;
    _trustStoreAutoUpdate = sslContextRep._trustStoreAutoUpdate;
	_trustStoreUserName = sslContextRep._trustStoreUserName;
    _certificateVerifyFunction = sslContextRep._certificateVerifyFunction;
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

    if (_verifyPeer)
    {
        //ATTN: We might still need a flag to specify SSL_VERIFY_FAIL_IF_NO_PEER_CERT
        // If SSL_VERIFY_FAIL_IF_NO_PEER_CERT is ON, SSL will immediately be terminated 
        // if the client sends no certificate or sends an untrusted certificate.  The 
        // callback function is not called in this case; the handshake is simply terminated.
        // This value has NO effect in from a client perspective

        if (_certificateVerifyFunction != NULL)
        {
            PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL3, 
                "---> SSL: certificate verification callback specified");
            SSL_CTX_set_verify(sslContext, 
                SSL_VERIFY_PEER | SSL_VERIFY_CLIENT_ONCE, prepareForCallback);
        }
        else
        {
            PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL3, "---> SSL: Trust Store specified");
            SSL_CTX_set_verify(sslContext, 
                SSL_VERIFY_PEER | SSL_VERIFY_CLIENT_ONCE | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, 
                prepareForCallback);
        }
    }
    else
    {
        PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL3,
            "---> SSL: Trust Store and certificate verification callback are NOT specified");
        SSL_CTX_set_verify(sslContext, SSL_VERIFY_NONE, NULL);
    }

    //
    // Check if there is CA certificate file or directory specified. If specified,
    // and is not empty, load the certificates from the Trust store.
    //
    if (_trustStore != String::EMPTY)
    {
        //
        // The truststore may be a single file of CA certificates OR
        // a directory containing multiple CA certificates.
        // Check which one it is, and call the load_verify_locations function 
        // with the appropriate parameter.  Note: It is possible to have both
        // options, in which case the CA file takes precedence over the CA path.  
        // However, since there is currently only one trust parameter to the
        // SSL functions, only allow one choice here.  
        //
        if (FileSystem::isDirectory(_trustStore))
        {
            PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL3, 
                            "---> SSL: Truststore is a directory");
            //
            // load certificates from the trust store
            //
            PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL2,
                "---> SSL: Loading certificates from the trust store: " + _trustStore);

            if ((!SSL_CTX_load_verify_locations(sslContext, NULL, _trustStore.getCString())) ||
                (!SSL_CTX_set_default_verify_paths(sslContext)))
            {
                PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL2,
                    "---> SSL: Could not load certificates from the trust store: " + _trustStore);
                //l10n
                MessageLoaderParms parms("Common.SSLContext.COULD_NOT_LOAD_CERTIFICATES",
                    "Could not load certificates in to trust store.");
                PEG_METHOD_EXIT();
                throw SSLException(parms);
            }

        } 
        else if (FileSystem::exists(_trustStore))
        {
            PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL3, 
                            "---> SSL: Truststore is a file");
            //
            // Get size of the trust store file:
            //
            Uint32 fileSize = 0;

            FileSystem::getFileSize(_trustStore, fileSize);

            if (fileSize > 0)
            {
                //
                // load certificates from the trust store
                //
                PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL2,
                    "---> SSL: Loading certificates from the trust store: " + _trustStore);

                if ((!SSL_CTX_load_verify_locations(sslContext, _trustStore.getCString(), NULL)) ||
                    (!SSL_CTX_set_default_verify_paths(sslContext)))
                {
                    PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL2,
                        "---> SSL: Could not load certificates from the trust store: " + _trustStore);
                    //l10n
                    MessageLoaderParms parms("Common.SSLContext.COULD_NOT_LOAD_CERTIFICATES",
                        "Could not load certificates in to trust store.");
                    PEG_METHOD_EXIT();
                    throw SSLException(parms);
                }
            }
            else
            {
                //
                // no certificates found in the trust store
                //
                PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL2,
                    "---> SSL: No certificates to load from the trust store: " + _trustStore);
            }
        }
    }

    Boolean keyLoaded = false;

    //
    // Check if there is a certificate file (file containing server 
    // certificate) specified. If specified, validate and load the 
    // certificate.
    //
    if (_certPath != String::EMPTY)
    {
        //
        // load the specified server certificates
        //
        PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL2,
            "---> SSL: Loading server certificate from: " + _certPath);

        if (SSL_CTX_use_certificate_file(sslContext, 
            _certPath.getCString(), SSL_FILETYPE_PEM) <=0)
        {
            PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL2,
                "---> SSL: No server certificate found in " + _certPath);
            MessageLoaderParms parms("Common.SSLContext.COULD_NOT_GET_SERVER_CERTIFICATE",
            					     "Could not get server certificate.");
            PEG_METHOD_EXIT();
            throw SSLException(parms);
        }

        //
        // If there is no key file (file containing server
        // private key) specified, then try loading the key from the certificate file.
        // As of 2.4, if a keyfile is specified, its location is verified during server
        // startup and will throw an error if the path is invalid.
        //
        if (_keyPath == String::EMPTY)
        {
            PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL2,
                "---> SSL: loading private key from: " + _certPath);
            //
            // load the private key and check for validity
            //
            if (!_verifyPrivateKey(sslContext, _certPath))
            {
                MessageLoaderParms parms("Common.SSLContext.COULD_NOT_GET_PRIVATE_KEY",
            					         "Could not get private key.");
                PEG_METHOD_EXIT();
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
    if (_keyPath != String::EMPTY && !keyLoaded)
    {
        PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL2,
            "---> SSL: loading private key from: " + _keyPath);
        //
        // load given private key and check for validity
        //
        if (!_verifyPrivateKey(sslContext, _keyPath))
        {
            MessageLoaderParms parms("Common.SSLContext.COULD_NOT_GET_PRIVATE_KEY",
            					         "Could not get private key.");
            PEG_METHOD_EXIT();
            throw SSLException(parms);
        }
        keyLoaded = true;
    }

    PEG_METHOD_EXIT();
    return sslContext;
}

Boolean SSLContextRep::_verifyPrivateKey(SSL_CTX *ctx, const String& keyPath)
{
    PEG_METHOD_ENTER(TRC_SSL, "_verifyPrivateKey()");

    if (SSL_CTX_use_PrivateKey_file(ctx, keyPath.getCString(), SSL_FILETYPE_PEM) <= 0)
    {
        PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL2,
            "---> SSL: no private key found in " + String(keyPath));
        PEG_METHOD_EXIT();
        return false;
    }

    if (!SSL_CTX_check_private_key(ctx))
    {
        PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL2,
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

String SSLContextRep::getTrustStore() const
{
    return _trustStore;
}

String SSLContextRep::getCertPath() const
{
    return _certPath;
}

String SSLContextRep::getKeyPath() const
{
    return _keyPath;
}

Boolean SSLContextRep::isPeerVerificationEnabled() const
{
    return _verifyPeer;
}

Boolean SSLContextRep::isTrustStoreAutoUpdateEnabled() const
{
    return _trustStoreAutoUpdate;
}

String SSLContextRep::getTrustStoreUserName() const
{
	return _trustStoreUserName;
}

SSLCertificateVerifyFunction* SSLContextRep::getSSLCertificateVerifyFunction() const
{
    return _certificateVerifyFunction;
}

#else 

//
// these definitions are used if ssl is not available
//

SSLContextRep::SSLContextRep(const String& trustStore,
                       const String& certPath,
                       const String& keyPath,
                       SSLCertificateVerifyFunction* verifyCert,
                       Boolean trustStoreAutoUpdate,
					   String trustStoreUserName,
                       const String& randomFile) {}

SSLContextRep::SSLContextRep(const SSLContextRep& sslContextRep) {}

SSLContextRep::~SSLContextRep() {}

SSL_CTX * SSLContextRep::_makeSSLContext() { return 0; }

Boolean SSLContextRep::_verifyPrivateKey(SSL_CTX *ctx, 
                                         const String& keyPath) { return false; }

SSL_CTX * SSLContextRep::getContext() const { return 0; }

String SSLContextRep::getTrustStore() const { return String::EMPTY; }

String SSLContextRep::getCertPath() const { return String::EMPTY; }

String SSLContextRep::getKeyPath() const { return String::EMPTY; }

Boolean SSLContextRep::isPeerVerificationEnabled() const { return false; }

Boolean SSLContextRep::isTrustStoreAutoUpdateEnabled() const { return false; }

String SSLContextRep::getTrustStoreUserName() const { return String::EMPTY; }

SSLCertificateVerifyFunction* SSLContextRep::getSSLCertificateVerifyFunction() const { return NULL; }

void SSLContextRep::init_ssl() {}

void SSLContextRep::free_ssl() {}

#endif // end of PEGASUS_HAS_SSL

///////////////////////////////////////////////////////////////////////////////
//
// SSLContext
//
///////////////////////////////////////////////////////////////////////////////


SSLContext::SSLContext(
    const String& trustStore,
    SSLCertificateVerifyFunction* verifyCert,
    const String& randomFile)
{
    _rep = new SSLContextRep(trustStore, String::EMPTY, String::EMPTY,  verifyCert, false, String::EMPTY, randomFile);
}

SSLContext::SSLContext(
    const String& trustStore,
    const String& certPath,
    const String& keyPath,
    SSLCertificateVerifyFunction* verifyCert,
    const String& randomFile)
{
    _rep = new SSLContextRep(trustStore, certPath, keyPath, verifyCert, false, String::EMPTY, randomFile);
}

SSLContext::SSLContext(
        const String& trustStore,
        const String& certPath,
        const String& keyPath,
        SSLCertificateVerifyFunction* verifyCert,
		String trustStoreUserName,
        const String& randomFile)
{
    _rep = new SSLContextRep(trustStore, certPath, keyPath, verifyCert, false, trustStoreUserName, randomFile);
}

#ifdef PEGASUS_USE_AUTOMATIC_TRUSTSTORE_UPDATE
SSLContext::SSLContext(
        const String& trustStore,
        const String& certPath,
        const String& keyPath,
        SSLCertificateVerifyFunction* verifyCert,
        Boolean trustStoreAutoUpdate,
		String trustStoreUserName,
        const String& randomFile)
{
    _rep = new SSLContextRep(trustStore, certPath, keyPath, verifyCert, trustStoreAutoUpdate, trustStoreUserName, randomFile);
}
#endif


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

String SSLContext::getTrustStore() const
{
    return (_rep->getTrustStore());
}

String SSLContext::getCertPath() const
{
    return (_rep->getCertPath());
}

String SSLContext::getKeyPath() const
{
    return (_rep->getKeyPath()); 
}

Boolean SSLContext::isPeerVerificationEnabled() const
{
    return (_rep->isPeerVerificationEnabled());
}

#ifdef PEGASUS_USE_AUTOMATIC_TRUSTSTORE_UPDATE
Boolean SSLContext::isTrustStoreAutoUpdateEnabled() const
{
    return (_rep->isTrustStoreAutoUpdateEnabled());
}
#endif

String SSLContext::getTrustStoreUserName() const
{
	return (_rep->getTrustStoreUserName());
}

SSLCertificateVerifyFunction* SSLContext::getSSLCertificateVerifyFunction() const
{
    return (_rep->getSSLCertificateVerifyFunction());
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
    _rep->notBefore = CIMDateTime();
    _rep->notAfter = CIMDateTime();
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

String SSLCertificateInfo::toString() const
{
    char buf[1024];
    
    String s;

    s.append("Subject Name:\n\t");
    s.append(_rep->subjectName);
    s.append("\n");
    
    s.append("Issuer Name:\n\t");
    s.append(_rep->issuerName);
    s.append("\n");
    
    sprintf(buf, "Depth: %d\n", _rep->depth);
    s.append(buf);
    
    sprintf(buf, "Error code: %d\n", _rep->errorCode);
    s.append(buf);
    
    sprintf(buf, "Response (preverify) code: %d\n", _rep->respCode);
    s.append(buf);

    s.append("Error string: ");
    s.append(_rep->errorString);
    s.append("\n");
    
    sprintf(buf, "Version number: %d\n", _rep->versionNumber);
    s.append(buf);

    sprintf(buf, "Serial number: %ld\n", _rep->serialNumber);
    s.append(buf);
    
    s.append("Not before date: ");
    s.append((_rep->notBefore).toString());
    s.append("\n");

    s.append("Not after date: ");
    s.append((_rep->notAfter).toString());
    s.append("\n");

    return s;
}

SSLCallbackInfo::SSLCallbackInfo(SSLCertificateVerifyFunction* verifyCert)
{
    verifyCertificateCallback = verifyCert;
    _peerCertificate = NULL;  
}

SSLCallbackInfo::~SSLCallbackInfo()
{
    if (_peerCertificate) 
    {
        delete _peerCertificate;
    }
}

PEGASUS_NAMESPACE_END

