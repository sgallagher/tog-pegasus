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
//         (carolann_graves@hp.com)
//
// Modified By: Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
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
 *  Exception message string indicating missing &lt;SIMPLEREQ&gt; or
 *  &lt;MULTIREQ&gt; element.
 *
 */
static const char MISSING_ELEMENT_REQ [] = 
    "expected SIMPLEREQ or MULTIREQ element";
    
//l10n
static const char MISSING_ELEMENT_REQ_KEY [] = 
	"Common.XmlConstants.MISSING_ELEMENT_REQ";

/**
 *
 *  Exception message string indicating missing &lt;LOCALCLASSPATH&gt; or
 *  &lt;LOCALINSTANCEPATH&gt; element.
 *
 */
static const char MISSING_ELEMENT_LOCALPATH [] = 
    "expected LOCALCLASSPATH or LOCALINSTANCEPATH element";

//l10n
static const char MISSING_ELEMENT_LOCALPATH_KEY [] =
	"Common.XmlConstants.MISSING_ELEMENT_LOCALPATH_KEY";
PEGASUS_NAMESPACE_END

#endif /* Pegasus_XmlConstants_h */
