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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: Nitin Upasani, Hewlett-Packard Company (Nitin_Upasani@hp.com)
// 	        Yi Zhou, Hewlett-Packard Company (yi_zhou@hp.com)
// 	        Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_CIMExportRequestDispatcher_h
#define Pegasus_CIMExportRequestDispatcher_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/MessageQueueService.h>
#include <Pegasus/Common/CIMMessage.h>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMObject.h>
#include <Pegasus/ExportServer/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

/** This class dispatches CIM export requests. For now it simply
 */
class PEGASUS_EXPORT_SERVER_LINKAGE CIMExportRequestDispatcher
   : public MessageQueueService
{
   public:

      typedef MessageQueueService Base;

      CIMExportRequestDispatcher();

      CIMExportRequestDispatcher(
         Boolean dynamicReg,
         Boolean staticConsumers,
         Boolean persistence);

      virtual ~CIMExportRequestDispatcher();

      static void _forwardRequestCallback(AsyncOpNode *,
                                            MessageQueue *,
                                            void *);

   protected:

      virtual void _handle_async_request(AsyncRequest *req);

      virtual void handleEnqueue(Message *);
      
      virtual void handleEnqueue();
      
      AtomicInt dienow;

   private:

      CIMExportIndicationResponseMessage* _handleExportIndicationRequest(
         CIMExportIndicationRequestMessage* request);

      Boolean _dynamicReg;
      Boolean _staticConsumers;
      Boolean _persistence;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_CIMExportRequestDispatcher_h */
