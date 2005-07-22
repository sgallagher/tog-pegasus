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
// Author: Carol Ann Krug Graves, Hewlett-Packard Company
//             (carolann_graves@hp.com)
//
// Modified By:  
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_IndicationMessageConstants_h
#define Pegasus_IndicationMessageConstants_h

#include <Pegasus/Common/Config.h>


PEGASUS_NAMESPACE_BEGIN

//
//  Message substrings used in exception messages
//

// l10n

// some have been commented out and put directly in the code for localization

// this one is tricky because it is used in _checkRequiredProperty with the two
// below
static const char _MSG_MISSING_REQUIRED [] = "Missing required ";

static const char _MSG_KEY_PROPERTY [] = " key property";

static const char _MSG_KEY_PROPERTY_KEY [] = 
   "IndicationService.IndicationService._MSG_KEY_PROPERTY";

static const char _MSG_PROPERTY [] = " property";

static const char _MSG_PROPERTY_KEY [] = 
   "IndicationService.IndicationService._MSG_PROPERTY";

static const char _MSG_PROPERTY_PRESENT [] = " property present, but ";

static const char _MSG_VALUE_NOT [] = " value not ";

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

static const char _MSG_INVALID_CLASSNAME [] = "Invalid indication class name ";

static const char _MSG_IN_FROM [] = " in FROM clause of ";

static const char _MSG_INVALID_CLASSNAME_IN_FROM_PROPERTY_KEY [] = 
    "IndicationService.IndicationService._MSG_INVALID_CLASSNAME_IN_FROM_PROPERTY";

static const char _MSG_EXPIRED [] = 
    "Expired subscription may not be modified; has been deleted";

static const char _MSG_EXPIRED_KEY [] = 
    "IndicationService.IndicationService._MSG_EXPIRED";

static const char _MSG_REFERENCED [] = 
    "A Filter or Handler referenced by a subscription may not be deleted";

static const char _MSG_REFERENCED_KEY [] = 
    "IndicationService.IndicationService._MSG_REFERENCED";

static const char _MSG_INVALID_VALUE [] = "Invalid value ";

static const char _MSG_INVALID_TYPE [] = "Invalid type ";

static const char _MSG_UNSUPPORTED_VALUE [] = "Unsupported value ";

static const char _MSG_FOR_PROPERTY [] = " for property ";

static const char _MSG_ARRAY_OF [] = "array of ";

static const char _MSG_INVALID_TYPE_FOR_PROPERTY_KEY [] =
    "IndicationService.IndicationService._MSG_INVALID_TYPE_FOR_PROPERTY";

static const char _MSG_INVALID_TYPE_ARRAY_OF_FOR_PROPERTY_KEY [] =
    "IndicationService.IndicationService._MSG_INVALID_TYPE_ARRAY_OF_FOR_PROPERTY";

static const char _MSG_INVALID_VALUE_FOR_PROPERTY_KEY [] =
    "IndicationService.IndicationService._MSG_INVALID_VALUE_FOR_PROPERTY";

static const char _MSG_UNSUPPORTED_VALUE_FOR_PROPERTY_KEY [] =
    "IndicationService.IndicationService._MSG_UNSUPPORTED_VALUE_FOR_PROPERTY";

static const char _MSG_CLASS_NOT_SERVED [] =
    "The specified class is not served by the Indication Service";

static const char _MSG_CLASS_NOT_SERVED_KEY [] =
    "IndicationService.IndicationService._MSG_CLASS_NOT_SERVED";

static const char _MSG_INVALID_INSTANCES [] =
    "One or more invalid Subscription instances were ignored";

static const char _MSG_INVALID_INSTANCES_KEY [] =
    "IndicationService.IndicationService.INVALID_SUBSCRIPTION_INSTANCES_IGNORED";

static const char _MSG_PROVIDER_NO_LONGER_SERVING [] =
    "Provider ($0) is no longer serving subscription ($1) in namespace $2";

static const char _MSG_PROVIDER_NO_LONGER_SERVING_KEY [] =
    "IndicationService.IndicationService._MSG_PROVIDER_NO_LONGER_SERVING";

static const char _MSG_PROVIDER_NOW_SERVING [] =
    "Provider ($0) is now serving subscription ($1) in namespace $2";

static const char _MSG_PROVIDER_NOW_SERVING_KEY [] =
    "IndicationService.IndicationService._MSG_PROVIDER_NOW_SERVING";

static const char _MSG_NO_PROVIDER [] = "Subscription ($0) in namespace $1 has no provider";

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
    "Property $0 referenced in the WQL WHERE clause was not found in the indication class $1";

static const char _MSG_WHERE_PROPERTY_NOT_FOUND_KEY [] =
    "IndicationService.IndicationService._MSG_WHERE_PROPERTY_NOT_FOUND";

static const char _MSG_SELECT_PROPERTY_NOT_FOUND [] =
    "Property $0 referenced in the WQL SELECT clause was not found in the indication class $1";

static const char _MSG_SELECT_PROPERTY_NOT_FOUND_KEY [] =
    "IndicationService.IndicationService._MSG_SELECT_PROPERTY_NOT_FOUND";

static const char _MSG_PROPERTY_NOT_SUPPORTED [] =
    "Property $0 is not supported in class $1";

static const char _MSG_PROPERTY_NOT_SUPPORTED_KEY [] =
    "IndicationService.IndicationService._MSG_PROPERTY_NOT_SUPPORTED";

PEGASUS_NAMESPACE_END

#endif  /* Pegasus_IndicationMessageConstants_h */
