#include <Pegasus/Provider/CMPI/cmpidt.h>
#include <Pegasus/Provider/CMPI/cmpift.h>


void
PROV_LOG (const char *fmt, ...);

void
PROV_LOG_CLOSE ();

void
PROV_LOG_OPEN (const char *file, const char *location);
	
const char *
strCMPIValueState (CMPIValueState state);

const char *
strCMPIPredOp (CMPIPredOp op);
	

const char *
strCMPIStatus (CMPIStatus stat);

int _CMSameType( CMPIData value1, CMPIData value2 );
int _CMSameValue( CMPIData value1, CMPIData value2 );
int _CMSameObject( const CMPIObjectPath * object1, const CMPIObjectPath * object2 );
int _CMSameInstance( const CMPIInstance * instance1, const CMPIInstance * instance2 );
const char * _CMPIrcName ( CMPIrc rc );
const char * _CMPITypeName (CMPIType type);
char * _CMPIValueToString (CMPIData data);

void check_CMPIStatus (CMPIStatus rc);
//int _setProperty (const CMPIBroker *b,CMPIInstance * ci, const char *c,const char *p);
//
CMPIBoolean
evalute_selectcond (const CMPISelectCond * cond, CMPIAccessor *accessor, void *parm);

CMPIBoolean
evaluate(const CMPISelectExp *se, const CMPIInstance *inst,CMPIAccessor *inst_accessor, void *parm );

CMPIData
instance_accessor (const char *name, void *param);

