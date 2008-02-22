//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
//%/////////////////////////////////////////////////////////////////////////////

#ifndef SocketzOS_inline_h
#define SocketzOS_inline_h

#include <Pegasus/Common/Logger.h>
#ifdef PEGASUS_ZOS_SECURITY
// This include file will not be provided in the OpenGroup CVS for now.
// Do NOT try to include it in your compile
#include <Pegasus/Common/safCheckzOS_inline.h>
#endif
#include <sys/ioctl.h>
#include <net/rtrouteh.h>
#include <net/if.h>
#include "//'SYS1.SEZANMAC(EZBZTLSC)'"


PEGASUS_NAMESPACE_BEGIN


MP_Socket::MP_Socket(SocketHandle socket)
    : _socket(socket), _isSecure(false),
      _userAuthenticated(false)
{
    _username[0]=0;
}

MP_Socket::MP_Socket(
    SocketHandle socket,
    SSLContext * sslcontext,
    ReadWriteSem * sslContextObjectLock)
    : _socket(socket),
      _userAuthenticated(false)
{
    PEG_METHOD_ENTER(TRC_SSL, "MP_Socket::MP_Socket()");
    _username[0]=0;
    if (sslcontext != NULL)
    {
        _isSecure = true;
    }
    else
    {
        _isSecure = false;
    }
    PEG_METHOD_EXIT();
}

int MP_Socket::ATTLS_zOS_query()
{
   struct TTLS_IOCTL ioc;                     // ioctl data structure
   int rcIoctl;
   int errnoIoctl;
   int errno2Ioctl;

   PEG_METHOD_ENTER(TRC_SSL, "ATTLS_zOS_query()");

   memset(&ioc,0,sizeof(ioc));                // clean the structure
   ioc.TTLSi_Ver = TTLS_VERSION1;             // set used version of structure
   ioc.TTLSi_Req_Type = TTLS_QUERY_ONLY;      // initialize for query only
   ioc.TTLSi_BufferPtr = NULL;                // no buffer for the certificate
   ioc.TTLSi_BufferLen = 0;                   // will not use it

   rcIoctl = ioctl(_socket,SIOCTTLSCTL,(char *)&ioc);
   errnoIoctl = errno;
   errno2Ioctl =__errno2();

   if (rcIoctl < 0)
   {
      switch(errnoIoctl)
      {
          case(EINPROGRESS):
          case(EWOULDBLOCK):
          // Due to a race condition the native accept is not finished.
          // ioctl() retuns with ENotConn.
          case(ENOTCONN):
          {
              PEG_TRACE((TRC_SSL, Tracer::LEVEL4,
                  "Accept pending: %s ( errno $d, reason code 0x%08X ). ", 
                  strerror(errnoIoctl),
                  errnoIoctl,
                  errno2Ioctl));
              return 0; // accept pending
          }
          case(ECONNRESET):
          {
              Logger::put_l(
                  Logger::STANDARD_LOG, System::CIMSERVER, Logger::INFORMATION,
                  "Pegasus.Common.SocketzOS_inline.CONNECTION_RESET_ERROR",
                  "ATTLS reset the connection due to handshake failure. "
                      "Connection closed.");
              PEG_METHOD_EXIT();
              return -1;
          }
          default:
          {
              char str_errno2[10];
              sprintf(str_errno2,"%08X",errno2Ioctl);
              Logger::put_l(
                  Logger::ERROR_LOG, System::CIMSERVER, Logger::SEVERE,
                  "Pegasus.Common.SocketzOS_inline.UNEXPECTED_ERROR",
                  "An unexpected error occurs: $0 ( errno $1, reason code "
                      "0x$2 ). Connection closed.",
                  strerror(errnoIoctl),
                  errnoIoctl,
                  str_errno2);
              PEG_METHOD_EXIT();
              return -1;
          }
      } // end switch(errnoIoctl)
   } // -1 ioctl()

   // this should be a secured connection so
   // check the configuration of ATTLS policy.
   switch(ioc.TTLSi_Stat_Policy)
   {
       case(TTLS_POL_OFF):
       case(TTLS_POL_NO_POLICY):
       case(TTLS_POL_NOT_ENABLED):
       {
           Logger::put_l(
               Logger::ERROR_LOG, System::CIMSERVER, Logger::SEVERE,
               "Pegasus.Common.SocketzOS_inline.POLICY_NOT_ENABLED",
               "ATTLS policy is not active for the CIM Server HTTPS port. "
                   "Communication not secured. Connection closed.");
           PEG_METHOD_EXIT();
           return -1;
       }
       case(TTLS_POL_ENABLED):
       {
           // a policy exists so it is ensured that a secured connection will
           // be established
           break;
       }
       case(TTLS_POL_APPLCNTRL):
       {
           Logger::put_l(
               Logger::ERROR_LOG, System::CIMSERVER, Logger::SEVERE,
               "Pegasus.Common.SocketzOS_inline.APPLCNTRL",
               "ATTLS policy not valid for CIM Server. "
                   "Set ApplicationControlled to OFF. Connection closed.");
           PEG_METHOD_EXIT();
           return -1;
       }

   } // end switch(ioc.TTLSi_Stat_Policy)

   // check status of connection, configuration is ok for the CIM Server
   switch(ioc.TTLSi_Stat_Conn)
   {
       case(TTLS_CONN_NOTSECURE):
       case(TTLS_CONN_HS_INPROGRESS):
       {
           // the SSL handshake has not been finished yet, try late again.
           PEG_TRACE_CSTRING(TRC_SSL, Tracer::LEVEL4, "---> Accept pending.");
           return 0;
       }
       case(TTLS_CONN_SECURE):
       {
           break; // the connection is secure
       }


   } // end switch(ioc.TTLSi_Stat_Conn)

   //
   switch(ioc.TTLSi_Sec_Type)
   {
       case(TTLS_SEC_UNKNOWN):
       case(TTLS_SEC_CLIENT):
       {
           Logger::put_l(
               Logger::ERROR_LOG, System::CIMSERVER, Logger::SEVERE,
               "Pegasus.Common.SocketzOS_inline.WRONG_ROLE",
               "ATTLS policy specifies the wrong HandshakeRole for the "
                   "CIM Server HTTPS port. Communication not secured. "
                   "Connection closed.");
           PEG_METHOD_EXIT();
           return -1;

       }

       case(TTLS_SEC_SERVER):
       case(TTLS_SEC_SRV_CA_PASS):
       case(TTLS_SEC_SRV_CA_FULL):
       case(TTLS_SEC_SRV_CA_REQD):
       {
           PEG_TRACE_CSTRING(TRC_SSL, Tracer::LEVEL4,
               "---> ATTLS Security Type is valid but no SAFCHK.");
           PEG_METHOD_EXIT();
           return 1;
       }

       case(TTLS_SEC_SRV_CA_SAFCHK):
       {
           _userAuthenticated=true;
           memcpy(_username,ioc.TTLSi_UserID,ioc.TTLSi_UserID_Len);
           _username[ioc.TTLSi_UserID_Len]=0;   // null terminated string
           __etoa(_username);                   // the user name is in EBCDIC !
           PEG_TRACE((TRC_SSL, Tracer::LEVEL2,
               "---> ATTLS Security Type is SAFCHK. Resolved user ID \'%s\'",
               _username));

               // Check if the user is authorized to CIMSERV
#ifdef PEGASUS_ZOS_SECURITY
           if ( !CheckProfileCIMSERVclassWBEM(_username, __READ_RESOURCE) )
           {
               Logger::put_l(Logger::STANDARD_LOG, ZOS_SECURITY_NAME, 
                   Logger::WARNING,
                   "Pegasus.Common.SocketzOS_inline.NOREAD_CIMSERV_ACCESS",
                   "Request UserID $0 doesn't have READ permission"
                   " to profile CIMSERV CL(WBEM).",
                   _username);
               return -1;
           }
#endif
           PEG_METHOD_EXIT();
           return 1;

       }
   } // end switch(ioc.TTLSi_Sec_Type)
   // This should never be reached
   PEG_TRACE_CSTRING(TRC_SSL, Tracer::LEVEL4,
       "---> Never reach this! New/wrong return value of ioctl().");
   PEG_METHOD_EXIT();
   return -1;
} // end ATTLS_zOS_Query


PEGASUS_NAMESPACE_END
#endif

