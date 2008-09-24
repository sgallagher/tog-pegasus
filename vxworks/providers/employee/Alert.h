/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 1.2.4
//
//==============================================================================

#ifndef _cimple_Alert_h
#define _cimple_Alert_h

#include <cimple/cimple.h>
#include "CIM_Indication.h"

CIMPLE_NAMESPACE_BEGIN

// Alert keys:

/*[930]*/
class Alert : public Instance
{
public:
    // CIM_Indication features:
    Property<String> IndicationIdentifier;
    Property<Array_String> CorrelatedIndications;
    Property<Datetime> IndicationTime;
    Property<uint16> PerceivedSeverity;
    Property<String> OtherSeverity;

    // Alert features:
    Property<String> Msg;

    CIMPLE_CLASS(Alert)
};

CIMPLE_NAMESPACE_END

#endif /* _cimple_Alert_h */
