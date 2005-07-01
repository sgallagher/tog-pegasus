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
// Modified By: David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/CommonUTF.h>

#include "IndicationFormatter.h"


PEGASUS_NAMESPACE_BEGIN

void IndicationFormatter::validateTextFormat (
    const String & textStr,
    const CIMClass & indicationClass,
    const Array<String> & textFormatParams)
{
    PEG_METHOD_ENTER (TRC_IND_FORMATTER,
	"IndicationFormatter::validateTextFormat");

    String textFormatStr = textStr;
    String textFormatSubStr;
    String exceptionStr;

    Uint32 leftBrace = textFormatStr.find("{");
    Uint32 rightBrace;

    do {
           textFormatSubStr.clear();
           if (leftBrace != PEG_NOT_FOUND)
           {
               // Do not expect a right brace before the left
               // brace. e.g An invalid text format string could be:
              // "Indication occurred at 2, datetime} with
               // identify ID {3, string}"

               textFormatSubStr = textFormatStr.subString(
                   0, leftBrace);

               Uint32 rightBrace2 = textFormatSubStr.find("}");
               if (rightBrace2 != PEG_NOT_FOUND)
               {
                   textFormatSubStr = textFormatStr.subString(
	               0, (rightBrace2 + 1));
		   MessageLoaderParms parms(
		       "IndicationFormatter.IndicationFormatter._MSG_INVALID_SYNTAX_OF_FOR_PROPERTY",
		       "Invalid syntax at $0 in property $1",
		       textFormatSubStr,
		       _PROPERTY_TEXTFORMAT.getString());

		      exceptionStr.append(
			       MessageLoader::getMessage(parms));

                     PEG_METHOD_EXIT();
		     throw PEGASUS_CIM_EXCEPTION (
		        CIM_ERR_INVALID_PARAMETER, exceptionStr);
               }

               // expect right brace
               textFormatStr = textFormatStr.subString(
			   leftBrace+1, PEG_NOT_FOUND);
               rightBrace = textFormatStr.find("}");

               // Do not expect a left brace between left and right
               // braces. e.g A text string: "Indication occurred
	      // at {2, datetime with identify ID {3, string}" is
	      // an invalid format.

             if (rightBrace != PEG_NOT_FOUND)
             {
                 textFormatSubStr.clear();
                   textFormatSubStr = textFormatStr.subString(0,
		                      rightBrace);

                 Uint32 leftBrace2 = textFormatSubStr.find("{");
                 if (leftBrace2 != PEG_NOT_FOUND)
                 {
                     textFormatSubStr = textFormatStr.subString(
		         0, (leftBrace2 + 1));
			MessageLoaderParms parms(
			"IndicationFormatter.IndicationFormatter._MSG_INVALID_SYNTAX_OF_FOR_PROPERTY",
			"Invalid syntax at $0 in property $1",
			textFormatSubStr,
			_PROPERTY_TEXTFORMAT.getString());

			exceptionStr.append(
			    MessageLoader::getMessage(parms));

                     PEG_METHOD_EXIT();
		     throw PEGASUS_CIM_EXCEPTION (
			           CIM_ERR_INVALID_PARAMETER, exceptionStr);
                }

	        String propertyParam;
	        String propertyTypeStr;
	        String propertyIndexStr;
	        char propertyIndexBuffer[32];
	        Sint32 propertyIndex;

	        Uint32 comma;
	        Uint32 leftBracket;
	        Uint32 rightBracket;
	        Boolean isArray = false;

	        comma = textFormatSubStr.find(",");

	        // A dynamic content can have format either
	       // {index} or {index[x]}
	       if (comma == PEG_NOT_FOUND)
	       {
	            propertyParam =
		        textFormatSubStr.subString(0, PEG_NOT_FOUND);
                               propertyTypeStr = String::EMPTY;

		   leftBracket = textFormatSubStr.find("[");
		           rightBracket = textFormatSubStr.find("]");

	      }
	      // A dynamic content can have format either
	     // {index, type} or {index[x], type}
	     else
	     {
	          propertyParam =
		      textFormatSubStr.subString(0, comma);
		  propertyTypeStr = textFormatSubStr.subString(
		      comma +1, PEG_NOT_FOUND);

		 leftBracket = propertyParam.find("[");
		       rightBracket = propertyParam.find("]");
             }

	    // A dynamic content has syntax either
	    // {index} or {index, type}
	    if (leftBracket == PEG_NOT_FOUND)
	    {
	        // there is no left bracket, do not expect a
	        // right bracket
	        if (rightBracket != PEG_NOT_FOUND)
	        {
	            textFormatSubStr = textFormatStr.subString(
		        0, (rightBracket + 1));
		    MessageLoaderParms parms(
		        "IndicationFormatter.IndicationFormatter._MSG_INVALID_SYNTAX_OF_FOR_PROPERTY",
		        "Invalid syntax at $0 in property $1",
		        textFormatSubStr,
		        _PROPERTY_TEXTFORMAT.getString());

		    exceptionStr.append(
		        MessageLoader::getMessage(parms));

                   PEG_METHOD_EXIT();
		   throw PEGASUS_CIM_EXCEPTION (
		       CIM_ERR_INVALID_PARAMETER, exceptionStr);
	        }

		propertyIndexStr = propertyParam;
	        isArray = false;
	    }
	   // A dynamic content has syntax either
	   // {index[]} or {index[], type}
	   else
           {
	       // there is a left bracket, expect a right bracket
	       if (rightBracket == PEG_NOT_FOUND)
	       {
	           MessageLoaderParms parms(
		       "IndicationFormatter.IndicationFormatter._MSG_INVALID_SYNTAX_OF_FOR_PROPERTY",
		       "Invalid syntax at $0 in property $1",
		       textFormatSubStr,
		       _PROPERTY_TEXTFORMAT.getString());

		   exceptionStr.append(
		       MessageLoader::getMessage(parms));

                   PEG_METHOD_EXIT();
		   throw PEGASUS_CIM_EXCEPTION (
		       CIM_ERR_INVALID_PARAMETER, exceptionStr);
	       }

	           propertyIndexStr =
		       propertyParam.subString(0, leftBracket);

		   isArray = true;
           }

           sprintf(propertyIndexBuffer, "%s",
	       (const char *)propertyIndexStr.getCString());

           // skip white space
           char * indexStr = propertyIndexBuffer;
           while (*indexStr && isspace(*indexStr))
           {
               *indexStr++;
           }

           _isValidIndex(indexStr);

	   propertyIndex = atoi(indexStr);

           // check the property index
           if ((propertyIndex < 0) ||
	       ((Uint32)propertyIndex >= textFormatParams.size()))
           {
	       // property index is out of bounds
	       MessageLoaderParms parms(
	       "IndicationFormatter.IndicationFormatter._MSG_INDEX_IS_OUT_OF_BOUNDS",
	       "The value of index $0 in property $1 is out of bounds",
	       propertyIndex,
	       _PROPERTY_TEXTFORMATPARAMETERS.getString());

	       exceptionStr.append(MessageLoader::getMessage(parms));

	       PEG_METHOD_EXIT();
	       throw PEGASUS_CIM_EXCEPTION (CIM_ERR_INVALID_PARAMETER, exceptionStr);
           }

	   if (propertyTypeStr != String::EMPTY)
	   {
	       _validatePropertyType(indicationClass,
	           textFormatParams[propertyIndex],
		   propertyTypeStr, isArray);
           }

           textFormatStr = textFormatStr.subString(
	       rightBrace+1, PEG_NOT_FOUND);
          }
         else // no right brace
         {
             MessageLoaderParms parms(
	        "IndicationFormatter.IndicationFormatter._MSG_INVALID_SYNTAX_OF_FOR_PROPERTY",
	        "Invalid syntax at $0 in property $1",
	        textFormatSubStr,
	        _PROPERTY_TEXTFORMAT.getString());

	    exceptionStr.append(
	        MessageLoader::getMessage(parms));

           PEG_METHOD_EXIT();
	   throw PEGASUS_CIM_EXCEPTION (
	       CIM_ERR_INVALID_PARAMETER, exceptionStr);
         }
        }
        else // no left brace
        {
            // does not expect right brace
           rightBrace = textFormatStr.find("}");

           if (rightBrace != PEG_NOT_FOUND)
           {
	       textFormatSubStr = textFormatStr.subString(
	           0, (rightBrace + 1));
	       MessageLoaderParms parms(
	           "IndicationFormatter.IndicationFormatter._MSG_INVALID_SYNTAX_OF_FOR_PROPERTY",
	           "Invalid syntax at $0 in property $1",
		   textFormatSubStr,
		   _PROPERTY_TEXTFORMAT.getString());

		   exceptionStr.append(
		   MessageLoader::getMessage(parms));

                  PEG_METHOD_EXIT();
		  throw PEGASUS_CIM_EXCEPTION (
		    CIM_ERR_INVALID_PARAMETER, exceptionStr);
            }

           break;
       }

         leftBrace = textFormatStr.find("{");
     } while (textFormatStr.size() > 0);

    PEG_METHOD_EXIT();
}

void IndicationFormatter::_validatePropertyType (
    const CIMClass & indicationClass,
    const String & propertyParam,
    const String & typeStr,
    const Boolean & isArray)
{
    PEG_METHOD_ENTER (TRC_IND_FORMATTER,
	"IndicationFormatter::_validatePropertyType");

    String exceptionStr;
    char propertyTypeBuffer[32];
    char * providedPropertyType;
    Array <String> validPropertyTypes;

    String propertyTypeStr = typeStr;

    validPropertyTypes.append ( "boolean");
    validPropertyTypes.append ( "uint8");
    validPropertyTypes.append ( "sint8");
    validPropertyTypes.append ( "uint16");
    validPropertyTypes.append ( "sint16");
    validPropertyTypes.append ( "uint32");
    validPropertyTypes.append ( "sint32");
    validPropertyTypes.append ( "uint64");
    validPropertyTypes.append ( "sint64");
    validPropertyTypes.append ( "real32");
    validPropertyTypes.append ( "real64");
    validPropertyTypes.append ( "char16");
    validPropertyTypes.append ( "string");
    validPropertyTypes.append ( "datetime");
    validPropertyTypes.append ( "reference");

    propertyTypeStr.toLower();
    sprintf(propertyTypeBuffer, "%s",
        (const char *)propertyTypeStr.getCString());

    // skip white space
    providedPropertyType = propertyTypeBuffer;
    while (*providedPropertyType && isspace(*providedPropertyType))
    {
        providedPropertyType++;
    }

    String providedTypeStr = providedPropertyType;

    Uint32 space = providedTypeStr.find(" ");

    if (space != PEG_NOT_FOUND)
    {
	// skip the appended space from the providedTypeStr
	// e.g {1, string  }
	String restTypeStr = providedTypeStr.subString(space, PEG_NOT_FOUND);

        Uint32 i = 0;
        while (restTypeStr[i] == ' ')
        {
            i++;
        }

        restTypeStr = restTypeStr.subString(i, PEG_NOT_FOUND);
	if (strlen(restTypeStr.getCString()) == 0)
	{
	    providedTypeStr = providedTypeStr.subString(0, space);
	}
        else
        {
            // the provided property type is not a valid type
            // e.g. {1, string  xxx}
            MessageLoaderParms parms(
                "IndicationFormatter.IndicationFormatter._MSG_INVALID_TYPE_OF_FOR_PROPERTY",
            "Invalid property type of $0 in property $1",
            providedPropertyType,
            _PROPERTY_TEXTFORMAT.getString());
        } 
    }

    //
    // Checks if the provided property type is a valid type
    //
    if (!(Contains (validPropertyTypes, providedTypeStr)))
    {
	// the provided property type is not valid type
	MessageLoaderParms parms(
	"IndicationFormatter.IndicationFormatter._MSG_INVALID_TYPE_OF_FOR_PROPERTY",
	"Invalid property type of $0 in property $1",
	providedPropertyType,
	_PROPERTY_TEXTFORMAT.getString());

	exceptionStr.append(MessageLoader::getMessage(parms));

	PEG_METHOD_EXIT();
	throw PEGASUS_CIM_EXCEPTION (
	    CIM_ERR_INVALID_PARAMETER, exceptionStr);
    }

    for (Uint32 i = 0; i < indicationClass.getPropertyCount (); i++)
    {
	CIMName propertyName = indicationClass.getProperty (i).getName ();

        if (String::equalNoCase(propertyParam, (propertyName.getString())))
        {
	    // get the property type;
	    CIMType propertyType =
	                indicationClass.getProperty (i).getType();

	    // Check if the property is an array type
	    if ((isArray && !(indicationClass.getProperty(i).isArray())) ||
		(!isArray && indicationClass.getProperty(i).isArray()))
	    {
		MessageLoaderParms parms(
		"IndicationFormatter.IndicationFormatter._MSG_PROPERTY_IS_NOT_AN_ARRAY_TYPE",
		"The property $0 is not an array type",
                propertyName.getString());

	        exceptionStr.append(MessageLoader::getMessage(parms));

	        PEG_METHOD_EXIT();
	        throw PEGASUS_CIM_EXCEPTION (
	            CIM_ERR_INVALID_PARAMETER, exceptionStr);
	    }

	    // property type matchs
            if (String::equalNoCase(providedTypeStr,
	        cimTypeToString(propertyType)))
            {
	        break;
	    }
	    else
	    {
	        MessageLoaderParms parms(
	        "IndicationFormatter.IndicationFormatter._MSG_MISS_MATCHED_TYPE_OF_FOR_PROPERTY",
	        "The provided property type of $0 in $1 does not match the property type $2",
	         providedPropertyType,
		cimTypeToString(propertyType),
	        _PROPERTY_TEXTFORMAT.getString());

	        exceptionStr.append(MessageLoader::getMessage(parms));

	        PEG_METHOD_EXIT();
	        throw PEGASUS_CIM_EXCEPTION (
	            CIM_ERR_INVALID_PARAMETER, exceptionStr);
            }

        }
    }

    PEG_METHOD_EXIT();
}

void IndicationFormatter::validateTextFormatParameters (
    const CIMPropertyList & propertyList,
    const CIMClass & indicationClass,
    const Array<String> & textFormatParams)
{
    PEG_METHOD_ENTER (TRC_IND_FORMATTER,
	"IndicationFormatter::validateTextFormatParameters");

    Array <String> indicationClassProperties;
    String exceptionStr;

    // All the properties are selected
    if (propertyList.isNull ())
    {
       for (Uint32 i = 0; i < indicationClass.getPropertyCount (); i++)
       {
	   indicationClassProperties.append(
	       indicationClass.getProperty (i).getName ().getString());
       }
    }
    // partial properties are selected
    else
    {
        Array<CIMName> propertyNames = propertyList.getPropertyNameArray();

	for (Uint32 j = 0; j < propertyNames.size(); j++)
	{
	    indicationClassProperties.append(propertyNames[j].getString());
	}
    }

    // check if the textFormatParams is contained in the
    // indicationClassProperties
    for (Uint32 k = 0; k < textFormatParams.size(); k++)
    {
        if (!Contains(indicationClassProperties, textFormatParams[k]))
	{
	    // The property name in TextFormatParameters is not
	    // included in the select clause of the associated filter query
	    MessageLoaderParms parms(
	    "IndicationFormatter.IndicationFormatter._MSG_MISS_MATCHED_PROPERTY_NAME",
	    "The property name $0 in $1 does not match the properties in the select clause",
	    textFormatParams[k],
	    _PROPERTY_TEXTFORMATPARAMETERS.getString());

	    exceptionStr.append(MessageLoader::getMessage(parms));

	    PEG_METHOD_EXIT();
	    throw PEGASUS_CIM_EXCEPTION (
		CIM_ERR_INVALID_PARAMETER, exceptionStr);
	}
    }

    PEG_METHOD_EXIT();
}

void IndicationFormatter::_isValidIndex (
    const char * indexStr)
{

    PEG_METHOD_ENTER (TRC_IND_FORMATTER,
	"IndicationFormatter::_isValidIndex");

    String exceptionStr;

    String indexSubStr = indexStr;
    Uint32 space = indexSubStr.find(" ");
    if (space != PEG_NOT_FOUND)
    {
        String restIndexSubStr = indexSubStr.subString(space, PEG_NOT_FOUND);

        // skip the appended space from the indexSubStr
        Uint32 k=0;
        while (restIndexSubStr[k] == ' ') 
        {
            k++;
        }

        restIndexSubStr = restIndexSubStr.subString(k, PEG_NOT_FOUND);

        if (restIndexSubStr.size() == 0)
        {
            indexSubStr = indexSubStr.subString(0, space);
        }
        // invalid index string [12 xxx]
        else
        {
            // invalid index string
            MessageLoaderParms parms(
                "IndicationFormatter.IndicationFormatter._MSG_INVALID_INDEX",
                "Invalid index string $0",
                indexStr);

            exceptionStr.append(MessageLoader::getMessage(parms));

            PEG_METHOD_EXIT();
            throw PEGASUS_CIM_EXCEPTION (
                CIM_ERR_INVALID_PARAMETER, exceptionStr);

        }
    }

    Uint32 i=0;
    while ((indexSubStr[i] >= '0') && (indexSubStr[i] <= '9'))
    {
        i++;
    }

    // invalid index string [12xxx}
    if (i != indexSubStr.size() )
    {
        // invalid index string
        MessageLoaderParms parms(
            "IndicationFormatter.IndicationFormatter._MSG_INVALID_INDEX",
            "Invalid index string $0",
            indexStr);
        exceptionStr.append(MessageLoader::getMessage(parms));

        PEG_METHOD_EXIT();
        throw PEGASUS_CIM_EXCEPTION (
            CIM_ERR_INVALID_PARAMETER, exceptionStr);
    }

    PEG_METHOD_EXIT();
}

String IndicationFormatter::getFormattedIndText(
    const CIMInstance & subscription,
    const CIMInstance & indication,
    const ContentLanguages & contentLangs)
{
    PEG_METHOD_ENTER (TRC_IND_FORMATTER,
        "IndicationFormatter::getFormattedIndText");

    String indicationText;
    String textFormat = String::EMPTY;
    CIMValue textFormatValue;
    CIMValue textFormatParamsValue;

    Array<String> textFormatParams;

        // get TextFormat from subscription
        Uint32 textFormatPos =
	    subscription.findProperty(_PROPERTY_TEXTFORMAT);

	// if the property TextFormat is not found,
	// indication is constructed with default format
        if (textFormatPos == PEG_NOT_FOUND)
        {
            indicationText = _formatDefaultIndicationText(indication,
							 contentLangs);
        }
        else
        {
            textFormatValue = subscription.getProperty(textFormatPos).
		getValue();

	    // if the value of textFormat is NULL,
	    // indication is constructed with default format
            if (textFormatValue.isNull())
            {
                indicationText = _formatDefaultIndicationText(indication,
							     contentLangs);
	    }
	    else
	    {
                // get TextFormatParameters from subscription
                Uint32 textFormatParamsPos = subscription.findProperty(
		    _PROPERTY_TEXTFORMATPARAMETERS);

	        if (textFormatParamsPos != PEG_NOT_FOUND)
	        {
                    textFormatParamsValue = subscription.getProperty(
	                textFormatParamsPos).getValue();
                }

		// constructs indication with specified format
		if ((textFormatValue.getType() == CIMTYPE_STRING) &&
		    !(textFormatValue.isArray()))
                {
		    textFormatValue.get(textFormat);
		    if (!textFormatParamsValue.isNull())
		    {
			if ((textFormatParamsValue.getType() ==
			     CIMTYPE_STRING) &&
                            (textFormatParamsValue.isArray()))
                        {
		            textFormatParamsValue.get(textFormatParams);
                        }
		    }

		    indicationText = _formatIndicationText(textFormat,
				                          textFormatParams,
				                          indication,
				                          contentLangs);
		}
		else
		{
                    indicationText = _formatDefaultIndicationText(indication,
				                                 contentLangs);
		}
	    }

        }

        PEG_METHOD_EXIT();
	return (indicationText);
}

String IndicationFormatter::_formatDefaultIndicationText(
    const CIMInstance & indication,
    const ContentLanguages & contentLangs)
{
    PEG_METHOD_ENTER (TRC_IND_FORMATTER,
        "IndicationFormatter::_formatDefaultIndicationText");

    CIMInstance indicationInstance = indication.clone();
    String propertyName;
    String indicationStr;
    Uint32 propertyCount = indicationInstance.getPropertyCount();

    indicationStr.append("Indication (default format):");

    Boolean canLocalize = false;

#if defined(PEGASUS_HAS_ICU) && defined(PEGASUS_INDFORMATTER_USE_ICU)
    Locale locale;
    canLocalize = _canLocalize(contentLangs, locale);
#endif

    for (Uint32 i=0; i < propertyCount; i++)
    {
        CIMProperty property = indicationInstance.getProperty(i);
        propertyName = property.getName().getString();
        CIMValue propertyValue = property.getValue();
        Boolean valueIsNull = propertyValue.isNull();
        Boolean isArray = propertyValue.isArray();

        indicationStr.append(propertyName);
        indicationStr.append(" = ");

	CIMType type = propertyValue.getType();

        if (!valueIsNull)
        {
            if (isArray)
            {
		indicationStr.append(_getArrayValues(propertyValue, "",
		    contentLangs));
            }
            else // value is not an array
            {
#if defined(PEGASUS_HAS_ICU) && defined(PEGASUS_INDFORMATTER_USE_ICU)
		if (canLocalize)
		{
		    if (type == CIMTYPE_DATETIME)
		    {
			CIMDateTime dateTimeValue;
			propertyValue.get(dateTimeValue);
		        indicationStr.append(_localizeDateTime(dateTimeValue,
		    	    locale));
		    }
		    else if (type == CIMTYPE_BOOLEAN)
		    {
			Boolean booleanValue;
			propertyValue.get(booleanValue);
		        indicationStr.append(_localizeBooleanStr(booleanValue,
		    	    locale));
		    }
		    else
		    {
			indicationStr.append(propertyValue.toString());
		    }
		}
		else
		{
		    if (type == CIMTYPE_BOOLEAN)
		    {
                        indicationStr.append(_getBooleanStr(propertyValue));
		    }
		    else
		    {
                        indicationStr.append(propertyValue.toString());
		    }
		}
#else
		if (type == CIMTYPE_BOOLEAN)
		{
                    indicationStr.append(_getBooleanStr(propertyValue));
		}
		else
		{
                    indicationStr.append(propertyValue.toString());
		}
#endif
            }
        }
	else
	{
	    indicationStr.append("NULL");
	}

        if (i < propertyCount -1)
        {
            indicationStr.append(", ");
        }

        propertyName.clear();
    }

    PEG_METHOD_EXIT();

    return (indicationStr);
}

String IndicationFormatter::_formatIndicationText(
    const String & textFormat,
    const Array<String>& textFormatParams,
    const CIMInstance & indication,
    const ContentLanguages & contentLangs)
{
    PEG_METHOD_ENTER (TRC_IND_FORMATTER,
        "IndicationFormatter::_formatIndicationText");

    String indicationText;
    String textStr;
    String indicationFormat = textFormat;
    String propertyValue;
    String propertyParam;

    String propertyIndexStr;
    char propertyIndexBuffer[16];
    Sint32 propertyIndex;

    Uint32 leftBrace = textFormat.find("{");
    Uint32 rightBrace;
    Uint32 comma;
    Uint32 leftBracket;
    Uint32 rightBracket;

    String arrayIndexStr;

    indicationText.clear();

    // Parsing the specified indication text format.
    // As an example, a format string for a UPS AlertIndication
    // could be defined as follows: A {4, string} UPS Alert was
    // detected on the device {6[1]}.
    while (leftBrace != PEG_NOT_FOUND)
    {
	textStr.clear();
	propertyParam.clear();
	propertyIndexStr.clear();
	arrayIndexStr.clear();

        // there is a left brace
        textStr = indicationFormat.subString(0, leftBrace);

        indicationText.append(textStr);

        indicationFormat = indicationFormat.subString(leftBrace+1,
                                                      PEG_NOT_FOUND);
        rightBrace = indicationFormat.find("}");

        // expecting a right brace
        if (rightBrace != PEG_NOT_FOUND)
        {
            // gets property index which is inside braces.
            // The supported formats are: {index} or {index, type}
	    // or {index[x]} or {index[x], type}
            propertyParam = indicationFormat.subString(0,
                                                       rightBrace);
            comma = propertyParam.find(",");

	    // A dynamic content has syntax {index, type} or {index[x], type}
            if (comma != PEG_NOT_FOUND)
            {
		propertyParam = propertyParam.subString(0, comma);
            }

	    leftBracket = propertyParam.find("[");

	    // A dynamic content has syntax {index} or {index, type}
	    if (leftBracket == PEG_NOT_FOUND)
	    {
                propertyIndexStr = propertyParam;
	    }
	    // A dynamic content has syntax {index[x]} or {index[x], type}
	    else
	    {
	        propertyIndexStr = propertyParam.subString(0,leftBracket);

		propertyParam = propertyParam.subString(
		    leftBracket, PEG_NOT_FOUND);

	        rightBracket = propertyParam.find("]");

		arrayIndexStr = propertyParam.subString(1, rightBracket-1);
	    }

            sprintf(propertyIndexBuffer, "%s", (const char *)
	        propertyIndexStr.getCString());
	    try
	    {
		_isValidIndex(propertyIndexBuffer);
                propertyIndex = atoi(propertyIndexBuffer);
	    }
	    catch (CIMException & c)
	    {
		propertyIndex = -1;

		PEG_TRACE_STRING(TRC_IND_FORMATTER, Tracer::LEVEL4, c.getMessage());
	    }


            // property index is out of range
            if ((propertyIndex < 0) ||
                ((Uint32)propertyIndex >= textFormatParams.size()))
            {
		propertyValue = "UNKNOWN";
            }
            else
	    {

                // get indication property value
                propertyValue = _getIndPropertyValue(
                    textFormatParams[propertyIndex],
		    arrayIndexStr,
                    indication,
		    contentLangs);
            }

            indicationText.append(propertyValue);
        }

        indicationFormat = indicationFormat.subString(rightBrace+1,
                                                      PEG_NOT_FOUND);

        leftBrace = indicationFormat.find("{");

    }

    indicationText.append(indicationFormat);

    PEG_METHOD_EXIT();

    return (indicationText);
}

String IndicationFormatter::_getIndPropertyValue(
    const String & specifiedPropertyName,
    const String & arrayIndexStr,
    const CIMInstance & indication,
    const ContentLanguages & contentLangs)
{
    PEG_METHOD_ENTER (TRC_IND_FORMATTER,
        "IndicationFormatter::_getIndPropertyValue");

    CIMInstance indicationInstance = indication.clone();
    String propertyName;

    Boolean canLocalize = false;

#if defined(PEGASUS_HAS_ICU) && defined(PEGASUS_INDFORMATTER_USE_ICU)
    Locale locale;
    canLocalize = _canLocalize(contentLangs, locale);
#endif

    for (Uint32 i=0; i < indicationInstance.getPropertyCount(); i++)
    {
        CIMProperty property = indicationInstance.getProperty(i);
        propertyName = property.getName().getString();

        // get specified property value
        if (String::equalNoCase(propertyName, specifiedPropertyName))
        {
            CIMValue propertyValue = property.getValue();
            Boolean valueIsNull = propertyValue.isNull();
	    CIMType type = propertyValue.getType();

            if (!valueIsNull)
            {
                Boolean isArray = propertyValue.isArray();

                if (isArray)
                {
                    PEG_METHOD_EXIT();
		    return (_getArrayValues(propertyValue, arrayIndexStr,
			                    contentLangs));
                }
                else // value is not an array
                {
#if defined(PEGASUS_HAS_ICU) && defined(PEGASUS_INDFORMATTER_USE_ICU)
		    if (canLocalize)
		    {
                        if (type == CIMTYPE_DATETIME)
		        {
			    CIMDateTime dateTimeValue;
			    propertyValue.get(dateTimeValue);
                            PEG_METHOD_EXIT();
			    return(_localizeDateTime(dateTimeValue, locale));
		        }
                        else if (type == CIMTYPE_BOOLEAN)
		        {
			    Boolean booleanValue;
			    propertyValue.get(booleanValue);
                            PEG_METHOD_EXIT();
			    return(_localizeBooleanStr(booleanValue, locale));
		        }
                        else
                        {
                            PEG_METHOD_EXIT();
                            return (propertyValue.toString());
                        }
		    }
		    else
		    {
			if (type == CIMTYPE_BOOLEAN)
			{
                            PEG_METHOD_EXIT();
                            return (_getBooleanStr(propertyValue));
			}
			else
			{
                            PEG_METHOD_EXIT();
                            return (propertyValue.toString());
			}
		    }
#else
		    if (type == CIMTYPE_BOOLEAN)
		    {
                        PEG_METHOD_EXIT();
                        return (_getBooleanStr(propertyValue));
		    }
		    else
		    {
                        PEG_METHOD_EXIT();
                        return (propertyValue.toString());
		    }
#endif
                }

            }
            else // value is NULL
            {
                PEG_METHOD_EXIT();
                return ("NULL");
            }

        }
        propertyName.clear();
    }

    PEG_METHOD_EXIT();

    return ("UNKNOWN");
}

String IndicationFormatter::_getArrayValues(
    const CIMValue & propertyValue,
    const String & arrayIndexStr,
    const ContentLanguages & contentLangs)
{
    PEG_METHOD_ENTER (TRC_IND_FORMATTER,
	"IndicationFormatter::_getArrayValues");

    CIMType type = propertyValue.getType();
    String arrayValues;
    char propertyValueBuffer[2048];
    Uint32 arraySize = propertyValue.getArraySize();

    char arrayIndexBuffer[16];
    Sint32 arrayIndex = 0;
    Uint32 sizeOfArrayIndexStr = arrayIndexStr.size();

    // there is an index value enclosed in brackets (e.g. [2])
    if (sizeOfArrayIndexStr != 0)
    {
        sprintf(arrayIndexBuffer, "%s", (const char *)
	    arrayIndexStr.getCString());

	try
	{
	    _isValidIndex(arrayIndexBuffer);
            arrayIndex = atoi(arrayIndexBuffer);
	}
	catch (CIMException & c)
	{
	    arrayIndex = -1;

	    PEG_TRACE_STRING(TRC_IND_FORMATTER, Tracer::LEVEL4,c.getMessage());
	}
    }

    // Array index is out of range
    if (sizeOfArrayIndexStr != 0 &&
	((arrayIndex < 0) || ((Uint32)arrayIndex >= arraySize)))
    {
	arrayValues = "UNKNOWN";

        PEG_METHOD_EXIT();
        return (arrayValues);
    }

    switch (type)
    {
	case CIMTYPE_UINT8:
	{
	    Array<Uint8> propertyValueUint8;
	    propertyValue.get(propertyValueUint8);

	    // Empty brackets (e.g. []), gets all values of the array
	    if (sizeOfArrayIndexStr == 0)
	    {
                arrayValues.append("[");
		for (Uint32 i=0; i<arraySize; i++)
		{
		    sprintf(propertyValueBuffer, "%u",
			propertyValueUint8[i]);
		    arrayValues.append(propertyValueBuffer);

		    if ( i < arraySize-1)
		    {
			arrayValues.append(",");
		    }
		}

		arrayValues.append("]");
	    }
	    else
	    {
	        sprintf(propertyValueBuffer, "%u",
		    propertyValueUint8[arrayIndex]);
	        arrayValues = propertyValueBuffer;
	    }

            break;
	}

	case CIMTYPE_UINT16:
	{
	    Array<Uint16> propertyValueUint16;
	    propertyValue.get(propertyValueUint16);

	    // Empty brackets (e.g. []), gets all values of the array
	    if (sizeOfArrayIndexStr == 0)
	    {
                arrayValues.append("[");
		for (Uint32 i=0; i<arraySize; i++)
		{
		    sprintf(propertyValueBuffer, "%u",
			propertyValueUint16[i]);
		    arrayValues.append(propertyValueBuffer);

		    if ( i < arraySize-1)
		    {
			arrayValues.append(",");
		    }
		}

		arrayValues.append("]");
	    }
	    else
	    {
	        sprintf(propertyValueBuffer, "%u",
		    propertyValueUint16[arrayIndex]);
	        arrayValues = propertyValueBuffer;
	    }

            break;
	}

	case CIMTYPE_UINT32:
	{
	    Array<Uint32> propertyValueUint32;
	    propertyValue.get(propertyValueUint32);

	    // Empty brackets (e.g. []), gets all values of the array
	    if (sizeOfArrayIndexStr == 0)
	    {
                arrayValues.append("[");
		for (Uint32 i=0; i<arraySize; i++)
		{
		    sprintf(propertyValueBuffer, "%u",
			propertyValueUint32[i]);
		    arrayValues.append(propertyValueBuffer);

		    if ( i < arraySize-1)
		    {
			arrayValues.append(",");
		    }
		}

		arrayValues.append("]");
	    }
	    else
	    {
	        sprintf(propertyValueBuffer, "%u",
		    propertyValueUint32[arrayIndex]);
	        arrayValues = propertyValueBuffer;
	    }

            break;
	}

	case CIMTYPE_UINT64:
	{
	    Array<Uint64> propertyValueUint64;
	    propertyValue.get(propertyValueUint64);

	    // Empty brackets (e.g. []), gets all values of the array
	    if (sizeOfArrayIndexStr == 0)
	    {
                arrayValues.append("[");
		for (Uint32 i=0; i<arraySize; i++)
		{
		    sprintf(propertyValueBuffer,
			    "%" PEGASUS_64BIT_CONVERSION_WIDTH "u",
			    propertyValueUint64[i]);
		    arrayValues.append(propertyValueBuffer);

		    if ( i < arraySize-1)
		    {
			arrayValues.append(",");
		    }
		}

		arrayValues.append("]");
	    }
	    else
	    {
	        sprintf(propertyValueBuffer,
			"%" PEGASUS_64BIT_CONVERSION_WIDTH "u",
		        propertyValueUint64[arrayIndex]);
	        arrayValues = propertyValueBuffer;
	    }

            break;
	}

	case CIMTYPE_SINT8:
	{
	    Array<Sint8> propertyValueSint8;
	    propertyValue.get(propertyValueSint8);

	    // Empty brackets (e.g. []), gets all values of the array
	    if (sizeOfArrayIndexStr == 0)
	    {
                arrayValues.append("[");
		for (Uint32 i=0; i<arraySize; i++)
		{
		    sprintf(propertyValueBuffer, "%i",
			propertyValueSint8[i]);
		    arrayValues.append(propertyValueBuffer);

		    if ( i < arraySize-1)
		    {
			arrayValues.append(",");
		    }
		}

		arrayValues.append("]");
	    }
	    else
	    {
	        sprintf(propertyValueBuffer, "%i",
		    propertyValueSint8[arrayIndex]);
	        arrayValues = propertyValueBuffer;
	    }

            break;
	}

	case CIMTYPE_SINT16:
	{
	    Array<Sint16> propertyValueSint16;
	    propertyValue.get(propertyValueSint16);

	    // Empty brackets (e.g. []), gets all values of the array
	    if (sizeOfArrayIndexStr == 0)
	    {
                arrayValues.append("[");
		for (Uint32 i=0; i<arraySize; i++)
		{
		    sprintf(propertyValueBuffer, "%i",
			propertyValueSint16[i]);
		    arrayValues.append(propertyValueBuffer);

		    if ( i < arraySize-1)
		    {
			arrayValues.append(",");
		    }
		}

		arrayValues.append("]");
	    }
	    else
	    {
	        sprintf(propertyValueBuffer, "%i",
		    propertyValueSint16[arrayIndex]);
	        arrayValues = propertyValueBuffer;
	    }

            break;
	}

	case CIMTYPE_SINT32:
	{
	    Array<Sint32> propertyValueSint32;
	    propertyValue.get(propertyValueSint32);

	    // Empty brackets (e.g. []), gets all values of the array
	    if (sizeOfArrayIndexStr == 0)
	    {
                arrayValues.append("[");
		for (Uint32 i=0; i<arraySize; i++)
		{
		    sprintf(propertyValueBuffer, "%i",
			propertyValueSint32[i]);
		    arrayValues.append(propertyValueBuffer);

		    if ( i < arraySize-1)
		    {
			arrayValues.append(",");
		    }
		}

		arrayValues.append("]");
	    }
	    else
	    {
	        sprintf(propertyValueBuffer, "%i",
		    propertyValueSint32[arrayIndex]);
	        arrayValues = propertyValueBuffer;
	    }

            break;
	}

	case CIMTYPE_SINT64:
	{
	    Array<Sint64> propertyValueSint64;
	    propertyValue.get(propertyValueSint64);

	    // Empty brackets (e.g. []), gets all values of the array
	    if (sizeOfArrayIndexStr == 0)
	    {
                arrayValues.append("[");
		for (Uint32 i=0; i<arraySize; i++)
		{
		    sprintf(propertyValueBuffer,
                            "%" PEGASUS_64BIT_CONVERSION_WIDTH "d",
                            propertyValueSint64[i]);
		    arrayValues.append(propertyValueBuffer);

		    if ( i < arraySize-1)
		    {
			arrayValues.append(",");
		    }
		}

		arrayValues.append("]");
	    }
	    else
	    {
	        sprintf(propertyValueBuffer,
			"%" PEGASUS_64BIT_CONVERSION_WIDTH "d",
                        propertyValueSint64[arrayIndex]);
	        arrayValues = propertyValueBuffer;
	    }

            break;
	}

	case CIMTYPE_REAL32:
	{
	    Array<Real32> propertyValueReal32;
	    propertyValue.get(propertyValueReal32);

	    // Empty brackets (e.g. []), gets all values of the array
	    if (sizeOfArrayIndexStr == 0)
	    {
                arrayValues.append("[");
		for (Uint32 i=0; i<arraySize; i++)
		{
		    sprintf(propertyValueBuffer, "%f",
			propertyValueReal32[i]);
		    arrayValues.append(propertyValueBuffer);

		    if ( i < arraySize-1)
		    {
			arrayValues.append(",");
		    }
		}

		arrayValues.append("]");
	    }
	    else
	    {
	        sprintf(propertyValueBuffer, "%f",
		    propertyValueReal32[arrayIndex]);
	        arrayValues = propertyValueBuffer;
	    }

            break;
	}

	case CIMTYPE_REAL64:
	{
	    Array<Real64> propertyValueReal64;
	    propertyValue.get(propertyValueReal64);

	    // Empty brackets (e.g. []), gets all values of the array
	    if (sizeOfArrayIndexStr == 0)
	    {
                arrayValues.append("[");
		for (Uint32 i=0; i<arraySize; i++)
		{
		    sprintf(propertyValueBuffer, "%f",
			propertyValueReal64[i]);
		    arrayValues.append(propertyValueBuffer);

		    if ( i < arraySize-1)
		    {
			arrayValues.append(",");
		    }
		}

		arrayValues.append("]");
	    }
	    else
	    {
	        sprintf(propertyValueBuffer, "%f",
		    propertyValueReal64[arrayIndex]);
	        arrayValues = propertyValueBuffer;
	    }

            break;
	}

	case CIMTYPE_BOOLEAN:
	{
	    Array<Boolean> booleanValue;
	    propertyValue.get(booleanValue);

	    Boolean canLocalize = false;

#if defined(PEGASUS_HAS_ICU) && defined(PEGASUS_INDFORMATTER_USE_ICU)
	    Locale locale;
	    canLocalize = _canLocalize(contentLangs, locale);
#endif

	    // Empty brackets (e.g. []), gets all values of the array
	    if (sizeOfArrayIndexStr == 0)
	    {
                arrayValues.append("[");
		for (Uint32 i=0; i<arraySize; i++)
		{
#if defined(PEGASUS_HAS_ICU) && defined(PEGASUS_INDFORMATTER_USE_ICU)
		    if (canLocalize)
		    {
			arrayValues.append(_localizeBooleanStr(
			    booleanValue[i], locale));
		    }
		    else
		    {
			arrayValues.append(_getBooleanStr(booleanValue[i]));
		    }
#else
		    arrayValues.append(_getBooleanStr(booleanValue[i]));

#endif
		    if ( i < arraySize-1)
		    {
			arrayValues.append(",");
		    }
		}

		arrayValues.append("]");
	    }
	    else
	    {
#if defined(PEGASUS_HAS_ICU) && defined(PEGASUS_INDFORMATTER_USE_ICU)
                if (canLocalize)
		{
		    arrayValues = _localizeBooleanStr(
			booleanValue[arrayIndex], locale);
		}
		else
		{
		    arrayValues = _getBooleanStr(booleanValue[arrayIndex]);
		}
#else
		arrayValues = _getBooleanStr(booleanValue[arrayIndex]);
#endif
	    }

            break;
	}

	case CIMTYPE_CHAR16:
	{
	    Array<Char16> propertyValueChar16;
	    propertyValue.get(propertyValueChar16);

	    // Empty brackets (e.g. []), gets all values of the array
	    if (sizeOfArrayIndexStr == 0)
	    {
                arrayValues.append("[");
		for (Uint32 i=0; i<arraySize; i++)
		{
		    arrayValues.append(propertyValueChar16[i]);

		    if ( i < arraySize-1)
		    {
			arrayValues.append(",");
		    }
		}

		arrayValues.append("]");
	    }
	    else
	    {
	        arrayValues.append(propertyValueChar16[arrayIndex]);
	    }

            break;
	}

	case CIMTYPE_STRING:
	{
	    Array<String> propertyValueString;
	    propertyValue.get(propertyValueString);

	    // Empty brackets (e.g. []), gets all values of the array
	    if (sizeOfArrayIndexStr == 0)
	    {
                arrayValues.append("[");
		for (Uint32 i=0; i<arraySize; i++)
		{
		    arrayValues.append(propertyValueString[i]);

		    if ( i < arraySize-1)
		    {
			arrayValues.append(",");
		    }
		}

		arrayValues.append("]");
	    }
	    else
	    {
	        arrayValues.append(propertyValueString[arrayIndex]);
	    }

            break;
	}

	case CIMTYPE_DATETIME:
	{
	    Array<CIMDateTime> propertyValueDateTime;
	    propertyValue.get(propertyValueDateTime);

            Boolean canLocalize = false;

#if defined(PEGASUS_HAS_ICU) && defined(PEGASUS_INDFORMATTER_USE_ICU)
	    Locale locale;
	    canLocalize = _canLocalize(contentLangs, locale);
#endif

	    // Empty brackets (e.g. []), gets all values of the array
	    if (sizeOfArrayIndexStr == 0)
	    {
                arrayValues.append("[");
		for (Uint32 i=0; i<arraySize; i++)
		{
#if defined(PEGASUS_HAS_ICU) && defined(PEGASUS_INDFORMATTER_USE_ICU)
                    if (canLocalize)
		    {
			arrayValues.append(_localizeDateTime(
			    propertyValueDateTime[i], locale));
		    }
		    else
		    {
		        arrayValues.append(propertyValueDateTime[i].toString());
		    }
#else
		    arrayValues.append(propertyValueDateTime[i].toString());
#endif

		    if ( i < arraySize-1)
		    {
			arrayValues.append(",");
		    }
		}

		arrayValues.append("]");
	    }
	    else
	    {
#if defined(PEGASUS_HAS_ICU) && defined(PEGASUS_INDFORMATTER_USE_ICU)
		if (canLocalize)
		{
		    arrayValues.append(_localizeDateTime(
			propertyValueDateTime[arrayIndex], locale));
		}
		else
		{
		    arrayValues.append(propertyValueDateTime
			[arrayIndex].toString());
		}
#else
	        arrayValues.append(propertyValueDateTime
		    [arrayIndex].toString());
#endif
	    }

            break;
	}

	case CIMTYPE_REFERENCE:
	{
	    Array<CIMObjectPath> propertyValueRef;
	    propertyValue.get(propertyValueRef);

	    // Empty brackets (e.g. []), gets all values of the array
	    if (sizeOfArrayIndexStr == 0)
	    {
                arrayValues.append("[");
		for (Uint32 i=0; i<arraySize; i++)
		{
		    arrayValues.append(propertyValueRef[i].toString());

		    if ( i < arraySize-1)
		    {
			arrayValues.append(",");
		    }
		}

		arrayValues.append("]");
	    }
	    else
	    {
	        arrayValues.append(propertyValueRef
		    [arrayIndex].toString());
	    }

            break;
	}

	default:
	{
	    arrayValues.append("UNKNOWN");

            PEG_TRACE_STRING(TRC_IND_FORMATTER, Tracer::LEVEL4,
		"Unknown CIMType: " + type);

            break;
	}
    }

    PEG_METHOD_EXIT();
    return (arrayValues);
}

String IndicationFormatter::_getBooleanStr(
    const CIMValue & booleanCIMValue)
{

    PEG_METHOD_ENTER (TRC_IND_FORMATTER,
	"IndicationFormatter::_getBooleanStr");

    Boolean propertyValueBoolean;
    booleanCIMValue.get(propertyValueBoolean);

    if (propertyValueBoolean)
    {
        PEG_METHOD_EXIT();
        return ("true");
    }
    else
    {
        PEG_METHOD_EXIT();
        return ("false");
    }
}

String IndicationFormatter::_getBooleanStr(
    const Boolean & booleanValue)
{

    PEG_METHOD_ENTER (TRC_IND_FORMATTER,
	"IndicationFormatter::_getBooleanStr");

    if (booleanValue)
    {
        PEG_METHOD_EXIT();
        return ("true");
    }
    else
    {
        PEG_METHOD_EXIT();
        return ("false");
    }
}

#if defined(PEGASUS_HAS_ICU) && defined(PEGASUS_INDFORMATTER_USE_ICU)

Boolean IndicationFormatter::_canLocalize(
    const ContentLanguages & contentLangs,
    Locale & locale)
{
    PEG_METHOD_ENTER (TRC_IND_FORMATTER,
	"IndicationFormatter::_canLocalize");

    if (!InitializeICU::initICUSuccessful())
    {
	return (false);
    }

    Array<ContentLanguageElement> elements;
    contentLangs.getAllLanguageElements(elements);

    // If the Content-Languages has multiple language tag, do not localize
    if (elements.size() > 1)
    {
        // there is more then one language tags
	PEG_METHOD_EXIT();
	return (false);
    }
    else if (elements.size() == 1)
    {
        // get the locale
        String language = elements[0].getLanguage();
        String country = elements[0].getCountry();
        String variant = elements[0].getVariant();

        locale = Locale((const char *) language.getCString(),
			(const char *) country.getCString(),
			(const char *) variant.getCString());

        // the locale is bogus state
        if (locale.isBogus())
        {
	    PEG_METHOD_EXIT();
	    return (false);
        }
        else
        {
	    PEG_METHOD_EXIT();
	    return (true);
        }
    }
    else
    {
	locale = Locale::getDefault();

        PEG_METHOD_EXIT();
        return (true);
    }

}

String IndicationFormatter::_localizeDateTime(
    const CIMDateTime & propertyValueDateTime,
    const Locale & locale)
{

    PEG_METHOD_ENTER (TRC_IND_FORMATTER,
	"IndicationFormatter::_localizeDateTime");

    // Convert dateTimeValue to be microSeconds,
    // the number of microseconds from the epoch starting
    // 0/0/0000 (12 am Jan 1, 1BCE)
    //

    CIMDateTime dateTimeValue = propertyValueDateTime;
    Uint64 dateTimeValueInMicroSecs =
        dateTimeValue.toMicroSeconds();

    // In ICU, as UTC milliseconds from the epoch starting
    // (1 January 1970 0:00 UTC)
    CIMDateTime dt;
    dt.set("19700101000000.000000+000");

    // Convert dateTimeValue to be milliSeconds,
    // the number of milliSeconds from the epoch starting
    // (1 January 1970 0:00 UTC)
    UDate dateTimeValueInMilliSecs =
       (Sint64)(dateTimeValueInMicroSecs - dt.toMicroSeconds())/1000;

    // Create a formatter for DATE and TIME with medium length
    // such as Jan 12, 1952 3:30:32pm
    DateFormat *fmt;

    try
    {
        if (locale == 0)
        {
            fmt = DateFormat::createDateTimeInstance(DateFormat::MEDIUM,
                                                     DateFormat::MEDIUM);
        }
        else
        {
            fmt = DateFormat::createDateTimeInstance(DateFormat::MEDIUM,
                                                     DateFormat::MEDIUM,
                                                     locale);
        }
    }
    catch(Exception& e)
    {
        PEG_TRACE_STRING(TRC_IND_FORMATTER, Tracer::LEVEL4, e.getMessage());
        PEG_METHOD_EXIT();
        return (dateTimeValue.toString());
    }
    catch(...)
    {
        PEG_TRACE_STRING(TRC_IND_FORMATTER, Tracer::LEVEL4,
        "Caught General Exception During DateFormat::createDateTimeInstance");

        PEG_METHOD_EXIT();
        return (dateTimeValue.toString());
    }

    if (fmt == 0)
    {
        PEG_TRACE_STRING(TRC_IND_FORMATTER, Tracer::LEVEL4,
            "Memory allocation error creating DateTime instance.");
        PEG_METHOD_EXIT();
        return (dateTimeValue.toString());
    }

    // Format the Date and Time
    UErrorCode status = U_ZERO_ERROR;
    UnicodeString dateTimeUniStr;
    fmt->format(dateTimeValueInMilliSecs, dateTimeUniStr, status);

    if (U_FAILURE(status))
    {
        delete fmt;
        PEG_METHOD_EXIT();
        return (dateTimeValue.toString());
    }

    // convert UnicodeString to char *
    char dateTimeBuffer[256];
    char *extractedStr = 0;

    // Copy the contents of the string into dateTimeBuffer
    Uint32 strLen = dateTimeUniStr.extract(0, sizeof(dateTimeBuffer),
					   dateTimeBuffer);

    // There is not enough space in dateTimeBuffer
    if (strLen > sizeof(dateTimeBuffer))
    {
	extractedStr = new char[strLen + 1];
	strLen = dateTimeUniStr.extract(0, strLen + 1, extractedStr);
    }
    else
    {
	extractedStr = dateTimeBuffer;
    }

    String datetimeStr = extractedStr;

    if (extractedStr != dateTimeBuffer)
    {
	delete extractedStr;
    }

    delete fmt;

    PEG_METHOD_EXIT();
    return (datetimeStr);
}

String IndicationFormatter::_localizeBooleanStr(
    const Boolean & booleanValue,
    const Locale & locale)
{

    PEG_METHOD_ENTER (TRC_IND_FORMATTER,
	"IndicationFormatter::_localizeBooleanStr");

    if (booleanValue)
    {
	MessageLoaderParms parms(
	    "IndicationFormatter.IndicationFormatter._MSG_BOOLEAN_TRUE",
	    "true");

        PEG_METHOD_EXIT();
        return (MessageLoader::getMessage(parms));
    }
    else
    {
	MessageLoaderParms parms(
	    "IndicationFormatter.IndicationFormatter._MSG_BOOLEAN_FALSE",
	    "false");

        PEG_METHOD_EXIT();
        return (MessageLoader::getMessage(parms));
    }
}
#endif

PEGASUS_NAMESPACE_END
