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
// Author: Carol Ann Krug Graves, Hewlett-Packard Company 
//         (carolann_graves@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_XmlConstants_h
#define Pegasus_XmlConstants_h

// REVIEW: Consider moving these to classes which they are associated with.

PEGASUS_NAMESPACE_BEGIN

//
//  This include file contains constants related to the CIM XML encoding,
//  including XML elements, and exception message strings appropriate for 
//  XML validation exception conditions.
//

//
//  XML Elements
//

/**
 *  The element used to define a local namespace path.
 */
static const char XML_ELEMENT_LOCALNAMESPACEPATH [] = "LOCALNAMESPACEPATH";

/**
 *  The element used to define a namespace component of a namespace path.
 */
static const char XML_ELEMENT_NAMESPACE [] = "NAMESPACE";

/**
 *  The element used to define a multiple CIM operation request.
 */
static const char XML_ELEMENT_MULTIREQ [] = "MULTIREQ";

/**
 *  The element used to define a simple CIM operation request.
 */
static const char XML_ELEMENT_SIMPLEREQ [] = "SIMPLEREQ";

/**
 *  The element used to define an inrinsic method invocation.
 */
static const char XML_ELEMENT_IMETHODCALL [] = "IMETHODCALL";

/**
 *  The element used to define an extrinsic method invocation.
 */
static const char XML_ELEMENT_METHODCALL [] = "METHODCALL";

/**
 *  The element used to define a local path to a CIM class.
 */
static const char XML_ELEMENT_LOCALCLASSPATH [] = "LOCALCLASSPATH";

/**
 *  The element used to define a local path to a CIM instance.
 */
static const char XML_ELEMENT_LOCALINSTANCEPATH [] = "LOCALINSTANCEPATH";


//
//  Exception Message Strings
//

/**
 *
 *  Exception message string indicating missing &lt;MESSAGE&gt; element.
 *
 */
static const char MISSING_ELEMENT_MESSAGE [] = 
    "Failed to find <MESSAGE> element";

/**
 *
 *  Exception message string indicating missing &lt;SIMPLEREQ&gt; or
 *  &lt;MULTIREQ&gt; element.
 *
 */
static const char MISSING_ELEMENT_REQ [] = 
    "Failed to find <SIMPLEREQ> or <MULTIREQ> element";

/**
 *
 *  Exception message string indicating missing &lt;METHODCALL&gt; or
 *  &lt;IMETHODCALL&gt; element.
 *
 */
static const char MISSING_ELEMENT_METHODCALL [] = 
    "Failed to find <METHODCALL> or <IMETHODCALL> element";

/**
 *
 *  Exception message string indicating missing &lt;LOCALNAMESPACEPATH&gt; 
 *  element.
 *
 */
static const char MISSING_ELEMENT_LOCALNAMESPACEPATH [] = 
    "Failed to find <LOCALNAMESPACEPATH> element";

/**
 *
 *  Exception message string indicating missing &lt;LOCALCLASSPATH&gt; or
 *  &lt;LOCALINSTANCEPATH&gt; element.
 *
 */
static const char MISSING_ELEMENT_LOCALPATH [] = 
    "Failed to find <LOCALCLASSPATH> or <LOCALINSTANCEPATH> element";

/**
 *
 *  Exception message string indicating missing &lt;CLASSNAME&gt; element.
 *
 */
static const char MISSING_ELEMENT_CLASSNAME [] = 
    "Failed to find <CLASSNAME> element";

/**
 *
 *  Exception message string indicating missing &lt;INSTANCENAME&gt; element.
 *
 */
static const char MISSING_ELEMENT_INSTANCENAME [] = 
    "Failed to find <INSTANCENAME> element";


PEGASUS_NAMESPACE_END

#endif /* Pegasus_XmlConstants_h */
