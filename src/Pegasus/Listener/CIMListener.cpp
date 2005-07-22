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
// Author: Dong Xiang, EMC Corporation (xiang_dong@emc.com)
//
// Modified By:   Dan Gorey (djgorey@us.ibm.com)
//                Amit K Arora, IBM (amita@in.ibm.com) for PEP#183
//                Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//                David Dillard, VERITAS Software Corp.
//                    (david.dillard@veritas.com)
//                Vijay Eli, IBM (vijay.eli@in.ibm.com) for bug#3425
//                Aruran, IBM (ashanmug@in.ibm.com) for Bug# 3604
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CIMListener.h"

#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/SSLContext.h>
#include <Pegasus/Common/Monitor.h>
#include <Pegasus/Common/HTTPAcceptor.h>
#include <Pegasus/Common/PegasusVersion.h>
#include <Pegasus/Common/MessageLoader.h>

#include <Pegasus/ExportServer/CIMExportResponseEncoder.h>
#include <Pegasus/ExportServer/CIMExportRequestDecoder.h>

#include <Pegasus/Consumer/CIMIndicationConsumer.h>
#include <Pegasus/Listener/CIMListenerIndicationDispatcher.h>

PEGASUS_NAMESPACE_BEGIN
/////////////////////////////////////////////////////////////////////////////
// CIMListenerService
/////////////////////////////////////////////////////////////////////////////
class CIMListenerService
{
public:
	CIMListenerService(Uint32 portNumber, SSLContext* sslContext=NULL);
	CIMListenerService(CIMListenerService& svc);
  ~CIMListenerService();

	void				init();
	/** bind to the port
	*/
	void				bind();
	/** runForever Main runloop for the server.
	*/
	void runForever();

	/** Call to gracefully shutdown the server.  The server connection socket
	will be closed to disable new connections from clients.
	*/
	void stopClientConnection();

	/** Call to gracefully shutdown the server.  It is called when the server
	has been stopped and is ready to be shutdown.  Next time runForever()
	is called, the server shuts down.
	*/
	void shutdown();

	/** Return true if the server has shutdown, false otherwise.
	*/
	Boolean terminated() const { return _dieNow; };

	/** Call to resume the sever.
	*/
	void resume();

	/** Call to set the CIMServer state.  Also inform the appropriate
	message queues about the current state of the CIMServer.
	*/
	void setState(Uint32 state);

	Uint32 getOutstandingRequestCount();

	/** Returns the indication listener dispatcher
	 */
	CIMListenerIndicationDispatcher* getIndicationDispatcher() const;

	/** Returns the indication listener dispatcher
	 */
	void setIndicationDispatcher(CIMListenerIndicationDispatcher* dispatcher);

	/** Returns the port number being used.
	 */
	Uint32 getPortNumber() const;

	static PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL _listener_routine(void *param);

private:
	Uint32 _portNumber;
	SSLContext* _sslContext;
	Monitor* _monitor;
	HTTPAcceptor* _acceptor;

  Boolean					_dieNow;

  CIMListenerIndicationDispatcher* _dispatcher;

  CIMExportResponseEncoder* _responseEncoder;
  CIMExportRequestDecoder*  _requestDecoder;

};

CIMListenerService::CIMListenerService(Uint32 portNumber, SSLContext* sslContext)
:_portNumber(portNumber)
,_sslContext(sslContext)
,_monitor(NULL)
,_acceptor(NULL)
,_dieNow(false)
,_dispatcher(NULL)
,_responseEncoder(NULL)
,_requestDecoder(NULL)
{
}

CIMListenerService::CIMListenerService(CIMListenerService& svc)
:_portNumber(svc._portNumber)
,_sslContext(svc._sslContext)
,_monitor(NULL)
,_acceptor(NULL)
,_dieNow(svc._dieNow)
,_dispatcher(NULL)
,_responseEncoder(NULL)
,_requestDecoder(NULL)
{
}
CIMListenerService::~CIMListenerService()
{
    // if port is alive, clean up the port
    //delete _sslContext;

    delete _responseEncoder;

    delete _requestDecoder;

    //delete _dispatcher;

    delete _acceptor;

    delete _monitor;
}

void CIMListenerService::init()
{
	PEG_METHOD_ENTER(TRC_LISTENER, "CIMListenerService::init");

  if(NULL == _monitor) _monitor = new Monitor();

	//_dispatcher = new CIMListenerIndicationDispatcher();

  if(NULL == _responseEncoder) _responseEncoder = new CIMExportResponseEncoder();
  if(NULL == _requestDecoder) _requestDecoder = new CIMExportRequestDecoder(
                                      _dispatcher,_responseEncoder->getQueueId());

  if(NULL == _acceptor) _acceptor = new HTTPAcceptor(
		 _monitor,
		 _requestDecoder,
		 false,
		 _portNumber,
		 _sslContext,
                 false);

  bind();

  PEG_METHOD_EXIT();
}

void CIMListenerService::bind()
{
  if(_acceptor!=NULL)
    { // Bind to the port
      _acceptor->bind();

      //listener.addAcceptor(false, portNumberHttp, false);
      Logger::put(Logger::STANDARD_LOG, System::CIMLISTENER, Logger::INFORMATION,
                        "Listening on HTTP port $0.", _portNumber);

    }
}

void CIMListenerService::runForever()
{
  static int modulator = 0;

  if(!_dieNow)
    {
      if(false == _monitor->run(500000))
	{
	  modulator++;
      try
      {
	     //MessageQueueService::_check_idle_flag = 1;
		 //MessageQueueService::_polling_sem.signal();
		 MessageQueueService::get_thread_pool()->cleanupIdleThreads();
      }
	  catch(...)
      {
      }
	}
/*
      if (handleShutdownSignal)
      {
        Tracer::trace(TRC_SERVER, Tracer::LEVEL3,
	"CIMServer::runForever - signal received.  Shutting down.");

	ShutdownService::getInstance(this)->shutdown(true, 10, false);
	handleShutdownSignal = false;
      }
*/
    }
}

void CIMListenerService::shutdown()
{
    PEG_METHOD_ENTER(TRC_LISTENER, "CIMListenerService::shutdown()");

    _dieNow = true;
    _monitor->tickle();

    PEG_METHOD_EXIT();
}

void CIMListenerService::resume()
{
    PEG_METHOD_ENTER(TRC_LISTENER, "CIMListenerService::resume()");

    if(_acceptor!=NULL)
        _acceptor->reopenConnectionSocket();

    PEG_METHOD_EXIT();
}

void CIMListenerService::stopClientConnection()
{
    PEG_METHOD_ENTER(TRC_LISTENER, "CIMListenerService::stopClientConnection()");

    // tell Monitor to stop listening for client connections
    _monitor->stopListeningForConnections(true);

    //
    // Wait 150 milliseconds to allow time for the Monitor to stop
    // listening for client connections.
    //
    // This wait time is the timeout value for the select() call
    // in the Monitor's run() method (currently set to 100
    // milliseconds) plus a delta of 50 milliseconds.  The reason
    // for the wait here is to make sure that the Monitor entries
    // are updated before closing the connection sockets.
    //
    // pegasus_sleep(150); Not needed now due to the semaphore in the Monitor

    if(_acceptor!=NULL)
    _acceptor->closeConnectionSocket();

    PEG_METHOD_EXIT();
}

Uint32 CIMListenerService::getOutstandingRequestCount()
{
    return _acceptor->getOutstandingRequestCount();
}

CIMListenerIndicationDispatcher* CIMListenerService::getIndicationDispatcher() const
{
    return _dispatcher;
}

void CIMListenerService::setIndicationDispatcher(CIMListenerIndicationDispatcher* dispatcher)
{
    _dispatcher = dispatcher;
}

Uint32 CIMListenerService::getPortNumber() const
{

    Uint32 portNumber = _portNumber;

    if (( portNumber == 0 ) && ( _acceptor != 0 ))
    {
        portNumber = _acceptor->getPortNumber();
    }

    return(portNumber);
}

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL CIMListenerService::_listener_routine(void *param)
{
  try { 
    AutoPtr<CIMListenerService> svc(reinterpret_cast<CIMListenerService *>(param));

    //svc->init(); bug 1394
    while(!svc->terminated())
    {
#if defined(PEGASUS_PLATFORM_DARWIN_PPC_GNU)
        pthread_testcancel();
#endif
        svc->runForever();
    }
  } catch (...) 
  { 
        Tracer::trace(TRC_SERVER, Tracer::LEVEL2,
			"Unknown exception thrown in _listener_routine.");
  }
    return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CIMListenerRep
/////////////////////////////////////////////////////////////////////////////
class CIMListenerRep
{
public:
	CIMListenerRep(Uint32 portNumber, SSLContext* sslContext=NULL);
  ~CIMListenerRep();

	Uint32 getPortNumber() const;

	SSLContext* getSSLContext() const;
	void setSSLContext(SSLContext* sslContext);

	void start();
	void stop();

	Boolean isAlive();

	Boolean addConsumer(CIMIndicationConsumer* consumer);
	Boolean removeConsumer(CIMIndicationConsumer* consumer);

private:
  Boolean waitForPendingRequests(Uint32 shutdownTimeout);

  Uint32 _portNumber;
  SSLContext* _sslContext;

  CIMListenerIndicationDispatcher* _dispatcher;
  ThreadPool* _thread_pool;
  CIMListenerService* _svc;
  Semaphore *_listener_sem;
};

CIMListenerRep::CIMListenerRep(Uint32 portNumber, SSLContext* sslContext)
:_portNumber(portNumber)
,_sslContext(sslContext)
,_dispatcher(new CIMListenerIndicationDispatcher())
,_thread_pool(NULL)
,_svc(NULL)
,_listener_sem(NULL)
{
}

CIMListenerRep::~CIMListenerRep()
{
    // if port is alive, clean up the port
    if (_thread_pool != 0)
    {
        // Block incoming export requests and unbind the port
        _svc->stopClientConnection();

        // Wait until pending export requests in the server are done.
        waitForPendingRequests(10);

        // Shutdown the CIMListenerService
        _svc->shutdown();
    }

    delete _sslContext;
    delete _dispatcher;
    delete _thread_pool;
    delete _listener_sem;

  // don't delete _svc, this is deleted by _listener_routine
}

Uint32 CIMListenerRep::getPortNumber() const
{
    Uint32 portNumber;

    if ( _svc == 0 )
    {
        portNumber = _portNumber;
    }
    else portNumber = _svc->getPortNumber();

    return portNumber;
}

SSLContext* CIMListenerRep::getSSLContext() const
{
    return _sslContext;
}

void CIMListenerRep::setSSLContext(SSLContext* sslContext)
{
    delete _sslContext;
    _sslContext = sslContext;
}

void CIMListenerRep::start()
{
    // spawn a thread to do this
    if(_thread_pool==0)
    {
        AutoPtr<CIMListenerService> svc(new CIMListenerService(_portNumber,_sslContext));

        svc->setIndicationDispatcher(_dispatcher);
        svc->init();

        struct timeval deallocateWait = {15, 0};
        AutoPtr<ThreadPool> threadPool(new ThreadPool(0, "Listener", 0, 1, deallocateWait));
        AutoPtr<Semaphore> sem(new Semaphore(0));
        if (threadPool->allocate_and_awaken(svc.get(), CIMListenerService::_listener_routine, sem.get()) != PEGASUS_THREAD_OK)
    	{
	    Logger::put(Logger::STANDARD_LOG, System::CIMSERVER, Logger::TRACE,
			"Not enough threads to start CIMListernerService.");
 
	    Tracer::trace(TRC_SERVER, Tracer::LEVEL2,
			"Could not allocate thread for CIMListenerService::_listener_routine."); 
	    throw Exception(MessageLoaderParms("Listener.CIMListener.CANNOT_ALLOCATE_THREAD",
				"Could not allocate thread."));
        }
        Logger::put(Logger::STANDARD_LOG,System::CIMLISTENER, Logger::INFORMATION,
                        "CIMListener started");

        _svc = svc.release();
        _thread_pool = threadPool.release();
        _listener_sem = sem.release();
    }
}

void CIMListenerRep::stop()
{
  if(_thread_pool!=NULL)
  {
    //
    // Graceful shutdown of the listener service
    //

    // Block incoming export requests and unbind the port
    _svc->stopClientConnection();

    // Wait until pending export requests in the server are done.
    waitForPendingRequests(10);

    // Shutdown the CIMListenerService
    _svc->shutdown();

    // Wait for the _listener_routine thread to exit.
    // The thread could be delivering an export, so give it 3sec.
    // Note that _listener_routine deletes the CIMListenerService,
    // so no need to delete _svc.
    try
    {
      _listener_sem->time_wait(3000);
    }
    catch (const TimeOut &)
    {
      // No need to do anything, the thread pool will be deleted below
      // to cancel the _listener_routine thread if it is still running.
    }

    delete _listener_sem;
    _listener_sem = NULL;

    // Delete the thread pool.  This cancels the listener thread if it is still
    // running.
    delete _thread_pool;
    _thread_pool = NULL;

    Logger::put(Logger::STANDARD_LOG,System::CIMLISTENER,
		Logger::INFORMATION,
		"CIMListener stopped");
  }
}

Boolean CIMListenerRep::isAlive()
{
	return (_thread_pool!=NULL)?true:false;
}

Boolean CIMListenerRep::addConsumer(CIMIndicationConsumer* consumer)
{
	return _dispatcher->addConsumer(consumer);
}
Boolean CIMListenerRep::removeConsumer(CIMIndicationConsumer* consumer)
{
	return _dispatcher->removeConsumer(consumer);
}

Boolean CIMListenerRep::waitForPendingRequests(Uint32 shutdownTimeout)
{
  // Wait for 10 sec max
  Uint32 reqCount;
  Uint32 countDown = shutdownTimeout * 10;
  for (; countDown > 0; countDown--)
  {
    reqCount = _svc->getOutstandingRequestCount();
    if (reqCount > 0)
      pegasus_sleep(100);
    else
      return true;
  }

  return false;
}

/////////////////////////////////////////////////////////////////////////////
// CIMListener
/////////////////////////////////////////////////////////////////////////////
CIMListener::CIMListener(Uint32 portNumber, SSLContext* sslContext)
:_rep(new CIMListenerRep(portNumber,sslContext))
{
}
CIMListener::~CIMListener()
{
	if(_rep!=NULL)
		delete static_cast<CIMListenerRep*>(_rep);
	_rep=NULL;
}

Uint32 CIMListener::getPortNumber() const
{
	return static_cast<CIMListenerRep*>(_rep)->getPortNumber();
}

SSLContext* CIMListener::getSSLContext() const
{
	return static_cast<CIMListenerRep*>(_rep)->getSSLContext();
}
void CIMListener::setSSLContext(SSLContext* sslContext)
{
	static_cast<CIMListenerRep*>(_rep)->setSSLContext(sslContext);
}
void CIMListener::start()
{
	static_cast<CIMListenerRep*>(_rep)->start();
}
void CIMListener::stop()
{
	static_cast<CIMListenerRep*>(_rep)->stop();
}

Boolean CIMListener::isAlive() const
{
	return static_cast<CIMListenerRep*>(_rep)->isAlive();
}

Boolean CIMListener::addConsumer(CIMIndicationConsumer* consumer)
{
	return static_cast<CIMListenerRep*>(_rep)->addConsumer(consumer);
}
Boolean CIMListener::removeConsumer(CIMIndicationConsumer* consumer)
{
	return static_cast<CIMListenerRep*>(_rep)->removeConsumer(consumer);
}

PEGASUS_NAMESPACE_END
