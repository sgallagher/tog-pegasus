#ifndef Pegasus_Channel_h
#define Pegasus_Channel_h

#include <Pegasus/Common/Config.h>

PEGASUS_NAMESPACE_BEGIN

/**
    For a quicker understanding, look first at the ChannelConnector class.
    Then look at the ChannelAcceptor class.
*/

/** The Channel class defines the interface used for reading and writing a
    channel. A pair of channels is created for each client/server channel (one
    in the client process and one in the server process). For example, if a
    client connects to three servers, the client will have tree Channel
    instances (one for each server to which the client is connected). If two
    clients connect to a server, that server will have two Channel instances
    (one for each client connected to it).

    ATTN: Mike. First sentence.  probably need to define channel itself rahter
    than thus channel class defines channel.  EX. Channel class defines the
    channel which is the virtualization of a communication path to a remote
    endpoint.

    Channels are created automatically by the underlying implementation. It is
    never necessary for the user to create a channel directly. By convention,
    Channel implementations are hidden from the user so that he may not
    instantiate them.
*/
class PEGASUS_COMM_LINKAGE Channel
{
public:

    /** Virtual destructor.
    */
    virtual ~Channel() = 0;

    /** Method to read up to size bytes from the channel into the memory area
	specified by the ptr argument
	@param ptr - Pointer to memory area into which bytes are read.
	@param size - The maximum number of byest to read.
	@return The number of bytes read or -1 on failure.  The return value may
	be smaller that the size argument
    */
    virtual Sint32 read(void* ptr, Uint32 size) = 0;

    /** Method to write up to size bytes onto the channel from the memory
        area specified by the ptr argument.
        @parm ptr - Pointer to memory area from which bytes are written to the
        channel
	@param size - Maximum number of bytes to write to the channel
	@retrun - Thie method returns the number of bytes
	written or negative one on failure. The return value may be smaller
	than the size argument.
    */
    virtual Sint32 write(const void* ptr, Uint32 size) = 0;

    /** Method to read size bytes from the channel into the memory area
	specified by the ptr argument.
	@param ptr - Pointer to memory area into which input bytes are placed.
	@param size - Number of bytes to read into the memory area.
	@return - This method will return the size argument or negative one on
	failure.
	COMMENT: Clafify issue if for some reason less than this received
    */
    virtual Sint32 readN(void* ptr, Uint32 size) = 0;

    /** Method to write size bytes onto the channel from the memory area
	specified by the ptr argument.
	@param ptr - Pointer to memory area from which data is written
	@param size - Number of bytes to write.
	@return - This method will return the size argument or negative one on
	failure.
    */
    virtual Sint32 writeN(const void* ptr, Uint32 size) = 0;

    /** Enable blocking I/O. This will affect whether the read() and write()
	routines block or not.
	COMMENT: Why not do one function and call it blocking(boolean).
    */
    virtual void enableBlocking() = 0;

    /** Disable blocking I/O. This will affect whether the read() and write()
	routines block or not.
    */
    virtual void disableBlocking() = 0;
};

/** The user of the channel module derives from the ChannelHandler class
    to provide implementations of the handleOpen(), handleInput(), and
    handleClose() methods. These methods are called to notify the user of
    input/output events which occur on the channel. For each Channel instance,
    there is a corresponding ChannelHandler instance (the Channel implementation
    will most likely maintain a pointer to its ChannelHandler).

    To illustrate when each method is called we consider and example. When a
    client connects to a server, Channel/ChannelHandler instance pairs are
    created in both process. Then the ChannelHandler::handleOpen() method is
    called in the server. Then the ChannelHandler::handleInput() method is
    called in the client. This notifies the user of the channel module that
    a channel has been opened.

    Now suppose that the client process calls Channel::write(). This sends
    a stream of bytes (specified by the argument of write()) to the server.
    This causes the corresponding ChannelHandler::handleInput() method to
    be called. In order to read the stream of bytes, the server must call
    the read() method of channel argument.

    Similarly, the server may call Channel::write() which sends a
    stream of bytes back to client, causing the corresponding
    ChannelHandler::handleInput() to be called. And similarly, the read()
    method of the channel argument may be invoked obtain the stream of bytes.

    Establishing a channel is asymmetric: the client is active and the
    server is passive. But once a channel is established, all communication
    occurs symmetrically: either party may initiate a write at any time causing
    the other peer's ChannelHandler::handleInput() to be invoked.

    When a connection is closed by one peer, the handleClose() method is
    called on the corresponding ChannelHandler instance in the other peer.
    The handleClose() will also be closed if the remote process exits (in
    TCP is referred to as hanging up).
*/
class PEGASUS_COMM_LINKAGE ChannelHandler
{
public:

    /** Virtual destructor.
    */
    virtual ~ChannelHandler() = 0;

    /** Called when a connection is finally established. For the client this
	happens immediately after the client initiates a connection. For the
	server, this happens immediately after a client connects to the
	server. Note that a distinct handler is created for each communication
	end point. That is for each client/server connection there will be
	two channels created on each end and this method will be called in
	each.
	@return The method returns false to close the connection. COMMENT: This
	not really clear.  Does it ever return true?
    */
    virtual Boolean handleOpen(Channel* channel) = 0;

    /** Called when the remote peer calls the Channel::write() method on
	the corresponding chanel.
	@return The method returns false to close the
	connection.
    */
    virtual Boolean handleInput(Channel* channel) = 0;

    /** Called when the remote peer closes the connection. This method will
	also be called when the remote peer process exits.
    */
    virtual void handleClose(Channel* channel) = 0;
};

/** The ChannelHandlerFactory class must be derived from and implemented
    by the user. The ChannelConnector and ChannelAcceptor use the
    ChannelHandlerFactory to create instances of ChannelHandler when
    connections are initated (by the client) and when they are accepted
    (by the server). Implementations of this class may be used to pass
    constructor arguments to the ChannelHandler implementation. Here is
    an example of a ChannelHandlerFactory which passes an integer value
    to the constructor of the ChannelHandler's constructor.

    <pre>
	class MyChannelHandler : public ChannelHandler
	{
	public:

	    MyChannelHandler(int someValue);

	    virtual ~MyChannelHandler();

	    virtual Boolean handleOpen(Channel* channel);

	    virtual Boolean handleInput(Channel* channel);

	    virtual void handleClose(Channel* channel);

	private:
	    int _someValue;
	};

	class MyChannelHandlerFactory : public ChannelHandlerFactory
	{
	public:

	    MyChannelHandlerFactory(int someValue) : _someValue(someValue)
	    {

	    }

	    virtual ~MyChannelHandlerFactory();

	    virtual ChannelHandler* create()
	    {
		return new MyChannelHandler(_someValue);
	    }

	private:
	    int _someValue;
	};
    </pre>

    If the ChannelHandler implementation has no constructor arguments
    (and uses a simple default constructor), then it is not necessary to
    implement a ChannelHandlerFactory. Instead, it is possible to use
    the DefaultChannelHandlerFactory<> template defined later. Here is
    an example:

    <pre>
	ChannelHandlerFactory* factory;
	factory = new DefaultChannelHandlerFactory<MyChannelHandler>;
    <pre>
*/
class ChannelHandlerFactory
{
public:

    virtual ~ChannelHandlerFactory() = 0;

    virtual ChannelHandler* create() = 0;
};

/** The DefaultChannelHandlerFactory<> template class provides an easy way
    to provide a ChannelHandlerFactory implementation in cases in which
    the ChannelHandler has a simple default constructor (rather than one
    with arguments).
*/
template<class CHANNEL_HANDLER>
class DefaultChannelHandlerFactory : public ChannelHandlerFactory
{
public:

    virtual ~ChannelHandlerFactory() { }

    virtual ChannelHandler* create() { return new CHANNEL_HANDLER(); }
};

/** The ChannelConnector is a client-side object used to create and connect
    a channel to a server (see the connect() method). A ChannelHandlerFactory
    is passed to the constructor. Each time connect() is called, an instance
    of Channel (internal implementation) is created and an instance of
    ChannelHandler is created using the supplied factory. As described in the
    ChannelHandler documentation, Methods of the ChannelHandler are invoked
    in response to various events. The following example, illustrates the use
    of the ChannelConnector:

    <pre>
	class MyChannelHandler : public ChannelHandler
	{
	public:

	    virtual Boolean handleOpen(Channel* channel);

	    virtual Boolean handleInput(Channel* channel);

	    virtual void handleClose(Channel* channel);
	};

	...

	// Create a channel handler factory:

	typedef DefaultChannelHandlerFactory<MyChannelHandler> MyFactory;
	MyFactory* factory = new MyFactory;

	// Create a channel connector (in this example, a hypothetical TCP
	// channel connector implementation is used).

	ChannelConnector* connector = new TcpChannelConnector;

	// Connect to a TCP server to megabucks.com on port 8888; this causes
	// the following actions to be taken:
	//
	//     1. A connection is made to the given address.
	//     2. An instance of Channel is create for the new connection.
	//     3. The factory is used to create an instance of MyChannelHandler.
	//     4. The handleOpen() method is called on the new MyChannelHandler.
	//
	// It is not necessary to retain a pointer to the channel.  The
	// internal implementation is responsible for disposing of the channel
	// object.

	Channel* channel = connector->connect("megabucks.com:8888");

	// Write something to the server. This will cause the handleInput()
	// method to be called on the server's corresponding ChannelHandler.

	const char MESSAGE[] = "Hello World";
	channel->write(MESSAGE, sizeof(MESSAGE));
    </pre>
*/
class PEGASUS_COMM_LINKAGE ChannelConnector
{
public:

    /** Constructor. Stores the factory argument for later use. The factory
	is used later by the connect method to create ChannelCallbacks.
    */
    ChannelConnector(ChannelHandlerFactory* factory) : _factory(factory) { }

    /**  Virtual destructor.
    */
    virtual ~ChannelConnector() = 0;

    /** Connects to the given address. Creates a Channel instance to hold state
	about the connection. Then the factory is used to create a
	ChannelHandler for the connection. Next, handleOpen() method of this
	new ChannelHandler is invoked. The newly created channel is returned.
	The channel connector is responsible for ultimately disposing of the
	connection object. The caller MUST NOT dispose of the object.
	@param address - string that ATTN:
	@return - Pointer to the new created channel.
    */
    virtual Channel* connect(const char* addresss) = 0;

    /** Disconnects from the server and destroys the channel argument. This
	method may be explicitly by the user or may be called implicitly when
	the server severs the connections or hangs up.
    */
    virtual void disconnect(Channel* channel) = 0;

private:

    ChannelHandlerFactory* _factory;
};

/** The ChannelAcceptor is a server-side object. It is used to listen for
    and accept client connections. When a client connects, The ChannelAcceptor
    creates a new Connection instance to represeent the connection. Then the
    ChannelHandlerFactory is used to create a ChannelHandler for that
    connection. Finally, the handleInput() method is called on the new
    ChannelHandler. The following example illustrates how to use the
    ChannelAcceptor. Note that the steps for defining a ChannelHandler and
    ChannelHandlerFactory are the same for a client.

    <pre>
	class MyChannelHandler : public ChannelHandler
	{
	public:

	    virtual Boolean handleOpen(Channel* channel);

	    virtual Boolean handleInput(Channel* channel);

	    virtual void handleClose(Channel* channel);
	};

	...

	// Create a channel handler factory:

	typedef DefaultChannelHandlerFactory<MyChannelHandler> MyFactory;
	MyFactory* factory = new MyFactory;

	// Create a ChannelAcceptor (in this example, a hypothetical TCP
	// channel acceptor implementation is used).

	ChannelAcceptor* acceptor = new TcpChannelAcceptor;

	// Bind the acceptor to a given address to listen on (in this case
	// it happens to be a TCP port:

	acceptor->bind("8888");
    </pre>
*/
class PEGASUS_COMM_LINKAGE ChannelAcceptor
{
public:

    /** Constructor. Store the factory for later use

    ChannelAcceptor(ChannelHandlerFactory* factory) : _factory(factory) { }

    virtual ~ChannelAcceptor() = 0;

    virtual void bind(const char* bindString) = 0;

    virtual void accept(Channel* channel) = 0;

private:

    ChannelHandlerFactory* _factory;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_Channel_h */

