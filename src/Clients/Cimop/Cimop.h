// Copyright (c) 2001, 2002 Hewlett-Packard Company

#ifndef _Cimop_h_
#define _Cimop_h_

#include <Pegasus/Client/CIMClient.h>

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

void _usage();
void _giUsage();
void _diUsage();
void _gpUsage();
void _spUsage();
void _miUsage();
void _ciUsage();
int _getClass(const int, const char **);
int _enumerateClasses(const int, const char **);
int _enumerateClassNames(const int, const char **);
int _getInstance(const int, const char **);
int _enumerateInstances(const int, const char **);
int _enumerateInstanceNames(const int, const char **);
int _getProperty(const int, const char **);
int _setProperty(const int, const char **);
int _invokeMethod(const int, const char **);
int _createClass(const int, const char **);
int _modifyClass(const int, const char **);
int _deleteClass(const int, const char **);
int _createInstance(const int, const char **);
int _modifyInstance(const int, const char **);
int _deleteInstance(const int, const char **);
int _associators(const int, const char **);
int _associatorNames(const int, const char **);
int _references(const int, const char **);
int _referenceNames(const int, const char **);
int _execQuery(const int, const char **);
int _getQualifier(const int, const char **);
int _setQualifier(const int, const char **);
int _deleteQualifier(const int, const char **);
int _enumerateQualifiers(const int, const char **);
void _displayInstance(CIMInstance&);
Array<CIMKeyBinding> _inputInstanceKeys(CIMClass&);
CIMObjectPath _selectInstance(const String&);
Boolean _isKey(const CIMProperty&);
String _displayProperty(const CIMProperty&);
CIMValue _makeValue(const char*, const CIMProperty&);
CIMKeyBinding _makeKey(const CIMProperty &);
int _recursiveClassEnum(const CIMName&, String);
CIMClass _makeNamespaceClass(void);
Boolean verifyServerCertificate(SSLCertificateInfo &certInfo);

#endif
