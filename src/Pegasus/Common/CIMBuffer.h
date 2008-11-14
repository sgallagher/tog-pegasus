#ifndef Pegasus_CIMBuffer_h
#define Pegasus_CIMBuffer_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/CIMQualifierList.h>
#include <Pegasus/Common/CIMParamValue.h>
#include <Pegasus/Common/CIMNameCast.h>

#define PEGASUS_USE_MAGIC

PEGASUS_NAMESPACE_BEGIN

/** This class serializes/deserializes CIM objects (String, CIMInstance, etc.)
    to/from a binary message stream. Serialized objects have two main
    characteristics. (1) They are aligned on suitable boundaries so the basic
    types can be accessed in place without the risk of aligment faults. (2)
    They represents strings as UCS2 characters. Performance is an overriding
    design goal of this class. The CIMBuffer class is suitable for binary
    protocols since it sacrifices size for performance; Whereas the Packer
    class is more suitable for disk storage since it favors size over 
    performance.
*/
class PEGASUS_COMMON_LINKAGE CIMBuffer
{
public:

    CIMBuffer(size_t size);

    CIMBuffer(char* data, size_t size)
    {
        _data = data;
        _ptr = _data;
        _end = data + size;
    }

    ~CIMBuffer();

    void rewind()
    {
        _ptr = _data;
    }

    size_t capacity()
    {
        return _end - _data;
    }

    size_t size()
    {
        return _ptr - _data;
    }

    const char* getData() const 
    {
        return _data;
    }

    static size_t round(size_t size)
    {
        /* Round up to nearest multiple of 8 */
        return (size + 7) & ~7;
    }

    void putBoolean(Boolean x)
    {
        if (_end - _ptr < 8)
            _grow(sizeof(x));

        *((Boolean*)_ptr) = x;
        _ptr += 8;
    }

    void putUint8(Uint8 x)
    {
        if (_end - _ptr < 8)
            _grow(sizeof(x));

        *((Uint8*)_ptr) = x;
        _ptr += 8;
    }

    void putSint8(Sint8 x)
    {
        if (_end - _ptr < 8)
            _grow(sizeof(x));

        *((Sint8*)_ptr) = x;
        _ptr += 8;
    }

    void putUint16(Uint16 x)
    {
        if (_end - _ptr < 8)
            _grow(sizeof(x));

        *((Uint16*)_ptr) = x;
        _ptr += 8;
    }

    void putSint16(Sint16 x)
    {
        if (_end - _ptr < 8)
            _grow(sizeof(x));

        *((Sint16*)_ptr) = x;
        _ptr += 8;
    }

    void putUint32(Uint32 x)
    {
        if (_end - _ptr < 8)
            _grow(sizeof(x));

        *((Uint32*)_ptr) = x;
        _ptr += 8;
    }

    void putSint32(Sint32 x)
    {
        if (_end - _ptr < 8)
            _grow(sizeof(x));

        *((Sint32*)_ptr) = x;
        _ptr += 8;
    }

    void putUint64(Uint64 x)
    {
        if (_end - _ptr < 8)
            _grow(sizeof(x));

        *((Uint64*)_ptr) = x;
        _ptr += 8;
    }

    void putSint64(Sint64 x)
    {
        if (_end - _ptr < 8)
            _grow(sizeof(x));

        *((Sint64*)_ptr) = x;
        _ptr += 8;
    }

    void putReal32(Real32 x)
    {
        if (_end - _ptr < 8)
            _grow(sizeof(x));

        *((Real32*)_ptr) = x;
        _ptr += 8;
    }

    void putReal64(Real64 x)
    {
        if (_end - _ptr < 8)
            _grow(sizeof(x));

        *((Real64*)_ptr) = x;
        _ptr += 8;
    }

    void putChar16(Char16 x)
    {
        if (_end - _ptr < 8)
            _grow(sizeof(x));

        *((Char16*)_ptr) = x;
        _ptr += 8;
    }

    void putBytes(const void* data, size_t size)
    {
        size_t r = round(size);

        if (_end - _ptr < ptrdiff_t(r))
            _grow(r);

        memcpy(_ptr, data, size);
        _ptr += r;
    }

    void putString(const String& x)
    {
        Uint32 n = x.size();
        putUint32(n);
        putBytes(x.getChar16Data(), n * sizeof(Char16));
    }

    void putDateTime(const CIMDateTime& x)
    {
        putUint64(x.toMicroSeconds());
        putBoolean(x.isInterval());
    }

    void putBooleanA(const Array<Boolean>& x)
    {
        Uint32 n = x.size();
        putUint32(n);
        putBytes(x.getData(), n * sizeof(Boolean));
    }

    void putUint8A(const Array<Uint8>& x)
    {
        Uint32 n = x.size();
        putUint32(n);
        putBytes(x.getData(), n * sizeof(Uint8));
    }

    void putSint8A(const Array<Sint8>& x)
    {
        Uint32 n = x.size();
        putUint32(n);
        putBytes(x.getData(), n * sizeof(Sint8));
    }

    void putUint16A(const Array<Uint16>& x)
    {
        Uint32 n = x.size();
        putUint32(n);
        putBytes(x.getData(), n * sizeof(Uint16));
    }

    void putSint16A(const Array<Sint16>& x)
    {
        Uint32 n = x.size();
        putUint32(n);
        putBytes(x.getData(), n * sizeof(Sint16));
    }

    void putUint32A(const Array<Uint32>& x)
    {
        Uint32 n = x.size();
        putUint32(n);
        putBytes(x.getData(), n * sizeof(Uint32));
    }

    void putSint32A(const Array<Sint32>& x)
    {
        Uint32 n = x.size();
        putUint32(n);
        putBytes(x.getData(), n * sizeof(Sint32));
    }

    void putUint64A(const Array<Uint64>& x)
    {
        Uint32 n = x.size();
        putUint32(n);
        putBytes(x.getData(), n * sizeof(Uint64));
    }

    void putSint64A(const Array<Sint64>& x)
    {
        Uint32 n = x.size();
        putUint32(n);
        putBytes(x.getData(), n * sizeof(Sint64));
    }

    void putReal32A(const Array<Real32>& x)
    {
        Uint32 n = x.size();
        putUint32(n);
        putBytes(x.getData(), n * sizeof(Real32));
    }

    void putReal64A(const Array<Real64>& x)
    {
        Uint32 n = x.size();
        putUint32(n);
        putBytes(x.getData(), n * sizeof(Real64));
    }

    void putChar16A(const Array<Char16>& x)
    {
        Uint32 n = x.size();
        putUint32(n);
        putBytes(x.getData(), n * sizeof(Char16));
    }

    void putStringA(const Array<String>& x)
    {
        Uint32 n = x.size();
        putUint32(n);

        for (size_t i = 0; i < n; i++)
            putString(x[i]);
    }

    void putDateTimeA(const Array<CIMDateTime>& x)
    {
        Uint32 n = x.size();
        putUint32(n);

        for (size_t i = 0; i < n; i++)
            putDateTime(x[i]);
    }

    bool getBoolean(Boolean& x)
    {
        if (_end - _ptr < 8)
            return false;

        x = *((Boolean*)_ptr);
        _ptr += 8;
        return true;
    }

    bool getUint8(Uint8& x)
    {
        if (_end - _ptr < 8)
            return false;

        x = *((Uint8*)_ptr);
        _ptr += 8;
        return true;
    }

    bool getSint8(Sint8& x)
    {
        if (_end - _ptr < 8)
            return false;

        x = *((Sint8*)_ptr);
        _ptr += 8;
        return true;
    }

    bool getUint16(Uint16& x)
    {
        if (_end - _ptr < 8)
            return false;

        x = *((Uint16*)_ptr);
        _ptr += 8;
        return true;
    }

    bool getSint16(Sint16& x)
    {
        if (_end - _ptr < 8)
            return false;

        x = *((Sint16*)_ptr);
        _ptr += 8;
        return true;
    }

    bool getUint32(Uint32& x)
    {
        if (_end - _ptr < 8)
            return false;

        x = *((Uint32*)_ptr);
        _ptr += 8;
        return true;
    }

    bool getSint32(Sint32& x)
    {
        if (_end - _ptr < 8)
            return false;

        x = *((Sint32*)_ptr);
        _ptr += 8;
        return true;
    }

    bool getUint64(Uint64& x)
    {
        if (_end - _ptr < 8)
            return false;

        x = *((Uint64*)_ptr);
        _ptr += 8;
        return true;
    }

    bool getSint64(Sint64& x)
    {
        if (_end - _ptr < 8)
            return false;

        x = *((Sint64*)_ptr);
        _ptr += 8;
        return true;
    }

    bool getReal32(Real32& x)
    {
        if (_end - _ptr < 8)
            return false;

        x = *((Real32*)_ptr);
        _ptr += 8;
        return true;
    }

    bool getReal64(Real64& x)
    {
        if (_end - _ptr < 8)
            return false;

        x = *((Real64*)_ptr);
        _ptr += 8;
        return true;
    }

    bool getChar16(Char16& x)
    {
        if (_end - _ptr < 8)
            return false;

        x = *((Char16*)_ptr);
        _ptr += 8;
        return true;
    }

    bool getString(String& x);

    bool getDateTime(CIMDateTime& x)
    {
        Uint64 usec;

        if (!getUint64(usec))
            return false;

        Boolean interval;

        if (!getBoolean(interval))
            return false;

        x = CIMDateTime(usec, interval);
        return true;
    }

    bool getBooleanA(Array<Boolean>& x)
    {
        Uint32 n;

        if (!getUint32(n))
            return false;

        size_t r = round(n * sizeof(Boolean));

        if (_end - _ptr < ptrdiff_t(r))
            return false;

        x.append((const Boolean*)_ptr, n);
        _ptr += r;
        return true;
    }

    bool getUint8A(Array<Uint8>& x)
    {
        Uint32 n;

        if (!getUint32(n))
            return false;

        size_t r = round(n * sizeof(Uint8));

        if (_end - _ptr < ptrdiff_t(r))
            return false;

        x.append((const Uint8*)_ptr, n);
        _ptr += r;
        return true;
    }

    bool getSint8A(Array<Sint8>& x)
    {
        Uint32 n;

        if (!getUint32(n))
            return false;

        size_t r = round(n * sizeof(Sint8));

        if (_end - _ptr < ptrdiff_t(r))
            return false;

        x.append((const Sint8*)_ptr, n);
        _ptr += r;
        return true;
    }

    bool getUint16A(Array<Uint16>& x)
    {
        Uint32 n;

        if (!getUint32(n))
            return false;

        size_t r = round(n * sizeof(Uint16));

        if (_end - _ptr < ptrdiff_t(r))
            return false;

        x.append((const Uint16*)_ptr, n);
        _ptr += r;
        return true;
    }

    bool getSint16A(Array<Sint16>& x)
    {
        Uint32 n;

        if (!getUint32(n))
            return false;

        size_t r = round(n * sizeof(Sint16));

        if (_end - _ptr < ptrdiff_t(r))
            return false;

        x.append((const Sint16*)_ptr, n);
        _ptr += r;
        return true;
    }

    bool getUint32A(Array<Uint32>& x)
    {
        Uint32 n;

        if (!getUint32(n))
            return false;

        size_t r = round(n * sizeof(Uint32));

        if (_end - _ptr < ptrdiff_t(r))
            return false;

        x.append((const Uint32*)_ptr, n);
        _ptr += r;
        return true;
    }

    bool getSint32A(Array<Sint32>& x)
    {
        Uint32 n;

        if (!getUint32(n))
            return false;

        size_t r = round(n * sizeof(Sint32));

        if (_end - _ptr < ptrdiff_t(r))
            return false;

        x.append((const Sint32*)_ptr, n);
        _ptr += r;
        return true;
    }

    bool getUint64A(Array<Uint64>& x)
    {
        Uint32 n;

        if (!getUint32(n))
            return false;

        size_t r = round(n * sizeof(Uint64));

        if (_end - _ptr < ptrdiff_t(r))
            return false;

        x.append((const Uint64*)_ptr, n);
        _ptr += r;
        return true;
    }

    bool getSint64A(Array<Sint64>& x)
    {
        Uint32 n;

        if (!getUint32(n))
            return false;

        size_t r = round(n * sizeof(Sint64));

        if (_end - _ptr < ptrdiff_t(r))
            return false;

        x.append((const Sint64*)_ptr, n);
        _ptr += r;
        return true;
    }

    bool getReal32A(Array<Real32>& x)
    {
        Uint32 n;

        if (!getUint32(n))
            return false;

        size_t r = round(n * sizeof(Real32));

        if (_end - _ptr < ptrdiff_t(r))
            return false;

        x.append((const Real32*)_ptr, n);
        _ptr += r;
        return true;
    }

    bool getReal64A(Array<Real64>& x)
    {
        Uint32 n;

        if (!getUint32(n))
            return false;

        size_t r = round(n * sizeof(Real64));

        if (_end - _ptr < ptrdiff_t(r))
            return false;

        x.append((const Real64*)_ptr, n);
        _ptr += r;
        return true;
    }

    bool getChar16A(Array<Char16>& x)
    {
        Uint32 n;

        if (!getUint32(n))
            return false;

        size_t r = round(n * sizeof(Char16));

        if (_end - _ptr < ptrdiff_t(r))
            return false;

        x.append((const Char16*)_ptr, n);
        _ptr += r;
        return true;
    }

    bool getStringA(Array<String>& x)
    {
        Uint32 n;

        if (!getUint32(n))
            return false;

        for (Uint32 i = 0; i < n; i++)
        {
            String tmp;

            if (!getString(tmp))
                return false;

            x.append(tmp);
        }

        return true;
    }

    bool getDateTimeA(Array<CIMDateTime>& x)
    {
        Uint32 n;

        if (!getUint32(n))
            return false;

        for (Uint32 i = 0; i < n; i++)
        {
            CIMDateTime tmp;

            if (!getDateTime(tmp))
                return false;

            x.append(tmp);
        }

        return true;
    }

    void putValue(const CIMValue& x);

    bool getValue(CIMValue& x);

    void putKeyBinding(const CIMKeyBinding& x);

    bool getKeyBinding(CIMKeyBinding& x);

    void putObjectPath(const CIMObjectPath& x);

    bool getObjectPath(CIMObjectPath& x);

    void putQualifier(const CIMQualifier& x);

    bool getQualifier(CIMQualifier& x);

    void putQualifierList(const CIMQualifierList& x);

    bool getQualifierList(CIMQualifierList& x);

    void putProperty(const CIMProperty& x);

    bool getProperty(CIMProperty& x);

    void putInstance(const CIMInstance& x);

    bool getInstance(CIMInstance& x);

    void putClass(const CIMClass& x);

    bool getClass(CIMClass& x);

    void putParameter(const CIMParameter& x);

    bool getParameter(CIMParameter& x);

    void putMethod(const CIMMethod& x);

    bool getMethod(CIMMethod& x);

    void putPropertyList(const CIMPropertyList& x);

    bool getPropertyList(CIMPropertyList& x);

    void putObject(const CIMObject& x);

    bool getObject(CIMObject& x);

    void putParamValue(const CIMParamValue& x);

    bool getParamValue(CIMParamValue& x);

    void putName(const CIMName& x)
    {
        putString(x.getString());
    }

    void putNamespaceName(const CIMNamespaceName& x)
    {
        putString(x.getString());
    }

    bool getName(CIMName& x)
    {
        String tmp;

        if (!getString(tmp))
            return false;

        x = CIMNameCast(tmp);
        return true;
    }

    bool getNamespaceName(CIMNamespaceName& x)
    {
        String tmp;

        if (!getString(tmp))
            return false;

        x = CIMNamespaceNameCast(tmp);
        return true;
    }

    void putNameA(const Array<CIMName>& x)
    {
        Uint32 n = x.size();
        putUint32(n);

        for (size_t i = 0; i < n; i++)
            putName(x[i]);
    }

    bool getNameA(Array<CIMName>& x)
    {
        Uint32 n;

        if (!getUint32(n))
            return false;

        for (Uint32 i = 0; i < n; i++)
        {
            String tmp;

            if (!getString(tmp))
                return false;

            x.append(CIMNameCast(tmp));
        }

        return true;
    }

    void putObjectPathA(const Array<CIMObjectPath>& x)
    {
        Uint32 n = x.size();
        putUint32(n);

        for (size_t i = 0; i < n; i++)
            putObjectPath(x[i]);
    }

    bool getObjectPathA(Array<CIMObjectPath>& x)
    {
        Uint32 n;

        if (!getUint32(n))
            return false;

        for (Uint32 i = 0; i < n; i++)
        {
            CIMObjectPath tmp;

            if (!getObjectPath(tmp))
                return false;

            x.append(tmp);
        }

        return true;
    }

    void putInstanceA(const Array<CIMInstance>& x)
    {
        Uint32 n = x.size();
        putUint32(n);

        for (size_t i = 0; i < n; i++)
            putInstance(x[i]);
    }

    bool getInstanceA(Array<CIMInstance>& x)
    {
        Uint32 n;

        if (!getUint32(n))
            return false;

        for (Uint32 i = 0; i < n; i++)
        {
            CIMInstance tmp;

            if (!getInstance(tmp))
                return false;

            x.append(tmp);
        }

        return true;
    }

    void putObjectA(const Array<CIMObject>& x)
    {
        Uint32 n = x.size();
        putUint32(n);

        for (size_t i = 0; i < n; i++)
            putObject(x[i]);
    }

    bool getObjectA(Array<CIMObject>& x)
    {
        Uint32 n;

        if (!getUint32(n))
            return false;

        for (Uint32 i = 0; i < n; i++)
        {
            CIMObject tmp;

            if (!getObject(tmp))
                return false;

            x.append(tmp);
        }

        return true;
    }

    void putParamValueA(const Array<CIMParamValue>& x)
    {
        Uint32 n = x.size();
        putUint32(n);

        for (size_t i = 0; i < n; i++)
            putParamValue(x[i]);
    }

    bool getParamValueA(Array<CIMParamValue>& x)
    {
        Uint32 n;

        if (!getUint32(n))
            return false;

        for (Uint32 i = 0; i < n; i++)
        {
            CIMParamValue tmp;

            if (!getParamValue(tmp))
                return false;

            x.append(tmp);
        }

        return true;
    }

    void putPresent(Boolean flag);

    bool getPresent(Boolean& flag);

private:

    void _grow(size_t size);

    void _putMagic(Uint32 magic)
    {
#if defined(PEGASUS_USE_MAGIC)
        putUint32(magic);
#endif
    }

    bool _testMagic(Uint32 magic)
    {
#if defined(PEGASUS_USE_MAGIC)
        Uint32 tmp;

        if (!getUint32(tmp))
            return false;

        return tmp == magic;
#else
        return true;
#endif
    }

    char* _data;
    char* _end;
    char* _ptr;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_CIMBuffer_h */
