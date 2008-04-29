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

#ifndef Pegasus_WsmValue_h
#define Pegasus_WsmValue_h

#include <Pegasus/Common/String.h>
#include <Pegasus/Common/ArrayInternal.h>
#include <Pegasus/WsmServer/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

class WsmEndpointReference;
class WsmInstance;

enum WsmType
{
    WSMTYPE_REFERENCE,
    WSMTYPE_OBJECT,
    WSMTYPE_INSTANCE,
    WSMTYPE_OTHER
};


class PEGASUS_WSMSERVER_LINKAGE WsmValue
{
public:

    WsmValue();
    WsmValue(const WsmEndpointReference& ref);
    WsmValue(const Array<WsmEndpointReference>& ref);
    WsmValue(const WsmInstance& inst);
    WsmValue(const Array<WsmInstance>& inst);
    WsmValue(const String& str);
    WsmValue(const Array<String>& str);
    WsmValue(const WsmValue& val);

    ~WsmValue()
    {
        _release();
    }

    WsmValue& operator=(const WsmValue& val);

    Boolean isArray() const { return _isArray; }
    Boolean isNull() const { return _isNull; }
    void setNull();
    WsmType getType() const { return _type; }
    Uint32 getArraySize();

    void toArray();
    void add(const WsmValue& val);

    void get(WsmEndpointReference& ref) const;
    void get(Array<WsmEndpointReference>& ref) const;
    void get(WsmInstance& inst) const;
    void get(Array<WsmInstance>& inst) const;
    void get(String& str) const;
    void get(Array<String>& str) const;

    void set(const WsmEndpointReference& ref);
    void set(const Array<WsmEndpointReference>& ref);
    void set(const WsmInstance& inst);
    void set(const Array<WsmInstance>& inst);
    void set(const String& str);
    void set(const Array<String>& str);

private:

    void _release();

    WsmType _type;
    Boolean _isArray;
    Boolean _isNull;

    union WsmValueRep
    {
        WsmEndpointReference* ref;
        Array<WsmEndpointReference>* refa;
        WsmInstance* inst;
        Array<WsmInstance>* insta;
        Array<String>* stra;
        String* str;
    } _rep;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_WsmValue_h */
