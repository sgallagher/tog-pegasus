//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CIMListener.h"

#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/SSLContext.h>
#include <Pegasus/Common/Monitor.h>
#include <Pegasus/Common/HTTPAcceptor.h>
#include <Pegasus/Common/PegasusVersion.h>

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
	Boolean terminated() { return _dieNow; };
	
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

	static PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL _listener_routine(void *param);

private:
	Uint32			_portNumber;
	SSLContext* _sslContext;

	Monitor*				_monitor;
  HTTPAcceptor*   _acceptor;
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
	//if(_sslContext!=NULL)
	//	delete _sslContext;

	if(_responseEncoder!=NULL)
		delete _responseEncoder;

	if(_requestDecoder!=NULL)
		delete _requestDecoder;

	//if(_dispatcher!=NULL)
	//	delete _dispatcher;

	if(_monitor!=NULL)
		delete _monitor;

	if(_acceptor!=NULL)
		delete _acceptor;
}

void CIMListenerService::init()
{
	PEG_METHOD_ENTER(TRC_LISTENER, "CIMListenerService::init");

  _monitor = new Monitor(true);
	//_dispatcher = new CIMListenerIndicationDispatcher();

	_responseEncoder = new CIMExportResponseEncoder();
  _requestDecoder = new CIMExportRequestDecoder(
		_dispatcher,
		_responseEncoder->getQueueId());

	_acceptor = new HTTPAcceptor(
		 _monitor, 
		 _requestDecoder, 
		 false, 
		 _portNumber, 
		 _sslContext);

	bind();

    PEG_METHOD_EXIT();
}
void CIMListenerService::bind()
{
	if(_acceptor!=NULL)
	{ // Bind to the port
		_acceptor->bind();

		PEGASUS_STD(cout) << "Listening on HTTP port " << _portNumber << PEGASUS_STD(endl);
		
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
		if(false == _monitor->run(100))
		{
			modulator++;
			if(!(modulator % 5000) )
			{
				try 
				{
					//MessageQueueService::_check_idle_flag = 1;
					//MessageQueueService::_polling_sem.signal();
                                        MessageQueueService::get_thread_pool()->kill_idle_threads();
				}
				catch(...)
				{
				}
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
    _monitor->stopListeningForConnections();

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
    pegasus_sleep(150);

    if(_acceptor!=NULL)
    _acceptor->closeConnectionSocket();

    PEG_METHOD_EXIT();
}


CIMListenerIndicationDispatcher* CIMListenerService::getIndicationDispatcher() const
{
	return _dispatcher;
}
void CIMListenerService::setIndicationDispatcher(CIMListenerIndicationDispatcher* dispatcher)
{
	_dispatcher = dispatcher;
}

PEGASUS_THREAD_RETURN PEGASUS_THREAD_CDECL CIMListenerService::_listener_routine(void *param)
{
  CIMListenerService *svc = reinterpret_cast<CIMListenerService *>(param);

	svc->init();
	while(!svc->terminated())
	{
	  svc->runForever();
}
	delete svc;

	return 0;
}
static struct timeval create_time = {0, 1};
static struct timeval destroy_time = {15, 0};
static struct timeval deadlock_time = {0, 0};

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
	Uint32			_portNumber;
	SSLContext* _sslContext;

  CIMListenerIndicationDispatcher* _dispatcher;
	ThreadPool* _thread_pool;
};

CIMListenerRep::CIMListenerRep(Uint32 portNumber, SSLContext* sslContext)
:_portNumber(portNumber)
,_sslContext(sslContext)
,_dispatcher(new CIMListenerIndicationDispatcher())
,_thread_pool(NULL)
{
}
CIMListenerRep::~CIMListenerRep()
{
	// if port is alive, clean up the port
	if(_sslContext!=NULL)
		delete _sslContext;

	if(_dispatcher!=NULL)
		delete _dispatcher;

	if(_thread_pool!=NULL)
		delete _thread_pool;
}

Uint32 CIMListenerRep::getPortNumber() const
{
	return _portNumber;
}

SSLContext* CIMListenerRep::getSSLContext() const
{
	return _sslContext;
}
void CIMListenerRep::setSSLContext(SSLContext* sslContext)
{
	if(_sslContext!=NULL)
		delete _sslContext;

	_sslContext = sslContext;
}
void CIMListenerRep::start()
{
	// spawn a thread to do this
	if(_thread_pool==NULL)
	{
		_thread_pool = new ThreadPool(0, "Listener", 0, 1, 
			create_time, destroy_time, deadlock_time);

		CIMListenerService* svc = new CIMListenerService(_portNumber,_sslContext);
		svc->setIndicationDispatcher(_dispatcher);

		_thread_pool->allocate_and_awaken(svc,CIMListenerService::_listener_routine);

		Logger::put(Logger::STANDARD_LOG,System::CIMLISTENER,
					      Logger::INFORMATION,
				        "CIMListener started");

		PEGASUS_STD(cerr) << "CIMlistener started" << PEGASUS_STD(endl);
	}
}

void CIMListenerRep::stop()
{
	if(_thread_pool!=NULL)
	{ // stop the thread
		
		delete _thread_pool;
		
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

Boolean CIMListener::isAlive()
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
