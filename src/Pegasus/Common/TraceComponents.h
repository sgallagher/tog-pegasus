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
// Author: Sushma Fernandes, Hewlett-Packard Company (sushma_fernandes@hp.com)
//
// Modified By: Yi Zhou (yi_zhou@hp.com)
//              Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
//              Carol Ann Krug Graves, Hewlett-Packard Company
//                (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

// REVIEW: Ask how tracing works (note to myself).

#ifndef Pegasus_TraceComponents_h
#define Pegasus_TraceComponents_h

PEGASUS_NAMESPACE_BEGIN

/** Steps for adding tracing to a new component:

    1. Identify whether the component being added belongs to an existing list
       of trace components. Refer to the trace component list 
       (TRACE_COMPONENT_LIST []) defined in this file. If an appropriate 
       component is found, note the component identifier (starts with a TRC_) 
       from the list of TRACE_COMPONENT_IDs defined in this file and skip 
       step 2. If there is no appropriate component available, move to step 2.

    2. Add the new component to this file. This step includes adding a 
       component name and a component identifier.

         a. Add the <component name> to the TRACE_COMPONENT_LIST []. This name 
            will be used to turn on tracing for the component.
         b. Add the component identifier to the TRACE_COMPONENT_ID list. 
            The Component identifier must be formed by prefixing the component 
            name with "TRC_"(E.g. TRC_<component name>). The developer should 
            use the component identifier, to identify a component in all 
            trace calls.

    (NOTE: It is important that both the TRACE_COMPONENT_ID and 
           TRACE_COMPONENT_LIST [] are updated appropriately to include the 
           new component.)

    3. Add the trace calls to the component code. The TRACE_COMPONENT_ID 
       should be used to identify a component in all trace calls.
*/    

/** String constants for naming the various Trace components. 
    These strings will used to while turning on tracing for the respective
    components. The component list needs to be updated whenever a new trace 
    component needs to be added. An appropriate string constant should be 
    defined to identify the component to be traced.
 
    Example: 
    The following example shows the usage of trace component names. 
    The setTraceComponents method is used to turn on tracing for the 
    components, config and repository. The component names are passed as a 
    comma separated list.
 
       Tracer::setTraceComponents( "Config,Repository");
             
 */
static char const* TRACE_COMPONENT_LIST[] =
{
    "Channel",
    "XmlParser",
    "XmlWriter",
    "XmlReader",
    "XmlIO",
    "Http",
    "CimData",
    "ProvManager",
    "Repository",
    "Dispatcher",
    "OsAbstraction",
    "Config",
    "IndDelivery",
    "IndHandler",
    "Authentication",
    "Authorization",
    "UserManager",
    "SubscriptionService",
    "Registration",
    "Shutdown",
    "Server",
    "IndicationService",
    "IndicationServiceInternal",
    "ConfigurationManager",
    "MessageQueueService",
    "ProviderManager",
    "ObjectResolution",
    "WQL",
    "CQL",
    "Thread",
    "MetaDispatcher",
    "IPC",
    "IndicationHandlerService",
    "CIMExportRequestDispatcher",
    "Memory",
    "SSL",
    "ControlProvider",
    "AsyncOpNode",
    "CIMOMHandle",
    "BinaryMessageHandler",
    "L10N",
    "ExportClient",
    "Listener",
    "DiscardedData",
    "ProviderAgent",
    "IndicationFormatter",
    "StatisticalData"
};
/** Constants identifying the Trace components. These constants are used by 
    the component to identify its Trace messages. The component ID needs to
    be updated whenever a new trace component needs to be added. 
    The constants need to be prepended with "TRC_". The position of the
    constant identifier needs to correspond with the position of the constant
    string definition in the TRACE_COMPONENT_LIST. 
 
    Example: 
    The Configuration framework component will be using the constant,
    TRC_CONFIG while invoking the trace call.
              
        PEG_METHOD_ENTER(TRC_CONFIG, "ConfigManager::updateCurrentValue()");
 */

enum TRACE_COMPONENT_ID
{
    TRC_CHANNEL,
    TRC_XML_PARSER,
    TRC_XML_WRITER,
    TRC_XML_READER,
    TRC_XML_IO,
    TRC_HTTP,
    TRC_CIM_DATA,
    TRC_PROV_MANAGER,
    TRC_REPOSITORY,
    TRC_DISPATCHER,
    TRC_OS_ABSTRACTION,
    TRC_CONFIG,
    TRC_IND_DELIVERY,
    TRC_IND_HANDLER,
    TRC_AUTHENTICATION,
    TRC_AUTHORIZATION,
    TRC_USER_MANAGER,
    TRC_SUBSCRIPTION_SERVICE,
    TRC_REGISTRATION,
    TRC_SHUTDOWN,
    TRC_SERVER,
    TRC_INDICATION_SERVICE,
    TRC_INDICATION_SERVICE_INTERNAL,
    TRC_CONFIGURATION_MANAGER,
    TRC_MESSAGEQUEUESERVICE,
    TRC_PROVIDERMANAGER,
    TRC_OBJECTRESOLUTION,
    TRC_WQL,
    TRC_CQL,
    TRC_THREAD,
    TRC_META_DISPATCH,
    TRC_IPC,
    TRC_IND_HANDLE,
    TRC_EXP_REQUEST_DISP,
    TRC_MEMORY,
    TRC_SSL,
    TRC_CONTROLPROVIDER,
    TRC_ASYNC_OPNODE,
    TRC_CIMOM_HANDLE,
    TRC_BINARY_MSG_HANDLER,
    TRC_L10N,
    TRC_EXPORT_CLIENT,
    TRC_LISTENER,
    TRC_DISCARDED_DATA,
    TRC_PROVIDERAGENT,
    TRC_IND_FORMATTER,
    TRC_STATISTICAL_DATA
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_TraceComponents */
