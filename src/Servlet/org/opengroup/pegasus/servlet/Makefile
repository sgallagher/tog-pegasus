JAVAC=/opt/java1.2/bin/javac -classpath $(CLASSPATH)

CLASSES =  			      \
		AuthenticationFailedException.class \
		SocketUnavailableException.class \
		LocalAuthHandler.class  \
		Log.class  \
		Trace.class  \
		ConnectionPool.class  \
		CIMOMResponse.class  \
		CIMServerSocket.class  \
		CIMServlet.class       

all: $(CLASSES)


CIMServlet.class: CIMServlet.java
	$(JAVAC) CIMServlet.java

CIMOMResponse.class: CIMOMResponse.java
	$(JAVAC) CIMOMResponse.java

CIMServerSocket.class: CIMServerSocket.java
	$(JAVAC) CIMServerSocket.java

ConnectionPool.class: ConnectionPool.java
	$(JAVAC) ConnectionPool.java

Log.class: Log.java
	$(JAVAC) Log.java

Trace.class: Trace.java
	$(JAVAC) Trace.java

LocalAuthHandler.class: LocalAuthHandler.java
	$(JAVAC) LocalAuthHandler.java

AuthenticationFailedException.class: AuthenticationFailedException.java
	$(JAVAC) AuthenticationFailedException.java

SocketUnavailableException.class: SocketUnavailableException.java
	$(JAVAC) SocketUnavailableException.java

clean:
	rm -f *.class

