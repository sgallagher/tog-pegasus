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

#ifndef Pegasus_MessageQueue_h
#define Pegasus_MessageQueue_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Message.h>
#include <Pegasus/Common/Mutex.h>
#include <Pegasus/Common/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

/** The MessageQueue class represents a queue abstraction and is used by
    modules to exchange messages. Methods are provided for enqueuing,
    dequeuing, removing, iterating messages. Some methods are virtual and
    may be overriden but subclasses to modify the behavior.

    <h1>A Word about Queue Ids</h1>

    You may pass a specific queue id to the MessageQueue() constructor. The
    default is to call MessageQueue::getNextQueueId() to obtain one. Only pass
    queue ids generated by calling MessageQueue::getNextQueueId() to this
    constructor. Otherwise, you might end up with two queues with the same
    queue id.

    A technique we encourage, is to declare global queue ids like this:

    <pre>
    extern const Uint32 GROCERY_QUEUE_ID;
    </pre>

    And then define them like this:

    <pre>
    const Uint32 GROCERY_QUEUE_ID = MessageQueue::getNextQueueId();
    </pre>

    And then pass them to the constructor of MessageQueue (from the derived
    class). In this way you will secure a unique constant identifier by which
    you may refer to a queue later on.

    <h1>A Word about using the find() Methods</h1>

    There are two find() methods. One that takes a queue id and one that
    takes a name. The time complexity of the former is O(1); whereas, the
    time complexity of the latter is O(n). Therefore, use the queue id form
    since it is more efficient.
*/
class PEGASUS_COMMON_LINKAGE MessageQueue
{
public:

    /** This constructor places this object on a queue table which is
    maintained by this class. Each message queue has a queue-id (which
    may be obtained by calling getQueueId()). The queue-id may be passed
    to lookupQueue() to obtain a pointer to the corresponding queue).

    @param queueId the queue id to be used by this object. ONLY PASS IN
    QUEUE IDS WHICH WERE GENERATED USING MessageQueue::getNextQueueId().
    Otherwise, you might end up with more than one queue with the same
    queue id.
    */
    MessageQueue(
        const char *name,
        Boolean async = false,
        Uint32 queueId = MessageQueue::getNextQueueId());

    /** Removes this queue from the queue table. */
    virtual ~MessageQueue();

    /** Enques a message (places it at the back of the queue).
    @param message pointer to message to be enqueued.
    @exception  NullPointer exception if message parameter is null.
    @exception  IPCException if socket call has an error
    */
    virtual void enqueue(Message* message);

    /** allows a caller to determine if this message queue is asynchronous or
    not.
    */
    virtual Boolean isAsync() const { return _async; }

    /** Dequeues a message (removes it from the front of the queue).
    @return pointer to message or zero if queue is empty.
    @exception IPCException Thrown if an IPC error occurs.
    */
    virtual Message* dequeue();

    /** Returns true if there are no messages on the queue. */
    Boolean isEmpty() const throw() { return _front == 0; }

    /** Returns the number of messages on the queue. */
    Uint32 getCount() const throw() { return _count; }

    /** Retrieve the queue id for this queue. */
    Uint32 getQueueId() const throw() { return _queueId; }

    Uint32 get_capabilities() const throw()
    {
        return _capabilities;
    }

    #ifdef PEGASUS_DEBUG
    /** Prints the contents of this queue by calling the print() method
    of each message.
    @param os stream onto which the output is placed.
    @exception IPCException Thrown if an IPC error occurs.
    */
    void print(PEGASUS_STD(ostream)& os) const;
    #endif 

    /** Provide a string name for this queue to be used by the print method.
     */
    const char* getQueueName() const;

    /** This method is called after a message has been enqueued. This default
    implementation does nothing. Derived classes may override this to
    take some action each time a message is enqueued (for example, this
    method could handle the incoming message in the thread of the caller
    of enqueue()).
    */
    virtual void handleEnqueue() ;

    /** This method <b>may</b> be called prior to enqueueing an message.
    the message queue can inform the caller that it does not want
    to handle the message by returning false **/

    virtual Boolean messageOK(const Message *msg) { return true ;}

    /** Lookup a message queue from a queue id. Note this is an O(1) operation.
    @exception IPCException Thrown if an IPC error occurs.
     */
    static MessageQueue* lookup(Uint32 queueId);

    /** Lookup a message given a queue name. NOte this is an O(N) operation.
    @exception IPCException Thrown if an IPC error occurs.
     */
    static MessageQueue* lookup(const char *name);

    /** Get the next available queue id. It always returns a non-zero
    queue id an monotonically increases and finally wraps (to one)
    after reaching the maximum unsigned 32 bit integer.
    @exception IPCException Thrown if an IPC error occurs.
    */
    static Uint32 getNextQueueId();

    /** Put the queue id into the stack.
    */
    static void putQueueId(Uint32 queueId);

protected:
    Uint32 _queueId;
    char *_name;
    Uint32 _capabilities;

private:
    Mutex _mut;
    Uint32 _count;
    Message* _front;
    Message* _back;
    Boolean _async;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_MessageQueue_h */
