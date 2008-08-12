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

#ifndef Pegasus_IndicationMessageConstants_h
#define Pegasus_IndicationMessageConstants_h

#include <Pegasus/Common/Config.h>

/*
    Normally it is not good practice to put static constants into headers
    because the footprint grows with each use of the header.  In this case,
    there is only one use of this header so it is actually a smaller footprint
    to use the header file than define extern definitions.  If this header is
    EVER used in more than one cpp file, it should be modified to create a .cpp
    file for the definitions.
*/

PEGASUS_NAMESPACE_BEGIN

//
//  Message substrings used in exception messages
//

// some have been commented out and put directly in the code for localization

static const char _MSG_KEY_PROPERTY [] = "The key property $0 is missing.";

static const char _MSG_KEY_PROPERTY_KEY [] =
   "IndicationService.IndicationService._MSG_KEY_PROPERTY";

static const char _MSG_PROPERTY [] = "The required property $0 is missing.";

static const char _MSG_PROPERTY_KEY [] =
   "IndicationService.IndicationService._MSG_PROPERTY";

static const char _MSG_PROPERTY_PRESENT_BUT_VALUE_NOT[] =
    "The $0 property is present, but the $1 value is not $2.";

static const char _MSG_PROPERTY_PRESENT_BUT_VALUE_NOT_KEY [] =
   "IndicationService.IndicationService._MSG_PROPERTY_PRESENT_BUT_VALUE_NOT";

static const char _MSG_NO_PROVIDERS [] =
    "There are no providers capable of serving the subscription";

static const char _MSG_NO_PROVIDERS_KEY [] =
    "IndicationService.IndicationService._MSG_NO_PROVIDERS";

static const char _MSG_NOT_ACCEPTED [] =
    "No providers accepted the subscription";

static const char _MSG_NOT_ACCEPTED_KEY [] =
    "IndicationService.IndicationService._MSG_NOT_ACCEPTED";

static const char _MSG_INVALID_CLASSNAME_IN_FROM_PROPERTY[] =
    "The Indication class name $0 is not valid in the FROM clause of $1 $2 "
        "property.";

static const char _MSG_INVALID_CLASSNAME_IN_FROM_PROPERTY_KEY [] =
    "IndicationService.IndicationService."
        "_MSG_INVALID_CLASSNAME_IN_FROM_PROPERTY";

static const char _MSG_EXPIRED [] =
    "Expired subscription may not be modified; has been deleted";

static const char _MSG_EXPIRED_KEY [] =
    "IndicationService.IndicationService._MSG_EXPIRED";

static const char _MSG_REFERENCED [] =
    "A Filter or Handler referenced by a subscription may not be deleted";

static const char _MSG_REFERENCED_KEY [] =
    "IndicationService.IndicationService._MSG_REFERENCED";

static const char _MSG_INVALID_TYPE_FOR_PROPERTY[] =
    "The value of type $0 is not valid for property $1.";

static const char _MSG_INVALID_TYPE_FOR_PROPERTY_KEY [] =
    "IndicationService.IndicationService._MSG_INVALID_TYPE_FOR_PROPERTY";

static const char _MSG_INVALID_TYPE_ARRAY_OF_FOR_PROPERTY[] =
    "The value of an array of type $0 is not valid for property $1.";

static const char _MSG_INVALID_TYPE_ARRAY_OF_FOR_PROPERTY_KEY [] =
    "IndicationService.IndicationService."
        "_MSG_INVALID_TYPE_ARRAY_OF_FOR_PROPERTY";

static const char _MSG_INVALID_VALUE_FOR_PROPERTY[] =
    "The value $0 is not valid for property $1.";

static const char _MSG_INVALID_VALUE_FOR_PROPERTY_KEY [] =
    "IndicationService.IndicationService._MSG_INVALID_VALUE_FOR_PROPERTY";

static const char _MSG_UNSUPPORTED_VALUE_FOR_PROPERTY[] =
    "The value $0 is not supported for property $1.";

static const char _MSG_UNSUPPORTED_VALUE_FOR_PROPERTY_KEY [] =
    "IndicationService.IndicationService._MSG_UNSUPPORTED_VALUE_FOR_PROPERTY";

static const char _MSG_CLASS_NOT_SERVED [] =
    "The specified class is not served by the Indication Service";

static const char _MSG_CLASS_NOT_SERVED_KEY [] =
    "IndicationService.IndicationService._MSG_CLASS_NOT_SERVED";

static const char _MSG_INVALID_INSTANCES [] =
    "One or more invalid Subscription instances were ignored";

static const char _MSG_INVALID_INSTANCES_KEY [] =
    "IndicationService.IndicationService."
        "INVALID_SUBSCRIPTION_INSTANCES_IGNORED";

static const char _MSG_INVALID_INSTANCE [] =
    "An invalid Subscription instance was ignored: $0";

static const char _MSG_INVALID_INSTANCE_KEY [] =
    "IndicationService.IndicationService.INVALID_SUBSCRIPTION_INSTANCE_IGNORED";

static const char _MSG_PROVIDER_NO_LONGER_SERVING [] =
    "Provider ($0) is no longer serving subscription ($1) in namespace $2";

static const char _MSG_PROVIDER_NO_LONGER_SERVING_KEY [] =
    "IndicationService.IndicationService._MSG_PROVIDER_NO_LONGER_SERVING";

static const char _MSG_PROVIDER_NOW_SERVING [] =
    "Provider ($0) is now serving subscription ($1) in namespace $2";

static const char _MSG_PROVIDER_NOW_SERVING_KEY [] =
    "IndicationService.IndicationService._MSG_PROVIDER_NOW_SERVING";

static const char _MSG_NO_PROVIDER [] =
    "Subscription ($0) in namespace $1 has no provider";

static const char _MSG_NO_PROVIDER_KEY [] =
    "IndicationService.IndicationService._MSG_NO_PROVIDER";

static const char _MSG_NON_PRIVILEGED_ACCESS_DISABLED [] =
    "User ($0) is not authorized to perform this operation.";

static const char _MSG_NON_PRIVILEGED_ACCESS_DISABLED_KEY [] =
    "IndicationService.IndicationService._MSG_NON_PRIVILEGED_ACCESS_DISABLED";

static const char _MSG_ARRAY_NOT_SUPPORTED_IN_WHERE [] =
    "Array property $0 is not supported in the WQL WHERE clause";

static const char _MSG_ARRAY_NOT_SUPPORTED_IN_WHERE_KEY [] =
    "IndicationService.IndicationService._MSG_ARRAY_NOT_SUPPORTED_IN_WHERE";

static const char _MSG_WHERE_PROPERTY_NOT_FOUND [] =
    "Property $0 referenced in the WQL WHERE clause was not found in the "
        "indication class $1";

static const char _MSG_WHERE_PROPERTY_NOT_FOUND_KEY [] =
    "IndicationService.IndicationService._MSG_WHERE_PROPERTY_NOT_FOUND";

static const char _MSG_SELECT_PROPERTY_NOT_FOUND [] =
    "Property $0 referenced in the WQL SELECT clause was not found in the "
        "indication class $1";

static const char _MSG_SELECT_PROPERTY_NOT_FOUND_KEY [] =
    "IndicationService.IndicationService._MSG_SELECT_PROPERTY_NOT_FOUND";

static const char _MSG_PROPERTY_NOT_SUPPORTED [] =
    "Property $0 is not supported in class $1";

static const char _MSG_PROPERTY_NOT_SUPPORTED_KEY [] =
    "IndicationService.IndicationService._MSG_PROPERTY_NOT_SUPPORTED";

static const char _MSG_DUPLICATE_SUBSCRIPTION_REQUEST_KEY [] = 
    "IndicationService.IndicationService._MSG_DUPLICATE_SUBSCRIPTION_REQUEST";

static const char _MSG_DUPLICATE_SUBSCRIPTION_REQUEST [] = 
    "Similar create subscription request is being processed. "
        "Subscription path : $0";

PEGASUS_NAMESPACE_END

#endif  /* Pegasus_IndicationMessageConstants_h */
