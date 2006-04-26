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
// Author: Tim Potter <tpot@hp.com>
//
//%/////////////////////////////////////////////////////////////////////////

#ifndef _RPMPROVIDER_H
#define _RPMPROVIDER_H

#include <Pegasus/Provider/CIMInstanceProvider.h>
#include <Pegasus/Provider/CIMAssociationProvider.h>
#include "RPMProviderUtils.h"

class RPMProvider: 
  public CIMInstanceProvider,
  public CIMAssociationProvider {

 public:

    RPMProvider();
    ~RPMProvider();

    // CIMProvider methods

    void initialize(CIMOMHandle &handle);
    void terminate();

    // CIMInstanceProvider methods

    void getInstance(const OperationContext &context,
		     const CIMObjectPath &ref,
		     const Boolean includeQualifiers,
		     const Boolean includeClassOrigin,
		     const CIMPropertyList &propertyList,
		     InstanceResponseHandler &handler);

    void enumerateInstances(const OperationContext &context,
			    const CIMObjectPath &ref,
			    const Boolean includeQualifiers,
			    const Boolean includeClassOrigin,
			    const CIMPropertyList &propertyList,
			    InstanceResponseHandler &handler);

    void enumerateInstanceNames(const OperationContext &context,
				const CIMObjectPath &ref,
				ObjectPathResponseHandler &handler);

    void modifyInstance(const OperationContext &context,
			const CIMObjectPath &ref,
			const CIMInstance &instanceObject,
			const Boolean includeQualifiers,
			const CIMPropertyList &propertyList,
			ResponseHandler &handler);

    void createInstance(const OperationContext &context,
			const CIMObjectPath &ref,
			const CIMInstance &instanceObject,
			ObjectPathResponseHandler &handler);

    void deleteInstance(const OperationContext &context,
			const CIMObjectPath &ref,
			ResponseHandler &handler);

    // CIMAssociationProvider methods

    void associators(const OperationContext &context,
		     const CIMObjectPath &objectName,
		     const CIMName &associationClass,
		     const CIMName &resultClass,
		     const String &role,
		     const String &resultRole,
		     const Boolean includeQualifiers,
		     const Boolean includeClassOrigin,
		     const CIMPropertyList &propertyList,
		     ObjectResponseHandler &handler);

    void associatorNames(const OperationContext &context,
			 const CIMObjectPath &objectName,
			 const CIMName &associationClass,
			 const CIMName &resultClass,
			 const String &role,
			 const String &resultRole,
			 ObjectPathResponseHandler &handler);

    void references(const OperationContext &context,
		    const CIMObjectPath &objectName,
		    const CIMName &resultClass,
		    const String &role,
		    const Boolean includeQualifiers,
		    const Boolean includeClassOrigin,
		    const CIMPropertyList &propertyList,
		    ObjectResponseHandler &handler);

    void referenceNames(const OperationContext &context,
			const CIMObjectPath &objectName,
			const CIMName &resultClass,
			const String &role,
			ObjectPathResponseHandler &handler);

 private:

    // Helpers for enumerateInstanceNames() and enumerateInstances()

    Array<CIMObjectPath> enumeratePackageInstanceNames();
    Array<CIMInstance> enumeratePackageInstances();

    Array<CIMObjectPath> enumerateFileInstanceNames();
    Array<CIMInstance> enumerateFileInstances();

    Array<CIMObjectPath> enumerateProductInstanceNames();
    Array<CIMInstance> enumerateProductInstances();

    // Helpers for getInstance()

    void getPackageInstance(const OperationContext &context,
			    const CIMObjectPath &ref,
			    const Boolean includeQualifiers,
			    const Boolean includeClassOrigin,
			    const CIMPropertyList &propertyList,
			    InstanceResponseHandler &handler);

    void getFileInstance(const OperationContext &context,
			 const CIMObjectPath &ref,
			 const Boolean includeQualifiers,
			 const Boolean includeClassOrigin,
			 const CIMPropertyList &propertyList,
			 InstanceResponseHandler &handler);

    void getProductInstance(const OperationContext &context,
			    const CIMObjectPath &ref,
			    const Boolean includeQualifiers,
			    const Boolean includeClassOrigin,
			    const CIMPropertyList &propertyList,
			    InstanceResponseHandler &handler);

    // Helpers for associators() and associatorNames()

    void packageAssociators(const OperationContext &context,
			    const CIMObjectPath &objectName,
			    const CIMName &associationClass,
			    const CIMName &resultClass,
			    const String &role,
			    const String &resultRole,
			    const Boolean includeQualifiers,
			    const Boolean includeClassOrigin,
			    const CIMPropertyList &propertyList,
			    ObjectResponseHandler &handler);

    void fileAssociators(const OperationContext &context,
			 const CIMObjectPath &objectName,
			 const CIMName &associationClass,
			 const CIMName &resultClass,
			 const String &role,
			 const String &resultRole,
			 const Boolean includeQualifiers,
			 const Boolean includeClassOrigin,
			 const CIMPropertyList &propertyList,
			 ObjectResponseHandler &handler);

    void packageAssociatorNames(const OperationContext &context,
				const CIMObjectPath &objectName,
				const CIMName &associationClass,
				const CIMName &resultClass,
				const String &role,
				const String &resultRole,
				ObjectPathResponseHandler &handler);
   
    void fileAssociatorNames(const OperationContext &context,
			     const CIMObjectPath &objectName,
			     const CIMName &associationClass,
			     const CIMName &resultClass,
			     const String &role,
			     const String &resultRole,
			     ObjectPathResponseHandler &handler);

    // Helpers for references() and referenceNames()

    void packageReferences(const OperationContext &context,
			   const CIMObjectPath &objectName,
			   const CIMName &resultClass,
			   const String &role,
			   const Boolean includeQualifiers,
			   const Boolean includeClassOrigin,
			   const CIMPropertyList &propertyList,
			   ObjectResponseHandler &handler);

    void fileReferences(const OperationContext &context,
			const CIMObjectPath &objectName,
			const CIMName &resultClass,
			const String &role,
			const Boolean includeQualifiers,
			const Boolean includeClassOrigin,
			const CIMPropertyList &propertyList,
			ObjectResponseHandler &handler);

    void packageReferenceNames(const OperationContext &context,
			       const CIMObjectPath &objectName,
			       const CIMName &resultClass,
			       const String &role,
			       ObjectPathResponseHandler &handler);

    void fileReferenceNames(const OperationContext &context,
			    const CIMObjectPath &objectName,
			    const CIMName &resultClass,
			    const String &role,
			    ObjectPathResponseHandler &handler);

    // Handle on RPM db

    rpmdb _db;      
};

#endif // _RPMPROVIDER_H
