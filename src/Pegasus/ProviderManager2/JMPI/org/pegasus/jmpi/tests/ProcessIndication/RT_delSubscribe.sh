wbemexec -p 5988 -h localhost DeleteSubscription01.xml > DeleteSubscription01.rsp && diff DeleteSubscription01.rsp DeleteSubscription01rspgood.xml
wbemexec -p 5988 -h localhost DeleteFilter01.xml > DeleteFilter01.rsp && diff DeleteFilter01.rsp DeleteFilter01rspgood.xml
wbemexec -p 5988 -h localhost DeleteHandler01.xml > DeleteHandler01.rsp && diff DeleteHandler01.rsp DeleteHandler01rspgood.xml
