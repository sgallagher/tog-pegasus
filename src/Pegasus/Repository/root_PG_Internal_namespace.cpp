#include "root_PG_Internal_namespace.h"

/*NOCHKSRC*/

PEGASUS_NAMESPACE_BEGIN

//==============================================================================
//
// Qualifiers
//
//==============================================================================

static SourceQualifierDecl
_ASSOCIATION_qualifier_decl =
{
    /* name */
    "ASSOCIATION",
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* scope */
    PEGASUS_SCOPE_ASSOCIATION,
    /* flavor */
    PEGASUS_FLAVOR_TOSUBCLASS|PEGASUS_FLAVOR_DISABLEOVERRIDE,
    /* value */
    "\000",
};

static SourceQualifierDecl
_INDICATION_qualifier_decl =
{
    /* name */
    "INDICATION",
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* scope */
    PEGASUS_SCOPE_CLASS|PEGASUS_SCOPE_INDICATION,
    /* flavor */
    PEGASUS_FLAVOR_TOSUBCLASS|PEGASUS_FLAVOR_DISABLEOVERRIDE,
    /* value */
    "\000",
};

static SourceQualifierDecl
_Abstract_qualifier_decl =
{
    /* name */
    "Abstract",
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* scope */
    PEGASUS_SCOPE_CLASS|PEGASUS_SCOPE_ASSOCIATION|PEGASUS_SCOPE_INDICATION,
    /* flavor */
    PEGASUS_FLAVOR_OVERRIDABLE|PEGASUS_FLAVOR_RESTRICTED,
    /* value */
    "\000",
};

static SourceQualifierDecl
_Aggregate_qualifier_decl =
{
    /* name */
    "Aggregate",
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* scope */
    PEGASUS_SCOPE_REFERENCE,
    /* flavor */
    PEGASUS_FLAVOR_TOSUBCLASS|PEGASUS_FLAVOR_DISABLEOVERRIDE,
    /* value */
    "\000",
};

static SourceQualifierDecl
_Aggregation_qualifier_decl =
{
    /* name */
    "Aggregation",
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* scope */
    PEGASUS_SCOPE_ASSOCIATION,
    /* flavor */
    PEGASUS_FLAVOR_TOSUBCLASS|PEGASUS_FLAVOR_DISABLEOVERRIDE,
    /* value */
    "\000",
};

static SourceQualifierDecl
_ArrayType_qualifier_decl =
{
    /* name */
    "ArrayType",
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* scope */
    PEGASUS_SCOPE_PROPERTY|PEGASUS_SCOPE_PARAMETER,
    /* flavor */
    PEGASUS_FLAVOR_TOSUBCLASS|PEGASUS_FLAVOR_DISABLEOVERRIDE,
    /* value */
    "Bag",
};

static SourceQualifierDecl
_BitMap_qualifier_decl =
{
    /* name */
    "BitMap",
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* scope */
    PEGASUS_SCOPE_PROPERTY|PEGASUS_SCOPE_METHOD|PEGASUS_SCOPE_PARAMETER,
    /* flavor */
    PEGASUS_FLAVOR_OVERRIDABLE|PEGASUS_FLAVOR_TOSUBCLASS,
    /* value */
    0,
};

static SourceQualifierDecl
_BitValues_qualifier_decl =
{
    /* name */
    "BitValues",
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* scope */
    PEGASUS_SCOPE_PROPERTY|PEGASUS_SCOPE_METHOD|PEGASUS_SCOPE_PARAMETER,
    /* flavor */
    PEGASUS_FLAVOR_OVERRIDABLE|PEGASUS_FLAVOR_TOSUBCLASS|PEGASUS_FLAVOR_TRANSLATABLE,
    /* value */
    0,
};

static SourceQualifierDecl
_ClassConstraint_qualifier_decl =
{
    /* name */
    "ClassConstraint",
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* scope */
    PEGASUS_SCOPE_CLASS|PEGASUS_SCOPE_ASSOCIATION|PEGASUS_SCOPE_INDICATION,
    /* flavor */
    PEGASUS_FLAVOR_OVERRIDABLE|PEGASUS_FLAVOR_TOSUBCLASS,
    /* value */
    0,
};

static SourceQualifierDecl
_Composition_qualifier_decl =
{
    /* name */
    "Composition",
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* scope */
    PEGASUS_SCOPE_ASSOCIATION,
    /* flavor */
    PEGASUS_FLAVOR_TOSUBCLASS|PEGASUS_FLAVOR_DISABLEOVERRIDE,
    /* value */
    "\000",
};

static SourceQualifierDecl
_Counter_qualifier_decl =
{
    /* name */
    "Counter",
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* scope */
    PEGASUS_SCOPE_PROPERTY|PEGASUS_SCOPE_METHOD|PEGASUS_SCOPE_PARAMETER,
    /* flavor */
    PEGASUS_FLAVOR_OVERRIDABLE|PEGASUS_FLAVOR_TOSUBCLASS,
    /* value */
    "\000",
};

static SourceQualifierDecl
_Deprecated_qualifier_decl =
{
    /* name */
    "Deprecated",
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* scope */
    PEGASUS_SCOPE_ANY,
    /* flavor */
    PEGASUS_FLAVOR_OVERRIDABLE|PEGASUS_FLAVOR_RESTRICTED,
    /* value */
    0,
};

static SourceQualifierDecl
_Description_qualifier_decl =
{
    /* name */
    "Description",
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* scope */
    PEGASUS_SCOPE_ANY,
    /* flavor */
    PEGASUS_FLAVOR_OVERRIDABLE|PEGASUS_FLAVOR_TOSUBCLASS|PEGASUS_FLAVOR_TRANSLATABLE,
    /* value */
    0,
};

static SourceQualifierDecl
_DisplayName_qualifier_decl =
{
    /* name */
    "DisplayName",
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* scope */
    PEGASUS_SCOPE_ANY,
    /* flavor */
    PEGASUS_FLAVOR_OVERRIDABLE|PEGASUS_FLAVOR_TOSUBCLASS|PEGASUS_FLAVOR_TRANSLATABLE,
    /* value */
    0,
};

static SourceQualifierDecl
_DN_qualifier_decl =
{
    /* name */
    "DN",
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* scope */
    PEGASUS_SCOPE_PROPERTY|PEGASUS_SCOPE_METHOD|PEGASUS_SCOPE_PARAMETER,
    /* flavor */
    PEGASUS_FLAVOR_TOSUBCLASS|PEGASUS_FLAVOR_DISABLEOVERRIDE,
    /* value */
    "\000",
};

static SourceQualifierDecl
_EmbeddedInstance_qualifier_decl =
{
    /* name */
    "EmbeddedInstance",
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* scope */
    PEGASUS_SCOPE_PROPERTY|PEGASUS_SCOPE_METHOD|PEGASUS_SCOPE_PARAMETER,
    /* flavor */
    PEGASUS_FLAVOR_TOSUBCLASS|PEGASUS_FLAVOR_DISABLEOVERRIDE,
    /* value */
    0,
};

static SourceQualifierDecl
_EmbeddedObject_qualifier_decl =
{
    /* name */
    "EmbeddedObject",
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* scope */
    PEGASUS_SCOPE_PROPERTY|PEGASUS_SCOPE_METHOD|PEGASUS_SCOPE_PARAMETER,
    /* flavor */
    PEGASUS_FLAVOR_TOSUBCLASS|PEGASUS_FLAVOR_DISABLEOVERRIDE,
    /* value */
    "\000",
};

static SourceQualifierDecl
_Exception_qualifier_decl =
{
    /* name */
    "Exception",
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* scope */
    PEGASUS_SCOPE_CLASS|PEGASUS_SCOPE_INDICATION,
    /* flavor */
    PEGASUS_FLAVOR_TOSUBCLASS|PEGASUS_FLAVOR_TOINSTANCE|PEGASUS_FLAVOR_DISABLEOVERRIDE,
    /* value */
    "\000",
};

static SourceQualifierDecl
_Experimental_qualifier_decl =
{
    /* name */
    "Experimental",
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* scope */
    PEGASUS_SCOPE_ANY,
    /* flavor */
    PEGASUS_FLAVOR_OVERRIDABLE|PEGASUS_FLAVOR_RESTRICTED,
    /* value */
    "\000",
};

static SourceQualifierDecl
_Gauge_qualifier_decl =
{
    /* name */
    "Gauge",
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* scope */
    PEGASUS_SCOPE_PROPERTY|PEGASUS_SCOPE_METHOD|PEGASUS_SCOPE_PARAMETER,
    /* flavor */
    PEGASUS_FLAVOR_OVERRIDABLE|PEGASUS_FLAVOR_TOSUBCLASS,
    /* value */
    "\000",
};

static SourceQualifierDecl
_In_qualifier_decl =
{
    /* name */
    "In",
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* scope */
    PEGASUS_SCOPE_PARAMETER,
    /* flavor */
    PEGASUS_FLAVOR_TOSUBCLASS|PEGASUS_FLAVOR_DISABLEOVERRIDE,
    /* value */
    "\001",
};

static SourceQualifierDecl
_Key_qualifier_decl =
{
    /* name */
    "Key",
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* scope */
    PEGASUS_SCOPE_PROPERTY|PEGASUS_SCOPE_REFERENCE,
    /* flavor */
    PEGASUS_FLAVOR_TOSUBCLASS|PEGASUS_FLAVOR_DISABLEOVERRIDE,
    /* value */
    "\000",
};

static SourceQualifierDecl
_MappingStrings_qualifier_decl =
{
    /* name */
    "MappingStrings",
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* scope */
    PEGASUS_SCOPE_ANY,
    /* flavor */
    PEGASUS_FLAVOR_OVERRIDABLE|PEGASUS_FLAVOR_TOSUBCLASS,
    /* value */
    0,
};

static SourceQualifierDecl
_Max_qualifier_decl =
{
    /* name */
    "Max",
    /* type */
    CIMTYPE_UINT32,
    /* subscript */
    -1,
    /* scope */
    PEGASUS_SCOPE_REFERENCE,
    /* flavor */
    PEGASUS_FLAVOR_OVERRIDABLE|PEGASUS_FLAVOR_TOSUBCLASS,
    /* value */
    0,
};

static SourceQualifierDecl
_MaxLen_qualifier_decl =
{
    /* name */
    "MaxLen",
    /* type */
    CIMTYPE_UINT32,
    /* subscript */
    -1,
    /* scope */
    PEGASUS_SCOPE_PROPERTY|PEGASUS_SCOPE_METHOD|PEGASUS_SCOPE_PARAMETER,
    /* flavor */
    PEGASUS_FLAVOR_OVERRIDABLE|PEGASUS_FLAVOR_TOSUBCLASS,
    /* value */
    0,
};

static SourceQualifierDecl
_MaxValue_qualifier_decl =
{
    /* name */
    "MaxValue",
    /* type */
    CIMTYPE_SINT64,
    /* subscript */
    -1,
    /* scope */
    PEGASUS_SCOPE_PROPERTY|PEGASUS_SCOPE_METHOD|PEGASUS_SCOPE_PARAMETER,
    /* flavor */
    PEGASUS_FLAVOR_OVERRIDABLE|PEGASUS_FLAVOR_TOSUBCLASS,
    /* value */
    0,
};

static SourceQualifierDecl
_MethodConstraint_qualifier_decl =
{
    /* name */
    "MethodConstraint",
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* scope */
    PEGASUS_SCOPE_METHOD,
    /* flavor */
    PEGASUS_FLAVOR_OVERRIDABLE|PEGASUS_FLAVOR_TOSUBCLASS,
    /* value */
    0,
};

static SourceQualifierDecl
_Min_qualifier_decl =
{
    /* name */
    "Min",
    /* type */
    CIMTYPE_UINT32,
    /* subscript */
    -1,
    /* scope */
    PEGASUS_SCOPE_REFERENCE,
    /* flavor */
    PEGASUS_FLAVOR_OVERRIDABLE|PEGASUS_FLAVOR_TOSUBCLASS,
    /* value */
    "\000\000\000\000",
};

static SourceQualifierDecl
_MinLen_qualifier_decl =
{
    /* name */
    "MinLen",
    /* type */
    CIMTYPE_UINT32,
    /* subscript */
    -1,
    /* scope */
    PEGASUS_SCOPE_PROPERTY|PEGASUS_SCOPE_METHOD|PEGASUS_SCOPE_PARAMETER,
    /* flavor */
    PEGASUS_FLAVOR_OVERRIDABLE|PEGASUS_FLAVOR_TOSUBCLASS,
    /* value */
    "\000\000\000\000",
};

static SourceQualifierDecl
_MinValue_qualifier_decl =
{
    /* name */
    "MinValue",
    /* type */
    CIMTYPE_SINT64,
    /* subscript */
    -1,
    /* scope */
    PEGASUS_SCOPE_PROPERTY|PEGASUS_SCOPE_METHOD|PEGASUS_SCOPE_PARAMETER,
    /* flavor */
    PEGASUS_FLAVOR_OVERRIDABLE|PEGASUS_FLAVOR_TOSUBCLASS,
    /* value */
    0,
};

static SourceQualifierDecl
_ModelCorrespondence_qualifier_decl =
{
    /* name */
    "ModelCorrespondence",
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* scope */
    PEGASUS_SCOPE_ANY,
    /* flavor */
    PEGASUS_FLAVOR_OVERRIDABLE|PEGASUS_FLAVOR_TOSUBCLASS,
    /* value */
    0,
};

static SourceQualifierDecl
_Nonlocal_qualifier_decl =
{
    /* name */
    "Nonlocal",
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* scope */
    PEGASUS_SCOPE_REFERENCE,
    /* flavor */
    PEGASUS_FLAVOR_OVERRIDABLE|PEGASUS_FLAVOR_TOSUBCLASS,
    /* value */
    0,
};

static SourceQualifierDecl
_NonlocalType_qualifier_decl =
{
    /* name */
    "NonlocalType",
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* scope */
    PEGASUS_SCOPE_REFERENCE,
    /* flavor */
    PEGASUS_FLAVOR_OVERRIDABLE|PEGASUS_FLAVOR_TOSUBCLASS,
    /* value */
    0,
};

static SourceQualifierDecl
_NullValue_qualifier_decl =
{
    /* name */
    "NullValue",
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* scope */
    PEGASUS_SCOPE_PROPERTY,
    /* flavor */
    PEGASUS_FLAVOR_TOSUBCLASS|PEGASUS_FLAVOR_DISABLEOVERRIDE,
    /* value */
    0,
};

static SourceQualifierDecl
_Octetstring_qualifier_decl =
{
    /* name */
    "Octetstring",
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* scope */
    PEGASUS_SCOPE_PROPERTY|PEGASUS_SCOPE_METHOD|PEGASUS_SCOPE_PARAMETER,
    /* flavor */
    PEGASUS_FLAVOR_TOSUBCLASS|PEGASUS_FLAVOR_DISABLEOVERRIDE,
    /* value */
    "\000",
};

static SourceQualifierDecl
_Out_qualifier_decl =
{
    /* name */
    "Out",
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* scope */
    PEGASUS_SCOPE_PARAMETER,
    /* flavor */
    PEGASUS_FLAVOR_TOSUBCLASS|PEGASUS_FLAVOR_DISABLEOVERRIDE,
    /* value */
    "\000",
};

static SourceQualifierDecl
_Override_qualifier_decl =
{
    /* name */
    "Override",
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* scope */
    PEGASUS_SCOPE_PROPERTY|PEGASUS_SCOPE_REFERENCE|PEGASUS_SCOPE_METHOD,
    /* flavor */
    PEGASUS_FLAVOR_OVERRIDABLE|PEGASUS_FLAVOR_RESTRICTED,
    /* value */
    0,
};

static SourceQualifierDecl
_Propagated_qualifier_decl =
{
    /* name */
    "Propagated",
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* scope */
    PEGASUS_SCOPE_PROPERTY,
    /* flavor */
    PEGASUS_FLAVOR_TOSUBCLASS|PEGASUS_FLAVOR_DISABLEOVERRIDE,
    /* value */
    0,
};

static SourceQualifierDecl
_PropertyConstraint_qualifier_decl =
{
    /* name */
    "PropertyConstraint",
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* scope */
    PEGASUS_SCOPE_PROPERTY|PEGASUS_SCOPE_REFERENCE,
    /* flavor */
    PEGASUS_FLAVOR_OVERRIDABLE|PEGASUS_FLAVOR_TOSUBCLASS,
    /* value */
    0,
};

static SourceQualifierDecl
_Read_qualifier_decl =
{
    /* name */
    "Read",
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* scope */
    PEGASUS_SCOPE_PROPERTY,
    /* flavor */
    PEGASUS_FLAVOR_OVERRIDABLE|PEGASUS_FLAVOR_TOSUBCLASS,
    /* value */
    "\001",
};

static SourceQualifierDecl
_Required_qualifier_decl =
{
    /* name */
    "Required",
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* scope */
    PEGASUS_SCOPE_PROPERTY|PEGASUS_SCOPE_REFERENCE|PEGASUS_SCOPE_METHOD|PEGASUS_SCOPE_PARAMETER,
    /* flavor */
    PEGASUS_FLAVOR_TOSUBCLASS|PEGASUS_FLAVOR_DISABLEOVERRIDE,
    /* value */
    "\000",
};

static SourceQualifierDecl
_Revision_qualifier_decl =
{
    /* name */
    "Revision",
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* scope */
    PEGASUS_SCOPE_CLASS|PEGASUS_SCOPE_ASSOCIATION|PEGASUS_SCOPE_INDICATION,
    /* flavor */
    PEGASUS_FLAVOR_OVERRIDABLE|PEGASUS_FLAVOR_TOSUBCLASS|PEGASUS_FLAVOR_TRANSLATABLE,
    /* value */
    0,
};

static SourceQualifierDecl
_CLASS_qualifier_decl =
{
    /* name */
    "CLASS",
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* scope */
    PEGASUS_SCOPE_PROPERTY|PEGASUS_SCOPE_METHOD,
    /* flavor */
    PEGASUS_FLAVOR_TOSUBCLASS|PEGASUS_FLAVOR_TRANSLATABLE|PEGASUS_FLAVOR_DISABLEOVERRIDE,
    /* value */
    0,
};

static SourceQualifierDecl
_Source_qualifier_decl =
{
    /* name */
    "Source",
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* scope */
    PEGASUS_SCOPE_CLASS|PEGASUS_SCOPE_ASSOCIATION|PEGASUS_SCOPE_INDICATION,
    /* flavor */
    PEGASUS_FLAVOR_OVERRIDABLE|PEGASUS_FLAVOR_TOSUBCLASS,
    /* value */
    0,
};

static SourceQualifierDecl
_SourceType_qualifier_decl =
{
    /* name */
    "SourceType",
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* scope */
    PEGASUS_SCOPE_CLASS|PEGASUS_SCOPE_ASSOCIATION|PEGASUS_SCOPE_INDICATION|PEGASUS_SCOPE_REFERENCE,
    /* flavor */
    PEGASUS_FLAVOR_OVERRIDABLE|PEGASUS_FLAVOR_TOSUBCLASS,
    /* value */
    0,
};

static SourceQualifierDecl
_Static_qualifier_decl =
{
    /* name */
    "Static",
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* scope */
    PEGASUS_SCOPE_PROPERTY|PEGASUS_SCOPE_METHOD,
    /* flavor */
    PEGASUS_FLAVOR_TOSUBCLASS|PEGASUS_FLAVOR_DISABLEOVERRIDE,
    /* value */
    "\000",
};

static SourceQualifierDecl
_Terminal_qualifier_decl =
{
    /* name */
    "Terminal",
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* scope */
    PEGASUS_SCOPE_CLASS|PEGASUS_SCOPE_ASSOCIATION|PEGASUS_SCOPE_INDICATION,
    /* flavor */
    PEGASUS_FLAVOR_OVERRIDABLE|PEGASUS_FLAVOR_TOSUBCLASS,
    /* value */
    "\000",
};

static SourceQualifierDecl
_UMLPackagePath_qualifier_decl =
{
    /* name */
    "UMLPackagePath",
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* scope */
    PEGASUS_SCOPE_CLASS|PEGASUS_SCOPE_ASSOCIATION|PEGASUS_SCOPE_INDICATION,
    /* flavor */
    PEGASUS_FLAVOR_OVERRIDABLE|PEGASUS_FLAVOR_TOSUBCLASS,
    /* value */
    0,
};

static SourceQualifierDecl
_Units_qualifier_decl =
{
    /* name */
    "Units",
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* scope */
    PEGASUS_SCOPE_PROPERTY|PEGASUS_SCOPE_METHOD|PEGASUS_SCOPE_PARAMETER,
    /* flavor */
    PEGASUS_FLAVOR_OVERRIDABLE|PEGASUS_FLAVOR_TOSUBCLASS|PEGASUS_FLAVOR_TRANSLATABLE,
    /* value */
    0,
};

static SourceQualifierDecl
_ValueMap_qualifier_decl =
{
    /* name */
    "ValueMap",
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* scope */
    PEGASUS_SCOPE_PROPERTY|PEGASUS_SCOPE_METHOD|PEGASUS_SCOPE_PARAMETER,
    /* flavor */
    PEGASUS_FLAVOR_OVERRIDABLE|PEGASUS_FLAVOR_TOSUBCLASS,
    /* value */
    0,
};

static SourceQualifierDecl
_Values_qualifier_decl =
{
    /* name */
    "Values",
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* scope */
    PEGASUS_SCOPE_PROPERTY|PEGASUS_SCOPE_METHOD|PEGASUS_SCOPE_PARAMETER,
    /* flavor */
    PEGASUS_FLAVOR_OVERRIDABLE|PEGASUS_FLAVOR_TOSUBCLASS|PEGASUS_FLAVOR_TRANSLATABLE,
    /* value */
    0,
};

static SourceQualifierDecl
_Version_qualifier_decl =
{
    /* name */
    "Version",
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* scope */
    PEGASUS_SCOPE_CLASS|PEGASUS_SCOPE_ASSOCIATION|PEGASUS_SCOPE_INDICATION,
    /* flavor */
    PEGASUS_FLAVOR_OVERRIDABLE|PEGASUS_FLAVOR_TRANSLATABLE|PEGASUS_FLAVOR_RESTRICTED,
    /* value */
    0,
};

static SourceQualifierDecl
_Weak_qualifier_decl =
{
    /* name */
    "Weak",
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* scope */
    PEGASUS_SCOPE_REFERENCE,
    /* flavor */
    PEGASUS_FLAVOR_TOSUBCLASS|PEGASUS_FLAVOR_TOINSTANCE|PEGASUS_FLAVOR_DISABLEOVERRIDE,
    /* value */
    "\000",
};

static SourceQualifierDecl
_Write_qualifier_decl =
{
    /* name */
    "Write",
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* scope */
    PEGASUS_SCOPE_PROPERTY,
    /* flavor */
    PEGASUS_FLAVOR_OVERRIDABLE|PEGASUS_FLAVOR_TOSUBCLASS,
    /* value */
    "\000",
};

static SourceQualifierDecl
_Alias_qualifier_decl =
{
    /* name */
    "Alias",
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* scope */
    PEGASUS_SCOPE_PROPERTY|PEGASUS_SCOPE_REFERENCE|PEGASUS_SCOPE_METHOD,
    /* flavor */
    PEGASUS_FLAVOR_OVERRIDABLE|PEGASUS_FLAVOR_TOSUBCLASS|PEGASUS_FLAVOR_TRANSLATABLE,
    /* value */
    0,
};

static SourceQualifierDecl
_Delete_qualifier_decl =
{
    /* name */
    "Delete",
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* scope */
    PEGASUS_SCOPE_ASSOCIATION|PEGASUS_SCOPE_REFERENCE,
    /* flavor */
    PEGASUS_FLAVOR_OVERRIDABLE|PEGASUS_FLAVOR_TOSUBCLASS,
    /* value */
    "\000",
};

static SourceQualifierDecl
_Expensive_qualifier_decl =
{
    /* name */
    "Expensive",
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* scope */
    PEGASUS_SCOPE_ANY,
    /* flavor */
    PEGASUS_FLAVOR_OVERRIDABLE|PEGASUS_FLAVOR_TOSUBCLASS,
    /* value */
    "\000",
};

static SourceQualifierDecl
_Ifdeleted_qualifier_decl =
{
    /* name */
    "Ifdeleted",
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* scope */
    PEGASUS_SCOPE_ASSOCIATION|PEGASUS_SCOPE_REFERENCE,
    /* flavor */
    PEGASUS_FLAVOR_OVERRIDABLE|PEGASUS_FLAVOR_TOSUBCLASS,
    /* value */
    "\000",
};

static SourceQualifierDecl
_Invisible_qualifier_decl =
{
    /* name */
    "Invisible",
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* scope */
    PEGASUS_SCOPE_CLASS|PEGASUS_SCOPE_ASSOCIATION|PEGASUS_SCOPE_PROPERTY|PEGASUS_SCOPE_REFERENCE|PEGASUS_SCOPE_METHOD,
    /* flavor */
    PEGASUS_FLAVOR_OVERRIDABLE|PEGASUS_FLAVOR_TOSUBCLASS,
    /* value */
    "\000",
};

static SourceQualifierDecl
_Large_qualifier_decl =
{
    /* name */
    "Large",
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* scope */
    PEGASUS_SCOPE_CLASS|PEGASUS_SCOPE_PROPERTY,
    /* flavor */
    PEGASUS_FLAVOR_OVERRIDABLE|PEGASUS_FLAVOR_TOSUBCLASS,
    /* value */
    "\000",
};

static SourceQualifierDecl
_PropertyUsage_qualifier_decl =
{
    /* name */
    "PropertyUsage",
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* scope */
    PEGASUS_SCOPE_PROPERTY,
    /* flavor */
    PEGASUS_FLAVOR_OVERRIDABLE|PEGASUS_FLAVOR_TOSUBCLASS,
    /* value */
    "CurrentContext",
};

static SourceQualifierDecl
_Provider_qualifier_decl =
{
    /* name */
    "Provider",
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* scope */
    PEGASUS_SCOPE_ANY,
    /* flavor */
    PEGASUS_FLAVOR_OVERRIDABLE|PEGASUS_FLAVOR_TOSUBCLASS,
    /* value */
    0,
};

static SourceQualifierDecl
_SyntaxType_qualifier_decl =
{
    /* name */
    "SyntaxType",
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* scope */
    PEGASUS_SCOPE_PROPERTY|PEGASUS_SCOPE_REFERENCE|PEGASUS_SCOPE_METHOD|PEGASUS_SCOPE_PARAMETER,
    /* flavor */
    PEGASUS_FLAVOR_OVERRIDABLE|PEGASUS_FLAVOR_TOSUBCLASS,
    /* value */
    0,
};

static SourceQualifierDecl
_Syntax_qualifier_decl =
{
    /* name */
    "Syntax",
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* scope */
    PEGASUS_SCOPE_PROPERTY|PEGASUS_SCOPE_REFERENCE|PEGASUS_SCOPE_METHOD|PEGASUS_SCOPE_PARAMETER,
    /* flavor */
    PEGASUS_FLAVOR_OVERRIDABLE|PEGASUS_FLAVOR_TOSUBCLASS,
    /* value */
    0,
};

static SourceQualifierDecl
_TriggerType_qualifier_decl =
{
    /* name */
    "TriggerType",
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* scope */
    PEGASUS_SCOPE_CLASS|PEGASUS_SCOPE_ASSOCIATION|PEGASUS_SCOPE_INDICATION|PEGASUS_SCOPE_PROPERTY|PEGASUS_SCOPE_REFERENCE|PEGASUS_SCOPE_METHOD,
    /* flavor */
    PEGASUS_FLAVOR_OVERRIDABLE|PEGASUS_FLAVOR_TOSUBCLASS,
    /* value */
    0,
};

static SourceQualifierDecl
_UnknownValues_qualifier_decl =
{
    /* name */
    "UnknownValues",
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* scope */
    PEGASUS_SCOPE_PROPERTY,
    /* flavor */
    PEGASUS_FLAVOR_TOSUBCLASS|PEGASUS_FLAVOR_DISABLEOVERRIDE,
    /* value */
    0,
};

static SourceQualifierDecl
_UnsupportedValues_qualifier_decl =
{
    /* name */
    "UnsupportedValues",
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* scope */
    PEGASUS_SCOPE_PROPERTY,
    /* flavor */
    PEGASUS_FLAVOR_TOSUBCLASS|PEGASUS_FLAVOR_DISABLEOVERRIDE,
    /* value */
    0,
};

//==============================================================================
//
// Forward class declarations
//
//==============================================================================

extern SourceClass __root_PG_Internal_PG_Authorization;
extern SourceClass __root_PG_Internal_PG_ConfigSetting;
extern SourceClass __root_PG_Internal_PG_User;
extern SourceClass __root_PG_Internal_PG_ShutdownService;
extern SourceClass __root_PG_Internal_PG_SSLCertificate;
extern SourceClass __root_PG_Internal_PG_SSLCertificateRevocationList;

//==============================================================================
//
// Class: PG_Authorization
//
//==============================================================================

static const char*
_PG_Authorization_Username_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static SourceProperty
_PG_Authorization_Username =
{
    /* flags */
    PEGASUS_FLAG_PROPERTY|PEGASUS_FLAG_KEY|PEGASUS_FLAG_READ,
    /* name */
    "Username",
    /* qualifiers */
    _PG_Authorization_Username_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_PG_Authorization_Namespace_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static SourceProperty
_PG_Authorization_Namespace =
{
    /* flags */
    PEGASUS_FLAG_PROPERTY|PEGASUS_FLAG_KEY|PEGASUS_FLAG_READ,
    /* name */
    "Namespace",
    /* qualifiers */
    _PG_Authorization_Namespace_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_PG_Authorization_Authorization_qualifiers[] =
{
    0,
};

static SourceProperty
_PG_Authorization_Authorization =
{
    /* flags */
    PEGASUS_FLAG_PROPERTY|PEGASUS_FLAG_READ,
    /* name */
    "Authorization",
    /* qualifiers */
    _PG_Authorization_Authorization_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static SourceFeature*
_PG_Authorization_features[] =
{
    (SourceFeature*)&_PG_Authorization_Username,
    (SourceFeature*)&_PG_Authorization_Namespace,
    (SourceFeature*)&_PG_Authorization_Authorization,
    0,
};

static const char*
__root_PG_Internal_PG_Authorization_qualifiers[] =
{
    /* Version */
    "\0632.3.0",
    0,
};

SourceClass
__root_PG_Internal_PG_Authorization =
{
    /* flags */
    PEGASUS_FLAG_CLASS,
    /* name */
    "PG_Authorization",
    /* qualifiers */
    __root_PG_Internal_PG_Authorization_qualifiers,
    /* super */
    0,
    /* features */
    _PG_Authorization_features,
};

//==============================================================================
//
// Class: PG_ConfigSetting
//
//==============================================================================

static const char*
_PG_ConfigSetting_PropertyName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static SourceProperty
_PG_ConfigSetting_PropertyName =
{
    /* flags */
    PEGASUS_FLAG_PROPERTY|PEGASUS_FLAG_KEY|PEGASUS_FLAG_READ,
    /* name */
    "PropertyName",
    /* qualifiers */
    _PG_ConfigSetting_PropertyName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_PG_ConfigSetting_DefaultValue_qualifiers[] =
{
    0,
};

static SourceProperty
_PG_ConfigSetting_DefaultValue =
{
    /* flags */
    PEGASUS_FLAG_PROPERTY|PEGASUS_FLAG_READ,
    /* name */
    "DefaultValue",
    /* qualifiers */
    _PG_ConfigSetting_DefaultValue_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_PG_ConfigSetting_CurrentValue_qualifiers[] =
{
    0,
};

static SourceProperty
_PG_ConfigSetting_CurrentValue =
{
    /* flags */
    PEGASUS_FLAG_PROPERTY|PEGASUS_FLAG_READ,
    /* name */
    "CurrentValue",
    /* qualifiers */
    _PG_ConfigSetting_CurrentValue_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_PG_ConfigSetting_PlannedValue_qualifiers[] =
{
    0,
};

static SourceProperty
_PG_ConfigSetting_PlannedValue =
{
    /* flags */
    PEGASUS_FLAG_PROPERTY|PEGASUS_FLAG_READ,
    /* name */
    "PlannedValue",
    /* qualifiers */
    _PG_ConfigSetting_PlannedValue_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_PG_ConfigSetting_DynamicProperty_qualifiers[] =
{
    0,
};

static SourceProperty
_PG_ConfigSetting_DynamicProperty =
{
    /* flags */
    PEGASUS_FLAG_PROPERTY|PEGASUS_FLAG_READ,
    /* name */
    "DynamicProperty",
    /* qualifiers */
    _PG_ConfigSetting_DynamicProperty_qualifiers,
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* value */
    0,
};

static SourceFeature*
_PG_ConfigSetting_features[] =
{
    (SourceFeature*)&_PG_ConfigSetting_PropertyName,
    (SourceFeature*)&_PG_ConfigSetting_DefaultValue,
    (SourceFeature*)&_PG_ConfigSetting_CurrentValue,
    (SourceFeature*)&_PG_ConfigSetting_PlannedValue,
    (SourceFeature*)&_PG_ConfigSetting_DynamicProperty,
    0,
};

static const char*
__root_PG_Internal_PG_ConfigSetting_qualifiers[] =
{
    /* Version */
    "\0632.3.0",
    0,
};

SourceClass
__root_PG_Internal_PG_ConfigSetting =
{
    /* flags */
    PEGASUS_FLAG_CLASS,
    /* name */
    "PG_ConfigSetting",
    /* qualifiers */
    __root_PG_Internal_PG_ConfigSetting_qualifiers,
    /* super */
    0,
    /* features */
    _PG_ConfigSetting_features,
};

//==============================================================================
//
// Class: PG_User
//
//==============================================================================

static const char*
_PG_User_Username_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static SourceProperty
_PG_User_Username =
{
    /* flags */
    PEGASUS_FLAG_PROPERTY|PEGASUS_FLAG_KEY|PEGASUS_FLAG_READ,
    /* name */
    "Username",
    /* qualifiers */
    _PG_User_Username_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_PG_User_Password_qualifiers[] =
{
    0,
};

static SourceProperty
_PG_User_Password =
{
    /* flags */
    PEGASUS_FLAG_PROPERTY|PEGASUS_FLAG_READ,
    /* name */
    "Password",
    /* qualifiers */
    _PG_User_Password_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_PG_User_modifyPassword_oldPassword_qualifiers[] =
{
    /* IN */
    "\024\001",
    0,
};

static SourceProperty
_PG_User_modifyPassword_oldPassword =
{
    /* flags */
    PEGASUS_FLAG_PROPERTY|PEGASUS_FLAG_IN,
    /* name */
    "oldPassword",
    /* qualifiers */
    _PG_User_modifyPassword_oldPassword_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_PG_User_modifyPassword_newPassword_qualifiers[] =
{
    /* IN */
    "\024\001",
    0,
};

static SourceProperty
_PG_User_modifyPassword_newPassword =
{
    /* flags */
    PEGASUS_FLAG_PROPERTY|PEGASUS_FLAG_IN,
    /* name */
    "newPassword",
    /* qualifiers */
    _PG_User_modifyPassword_newPassword_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static SourceFeature*
_PG_User_modifyPassword_parameters[] =
{
    (SourceFeature*)&_PG_User_modifyPassword_oldPassword,
    (SourceFeature*)&_PG_User_modifyPassword_newPassword,
    0,
};

static const char*
_PG_User_modifyPassword_qualifiers[] =
{
    0,
};

static SourceMethod
_PG_User_modifyPassword =
{
    /* flags */
    PEGASUS_FLAG_METHOD,
    /* name */
    "PG_User",
    /* qualifiers */
    _PG_User_modifyPassword_qualifiers,
    /* type */
    CIMTYPE_UINT32,
    /* parameters */
    _PG_User_modifyPassword_parameters,
};

static SourceFeature*
_PG_User_features[] =
{
    (SourceFeature*)&_PG_User_Username,
    (SourceFeature*)&_PG_User_Password,
    (SourceFeature*)&_PG_User_modifyPassword,
    0,
};

static const char*
__root_PG_Internal_PG_User_qualifiers[] =
{
    /* Version */
    "\0632.3.0",
    0,
};

SourceClass
__root_PG_Internal_PG_User =
{
    /* flags */
    PEGASUS_FLAG_CLASS,
    /* name */
    "PG_User",
    /* qualifiers */
    __root_PG_Internal_PG_User_qualifiers,
    /* super */
    0,
    /* features */
    _PG_User_features,
};

//==============================================================================
//
// Class: PG_ShutdownService
//
//==============================================================================

static const char*
_PG_ShutdownService_shutdown_force_qualifiers[] =
{
    /* IN */
    "\024\001",
    0,
};

static SourceProperty
_PG_ShutdownService_shutdown_force =
{
    /* flags */
    PEGASUS_FLAG_PROPERTY|PEGASUS_FLAG_IN,
    /* name */
    "force",
    /* qualifiers */
    _PG_ShutdownService_shutdown_force_qualifiers,
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_PG_ShutdownService_shutdown_timeout_qualifiers[] =
{
    /* IN */
    "\024\001",
    0,
};

static SourceProperty
_PG_ShutdownService_shutdown_timeout =
{
    /* flags */
    PEGASUS_FLAG_PROPERTY|PEGASUS_FLAG_IN,
    /* name */
    "timeout",
    /* qualifiers */
    _PG_ShutdownService_shutdown_timeout_qualifiers,
    /* type */
    CIMTYPE_UINT32,
    /* subscript */
    -1,
    /* value */
    0,
};

static SourceFeature*
_PG_ShutdownService_shutdown_parameters[] =
{
    (SourceFeature*)&_PG_ShutdownService_shutdown_force,
    (SourceFeature*)&_PG_ShutdownService_shutdown_timeout,
    0,
};

static const char*
_PG_ShutdownService_shutdown_qualifiers[] =
{
    /* Static */
    "\055\001",
    0,
};

static SourceMethod
_PG_ShutdownService_shutdown =
{
    /* flags */
    PEGASUS_FLAG_METHOD|PEGASUS_FLAG_STATIC,
    /* name */
    "PG_ShutdownService",
    /* qualifiers */
    _PG_ShutdownService_shutdown_qualifiers,
    /* type */
    CIMTYPE_UINT32,
    /* parameters */
    _PG_ShutdownService_shutdown_parameters,
};

static SourceFeature*
_PG_ShutdownService_features[] =
{
    (SourceFeature*)&_PG_ShutdownService_shutdown,
    0,
};

static const char*
__root_PG_Internal_PG_ShutdownService_qualifiers[] =
{
    /* Version */
    "\0632.7.0",
    0,
};

SourceClass
__root_PG_Internal_PG_ShutdownService =
{
    /* flags */
    PEGASUS_FLAG_CLASS,
    /* name */
    "PG_ShutdownService",
    /* qualifiers */
    __root_PG_Internal_PG_ShutdownService_qualifiers,
    /* super */
    0,
    /* features */
    _PG_ShutdownService_features,
};

//==============================================================================
//
// Class: PG_SSLCertificate
//
//==============================================================================

static const char*
_PG_SSLCertificate_IssuerName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static SourceProperty
_PG_SSLCertificate_IssuerName =
{
    /* flags */
    PEGASUS_FLAG_PROPERTY|PEGASUS_FLAG_KEY|PEGASUS_FLAG_READ,
    /* name */
    "IssuerName",
    /* qualifiers */
    _PG_SSLCertificate_IssuerName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_PG_SSLCertificate_SerialNumber_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static SourceProperty
_PG_SSLCertificate_SerialNumber =
{
    /* flags */
    PEGASUS_FLAG_PROPERTY|PEGASUS_FLAG_KEY|PEGASUS_FLAG_READ,
    /* name */
    "SerialNumber",
    /* qualifiers */
    _PG_SSLCertificate_SerialNumber_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_PG_SSLCertificate_TruststoreType_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* ValueMap */
    "\061\000\0050\0001\0002\0003\0004\000",
    /* Values */
    "\062\000\005Other\000Unknown\000Server Truststore\000ExportTruststore\000Client Truststore\000",
    0,
};

static SourceProperty
_PG_SSLCertificate_TruststoreType =
{
    /* flags */
    PEGASUS_FLAG_PROPERTY|PEGASUS_FLAG_KEY|PEGASUS_FLAG_READ,
    /* name */
    "TruststoreType",
    /* qualifiers */
    _PG_SSLCertificate_TruststoreType_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_PG_SSLCertificate_SubjectName_qualifiers[] =
{
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static SourceProperty
_PG_SSLCertificate_SubjectName =
{
    /* flags */
    PEGASUS_FLAG_PROPERTY|PEGASUS_FLAG_READ,
    /* name */
    "SubjectName",
    /* qualifiers */
    _PG_SSLCertificate_SubjectName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_PG_SSLCertificate_NotBefore_qualifiers[] =
{
    0,
};

static SourceProperty
_PG_SSLCertificate_NotBefore =
{
    /* flags */
    PEGASUS_FLAG_PROPERTY|PEGASUS_FLAG_READ,
    /* name */
    "NotBefore",
    /* qualifiers */
    _PG_SSLCertificate_NotBefore_qualifiers,
    /* type */
    CIMTYPE_DATETIME,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_PG_SSLCertificate_NotAfter_qualifiers[] =
{
    0,
};

static SourceProperty
_PG_SSLCertificate_NotAfter =
{
    /* flags */
    PEGASUS_FLAG_PROPERTY|PEGASUS_FLAG_READ,
    /* name */
    "NotAfter",
    /* qualifiers */
    _PG_SSLCertificate_NotAfter_qualifiers,
    /* type */
    CIMTYPE_DATETIME,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_PG_SSLCertificate_TruststorePath_qualifiers[] =
{
    0,
};

static SourceProperty
_PG_SSLCertificate_TruststorePath =
{
    /* flags */
    PEGASUS_FLAG_PROPERTY|PEGASUS_FLAG_READ,
    /* name */
    "TruststorePath",
    /* qualifiers */
    _PG_SSLCertificate_TruststorePath_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_PG_SSLCertificate_RegisteredUserName_qualifiers[] =
{
    0,
};

static SourceProperty
_PG_SSLCertificate_RegisteredUserName =
{
    /* flags */
    PEGASUS_FLAG_PROPERTY|PEGASUS_FLAG_READ,
    /* name */
    "RegisteredUserName",
    /* qualifiers */
    _PG_SSLCertificate_RegisteredUserName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_PG_SSLCertificate_CertificateType_qualifiers[] =
{
    /* ValueMap */
    "\061\000\0040\0002\0003\0004\000",
    /* Values */
    "\062\000\004unknown\000authority\000authority issued end-entity\000self-signed identity\000",
    0,
};

static SourceProperty
_PG_SSLCertificate_CertificateType =
{
    /* flags */
    PEGASUS_FLAG_PROPERTY|PEGASUS_FLAG_READ,
    /* name */
    "CertificateType",
    /* qualifiers */
    _PG_SSLCertificate_CertificateType_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_PG_SSLCertificate_addCertificate_certificateContents_qualifiers[] =
{
    /* in */
    "\024\001",
    0,
};

static SourceProperty
_PG_SSLCertificate_addCertificate_certificateContents =
{
    /* flags */
    PEGASUS_FLAG_PROPERTY|PEGASUS_FLAG_IN,
    /* name */
    "certificateContents",
    /* qualifiers */
    _PG_SSLCertificate_addCertificate_certificateContents_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_PG_SSLCertificate_addCertificate_userName_qualifiers[] =
{
    /* in */
    "\024\001",
    0,
};

static SourceProperty
_PG_SSLCertificate_addCertificate_userName =
{
    /* flags */
    PEGASUS_FLAG_PROPERTY|PEGASUS_FLAG_IN,
    /* name */
    "userName",
    /* qualifiers */
    _PG_SSLCertificate_addCertificate_userName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_PG_SSLCertificate_addCertificate_certificateType_qualifiers[] =
{
    /* in */
    "\024\001",
    0,
};

static SourceProperty
_PG_SSLCertificate_addCertificate_certificateType =
{
    /* flags */
    PEGASUS_FLAG_PROPERTY|PEGASUS_FLAG_IN,
    /* name */
    "certificateType",
    /* qualifiers */
    _PG_SSLCertificate_addCertificate_certificateType_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static SourceFeature*
_PG_SSLCertificate_addCertificate_parameters[] =
{
    (SourceFeature*)&_PG_SSLCertificate_addCertificate_certificateContents,
    (SourceFeature*)&_PG_SSLCertificate_addCertificate_userName,
    (SourceFeature*)&_PG_SSLCertificate_addCertificate_certificateType,
    0,
};

static const char*
_PG_SSLCertificate_addCertificate_qualifiers[] =
{
    /* static */
    "\055\001",
    0,
};

static SourceMethod
_PG_SSLCertificate_addCertificate =
{
    /* flags */
    PEGASUS_FLAG_METHOD|PEGASUS_FLAG_STATIC,
    /* name */
    "PG_SSLCertificate",
    /* qualifiers */
    _PG_SSLCertificate_addCertificate_qualifiers,
    /* type */
    CIMTYPE_BOOLEAN,
    /* parameters */
    _PG_SSLCertificate_addCertificate_parameters,
};

static SourceFeature*
_PG_SSLCertificate_features[] =
{
    (SourceFeature*)&_PG_SSLCertificate_IssuerName,
    (SourceFeature*)&_PG_SSLCertificate_SerialNumber,
    (SourceFeature*)&_PG_SSLCertificate_TruststoreType,
    (SourceFeature*)&_PG_SSLCertificate_SubjectName,
    (SourceFeature*)&_PG_SSLCertificate_NotBefore,
    (SourceFeature*)&_PG_SSLCertificate_NotAfter,
    (SourceFeature*)&_PG_SSLCertificate_TruststorePath,
    (SourceFeature*)&_PG_SSLCertificate_RegisteredUserName,
    (SourceFeature*)&_PG_SSLCertificate_CertificateType,
    (SourceFeature*)&_PG_SSLCertificate_addCertificate,
    0,
};

static const char*
__root_PG_Internal_PG_SSLCertificate_qualifiers[] =
{
    /* Version */
    "\0632.6.0",
    0,
};

SourceClass
__root_PG_Internal_PG_SSLCertificate =
{
    /* flags */
    PEGASUS_FLAG_CLASS,
    /* name */
    "PG_SSLCertificate",
    /* qualifiers */
    __root_PG_Internal_PG_SSLCertificate_qualifiers,
    /* super */
    0,
    /* features */
    _PG_SSLCertificate_features,
};

//==============================================================================
//
// Class: PG_SSLCertificateRevocationList
//
//==============================================================================

static const char*
_PG_SSLCertificateRevocationList_IssuerName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static SourceProperty
_PG_SSLCertificateRevocationList_IssuerName =
{
    /* flags */
    PEGASUS_FLAG_PROPERTY|PEGASUS_FLAG_KEY|PEGASUS_FLAG_READ,
    /* name */
    "IssuerName",
    /* qualifiers */
    _PG_SSLCertificateRevocationList_IssuerName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_PG_SSLCertificateRevocationList_LastUpdate_qualifiers[] =
{
    0,
};

static SourceProperty
_PG_SSLCertificateRevocationList_LastUpdate =
{
    /* flags */
    PEGASUS_FLAG_PROPERTY|PEGASUS_FLAG_READ,
    /* name */
    "LastUpdate",
    /* qualifiers */
    _PG_SSLCertificateRevocationList_LastUpdate_qualifiers,
    /* type */
    CIMTYPE_DATETIME,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_PG_SSLCertificateRevocationList_NextUpdate_qualifiers[] =
{
    0,
};

static SourceProperty
_PG_SSLCertificateRevocationList_NextUpdate =
{
    /* flags */
    PEGASUS_FLAG_PROPERTY|PEGASUS_FLAG_READ,
    /* name */
    "NextUpdate",
    /* qualifiers */
    _PG_SSLCertificateRevocationList_NextUpdate_qualifiers,
    /* type */
    CIMTYPE_DATETIME,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_PG_SSLCertificateRevocationList_RevokedSerialNumbers_qualifiers[] =
{
    0,
};

static SourceProperty
_PG_SSLCertificateRevocationList_RevokedSerialNumbers =
{
    /* flags */
    PEGASUS_FLAG_PROPERTY|PEGASUS_FLAG_READ,
    /* name */
    "RevokedSerialNumbers",
    /* qualifiers */
    _PG_SSLCertificateRevocationList_RevokedSerialNumbers_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* value */
    0,
};

static const char*
_PG_SSLCertificateRevocationList_RevocationDates_qualifiers[] =
{
    0,
};

static SourceProperty
_PG_SSLCertificateRevocationList_RevocationDates =
{
    /* flags */
    PEGASUS_FLAG_PROPERTY|PEGASUS_FLAG_READ,
    /* name */
    "RevocationDates",
    /* qualifiers */
    _PG_SSLCertificateRevocationList_RevocationDates_qualifiers,
    /* type */
    CIMTYPE_DATETIME,
    /* subscript */
    0,
    /* value */
    0,
};

static const char*
_PG_SSLCertificateRevocationList_addCertificateRevocationList_CRLContents_qualifiers[] =
{
    /* in */
    "\024\001",
    0,
};

static SourceProperty
_PG_SSLCertificateRevocationList_addCertificateRevocationList_CRLContents =
{
    /* flags */
    PEGASUS_FLAG_PROPERTY|PEGASUS_FLAG_IN,
    /* name */
    "CRLContents",
    /* qualifiers */
    _PG_SSLCertificateRevocationList_addCertificateRevocationList_CRLContents_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static SourceFeature*
_PG_SSLCertificateRevocationList_addCertificateRevocationList_parameters[] =
{
    (SourceFeature*)&_PG_SSLCertificateRevocationList_addCertificateRevocationList_CRLContents,
    0,
};

static const char*
_PG_SSLCertificateRevocationList_addCertificateRevocationList_qualifiers[] =
{
    /* static */
    "\055\001",
    0,
};

static SourceMethod
_PG_SSLCertificateRevocationList_addCertificateRevocationList =
{
    /* flags */
    PEGASUS_FLAG_METHOD|PEGASUS_FLAG_STATIC,
    /* name */
    "PG_SSLCertificateRevocationList",
    /* qualifiers */
    _PG_SSLCertificateRevocationList_addCertificateRevocationList_qualifiers,
    /* type */
    CIMTYPE_BOOLEAN,
    /* parameters */
    _PG_SSLCertificateRevocationList_addCertificateRevocationList_parameters,
};

static SourceFeature*
_PG_SSLCertificateRevocationList_features[] =
{
    (SourceFeature*)&_PG_SSLCertificateRevocationList_IssuerName,
    (SourceFeature*)&_PG_SSLCertificateRevocationList_LastUpdate,
    (SourceFeature*)&_PG_SSLCertificateRevocationList_NextUpdate,
    (SourceFeature*)&_PG_SSLCertificateRevocationList_RevokedSerialNumbers,
    (SourceFeature*)&_PG_SSLCertificateRevocationList_RevocationDates,
    (SourceFeature*)&_PG_SSLCertificateRevocationList_addCertificateRevocationList,
    0,
};

static const char*
__root_PG_Internal_PG_SSLCertificateRevocationList_qualifiers[] =
{
    /* Version */
    "\0632.3.0",
    0,
};

SourceClass
__root_PG_Internal_PG_SSLCertificateRevocationList =
{
    /* flags */
    PEGASUS_FLAG_CLASS,
    /* name */
    "PG_SSLCertificateRevocationList",
    /* qualifiers */
    __root_PG_Internal_PG_SSLCertificateRevocationList_qualifiers,
    /* super */
    0,
    /* features */
    _PG_SSLCertificateRevocationList_features,
};

//==============================================================================
//
// Qualifier array
//
//==============================================================================

static SourceQualifierDecl*
_qualifiers[] =
{
    &_ASSOCIATION_qualifier_decl,
    &_INDICATION_qualifier_decl,
    &_Abstract_qualifier_decl,
    &_Aggregate_qualifier_decl,
    &_Aggregation_qualifier_decl,
    &_ArrayType_qualifier_decl,
    &_BitMap_qualifier_decl,
    &_BitValues_qualifier_decl,
    &_ClassConstraint_qualifier_decl,
    &_Composition_qualifier_decl,
    &_Counter_qualifier_decl,
    &_Deprecated_qualifier_decl,
    &_Description_qualifier_decl,
    &_DisplayName_qualifier_decl,
    &_DN_qualifier_decl,
    &_EmbeddedInstance_qualifier_decl,
    &_EmbeddedObject_qualifier_decl,
    &_Exception_qualifier_decl,
    &_Experimental_qualifier_decl,
    &_Gauge_qualifier_decl,
    &_In_qualifier_decl,
    &_Key_qualifier_decl,
    &_MappingStrings_qualifier_decl,
    &_Max_qualifier_decl,
    &_MaxLen_qualifier_decl,
    &_MaxValue_qualifier_decl,
    &_MethodConstraint_qualifier_decl,
    &_Min_qualifier_decl,
    &_MinLen_qualifier_decl,
    &_MinValue_qualifier_decl,
    &_ModelCorrespondence_qualifier_decl,
    &_Nonlocal_qualifier_decl,
    &_NonlocalType_qualifier_decl,
    &_NullValue_qualifier_decl,
    &_Octetstring_qualifier_decl,
    &_Out_qualifier_decl,
    &_Override_qualifier_decl,
    &_Propagated_qualifier_decl,
    &_PropertyConstraint_qualifier_decl,
    &_Read_qualifier_decl,
    &_Required_qualifier_decl,
    &_Revision_qualifier_decl,
    &_CLASS_qualifier_decl,
    &_Source_qualifier_decl,
    &_SourceType_qualifier_decl,
    &_Static_qualifier_decl,
    &_Terminal_qualifier_decl,
    &_UMLPackagePath_qualifier_decl,
    &_Units_qualifier_decl,
    &_ValueMap_qualifier_decl,
    &_Values_qualifier_decl,
    &_Version_qualifier_decl,
    &_Weak_qualifier_decl,
    &_Write_qualifier_decl,
    &_Alias_qualifier_decl,
    &_Delete_qualifier_decl,
    &_Expensive_qualifier_decl,
    &_Ifdeleted_qualifier_decl,
    &_Invisible_qualifier_decl,
    &_Large_qualifier_decl,
    &_PropertyUsage_qualifier_decl,
    &_Provider_qualifier_decl,
    &_SyntaxType_qualifier_decl,
    &_Syntax_qualifier_decl,
    &_TriggerType_qualifier_decl,
    &_UnknownValues_qualifier_decl,
    &_UnsupportedValues_qualifier_decl,
    0,
};

//==============================================================================
//
// Class array
//
//==============================================================================

static SourceClass*
_classes[] =
{
    &__root_PG_Internal_PG_Authorization,
    &__root_PG_Internal_PG_ConfigSetting,
    &__root_PG_Internal_PG_User,
    &__root_PG_Internal_PG_ShutdownService,
    &__root_PG_Internal_PG_SSLCertificate,
    &__root_PG_Internal_PG_SSLCertificateRevocationList,
    0,
};

const SourceNameSpace root_PG_Internal_namespace =
{
    "root/PG_Internal",
    _qualifiers,
    _classes,
};

PEGASUS_NAMESPACE_END
