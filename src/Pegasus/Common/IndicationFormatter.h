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
// Author: Yi Zhou, Hewlett-Packard Company (yi.zhou@hp.com)
//
// Modified By: 
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_IndicationFormatter_h
#define Pegasus_IndicationFormatter_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Linkage.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/Constants.h>

#if defined(PEGASUS_HAS_ICU) && defined(PEGASUS_INDFORMATTER_USE_ICU)
#include <unicode/locid.h>
#include <unicode/datefmt.h>
#include <unicode/unistr.h>
#endif


PEGASUS_NAMESPACE_BEGIN

/** 
    The IndicationFormatter class formats an indication message.

    @author  Hewlett-Packard Company

*/
class PEGASUS_COMMON_LINKAGE IndicationFormatter
{
public:

    /**
        Based on subscription instance, gets the formatted indication text 
	message from the received indication instance. 

        @param   subscription   the subscription instance
        @param   indication     the received indication instance
        @param   contentLangs   the Content-Languages in the 
				subscription instance 

        @return  String containing the formatted indication text message 
    */
    static String getFormattedIndText(
        const CIMInstance & subscription,
        const CIMInstance & indication,
	const ContentLanguages & contentLangs);


    /**
        Validates the syntax and the provided type for the property 
	TextFormat in the instance.
        If the value of the property has a syntax error, or the 
	provided type does not match the property type,
        an exception is thrown.

        This function is called by the _canCreate function, and is used to 
        validate the syntax and the provided type for the property 
	TextFormat in the Formatted Subscription instance.

        @param   textFormatStr     the string to be validated
	@param   indicationClass   the indication class
	@param   textFormatParams  the value of the TextFormatParameters

        @throw   CIM_ERR_INVALID_PARAMETER  if there is a syntax error 
					    for the value of property 
					    TextFormat or type mismatch
     */
    static void validateTextFormat (
        const String & textFormatStr,
        const CIMClass & indicationClass,
	const Array<String> & textFormatParams);

    /**
        Validates the value of the property TextFormatParameters in the 
	subscriptionInstance.
        If the property name in the TextFormatParameters is not the name
	contained in the select clause of the associated filter query
	in the filterInstance, an exception is thrown.

        @param   propertyList        the list of properties to be validated
        @param   indicationClass     the indication class
	@param   textFormatParams    the value of the property
				     TextFormatParameters

        @throw   CIM_ERR_INVALID_PARAMETER  if the select clause of the
					    filter query does not contain
					    the provided property name
     */
    static void validateTextFormatParameters (
        const CIMPropertyList & propertyList,	
        const CIMClass & indicationClass,
	const Array<String> & textFormatParams);

private:

    /**
	gets specified indication property value from the received 
	indication instance.

	@param  propertyName  the specified property name
	@param  arrayIndexStr the specified index string of the array 
	@param  indication    the received indication instance
	@param  contentLangs  the Content Languages 
        @return String containing property value
    */
        
    static String _getIndPropertyValue(
        const String & propertyName,
	const String & arrayIndexStr,
        const CIMInstance & indication,
	const ContentLanguages & contentLangs);

    /**
	Retrieves the array values referenced by the specified
	CIMValue.

	@param  propertyValue the CIMValue 
	@param  indexStr the array index
	@param  contentLangs  the Content Languages 
	@return String containing array values referenced by the specified
		CIMValue
    */
    static String _getArrayValues(
	const CIMValue & propertyValue,
        const String & indexStr,
	const ContentLanguages & contentLangs);

    /**
        Validates the index string 

	Index string only can be an integer. Otherwise, 
        an exception is thrown.

        @param   indexStr          index string to be validated

        @throw   CIM_ERR_INVALID_PARAMETER  if the index is invalid  
     */
    static void _isValidIndex(
	const char * indexStr);

    /**
        Validates the provided property type string. 
        If the provided property type does not match the type of the property,
	or the provided property type is not valid type, or the dynamic
	content with an array index refers to a non array type, an 
	exception is thrown.

        This function is called by the _validatePropertySyntaxAndType
	function, and is used to validate the provided property type. 

        @param   indicationClass   the indication class 
        @param   propertyParam     the provided property 
        @param   propertyTypeStr   the provided property type      
	@param   isArray           indicates whether refers to an array type 

        @throw   CIM_ERR_INVALID_PARAMETER  if the provided property type is
					    unexpected
     */
    static void _validatePropertyType (
	const CIMClass & indicationClass,
	const String & propertyParam,
	const String & propertyTypeStr,
	const Boolean & isArray);

    /**
        Converts the CIMValue of the boolean to be string "true" or "false" 

        @param   booleanCIMValue   Boolean CIMValue to be converted 

        @return  the string representing the boolean CIMValue
     */
    static String _getBooleanStr(
	const CIMValue & booleanCIMValue);

    /**
        Converts the boolean value to be string "true" or "false" 

        @param   booleanValue   Boolean value to be converted 

        @return  the string representing the boolean value 
     */
    static String _getBooleanStr(
	const Boolean & booleanValue);

    /**
        Constructs a default indication text message from the received 
	indication instance. 

        @param   indication     the received indication instance
        @param   contentLangs   the Content-Languages in the 
				subscription instance 

        @return  String containing default indication text message 
    */
    static String _formatDefaultIndicationText(
        const CIMInstance & indication,
	const ContentLanguages & contentLangs);

    /**
        Constructs a human readable indication text message from the
	specified indication textFormat, textFormatParams, and
	received indication instance.

        @param   textFormat        the specified indication textFormat 
        @param   textFormatParams  the indexed array containing the names
				   of properties defined in the select
				   clause of the associated
				   CIM_IndicationFilter Query
        @param   indication        the received indication instance 
        @param   contentLangs      the Content-Languages in the 
				   subscription instance 

        @return  String containing a human readable indication text message 
    */
    static String _formatIndicationText(
	const String & textFormat,
	const Array<String>& textFormatParams,
	const CIMInstance & indication,
        const ContentLanguages & contentLangs);

#if defined(PEGASUS_HAS_ICU) && defined(PEGASUS_INDFORMATTER_USE_ICU)  
    /**
        Determines if a property value can be localized. 
        A property value can only be localized if the subscription
	ContentLanguages includes no more than one language tag.

        @param   contentLangs    the Content-Languages in the
	                         subscription instance
        @param   locale          locale to be set on return if return 
				 value is true

        @return  True, if valid locale returned
		 False, Otherwise
     */
    static Boolean _canLocalize(
	const ContentLanguages & contentLangs,
	Locale & locale);

    /**
        Localizes the CIMTYPE_DATETIME property value 

        @param   dateTimeValue  the value of datetime to be localized 
        @param   locale         locale to be used in the localization

        @return  the string representation of the datetime value 
     */
    static String _localizeDateTime(
	const CIMDateTime & dateTimeValue,
	const Locale & locale);

    /**
        Localizes the CIMTYPE_BOOLEAN property value

        @param   booleanValue   the value of the boolean to be localized 
        @param   locale         locale to be used in the localization

        @return  the string representation of the boolean value 
     */
    static String _localizeBooleanStr(
	const Boolean & booleanValue,
	const Locale & locale);

#endif

};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_IndicationFormatter_h */
