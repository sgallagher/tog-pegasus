wbemexec -p 5988 -h localhost SendTestIndication1.xml > SendTestIndication1.rsp && diff SendTestIndication1.rsp SendTestIndication1rspgood.xml
wbemexec -p 5988 -h localhost SendTestIndication2.xml > SendTestIndication2.rsp && diff SendTestIndication2.rsp SendTestIndication2rspgood.xml
