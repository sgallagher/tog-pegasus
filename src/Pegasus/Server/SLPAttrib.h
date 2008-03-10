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

#ifndef Pegasus_SLPAttrib_h
#define Pegasus_SLPAttrib_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Server/Linkage.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMProperty.h>

PEGASUS_NAMESPACE_BEGIN
class PEGASUS_SERVER_LINKAGE SLPAttrib
{

public:

       
    /** Constructor.
     *  Fills slpType and slpUrl attribute
     */
    SLPAttrib();

    ~SLPAttrib();

    /** fillData.
     *
     *  Fills all the data required for the pegasus cimserver SLP
     *  advertisement.
     */
      String fillData(const CIMInstance &commInst,
                      const Array<CIMInstance> &objInstances,
                      const Array<CIMInstance> &nameSpaceInstances,
                      const Array<CIMInstance> &commMechInstances,
                      const Array<CIMInstance> &regProfileInstances,
                      const CIMClass & nameSpaceClass,
                      const CIMClass &commMechClass);

    /**
     * Gets all registrations necessary  for cimserver SLP advertisement.
     * @param SLP http attribs.
     * @param SLP https attribs. 
     */
    static void getAllRegs(Array<SLPAttrib> &httpAttribs, 
        Array<SLPAttrib> &httpsAttribs);

    /** formAttributes.
     *
     *  Forms the attributes string for the advertisement.
     */
    void formAttributes(void);

    /** getMappedValue.
     *
     *  Gets the value mapped between "valuemap" and "values".
     */
    String getMappedValue(const CIMProperty& cimProperty, CIMValue value);

    /*
      * accessor methods
      */
    String getAttributes(void) const;

    String getServiceType(void) const;

    String getServiceUrl(void) const;

private:

    // SLP data

    String attributes;
    String serviceType;
    String serviceUrl;
    String serviceHiName;
    String serviceHiDescription;
    String serviceId;
    String nameSpaces;
    String classes;
    String communicationMechanism;
    String otherCommunicationMechanismDescription;
    String interopSchemaNamespace;
    String protocolVersion;
    String functionalProfilesSupported;
    String functionalProfileDescriptions;
    String multipleOperationsSupported;
    String authenticationMechanismsSupported;
    String authenticationMechanismDescriptions;
    String registeredProfiles;

};

#define PEGASUS_ARRAY_T SLPAttrib
#include <Pegasus/Common/ArrayInter.h>
#undef PEGASUS_ARRAY_T

PEGASUS_NAMESPACE_END
#endif
