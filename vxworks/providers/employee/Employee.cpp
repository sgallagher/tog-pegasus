/*NOCHKSRC*/
//==============================================================================
//
// PLEASE DO NOT EDIT; THIS FILE WAS AUTOMATICALLY GENERATED BY GENCLASS 1.2.4
//
//==============================================================================

#include <cimple/Meta_Class.h>
#include <cimple/Meta_Property.h>
#include <cimple/Meta_Reference.h>
#include "Employee.h"

CIMPLE_NAMESPACE_BEGIN

using namespace cimple;

extern const Meta_Repository __meta_repository_C6E4565B48EC1112A92E2691F2CFFCA2;

/*[1304]*/
extern const Meta_Property
_Employee_Id;

/*[1327]*/
const Meta_Property
_Employee_Id =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_KEY|CIMPLE_FLAG_READ,
    "Id",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT32,
    0, /* subscript */
    CIMPLE_OFF(Employee,Id),
    0, /* value */
};

/*[1304]*/
extern const Meta_Property
_Employee_First;

/*[1327]*/
const Meta_Property
_Employee_First =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "First",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    STRING,
    0, /* subscript */
    CIMPLE_OFF(Employee,First),
    0, /* value */
};

/*[1304]*/
extern const Meta_Property
_Employee_Last;

/*[1327]*/
const Meta_Property
_Employee_Last =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "Last",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    STRING,
    0, /* subscript */
    CIMPLE_OFF(Employee,Last),
    0, /* value */
};

/*[1304]*/
extern const Meta_Property
_Employee_Gender;

/*[1327]*/
const Meta_Property
_Employee_Gender =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "Gender",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT32,
    0, /* subscript */
    CIMPLE_OFF(Employee,Gender),
    0, /* value */
};

/*[1304]*/
extern const Meta_Property
_Employee_Active;

/*[276]*/
static const Meta_Value_Scalar<boolean>
_Employee_Active_MV =
{
    true
};

/*[1327]*/
const Meta_Property
_Employee_Active =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "Active",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    BOOLEAN,
    0, /* subscript */
    CIMPLE_OFF(Employee,Active),
    (const Meta_Value*)(void*)&_Employee_Active_MV,
};

/*[1304]*/
extern const Meta_Property
_Employee_OutOfOffice;

/*[1327]*/
const Meta_Property
_Employee_OutOfOffice =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_READ,
    "OutOfOffice",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    BOOLEAN,
    0, /* subscript */
    CIMPLE_OFF(Employee,OutOfOffice),
    0, /* value */
};

/*[1518]*/
static const Meta_Property
_Employee_SetOutOfOfficeState_OutOfOfficeState =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_IN,
    "OutOfOfficeState",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    BOOLEAN,
    0,
    CIMPLE_OFF(Employee_SetOutOfOfficeState_method,OutOfOfficeState),
    0, /* value */
};

/*[1518]*/
static const Meta_Property
_Employee_SetOutOfOfficeState_PreviousOutOfOfficeState =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_OUT,
    "PreviousOutOfOfficeState",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    BOOLEAN,
    0,
    CIMPLE_OFF(Employee_SetOutOfOfficeState_method,PreviousOutOfOfficeState),
    0, /* value */
};

/*[1671]*/
static const Meta_Property
_Employee_SetOutOfOfficeState_return_value =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_OUT,
    "return_value",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT32,
    0,
    CIMPLE_OFF(Employee_SetOutOfOfficeState_method,return_value),
    0, /* value */
};

/*[1647]*/
static Meta_Feature* _Employee_SetOutOfOfficeState_MFA[] =
{
    (Meta_Feature*)(void*)&_Employee_SetOutOfOfficeState_OutOfOfficeState,
    (Meta_Feature*)(void*)&_Employee_SetOutOfOfficeState_PreviousOutOfOfficeState,
    (Meta_Feature*)(void*)&_Employee_SetOutOfOfficeState_return_value
};

/*[1849]*/
const Meta_Method
Employee_SetOutOfOfficeState_method::static_meta_class =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_METHOD,
    "SetOutOfOfficeState",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    _Employee_SetOutOfOfficeState_MFA,
    CIMPLE_ARRAY_SIZE(_Employee_SetOutOfOfficeState_MFA),
    sizeof(Employee_SetOutOfOfficeState_method),
    UINT32,
};

/*[1671]*/
static const Meta_Property
_Employee_GetEmployeeCount_return_value =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_PROPERTY|CIMPLE_FLAG_OUT,
    "return_value",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    UINT32,
    0,
    CIMPLE_OFF(Employee_GetEmployeeCount_method,return_value),
    0, /* value */
};

/*[1647]*/
static Meta_Feature* _Employee_GetEmployeeCount_MFA[] =
{
    (Meta_Feature*)(void*)&_Employee_GetEmployeeCount_return_value
};

/*[1849]*/
const Meta_Method
Employee_GetEmployeeCount_method::static_meta_class =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_METHOD|CIMPLE_FLAG_STATIC,
    "GetEmployeeCount",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    _Employee_GetEmployeeCount_MFA,
    CIMPLE_ARRAY_SIZE(_Employee_GetEmployeeCount_MFA),
    sizeof(Employee_GetEmployeeCount_method),
    UINT32,
};

/*[2027]*/
static Meta_Feature* _Employee_MFA[] =
{
    (Meta_Feature*)(void*)&_Employee_Id,
    (Meta_Feature*)(void*)&_Employee_First,
    (Meta_Feature*)(void*)&_Employee_Last,
    (Meta_Feature*)(void*)&_Employee_Gender,
    (Meta_Feature*)(void*)&_Employee_Active,
    (Meta_Feature*)(void*)&_Employee_OutOfOffice,
    (Meta_Feature*)(void*)&Employee_SetOutOfOfficeState_method::static_meta_class,
    (Meta_Feature*)(void*)&Employee_GetEmployeeCount_method::static_meta_class,
};

/*[2074]*/
static const Meta_Feature_Local _locals[] =
{
    {1},
    {1},
    {1},
    {1},
    {1},
    {1},
    {1},
    {1},
};

/*[2094]*/
const Meta_Class Employee::static_meta_class =
{
    CIMPLE_ATOMIC_INITIALIZER, /* refs */
    CIMPLE_FLAG_CLASS,
    "Employee",
    0, /* meta_qualifiers */
    0, /* num_meta_qaulifiers */
    _Employee_MFA,
    CIMPLE_ARRAY_SIZE(_Employee_MFA),
    sizeof(Employee),
    _locals,
    0, /* super_class */ 
    1, /* num_keys */
    &__meta_repository_C6E4565B48EC1112A92E2691F2CFFCA2,
};

CIMPLE_NAMESPACE_END

