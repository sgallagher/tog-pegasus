//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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
// Author:      Adrian Schuur, schuur@de.ibm.com
//
// Modified By: Heidi Neuman, heidineu@de.ibm.com
//              Angel Nunez Mencias, anunez@de.ibm.com
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef _CmpiDateTime_h_
#define _CmpiDateTime_h_

#include "cmpidt.h"
#include "cmpift.h"

#include "CmpiObject.h"
#include "CmpiStatus.h"
#include "CmpiBaseMI.h"
#include "Linkage.h"


/** This class represents the instance of a CIM DateTime artifact.
*/

class PEGASUS_CMPI_PROVIDER_LINKAGE CmpiDateTime : public CmpiObject {

protected:
   void *CmpiDateTime::makeDateTime(CMPIBroker *mb);
   void *CmpiDateTime::makeDateTime(CMPIBroker *mb, const char* utcTime);
   void *CmpiDateTime::makeDateTime(CMPIBroker *mb, const CMPIUint64 binTime,
                                    const CmpiBoolean interval);
private:
public:

   /** Constructor from CMPI type
   */
   inline CmpiDateTime(const CMPIDateTime* enc)
   {
      this->enc=(void*)enc;
   }


   /** Constructor - Return current time and date.
   */
   inline CmpiDateTime() {
      enc=makeDateTime(CmpiProviderBase::getBroker());
   }

   inline CmpiDateTime(const CmpiDateTime& original) : CmpiObject(0){
      enc=makeDateTime(CmpiProviderBase::getBroker(),
                       ((CmpiDateTime&)original).getDateTime(),
                       ((CmpiDateTime&)original).isInterval());
   }

   /** Constructor - Initialize time and date according to <utcTime>.
   @param utcTime Date/Time definition in UTC format
   */
   inline CmpiDateTime(const char* utcTime) {
      enc=makeDateTime(CmpiProviderBase::getBroker(),utcTime);
   }

   /** Constructor - Initialize time and date according to <binTime>.
   @param binTime Date/Time definition in binary format in microsecods
        starting since 00:00:00 GMT, Jan 1,1970.
   @param interval defines Date/Time definition to be an interval value
   */
   inline CmpiDateTime(const CMPIUint64 binTime, const CmpiBoolean interval) {
      enc=makeDateTime(CmpiProviderBase::getBroker(),binTime,interval);
   }

   /** isInterval - Tests whether time is an interval value.
   @return CmpiBoolean true when time is an interval value.
   */
   CmpiBoolean isInterval() const;


   /** getDateTime - Returns DateTime in binary format.
   @return Uint64 DateTime in binary.
   */
   CMPIUint64 getDateTime() const;

   /** Gets the encapsulated CMPIDateTime.
   */
   inline CMPIDateTime *getEnc() const
   {
      return (CMPIDateTime*)enc;
   }
};

#endif
