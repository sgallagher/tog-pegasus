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
// Author:      Mark Hamzy, hamzy@us.ibm.com
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef PEGASUS_DEBUG

// Retail build

#define DEBUG_ConvertJavaToC(j,c,p)   (c)p
#define DEBUG_ConvertCToJava(c,j,p)   (j)p
#define DEBUG_ConvertCleanup(c,p)

#else

// Debug build

#define CONVERT_NONE      1
#define CONVERT_BIG_PAD   0
#define CONVERT_SMALL_PAD 0

#if CONVERT_NONE

#define DEBUG_ConvertJavaToC(j,c,p)   (c)p
#define DEBUG_ConvertCToJava(c,j,p)   (j)p
#define DEBUG_ConvertCleanup(c,p)

#elif CONVERT_BIG_PAD

#define DEBUG_ConvertJavaToC(j,c,p)   TemplateConvertJavaToC <j,c> (p)
#define DEBUG_ConvertCToJava(c,j,p)   TemplateConvertCToJava <c,j> (p)
#define DEBUG_ConvertCleanup(j,p)     TemplateConvertCleanup <j>   (p)

#define CONVERT_SIGNATURE_BEGIN 0x12345678
#define CONVERT_SIGNATURE_END   0xFEDCBA98
#define CONVERT_SIZE            (4 * sizeof (void *))

#define ConvertEndianSwap(n)    ((((n) & 0xff000000) >> 24) | \
                                 (((n) & 0x00ff0000) >> 8)  | \
                                 (((n) & 0x0000ff00) << 8)  | \
                                 (((n) & 0x000000ff) << 24) )

template <typename J, typename C>
C TemplateConvertJavaToC (J p)
{
   unsigned int *pi    = (unsigned int *)p;
   unsigned int *ptrap = 0;

   if (pi[0] != CONVERT_SIGNATURE_BEGIN)
   {
      PEGASUS_STD(cout)<<"TemplateConvertJavaToC "<<PEGASUS_STD(hex)<<(void*)p<<PEGASUS_STD(dec)<<PEGASUS_STD(endl);
      *ptrap = 0;
   }
   if (pi[1] != ConvertEndianSwap ((unsigned int)pi[2]))
   {
      *ptrap = 0;
   }
   if (pi[3] != CONVERT_SIGNATURE_END)
   {
      PEGASUS_STD(cout)<<"TemplateConvertJavaToC "<<PEGASUS_STD(hex)<<(void*)p<<PEGASUS_STD(dec)<<PEGASUS_STD(endl);
      *ptrap = 0;
   }

   PEGASUS_STD(cout)<<"TemplateConvertJavaToC "<<PEGASUS_STD(hex)<<(void*)p<<" -> "<<(void*)pi[1]<<PEGASUS_STD(dec)<<PEGASUS_STD(endl);

   return (C)pi[1];
}

template <typename C, typename J>
J TemplateConvertCToJava (C p)
{
   unsigned int *pi    = (unsigned int *)std::malloc (CONVERT_SIZE);
   unsigned int *ptrap = 0;

   if (pi)
   {
      pi[0] = CONVERT_SIGNATURE_BEGIN;
      pi[1] = (unsigned int)p;
      pi[2] = ConvertEndianSwap ((unsigned int)p);
      pi[3] = CONVERT_SIGNATURE_END;
   }
   else
   {
      PEGASUS_STD(cout)<<"TemplateConvertCToJava"<<PEGASUS_STD(hex)<<(void*)p<<PEGASUS_STD(dec)<<PEGASUS_STD(endl);
      *ptrap = 0;
   }

   PEGASUS_STD(cout)<<"TemplateConvertCToJava "<<PEGASUS_STD(hex)<<(void*)p<<" -> "<<(void*)pi<<PEGASUS_STD(dec)<<PEGASUS_STD(endl);

   return (J)pi;
}

template <typename J>
void TemplateConvertCleanup (J p)
{
   unsigned int *pi    = (unsigned int *)p;
   unsigned int *ptrap = 0;

   PEGASUS_STD(cout)<<"TemplateConvertCleanup "<<PEGASUS_STD(hex)<<(void*)p<<PEGASUS_STD(dec)<<PEGASUS_STD(endl);

   if (pi[0] != CONVERT_SIGNATURE_BEGIN)
   {
      *ptrap = 0;
   }
   if (pi[1] != ConvertEndianSwap ((unsigned int)pi[2]))
   {
      *ptrap = 0;
   }
   if (pi[3] != CONVERT_SIGNATURE_END)
   {
      *ptrap = 0;
   }

   std::memset (pi, 0, CONVERT_SIZE);
   std::free ((void *)pi);
}

#elif CONVERT_SMALL_PAD

#define DEBUG_ConvertJavaToC(j,c,p)   TemplateConvertJavaToC <j,c> (p)
#define DEBUG_ConvertCToJava(c,j,p)   TemplateConvertCToJava <c,j> (p)
#define DEBUG_ConvertCleanup(j,p)     TemplateConvertCleanup <j>   (p)

#define CONVERT_SIGNATURE_BEGIN 0xFE
#define CONVERT_SIGNATURE_END   0x12
#define CONVERT_SIZE            (sizeof (unsigned char) + sizeof (void *) + sizeof (unsigned char))

template <typename J, typename C>
C TemplateConvertJavaToC (J p)
{
   unsigned char *puch  = (unsigned char *)p;
   int           *ptrap = 0;

   if (puch[0] != CONVERT_SIGNATURE_BEGIN)
   {
      PEGASUS_STD(cout)<<"TemplateConvertJavaToC "<<PEGASUS_STD(hex)<<(void*)p<<PEGASUS_STD(dec)<<PEGASUS_STD(endl);
      *ptrap = 0;
   }
   if (puch[1 + sizeof (int)] != CONVERT_SIGNATURE_END)
   {
      PEGASUS_STD(cout)<<"TemplateConvertJavaToC "<<PEGASUS_STD(hex)<<(void*)p<<PEGASUS_STD(dec)<<PEGASUS_STD(endl);
      *ptrap = 0;
   }

   PEGASUS_STD(cout)<<"TemplateConvertJavaToC "<<PEGASUS_STD(hex)<<(void*)p<<" -> "<<(void*)(*((int *)(puch + 1)))<<PEGASUS_STD(dec)<<PEGASUS_STD(endl);

   return (C)(void *)(*((int *)(puch + 1)));
}

template <typename C, typename J>
J TemplateConvertCToJava (C p)
{
   unsigned char *puch  = (unsigned char *)std::malloc (CONVERT_SIZE);
   int           *ptrap = 0;

   if (puch)
   {
      puch[0]                = CONVERT_SIGNATURE_BEGIN;
      *(int *)(puch + 1)     = (int)p;
      puch[1 + sizeof (int)] = CONVERT_SIGNATURE_END;
   }
   else
   {
      PEGASUS_STD(cout)<<"TemplateConvertCToJava"<<PEGASUS_STD(hex)<<(void*)p<<PEGASUS_STD(dec)<<PEGASUS_STD(endl);
      *ptrap = 0;
   }

   PEGASUS_STD(cout)<<"TemplateConvertCToJava "<<PEGASUS_STD(hex)<<(void*)p<<" -> "<<(void *)puch<<PEGASUS_STD(dec)<<PEGASUS_STD(endl);

   return (J)puch;
}

template <typename J>
void TemplateConvertCleanup (J p)
{
   unsigned char *puch  = (unsigned char *)p;
   int           *ptrap = 0;

   PEGASUS_STD(cout)<<"TemplateConvertCleanup "<<PEGASUS_STD(hex)<<(void*)p<<PEGASUS_STD(dec)<<PEGASUS_STD(endl);

   if (puch[0] != CONVERT_SIGNATURE_BEGIN)
   {
      *ptrap = 0;
   }
   if (puch[1 + sizeof (int)] != CONVERT_SIGNATURE_END)
   {
      *ptrap = 0;
   }

   std::memset (puch, 0, CONVERT_SIZE);
   std::free ((void *)puch);
}

#else

#error "Unsupported conversion case in src/Pegasus/ProviderManager2/JMPI/Convert.h"

#endif

#endif
