//%LICENSE////////////////////////////////////////////////////////////////
//
// Licensed to The Open Group (TOG) under one or more contributor license
// agreements.  Refer to the OpenPegasusNOTICE.txt file distributed with
// this work for additional information regarding copyright ownership.
// Each contributor licenses this file to you under the OpenPegasus Open
// Source License; you may not use this file except in compliance with the
// License.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//////////////////////////////////////////////////////////////////////////
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/PegasusAssert.h>

#include "WsmValue.h"
#include "WsmEndpointReference.h"
#include "WsmInstance.h"

PEGASUS_NAMESPACE_BEGIN

WsmValue::WsmValue()
    : _type(WSMTYPE_OTHER),
      _isArray(false),
      _isNull(true)
{
    _rep.str = 0;
}

WsmValue::WsmValue(const WsmValue& val)
    : _type(WSMTYPE_OTHER),
      _isArray(false),
      _isNull(true)
{
    *this = val;
}

WsmValue::WsmValue(const WsmEndpointReference& ref)
    : _type(WSMTYPE_REFERENCE),
      _isArray(false),
      _isNull(false)
{
    _rep.ref = new WsmEndpointReference(ref);
}

WsmValue::WsmValue(const Array<WsmEndpointReference>& ref)
    : _type(WSMTYPE_REFERENCE),
      _isArray(true),
      _isNull(false)
{
    _rep.refa = new Array<WsmEndpointReference>(ref);
}

WsmValue::WsmValue(const WsmInstance& inst)
    : _type(WSMTYPE_INSTANCE),
      _isArray(false),
      _isNull(false)
{
    _rep.inst = new WsmInstance(inst);
}

WsmValue::WsmValue(const Array<WsmInstance>& inst)
    : _type(WSMTYPE_INSTANCE),
      _isArray(true),
      _isNull(false)
{
    _rep.insta = new Array<WsmInstance>(inst);
}

WsmValue::WsmValue(const String& str)
    : _type(WSMTYPE_OTHER),
      _isArray(false),
      _isNull(false)
{
    _rep.str = new String(str);
}

WsmValue::WsmValue(const Array<String>& str)
    : _type(WSMTYPE_OTHER),
      _isArray(true),
      _isNull(false)
{
    _rep.stra = new Array<String>(str);
}

WsmValue& WsmValue::operator=(const WsmValue& val)
{
    if (this == &val)
    {
        return *this;
    }

    _release();

    if (!val._isNull)
    {
        if (val._isArray)
        {
            switch (val._type)
            {
                case WSMTYPE_REFERENCE:
                    _rep.refa = new Array<WsmEndpointReference>(*val._rep.refa);
                    break;

                case WSMTYPE_INSTANCE:
                    _rep.insta = new Array<WsmInstance>(*val._rep.insta);
                    break;

                case WSMTYPE_OTHER:
                    _rep.stra = new Array<String>(*val._rep.stra);
                    break;

                default:
                    PEGASUS_ASSERT(0);
                    break;
            }
        }
        else
        {
            switch (val._type)
            {
                case WSMTYPE_REFERENCE:
                    _rep.ref = new WsmEndpointReference(*val._rep.ref);
                    break;

                case WSMTYPE_INSTANCE:
                    _rep.inst = new WsmInstance(*val._rep.inst);
                    break;

                case WSMTYPE_OTHER:
                    _rep.str = new String(*val._rep.str);
                    break;

                default:
                    PEGASUS_ASSERT(0);
                    break;
            }
        }
    }

    _type = val._type;
    _isArray = val._isArray;
    _isNull = val._isNull;

    return *this;
}

Uint32 WsmValue::getArraySize()
{
    if (_isArray)
    {
        switch (_type)
        {
            case WSMTYPE_REFERENCE:
                return _rep.refa->size();

            case WSMTYPE_INSTANCE:
                return _rep.insta->size();

            case WSMTYPE_OTHER:
                return _rep.stra->size();

            default:
                PEGASUS_ASSERT(0);
        }
    }

    return 0;
}

void WsmValue::_release()
{
    if (_isNull)
        return;

    if (_isArray)
    {
        switch (_type)
        {
            case WSMTYPE_REFERENCE:
                delete _rep.refa;
                break;

            case WSMTYPE_INSTANCE:
                delete _rep.insta;
                break;

            case WSMTYPE_OTHER:
                delete _rep.stra;
                break;

            default:
                PEGASUS_ASSERT(0);
                break;
        }
    }
    else
    {
        switch (_type)
        {
            case WSMTYPE_REFERENCE:
                delete _rep.ref;
                break;

            case WSMTYPE_INSTANCE:
                delete _rep.inst;
                break;

            case WSMTYPE_OTHER:
                delete _rep.str;
                break;

            default:
                PEGASUS_ASSERT(0);
                break;
        }
    }
}

void WsmValue::get(WsmEndpointReference& ref) const
{
    if (_type != WSMTYPE_REFERENCE || _isArray)
        throw TypeMismatchException();

    if (!_isNull)
        ref = *_rep.ref;
}

void WsmValue::get(Array<WsmEndpointReference>& ref) const
{
    if (_type != WSMTYPE_REFERENCE || !_isArray)
        throw TypeMismatchException();

    if (!_isNull)
        ref = *_rep.refa;
}

void WsmValue::get(WsmInstance& inst) const
{
    if (_type != WSMTYPE_INSTANCE || _isArray)
        throw TypeMismatchException();

    if (!_isNull)
        inst = *_rep.inst;
}

void WsmValue::get(Array<WsmInstance>& inst) const
{
    if (_type != WSMTYPE_INSTANCE || !_isArray)
        throw TypeMismatchException();

    if (!_isNull)
        inst = *_rep.insta;
}

void WsmValue::get(String& str) const
{
    if (_type != WSMTYPE_OTHER || _isArray)
        throw TypeMismatchException();

    if (!_isNull)
        str = *_rep.str;
}

void WsmValue::get(Array<String>& str) const
{
    if (_type != WSMTYPE_OTHER || !_isArray)
        throw TypeMismatchException();

    if (!_isNull)
        str = *_rep.stra;
}

void WsmValue::set(const WsmEndpointReference& ref)
{
    _release();
    _type = WSMTYPE_REFERENCE;
    _isArray = false;
    _isNull = false;
    _rep.ref = new WsmEndpointReference(ref);
}

void WsmValue::set(const Array<WsmEndpointReference>& ref)
{
    _release();
    _type = WSMTYPE_REFERENCE;
    _isArray = true;
    _isNull = false;
    _rep.refa = new Array<WsmEndpointReference>(ref);
}

void WsmValue::set(const WsmInstance& inst)
{
    _release();
    _type = WSMTYPE_INSTANCE;
    _isArray = false;
    _isNull = false;
    _rep.inst = new WsmInstance(inst);
}

void WsmValue::set(const Array<WsmInstance>& inst)
{
    _release();
    _type = WSMTYPE_INSTANCE;
    _isArray = true;
    _isNull = false;
    _rep.insta = new Array<WsmInstance>(inst);
}

void WsmValue::set(const String& str)
{
    _release();
    _type = WSMTYPE_OTHER;
    _isArray = false;
    _isNull = false;
    _rep.str = new String(str);
}

void WsmValue::set(const Array<String>& str)
{
    _release();
    _type = WSMTYPE_OTHER;
    _isArray = true;
    _isNull = false;
    _rep.stra = new Array<String>(str);
}

void WsmValue::setNull()
{
    _release();
    _type = WSMTYPE_OTHER;
    _isArray = false;
    _isNull = true;
}

void WsmValue::add(const WsmValue& val)
{
    PEGASUS_ASSERT(!val._isArray);

    if (_type != val._type)
        throw TypeMismatchException();

    if (_isArray)
    {
        switch (_type)
        {
            case WSMTYPE_REFERENCE:
            {
                _rep.refa->append(*val._rep.ref);
                break;
            }
            case WSMTYPE_INSTANCE:
            {
                _rep.insta->append(*val._rep.inst);
                break;
            }
            case WSMTYPE_OTHER:
            {
                _rep.stra->append(*val._rep.str);
                break;
            }
            default:
            {
                PEGASUS_ASSERT(0);
                break;
            }
        }
    }
    else
    {
        switch (_type)
        {
            case WSMTYPE_REFERENCE:
            {
                Array<WsmEndpointReference> ref;
                ref.append(*_rep.ref);
                ref.append(*val._rep.ref);
                set(ref);
                break;
            }
            case WSMTYPE_INSTANCE:
            {
                Array<WsmInstance> inst;
                inst.append(*_rep.inst);
                inst.append(*val._rep.inst);
                set(inst);
                break;
            }
            case WSMTYPE_OTHER:
            {
                Array<String> str;
                str.append(*_rep.str);
                str.append(*val._rep.str);
                set(str);
                break;
            }
            default:
            {
                PEGASUS_ASSERT(0);
                break;
            }
        }
    }
}


void WsmValue::toArray()
{
    if (_isArray)
        return;

    switch (_type)
    {
        case WSMTYPE_REFERENCE:
        {
            Array<WsmEndpointReference> ref;
            ref.append(*_rep.ref);
            set(ref);
            break;
        }
        case WSMTYPE_INSTANCE:
        {
            Array<WsmInstance> inst;
            inst.append(*_rep.inst);
            set(inst);
            break;
        }
        case WSMTYPE_OTHER:
        {
            Array<String> str;
            str.append(*_rep.str);
            set(str);
            break;
        }
        default:
        {
            PEGASUS_ASSERT(0);
            break;
        }
    }
}

PEGASUS_NAMESPACE_END
