wbemexec -h localhost -p 5988 CreateFilter01.xml > CreateFilter01.rsp && diff CreateFilter01.rsp CreateFilter01rspgood.xml
wbemexec -h localhost -p 5988 CreateHandler01.xml > CreateHandler01.rsp && diff CreateHandler01.rsp CreateHandler01rspgood.xml
wbemexec -h localhost -p 5988 CreateSubscription01.xml > CreateSubscription01.rsp && diff CreateSubscription01.rsp CreateSubscription01rspgood.xml
