#include "SubscriptionFilterQueryContainer.h"
//#include <Pegasus/Query/QueryExpression/QueryExpression.h>
//********************************
PEGASUS_NAMESPACE_BEGIN
//
// SubscriptionFilterQueryContainer
//

class SubscriptionFilterQueryContainerRep
{
public:
    QueryExpression filterExpr;
};

const String SubscriptionFilterQueryContainer::NAME =
    "SubscriptionFilterQueryContainer";
    
SubscriptionFilterQueryContainer::SubscriptionFilterQueryContainer
    (const OperationContext::Container & container)
{
    const SubscriptionFilterQueryContainer * p =
        dynamic_cast<const SubscriptionFilterQueryContainer *>(&container);
    
    if(p == 0)
    {
        throw DynamicCastFailedException();
    }
    
    _rep = new SubscriptionFilterQueryContainerRep();
    _rep->filterExpr = p->_rep->filterExpr;
}   

SubscriptionFilterQueryContainer::SubscriptionFilterQueryContainer
    (const SubscriptionFilterQueryContainer & container)
{
    _rep = new SubscriptionFilterQueryContainerRep();
    _rep->filterExpr = container._rep->filterExpr;
}

SubscriptionFilterQueryContainer::SubscriptionFilterQueryContainer(
    const String & filterQuery,
    const String & queryLanguage)
{   
    _rep = new SubscriptionFilterQueryContainerRep();
    _rep->filterExpr = QueryExpression(queryLanguage,filterQuery);
}

SubscriptionFilterQueryContainer::~SubscriptionFilterQueryContainer
    (void)
{
    delete _rep;
}
SubscriptionFilterQueryContainer &
    SubscriptionFilterQueryContainer::operator=(
    const SubscriptionFilterQueryContainer & container)
{
    if (this == &container)
    {
        return (*this);
    }

    _rep->filterExpr = container._rep->filterExpr;

    return (*this);
}

String SubscriptionFilterQueryContainer::getName(void) const
{   
    return(NAME);
}

OperationContext::Container * SubscriptionFilterQueryContainer::clone(void) const
{  
    return(new SubscriptionFilterQueryContainer(_rep->filterExpr.getQuery(),
        _rep->filterExpr.getQueryLanguage()));
}

void SubscriptionFilterQueryContainer::destroy(void)
{   
    delete this;
}

QueryExpression SubscriptionFilterQueryContainer::getQueryExpression(void) const
{   
    return(_rep->filterExpr);
}

// Unimplemented, hidden constructor not intended for actual use
SubscriptionFilterQueryContainer::SubscriptionFilterQueryContainer()
{
}
PEGASUS_NAMESPACE_END
