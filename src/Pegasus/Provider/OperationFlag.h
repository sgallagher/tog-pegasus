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
// Author: Chip Vincent (cvincent@us.ibm.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_OperationFlag_h
#define Pegasus_OperationFlag_h

#include <Pegasus/Common/Config.h>

PEGASUS_NAMESPACE_BEGIN

/**
Flags that instruct a provider to modify the
behavior of certain operations.

<p>This class defines symbols for flags that can be
set in a parameter passed to various provider interface
functions.</p>
*/
class PEGASUS_PROVIDER_LINKAGE OperationFlag
{
public:
    /**
    */
    static const Uint32 NONE;

    /**
    Include only elements of the specified class.
    
    <p>The <tt>LOCAL_ONLY</tt> flag instructs the provider to
    return only elements (properties and qualifiers) that are
    defined or overridden in the class named in the operation.</p>
    <p>Support of this flag is optional.</p>
    */
    static const Uint32 LOCAL_ONLY;

    /**
    Include qualifiers in the returned result.
    
    <p>The <tt>INCLUDE_QUALIFIERS</tt> flag instructs the provider
    to include all qualifiers in instances returned. Qualifiers
    can be added to an {@link CIMInstance instance} and to a
    {@link CIMProperty property} or a {@link CIMMethod method}
    within an instance with the
    <tt>addQualifier</tt> functions of these objects.</p>
    */
    static const Uint32 INCLUDE_QUALIFIERS;

    /**
    Include the class in which the element is defined.
    
    <p>The <tt>INCLUDE_CLASS_ORIGIN</tt> flag instructs the
    provider to include, as an attribute on properties and
    methods, the class in which they were defined
    or overridden. When a provider is returning an instance,
    some of the properties and methods of the class may be
    inherited, and others may be defined or overridden in the
    class that was specified in the operation. This flag
    instructs the provider to indicate the class origin for
    each element. The class name can be returned as an
    attribute on a {@link CIMProperty property} or
    {@link CIMMethod method} with the <tt>setClassOrigin</tt>
    functions of these objects.</p>
    */
    static const Uint32 INCLUDE_CLASS_ORIGIN;

    /**
    Include elements of all classes down to the creation
    class of the element being returned.
    
    <p>The <tt>DEEP_INHERITANCE</tt> flag instructs the provider
    to return all properties and qualifiers of the instance(s)
    being returned, regardless of the class specified in the
    operation. In an enumeration operation, instances may be
    returned for subclasses of the class that was named in the
    operation. When this flag is set, these instances must
    include elements that may have been added in the subclasses.
    When the flag is not set, the provider returns instances
    containing only the elements of the specified class.</p>
    
    <p>Note that <tt>DEEP_INHERITANCE</tt> has a different
    meaning for clients when doing an <tt>enumerateInstances</tt>
    operation versus an <tt>enumerateClassNames</tt> operation.
    In the latter (which is not implemented by providers), the client
    does <i>not</i> receive all subclass definitions when the flag is
    not set. However, when enumerating instances, the client <i>does</i>
    receive instances of subclasses, even when the flag is not set.
    The flag controls only the presence or absence of subclass properties
    in instance enumeration, not the presence or absence of the
    instances themselves.</p>    
    */
    static const Uint32 DEEP_INHERITANCE;

    /**
    Reserved for future use.
    */
    static const Uint32 REMOTE_ONLY;

    /**
    Convert an operation flag symbol to the integer value of
    its bit position.
    
    <p>The <tt>convert</tt> function converts its Boolean arguments
    to an unsigned 32-bit integer
    containing the flags as bits. The values for the flag bits
    are:</p>
    <tt><ul>
    <li>LOCAL_ONLY = 0x00000001</li>
    <li>INCLUDE_QUALIFIERS = 0x00000002</li>
    <li>INCLUDE_CLASS_ORIGIN = 0x00000004</li>
    <li>DEEP_INHERITANCE = 0x00000008</li>
    </ul></tt>
    <p>Other values are reserved for future use.</p>
    */
    static Uint32 convert(
	const Boolean localOnly,
	const Boolean includeQualifiers = false,
	const Boolean includeClassOrigin = false,
	const Boolean deepInheritance = false,
	const Boolean remoteOnly = false);

};

//const Uint32 OperationFlag::NONE = 0x00000000;
//const Uint32 OperationFlag::LOCAL_ONLY = 0x00000001;
//const Uint32 OperationFlag::INCLUDE_QUALIFIERS = 0x00000002;
//const Uint32 OperationFlag::INCLUDE_CLASS_ORIGIN = 0x00000004;
//const Uint32 OperationFlag::DEEP_INHERITANCE = 0x00000008;
//const Uint32 OperationFlag::REMOTE_ONLY = 0x00000020;

PEGASUS_NAMESPACE_END

#endif
