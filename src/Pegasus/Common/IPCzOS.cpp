//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
// Author: Marek Szermutzky (mszermutzky@de.ibm.com)
//         Robert Kieninger (kieningr@de.ibm.com)
//
//%/////////////////////////////////////////////////////////////////////////////

PEGASUS_NAMESPACE_BEGIN

//-----------------------------------------------------------------
/// Native z/OS inline implementation of AtomicInt class
//-----------------------------------------------------------------
AtomicInt::AtomicInt()
{
   _rep = 0;
}

AtomicInt::AtomicInt( Uint32 initial)
{
   _rep = (cs_t)initial;
}

AtomicInt::~AtomicInt() {}

AtomicInt::AtomicInt(const AtomicInt& original)
{
   _rep = original._rep;
}

AtomicInt& AtomicInt::operator=(Uint32 i)
{
   cs_t x = (cs_t)i;
   cs_t old = _rep;
   while ( cs( &old, &_rep, x) )
   {
      old = _rep;
   }

   return *this;
}

AtomicInt& AtomicInt::operator=(const AtomicInt& original)
{
   if(this != &original)
   {
      cs_t x = original._rep;
      cs_t old = _rep;
      while ( cs( &old, &_rep, x) )
      {
         old = _rep;
      }
   }

   return *this;
}

Uint32 AtomicInt::value(void) const
{
   return (Uint32)_rep;
}

void AtomicInt::operator++(void)
{
   Uint32 x = (Uint32)_rep;
   Uint32 old = x;
   x++;
   while ( cs( (cs_t*)&old, &_rep, (cs_t)x) )
   {
      x = (Uint32)_rep;
      old = x;
      x++;
   }
}

void AtomicInt::operator--(void)
{
   Uint32 x = (Uint32)_rep;
   Uint32 old = x;
   x--;
   while ( cs( (cs_t*)&old, &_rep, (cs_t) x) )
   {
      x = (Uint32)_rep;
      old = x;
      x--;
   }
}

void AtomicInt::operator++(int)
{
   Uint32 x = (Uint32)_rep;
   Uint32 old = x;
   x++;
   while ( cs( (cs_t*)&old, &_rep, (cs_t) x) )
   {
      x = (Uint32)_rep;
      old = x;
      x++;
   }
}

void AtomicInt::operator--(int)
{
   Uint32 x = (Uint32)_rep;
   Uint32 old = x;
   x--;
   while ( cs( (cs_t*)&old, &_rep, (cs_t) x) )
   {
      x = (Uint32)_rep;
      old = x;
      x--;
   }
}


Uint32 AtomicInt::operator+(const AtomicInt& val)
{
   Uint32 x = (Uint32)_rep + (Uint32)val._rep;

   return x;
}

Uint32 AtomicInt::operator+(Uint32 val)
{
   Uint32 x = (Uint32)_rep + val;

   return x;
}

Uint32 AtomicInt::operator-(const AtomicInt& val)
{
   Uint32 x = (Uint32)_rep - (Uint32)val._rep;

   return x;
}

Uint32 AtomicInt::operator-(Uint32 val)
{
   Uint32 x = (Uint32)_rep - val;

   return x;
}

AtomicInt& AtomicInt::operator+=(const AtomicInt& val)
{
   cs_t old = _rep;
   Uint32 x = (Uint32)old + (Uint32)val._rep;
   while ( cs( &old, &_rep, (cs_t)x) )
   {
      old = _rep;
      x = (Uint32)old + (Uint32)val._rep;
   }

   return *this;
}

AtomicInt& AtomicInt::operator+=(Uint32 val)
{
   cs_t old = _rep;
   Uint32 x = (Uint32)old + val;
   while ( cs( &old, &_rep, (cs_t)x) )
   {
      old = _rep;
      x = (Uint32)old + val;
   }

   return *this;
}

AtomicInt& AtomicInt::operator-=(const AtomicInt& val)
{
   cs_t old = _rep;
   Uint32 x = (Uint32)old - (Uint32)val._rep;
   while ( cs( &old, &_rep, (cs_t)x) )
   {
      old = _rep;
      x = (Uint32)old - (Uint32)val._rep;
   }

   return *this;
}

AtomicInt& AtomicInt::operator-=(Uint32 val)
{
   cs_t old = _rep;
   Uint32 x = (Uint32)old - val;
   while ( cs( &old, &_rep, (cs_t)x) )
   {
      old = _rep;
      x = (Uint32)old - val;
   }

   return *this;
}

Boolean AtomicInt::DecAndTestIfZero()
{
   Uint32 x = (Uint32)_rep;
   Uint32 old = x;
   x--;
   while ( cs( (cs_t*)&old, &_rep, (cs_t) x) )
   {
      x = (Uint32)_rep;
      old = x;
      x--;
   }
   return x==0;
}

PEGASUS_NAMESPACE_END
