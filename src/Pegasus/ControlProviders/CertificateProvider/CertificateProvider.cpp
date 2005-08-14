//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
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
// Author: Heather Sterling (hsterl@us.ibm.com), PEP187
//
// Modified By:
//         Nag Boranna, Hewlett-Packard Company (nagaraja_boranna@hp.com)
//
//%////////////////////////////////////////////////////////////////////////////

#include "CertificateProvider.h"

#define OPENSSL_NO_KRB5 1
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <openssl/rand.h>

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/PegasusVersion.h>

#include <cctype>
#include <iostream>

#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/OperationContext.h>
#include <Pegasus/Common/Logger.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/XmlReader.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/XmlParser.h>

#ifdef PEGASUS_OS_OS400
#include <qycmutilu2.H>
#include "OS400ConvertChar.h"
#endif

#include <stdlib.h>

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

//PG_SSLCertificate property names
static const CIMName ISSUER_NAME_PROPERTY           = "IssuerName";
static const CIMName SERIAL_NUMBER_PROPERTY         = "SerialNumber";
static const CIMName SUBJECT_NAME_PROPERTY          = "SubjectName";
static const CIMName USER_NAME_PROPERTY             = "RegisteredUserName";
static const CIMName TRUSTSTORE_TYPE_PROPERTY       = "TruststoreType";
static const CIMName FILE_NAME_PROPERTY             = "TruststorePath";
static const CIMName NOT_BEFORE_PROPERTY            = "NotBefore";
static const CIMName NOT_AFTER_PROPERTY             = "NotAfter";

//PG_SSLCertificateRevocationList property names
//also has IssuerName
static const CIMName LAST_UPDATE_PROPERTY           = "LastUpdate";
static const CIMName NEXT_UPDATE_PROPERTY           = "NextUpdate";
static const CIMName REVOKED_SERIAL_NUMBERS_PROPERTY    = "RevokedSerialNumbers";
static const CIMName REVOCATION_DATES_PROPERTY      = "RevocationDates";

//method names for PG_SSLCertificate
static const CIMName METHOD_ADD_CERTIFICATE         = "addCertificate";
static const CIMName PARAMETER_CERT_CONTENTS        = "certificateContents";
static const CIMName PARAMETER_USERNAME             = "userName";
static const CIMName PARAMETER_TRUSTSTORE_TYPE      = "truststoreType";

//method names for PG_SSLCertificateRevocationList
static const CIMName METHOD_ADD_CRL                 = "addCertificateRevocationList";
static const CIMName PARAMETER_CRL_CONTENTS         = "CRLContents";

//truststore and crlstore directory mutexes
static Mutex _trustStoreMutex;
static Mutex _crlStoreMutex;

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

/** Convert ASN1_UTCTIME to CIMDateTime
 */
inline CIMDateTime getDateTime(const ASN1_UTCTIME *utcTime)
{
    struct tm time;
    int offset;
    Timestamp_t timeStamp;
    char tempString[80];
    char plusOrMinus = '+';
    unsigned char* utcTimeData = utcTime->data;

    memset(&time, '\0', sizeof(time));

#define g2(p) ( ( (p)[0] - '0' ) * 10 + (p)[1] - '0' )

    if (utcTime->type == V_ASN1_GENERALIZEDTIME)
    {
        time.tm_year = g2(utcTimeData) * 100;
        utcTimeData += 2;  // Remaining data is equivalent to ASN1_UTCTIME type
        time.tm_year += g2(utcTimeData);
    }
    else
    {
        time.tm_year = g2(utcTimeData);
        if (time.tm_year < 50)
        {
            time.tm_year += 2000;
        }
        else
        {
            time.tm_year += 1900;
        }
    }

    time.tm_mon = g2(utcTimeData + 2) - 1;
    time.tm_mday = g2(utcTimeData + 4);
    time.tm_hour = g2(utcTimeData + 6);
    time.tm_min = g2(utcTimeData + 8);
    time.tm_sec = g2(utcTimeData + 10);

    if (utcTimeData[12] == 'Z')
    {
        offset = 0;
    }
    else
    {
        offset = g2(utcTimeData + 13) * 60 + g2(utcTimeData + 15);
        if (utcTimeData[12] == '-')
        {
            plusOrMinus = '-';
        }
    }
#undef g2

    memset((void *)&timeStamp, 0, sizeof(Timestamp_t));

    // Format the date.
    sprintf((char *) &timeStamp,"%04d%02d%02d%02d%02d%02d.%06d%04d",
            time.tm_year,
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

/**
 * The issuer name should be in the format /type0=value0/type1=value1/type2=...
 * where characters may be escaped by \
 */
inline X509_NAME *getIssuerName(char *issuer, long chtype)
{
	PEG_METHOD_ENTER(TRC_CONTROLPROVIDER, "CertificateProvider::getIssuerName");

	//allocate buffers for type-value pairs
    size_t buflen = strlen(issuer)+1; 
    char *buf = (char*) malloc(buflen);
    size_t maxPairs = buflen / 2 + 1; 
    char **types = (char**) malloc(maxPairs * sizeof (char *));  //types
    char **values = (char**) malloc(maxPairs * sizeof (char *)); //values

    if (!buf || !types || !values)
    {
        return NULL;
    }

    char *sp = issuer, *bp = buf;
	int count = 0;

    while (*sp)
    {
        PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4, "CertificateProvider::getIssuerName WHILE");

		if (*sp != '/') 
		{
			break;
		}
		sp++;

        types[count] = bp;
        while (*sp)
        {
            if (*sp == '\\')
            {
                if (*++sp)
                {
                    *bp++ = *sp++;
                }

            } else if (*sp == '=')
            {
                sp++;
                *bp++ = '\0';
                break;
            } else
            {
                *bp++ = *sp++;
            }
        }

        values[count] = bp;
        while (*sp)
        {
            if (*sp == '\\')
            {
                if (*++sp)
                {
                    *bp++ = *sp++;
                }
            }
            else if (*sp == '/')
            {
                break;
            }
            else
            {
                *bp++ = *sp++;
            }
        }
        *bp++ = '\0';
        count++;
    }


    PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4, "CertificateProvider::getIssuerName  WHILE EXIT");

	//create the issuername object and add each type/value pair
	X509_NAME* issuerNameNew = X509_NAME_new();
	int nid;

    for (int i = 0; i < count; i++)
    {
		nid = OBJ_txt2nid(types[i]);

		//if we don't recognize the name element or there is no corresponding value, continue to the next one
		if (nid == NID_undef || !*values[i]) 
		{
            continue;
        }

        if (!X509_NAME_add_entry_by_NID(issuerNameNew, nid, chtype, (unsigned char*)values[i], -1, -1, 0))
		{
			X509_NAME_free(issuerNameNew);
			issuerNameNew = NULL;
			break;
		}
    }

    free(types);
    free(values);
    free(buf);

    PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4, "Got issuerName successfully");
	PEG_METHOD_EXIT();

    return issuerNameNew;
}

/** Determines whether the user has sufficient access to perform a certificate operation.
  */
Boolean CertificateProvider::_verifyAuthorization(const String& userName)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER, "CertificateProvider::_verifyAuthorization");

    if (_enableAuthentication) 
    {
#if !defined(PEGASUS_OS_OS400)
        if (!System::isPrivilegedUser(userName)) 
#else
        CString user = userName.getCString();
        const char * tmp = (const char *)user;
        AtoE((char *)tmp);
        if (!ycmCheckUserSecurityAuthorities(tmp))
#endif
        {
            PEG_METHOD_EXIT();
            return false;
        }
    }

    PEG_METHOD_EXIT();
    return true;
}

/** Constructor
 */
CertificateProvider::CertificateProvider(CIMRepository* repository,
                                         SSLContextManager* sslContextMgr) :
_cimom(0), 
_repository(repository),
_sslContextMgr(sslContextMgr),
_enableAuthentication(false)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER, "CertificateProvider::CertificateProvider");

    ConfigManager* configManager = ConfigManager::getInstance();

    //get config properties
    if (String::equalNoCase(configManager->getCurrentValue("enableAuthentication"), "true")) 
    {
        _enableAuthentication = true;
    }

    _sslTrustStore = ConfigManager::getHomedPath(configManager->getCurrentValue("sslTrustStore"));
    _exportSSLTrustStore = ConfigManager::getHomedPath(configManager->getCurrentValue("exportSSLTrustStore"));
    _crlStore = ConfigManager::getHomedPath(configManager->getCurrentValue("crlStore"));

    PEG_METHOD_EXIT();
}

/** Destructor
 */ 
CertificateProvider::~CertificateProvider(void)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER, "CertificateProvider::~CertificateProvider");
    PEG_METHOD_EXIT();
}

/** Called when a provider is loaded
 */ 
void CertificateProvider::initialize(CIMOMHandle & cimom)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER, "CertificateProvider::initialize");

    // save the cimom handle in case it is needed to service and operation.
    _cimom = &cimom;

    PEG_METHOD_EXIT();
}

/** Called before a provider is unloaded
 */ 
void CertificateProvider::terminate(void)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER, "CertificateProvider::terminate");

    // delete self. this is necessary because the entry point for this object allocated it, and
    // the module is responsible for its memory management.
    delete this;

    PEG_METHOD_EXIT();
}

/** Delivers a single instance to the CIMOM
 */ 
void CertificateProvider::getInstance(
                              const OperationContext & context,
                              const CIMObjectPath & cimObjectPath,
                              const Boolean includeQualifiers,
                              const Boolean includeClassOrigin,
                              const CIMPropertyList & propertyList,
                              InstanceResponseHandler & handler)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER, "CertificateProvider::getInstance");

    //verify authorization
    const IdentityContainer container = context.get(IdentityContainer::NAME);
    if (!_verifyAuthorization(container.getUserName())) 
    {
        MessageLoaderParms parms("ControlProviders.CertificateProvider.MUST_BE_PRIVILEGED_USER",
                                 "Superuser authority is required to run this CIM operation.");
        throw CIMException(CIM_ERR_ACCESS_DENIED, parms);
    }

    CIMName className(cimObjectPath.getClassName());

    //verify classname
    if (className == PEGASUS_CLASSNAME_CERTIFICATE)
    {
        // process request
        handler.processing();
    
        //verify the keys are set
        //ATTN: do we need to do this, or will the getInstance call handle it?
        Array<CIMKeyBinding> keyBindings = cimObjectPath.getKeyBindings();
        String keyName;
    
        for (Uint32 i=0; i < keyBindings.size(); i++)
        {
            keyName = keyBindings[i].getName().getString();
    
            if (!String::equal(keyName, ISSUER_NAME_PROPERTY.getString()) && !String::equal(keyName, SERIAL_NUMBER_PROPERTY.getString())) 
            {
                throw CIMException(CIM_ERR_INVALID_PARAMETER, keyName);
            }
        }
    
        CIMInstance cimInstance = _repository->getInstance(cimObjectPath.getNameSpace(), cimObjectPath);
    
        PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4, "Returning certificate COP " + cimInstance.getPath().toString());
    
        // deliver instance
        handler.deliver(cimInstance);
    
        // complete request
        handler.complete();

    } else if (className == PEGASUS_CLASSNAME_CRL)
    {
        //ATTN: Fill in

    } else
    {
        throw CIMException(CIM_ERR_INVALID_CLASS, className.getString());
    }

    PEG_METHOD_EXIT();
}

/** Builds and returns a PG_SSLCertificateRevocationList from an X509_CRL object
 */ 
inline CIMInstance _getCRLInstance(X509_CRL* xCrl, String host, CIMNamespaceName nameSpace)
{
    char issuerName[1024];
    STACK_OF(X509_REVOKED) *revoked = NULL;
    X509_REVOKED *r = NULL;
    int numRevoked = -1;
    long rawSerialNumber;
    char serial[1024];
    CIMDateTime revocationDate;

    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER, "CertificateProvider::_getCRLInstance");

    // build instance
    CIMInstance cimInstance(PEGASUS_CLASSNAME_CRL);

    // CA issuer name
    sprintf(issuerName, "%s", X509_NAME_oneline(X509_CRL_get_issuer(xCrl), NULL, 0));
    cimInstance.addProperty(CIMProperty(ISSUER_NAME_PROPERTY, CIMValue(String(issuerName))));

    // validity dates
    CIMDateTime lastUpdate = getDateTime(X509_CRL_get_lastUpdate(xCrl));
    cimInstance.addProperty(CIMProperty(NEXT_UPDATE_PROPERTY, CIMValue(lastUpdate)));

    CIMDateTime nextUpdate = getDateTime(X509_CRL_get_nextUpdate(xCrl));
    cimInstance.addProperty(CIMProperty(LAST_UPDATE_PROPERTY, CIMValue(nextUpdate)));

    Array<String> revokedSerialNumbers;
    Array<CIMDateTime> revocationDates;

    // get revoked certificate information
    revoked = X509_CRL_get_REVOKED(xCrl);
    numRevoked = sk_X509_REVOKED_num(revoked);

    for (int i = 0; i < numRevoked; i++) 
    {
        r = sk_X509_REVOKED_value(revoked, i);
        rawSerialNumber = ASN1_INTEGER_get(r->serialNumber);
        sprintf(serial, "%lu", rawSerialNumber);
        revokedSerialNumbers.append(String(serial));

        revocationDate = getDateTime(r->revocationDate);
        revocationDates.append(revocationDate);
    }

    cimInstance.addProperty(CIMProperty(REVOKED_SERIAL_NUMBERS_PROPERTY, CIMValue(revokedSerialNumbers)));
    cimInstance.addProperty(CIMProperty(REVOCATION_DATES_PROPERTY, CIMValue(revocationDates)));

     // set keys
    Array<CIMKeyBinding> keys;
    CIMKeyBinding key;
    key.setName(ISSUER_NAME_PROPERTY.getString());
    key.setValue(issuerName);
    key.setType(CIMKeyBinding::STRING);
    keys.append(key);

    // set object path for instance
    cimInstance.setPath(CIMObjectPath(host, nameSpace, PEGASUS_CLASSNAME_CRL, keys));

    PEG_METHOD_EXIT();

    return (cimInstance);
}

/** Delivers the complete collection of instances to the CIMOM
 */ 
void CertificateProvider::enumerateInstances(
                                     const OperationContext & context,
                                     const CIMObjectPath & cimObjectPath,
                                     const Boolean includeQualifiers,
                                     const Boolean includeClassOrigin,
                                     const CIMPropertyList & propertyList,
                                     InstanceResponseHandler & handler)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER, "CertificateProvider::enumerateInstances");

    //verify authorization
    const IdentityContainer container = context.get(IdentityContainer::NAME);
    if (!_verifyAuthorization(container.getUserName())) 
    {
        MessageLoaderParms parms("ControlProviders.CertificateProvider.MUST_BE_PRIVILEGED_USER",
                                 "Superuser authority is required to run this CIM operation.");
        throw CIMException(CIM_ERR_ACCESS_DENIED, parms);
    }

    CIMName className(cimObjectPath.getClassName());

    //verify classname
    if (className == PEGASUS_CLASSNAME_CERTIFICATE)
    {
        // process request
        handler.processing();
    
        // get instances from the repository
        Array<CIMInstance> cimInstances;
        cimInstances = _repository->enumerateInstances(cimObjectPath.getNameSpace(), PEGASUS_CLASSNAME_CERTIFICATE);
    
        for (Uint32 i = 0, n = cimInstances.size(); i < n; i++)
        {
            PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4, "Delivering CIMInstance " + cimInstances[i].getPath().toString());
    
            // deliver each instance
            handler.deliver(cimInstances[i]);
        }
    
        // complete request
        handler.complete();

    } else if (className == PEGASUS_CLASSNAME_CRL)
    {
        // process request
        handler.processing();

        FileSystem::translateSlashes(_crlStore); 

        if (FileSystem::isDirectory(_crlStore) && FileSystem::canWrite(_crlStore))
        {
            Array<String> crlFiles;
            if (FileSystem::getDirectoryContents(_crlStore, crlFiles))
            {
                Uint32 count = crlFiles.size();
                for (Uint32 i = 0; i < count; i++)
                {
                    String filename = crlFiles[i];

                    PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4, "Filename " + filename);

                    //ATTN: Is this a two-way hash?  If so, I don't need to read in the CRL just to determine the issuer name
                    BIO* inFile = BIO_new(BIO_s_file());
                    X509_CRL* xCrl = NULL;
                    char fullPathName[1024];

                    sprintf(fullPathName, "%s/%s", (const char*)_crlStore.getCString(), (const char*)filename.getCString());

                    if (BIO_read_filename(inFile, fullPathName))
                    {
                        PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4, "Successfully read filename");

                         if (PEM_read_bio_X509_CRL(inFile, &xCrl, NULL, NULL))
                         {
                            // build instance
                            CIMInstance cimInstance = _getCRLInstance(xCrl, cimObjectPath.getHost(), cimObjectPath.getNameSpace());
                            
                            PEG_TRACE_STRING(TRC_CONTROLPROVIDER,Tracer::LEVEL4, "Delivering CIMInstance: " + cimInstance.getPath().toString());

                            // deliver instance
                            handler.deliver(cimInstance);
                         }

                    } else
                    {
                        //error
                        PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3, "Error reading CRL file");
                    }

                    BIO_free_all(inFile);

                } //end for
            
                // complete request
                handler.complete();

            } else
            {
                PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3, "Error: Could not read sslCRLStore directory.");
                MessageLoaderParms parms("ControlProviders.CertificateProvider.COULD_NOT_READ_DIRECTORY",
                                         "Cannot read directory $0.", _crlStore);
                throw CIMException(CIM_ERR_FAILED, parms);
            }
        } else
        {
            PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3, "Error: sslCRLStore is not a valid directory.");
            MessageLoaderParms parms("ControlProviders.CertificateProvider.INVALID_DIRECTORY",
                                     "Invalid directory $0.", _crlStore);
            throw CIMException(CIM_ERR_FAILED, parms);
        }
    } else
    {
        throw CIMException(CIM_ERR_INVALID_CLASS, className.getString());
    }

    PEG_METHOD_EXIT();
}

/** Delivers the complete collection of instance names (CIMObjectPaths) to the CIMOM
 */ 
void CertificateProvider::enumerateInstanceNames(
                                         const OperationContext & context,
                                         const CIMObjectPath & cimObjectPath,
                                         ObjectPathResponseHandler & handler)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER, "CertificateProvider::enumerateInstanceNames");

    //verify authorization
    const IdentityContainer container = context.get(IdentityContainer::NAME);
    if (!_verifyAuthorization(container.getUserName())) 
    {
        MessageLoaderParms parms("ControlProviders.CertificateProvider.MUST_BE_PRIVILEGED_USER",
                                 "Superuser authority is required to run this CIM operation.");
        throw CIMException(CIM_ERR_ACCESS_DENIED, parms);
    }

    CIMName className(cimObjectPath.getClassName());

    //verify classname
    if (className == PEGASUS_CLASSNAME_CERTIFICATE)
    {
        // process request
        handler.processing();
    
        Array<CIMObjectPath> instanceNames = _repository->enumerateInstanceNames(cimObjectPath.getNameSpace(), PEGASUS_CLASSNAME_CERTIFICATE);
    
        for (Uint32 i = 0, n = instanceNames.size(); i < n; i++)
        {
            PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4, "Delivering CIMObjectPath: " + instanceNames[i].toString());
    
            // deliver object path
            handler.deliver(instanceNames[i]);
        }
    
        // complete request
        handler.complete();

    } else if (className == PEGASUS_CLASSNAME_CRL)
    {
         // process request
        handler.processing();

        FileSystem::translateSlashes(_crlStore); 
        if (FileSystem::isDirectory(_crlStore) && FileSystem::canWrite(_crlStore))
        {
            Array<String> crlFiles;
            if (FileSystem::getDirectoryContents(_crlStore, crlFiles))
            {
                Uint32 count = crlFiles.size();
                for (Uint32 i = 0; i < count; i++)
                {
                    String filename = crlFiles[i];

                    PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3, "Filename " + filename);

                    CIMObjectPath cimObjectPath;

                    //ATTN: Is this a two-way hash?  If so, I don't need to read in the CRL just to determine the issuer name
                    BIO* inFile = BIO_new(BIO_s_file());
                    X509_CRL* xCrl = NULL;
                    char issuerName[1024];
                    char fullPathName[1024];

                    sprintf(fullPathName, "%s/%s", (const char*)_crlStore.getCString(), (const char*)filename.getCString());

                    if (BIO_read_filename(inFile, fullPathName))
                    {
                        PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3, "Successfully read filename");

                         if (PEM_read_bio_X509_CRL(inFile, &xCrl, NULL, NULL))
                         {
                            PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3, "Successfully read CRL file");
                            sprintf(issuerName, "%s", X509_NAME_oneline(X509_CRL_get_issuer(xCrl), NULL, 0));
    
                            // build object path
                            Array<CIMKeyBinding> keys;
                            CIMKeyBinding key;
                            key.setName(ISSUER_NAME_PROPERTY.getString());
                            key.setValue(issuerName);
                            key.setType(CIMKeyBinding::STRING);
                            keys.append(key);
                        
                            // set object path for instance
                            CIMObjectPath instanceName(cimObjectPath.getHost(), cimObjectPath.getNameSpace(), PEGASUS_CLASSNAME_CRL, keys);
                    
                            PEG_TRACE_STRING(TRC_CONTROLPROVIDER,Tracer::LEVEL4, "Instance Name: " + instanceName.toString());

                            handler.deliver(instanceName);
                         }

                    } else
                    {
                        //error
                        PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3, "Error reading CRL file");
                    }

                    BIO_free_all(inFile);
                } //end for
            
                // complete request
                handler.complete();

            } else
            {
                PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3, "Error: Could not read sslCRLStore directory.");
                MessageLoaderParms parms("ControlProviders.CertificateProvider.COULD_NOT_READ_DIRECTORY",
                                         "Cannot read directory $0.", _crlStore);
                throw CIMException(CIM_ERR_FAILED, parms);
            }
        } else
        {
            PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3, "Error: sslCRLStore is not a valid directory.");
            MessageLoaderParms parms("ControlProviders.CertificateProvider.INVALID_DIRECTORY",
                                     "Invalid directory $0.", _crlStore);
            throw CIMException(CIM_ERR_FAILED, parms);
        }
    } else
    {
        throw CIMException(CIM_ERR_INVALID_CLASS, className.getString());
    } 

    PEG_METHOD_EXIT();
}

/** Not supported.  Use invokeMethod to create a certificate or CRL
  */ 
void CertificateProvider::createInstance(
                                 const OperationContext & context,
                                 const CIMObjectPath & cimObjectPath,
                                 const CIMInstance & cimInstance,
                                 ObjectPathResponseHandler & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED, "CertificateProvider::createInstance");
}

/** Not supported.
  */ 
void CertificateProvider::modifyInstance(
                                 const OperationContext & context,
                                 const CIMObjectPath & cimObjectPath,
                                 const CIMInstance & cimInstance,
                                 const Boolean includeQualifiers,
                                 const CIMPropertyList & propertyList,
                                 ResponseHandler & handler)
{
    throw CIMException(CIM_ERR_NOT_SUPPORTED, "CertificateProvider::modifyInstance");
}

/** Deletes the internal object denoted by the specified CIMObjectPath
 */ 
void CertificateProvider::deleteInstance(
                                 const OperationContext & context,
                                 const CIMObjectPath & cimObjectPath,
                                 ResponseHandler & handler)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER, "CertificateProvider::deleteInstance");

    //verify authorization
    const IdentityContainer container = context.get(IdentityContainer::NAME);
    if (!_verifyAuthorization(container.getUserName())) 
    {
        MessageLoaderParms parms("ControlProviders.CertificateProvider.MUST_BE_PRIVILEGED_USER",
                                 "Superuser authority is required to run this CIM operation.");
        throw CIMException(CIM_ERR_ACCESS_DENIED, parms);
    }

    CIMName className(cimObjectPath.getClassName());

    //verify classname
    if (className == PEGASUS_CLASSNAME_CERTIFICATE)
    {
        // process request
        handler.processing();
    
        String certificateFileName = String::EMPTY;
        String issuerName = String::EMPTY;
        String userName = String::EMPTY;
        Uint16 truststoreType;
        CIMInstance cimInstance;

        try
        {
            cimInstance = _repository->getInstance(cimObjectPath.getNameSpace(), cimObjectPath);

        } catch (Exception& ex)
        {
             PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3, "The certificate does not exist.");
             MessageLoaderParms parms("ControlProviders.CertificateProvider.CERT_DNE",
                                      "The certificate does not exist.");
             throw CIMException(CIM_ERR_NOT_FOUND, parms);
        }

        CIMProperty cimProperty;

        //certificate file name
        cimProperty = cimInstance.getProperty(cimInstance.findProperty(FILE_NAME_PROPERTY));
        cimProperty.getValue().get(certificateFileName);

        //issuer name
        cimProperty = cimInstance.getProperty(cimInstance.findProperty(ISSUER_NAME_PROPERTY));
        cimProperty.getValue().get(issuerName);

        //user name
        cimProperty = cimInstance.getProperty(cimInstance.findProperty(USER_NAME_PROPERTY));
        cimProperty.getValue().get(userName);

        cimProperty = cimInstance.getProperty(cimInstance.findProperty(TRUSTSTORE_TYPE_PROPERTY));
        cimProperty.getValue().get(truststoreType);

        PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4, "Issuer name " + issuerName);
        PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4, "User name " + userName);
        PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4, "Truststore type: " +
                                                      cimProperty.getValue().toString());
        
        AutoMutex lock(_trustStoreMutex);

        if (FileSystem::exists(certificateFileName)) 
        {
            if (FileSystem::removeFile(certificateFileName)) 
            {
                PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3, "Successfully deleted certificate file " + certificateFileName);
                
                // only delete from repository if we successfully deleted it from the truststore, otherwise it is still technically "trusted"
                _repository->deleteInstance(cimObjectPath.getNameSpace(), cimObjectPath);

                //
                // Request SSLContextManager to delete the certificate from the cache
                //
                try
                {
                    switch (truststoreType)
                    {
                        case SERVER_TRUSTSTORE :
                            _sslContextMgr->reloadTrustStore(SSLContextManager::SERVER_CONTEXT);
                            break;

                        case EXPORT_TRUSTSTORE :
                            _sslContextMgr->reloadTrustStore(SSLContextManager::EXPORT_CONTEXT);
                            break;
    
                        default: break;
                    }
                }
                catch (SSLException& ex)
                {
                    PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3, 
                        "Trust store reload failed, " + ex.getMessage());

                    MessageLoaderParms parms("ControlProviders.CertificateProvider.TRUSTSTORE_RELOAD_FAILED",
                        "Trust store reload failed, certificate deletion will not be effective until cimserver restart.");
                    throw CIMException(CIM_ERR_FAILED, parms);
                }

                Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
                            "The certificate registered to $0 from issuer $1 has been deleted from the truststore.",
                            userName,
                            issuerName);

            } else
            {
                 PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3, "Could not delete file.");
                 MessageLoaderParms parms("ControlProviders.CertificateProvider.DELETE_FAILED",
                                          "Could not delete file $0.", certificateFileName);
                 throw CIMException(CIM_ERR_FAILED, parms);
            }
        } else
        {
             PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3, "File does not exist.");
             MessageLoaderParms parms("ControlProviders.CertificateProvider.FILE_DNE",
                                      "File does not exist $0.", certificateFileName);
             throw CIMException(CIM_ERR_FAILED, parms);
        }
    
        // complete request
        handler.complete();

    } else if (className == PEGASUS_CLASSNAME_CRL)
    {
        Array<CIMKeyBinding> keys;
        CIMKeyBinding key;
        String issuerName;

        keys = cimObjectPath.getKeyBindings();
        if (keys.size() && String::equal(keys[0].getName().getString(), ISSUER_NAME_PROPERTY.getString()))
        {
            issuerName = keys[0].getValue();
        }

        PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4, "CRL COP" + cimObjectPath.toString());
        PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4, "Issuer Name " + issuerName);

        //ATTN: it would nice to be able to do this by getting the hash directly from the issuerName
        //unfortunately, there does not seem to be an easy way to achieve this
        //the closest I can get is to add the individual DN components using X509_NAME_add_entry_by_NID
        //which involves a lot of tedious parsing.
        //look in the do_subject method of apps.h for how this is done
        //X509_NAME* name = X509_name_new();

        char issuerChar[1024];
        sprintf(issuerChar, "%s", (const char*) issuerName.getCString());

        X509_NAME* name = getIssuerName(issuerChar, MBSTRING_ASC);

        AutoMutex lock(_crlStoreMutex);

        String crlFileName = _getCRLFileName(_crlStore, X509_NAME_hash(name));
        if (FileSystem::exists(crlFileName)) 
        {
            if (FileSystem::removeFile(crlFileName)) 
            {
                PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3, "Successfully deleted CRL file " + crlFileName);

                //
                // reload the CRL store to refresh the cache
                //
                _sslContextMgr->reloadCRLStore();

                Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
                            "The CRL from issuer $0 has been deleted.",
                            issuerName);

            } else
            {
                PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3, "Could not delete file.");
                 MessageLoaderParms parms("ControlProviders.CertificateProvider.DELETE_FAILED",
                                          "Could not delete file $0.", FileSystem::extractFileName(crlFileName));
                 throw CIMException(CIM_ERR_FAILED, parms);
            }
        } else
        {
             PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3, "File does not exist.");
             MessageLoaderParms parms("ControlProviders.CertificateProvider.FILE_DNE",
                                      "File does not exist $0.", FileSystem::extractFileName(crlFileName));
             throw CIMException(CIM_ERR_FAILED, parms);
        }
   
        X509_NAME_free(name);

    } else
    {
        throw CIMException(CIM_ERR_INVALID_CLASS, className.getString());
    }

    PEG_METHOD_EXIT();
}

/** Returns the CRL filename associated with the hashvalue that represents the issuer name.  
 *  There is only one CRL per issuer so the file name will always end in .r0
 */ 
String CertificateProvider::_getCRLFileName(String crlStore, unsigned long hashVal)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER, "CertificateProvider::_getCRLFileName");

    Uint32 index = 0;

    //The files are looked up by the CA issuer name hash value. 
    //Since only one CRL should exist for a given CA, the extension .r0 is appended to the CA hash
    char hashBuffer[32];
    sprintf(hashBuffer, "%lx", hashVal);

    String hashString = "";
    for (int j = 0; j < 32; j++)
    {
        if (hashBuffer[j] != '\0')
        {
            hashString.append(hashBuffer[j]);
        } else
        {
            break; // end of hash string
        }
    }

    char filename[1024];
    sprintf(filename, "%s/%s.r0", 
            (const char*)crlStore.getCString(),
            (const char*)hashString.getCString());

    PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4, "Searching for files like " + hashString + "in " + crlStore);

    FileSystem::translateSlashes(crlStore); 
    if (FileSystem::isDirectory(crlStore) && FileSystem::canWrite(crlStore))
    {
        if (FileSystem::exists(filename)) 
        {
            //overwrite
            PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3, "CRL already exists, overwriting");

        } else
        {
            //create
            PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3, "CRL does not exist, creating");
        }
    } else
    {
        PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3, "Cannot add CRL to CRL store : CRL directory DNE or does not have write privileges");
        MessageLoaderParms parms("ControlProviders.CertificateProvider.INVALID_DIRECTORY",
                                 "Invalid directory $0.", crlStore);
        throw CIMException(CIM_ERR_FAILED, parms);
    }

    PEG_METHOD_EXIT();

    return (String(filename));
}

/** Returns the new certificate filename for the hashvalue that represents the subject name.
 */ 
String CertificateProvider::_getNewCertificateFileName(String trustStore, unsigned long hashVal) 
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER, "CertificateProvider::_getNewCertificateFileName");

    //The files are looked up by the CA subject name hash value. 
    //If more than one CA certificate with the same name hash value exists, 
    //the extension must be different (e.g. 9d66eef0.0, 9d66eef0.1 etc)
    char hashBuffer[32];
    sprintf(hashBuffer, "%lx", hashVal);

    String hashString = "";
    for (int j = 0; j < 32; j++)
    {
        if (hashBuffer[j] != '\0')
        {
            hashString.append(hashBuffer[j]);
        } else
        {
            break; // end of hash string
        }
    }

    PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL4, "Searching for files like " + hashString);

    Uint32 index = 0;
    FileSystem::translateSlashes(trustStore); 
    if (FileSystem::isDirectory(trustStore) && FileSystem::canWrite(trustStore))
    {
        Array<String> trustedCerts;
        if (FileSystem::getDirectoryContents(trustStore, trustedCerts))
        {
            for (Uint32 i = 0; i < trustedCerts.size(); i++)
            {
                if (String::compare(trustedCerts[i], hashString, hashString.size()) == 0)
                {
                    index++;
                }
            }
        } else
        {
            PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3, "Error: Could not read truststore directory.");
            MessageLoaderParms parms("ControlProviders.CertificateProvider.COULD_NOT_READ_DIRECTORY",
                                     "Cannot read directory $0.", trustStore);
            throw CIMException(CIM_ERR_FAILED, parms);
        }
    } else
    {
        PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3, "Error: sslCRLStore is not a valid directory.");
        MessageLoaderParms parms("ControlProviders.CertificateProvider.INVALID_DIRECTORY",
                                 "Invalid directory $0.", trustStore);
        throw CIMException(CIM_ERR_FAILED, parms);
    }

    char filename[1024];
    sprintf(filename, "%s/%s.%d", 
            (const char*)trustStore.getCString(),
            (const char*)hashString.getCString(), 
            index);

    PEG_METHOD_EXIT();

    return (String(filename));
}

/** Calls an extrinsic method on the class.
 */ 
void CertificateProvider::invokeMethod(
    const OperationContext & context,
    const CIMObjectPath & cimObjectPath,
    const CIMName & methodName,
    const Array<CIMParamValue> & inParams,
    MethodResultResponseHandler & handler)
{
    PEG_METHOD_ENTER(TRC_CONTROLPROVIDER,"CertificateProvider::invokeMethod");

    //verify authorization
    const IdentityContainer container = context.get(IdentityContainer::NAME);
    if (!_verifyAuthorization(container.getUserName())) 
    {
        MessageLoaderParms parms("ControlProviders.CertificateProvider.MUST_BE_PRIVILEGED_USER",
                                 "Superuser authority is required to run this CIM operation.");
        throw CIMException(CIM_ERR_ACCESS_DENIED, parms);
    }

    CIMName className(cimObjectPath.getClassName());

    //verify classname
    if (className == PEGASUS_CLASSNAME_CERTIFICATE)
    {
        // process request
        handler.processing();
    
        if (methodName == METHOD_ADD_CERTIFICATE)
        {
            PEG_TRACE_STRING(TRC_CONTROLPROVIDER,Tracer::LEVEL4, "CertificateProvider::addCertificate()");
    
            String certificateContents = String::EMPTY;
            String userName = String::EMPTY;
            Uint16 truststoreType;
            CIMValue cimValue;
            
            cimValue = inParams[0].getValue();
            cimValue.get(certificateContents);
            
            cimValue = inParams[1].getValue();
            cimValue.get(userName);
            
            cimValue = inParams[2].getValue();
            cimValue.get(truststoreType);
    
            PEG_TRACE_STRING(TRC_CONTROLPROVIDER,Tracer::LEVEL4,"Certificate parameters:\n");
            PEG_TRACE_STRING(TRC_CONTROLPROVIDER,Tracer::LEVEL4,"\tcertificateContents:" + certificateContents);
            PEG_TRACE_STRING(TRC_CONTROLPROVIDER,Tracer::LEVEL4,"\tuserName:" + userName);
            
            //check for a valid truststore
            if (truststoreType != SERVER_TRUSTSTORE && truststoreType != EXPORT_TRUSTSTORE)
            {
                throw CIMException(CIM_ERR_INVALID_PARAMETER, "The truststore specified by truststoreType is invalid.");
            }

            //check for a valid username
            if (!System::isSystemUser(userName.getCString()))
            {
                throw CIMException(CIM_ERR_INVALID_PARAMETER, "The user specified by userName is not a valid system user.");
            }
    
            //read in the certificate contents
            BIO *mem = BIO_new(BIO_s_mem());
            char contents[2048];
            sprintf(contents, "%s", (const char*) certificateContents.getCString());
            BIO_puts(mem, contents);
    
            X509 *xCert = NULL;
            if (!PEM_read_bio_X509(mem, &xCert, 0, NULL))
            {
                BIO_free(mem);

                PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3, "Error: Could not read x509 PEM format.");
                MessageLoaderParms parms("ControlProviders.CertificateProvider.BAD_X509_FORMAT",
                                         "Could not read x509 PEM format.");
                throw CIMException(CIM_ERR_FAILED, parms);
            }
            BIO_free(mem);
            
            PEG_TRACE_STRING(TRC_CONTROLPROVIDER,Tracer::LEVEL4,"Read x509 certificate...");
            
            char buf[256];
            String issuerName = String::EMPTY;
            String serialNumber = String::EMPTY;
            String subjectName = String::EMPTY;
            CIMDateTime notBefore;
            CIMDateTime notAfter;
            
            //issuer name
            X509_NAME_oneline(X509_get_issuer_name(xCert), buf, 256);
            issuerName = String(buf);
            
            //serial number
            long rawSerialNumber = ASN1_INTEGER_get(X509_get_serialNumber(xCert));
            char serial[256];
            sprintf(serial, "%lu", rawSerialNumber);
            serialNumber = String(serial);
    
            //subject name
            X509_NAME_oneline(X509_get_subject_name(xCert), buf, 256);
            subjectName = String(buf);

            //validity dates
            notBefore = getDateTime(X509_get_notBefore(xCert));
            notAfter = getDateTime(X509_get_notAfter(xCert));

            PEG_TRACE_STRING(TRC_CONTROLPROVIDER,Tracer::LEVEL4,"IssuerName:" + issuerName);
            PEG_TRACE_STRING(TRC_CONTROLPROVIDER,Tracer::LEVEL4,"SerialNumber:" + serialNumber);
            PEG_TRACE_STRING(TRC_CONTROLPROVIDER,Tracer::LEVEL4,"SubjectName:" + subjectName);
            PEG_TRACE_STRING(TRC_CONTROLPROVIDER,Tracer::LEVEL4,"NotBefore:" + notBefore.toString());
            PEG_TRACE_STRING(TRC_CONTROLPROVIDER,Tracer::LEVEL4,"NotAfter:" + notAfter.toString());
            
            //check validity with current datetime
            //openssl will reject the certificate if it's not valid even if we add it to the truststore
            try
            {
                if ((CIMDateTime::getDifference(CIMDateTime::getCurrentDateTime(), notBefore) > 0))
                {
                    PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3, "Certificate or CRL is not valid yet.  Check the timestamps on your machine.");
                    MessageLoaderParms parms("ControlProviders.CertificateProvider.CERT_NOT_VALID_YET",
                                             "The certificate is not valid yet.  Check the timestamps on your machine.");
                    throw CIMException(CIM_ERR_FAILED, parms);
                }
                if (CIMDateTime::getDifference(notAfter, CIMDateTime::getCurrentDateTime()) > 0) 
                {
                    PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3, "Certificate or CRL is expired.");
                    MessageLoaderParms parms("ControlProviders.CertificateProvider.CERT_EXPIRED",
                                             "The certificate has expired.");
                    throw CIMException(CIM_ERR_FAILED, parms);
                }

            } catch (DateTimeOutOfRangeException& ex)
            {
                    PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3, "Certificate or CRL dates are out of range.");
                    MessageLoaderParms parms("ControlProviders.CertificateProvider.BAD_DATE_FORMAT",
                                             "The validity dates are out of range.");
                    throw CIMException(CIM_ERR_FAILED, parms);
            }

            String storePath = String::EMPTY;
            String storeId = String::EMPTY;
    
            switch (truststoreType) 
            {
            case SERVER_TRUSTSTORE : storePath = _sslTrustStore; 
                                     storeId = "server"; 
                                     break;
            case EXPORT_TRUSTSTORE : storePath = _exportSSLTrustStore; 
                                     storeId = "export";
                                     break;
            default: break;
            }
    
            AutoMutex lock(_trustStoreMutex);

            //attempt to add cert to truststore
            String certificateFileName = _getNewCertificateFileName(storePath, X509_subject_name_hash(xCert));
            PEG_TRACE_STRING(TRC_CONTROLPROVIDER,Tracer::LEVEL4,"Certificate " + certificateFileName + " registered to " + userName + "\n");
    
            // build instance
            CIMInstance cimInstance(PEGASUS_CLASSNAME_CERTIFICATE);
        
            cimInstance.addProperty(CIMProperty(ISSUER_NAME_PROPERTY, CIMValue(issuerName)));
            cimInstance.addProperty(CIMProperty(SERIAL_NUMBER_PROPERTY, CIMValue(serialNumber)));
            cimInstance.addProperty(CIMProperty(SUBJECT_NAME_PROPERTY, CIMValue(subjectName)));
            cimInstance.addProperty(CIMProperty(USER_NAME_PROPERTY, CIMValue(userName)));
            cimInstance.addProperty(CIMProperty(TRUSTSTORE_TYPE_PROPERTY, CIMValue(truststoreType)));
            cimInstance.addProperty(CIMProperty(FILE_NAME_PROPERTY, CIMValue(certificateFileName)));
            cimInstance.addProperty(CIMProperty(NOT_BEFORE_PROPERTY, CIMValue(notBefore)));
            cimInstance.addProperty(CIMProperty(NOT_AFTER_PROPERTY, CIMValue(notAfter)));
        
     // set keys
    Array<CIMKeyBinding> keys;
    CIMKeyBinding key;
    key.setName(ISSUER_NAME_PROPERTY.getString());
    key.setValue(issuerName);
    key.setType(CIMKeyBinding::STRING);
    keys.append(key);

    key.setName(SERIAL_NUMBER_PROPERTY.getString());
			key.setType(CIMKeyBinding::STRING);
    key.setValue(String(serialNumber));
    keys.append(key);

    key.setName(TRUSTSTORE_TYPE_PROPERTY.getString());
    key.setType(CIMKeyBinding::NUMERIC);
    char tmp[10];
    sprintf(tmp, "%d", truststoreType);
    key.setValue(String(tmp));
    keys.append(key);

            // set object path for instance
            cimInstance.setPath(CIMObjectPath(cimObjectPath.getHost(), cimObjectPath.getNameSpace(), PEGASUS_CLASSNAME_CERTIFICATE, keys));
    
            PEG_TRACE_STRING(TRC_CONTROLPROVIDER,Tracer::LEVEL4,"New certificate COP: " + cimInstance.getPath().toString());
    
            //attempt to add the instance to the repository first; that way if this instance already exist it will take care of throwing 
            //an error before we add the file to the truststore
            _repository->createInstance("root/PG_Internal", cimInstance);
    
            //ATTN: Take care of this conversion
            char newFileName[256];
            sprintf(newFileName, "%s", (const char*) certificateFileName.getCString());
        
            //use the ssl functions to write out the client x509 certificate
            //TODO: add some error checking here
            BIO* outFile = BIO_new(BIO_s_file());
            BIO_write_filename(outFile, newFileName);
            int i = PEM_write_bio_X509(outFile, xCert);
            BIO_free_all(outFile);
            
            Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE, 
                        "The certificate registered to $0 from issuer $1 has been added to the $2 truststore.", 
                        userName,
                        issuerName,
                        storeId);

            CIMValue returnValue(Boolean(true));
    
            handler.deliver(returnValue);
    
            handler.complete();
    
        } else
        {
            throw CIMException(CIM_ERR_METHOD_NOT_FOUND, methodName.getString());
        }

    } else if (className == PEGASUS_CLASSNAME_CRL)
    {
        if (methodName == METHOD_ADD_CRL)
        {
            PEG_TRACE_STRING(TRC_CONTROLPROVIDER,Tracer::LEVEL4,"CertificateProvider::addCertificateRevocationList");
    
            String crlContents = String::EMPTY;
            CIMValue cimValue;
            
            cimValue = inParams[0].getValue();
            cimValue.get(crlContents);
           
            PEG_TRACE_STRING(TRC_CONTROLPROVIDER,Tracer::LEVEL4,"inparam CRL contents:" + crlContents);
        
            //check for a valid CRL
            //read in the CRL contents
            BIO *mem = BIO_new(BIO_s_mem());
            char contents[2048];
            sprintf(contents, "%s", (const char*) crlContents.getCString());
            BIO_puts(mem, contents);
    
            X509_CRL *xCrl = NULL;
            if (!PEM_read_bio_X509_CRL(mem, &xCrl, NULL, NULL))
            {
                BIO_free(mem);
                
                PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3, "Error: Could not read x509 PEM format.");
                MessageLoaderParms parms("ControlProviders.CertificateProvider.BAD_X509_FORMAT",
                                         "Could not read x509 PEM format.");
                throw CIMException(CIM_ERR_FAILED, parms);
            }
            BIO_free(mem);
            
            PEG_TRACE_STRING(TRC_CONTROLPROVIDER,Tracer::LEVEL4,"Successfully read x509 CRL...");
            
            char buf[256];
            String issuerName = String::EMPTY;
            CIMDateTime lastUpdate;
            CIMDateTime nextUpdate;
            Array<String> revokedSerialNumbers;
            Array<CIMDateTime> revocationDates;

            //issuer name
            X509_NAME_oneline(X509_CRL_get_issuer(xCrl), buf, 256);
            issuerName = String(buf);

            //check validity of CRL
            //openssl will only issue a warning if the CRL is expired
            //However, we still don't want to let them register an expired or invalid CRL
            lastUpdate = getDateTime(X509_CRL_get_lastUpdate(xCrl));
            nextUpdate = getDateTime(X509_CRL_get_nextUpdate(xCrl));
            try
            {
                if ((CIMDateTime::getDifference(CIMDateTime::getCurrentDateTime(), lastUpdate) > 0))
                {
                    PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3, "The CRL is not valid yet.  Check the timestamps on your machine.");
                    MessageLoaderParms parms("ControlProviders.CertificateProvider.CRL_NOT_VALID_YET",
                                             "The CRL is not valid yet.  Check the timestamps on your machine.");
                    throw CIMException(CIM_ERR_FAILED, parms);
                }
                if (CIMDateTime::getDifference(nextUpdate, CIMDateTime::getCurrentDateTime()) > 0) 
                {
                    PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3, "This CRL is not up-to-date.  Check the CA for the latest one.");
                    MessageLoaderParms parms("ControlProviders.CertificateProvider.CRL_EXPIRED",
                                             "The CRL is not up-to-date.  Check with the issuing CA for the latest one.");
                    throw CIMException(CIM_ERR_FAILED, parms);
                }

            } catch (DateTimeOutOfRangeException& ex)
            {
                PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3, "Certificate or CRL dates are out of range.");
                MessageLoaderParms parms("ControlProviders.CertificateProvider.BAD_DATE_FORMAT",
                                             "Certificate or CRL dates are out of range.");
                throw CIMException(CIM_ERR_FAILED, parms);
            }
            
            STACK_OF(X509_REVOKED)* revokedCertificates = NULL;
            X509_REVOKED* revokedCertificate = NULL;
            int revokedCount = -1;

            revokedCertificates = X509_CRL_get_REVOKED(xCrl);
            revokedCount = sk_X509_REVOKED_num(revokedCertificates);
            
            char countStr[3];
            sprintf(countStr, "%d", revokedCount);

            if (revokedCount > 0)
            {
                PEG_TRACE_STRING(TRC_CONTROLPROVIDER,Tracer::LEVEL4, "CRL contains revoked certificate entries ");
                PEG_TRACE_STRING(TRC_CONTROLPROVIDER,Tracer::LEVEL4, countStr);
            } else
            {
                PEG_TRACE_STRING(TRC_CONTROLPROVIDER, Tracer::LEVEL3, "Error: CRL is empty.");
                MessageLoaderParms parms("ControlProviders.CertificateProvider.EMPTY_CRL",
                                         "The CRL is empty.");
                throw CIMException(CIM_ERR_FAILED, parms);
            }

            AutoMutex lock(_crlStoreMutex);

            String crlFileName = _getCRLFileName(_crlStore, X509_NAME_hash(X509_CRL_get_issuer(xCrl)));

            PEG_TRACE_STRING(TRC_CONTROLPROVIDER,Tracer::LEVEL4,"IssuerName:" + issuerName);
            PEG_TRACE_STRING(TRC_CONTROLPROVIDER,Tracer::LEVEL4, "FileName: " + crlFileName);

            //ATTN: Take care of this conversion
            //For some reason i cannot do this in the BIO_write_filename call
            char newFileName[256];
            sprintf(newFileName, "%s", (const char*) crlFileName.getCString());
        
            //use the ssl functions to write out the client x509 certificate
            //TODO: add some error checking here
            BIO* outFile = BIO_new(BIO_s_file());
            BIO_write_filename(outFile, newFileName);
            int i = PEM_write_bio_X509_CRL(outFile, xCrl);
            BIO_free_all(outFile);

            Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE, 
                        "The CRL for issuer $1 has been updated.", 
                        issuerName);

			//reload the CRL store
			PEG_TRACE_STRING(TRC_SSL, Tracer::LEVEL4, "Loading CRL store after an update");
            _sslContextMgr->reloadCRLStore();

            CIMValue returnValue(Boolean(true));
    
            handler.deliver(returnValue);
    
            handler.complete();
        } 
         else
        {
            throw CIMException(CIM_ERR_METHOD_NOT_FOUND, methodName.getString());
        }
    } else
    {
        throw CIMException(CIM_ERR_INVALID_CLASS, className.getString());
    }
    
}


PEGASUS_NAMESPACE_END


