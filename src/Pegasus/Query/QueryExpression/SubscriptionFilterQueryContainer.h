#ifndef Pegasus_SubscriptionFilterQueryContainer_h
#define Pegasus_SubscriptionFilterQueryContainer_h

#include <Pegasus/Query/QueryExpression/Linkage.h>
#include "QueryExpression.h"
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/OperationContext.h>

PEGASUS_NAMESPACE_BEGIN


class SubscriptionFilterQueryContainerRep;

class PEGASUS_QUERYEXPRESSION_LINKAGE SubscriptionFilterQueryContainer
    : virtual public OperationContext::Container
{
public:
    /** REVIEWERS: Insert description here. What parameters need descriptions?
    */
    static const String NAME;

    /** REVIEWERS: Insert description here. What parameters need descriptions?
    */
    SubscriptionFilterQueryContainer
        (const OperationContext::Container & container);

    /** REVIEWERS: Insert description here. What parameters need descriptions?
    @param conainer REVIEWERS: Insert description here.
    */
    SubscriptionFilterQueryContainer
        (const SubscriptionFilterQueryContainer & container);

    /** REVIEWERS: Insert description here. What parameters need descriptions?
    @param filterCondition REVIEWERS: Insert description here.
    @param queryLanguage REVIEWERS: Insert description here.
    */
    SubscriptionFilterQueryContainer
        (const String & filterQuery,
        const String & queryLanguage);

    /** REVIEWERS: Insert description here. What parameters need descriptions?
    */
    virtual ~SubscriptionFilterQueryContainer(void);

    /** REVIEWERS: Insert description here. What parameters need descriptions?
    @param container REVIEWERS: Insert description here.
    */
    SubscriptionFilterQueryContainer & operator=
        (const SubscriptionFilterQueryContainer & container);

    /** REVIEWERS: Insert description here. 
    */
    virtual String getName(void) const;

    /** REVIEWERS: Insert description here. What parameters need descriptions?
    */
    virtual OperationContext::Container * clone(void) const;

    /** REVIEWERS: Insert description here. 
    */
    virtual void destroy(void);

    /** REVIEWERS: Insert description here. 
    */
    QueryExpression getQueryExpression(void) const;

protected:
    SubscriptionFilterQueryContainerRep* _rep;

private:
    SubscriptionFilterQueryContainer();    // Unimplemented
};

PEGASUS_NAMESPACE_END

#endif
