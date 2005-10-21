#include <Pegasus/Provider/CMPI/cmpidt.h>
#include <Pegasus/Provider/CMPI/cmpift.h>


CMPI_EXTERN_C void
PROV_LOG (const char *fmt, ...);

CMPI_EXTERN_C void
PROV_LOG_CLOSE ();

CMPI_EXTERN_C void
PROV_LOG_OPEN (const char *file, const char *location);
	
CMPI_EXTERN_C const char *
strCMPIValueState (CMPIValueState state);

CMPI_EXTERN_C const char *
strCMPIPredOp (CMPIPredOp op);
	

CMPI_EXTERN_C const char *
strCMPIStatus (CMPIStatus stat);

CMPI_EXTERN_C int _CMSameType( CMPIData value1, CMPIData value2 );
CMPI_EXTERN_C int _CMSameValue( CMPIData value1, CMPIData value2 );
CMPI_EXTERN_C int _CMSameObject( const CMPIObjectPath * object1, const CMPIObjectPath * object2 );
CMPI_EXTERN_C int _CMSameInstance( const CMPIInstance * instance1, const CMPIInstance * instance2 );
CMPI_EXTERN_C const char * _CMPIrcName ( CMPIrc rc );
CMPI_EXTERN_C const char * _CMPITypeName (CMPIType type);
CMPI_EXTERN_C char * _CMPIValueToString (CMPIData data);

CMPI_EXTERN_C void check_CMPIStatus (CMPIStatus rc);
//int _setProperty (const CMPIBroker *b,CMPIInstance * ci, const char *c,const char *p);
//
CMPI_EXTERN_C CMPIBoolean
evalute_selectcond (const CMPISelectCond * cond, CMPIAccessor *accessor, void *parm);

CMPI_EXTERN_C CMPIBoolean
evaluate(const CMPISelectExp *se, const CMPIInstance *inst,CMPIAccessor *inst_accessor, void *parm );

CMPI_EXTERN_C CMPIData
instance_accessor (const char *name, void *param);

