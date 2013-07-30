cimr Demo

Requirements:
- Webserver (e.g. Apache HTTPD) to host the files contained
  in this directory
- Dojo 1.2+, available from http://dojotoolkit.org/
  Extract the downloaded archive into this directory;
  after extracting, this directory should contain the
  following sub-directories:
  cimr/
  dojo/
  dojox/
  dijit/

 Configuration:
 After loading the HTML/JavaScript application, AJAX calls are made
 to http://localhost/cimrs/<hostname> where hostname is entered via
 the UI -- this is due to the same-origin policy employed by browsers
 for AJAX requests.
 The following sample configuration configures Apache to proxy requests
 for that location towards OpenPegasus.

 /etc/httpd/conf.d/cimrs.conf:
 ProxyRequests Off

<Proxy *>
  Order deny,allow
  Allow from all
</Proxy>

ProxyPass /cimrs/localhost http://localhost:5988
ProxyPassReverse /cimrs/localhost http://localhost:5988

