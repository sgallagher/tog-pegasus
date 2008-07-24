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
//=============================================================================
//
//%////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_WsmFault_h
#define Pegasus_WsmFault_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Linkage.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/ContentLanguageList.h>
#include <Pegasus/Common/MessageLoader.h>
#include <Pegasus/WsmServer/WsmRequest.h>
#include <Pegasus/WsmServer/WsmUtils.h>

PEGASUS_NAMESPACE_BEGIN


class PEGASUS_WSMSERVER_LINKAGE  WsmFault
{
public:

    enum Code
    {
        s_Sender,
        s_Receiver
    };

    enum Subcode
    {
        wsman_AccessDenied,
        wsa_ActionNotSupported,
        wsman_AlreadyExists,
        wsen_CannotProcessFilter,
        wsman_Concurrency,
        wse_DeliveryModeRequestedUnavailable,
        wsman_DeliveryRefused,
        wsa_DestinationUnreachable,
        wsman_EncodingLimit,
        wsa_EndpointUnavailable,
        wse_EventSourceUnableToProcess,
        wsen_FilterDialectRequestedUnavailable,
        wse_FilteringNotSupported,
        wsen_FilteringNotSupported,
        wse_FilteringRequestedUnavailable,
        wsman_InternalError,
        wsman_FragmentDialectNotSupported,
        wsman_InvalidBookmark,
        wsen_InvalidEnumerationContext,
        wse_InvalidExpirationTime,
        wsen_InvalidExpirationTime,
        wse_InvalidMessage,
        wsa_InvalidMessageInformationHeader,
        wsman_InvalidOptions,
        wsman_InvalidParameter,
        wxf_InvalidRepresentation,
        wsman_InvalidSelectors,
        wsa_MessageInformationHeaderRequired,
        wsman_NoAck,
        wsman_QuotaLimit,
        wsman_SchemaValidationError,
        wsen_TimedOut,
        wsman_TimedOut,
        wse_UnableToRenew,
        wse_UnsupportedExpirationType,
        wsen_UnsupportedExpirationType,
        wsman_UnsupportedFeature,
        wsmb_PolymorphismModeNotSupported
    };

    struct FaultTableEntry
    {
        Code code;
        Subcode subcode;
        WsmNamespaces::Type nsType;
        const char* subcodeStr;
    };

    WsmFault(
        Subcode subcode,
        const MessageLoaderParms& msgParms,
        const String& faultDetail = String::EMPTY);

    WsmFault(
        Subcode subcode,
        const String& reason = String::EMPTY,
        const ContentLanguageList& reasonLanguage = ContentLanguageList(),
        const String& faultDetail = String::EMPTY);

    WsmFault(const WsmFault& fault);

    ~WsmFault()
    {
    }

    const char* getAction() const;
    String getCode() const;
    String getSubcode() const;
    String getReason() const;
    ContentLanguageList getReasonLanguage() const;
    String getFaultDetail() const;
    // Note: Other types of s:Detail data are not supported.

private:

    static FaultTableEntry _faultTable[];

    Subcode _subcode;
    String _reason;
    ContentLanguageList _reasonLanguage;
    String _faultDetail;
};

class PEGASUS_WSMSERVER_LINKAGE SoapNotUnderstoodFault
{
public:

    SoapNotUnderstoodFault(
        const String& nameSpace,
        const String& headerName);

    String getNamespace() const;
    String getHeaderName() const;
    String getMessage() const;
    ContentLanguageList getMessageLanguage() const;

private:

    String _nameSpace;
    String _headerName;
    String _message;
    ContentLanguageList _messageLanguage;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_WsmFault_h */
