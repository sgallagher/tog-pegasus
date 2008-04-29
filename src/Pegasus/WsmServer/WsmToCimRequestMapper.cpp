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

#include <cctype>
#include <cstdio>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/MessageLoader.h>
#include <Pegasus/Common/CIMNameUnchecked.h>
#include <Pegasus/Common/StringConversion.h>
#include <Pegasus/Common/XmlReader.h>
#include <Pegasus/Common/XmlWriter.h>
#include <Pegasus/Common/HostLocator.h>
#include <Pegasus/WsmServer/WsmConstants.h>
#include <Pegasus/WsmServer/WsmFault.h>
#include "WsmToCimRequestMapper.h"

PEGASUS_NAMESPACE_BEGIN

WsmToCimRequestMapper::WsmToCimRequestMapper(CIMRepository* repository)
    : _repository(repository)
{
}

WsmToCimRequestMapper::~WsmToCimRequestMapper()
{
}

CIMOperationRequestMessage* WsmToCimRequestMapper::mapToCimRequest(
    WsmRequest* request)
{
    AutoPtr<CIMOperationRequestMessage> cimRequest;

    switch (request->getType())
    {
        case WS_TRANSFER_GET:
            cimRequest.reset(mapToCimGetInstanceRequest(
                (WsmGetRequest*) request));
            break;

        case WS_TRANSFER_PUT:
            cimRequest.reset(mapToCimModifyInstanceRequest(
                (WsmPutRequest*) request));
            break;

        case WS_TRANSFER_CREATE:
            cimRequest.reset(mapToCimCreateInstanceRequest(
                (WsmCreateRequest*) request));
            break;

        case WS_TRANSFER_DELETE:
            cimRequest.reset(mapToCimDeleteInstanceRequest(
                (WsmDeleteRequest*) request));
            break;

        default:
            PEGASUS_ASSERT(0);
    }

    if (cimRequest.get())
    {
        cimRequest->operationContext.insert(
            IdentityContainer(request->userName));
        cimRequest->operationContext.set(
            AcceptLanguageListContainer(request->acceptLanguages));
        cimRequest->operationContext.set(
            ContentLanguageListContainer(request->contentLanguages));
        cimRequest->setHttpMethod(request->httpMethod);
        cimRequest->setCloseConnect(request->httpCloseConnect);
    }

    return cimRequest.release();
}

CIMGetInstanceRequestMessage*
    WsmToCimRequestMapper::mapToCimGetInstanceRequest(
        WsmGetRequest* request)
{
    CIMNamespaceName nameSpace;
    CIMObjectPath instanceName;

    _disallowAllClassesResourceUri(request->epr.resourceUri);

    // EPR to object path does a generic conversion, including conversion
    // of EPR address to host and namespace selector to CIM namespace.
    // For GetInstance operation instance name should only contain a
    // class name and key bindings.
    convertEPRToObjectPath(request->epr, instanceName);
    nameSpace = instanceName.getNameSpace();
    instanceName.setNameSpace(CIMNamespaceName());
    instanceName.setHost(String::EMPTY);

    CIMGetInstanceRequestMessage* cimRequest =
        new CIMGetInstanceRequestMessage(
            XmlWriter::getNextMessageId(),
            nameSpace,
            instanceName,
            false,
            false,
            false,
            CIMPropertyList(),
            QueueIdStack(request->queueId),
            request->authType,
            request->userName);
    cimRequest->ipAddress = request->ipAddress;

    return cimRequest;
}

CIMModifyInstanceRequestMessage*
    WsmToCimRequestMapper::mapToCimModifyInstanceRequest(
        WsmPutRequest* request)
{
    CIMNamespaceName nameSpace;
    CIMObjectPath instanceName;

    _disallowAllClassesResourceUri(request->epr.resourceUri);

    // EPR to object path does a generic conversion, including conversion
    // of EPR address to host and namespace selector to CIM namespace.
    // For ModifyInstance operation instance name should only contain a
    // class name and key bindings.
    convertEPRToObjectPath(request->epr, instanceName);
    nameSpace = instanceName.getNameSpace();
    instanceName.setNameSpace(CIMNamespaceName());
    instanceName.setHost(String::EMPTY);

    CIMInstance instance;
    convertWsmToCimInstance(request->instance, nameSpace, instance);
    instance.setPath(instanceName);

    CIMModifyInstanceRequestMessage* cimRequest =
        new CIMModifyInstanceRequestMessage(
            XmlWriter::getNextMessageId(),
            nameSpace,
            instance,
            false, // includeQualifiers
            CIMPropertyList(),
            QueueIdStack(request->queueId),
            request->authType,
            request->userName);

    cimRequest->ipAddress = request->ipAddress;

    return cimRequest;
}

CIMCreateInstanceRequestMessage*
    WsmToCimRequestMapper::mapToCimCreateInstanceRequest(
        WsmCreateRequest* request)
{
    CIMNamespaceName nameSpace;
    CIMObjectPath instanceName;

    // EPR to object path does a generic conversion, including conversion
    // of EPR address to host and namespace selector to CIM namespace.
    // For CreateInstance operation instance name should only contain a
    // class name and key bindings.
    convertEPRToObjectPath(request->epr, instanceName);
    nameSpace = instanceName.getNameSpace();
    instanceName.setNameSpace(CIMNamespaceName());
    instanceName.setHost(String::EMPTY);

    CIMInstance instance;
    convertWsmToCimInstance(request->instance, nameSpace, instance);

    CIMCreateInstanceRequestMessage* cimRequest =
        new CIMCreateInstanceRequestMessage(
            XmlWriter::getNextMessageId(),
            nameSpace,
            instance,
            QueueIdStack(request->queueId),
            request->authType,
            request->userName);
    cimRequest->ipAddress = request->ipAddress;

    return cimRequest;
}

CIMDeleteInstanceRequestMessage*
    WsmToCimRequestMapper::mapToCimDeleteInstanceRequest(
        WsmDeleteRequest* request)
{
    CIMNamespaceName nameSpace;
    CIMObjectPath instanceName;

    _disallowAllClassesResourceUri(request->epr.resourceUri);

    // EPR to object path does a generic conversion, including conversion
    // of EPR address to host and namespace selector to CIM namespace.
    // For DeleteInstance operation instance name should only contain a
    // class name and key bindings.
    convertEPRToObjectPath(request->epr, instanceName);
    nameSpace = instanceName.getNameSpace();
    instanceName.setNameSpace(CIMNamespaceName());
    instanceName.setHost(String::EMPTY);

    CIMDeleteInstanceRequestMessage* cimRequest =
        new CIMDeleteInstanceRequestMessage(
            XmlWriter::getNextMessageId(),
            nameSpace,
            instanceName,
            QueueIdStack(request->queueId),
            request->authType,
            request->userName);
    cimRequest->ipAddress = request->ipAddress;

    return cimRequest;
}

void WsmToCimRequestMapper::_disallowAllClassesResourceUri(
    const String& resourceUri)
{
    // DSP0227 R6-1:  A service should return a wsa:ActionNotSupported fault
    // if the "all classes" ResourceURI is used with any of the WS-Transfer
    // operations, even if this ResourceURI is supported for enumerations or
    // eventing.

    if (resourceUri == WSM_RESOURCEURI_ALLCLASSES)
    {
        throw WsmFault(
            WsmFault::wsa_ActionNotSupported,
            MessageLoaderParms(
                "WsmServer.WsmToCimRequestMapper.ALLCLASSES_URI_NOT_ALLOWED",
                "The \"all classes\" ResourceURI cannot be used with this "
                    "operation."),
            WSMAN_FAULTDETAIL_ACTIONMISMATCH);
    }
}

CIMName WsmToCimRequestMapper::convertResourceUriToClassName(
    const String& resourceUri)
{
    static const String RESOURCEURI_PREFIX =
        String(WSM_RESOURCEURI_CIMSCHEMAV2) + "/";

    if (String::compare(
            resourceUri,
            RESOURCEURI_PREFIX,
            RESOURCEURI_PREFIX.size()) == 0)
    {
        String className = resourceUri.subString(RESOURCEURI_PREFIX.size());

        if (CIMName::legal(className))
        {
            return CIMNameUnchecked(className);
        }
    }

    throw WsmFault(
        WsmFault::wsa_DestinationUnreachable,
        MessageLoaderParms(
            "WsmServer.WsmToCimRequestMapper.UNRECOGNIZED_RESOURCEURI",
            "Unrecognized ResourceURI value: $0.",
            resourceUri),
        WSMAN_FAULTDETAIL_INVALIDRESOURCEURI);
}

String WsmToCimRequestMapper::convertEPRAddressToHostname(const String& addr)
{
    // EPR address is formed by adding '/wsman' to the host name.
    // E.g. http://localhost:5988/wsman
    // Extract the host name from the EPR address
    String hostName;
    if (addr != WSM_ADDRESS_ANONYMOUS)
    {
        Uint32 pos1 = 0;
        if (String::compare(addr, "http://", 7) == 0)
            pos1 = 7;
        else if (String::compare(addr, "https://", 8) == 0)
            pos1 = 8;

        Uint32 pos2 = addr.reverseFind('/');
        if (pos1 != 0 && pos2 != PEG_NOT_FOUND && pos2 > pos1)
        {
            // The string between "http[s]://" and "/wsman" must be 
            // the host name
            HostLocator hostLoc(addr.subString(pos1, pos2 - pos1));
            if (hostLoc.isValid())
            {
                hostName = hostLoc.getHost();
            }
        }

        if (hostName.size() == 0)
        {
            // Invalid host name
            throw WsmFault(
                WsmFault::wsa_InvalidMessageInformationHeader,
                MessageLoaderParms(
                    "WsmServer.WsmToCimRequestMapper.INVALID_EPR_ADDRESS",
                    "The EPR address \"$0\" is not valid", addr),
                    WSMAN_FAULTDETAIL_INVALIDVALUE);
        }
    }
    return hostName;
}

void WsmToCimRequestMapper::convertEPRToObjectPath(
    const WsmEndpointReference& epr,
    CIMObjectPath& objectPath)
{
    Array<CIMKeyBinding> keyBindings;
    CIMNamespaceName namespaceName;

    // Convert the ResourceURI to a CIM class name
    CIMName className = convertResourceUriToClassName(epr.resourceUri);

    PEGASUS_ASSERT(epr.selectorSet);

    // Determine the namespace from the selector set
    for (Uint32 i = 0, n = epr.selectorSet->selectors.size(); i < n; i++)
    {
        if (String::equalNoCase(
                epr.selectorSet->selectors[i].name, "__cimnamespace"))
        {
            if (epr.selectorSet->selectors[i].type != WsmSelector::VALUE)
            {
                throw WsmFault(
                    WsmFault::wsman_InvalidSelectors,
                    MessageLoaderParms(
                        "WsmServer.WsmToCimRequestMapper."
                            "SELECTOR_TYPE_MISMATCH",
                        "Selector \"$0\" is not of the correct type.",
                        epr.selectorSet->selectors[i].name),
                    WSMAN_FAULTDETAIL_TYPEMISMATCH);
            }

            if (!CIMNamespaceName::legal(epr.selectorSet->selectors[i].value))
            {
                throw WsmFault(
                    WsmFault::wsman_InvalidSelectors,
                    MessageLoaderParms(
                        "WsmServer.WsmToCimRequestMapper."
                            "INVALID_SELECTOR_VALUE",
                        "The value \"$0\" is not valid for selector \"$1\".",
                        epr.selectorSet->selectors[i].value,
                        epr.selectorSet->selectors[i].name),
                    WSMAN_FAULTDETAIL_INVALIDVALUE);
            }

            namespaceName = epr.selectorSet->selectors[i].value;
            break;
        }
    }

    if (namespaceName.isNull())
    {
        namespaceName = PEGASUS_DEFAULT_WSM_NAMESPACE;
    }

    CIMClass cimClass = _repository->getClass(
        namespaceName,
        className,
        false /*localOnly*/);

    for (Uint32 i = 0, n = epr.selectorSet->selectors.size(); i < n; i++)
    {
        if (!String::equalNoCase(
                epr.selectorSet->selectors[i].name, "__cimnamespace"))
        {
            Uint32 propertyPos;

            if (!CIMName::legal(epr.selectorSet->selectors[i].name) ||
                ((propertyPos = cimClass.findProperty(CIMNameUnchecked(
                    epr.selectorSet->selectors[i].name))) == PEG_NOT_FOUND))
            {
                throw WsmFault(
                    WsmFault::wsman_InvalidSelectors,
                    MessageLoaderParms(
                        "WsmServer.WsmToCimRequestMapper.UNEXPECTED_SELECTOR",
                        "Selector \"$0\" is not expected for a resource of "
                            "class $1 in namespace $2.",
                        epr.selectorSet->selectors[i].name,
                        className.getString(),
                        namespaceName.getString()),
                    WSMAN_FAULTDETAIL_UNEXPECTEDSELECTORS);
            }

            CIMProperty property = cimClass.getProperty(propertyPos);
            CIMKeyBinding newKeyBinding(
                CIMNameUnchecked(epr.selectorSet->selectors[i].name),
                property.getValue());

            if (((newKeyBinding.getType() == CIMKeyBinding::REFERENCE) &&
                 (epr.selectorSet->selectors[i].type != WsmSelector::EPR)) ||
                ((newKeyBinding.getType() != CIMKeyBinding::REFERENCE) &&
                 (epr.selectorSet->selectors[i].type == WsmSelector::EPR)))
            {
                throw WsmFault(
                    WsmFault::wsman_InvalidSelectors,
                    MessageLoaderParms(
                        "WsmServer.WsmToCimRequestMapper."
                            "SELECTOR_TYPE_MISMATCH",
                        "Selector \"$0\" is not of the correct type.",
                        epr.selectorSet->selectors[i].name),
                    WSMAN_FAULTDETAIL_TYPEMISMATCH);
            }

            if (epr.selectorSet->selectors[i].type == WsmSelector::VALUE)
            {
                newKeyBinding.setValue(epr.selectorSet->selectors[i].value);
            }
            else
            {
                CIMObjectPath reference;
                convertEPRToObjectPath(
                    epr.selectorSet->selectors[i].epr,
                    reference);
                newKeyBinding.setValue(reference.toString());
            }

            keyBindings.append(newKeyBinding);
        }
    }

    objectPath = CIMObjectPath(
        convertEPRAddressToHostname(epr.address),
        namespaceName,
        className,
        keyBindings);
}

void WsmToCimRequestMapper::convertWsmToCimInstance(
    WsmInstance& wsmInstance,
    const CIMNamespaceName& nameSpace,
    CIMInstance& cimInstance)
{
    CIMName className(wsmInstance.getClassName());
    CIMClass cimClass =
        _repository->getClass(nameSpace, className, false /* localOnly */);

    cimInstance = CIMInstance(className);
    for (Uint32 i = 0, n = wsmInstance.getPropertyCount(); i < n; i++)
    {
        WsmProperty& wsmProperty = wsmInstance.getProperty(i);
        const String& wsmPropName = wsmProperty.getName();
        WsmValue& wsmPropValue = wsmProperty.getValue();

        if (CIMName::legal(wsmPropName))
        {
            Uint32 cimPropIdx = cimClass.findProperty(CIMName(wsmPropName));
            if (cimPropIdx != PEG_NOT_FOUND)
            {
                CIMProperty cimProperty = cimClass.getProperty(cimPropIdx);
                CIMValue cimPropValue(cimProperty.getValue());

                convertWsmToCimValue(wsmPropValue, nameSpace, cimPropValue);
                cimProperty.setValue(cimPropValue);
                cimInstance.addProperty(cimProperty);
                continue;
            }
        }

        throw WsmFault(
            WsmFault::wsman_SchemaValidationError,
            MessageLoaderParms(
                "WsmServer.WsmToCimRequestMapper.NO_SUCH_PROPERTY",
                "The $0 property does not exist.",
                wsmPropName));
    }
}

void WsmToCimRequestMapper::convertWsmToCimValue(
    WsmValue& wsmValue,
    const CIMNamespaceName& nameSpace,
    CIMValue& cimValue)
{
    if (wsmValue.isNull())
    {
        cimValue.setNullValue(
            cimValue.getType(), cimValue.isArray(), cimValue.getArraySize());
        return;
    }

    if (cimValue.isArray())
    {
        wsmValue.toArray();
        switch (wsmValue.getType())
        {
            case WSMTYPE_REFERENCE:
            {
                Array<WsmEndpointReference> eprs;
                Array<CIMObjectPath> objPaths;
                wsmValue.get(eprs);
                for (Uint32 i = 0, n = eprs.size(); i < n; i++)
                {
                    CIMObjectPath objPath;
                    convertEPRToObjectPath(eprs[i], objPath);
                    objPaths.append(objPath);
                }
                cimValue.set(objPaths);
                break;
            }

            case WSMTYPE_INSTANCE:
            {
                Array<WsmInstance> wsmInst;
                Array<CIMInstance> cimInst;
                wsmValue.get(wsmInst);
                for (Uint32 i = 0, n = wsmInst.size(); i < n; i++)
                {
                    CIMInstance inst;
                    convertWsmToCimInstance(wsmInst[i], nameSpace, inst);
                    cimInst.append(inst);
                }
                cimValue.set(cimInst);
                break;
            }

            case WSMTYPE_OTHER:
            {
                Array<String> strs;
                wsmValue.get(strs);
                convertStringArrayToCimValue(
                    strs, cimValue.getType(), cimValue);
                break;
            }

            default:
            {
                PEGASUS_ASSERT(0);
            }
        }
    }
    else
    {
        if (wsmValue.isArray())
        {
            throw TypeMismatchException();
        }

        switch (wsmValue.getType())
        {
            case WSMTYPE_REFERENCE:
            {
                WsmEndpointReference epr;
                CIMObjectPath objPath;
                wsmValue.get(epr);
                convertEPRToObjectPath(epr, objPath);
                cimValue.set(objPath);
                break;
            }

            case WSMTYPE_INSTANCE:
            {
                WsmInstance wsmInst;
                CIMInstance cimInst;
                wsmValue.get(wsmInst);
                convertWsmToCimInstance(wsmInst, nameSpace, cimInst);
                cimValue.set(cimInst);
                break;
            }

            case WSMTYPE_OTHER:
            {
                String str;
                wsmValue.get(str);
                convertStringToCimValue(str, cimValue.getType(), cimValue);
                break;
            }

            default:
            {
                PEGASUS_ASSERT(0);
            }
        }
    }
}

void WsmToCimRequestMapper::convertStringToCimValue(
    const String& str,
    CIMType cimType,
    CIMValue& cimValue)
{
    switch (cimType)
    {
        case CIMTYPE_BOOLEAN:
        {
            Boolean val;
            if (String::compare(str, "true") == 0 || 
                String::compare(str, "1") == 0)
            {
                val = 1;
            }
            else if (String::compare(str, "false") == 0 ||
                     String::compare(str, "0") == 0)
            {
                val = 0;
            }
            else
            {
                throw WsmFault(
                    WsmFault::wxf_InvalidRepresentation,
                    MessageLoaderParms(
                        "WsmServer.WsmToCimRequestMapper.INVALID_BOOLEAN_VALUE",
                        "The boolean value \"$0\" is not valid", str),
                    WSMAN_FAULTDETAIL_INVALIDVALUE);
            }
            cimValue.set(val);
            break;
        }

        case CIMTYPE_UINT8:
        case CIMTYPE_UINT16:
        case CIMTYPE_UINT32:
        case CIMTYPE_UINT64:
        {
            Uint64 val;
            if (!XmlReader::stringToUnsignedInteger(
                (const char*) str.getCString(), val))
            {
                throw WsmFault(
                    WsmFault::wxf_InvalidRepresentation,
                    MessageLoaderParms(
                        "WsmServer.WsmToCimRequestMapper.INVALID_UI_VALUE",
                        "The unsigned integer value \"$0\" is not valid", str),
                    WSMAN_FAULTDETAIL_INVALIDVALUE);
            }

            switch (cimType)
            {
                case CIMTYPE_UINT8:
                {
                    if (!StringConversion::checkUintBounds(val, cimType))
                    {
                        throw WsmFault(
                            WsmFault::wxf_InvalidRepresentation,
                            MessageLoaderParms(
                                "WsmServer.WsmToCimRequestMapper."
                                "VALUE_OUT_OF_RANGE",
                                "The $0 value \"$1\" is out of range",
                                "Uint8", str),
                            WSMAN_FAULTDETAIL_INVALIDVALUE);
                    }
                    cimValue.set(Uint8(val));
                    break;
                }
                case CIMTYPE_UINT16:
                {
                    if (!StringConversion::checkUintBounds(val, cimType))
                    {
                        throw WsmFault(
                            WsmFault::wxf_InvalidRepresentation,
                            MessageLoaderParms(
                                "WsmServer.WsmToCimRequestMapper."
                                "VALUE_OUT_OF_RANGE",
                                "The $0 value \"$1\" is out of range",
                                "Uint16", str),
                            WSMAN_FAULTDETAIL_INVALIDVALUE);
                    }
                    cimValue.set(Uint16(val));
                    break;
                }
                case CIMTYPE_UINT32:
                {
                    if (!StringConversion::checkUintBounds(val, cimType))
                    {
                        throw WsmFault(
                            WsmFault::wxf_InvalidRepresentation,
                            MessageLoaderParms(
                                "WsmServer.WsmToCimRequestMapper."
                                "VALUE_OUT_OF_RANGE",
                                "The $0 value \"$1\" is out of range",
                                "Uint32", str),
                            WSMAN_FAULTDETAIL_INVALIDVALUE);
                    }
                    cimValue.set(Uint32(val));
                    break;
                }
                case CIMTYPE_UINT64:
                {
                    cimValue.set(Uint64(val));
                    break;
                }
                default:
                {
                    PEGASUS_ASSERT(0);
                }
            }
            break;
        }

        case CIMTYPE_SINT8:
        case CIMTYPE_SINT16:
        case CIMTYPE_SINT32:
        case CIMTYPE_SINT64:
        {
            Sint64 val;
            if (!XmlReader::stringToSignedInteger(
                (const char*) str.getCString(), val))
            {
                throw WsmFault(
                    WsmFault::wxf_InvalidRepresentation,
                    MessageLoaderParms(
                        "WsmServer.WsmToCimRequestMapper.INVALID_SI_VALUE",
                        "The signed integer value \"$0\" is not valid", str),
                    WSMAN_FAULTDETAIL_INVALIDVALUE);
            }

            switch (cimType)
            {
                case CIMTYPE_SINT8:
                {
                    if (!StringConversion::checkSintBounds(val, cimType))
                    {
                        throw WsmFault(
                            WsmFault::wxf_InvalidRepresentation,
                            MessageLoaderParms(
                                "WsmServer.WsmToCimRequestMapper."
                                "VALUE_OUT_OF_RANGE",
                                "The $0 value \"$1\" is out of range",
                                "Sint8", str),
                            WSMAN_FAULTDETAIL_INVALIDVALUE);
                    }
                    cimValue.set(Sint8(val));
                    break;
                }
                case CIMTYPE_SINT16:
                {
                    if (!StringConversion::checkSintBounds(val, cimType))
                    {
                        throw WsmFault(
                            WsmFault::wxf_InvalidRepresentation,
                            MessageLoaderParms(
                                "WsmServer.WsmToCimRequestMapper."
                                "VALUE_OUT_OF_RANGE",
                                "The $0 value \"$1\" is out of range",
                                "Sint16", str),
                            WSMAN_FAULTDETAIL_INVALIDVALUE);
                    }
                    cimValue.set(Sint16(val));
                    break;
                }
                case CIMTYPE_SINT32:
                {
                    if (!StringConversion::checkSintBounds(val, cimType))
                    {
                        throw WsmFault(
                            WsmFault::wxf_InvalidRepresentation,
                            MessageLoaderParms(
                                "WsmServer.WsmToCimRequestMapper."
                                "VALUE_OUT_OF_RANGE",
                                "The $0 value \"$1\" is out of range",
                                "Sint32", str),
                            WSMAN_FAULTDETAIL_INVALIDVALUE);
                    }
                    cimValue.set(Sint32(val));
                    break;
                }
                case CIMTYPE_SINT64:
                {
                    cimValue.set(Sint64(val));
                    break;
                }
                default:
                {
                    PEGASUS_ASSERT(0);
                }
            }
            break;
        }

        case CIMTYPE_REAL32:
        {
            Real64 val;
            if (!StringConversion::stringToReal64(
                (const char*) str.getCString(), val))
            {
                throw WsmFault(
                    WsmFault::wxf_InvalidRepresentation,
                    MessageLoaderParms(
                        "WsmServer.WsmToCimRequestMapper.INVALID_RN_VALUE",
                        "The real number value \"$0\" is not valid", str),
                    WSMAN_FAULTDETAIL_INVALIDVALUE);
            }
            cimValue.set(Real32(val));
            break;
        }

        case CIMTYPE_REAL64:
        {
            Real64 val;
            if (!StringConversion::stringToReal64(
                (const char*) str.getCString(), val))
            {
                throw WsmFault(
                    WsmFault::wxf_InvalidRepresentation,
                    MessageLoaderParms(
                        "WsmServer.WsmToCimRequestMapper.INVALID_RN_VALUE",
                        "The real number value \"$0\" is not valid", str),
                    WSMAN_FAULTDETAIL_INVALIDVALUE);
            }
            cimValue.set(val);
            break;
        }

        case CIMTYPE_CHAR16:
        {
            if (str.size() != 1)
            {
                throw WsmFault(
                    WsmFault::wxf_InvalidRepresentation,
                    MessageLoaderParms(
                        "WsmServer.WsmToCimRequestMapper.INVALID_CHAR16_VALUE",
                        "The char16 value \"$0\" is not valid", str),
                    WSMAN_FAULTDETAIL_INVALIDVALUE);
            }
            cimValue.set(str[0]);
            break;
        }

        case CIMTYPE_STRING:
        {
            cimValue.set(str);
            break;
        }

        case CIMTYPE_DATETIME:
        {
            CIMDateTime tmp;
            try
            {
                tmp.set(str);
            }
            catch (InvalidDateTimeFormatException&)
            {
                throw WsmFault(
                    WsmFault::wxf_InvalidRepresentation,
                    MessageLoaderParms(
                        "WsmServer.WsmToCimRequestMapper.INVALID_DT_VALUE",
                        "The datetime value \"$0\" is not valid", str),
                    WSMAN_FAULTDETAIL_INVALIDVALUE);
            }
            cimValue.set(tmp);
            break;
        }

        default:
        {
            PEGASUS_ASSERT(0);
        }
    }
}

template<class T>
void _convertStringArrayToCimValueAux(
    const Array<String>& strs,
    CIMType cimType,
    CIMValue& cimValue)
{
    Array<T> arr;
    for (Uint32 i = 0, n = strs.size(); i < n; i++)
    {
        T val;
        CIMValue tmp;
        WsmToCimRequestMapper::convertStringToCimValue(strs[i], cimType, tmp);
        tmp.get(val);
        arr.append(val);
    }
    cimValue.set(arr);
}

void WsmToCimRequestMapper::convertStringArrayToCimValue(
    const Array<String>& strs,
    CIMType cimType,
    CIMValue& cimValue)
{
        switch (cimValue.getType())
        {
            case CIMTYPE_BOOLEAN:
                _convertStringArrayToCimValueAux<Boolean>(
                    strs, cimType, cimValue);
                break;
            case CIMTYPE_UINT8:
                _convertStringArrayToCimValueAux<Uint8>(
                    strs, cimType, cimValue);
                break;
            case CIMTYPE_SINT8:
                _convertStringArrayToCimValueAux<Sint8>(
                    strs, cimType, cimValue);
                break;
            case CIMTYPE_UINT16:
                _convertStringArrayToCimValueAux<Uint16>(
                    strs, cimType, cimValue);
                break;
            case CIMTYPE_SINT16:
                _convertStringArrayToCimValueAux<Sint16>(
                    strs, cimType, cimValue);
                break;
            case CIMTYPE_UINT32:
                _convertStringArrayToCimValueAux<Uint32>(
                    strs, cimType, cimValue);
                break;
            case CIMTYPE_SINT32:
                _convertStringArrayToCimValueAux<Sint32>(
                    strs, cimType, cimValue);
                break;
            case CIMTYPE_UINT64:
                _convertStringArrayToCimValueAux<Uint64>(
                    strs, cimType, cimValue);
                break;
            case CIMTYPE_SINT64:
                _convertStringArrayToCimValueAux<Sint64>(
                    strs, cimType, cimValue);
                break;
            case CIMTYPE_REAL32:
                _convertStringArrayToCimValueAux<Real32>(
                    strs, cimType, cimValue);
                break;
            case CIMTYPE_REAL64:
                _convertStringArrayToCimValueAux<Real64>(
                    strs, cimType, cimValue);
                break;
            case CIMTYPE_CHAR16:
                _convertStringArrayToCimValueAux<Char16>(
                    strs, cimType, cimValue);
                break;
            case CIMTYPE_STRING:
                _convertStringArrayToCimValueAux<String>(
                    strs, cimType, cimValue);
                break;
            case CIMTYPE_DATETIME:
                _convertStringArrayToCimValueAux<CIMDateTime>(
                    strs, cimType, cimValue);
                break;
            default:
                PEGASUS_ASSERT(0);
        }
}

PEGASUS_NAMESPACE_END
