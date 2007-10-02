#include "root_PG_InterOp_namespace.h"

/*NOCHKSRC*/

PEGASUS_NAMESPACE_BEGIN

//==============================================================================
//
// Qualifiers
//
//==============================================================================

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

static MetaQualifierDecl
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

extern MetaClass __root_PG_InterOp_CIM_ManagedElement;
extern MetaClass __root_PG_InterOp_CIM_Setting;
extern MetaClass __root_PG_InterOp_CIM_SettingContext;
extern MetaClass __root_PG_InterOp_CIM_SettingData;
extern MetaClass __root_PG_InterOp_CIM_Capabilities;
extern MetaClass __root_PG_InterOp_CIM_Collection;
extern MetaClass __root_PG_InterOp_CIM_CollectionOfMSEs;
extern MetaClass __root_PG_InterOp_CIM_Component;
extern MetaClass __root_PG_InterOp_CIM_ConcreteCollection;
extern MetaClass __root_PG_InterOp_CIM_ConcreteComponent;
extern MetaClass __root_PG_InterOp_CIM_Configuration;
extern MetaClass __root_PG_InterOp_CIM_ConfigurationComponent;
extern MetaClass __root_PG_InterOp_CIM_Dependency;
extern MetaClass __root_PG_InterOp_CIM_DependencyContext;
extern MetaClass __root_PG_InterOp_CIM_ElementCapabilities;
extern MetaClass __root_PG_InterOp_CIM_EnabledLogicalElementCapabilities;
extern MetaClass __root_PG_InterOp_CIM_FRU;
extern MetaClass __root_PG_InterOp_CIM_HostedDependency;
extern MetaClass __root_PG_InterOp_CIM_LocalizationCapabilities;
extern MetaClass __root_PG_InterOp_CIM_Location;
extern MetaClass __root_PG_InterOp_CIM_LogicalIdentity;
extern MetaClass __root_PG_InterOp_CIM_ManagedSystemElement;
extern MetaClass __root_PG_InterOp_CIM_MemberOfCollection;
extern MetaClass __root_PG_InterOp_CIM_MethodParameters;
extern MetaClass __root_PG_InterOp_CIM_OrderedMemberOfCollection;
extern MetaClass __root_PG_InterOp_CIM_ParametersForMethod;
extern MetaClass __root_PG_InterOp_CIM_ParameterValueSources;
extern MetaClass __root_PG_InterOp_CIM_PhysicalElement;
extern MetaClass __root_PG_InterOp_CIM_PowerManagementCapabilities;
extern MetaClass __root_PG_InterOp_CIM_Product;
extern MetaClass __root_PG_InterOp_CIM_ProductComponent;
extern MetaClass __root_PG_InterOp_CIM_ProductFRU;
extern MetaClass __root_PG_InterOp_CIM_ProductParentChild;
extern MetaClass __root_PG_InterOp_CIM_ProductPhysicalComponent;
extern MetaClass __root_PG_InterOp_CIM_ProductPhysicalElements;
extern MetaClass __root_PG_InterOp_CIM_ProductProductDependency;
extern MetaClass __root_PG_InterOp_CIM_Profile;
extern MetaClass __root_PG_InterOp_CIM_ReplaceableProductFRU;
extern MetaClass __root_PG_InterOp_CIM_Spared;
extern MetaClass __root_PG_InterOp_CIM_StatisticalData;
extern MetaClass __root_PG_InterOp_CIM_StatisticalInformation;
extern MetaClass __root_PG_InterOp_CIM_StatisticalSetting;
extern MetaClass __root_PG_InterOp_CIM_Statistics;
extern MetaClass __root_PG_InterOp_CIM_SupportAccess;
extern MetaClass __root_PG_InterOp_CIM_Synchronized;
extern MetaClass __root_PG_InterOp_CIM_SystemConfiguration;
extern MetaClass __root_PG_InterOp_CIM_SystemSetting;
extern MetaClass __root_PG_InterOp_CIM_SystemSettingContext;
extern MetaClass __root_PG_InterOp_CIM_SystemSpecificCollection;
extern MetaClass __root_PG_InterOp_CIM_SystemStatisticalInformation;
extern MetaClass __root_PG_InterOp_CIM_CollectedCollections;
extern MetaClass __root_PG_InterOp_CIM_CollectedMSEs;
extern MetaClass __root_PG_InterOp_CIM_CollectionConfiguration;
extern MetaClass __root_PG_InterOp_CIM_CollectionSetting;
extern MetaClass __root_PG_InterOp_CIM_CompatibleProduct;
extern MetaClass __root_PG_InterOp_CIM_ConcreteDependency;
extern MetaClass __root_PG_InterOp_CIM_ConcreteIdentity;
extern MetaClass __root_PG_InterOp_CIM_ContainedLocation;
extern MetaClass __root_PG_InterOp_CIM_DeviceStatisticalInformation;
extern MetaClass __root_PG_InterOp_CIM_ElementConfiguration;
extern MetaClass __root_PG_InterOp_CIM_ElementLocation;
extern MetaClass __root_PG_InterOp_CIM_ElementProfile;
extern MetaClass __root_PG_InterOp_CIM_ElementSetting;
extern MetaClass __root_PG_InterOp_CIM_ElementSettingData;
extern MetaClass __root_PG_InterOp_CIM_ElementStatisticalData;
extern MetaClass __root_PG_InterOp_CIM_FRUIncludesProduct;
extern MetaClass __root_PG_InterOp_CIM_FRUPhysicalElements;
extern MetaClass __root_PG_InterOp_CIM_LogicalElement;
extern MetaClass __root_PG_InterOp_CIM_PhysicalElementLocation;
extern MetaClass __root_PG_InterOp_CIM_PhysicalStatisticalInformation;
extern MetaClass __root_PG_InterOp_CIM_PhysicalStatistics;
extern MetaClass __root_PG_InterOp_CIM_ProductSupport;
extern MetaClass __root_PG_InterOp_CIM_RedundancyGroup;
extern MetaClass __root_PG_InterOp_CIM_RedundancySet;
extern MetaClass __root_PG_InterOp_CIM_RelatedStatisticalData;
extern MetaClass __root_PG_InterOp_CIM_RelatedStatistics;
extern MetaClass __root_PG_InterOp_CIM_SAPStatisticalInformation;
extern MetaClass __root_PG_InterOp_CIM_ScopedSettingData;
extern MetaClass __root_PG_InterOp_CIM_ServiceStatisticalInformation;
extern MetaClass __root_PG_InterOp_CIM_SettingAssociatedToCapabilities;
extern MetaClass __root_PG_InterOp_CIM_SoftwareIdentity;
extern MetaClass __root_PG_InterOp_CIM_SpareGroup;
extern MetaClass __root_PG_InterOp_CIM_StatisticsCollection;
extern MetaClass __root_PG_InterOp_CIM_StorageRedundancyGroup;
extern MetaClass __root_PG_InterOp_CIM_StorageRedundancySet;
extern MetaClass __root_PG_InterOp_CIM_ActsAsSpare;
extern MetaClass __root_PG_InterOp_CIM_DefaultSetting;
extern MetaClass __root_PG_InterOp_CIM_ElementSoftwareIdentity;
extern MetaClass __root_PG_InterOp_CIM_ExtraCapacityGroup;
extern MetaClass __root_PG_InterOp_CIM_IsSpare;
extern MetaClass __root_PG_InterOp_CIM_Job;
extern MetaClass __root_PG_InterOp_CIM_ProductSoftwareComponent;
extern MetaClass __root_PG_InterOp_CIM_RedundancyComponent;
extern MetaClass __root_PG_InterOp_CIM_ScopedSetting;
extern MetaClass __root_PG_InterOp_CIM_ConcreteJob;
extern MetaClass __root_PG_InterOp_CIM_EnabledLogicalElement;
extern MetaClass __root_PG_InterOp_CIM_LogicalDevice;
extern MetaClass __root_PG_InterOp_CIM_Realizes;
extern MetaClass __root_PG_InterOp_CIM_Service;
extern MetaClass __root_PG_InterOp_CIM_ServiceAccessPoint;
extern MetaClass __root_PG_InterOp_CIM_ServiceAccessURI;
extern MetaClass __root_PG_InterOp_CIM_ServiceAffectsElement;
extern MetaClass __root_PG_InterOp_CIM_ServiceAvailableToElement;
extern MetaClass __root_PG_InterOp_CIM_ServiceComponent;
extern MetaClass __root_PG_InterOp_CIM_ServiceSAPDependency;
extern MetaClass __root_PG_InterOp_CIM_ServiceStatistics;
extern MetaClass __root_PG_InterOp_CIM_StorageExtent;
extern MetaClass __root_PG_InterOp_CIM_System;
extern MetaClass __root_PG_InterOp_CIM_SystemComponent;
extern MetaClass __root_PG_InterOp_CIM_SystemDevice;
extern MetaClass __root_PG_InterOp_CIM_SystemPackaging;
extern MetaClass __root_PG_InterOp_CIM_SystemStatistics;
extern MetaClass __root_PG_InterOp_CIM_AdminDomain;
extern MetaClass __root_PG_InterOp_CIM_BasedOn;
extern MetaClass __root_PG_InterOp_CIM_ConfigurationForSystem;
extern MetaClass __root_PG_InterOp_CIM_ContainedDomain;
extern MetaClass __root_PG_InterOp_CIM_DeviceSAPImplementation;
extern MetaClass __root_PG_InterOp_CIM_DeviceServiceImplementation;
extern MetaClass __root_PG_InterOp_CIM_DeviceStatistics;
extern MetaClass __root_PG_InterOp_CIM_ExtentRedundancyComponent;
extern MetaClass __root_PG_InterOp_CIM_HostedAccessPoint;
extern MetaClass __root_PG_InterOp_CIM_HostedCollection;
extern MetaClass __root_PG_InterOp_CIM_HostedService;
extern MetaClass __root_PG_InterOp_CIM_InstalledSoftwareIdentity;
extern MetaClass __root_PG_InterOp_CIM_PowerManagementService;
extern MetaClass __root_PG_InterOp_CIM_ProductServiceComponent;
extern MetaClass __root_PG_InterOp_CIM_ProtocolEndpoint;
extern MetaClass __root_PG_InterOp_CIM_ProvidesServiceToElement;
extern MetaClass __root_PG_InterOp_CIM_RemoteServiceAccessPoint;
extern MetaClass __root_PG_InterOp_CIM_SAPAvailableForElement;
extern MetaClass __root_PG_InterOp_CIM_SAPSAPDependency;
extern MetaClass __root_PG_InterOp_CIM_SAPStatistics;
extern MetaClass __root_PG_InterOp_CIM_ServiceAccessBySAP;
extern MetaClass __root_PG_InterOp_CIM_ServiceServiceDependency;
extern MetaClass __root_PG_InterOp_CIM_SettingForSystem;
extern MetaClass __root_PG_InterOp_CIM_ActiveConnection;
extern MetaClass __root_PG_InterOp_CIM_BindsTo;
extern MetaClass __root_PG_InterOp_CIM_ProvidesEndpoint;
extern MetaClass __root_PG_InterOp_CIM_RemotePort;
extern MetaClass __root_PG_InterOp_CIM_Indication;
extern MetaClass __root_PG_InterOp_CIM_ClassIndication;
extern MetaClass __root_PG_InterOp_CIM_ClassModification;
extern MetaClass __root_PG_InterOp_CIM_IndicationFilter;
extern MetaClass __root_PG_InterOp_CIM_InstIndication;
extern MetaClass __root_PG_InterOp_CIM_InstMethodCall;
extern MetaClass __root_PG_InterOp_CIM_InstModification;
extern MetaClass __root_PG_InterOp_CIM_InstRead;
extern MetaClass __root_PG_InterOp_CIM_ListenerDestination;
extern MetaClass __root_PG_InterOp_CIM_ListenerDestinationCIMXML;
extern MetaClass __root_PG_InterOp_CIM_ProcessIndication;
extern MetaClass __root_PG_InterOp_CIM_SNMPTrapIndication;
extern MetaClass __root_PG_InterOp_CIM_AlertIndication;
extern MetaClass __root_PG_InterOp_CIM_AlertInstIndication;
extern MetaClass __root_PG_InterOp_CIM_ClassCreation;
extern MetaClass __root_PG_InterOp_CIM_ClassDeletion;
extern MetaClass __root_PG_InterOp_CIM_IndicationHandler;
extern MetaClass __root_PG_InterOp_CIM_IndicationHandlerCIMXML;
extern MetaClass __root_PG_InterOp_CIM_IndicationSubscription;
extern MetaClass __root_PG_InterOp_CIM_InstCreation;
extern MetaClass __root_PG_InterOp_CIM_InstDeletion;
extern MetaClass __root_PG_InterOp_CIM_ThresholdIndication;
extern MetaClass __root_PG_InterOp_CIM_FormattedIndicationSubscription;
extern MetaClass __root_PG_InterOp_CIM_Namespace;
extern MetaClass __root_PG_InterOp_CIM_RegisteredProfile;
extern MetaClass __root_PG_InterOp_CIM_RegisteredSubProfile;
extern MetaClass __root_PG_InterOp_CIM_SystemIdentification;
extern MetaClass __root_PG_InterOp_CIM_SystemInNamespace;
extern MetaClass __root_PG_InterOp_CIM_CIMOMStatisticalData;
extern MetaClass __root_PG_InterOp_CIM_ElementConformsToProfile;
extern MetaClass __root_PG_InterOp_CIM_ReferencedProfile;
extern MetaClass __root_PG_InterOp_CIM_SubProfileRequiresProfile;
extern MetaClass __root_PG_InterOp_CIM_WBEMService;
extern MetaClass __root_PG_InterOp_CIM_IdentificationOfManagedSystem;
extern MetaClass __root_PG_InterOp_CIM_ObjectManager;
extern MetaClass __root_PG_InterOp_CIM_ObjectManagerAdapter;
extern MetaClass __root_PG_InterOp_CIM_ObjectManagerCommunicationMechanism;
extern MetaClass __root_PG_InterOp_CIM_ProtocolAdapter;
extern MetaClass __root_PG_InterOp_CIM_CIMXMLCommunicationMechanism;
extern MetaClass __root_PG_InterOp_CIM_CommMechanismForAdapter;
extern MetaClass __root_PG_InterOp_CIM_CommMechanismForManager;
extern MetaClass __root_PG_InterOp_CIM_CommMechanismForObjectManagerAdapter;
extern MetaClass __root_PG_InterOp_CIM_NamespaceInManager;
extern MetaClass __root_PG_InterOp_PG_IndicationHandlerSNMPMapper;
extern MetaClass __root_PG_InterOp_PG_ListenerDestinationSystemLog;
extern MetaClass __root_PG_InterOp_PG_ListenerDestinationEmail;
extern MetaClass __root_PG_InterOp_PG_ProviderModule;
extern MetaClass __root_PG_InterOp_PG_Provider;
extern MetaClass __root_PG_InterOp_PG_Capabilities;
extern MetaClass __root_PG_InterOp_PG_CapabilitiesRegistration;
extern MetaClass __root_PG_InterOp_PG_ProviderCapabilities;
extern MetaClass __root_PG_InterOp_PG_ConsumerCapabilities;
extern MetaClass __root_PG_InterOp_PG_ProviderCapabilitiesElements;
extern MetaClass __root_PG_InterOp_PG_ProviderModuleElements;
extern MetaClass __root_PG_InterOp_PG_CIMXMLCommunicationMechanism;
extern MetaClass __root_PG_InterOp_PG_NameSpace;
extern MetaClass __root_PG_InterOp_CIM_ComputerSystem;
extern MetaClass __root_PG_InterOp_CIM_UnitaryComputerSystem;
extern MetaClass __root_PG_InterOp_PG_ComputerSystem;
extern MetaClass __root_PG_InterOp_PG_ProviderProfileCapabilities;
extern MetaClass __root_PG_InterOp_PG_ProviderReferencedProfiles;
extern MetaClass __root_PG_InterOp_PG_RegisteredProfile;
extern MetaClass __root_PG_InterOp_PG_RegisteredSubProfile;
extern MetaClass __root_PG_InterOp_PG_ReferencedProfile;
extern MetaClass __root_PG_InterOp_PG_ElementConformsToProfile;
extern MetaClass __root_PG_InterOp_PG_SubProfileRequiresProfile;
extern MetaClass __root_PG_InterOp_PG_SoftwareIdentity;
extern MetaClass __root_PG_InterOp_PG_ElementSoftwareIdentity;
extern MetaClass __root_PG_InterOp_PG_ObjectManager;
extern MetaClass __root_PG_InterOp_PG_CommMechanismForManager;
extern MetaClass __root_PG_InterOp_PG_NamespaceInManager;
extern MetaClass __root_PG_InterOp_PG_HostedObjectManager;
extern MetaClass __root_PG_InterOp_PG_HostedAccessPoint;

//==============================================================================
//
// Class: CIM_ManagedElement
//
//==============================================================================

static const char*
_CIM_ManagedElement_Caption_qualifiers[] =
{
    /* MaxLen */
    "\030\000\000\000\100",
    0,
};

static MetaProperty
_CIM_ManagedElement_Caption =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "Caption",
    /* qualifiers */
    _CIM_ManagedElement_Caption_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_ManagedElement_Description_qualifiers[] =
{
    0,
};

static MetaProperty
_CIM_ManagedElement_Description =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "Description",
    /* qualifiers */
    _CIM_ManagedElement_Description_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_ManagedElement_ElementName_qualifiers[] =
{
    0,
};

static MetaProperty
_CIM_ManagedElement_ElementName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "ElementName",
    /* qualifiers */
    _CIM_ManagedElement_ElementName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_ManagedElement_features[] =
{
    (MetaFeature*)&_CIM_ManagedElement_Caption,
    (MetaFeature*)&_CIM_ManagedElement_Description,
    (MetaFeature*)&_CIM_ManagedElement_ElementName,
    0,
};

static const char*
__root_PG_InterOp_CIM_ManagedElement_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Core::CoreElements",
    /* Abstract */
    "\002\001",
    /* Version */
    "\0632.10.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ManagedElement =
{
    /* flags */
    META_FLAG_CLASS|META_FLAG_ABSTRACT,
    /* name */
    "CIM_ManagedElement",
    /* qualifiers */
    __root_PG_InterOp_CIM_ManagedElement_qualifiers,
    /* super */
    0,
    /* features */
    _CIM_ManagedElement_features,
};

//==============================================================================
//
// Class: CIM_Setting
//
//==============================================================================

static const char*
_CIM_Setting_SettingID_qualifiers[] =
{
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_Setting_SettingID =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "SettingID",
    /* qualifiers */
    _CIM_Setting_SettingID_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Setting_VerifyOKToApplyToMSE_MSE_qualifiers[] =
{
    /* IN */
    "\024\001",
    0,
};

static MetaReference
_CIM_Setting_VerifyOKToApplyToMSE_MSE =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_IN,
    /* name */
    "MSE",
    /* qualifiers */
    _CIM_Setting_VerifyOKToApplyToMSE_MSE_qualifiers,
    /* subscript */
    -1,
    /* ref */
    &__root_PG_InterOp_CIM_ManagedSystemElement,
};

static const char*
_CIM_Setting_VerifyOKToApplyToMSE_TimeToApply_qualifiers[] =
{
    /* IN */
    "\024\001",
    0,
};

static MetaProperty
_CIM_Setting_VerifyOKToApplyToMSE_TimeToApply =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_IN,
    /* name */
    "TimeToApply",
    /* qualifiers */
    _CIM_Setting_VerifyOKToApplyToMSE_TimeToApply_qualifiers,
    /* type */
    CIMTYPE_DATETIME,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Setting_VerifyOKToApplyToMSE_MustBeCompletedBy_qualifiers[] =
{
    /* IN */
    "\024\001",
    0,
};

static MetaProperty
_CIM_Setting_VerifyOKToApplyToMSE_MustBeCompletedBy =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_IN,
    /* name */
    "MustBeCompletedBy",
    /* qualifiers */
    _CIM_Setting_VerifyOKToApplyToMSE_MustBeCompletedBy_qualifiers,
    /* type */
    CIMTYPE_DATETIME,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_Setting_VerifyOKToApplyToMSE_parameters[] =
{
    (MetaFeature*)&_CIM_Setting_VerifyOKToApplyToMSE_MSE,
    (MetaFeature*)&_CIM_Setting_VerifyOKToApplyToMSE_TimeToApply,
    (MetaFeature*)&_CIM_Setting_VerifyOKToApplyToMSE_MustBeCompletedBy,
    0,
};

static const char*
_CIM_Setting_VerifyOKToApplyToMSE_qualifiers[] =
{
    0,
};

static MetaMethod
_CIM_Setting_VerifyOKToApplyToMSE =
{
    /* flags */
    META_FLAG_METHOD,
    /* name */
    "CIM_Setting",
    /* qualifiers */
    _CIM_Setting_VerifyOKToApplyToMSE_qualifiers,
    /* type */
    CIMTYPE_UINT32,
    /* parameters */
    _CIM_Setting_VerifyOKToApplyToMSE_parameters,
};

static const char*
_CIM_Setting_ApplyToMSE_MSE_qualifiers[] =
{
    /* IN */
    "\024\001",
    0,
};

static MetaReference
_CIM_Setting_ApplyToMSE_MSE =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_IN,
    /* name */
    "MSE",
    /* qualifiers */
    _CIM_Setting_ApplyToMSE_MSE_qualifiers,
    /* subscript */
    -1,
    /* ref */
    &__root_PG_InterOp_CIM_ManagedSystemElement,
};

static const char*
_CIM_Setting_ApplyToMSE_TimeToApply_qualifiers[] =
{
    /* IN */
    "\024\001",
    0,
};

static MetaProperty
_CIM_Setting_ApplyToMSE_TimeToApply =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_IN,
    /* name */
    "TimeToApply",
    /* qualifiers */
    _CIM_Setting_ApplyToMSE_TimeToApply_qualifiers,
    /* type */
    CIMTYPE_DATETIME,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Setting_ApplyToMSE_MustBeCompletedBy_qualifiers[] =
{
    /* IN */
    "\024\001",
    0,
};

static MetaProperty
_CIM_Setting_ApplyToMSE_MustBeCompletedBy =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_IN,
    /* name */
    "MustBeCompletedBy",
    /* qualifiers */
    _CIM_Setting_ApplyToMSE_MustBeCompletedBy_qualifiers,
    /* type */
    CIMTYPE_DATETIME,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_Setting_ApplyToMSE_parameters[] =
{
    (MetaFeature*)&_CIM_Setting_ApplyToMSE_MSE,
    (MetaFeature*)&_CIM_Setting_ApplyToMSE_TimeToApply,
    (MetaFeature*)&_CIM_Setting_ApplyToMSE_MustBeCompletedBy,
    0,
};

static const char*
_CIM_Setting_ApplyToMSE_qualifiers[] =
{
    0,
};

static MetaMethod
_CIM_Setting_ApplyToMSE =
{
    /* flags */
    META_FLAG_METHOD,
    /* name */
    "CIM_Setting",
    /* qualifiers */
    _CIM_Setting_ApplyToMSE_qualifiers,
    /* type */
    CIMTYPE_UINT32,
    /* parameters */
    _CIM_Setting_ApplyToMSE_parameters,
};

static const char*
_CIM_Setting_VerifyOKToApplyToCollection_Collection_qualifiers[] =
{
    /* IN */
    "\024\001",
    0,
};

static MetaReference
_CIM_Setting_VerifyOKToApplyToCollection_Collection =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_IN,
    /* name */
    "Collection",
    /* qualifiers */
    _CIM_Setting_VerifyOKToApplyToCollection_Collection_qualifiers,
    /* subscript */
    -1,
    /* ref */
    &__root_PG_InterOp_CIM_CollectionOfMSEs,
};

static const char*
_CIM_Setting_VerifyOKToApplyToCollection_TimeToApply_qualifiers[] =
{
    /* IN */
    "\024\001",
    0,
};

static MetaProperty
_CIM_Setting_VerifyOKToApplyToCollection_TimeToApply =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_IN,
    /* name */
    "TimeToApply",
    /* qualifiers */
    _CIM_Setting_VerifyOKToApplyToCollection_TimeToApply_qualifiers,
    /* type */
    CIMTYPE_DATETIME,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Setting_VerifyOKToApplyToCollection_MustBeCompletedBy_qualifiers[] =
{
    /* IN */
    "\024\001",
    0,
};

static MetaProperty
_CIM_Setting_VerifyOKToApplyToCollection_MustBeCompletedBy =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_IN,
    /* name */
    "MustBeCompletedBy",
    /* qualifiers */
    _CIM_Setting_VerifyOKToApplyToCollection_MustBeCompletedBy_qualifiers,
    /* type */
    CIMTYPE_DATETIME,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Setting_VerifyOKToApplyToCollection_CanNotApply_qualifiers[] =
{
    /* IN */
    "\024\000",
    /* OUT */
    "\043\001",
    0,
};

static MetaProperty
_CIM_Setting_VerifyOKToApplyToCollection_CanNotApply =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_OUT,
    /* name */
    "CanNotApply",
    /* qualifiers */
    _CIM_Setting_VerifyOKToApplyToCollection_CanNotApply_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* value */
    0,
};

static MetaFeature*
_CIM_Setting_VerifyOKToApplyToCollection_parameters[] =
{
    (MetaFeature*)&_CIM_Setting_VerifyOKToApplyToCollection_Collection,
    (MetaFeature*)&_CIM_Setting_VerifyOKToApplyToCollection_TimeToApply,
    (MetaFeature*)&_CIM_Setting_VerifyOKToApplyToCollection_MustBeCompletedBy,
    (MetaFeature*)&_CIM_Setting_VerifyOKToApplyToCollection_CanNotApply,
    0,
};

static const char*
_CIM_Setting_VerifyOKToApplyToCollection_qualifiers[] =
{
    0,
};

static MetaMethod
_CIM_Setting_VerifyOKToApplyToCollection =
{
    /* flags */
    META_FLAG_METHOD,
    /* name */
    "CIM_Setting",
    /* qualifiers */
    _CIM_Setting_VerifyOKToApplyToCollection_qualifiers,
    /* type */
    CIMTYPE_UINT32,
    /* parameters */
    _CIM_Setting_VerifyOKToApplyToCollection_parameters,
};

static const char*
_CIM_Setting_ApplyToCollection_Collection_qualifiers[] =
{
    /* IN */
    "\024\001",
    0,
};

static MetaReference
_CIM_Setting_ApplyToCollection_Collection =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_IN,
    /* name */
    "Collection",
    /* qualifiers */
    _CIM_Setting_ApplyToCollection_Collection_qualifiers,
    /* subscript */
    -1,
    /* ref */
    &__root_PG_InterOp_CIM_CollectionOfMSEs,
};

static const char*
_CIM_Setting_ApplyToCollection_TimeToApply_qualifiers[] =
{
    /* IN */
    "\024\001",
    0,
};

static MetaProperty
_CIM_Setting_ApplyToCollection_TimeToApply =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_IN,
    /* name */
    "TimeToApply",
    /* qualifiers */
    _CIM_Setting_ApplyToCollection_TimeToApply_qualifiers,
    /* type */
    CIMTYPE_DATETIME,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Setting_ApplyToCollection_ContinueOnError_qualifiers[] =
{
    /* IN */
    "\024\001",
    0,
};

static MetaProperty
_CIM_Setting_ApplyToCollection_ContinueOnError =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_IN,
    /* name */
    "ContinueOnError",
    /* qualifiers */
    _CIM_Setting_ApplyToCollection_ContinueOnError_qualifiers,
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Setting_ApplyToCollection_MustBeCompletedBy_qualifiers[] =
{
    /* IN */
    "\024\001",
    0,
};

static MetaProperty
_CIM_Setting_ApplyToCollection_MustBeCompletedBy =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_IN,
    /* name */
    "MustBeCompletedBy",
    /* qualifiers */
    _CIM_Setting_ApplyToCollection_MustBeCompletedBy_qualifiers,
    /* type */
    CIMTYPE_DATETIME,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Setting_ApplyToCollection_CanNotApply_qualifiers[] =
{
    /* IN */
    "\024\000",
    /* OUT */
    "\043\001",
    0,
};

static MetaProperty
_CIM_Setting_ApplyToCollection_CanNotApply =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_OUT,
    /* name */
    "CanNotApply",
    /* qualifiers */
    _CIM_Setting_ApplyToCollection_CanNotApply_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* value */
    0,
};

static MetaFeature*
_CIM_Setting_ApplyToCollection_parameters[] =
{
    (MetaFeature*)&_CIM_Setting_ApplyToCollection_Collection,
    (MetaFeature*)&_CIM_Setting_ApplyToCollection_TimeToApply,
    (MetaFeature*)&_CIM_Setting_ApplyToCollection_ContinueOnError,
    (MetaFeature*)&_CIM_Setting_ApplyToCollection_MustBeCompletedBy,
    (MetaFeature*)&_CIM_Setting_ApplyToCollection_CanNotApply,
    0,
};

static const char*
_CIM_Setting_ApplyToCollection_qualifiers[] =
{
    0,
};

static MetaMethod
_CIM_Setting_ApplyToCollection =
{
    /* flags */
    META_FLAG_METHOD,
    /* name */
    "CIM_Setting",
    /* qualifiers */
    _CIM_Setting_ApplyToCollection_qualifiers,
    /* type */
    CIMTYPE_UINT32,
    /* parameters */
    _CIM_Setting_ApplyToCollection_parameters,
};

static const char*
_CIM_Setting_VerifyOKToApplyIncrementalChangeToMSE_MSE_qualifiers[] =
{
    /* IN */
    "\024\001",
    0,
};

static MetaReference
_CIM_Setting_VerifyOKToApplyIncrementalChangeToMSE_MSE =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_IN,
    /* name */
    "MSE",
    /* qualifiers */
    _CIM_Setting_VerifyOKToApplyIncrementalChangeToMSE_MSE_qualifiers,
    /* subscript */
    -1,
    /* ref */
    &__root_PG_InterOp_CIM_ManagedSystemElement,
};

static const char*
_CIM_Setting_VerifyOKToApplyIncrementalChangeToMSE_TimeToApply_qualifiers[] =
{
    /* IN */
    "\024\001",
    0,
};

static MetaProperty
_CIM_Setting_VerifyOKToApplyIncrementalChangeToMSE_TimeToApply =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_IN,
    /* name */
    "TimeToApply",
    /* qualifiers */
    _CIM_Setting_VerifyOKToApplyIncrementalChangeToMSE_TimeToApply_qualifiers,
    /* type */
    CIMTYPE_DATETIME,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Setting_VerifyOKToApplyIncrementalChangeToMSE_MustBeCompletedBy_qualifiers[] =
{
    /* IN */
    "\024\001",
    0,
};

static MetaProperty
_CIM_Setting_VerifyOKToApplyIncrementalChangeToMSE_MustBeCompletedBy =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_IN,
    /* name */
    "MustBeCompletedBy",
    /* qualifiers */
    _CIM_Setting_VerifyOKToApplyIncrementalChangeToMSE_MustBeCompletedBy_qualifiers,
    /* type */
    CIMTYPE_DATETIME,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Setting_VerifyOKToApplyIncrementalChangeToMSE_PropertiesToApply_qualifiers[] =
{
    /* IN */
    "\024\001",
    0,
};

static MetaProperty
_CIM_Setting_VerifyOKToApplyIncrementalChangeToMSE_PropertiesToApply =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_IN,
    /* name */
    "PropertiesToApply",
    /* qualifiers */
    _CIM_Setting_VerifyOKToApplyIncrementalChangeToMSE_PropertiesToApply_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* value */
    0,
};

static MetaFeature*
_CIM_Setting_VerifyOKToApplyIncrementalChangeToMSE_parameters[] =
{
    (MetaFeature*)&_CIM_Setting_VerifyOKToApplyIncrementalChangeToMSE_MSE,
    (MetaFeature*)&_CIM_Setting_VerifyOKToApplyIncrementalChangeToMSE_TimeToApply,
    (MetaFeature*)&_CIM_Setting_VerifyOKToApplyIncrementalChangeToMSE_MustBeCompletedBy,
    (MetaFeature*)&_CIM_Setting_VerifyOKToApplyIncrementalChangeToMSE_PropertiesToApply,
    0,
};

static const char*
_CIM_Setting_VerifyOKToApplyIncrementalChangeToMSE_qualifiers[] =
{
    0,
};

static MetaMethod
_CIM_Setting_VerifyOKToApplyIncrementalChangeToMSE =
{
    /* flags */
    META_FLAG_METHOD,
    /* name */
    "CIM_Setting",
    /* qualifiers */
    _CIM_Setting_VerifyOKToApplyIncrementalChangeToMSE_qualifiers,
    /* type */
    CIMTYPE_UINT32,
    /* parameters */
    _CIM_Setting_VerifyOKToApplyIncrementalChangeToMSE_parameters,
};

static const char*
_CIM_Setting_ApplyIncrementalChangeToMSE_MSE_qualifiers[] =
{
    /* IN */
    "\024\001",
    0,
};

static MetaReference
_CIM_Setting_ApplyIncrementalChangeToMSE_MSE =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_IN,
    /* name */
    "MSE",
    /* qualifiers */
    _CIM_Setting_ApplyIncrementalChangeToMSE_MSE_qualifiers,
    /* subscript */
    -1,
    /* ref */
    &__root_PG_InterOp_CIM_ManagedSystemElement,
};

static const char*
_CIM_Setting_ApplyIncrementalChangeToMSE_TimeToApply_qualifiers[] =
{
    /* IN */
    "\024\001",
    0,
};

static MetaProperty
_CIM_Setting_ApplyIncrementalChangeToMSE_TimeToApply =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_IN,
    /* name */
    "TimeToApply",
    /* qualifiers */
    _CIM_Setting_ApplyIncrementalChangeToMSE_TimeToApply_qualifiers,
    /* type */
    CIMTYPE_DATETIME,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Setting_ApplyIncrementalChangeToMSE_MustBeCompletedBy_qualifiers[] =
{
    /* IN */
    "\024\001",
    0,
};

static MetaProperty
_CIM_Setting_ApplyIncrementalChangeToMSE_MustBeCompletedBy =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_IN,
    /* name */
    "MustBeCompletedBy",
    /* qualifiers */
    _CIM_Setting_ApplyIncrementalChangeToMSE_MustBeCompletedBy_qualifiers,
    /* type */
    CIMTYPE_DATETIME,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Setting_ApplyIncrementalChangeToMSE_PropertiesToApply_qualifiers[] =
{
    /* IN */
    "\024\001",
    0,
};

static MetaProperty
_CIM_Setting_ApplyIncrementalChangeToMSE_PropertiesToApply =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_IN,
    /* name */
    "PropertiesToApply",
    /* qualifiers */
    _CIM_Setting_ApplyIncrementalChangeToMSE_PropertiesToApply_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* value */
    0,
};

static MetaFeature*
_CIM_Setting_ApplyIncrementalChangeToMSE_parameters[] =
{
    (MetaFeature*)&_CIM_Setting_ApplyIncrementalChangeToMSE_MSE,
    (MetaFeature*)&_CIM_Setting_ApplyIncrementalChangeToMSE_TimeToApply,
    (MetaFeature*)&_CIM_Setting_ApplyIncrementalChangeToMSE_MustBeCompletedBy,
    (MetaFeature*)&_CIM_Setting_ApplyIncrementalChangeToMSE_PropertiesToApply,
    0,
};

static const char*
_CIM_Setting_ApplyIncrementalChangeToMSE_qualifiers[] =
{
    0,
};

static MetaMethod
_CIM_Setting_ApplyIncrementalChangeToMSE =
{
    /* flags */
    META_FLAG_METHOD,
    /* name */
    "CIM_Setting",
    /* qualifiers */
    _CIM_Setting_ApplyIncrementalChangeToMSE_qualifiers,
    /* type */
    CIMTYPE_UINT32,
    /* parameters */
    _CIM_Setting_ApplyIncrementalChangeToMSE_parameters,
};

static const char*
_CIM_Setting_VerifyOKToApplyIncrementalChangeToCollection_Collection_qualifiers[] =
{
    /* IN */
    "\024\001",
    0,
};

static MetaReference
_CIM_Setting_VerifyOKToApplyIncrementalChangeToCollection_Collection =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_IN,
    /* name */
    "Collection",
    /* qualifiers */
    _CIM_Setting_VerifyOKToApplyIncrementalChangeToCollection_Collection_qualifiers,
    /* subscript */
    -1,
    /* ref */
    &__root_PG_InterOp_CIM_CollectionOfMSEs,
};

static const char*
_CIM_Setting_VerifyOKToApplyIncrementalChangeToCollection_TimeToApply_qualifiers[] =
{
    /* IN */
    "\024\001",
    0,
};

static MetaProperty
_CIM_Setting_VerifyOKToApplyIncrementalChangeToCollection_TimeToApply =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_IN,
    /* name */
    "TimeToApply",
    /* qualifiers */
    _CIM_Setting_VerifyOKToApplyIncrementalChangeToCollection_TimeToApply_qualifiers,
    /* type */
    CIMTYPE_DATETIME,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Setting_VerifyOKToApplyIncrementalChangeToCollection_MustBeCompletedBy_qualifiers[] =
{
    /* IN */
    "\024\001",
    0,
};

static MetaProperty
_CIM_Setting_VerifyOKToApplyIncrementalChangeToCollection_MustBeCompletedBy =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_IN,
    /* name */
    "MustBeCompletedBy",
    /* qualifiers */
    _CIM_Setting_VerifyOKToApplyIncrementalChangeToCollection_MustBeCompletedBy_qualifiers,
    /* type */
    CIMTYPE_DATETIME,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Setting_VerifyOKToApplyIncrementalChangeToCollection_PropertiesToApply_qualifiers[] =
{
    /* IN */
    "\024\001",
    0,
};

static MetaProperty
_CIM_Setting_VerifyOKToApplyIncrementalChangeToCollection_PropertiesToApply =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_IN,
    /* name */
    "PropertiesToApply",
    /* qualifiers */
    _CIM_Setting_VerifyOKToApplyIncrementalChangeToCollection_PropertiesToApply_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* value */
    0,
};

static const char*
_CIM_Setting_VerifyOKToApplyIncrementalChangeToCollection_CanNotApply_qualifiers[] =
{
    /* IN */
    "\024\000",
    /* OUT */
    "\043\001",
    0,
};

static MetaProperty
_CIM_Setting_VerifyOKToApplyIncrementalChangeToCollection_CanNotApply =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_OUT,
    /* name */
    "CanNotApply",
    /* qualifiers */
    _CIM_Setting_VerifyOKToApplyIncrementalChangeToCollection_CanNotApply_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* value */
    0,
};

static MetaFeature*
_CIM_Setting_VerifyOKToApplyIncrementalChangeToCollection_parameters[] =
{
    (MetaFeature*)&_CIM_Setting_VerifyOKToApplyIncrementalChangeToCollection_Collection,
    (MetaFeature*)&_CIM_Setting_VerifyOKToApplyIncrementalChangeToCollection_TimeToApply,
    (MetaFeature*)&_CIM_Setting_VerifyOKToApplyIncrementalChangeToCollection_MustBeCompletedBy,
    (MetaFeature*)&_CIM_Setting_VerifyOKToApplyIncrementalChangeToCollection_PropertiesToApply,
    (MetaFeature*)&_CIM_Setting_VerifyOKToApplyIncrementalChangeToCollection_CanNotApply,
    0,
};

static const char*
_CIM_Setting_VerifyOKToApplyIncrementalChangeToCollection_qualifiers[] =
{
    0,
};

static MetaMethod
_CIM_Setting_VerifyOKToApplyIncrementalChangeToCollection =
{
    /* flags */
    META_FLAG_METHOD,
    /* name */
    "CIM_Setting",
    /* qualifiers */
    _CIM_Setting_VerifyOKToApplyIncrementalChangeToCollection_qualifiers,
    /* type */
    CIMTYPE_UINT32,
    /* parameters */
    _CIM_Setting_VerifyOKToApplyIncrementalChangeToCollection_parameters,
};

static const char*
_CIM_Setting_ApplyIncrementalChangeToCollection_Collection_qualifiers[] =
{
    /* IN */
    "\024\001",
    0,
};

static MetaReference
_CIM_Setting_ApplyIncrementalChangeToCollection_Collection =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_IN,
    /* name */
    "Collection",
    /* qualifiers */
    _CIM_Setting_ApplyIncrementalChangeToCollection_Collection_qualifiers,
    /* subscript */
    -1,
    /* ref */
    &__root_PG_InterOp_CIM_CollectionOfMSEs,
};

static const char*
_CIM_Setting_ApplyIncrementalChangeToCollection_TimeToApply_qualifiers[] =
{
    /* IN */
    "\024\001",
    0,
};

static MetaProperty
_CIM_Setting_ApplyIncrementalChangeToCollection_TimeToApply =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_IN,
    /* name */
    "TimeToApply",
    /* qualifiers */
    _CIM_Setting_ApplyIncrementalChangeToCollection_TimeToApply_qualifiers,
    /* type */
    CIMTYPE_DATETIME,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Setting_ApplyIncrementalChangeToCollection_ContinueOnError_qualifiers[] =
{
    /* IN */
    "\024\001",
    0,
};

static MetaProperty
_CIM_Setting_ApplyIncrementalChangeToCollection_ContinueOnError =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_IN,
    /* name */
    "ContinueOnError",
    /* qualifiers */
    _CIM_Setting_ApplyIncrementalChangeToCollection_ContinueOnError_qualifiers,
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Setting_ApplyIncrementalChangeToCollection_MustBeCompletedBy_qualifiers[] =
{
    /* IN */
    "\024\001",
    0,
};

static MetaProperty
_CIM_Setting_ApplyIncrementalChangeToCollection_MustBeCompletedBy =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_IN,
    /* name */
    "MustBeCompletedBy",
    /* qualifiers */
    _CIM_Setting_ApplyIncrementalChangeToCollection_MustBeCompletedBy_qualifiers,
    /* type */
    CIMTYPE_DATETIME,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Setting_ApplyIncrementalChangeToCollection_PropertiesToApply_qualifiers[] =
{
    /* IN */
    "\024\001",
    0,
};

static MetaProperty
_CIM_Setting_ApplyIncrementalChangeToCollection_PropertiesToApply =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_IN,
    /* name */
    "PropertiesToApply",
    /* qualifiers */
    _CIM_Setting_ApplyIncrementalChangeToCollection_PropertiesToApply_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* value */
    0,
};

static const char*
_CIM_Setting_ApplyIncrementalChangeToCollection_CanNotApply_qualifiers[] =
{
    /* IN */
    "\024\000",
    /* OUT */
    "\043\001",
    0,
};

static MetaProperty
_CIM_Setting_ApplyIncrementalChangeToCollection_CanNotApply =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_OUT,
    /* name */
    "CanNotApply",
    /* qualifiers */
    _CIM_Setting_ApplyIncrementalChangeToCollection_CanNotApply_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* value */
    0,
};

static MetaFeature*
_CIM_Setting_ApplyIncrementalChangeToCollection_parameters[] =
{
    (MetaFeature*)&_CIM_Setting_ApplyIncrementalChangeToCollection_Collection,
    (MetaFeature*)&_CIM_Setting_ApplyIncrementalChangeToCollection_TimeToApply,
    (MetaFeature*)&_CIM_Setting_ApplyIncrementalChangeToCollection_ContinueOnError,
    (MetaFeature*)&_CIM_Setting_ApplyIncrementalChangeToCollection_MustBeCompletedBy,
    (MetaFeature*)&_CIM_Setting_ApplyIncrementalChangeToCollection_PropertiesToApply,
    (MetaFeature*)&_CIM_Setting_ApplyIncrementalChangeToCollection_CanNotApply,
    0,
};

static const char*
_CIM_Setting_ApplyIncrementalChangeToCollection_qualifiers[] =
{
    0,
};

static MetaMethod
_CIM_Setting_ApplyIncrementalChangeToCollection =
{
    /* flags */
    META_FLAG_METHOD,
    /* name */
    "CIM_Setting",
    /* qualifiers */
    _CIM_Setting_ApplyIncrementalChangeToCollection_qualifiers,
    /* type */
    CIMTYPE_UINT32,
    /* parameters */
    _CIM_Setting_ApplyIncrementalChangeToCollection_parameters,
};

static MetaFeature*
_CIM_Setting_features[] =
{
    (MetaFeature*)&_CIM_Setting_SettingID,
    (MetaFeature*)&_CIM_Setting_VerifyOKToApplyToMSE,
    (MetaFeature*)&_CIM_Setting_ApplyToMSE,
    (MetaFeature*)&_CIM_Setting_VerifyOKToApplyToCollection,
    (MetaFeature*)&_CIM_Setting_ApplyToCollection,
    (MetaFeature*)&_CIM_Setting_VerifyOKToApplyIncrementalChangeToMSE,
    (MetaFeature*)&_CIM_Setting_ApplyIncrementalChangeToMSE,
    (MetaFeature*)&_CIM_Setting_VerifyOKToApplyIncrementalChangeToCollection,
    (MetaFeature*)&_CIM_Setting_ApplyIncrementalChangeToCollection,
    0,
};

static const char*
__root_PG_InterOp_CIM_Setting_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Core::Settings",
    /* Abstract */
    "\002\001",
    /* Version */
    "\0632.8.1000",
    0,
};

MetaClass
__root_PG_InterOp_CIM_Setting =
{
    /* flags */
    META_FLAG_CLASS|META_FLAG_ABSTRACT,
    /* name */
    "CIM_Setting",
    /* qualifiers */
    __root_PG_InterOp_CIM_Setting_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_ManagedElement,
    /* features */
    _CIM_Setting_features,
};

//==============================================================================
//
// Class: CIM_SettingContext
//
//==============================================================================

static const char*
_CIM_SettingContext_Context_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Aggregate */
    "\003\001",
    0,
};

static MetaReference
_CIM_SettingContext_Context =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_AGGREGATE|META_FLAG_READ,
    /* name */
    "Context",
    /* qualifiers */
    _CIM_SettingContext_Context_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_Configuration,
};

static const char*
_CIM_SettingContext_Setting_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_SettingContext_Setting =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Setting",
    /* qualifiers */
    _CIM_SettingContext_Setting_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_Setting,
};

static MetaFeature*
_CIM_SettingContext_features[] =
{
    (MetaFeature*)&_CIM_SettingContext_Context,
    (MetaFeature*)&_CIM_SettingContext_Setting,
    0,
};

static const char*
__root_PG_InterOp_CIM_SettingContext_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::Settings",
    /* Aggregation */
    "\004\001",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_SettingContext =
{
    /* flags */
    META_FLAG_ASSOCIATION|META_FLAG_AGGREGATION,
    /* name */
    "CIM_SettingContext",
    /* qualifiers */
    __root_PG_InterOp_CIM_SettingContext_qualifiers,
    /* super */
    0,
    /* features */
    _CIM_SettingContext_features,
};

//==============================================================================
//
// Class: CIM_SettingData
//
//==============================================================================

static const char*
_CIM_SettingData_InstanceID_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaProperty
_CIM_SettingData_InstanceID =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "InstanceID",
    /* qualifiers */
    _CIM_SettingData_InstanceID_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_SettingData_ElementName_qualifiers[] =
{
    /* Required */
    "\050\001",
    /* Override */
    "\044ElementName",
    0,
};

static MetaProperty
_CIM_SettingData_ElementName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "ElementName",
    /* qualifiers */
    _CIM_SettingData_ElementName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_SettingData_features[] =
{
    (MetaFeature*)&_CIM_SettingData_InstanceID,
    (MetaFeature*)&_CIM_SettingData_ElementName,
    0,
};

static const char*
__root_PG_InterOp_CIM_SettingData_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Core::Settings",
    /* Abstract */
    "\002\001",
    /* Version */
    "\0632.10.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_SettingData =
{
    /* flags */
    META_FLAG_CLASS|META_FLAG_ABSTRACT,
    /* name */
    "CIM_SettingData",
    /* qualifiers */
    __root_PG_InterOp_CIM_SettingData_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_ManagedElement,
    /* features */
    _CIM_SettingData_features,
};

//==============================================================================
//
// Class: CIM_Capabilities
//
//==============================================================================

static const char*
_CIM_Capabilities_InstanceID_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaProperty
_CIM_Capabilities_InstanceID =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "InstanceID",
    /* qualifiers */
    _CIM_Capabilities_InstanceID_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Capabilities_ElementName_qualifiers[] =
{
    /* Required */
    "\050\001",
    /* Override */
    "\044ElementName",
    0,
};

static MetaProperty
_CIM_Capabilities_ElementName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "ElementName",
    /* qualifiers */
    _CIM_Capabilities_ElementName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_Capabilities_features[] =
{
    (MetaFeature*)&_CIM_Capabilities_InstanceID,
    (MetaFeature*)&_CIM_Capabilities_ElementName,
    0,
};

static const char*
__root_PG_InterOp_CIM_Capabilities_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Core::Capabilities",
    /* Abstract */
    "\002\001",
    /* Version */
    "\0632.7.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_Capabilities =
{
    /* flags */
    META_FLAG_CLASS|META_FLAG_ABSTRACT,
    /* name */
    "CIM_Capabilities",
    /* qualifiers */
    __root_PG_InterOp_CIM_Capabilities_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_ManagedElement,
    /* features */
    _CIM_Capabilities_features,
};

//==============================================================================
//
// Class: CIM_Collection
//
//==============================================================================

static MetaFeature*
_CIM_Collection_features[] =
{
    0,
};

static const char*
__root_PG_InterOp_CIM_Collection_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Core::Collection",
    /* Abstract */
    "\002\001",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_Collection =
{
    /* flags */
    META_FLAG_CLASS|META_FLAG_ABSTRACT,
    /* name */
    "CIM_Collection",
    /* qualifiers */
    __root_PG_InterOp_CIM_Collection_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_ManagedElement,
    /* features */
    _CIM_Collection_features,
};

//==============================================================================
//
// Class: CIM_CollectionOfMSEs
//
//==============================================================================

static const char*
_CIM_CollectionOfMSEs_CollectionID_qualifiers[] =
{
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_CollectionOfMSEs_CollectionID =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "CollectionID",
    /* qualifiers */
    _CIM_CollectionOfMSEs_CollectionID_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_CollectionOfMSEs_features[] =
{
    (MetaFeature*)&_CIM_CollectionOfMSEs_CollectionID,
    0,
};

static const char*
__root_PG_InterOp_CIM_CollectionOfMSEs_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Core::Collection",
    /* Abstract */
    "\002\001",
    /* Version */
    "\0632.10.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_CollectionOfMSEs =
{
    /* flags */
    META_FLAG_CLASS|META_FLAG_ABSTRACT,
    /* name */
    "CIM_CollectionOfMSEs",
    /* qualifiers */
    __root_PG_InterOp_CIM_CollectionOfMSEs_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_Collection,
    /* features */
    _CIM_CollectionOfMSEs_features,
};

//==============================================================================
//
// Class: CIM_Component
//
//==============================================================================

static const char*
_CIM_Component_GroupComponent_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Aggregate */
    "\003\001",
    0,
};

static MetaReference
_CIM_Component_GroupComponent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_AGGREGATE|META_FLAG_READ,
    /* name */
    "GroupComponent",
    /* qualifiers */
    _CIM_Component_GroupComponent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ManagedElement,
};

static const char*
_CIM_Component_PartComponent_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_Component_PartComponent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "PartComponent",
    /* qualifiers */
    _CIM_Component_PartComponent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ManagedElement,
};

static MetaFeature*
_CIM_Component_features[] =
{
    (MetaFeature*)&_CIM_Component_GroupComponent,
    (MetaFeature*)&_CIM_Component_PartComponent,
    0,
};

static const char*
__root_PG_InterOp_CIM_Component_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::CoreElements",
    /* Abstract */
    "\002\001",
    /* Aggregation */
    "\004\001",
    /* Version */
    "\0632.7.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_Component =
{
    /* flags */
    META_FLAG_ASSOCIATION|META_FLAG_ABSTRACT|META_FLAG_AGGREGATION,
    /* name */
    "CIM_Component",
    /* qualifiers */
    __root_PG_InterOp_CIM_Component_qualifiers,
    /* super */
    0,
    /* features */
    _CIM_Component_features,
};

//==============================================================================
//
// Class: CIM_ConcreteCollection
//
//==============================================================================

static const char*
_CIM_ConcreteCollection_InstanceID_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaProperty
_CIM_ConcreteCollection_InstanceID =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "InstanceID",
    /* qualifiers */
    _CIM_ConcreteCollection_InstanceID_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_ConcreteCollection_features[] =
{
    (MetaFeature*)&_CIM_ConcreteCollection_InstanceID,
    0,
};

static const char*
__root_PG_InterOp_CIM_ConcreteCollection_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Core::Collection",
    /* Version */
    "\0632.10.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ConcreteCollection =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "CIM_ConcreteCollection",
    /* qualifiers */
    __root_PG_InterOp_CIM_ConcreteCollection_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_Collection,
    /* features */
    _CIM_ConcreteCollection_features,
};

//==============================================================================
//
// Class: CIM_ConcreteComponent
//
//==============================================================================

static const char*
_CIM_ConcreteComponent_GroupComponent_qualifiers[] =
{
    /* Aggregate */
    "\003\001",
    /* Override */
    "\044GroupComponent",
    0,
};

static MetaReference
_CIM_ConcreteComponent_GroupComponent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_AGGREGATE|META_FLAG_READ,
    /* name */
    "GroupComponent",
    /* qualifiers */
    _CIM_ConcreteComponent_GroupComponent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ManagedElement,
};

static const char*
_CIM_ConcreteComponent_PartComponent_qualifiers[] =
{
    /* Override */
    "\044PartComponent",
    0,
};

static MetaReference
_CIM_ConcreteComponent_PartComponent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "PartComponent",
    /* qualifiers */
    _CIM_ConcreteComponent_PartComponent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ManagedElement,
};

static MetaFeature*
_CIM_ConcreteComponent_features[] =
{
    (MetaFeature*)&_CIM_ConcreteComponent_GroupComponent,
    (MetaFeature*)&_CIM_ConcreteComponent_PartComponent,
    0,
};

static const char*
__root_PG_InterOp_CIM_ConcreteComponent_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::CoreElements",
    /* Aggregation */
    "\004\001",
    /* Version */
    "\0632.10.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ConcreteComponent =
{
    /* flags */
    META_FLAG_ASSOCIATION|META_FLAG_AGGREGATION,
    /* name */
    "CIM_ConcreteComponent",
    /* qualifiers */
    __root_PG_InterOp_CIM_ConcreteComponent_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_Component,
    /* features */
    _CIM_ConcreteComponent_features,
};

//==============================================================================
//
// Class: CIM_Configuration
//
//==============================================================================

static const char*
_CIM_Configuration_Name_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_Configuration_Name =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Name",
    /* qualifiers */
    _CIM_Configuration_Name_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_Configuration_features[] =
{
    (MetaFeature*)&_CIM_Configuration_Name,
    0,
};

static const char*
__root_PG_InterOp_CIM_Configuration_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Core::Settings",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_Configuration =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "CIM_Configuration",
    /* qualifiers */
    __root_PG_InterOp_CIM_Configuration_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_ManagedElement,
    /* features */
    _CIM_Configuration_features,
};

//==============================================================================
//
// Class: CIM_ConfigurationComponent
//
//==============================================================================

static const char*
_CIM_ConfigurationComponent_ConfigGroup_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Aggregate */
    "\003\001",
    0,
};

static MetaReference
_CIM_ConfigurationComponent_ConfigGroup =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_AGGREGATE|META_FLAG_READ,
    /* name */
    "ConfigGroup",
    /* qualifiers */
    _CIM_ConfigurationComponent_ConfigGroup_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_Configuration,
};

static const char*
_CIM_ConfigurationComponent_ConfigComponent_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_ConfigurationComponent_ConfigComponent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "ConfigComponent",
    /* qualifiers */
    _CIM_ConfigurationComponent_ConfigComponent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_Configuration,
};

static MetaFeature*
_CIM_ConfigurationComponent_features[] =
{
    (MetaFeature*)&_CIM_ConfigurationComponent_ConfigGroup,
    (MetaFeature*)&_CIM_ConfigurationComponent_ConfigComponent,
    0,
};

static const char*
__root_PG_InterOp_CIM_ConfigurationComponent_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::Settings",
    /* Aggregation */
    "\004\001",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ConfigurationComponent =
{
    /* flags */
    META_FLAG_ASSOCIATION|META_FLAG_AGGREGATION,
    /* name */
    "CIM_ConfigurationComponent",
    /* qualifiers */
    __root_PG_InterOp_CIM_ConfigurationComponent_qualifiers,
    /* super */
    0,
    /* features */
    _CIM_ConfigurationComponent_features,
};

//==============================================================================
//
// Class: CIM_Dependency
//
//==============================================================================

static const char*
_CIM_Dependency_Antecedent_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_Dependency_Antecedent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Antecedent",
    /* qualifiers */
    _CIM_Dependency_Antecedent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ManagedElement,
};

static const char*
_CIM_Dependency_Dependent_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_Dependency_Dependent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Dependent",
    /* qualifiers */
    _CIM_Dependency_Dependent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ManagedElement,
};

static MetaFeature*
_CIM_Dependency_features[] =
{
    (MetaFeature*)&_CIM_Dependency_Antecedent,
    (MetaFeature*)&_CIM_Dependency_Dependent,
    0,
};

static const char*
__root_PG_InterOp_CIM_Dependency_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::CoreElements",
    /* Abstract */
    "\002\001",
    /* Version */
    "\0632.10.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_Dependency =
{
    /* flags */
    META_FLAG_ASSOCIATION|META_FLAG_ABSTRACT,
    /* name */
    "CIM_Dependency",
    /* qualifiers */
    __root_PG_InterOp_CIM_Dependency_qualifiers,
    /* super */
    0,
    /* features */
    _CIM_Dependency_features,
};

//==============================================================================
//
// Class: CIM_DependencyContext
//
//==============================================================================

static const char*
_CIM_DependencyContext_Context_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Aggregate */
    "\003\001",
    0,
};

static MetaReference
_CIM_DependencyContext_Context =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_AGGREGATE|META_FLAG_READ,
    /* name */
    "Context",
    /* qualifiers */
    _CIM_DependencyContext_Context_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_Configuration,
};

static const char*
_CIM_DependencyContext_Dependency_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_DependencyContext_Dependency =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Dependency",
    /* qualifiers */
    _CIM_DependencyContext_Dependency_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_Dependency,
};

static MetaFeature*
_CIM_DependencyContext_features[] =
{
    (MetaFeature*)&_CIM_DependencyContext_Context,
    (MetaFeature*)&_CIM_DependencyContext_Dependency,
    0,
};

static const char*
__root_PG_InterOp_CIM_DependencyContext_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::Settings",
    /* Aggregation */
    "\004\001",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_DependencyContext =
{
    /* flags */
    META_FLAG_ASSOCIATION|META_FLAG_AGGREGATION,
    /* name */
    "CIM_DependencyContext",
    /* qualifiers */
    __root_PG_InterOp_CIM_DependencyContext_qualifiers,
    /* super */
    0,
    /* features */
    _CIM_DependencyContext_features,
};

//==============================================================================
//
// Class: CIM_ElementCapabilities
//
//==============================================================================

static const char*
_CIM_ElementCapabilities_ManagedElement_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Min */
    "\033\000\000\000\001",
    /* Max */
    "\027\000\000\000\001",
    0,
};

static MetaReference
_CIM_ElementCapabilities_ManagedElement =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "ManagedElement",
    /* qualifiers */
    _CIM_ElementCapabilities_ManagedElement_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ManagedElement,
};

static const char*
_CIM_ElementCapabilities_Capabilities_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_ElementCapabilities_Capabilities =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Capabilities",
    /* qualifiers */
    _CIM_ElementCapabilities_Capabilities_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_Capabilities,
};

static MetaFeature*
_CIM_ElementCapabilities_features[] =
{
    (MetaFeature*)&_CIM_ElementCapabilities_ManagedElement,
    (MetaFeature*)&_CIM_ElementCapabilities_Capabilities,
    0,
};

static const char*
__root_PG_InterOp_CIM_ElementCapabilities_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::Capabilities",
    /* Version */
    "\0632.7.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ElementCapabilities =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_ElementCapabilities",
    /* qualifiers */
    __root_PG_InterOp_CIM_ElementCapabilities_qualifiers,
    /* super */
    0,
    /* features */
    _CIM_ElementCapabilities_features,
};

//==============================================================================
//
// Class: CIM_EnabledLogicalElementCapabilities
//
//==============================================================================

static const char*
_CIM_EnabledLogicalElementCapabilities_ElementNameEditSupported_qualifiers[] =
{
    /* MappingStrings */
    "\026\000\001FC-SWAPI.INCITS-T11|SWAPI_UNIT_CONFIG_CAPS_T|EditName\000",
    /* ModelCorrespondence */
    "\036\000\001CIM_ManagedElement.ElementName\000",
    0,
};

static MetaProperty
_CIM_EnabledLogicalElementCapabilities_ElementNameEditSupported =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "ElementNameEditSupported",
    /* qualifiers */
    _CIM_EnabledLogicalElementCapabilities_ElementNameEditSupported_qualifiers,
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_EnabledLogicalElementCapabilities_MaxElementNameLen_qualifiers[] =
{
    /* MaxValue */
    "\031\000\000\000\000\000\000\001\000",
    /* MappingStrings */
    "\026\000\001FC-SWAPI.INCITS-T11|SWAPI_UNIT_CONFIG_CAPS_T|MaxNameChars\000",
    /* ModelCorrespondence */
    "\036\000\001CIM_FCSwitchCapabilities.ElementNameEditSupported\000",
    0,
};

static MetaProperty
_CIM_EnabledLogicalElementCapabilities_MaxElementNameLen =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "MaxElementNameLen",
    /* qualifiers */
    _CIM_EnabledLogicalElementCapabilities_MaxElementNameLen_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_EnabledLogicalElementCapabilities_RequestedStatesSupported_qualifiers[] =
{
    /* ValueMap */
    "\061\000\0112\0003\0004\0006\0007\0008\0009\00010\00011\000",
    /* Values */
    "\062\000\011Enabled\000Disabled\000Shut Down\000Offline\000Test\000Defer\000Quiesce\000Reboot\000Reset\000",
    /* ModelCorrespondence */
    "\036\000\001CIM_EnabledLogicalElement.RequestStateChange\000",
    0,
};

static MetaProperty
_CIM_EnabledLogicalElementCapabilities_RequestedStatesSupported =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "RequestedStatesSupported",
    /* qualifiers */
    _CIM_EnabledLogicalElementCapabilities_RequestedStatesSupported_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    0,
    /* value */
    0,
};

static MetaFeature*
_CIM_EnabledLogicalElementCapabilities_features[] =
{
    (MetaFeature*)&_CIM_EnabledLogicalElementCapabilities_ElementNameEditSupported,
    (MetaFeature*)&_CIM_EnabledLogicalElementCapabilities_MaxElementNameLen,
    (MetaFeature*)&_CIM_EnabledLogicalElementCapabilities_RequestedStatesSupported,
    0,
};

static const char*
__root_PG_InterOp_CIM_EnabledLogicalElementCapabilities_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Core::Capabilities",
    /* Version */
    "\0632.11.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_EnabledLogicalElementCapabilities =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "CIM_EnabledLogicalElementCapabilities",
    /* qualifiers */
    __root_PG_InterOp_CIM_EnabledLogicalElementCapabilities_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_Capabilities,
    /* features */
    _CIM_EnabledLogicalElementCapabilities_features,
};

//==============================================================================
//
// Class: CIM_FRU
//
//==============================================================================

static const char*
_CIM_FRU_FRUNumber_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* MaxLen */
    "\030\000\000\000\100",
    /* MappingStrings */
    "\026\000\001MIF.DMTF|FRU|002.6\000",
    0,
};

static MetaProperty
_CIM_FRU_FRUNumber =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "FRUNumber",
    /* qualifiers */
    _CIM_FRU_FRUNumber_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_FRU_IdentifyingNumber_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* MaxLen */
    "\030\000\000\000\100",
    /* MappingStrings */
    "\026\000\001MIF.DMTF|FRU|002.7\000",
    0,
};

static MetaProperty
_CIM_FRU_IdentifyingNumber =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "IdentifyingNumber",
    /* qualifiers */
    _CIM_FRU_IdentifyingNumber_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_FRU_Vendor_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* MaxLen */
    "\030\000\000\001\000",
    /* MappingStrings */
    "\026\000\001MIF.DMTF|FRU|002.4\000",
    0,
};

static MetaProperty
_CIM_FRU_Vendor =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Vendor",
    /* qualifiers */
    _CIM_FRU_Vendor_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_FRU_Description_qualifiers[] =
{
    /* Override */
    "\044Description",
    /* MappingStrings */
    "\026\000\001MIF.DMTF|FRU|002.3\000",
    0,
};

static MetaProperty
_CIM_FRU_Description =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "Description",
    /* qualifiers */
    _CIM_FRU_Description_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_FRU_Name_qualifiers[] =
{
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_FRU_Name =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "Name",
    /* qualifiers */
    _CIM_FRU_Name_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_FRU_RevisionLevel_qualifiers[] =
{
    /* MaxLen */
    "\030\000\000\000\100",
    /* MappingStrings */
    "\026\000\001MIF.DMTF|FRU|002.8\000",
    0,
};

static MetaProperty
_CIM_FRU_RevisionLevel =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "RevisionLevel",
    /* qualifiers */
    _CIM_FRU_RevisionLevel_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_FRU_CustomerReplaceable_qualifiers[] =
{
    0,
};

static MetaProperty
_CIM_FRU_CustomerReplaceable =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "CustomerReplaceable",
    /* qualifiers */
    _CIM_FRU_CustomerReplaceable_qualifiers,
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_FRU_features[] =
{
    (MetaFeature*)&_CIM_FRU_FRUNumber,
    (MetaFeature*)&_CIM_FRU_IdentifyingNumber,
    (MetaFeature*)&_CIM_FRU_Vendor,
    (MetaFeature*)&_CIM_FRU_Description,
    (MetaFeature*)&_CIM_FRU_Name,
    (MetaFeature*)&_CIM_FRU_RevisionLevel,
    (MetaFeature*)&_CIM_FRU_CustomerReplaceable,
    0,
};

static const char*
__root_PG_InterOp_CIM_FRU_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Core::ProductFRU",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_FRU =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "CIM_FRU",
    /* qualifiers */
    __root_PG_InterOp_CIM_FRU_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_ManagedElement,
    /* features */
    _CIM_FRU_features,
};

//==============================================================================
//
// Class: CIM_HostedDependency
//
//==============================================================================

static const char*
_CIM_HostedDependency_Antecedent_qualifiers[] =
{
    /* Override */
    "\044Antecedent",
    /* Max */
    "\027\000\000\000\001",
    0,
};

static MetaReference
_CIM_HostedDependency_Antecedent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Antecedent",
    /* qualifiers */
    _CIM_HostedDependency_Antecedent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ManagedElement,
};

static const char*
_CIM_HostedDependency_Dependent_qualifiers[] =
{
    /* Override */
    "\044Dependent",
    0,
};

static MetaReference
_CIM_HostedDependency_Dependent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Dependent",
    /* qualifiers */
    _CIM_HostedDependency_Dependent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ManagedElement,
};

static MetaFeature*
_CIM_HostedDependency_features[] =
{
    (MetaFeature*)&_CIM_HostedDependency_Antecedent,
    (MetaFeature*)&_CIM_HostedDependency_Dependent,
    0,
};

static const char*
__root_PG_InterOp_CIM_HostedDependency_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::CoreElements",
    /* Version */
    "\0632.8.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_HostedDependency =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_HostedDependency",
    /* qualifiers */
    __root_PG_InterOp_CIM_HostedDependency_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_Dependency,
    /* features */
    _CIM_HostedDependency_features,
};

//==============================================================================
//
// Class: CIM_LocalizationCapabilities
//
//==============================================================================

static const char*
_CIM_LocalizationCapabilities_SupportedInputLocales_qualifiers[] =
{
    0,
};

static MetaProperty
_CIM_LocalizationCapabilities_SupportedInputLocales =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "SupportedInputLocales",
    /* qualifiers */
    _CIM_LocalizationCapabilities_SupportedInputLocales_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* value */
    0,
};

static const char*
_CIM_LocalizationCapabilities_SupportedOutputLocales_qualifiers[] =
{
    0,
};

static MetaProperty
_CIM_LocalizationCapabilities_SupportedOutputLocales =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "SupportedOutputLocales",
    /* qualifiers */
    _CIM_LocalizationCapabilities_SupportedOutputLocales_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* value */
    0,
};

static MetaFeature*
_CIM_LocalizationCapabilities_features[] =
{
    (MetaFeature*)&_CIM_LocalizationCapabilities_SupportedInputLocales,
    (MetaFeature*)&_CIM_LocalizationCapabilities_SupportedOutputLocales,
    0,
};

static const char*
__root_PG_InterOp_CIM_LocalizationCapabilities_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Core::Capabilities",
    /* Version */
    "\0632.9.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_LocalizationCapabilities =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "CIM_LocalizationCapabilities",
    /* qualifiers */
    __root_PG_InterOp_CIM_LocalizationCapabilities_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_Capabilities,
    /* features */
    _CIM_LocalizationCapabilities_features,
};

//==============================================================================
//
// Class: CIM_Location
//
//==============================================================================

static const char*
_CIM_Location_Name_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_Location_Name =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Name",
    /* qualifiers */
    _CIM_Location_Name_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Location_PhysicalPosition_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_Location_PhysicalPosition =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "PhysicalPosition",
    /* qualifiers */
    _CIM_Location_PhysicalPosition_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Location_Address_qualifiers[] =
{
    /* MaxLen */
    "\030\000\000\004\000",
    0,
};

static MetaProperty
_CIM_Location_Address =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "Address",
    /* qualifiers */
    _CIM_Location_Address_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_Location_features[] =
{
    (MetaFeature*)&_CIM_Location_Name,
    (MetaFeature*)&_CIM_Location_PhysicalPosition,
    (MetaFeature*)&_CIM_Location_Address,
    0,
};

static const char*
__root_PG_InterOp_CIM_Location_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Core::Physical",
    /* Version */
    "\0632.10.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_Location =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "CIM_Location",
    /* qualifiers */
    __root_PG_InterOp_CIM_Location_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_ManagedElement,
    /* features */
    _CIM_Location_features,
};

//==============================================================================
//
// Class: CIM_LogicalIdentity
//
//==============================================================================

static const char*
_CIM_LogicalIdentity_SystemElement_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_LogicalIdentity_SystemElement =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "SystemElement",
    /* qualifiers */
    _CIM_LogicalIdentity_SystemElement_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ManagedElement,
};

static const char*
_CIM_LogicalIdentity_SameElement_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_LogicalIdentity_SameElement =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "SameElement",
    /* qualifiers */
    _CIM_LogicalIdentity_SameElement_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ManagedElement,
};

static MetaFeature*
_CIM_LogicalIdentity_features[] =
{
    (MetaFeature*)&_CIM_LogicalIdentity_SystemElement,
    (MetaFeature*)&_CIM_LogicalIdentity_SameElement,
    0,
};

static const char*
__root_PG_InterOp_CIM_LogicalIdentity_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::CoreElements",
    /* Abstract */
    "\002\001",
    /* Version */
    "\0632.10.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_LogicalIdentity =
{
    /* flags */
    META_FLAG_ASSOCIATION|META_FLAG_ABSTRACT,
    /* name */
    "CIM_LogicalIdentity",
    /* qualifiers */
    __root_PG_InterOp_CIM_LogicalIdentity_qualifiers,
    /* super */
    0,
    /* features */
    _CIM_LogicalIdentity_features,
};

//==============================================================================
//
// Class: CIM_ManagedSystemElement
//
//==============================================================================

static const char*
_CIM_ManagedSystemElement_InstallDate_qualifiers[] =
{
    /* MappingStrings */
    "\026\000\001MIF.DMTF|ComponentID|001.5\000",
    0,
};

static MetaProperty
_CIM_ManagedSystemElement_InstallDate =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "InstallDate",
    /* qualifiers */
    _CIM_ManagedSystemElement_InstallDate_qualifiers,
    /* type */
    CIMTYPE_DATETIME,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_ManagedSystemElement_Name_qualifiers[] =
{
    /* MaxLen */
    "\030\000\000\004\000",
    0,
};

static MetaProperty
_CIM_ManagedSystemElement_Name =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "Name",
    /* qualifiers */
    _CIM_ManagedSystemElement_Name_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_ManagedSystemElement_OperationalStatus_qualifiers[] =
{
    /* ValueMap */
    "\061\000\0250\0001\0002\0003\0004\0005\0006\0007\0008\0009\00010\00011\00012\00013\00014\00015\00016\00017\00018\000..\0000x8000..\000",
    /* Values */
    "\062\000\025Unknown\000Other\000OK\000Degraded\000Stressed\000Predictive Failure\000Error\000Non-Recoverable Error\000Starting\000Stopping\000Stopped\000In Service\000No Contact\000Lost Communication\000Aborted\000Dormant\000Supporting Entity in Error\000Completed\000Power Mode\000DMTF Reserved\000Vendor Reserved\000",
    /* ArrayType */
    "\005Indexed",
    /* ModelCorrespondence */
    "\036\000\001CIM_ManagedSystemElement.StatusDescriptions\000",
    0,
};

static MetaProperty
_CIM_ManagedSystemElement_OperationalStatus =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "OperationalStatus",
    /* qualifiers */
    _CIM_ManagedSystemElement_OperationalStatus_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    0,
    /* value */
    0,
};

static const char*
_CIM_ManagedSystemElement_StatusDescriptions_qualifiers[] =
{
    /* ArrayType */
    "\005Indexed",
    /* ModelCorrespondence */
    "\036\000\001CIM_ManagedSystemElement.OperationalStatus\000",
    0,
};

static MetaProperty
_CIM_ManagedSystemElement_StatusDescriptions =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "StatusDescriptions",
    /* qualifiers */
    _CIM_ManagedSystemElement_StatusDescriptions_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* value */
    0,
};

static const char*
_CIM_ManagedSystemElement_Status_qualifiers[] =
{
    /* Deprecated */
    "\013\000\001CIM_ManagedSystemElement.OperationalStatus\000",
    /* ValueMap */
    "\061\000\015OK\000Error\000Degraded\000Unknown\000Pred Fail\000Starting\000Stopping\000Service\000Stressed\000NonRecover\000No Contact\000Lost Comm\000Stopped\000",
    /* MaxLen */
    "\030\000\000\000\012",
    0,
};

static MetaProperty
_CIM_ManagedSystemElement_Status =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "Status",
    /* qualifiers */
    _CIM_ManagedSystemElement_Status_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_ManagedSystemElement_HealthState_qualifiers[] =
{
    /* ValueMap */
    "\061\000\0100\0005\00010\00015\00020\00025\00030\000..\000",
    /* Values */
    "\062\000\010Unknown\000OK\000Degraded/Warning\000Minor failure\000Major failure\000Critical failure\000Non-recoverable error\000DMTF Reserved\000",
    0,
};

static MetaProperty
_CIM_ManagedSystemElement_HealthState =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "HealthState",
    /* qualifiers */
    _CIM_ManagedSystemElement_HealthState_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_ManagedSystemElement_features[] =
{
    (MetaFeature*)&_CIM_ManagedSystemElement_InstallDate,
    (MetaFeature*)&_CIM_ManagedSystemElement_Name,
    (MetaFeature*)&_CIM_ManagedSystemElement_OperationalStatus,
    (MetaFeature*)&_CIM_ManagedSystemElement_StatusDescriptions,
    (MetaFeature*)&_CIM_ManagedSystemElement_Status,
    (MetaFeature*)&_CIM_ManagedSystemElement_HealthState,
    0,
};

static const char*
__root_PG_InterOp_CIM_ManagedSystemElement_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Core::CoreElements",
    /* Abstract */
    "\002\001",
    /* Version */
    "\0632.11.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ManagedSystemElement =
{
    /* flags */
    META_FLAG_CLASS|META_FLAG_ABSTRACT,
    /* name */
    "CIM_ManagedSystemElement",
    /* qualifiers */
    __root_PG_InterOp_CIM_ManagedSystemElement_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_ManagedElement,
    /* features */
    _CIM_ManagedSystemElement_features,
};

//==============================================================================
//
// Class: CIM_MemberOfCollection
//
//==============================================================================

static const char*
_CIM_MemberOfCollection_Collection_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Aggregate */
    "\003\001",
    0,
};

static MetaReference
_CIM_MemberOfCollection_Collection =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_AGGREGATE|META_FLAG_READ,
    /* name */
    "Collection",
    /* qualifiers */
    _CIM_MemberOfCollection_Collection_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_Collection,
};

static const char*
_CIM_MemberOfCollection_Member_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_MemberOfCollection_Member =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Member",
    /* qualifiers */
    _CIM_MemberOfCollection_Member_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ManagedElement,
};

static MetaFeature*
_CIM_MemberOfCollection_features[] =
{
    (MetaFeature*)&_CIM_MemberOfCollection_Collection,
    (MetaFeature*)&_CIM_MemberOfCollection_Member,
    0,
};

static const char*
__root_PG_InterOp_CIM_MemberOfCollection_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::Collection",
    /* Aggregation */
    "\004\001",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_MemberOfCollection =
{
    /* flags */
    META_FLAG_ASSOCIATION|META_FLAG_AGGREGATION,
    /* name */
    "CIM_MemberOfCollection",
    /* qualifiers */
    __root_PG_InterOp_CIM_MemberOfCollection_qualifiers,
    /* super */
    0,
    /* features */
    _CIM_MemberOfCollection_features,
};

//==============================================================================
//
// Class: CIM_MethodParameters
//
//==============================================================================

static const char*
_CIM_MethodParameters_MethodParametersId_qualifiers[] =
{
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_MethodParameters_MethodParametersId =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "MethodParametersId",
    /* qualifiers */
    _CIM_MethodParameters_MethodParametersId_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_MethodParameters_ResetValues_SourceME_qualifiers[] =
{
    /* IN */
    "\024\001",
    0,
};

static MetaReference
_CIM_MethodParameters_ResetValues_SourceME =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_IN,
    /* name */
    "SourceME",
    /* qualifiers */
    _CIM_MethodParameters_ResetValues_SourceME_qualifiers,
    /* subscript */
    -1,
    /* ref */
    &__root_PG_InterOp_CIM_ManagedElement,
};

static MetaFeature*
_CIM_MethodParameters_ResetValues_parameters[] =
{
    (MetaFeature*)&_CIM_MethodParameters_ResetValues_SourceME,
    0,
};

static const char*
_CIM_MethodParameters_ResetValues_qualifiers[] =
{
    0,
};

static MetaMethod
_CIM_MethodParameters_ResetValues =
{
    /* flags */
    META_FLAG_METHOD,
    /* name */
    "CIM_MethodParameters",
    /* qualifiers */
    _CIM_MethodParameters_ResetValues_qualifiers,
    /* type */
    CIMTYPE_UINT32,
    /* parameters */
    _CIM_MethodParameters_ResetValues_parameters,
};

static MetaFeature*
_CIM_MethodParameters_features[] =
{
    (MetaFeature*)&_CIM_MethodParameters_MethodParametersId,
    (MetaFeature*)&_CIM_MethodParameters_ResetValues,
    0,
};

static const char*
__root_PG_InterOp_CIM_MethodParameters_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Core::MethodParms",
    /* Abstract */
    "\002\001",
    /* Version */
    "\0632.10.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_MethodParameters =
{
    /* flags */
    META_FLAG_CLASS|META_FLAG_ABSTRACT,
    /* name */
    "CIM_MethodParameters",
    /* qualifiers */
    __root_PG_InterOp_CIM_MethodParameters_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_ManagedElement,
    /* features */
    _CIM_MethodParameters_features,
};

//==============================================================================
//
// Class: CIM_OrderedMemberOfCollection
//
//==============================================================================

static const char*
_CIM_OrderedMemberOfCollection_AssignedSequence_qualifiers[] =
{
    0,
};

static MetaProperty
_CIM_OrderedMemberOfCollection_AssignedSequence =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "AssignedSequence",
    /* qualifiers */
    _CIM_OrderedMemberOfCollection_AssignedSequence_qualifiers,
    /* type */
    CIMTYPE_UINT64,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_OrderedMemberOfCollection_features[] =
{
    (MetaFeature*)&_CIM_OrderedMemberOfCollection_AssignedSequence,
    0,
};

static const char*
__root_PG_InterOp_CIM_OrderedMemberOfCollection_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::Collection",
    /* Aggregation */
    "\004\001",
    /* Version */
    "\0632.10.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_OrderedMemberOfCollection =
{
    /* flags */
    META_FLAG_ASSOCIATION|META_FLAG_AGGREGATION,
    /* name */
    "CIM_OrderedMemberOfCollection",
    /* qualifiers */
    __root_PG_InterOp_CIM_OrderedMemberOfCollection_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_MemberOfCollection,
    /* features */
    _CIM_OrderedMemberOfCollection_features,
};

//==============================================================================
//
// Class: CIM_ParametersForMethod
//
//==============================================================================

static const char*
_CIM_ParametersForMethod_Parameters_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_ParametersForMethod_Parameters =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Parameters",
    /* qualifiers */
    _CIM_ParametersForMethod_Parameters_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_MethodParameters,
};

static const char*
_CIM_ParametersForMethod_TheMethod_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_ParametersForMethod_TheMethod =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "TheMethod",
    /* qualifiers */
    _CIM_ParametersForMethod_TheMethod_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ManagedElement,
};

static const char*
_CIM_ParametersForMethod_MethodName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* MaxLen */
    "\030\000\000\000\100",
    0,
};

static MetaProperty
_CIM_ParametersForMethod_MethodName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "MethodName",
    /* qualifiers */
    _CIM_ParametersForMethod_MethodName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_ParametersForMethod_features[] =
{
    (MetaFeature*)&_CIM_ParametersForMethod_Parameters,
    (MetaFeature*)&_CIM_ParametersForMethod_TheMethod,
    (MetaFeature*)&_CIM_ParametersForMethod_MethodName,
    0,
};

static const char*
__root_PG_InterOp_CIM_ParametersForMethod_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::MethodParms",
    /* Version */
    "\0632.10.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ParametersForMethod =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_ParametersForMethod",
    /* qualifiers */
    __root_PG_InterOp_CIM_ParametersForMethod_qualifiers,
    /* super */
    0,
    /* features */
    _CIM_ParametersForMethod_features,
};

//==============================================================================
//
// Class: CIM_ParameterValueSources
//
//==============================================================================

static const char*
_CIM_ParameterValueSources_Parameters_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_ParameterValueSources_Parameters =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Parameters",
    /* qualifiers */
    _CIM_ParameterValueSources_Parameters_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_MethodParameters,
};

static const char*
_CIM_ParameterValueSources_ValueSource_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_ParameterValueSources_ValueSource =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "ValueSource",
    /* qualifiers */
    _CIM_ParameterValueSources_ValueSource_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ManagedElement,
};

static MetaFeature*
_CIM_ParameterValueSources_features[] =
{
    (MetaFeature*)&_CIM_ParameterValueSources_Parameters,
    (MetaFeature*)&_CIM_ParameterValueSources_ValueSource,
    0,
};

static const char*
__root_PG_InterOp_CIM_ParameterValueSources_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::MethodParms",
    /* Version */
    "\0632.10.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ParameterValueSources =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_ParameterValueSources",
    /* qualifiers */
    __root_PG_InterOp_CIM_ParameterValueSources_qualifiers,
    /* super */
    0,
    /* features */
    _CIM_ParameterValueSources_features,
};

//==============================================================================
//
// Class: CIM_PhysicalElement
//
//==============================================================================

static const char*
_CIM_PhysicalElement_Tag_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_PhysicalElement_Tag =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Tag",
    /* qualifiers */
    _CIM_PhysicalElement_Tag_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_PhysicalElement_Description_qualifiers[] =
{
    /* Override */
    "\044Description",
    /* MappingStrings */
    "\026\000\001MIF.DMTF|FRU|003.3\000",
    0,
};

static MetaProperty
_CIM_PhysicalElement_Description =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "Description",
    /* qualifiers */
    _CIM_PhysicalElement_Description_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_PhysicalElement_CreationClassName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_PhysicalElement_CreationClassName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "CreationClassName",
    /* qualifiers */
    _CIM_PhysicalElement_CreationClassName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_PhysicalElement_ElementName_qualifiers[] =
{
    /* Override */
    "\044ElementName",
    /* MappingStrings */
    "\026\000\001MIB.IETF|Entity-MIB.entPhysicalName\000",
    0,
};

static MetaProperty
_CIM_PhysicalElement_ElementName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "ElementName",
    /* qualifiers */
    _CIM_PhysicalElement_ElementName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_PhysicalElement_Manufacturer_qualifiers[] =
{
    /* MaxLen */
    "\030\000\000\001\000",
    /* MappingStrings */
    "\026\000\002MIB.IETF|Entity-MIB.entPhysicalMfgName\000MIF.DMTF|FRU|003.4\000",
    0,
};

static MetaProperty
_CIM_PhysicalElement_Manufacturer =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "Manufacturer",
    /* qualifiers */
    _CIM_PhysicalElement_Manufacturer_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_PhysicalElement_Model_qualifiers[] =
{
    /* MaxLen */
    "\030\000\000\001\000",
    /* MappingStrings */
    "\026\000\002MIB.IETF|Entity-MIB.entPhysicalDescr\000MIF.DMTF|FRU|003.5\000",
    0,
};

static MetaProperty
_CIM_PhysicalElement_Model =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "Model",
    /* qualifiers */
    _CIM_PhysicalElement_Model_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_PhysicalElement_SKU_qualifiers[] =
{
    /* MaxLen */
    "\030\000\000\000\100",
    0,
};

static MetaProperty
_CIM_PhysicalElement_SKU =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "SKU",
    /* qualifiers */
    _CIM_PhysicalElement_SKU_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_PhysicalElement_SerialNumber_qualifiers[] =
{
    /* MaxLen */
    "\030\000\000\001\000",
    /* MappingStrings */
    "\026\000\002MIB.IETF|Entity-MIB.entPhysicalSerialNum\000MIF.DMTF|FRU|003.7\000",
    0,
};

static MetaProperty
_CIM_PhysicalElement_SerialNumber =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "SerialNumber",
    /* qualifiers */
    _CIM_PhysicalElement_SerialNumber_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_PhysicalElement_Version_qualifiers[] =
{
    /* MaxLen */
    "\030\000\000\000\100",
    /* MappingStrings */
    "\026\000\002MIB.IETF|Entity-MIB.entPhysicalHardwareRev\000MIF.DMTF|FRU|003.8\000",
    0,
};

static MetaProperty
_CIM_PhysicalElement_Version =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "Version",
    /* qualifiers */
    _CIM_PhysicalElement_Version_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_PhysicalElement_PartNumber_qualifiers[] =
{
    /* MaxLen */
    "\030\000\000\001\000",
    /* MappingStrings */
    "\026\000\001MIB.IETF|Entity-MIB.entPhysicalModelName\000",
    0,
};

static MetaProperty
_CIM_PhysicalElement_PartNumber =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "PartNumber",
    /* qualifiers */
    _CIM_PhysicalElement_PartNumber_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_PhysicalElement_OtherIdentifyingInfo_qualifiers[] =
{
    /* Write */
    "\065\001",
    /* MappingStrings */
    "\026\000\001MIB.IETF|Entity-MIB.entPhysicalAlias\000",
    0,
};

static MetaProperty
_CIM_PhysicalElement_OtherIdentifyingInfo =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_WRITE,
    /* name */
    "OtherIdentifyingInfo",
    /* qualifiers */
    _CIM_PhysicalElement_OtherIdentifyingInfo_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_PhysicalElement_PoweredOn_qualifiers[] =
{
    0,
};

static MetaProperty
_CIM_PhysicalElement_PoweredOn =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "PoweredOn",
    /* qualifiers */
    _CIM_PhysicalElement_PoweredOn_qualifiers,
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_PhysicalElement_ManufactureDate_qualifiers[] =
{
    0,
};

static MetaProperty
_CIM_PhysicalElement_ManufactureDate =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "ManufactureDate",
    /* qualifiers */
    _CIM_PhysicalElement_ManufactureDate_qualifiers,
    /* type */
    CIMTYPE_DATETIME,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_PhysicalElement_VendorEquipmentType_qualifiers[] =
{
    /* MappingStrings */
    "\026\000\001MIB.IETF|Entity-MIB.entPhysicalVendorType\000",
    0,
};

static MetaProperty
_CIM_PhysicalElement_VendorEquipmentType =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "VendorEquipmentType",
    /* qualifiers */
    _CIM_PhysicalElement_VendorEquipmentType_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_PhysicalElement_UserTracking_qualifiers[] =
{
    /* Write */
    "\065\001",
    /* MappingStrings */
    "\026\000\001MIB.IETF|Entity-MIB.entPhysicalAssetID\000",
    0,
};

static MetaProperty
_CIM_PhysicalElement_UserTracking =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_WRITE,
    /* name */
    "UserTracking",
    /* qualifiers */
    _CIM_PhysicalElement_UserTracking_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_PhysicalElement_CanBeFRUed_qualifiers[] =
{
    /* MappingStrings */
    "\026\000\001MIB.IETF|Entity-MIB.entPhysicalIsFRU\000",
    0,
};

static MetaProperty
_CIM_PhysicalElement_CanBeFRUed =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "CanBeFRUed",
    /* qualifiers */
    _CIM_PhysicalElement_CanBeFRUed_qualifiers,
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_PhysicalElement_features[] =
{
    (MetaFeature*)&_CIM_PhysicalElement_Tag,
    (MetaFeature*)&_CIM_PhysicalElement_Description,
    (MetaFeature*)&_CIM_PhysicalElement_CreationClassName,
    (MetaFeature*)&_CIM_PhysicalElement_ElementName,
    (MetaFeature*)&_CIM_PhysicalElement_Manufacturer,
    (MetaFeature*)&_CIM_PhysicalElement_Model,
    (MetaFeature*)&_CIM_PhysicalElement_SKU,
    (MetaFeature*)&_CIM_PhysicalElement_SerialNumber,
    (MetaFeature*)&_CIM_PhysicalElement_Version,
    (MetaFeature*)&_CIM_PhysicalElement_PartNumber,
    (MetaFeature*)&_CIM_PhysicalElement_OtherIdentifyingInfo,
    (MetaFeature*)&_CIM_PhysicalElement_PoweredOn,
    (MetaFeature*)&_CIM_PhysicalElement_ManufactureDate,
    (MetaFeature*)&_CIM_PhysicalElement_VendorEquipmentType,
    (MetaFeature*)&_CIM_PhysicalElement_UserTracking,
    (MetaFeature*)&_CIM_PhysicalElement_CanBeFRUed,
    0,
};

static const char*
__root_PG_InterOp_CIM_PhysicalElement_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Core::Physical",
    /* Abstract */
    "\002\001",
    /* Version */
    "\0632.10.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_PhysicalElement =
{
    /* flags */
    META_FLAG_CLASS|META_FLAG_ABSTRACT,
    /* name */
    "CIM_PhysicalElement",
    /* qualifiers */
    __root_PG_InterOp_CIM_PhysicalElement_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_ManagedSystemElement,
    /* features */
    _CIM_PhysicalElement_features,
};

//==============================================================================
//
// Class: CIM_PowerManagementCapabilities
//
//==============================================================================

static const char*
_CIM_PowerManagementCapabilities_PowerCapabilities_qualifiers[] =
{
    /* ValueMap */
    "\061\000\0060\0001\0002\0003\0004\0005\000",
    /* Values */
    "\062\000\006Unknown\000Other\000Power Saving Modes Entered Automatically\000Power State Settable\000Power Cycling Supported\000Timed Power On Supported\000",
    /* MappingStrings */
    "\026\000\001MIF.DMTF|System Power Controls|001.2\000",
    /* ModelCorrespondence */
    "\036\000\002CIM_PowerManagementCapabilities.OtherPowerCapabilitiesDescriptions\000CIM_PowerManagementService.SetPowerState\000",
    0,
};

static MetaProperty
_CIM_PowerManagementCapabilities_PowerCapabilities =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "PowerCapabilities",
    /* qualifiers */
    _CIM_PowerManagementCapabilities_PowerCapabilities_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    0,
    /* value */
    0,
};

static const char*
_CIM_PowerManagementCapabilities_OtherPowerCapabilitiesDescriptions_qualifiers[] =
{
    /* ModelCorrespondence */
    "\036\000\001CIM_PowerManagementCapabilities.PowerCapabilities\000",
    0,
};

static MetaProperty
_CIM_PowerManagementCapabilities_OtherPowerCapabilitiesDescriptions =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "OtherPowerCapabilitiesDescriptions",
    /* qualifiers */
    _CIM_PowerManagementCapabilities_OtherPowerCapabilitiesDescriptions_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* value */
    0,
};

static MetaFeature*
_CIM_PowerManagementCapabilities_features[] =
{
    (MetaFeature*)&_CIM_PowerManagementCapabilities_PowerCapabilities,
    (MetaFeature*)&_CIM_PowerManagementCapabilities_OtherPowerCapabilitiesDescriptions,
    0,
};

static const char*
__root_PG_InterOp_CIM_PowerManagementCapabilities_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Core::PowerMgmt",
    /* Version */
    "\0632.7.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_PowerManagementCapabilities =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "CIM_PowerManagementCapabilities",
    /* qualifiers */
    __root_PG_InterOp_CIM_PowerManagementCapabilities_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_Capabilities,
    /* features */
    _CIM_PowerManagementCapabilities_features,
};

//==============================================================================
//
// Class: CIM_Product
//
//==============================================================================

static const char*
_CIM_Product_Name_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* MaxLen */
    "\030\000\000\001\000",
    /* MappingStrings */
    "\026\000\001MIF.DMTF|ComponentID|001.2\000",
    /* ModelCorrespondence */
    "\036\000\001PRS_Product.ProductName\000",
    0,
};

static MetaProperty
_CIM_Product_Name =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Name",
    /* qualifiers */
    _CIM_Product_Name_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Product_IdentifyingNumber_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* MaxLen */
    "\030\000\000\000\100",
    /* MappingStrings */
    "\026\000\001MIF.DMTF|ComponentID|001.4\000",
    0,
};

static MetaProperty
_CIM_Product_IdentifyingNumber =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "IdentifyingNumber",
    /* qualifiers */
    _CIM_Product_IdentifyingNumber_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Product_Vendor_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* MaxLen */
    "\030\000\000\001\000",
    /* MappingStrings */
    "\026\000\001MIF.DMTF|ComponentID|001.1\000",
    /* ModelCorrespondence */
    "\036\000\001PRS_Product.Vendor\000",
    0,
};

static MetaProperty
_CIM_Product_Vendor =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Vendor",
    /* qualifiers */
    _CIM_Product_Vendor_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Product_Version_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* MaxLen */
    "\030\000\000\000\100",
    /* MappingStrings */
    "\026\000\001MIF.DMTF|ComponentID|001.3\000",
    /* ModelCorrespondence */
    "\036\000\001PRS_Product.Version\000",
    0,
};

static MetaProperty
_CIM_Product_Version =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Version",
    /* qualifiers */
    _CIM_Product_Version_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Product_SKUNumber_qualifiers[] =
{
    /* MaxLen */
    "\030\000\000\000\100",
    0,
};

static MetaProperty
_CIM_Product_SKUNumber =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "SKUNumber",
    /* qualifiers */
    _CIM_Product_SKUNumber_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Product_WarrantyStartDate_qualifiers[] =
{
    /* MappingStrings */
    "\026\000\001MIF.DMTF|FRU|002.9\000",
    /* ModelCorrespondence */
    "\036\000\001CIM_Product.WarrantyDuration\000",
    0,
};

static MetaProperty
_CIM_Product_WarrantyStartDate =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "WarrantyStartDate",
    /* qualifiers */
    _CIM_Product_WarrantyStartDate_qualifiers,
    /* type */
    CIMTYPE_DATETIME,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Product_WarrantyDuration_qualifiers[] =
{
    /* Units */
    "\060Days",
    /* MappingStrings */
    "\026\000\001MIF.DMTF|FRU|002.10\000",
    /* ModelCorrespondence */
    "\036\000\001CIM_Product.WarrantyStartDate\000",
    0,
};

static MetaProperty
_CIM_Product_WarrantyDuration =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "WarrantyDuration",
    /* qualifiers */
    _CIM_Product_WarrantyDuration_qualifiers,
    /* type */
    CIMTYPE_UINT32,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_Product_features[] =
{
    (MetaFeature*)&_CIM_Product_Name,
    (MetaFeature*)&_CIM_Product_IdentifyingNumber,
    (MetaFeature*)&_CIM_Product_Vendor,
    (MetaFeature*)&_CIM_Product_Version,
    (MetaFeature*)&_CIM_Product_SKUNumber,
    (MetaFeature*)&_CIM_Product_WarrantyStartDate,
    (MetaFeature*)&_CIM_Product_WarrantyDuration,
    0,
};

static const char*
__root_PG_InterOp_CIM_Product_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Core::ProductFRU",
    /* Version */
    "\0632.7.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_Product =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "CIM_Product",
    /* qualifiers */
    __root_PG_InterOp_CIM_Product_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_ManagedElement,
    /* features */
    _CIM_Product_features,
};

//==============================================================================
//
// Class: CIM_ProductComponent
//
//==============================================================================

static const char*
_CIM_ProductComponent_GroupComponent_qualifiers[] =
{
    /* Aggregate */
    "\003\001",
    /* Override */
    "\044GroupComponent",
    /* ModelCorrespondence */
    "\036\000\001PRS_ProductComponent.Antecedent\000",
    0,
};

static MetaReference
_CIM_ProductComponent_GroupComponent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_AGGREGATE|META_FLAG_READ,
    /* name */
    "GroupComponent",
    /* qualifiers */
    _CIM_ProductComponent_GroupComponent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_Product,
};

static const char*
_CIM_ProductComponent_PartComponent_qualifiers[] =
{
    /* Override */
    "\044PartComponent",
    /* ModelCorrespondence */
    "\036\000\001PRS_ProductComponent.Dependent\000",
    0,
};

static MetaReference
_CIM_ProductComponent_PartComponent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "PartComponent",
    /* qualifiers */
    _CIM_ProductComponent_PartComponent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_Product,
};

static MetaFeature*
_CIM_ProductComponent_features[] =
{
    (MetaFeature*)&_CIM_ProductComponent_GroupComponent,
    (MetaFeature*)&_CIM_ProductComponent_PartComponent,
    0,
};

static const char*
__root_PG_InterOp_CIM_ProductComponent_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::ProductFRU",
    /* Aggregation */
    "\004\001",
    /* Composition */
    "\011\001",
    /* Version */
    "\0632.7.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ProductComponent =
{
    /* flags */
    META_FLAG_ASSOCIATION|META_FLAG_AGGREGATION,
    /* name */
    "CIM_ProductComponent",
    /* qualifiers */
    __root_PG_InterOp_CIM_ProductComponent_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_Component,
    /* features */
    _CIM_ProductComponent_features,
};

//==============================================================================
//
// Class: CIM_ProductFRU
//
//==============================================================================

static const char*
_CIM_ProductFRU_Product_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Max */
    "\027\000\000\000\001",
    0,
};

static MetaReference
_CIM_ProductFRU_Product =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Product",
    /* qualifiers */
    _CIM_ProductFRU_Product_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_Product,
};

static const char*
_CIM_ProductFRU_FRU_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_ProductFRU_FRU =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "FRU",
    /* qualifiers */
    _CIM_ProductFRU_FRU_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_FRU,
};

static MetaFeature*
_CIM_ProductFRU_features[] =
{
    (MetaFeature*)&_CIM_ProductFRU_Product,
    (MetaFeature*)&_CIM_ProductFRU_FRU,
    0,
};

static const char*
__root_PG_InterOp_CIM_ProductFRU_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::ProductFRU",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ProductFRU =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_ProductFRU",
    /* qualifiers */
    __root_PG_InterOp_CIM_ProductFRU_qualifiers,
    /* super */
    0,
    /* features */
    _CIM_ProductFRU_features,
};

//==============================================================================
//
// Class: CIM_ProductParentChild
//
//==============================================================================

static const char*
_CIM_ProductParentChild_Parent_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Aggregate */
    "\003\001",
    0,
};

static MetaReference
_CIM_ProductParentChild_Parent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_AGGREGATE|META_FLAG_READ,
    /* name */
    "Parent",
    /* qualifiers */
    _CIM_ProductParentChild_Parent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_Product,
};

static const char*
_CIM_ProductParentChild_Child_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_ProductParentChild_Child =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Child",
    /* qualifiers */
    _CIM_ProductParentChild_Child_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_Product,
};

static MetaFeature*
_CIM_ProductParentChild_features[] =
{
    (MetaFeature*)&_CIM_ProductParentChild_Parent,
    (MetaFeature*)&_CIM_ProductParentChild_Child,
    0,
};

static const char*
__root_PG_InterOp_CIM_ProductParentChild_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::ProductFRU",
    /* Aggregation */
    "\004\001",
    /* Composition */
    "\011\001",
    /* Version */
    "\0632.7.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ProductParentChild =
{
    /* flags */
    META_FLAG_ASSOCIATION|META_FLAG_AGGREGATION,
    /* name */
    "CIM_ProductParentChild",
    /* qualifiers */
    __root_PG_InterOp_CIM_ProductParentChild_qualifiers,
    /* super */
    0,
    /* features */
    _CIM_ProductParentChild_features,
};

//==============================================================================
//
// Class: CIM_ProductPhysicalComponent
//
//==============================================================================

static const char*
_CIM_ProductPhysicalComponent_GroupComponent_qualifiers[] =
{
    /* Override */
    "\044GroupComponent",
    /* Max */
    "\027\000\000\000\001",
    0,
};

static MetaReference
_CIM_ProductPhysicalComponent_GroupComponent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "GroupComponent",
    /* qualifiers */
    _CIM_ProductPhysicalComponent_GroupComponent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_Product,
};

static const char*
_CIM_ProductPhysicalComponent_PartComponent_qualifiers[] =
{
    /* Override */
    "\044PartComponent",
    0,
};

static MetaReference
_CIM_ProductPhysicalComponent_PartComponent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "PartComponent",
    /* qualifiers */
    _CIM_ProductPhysicalComponent_PartComponent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_PhysicalElement,
};

static MetaFeature*
_CIM_ProductPhysicalComponent_features[] =
{
    (MetaFeature*)&_CIM_ProductPhysicalComponent_GroupComponent,
    (MetaFeature*)&_CIM_ProductPhysicalComponent_PartComponent,
    0,
};

static const char*
__root_PG_InterOp_CIM_ProductPhysicalComponent_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::ProductFRU",
    /* Aggregation */
    "\004\001",
    /* Composition */
    "\011\001",
    /* Version */
    "\0632.7.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ProductPhysicalComponent =
{
    /* flags */
    META_FLAG_ASSOCIATION|META_FLAG_AGGREGATION,
    /* name */
    "CIM_ProductPhysicalComponent",
    /* qualifiers */
    __root_PG_InterOp_CIM_ProductPhysicalComponent_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_Component,
    /* features */
    _CIM_ProductPhysicalComponent_features,
};

//==============================================================================
//
// Class: CIM_ProductPhysicalElements
//
//==============================================================================

static const char*
_CIM_ProductPhysicalElements_Product_qualifiers[] =
{
    /* Deprecated */
    "\013\000\001CIM_ProductPhysicalComponent.GroupComponent\000",
    /* Key */
    "\025\001",
    /* Aggregate */
    "\003\001",
    /* Max */
    "\027\000\000\000\001",
    0,
};

static MetaReference
_CIM_ProductPhysicalElements_Product =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_AGGREGATE|META_FLAG_READ,
    /* name */
    "Product",
    /* qualifiers */
    _CIM_ProductPhysicalElements_Product_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_Product,
};

static const char*
_CIM_ProductPhysicalElements_Component_qualifiers[] =
{
    /* Deprecated */
    "\013\000\001CIM_ProductPhysicalComponent.PartComponent\000",
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_ProductPhysicalElements_Component =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Component",
    /* qualifiers */
    _CIM_ProductPhysicalElements_Component_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_PhysicalElement,
};

static MetaFeature*
_CIM_ProductPhysicalElements_features[] =
{
    (MetaFeature*)&_CIM_ProductPhysicalElements_Product,
    (MetaFeature*)&_CIM_ProductPhysicalElements_Component,
    0,
};

static const char*
__root_PG_InterOp_CIM_ProductPhysicalElements_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* Deprecated */
    "\013\000\001CIM_ProductPhysicalComponent\000",
    /* UMLPackagePath */
    "\057CIM::Core::ProductFRU",
    /* Aggregation */
    "\004\001",
    /* Composition */
    "\011\001",
    /* Version */
    "\0632.7.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ProductPhysicalElements =
{
    /* flags */
    META_FLAG_ASSOCIATION|META_FLAG_AGGREGATION,
    /* name */
    "CIM_ProductPhysicalElements",
    /* qualifiers */
    __root_PG_InterOp_CIM_ProductPhysicalElements_qualifiers,
    /* super */
    0,
    /* features */
    _CIM_ProductPhysicalElements_features,
};

//==============================================================================
//
// Class: CIM_ProductProductDependency
//
//==============================================================================

static const char*
_CIM_ProductProductDependency_RequiredProduct_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_ProductProductDependency_RequiredProduct =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "RequiredProduct",
    /* qualifiers */
    _CIM_ProductProductDependency_RequiredProduct_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_Product,
};

static const char*
_CIM_ProductProductDependency_DependentProduct_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_ProductProductDependency_DependentProduct =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "DependentProduct",
    /* qualifiers */
    _CIM_ProductProductDependency_DependentProduct_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_Product,
};

static const char*
_CIM_ProductProductDependency_TypeOfDependency_qualifiers[] =
{
    /* ValueMap */
    "\061\000\0040\0001\0002\0003\000",
    /* Values */
    "\062\000\004Unknown\000Other\000Product Must Be Installed\000Product Must Not Be Installed\000",
    0,
};

static MetaProperty
_CIM_ProductProductDependency_TypeOfDependency =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "TypeOfDependency",
    /* qualifiers */
    _CIM_ProductProductDependency_TypeOfDependency_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_ProductProductDependency_features[] =
{
    (MetaFeature*)&_CIM_ProductProductDependency_RequiredProduct,
    (MetaFeature*)&_CIM_ProductProductDependency_DependentProduct,
    (MetaFeature*)&_CIM_ProductProductDependency_TypeOfDependency,
    0,
};

static const char*
__root_PG_InterOp_CIM_ProductProductDependency_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::ProductFRU",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ProductProductDependency =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_ProductProductDependency",
    /* qualifiers */
    __root_PG_InterOp_CIM_ProductProductDependency_qualifiers,
    /* super */
    0,
    /* features */
    _CIM_ProductProductDependency_features,
};

//==============================================================================
//
// Class: CIM_Profile
//
//==============================================================================

static const char*
_CIM_Profile_InstanceID_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaProperty
_CIM_Profile_InstanceID =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "InstanceID",
    /* qualifiers */
    _CIM_Profile_InstanceID_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_Profile_features[] =
{
    (MetaFeature*)&_CIM_Profile_InstanceID,
    0,
};

static const char*
__root_PG_InterOp_CIM_Profile_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Core::Settings",
    /* Version */
    "\0632.7.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_Profile =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "CIM_Profile",
    /* qualifiers */
    __root_PG_InterOp_CIM_Profile_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_Collection,
    /* features */
    _CIM_Profile_features,
};

//==============================================================================
//
// Class: CIM_ReplaceableProductFRU
//
//==============================================================================

static const char*
_CIM_ReplaceableProductFRU_Product_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Max */
    "\027\000\000\000\001",
    0,
};

static MetaReference
_CIM_ReplaceableProductFRU_Product =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Product",
    /* qualifiers */
    _CIM_ReplaceableProductFRU_Product_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_Product,
};

static const char*
_CIM_ReplaceableProductFRU_FRU_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_ReplaceableProductFRU_FRU =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "FRU",
    /* qualifiers */
    _CIM_ReplaceableProductFRU_FRU_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_FRU,
};

static MetaFeature*
_CIM_ReplaceableProductFRU_features[] =
{
    (MetaFeature*)&_CIM_ReplaceableProductFRU_Product,
    (MetaFeature*)&_CIM_ReplaceableProductFRU_FRU,
    0,
};

static const char*
__root_PG_InterOp_CIM_ReplaceableProductFRU_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::ProductFRU",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ReplaceableProductFRU =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_ReplaceableProductFRU",
    /* qualifiers */
    __root_PG_InterOp_CIM_ReplaceableProductFRU_qualifiers,
    /* super */
    0,
    /* features */
    _CIM_ReplaceableProductFRU_features,
};

//==============================================================================
//
// Class: CIM_Spared
//
//==============================================================================

static const char*
_CIM_Spared_Antecedent_qualifiers[] =
{
    /* Override */
    "\044Antecedent",
    0,
};

static MetaReference
_CIM_Spared_Antecedent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Antecedent",
    /* qualifiers */
    _CIM_Spared_Antecedent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ManagedElement,
};

static const char*
_CIM_Spared_Dependent_qualifiers[] =
{
    /* Override */
    "\044Dependent",
    0,
};

static MetaReference
_CIM_Spared_Dependent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Dependent",
    /* qualifiers */
    _CIM_Spared_Dependent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ManagedElement,
};

static MetaFeature*
_CIM_Spared_features[] =
{
    (MetaFeature*)&_CIM_Spared_Antecedent,
    (MetaFeature*)&_CIM_Spared_Dependent,
    0,
};

static const char*
__root_PG_InterOp_CIM_Spared_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::Redundancy",
    /* Version */
    "\0632.10.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_Spared =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_Spared",
    /* qualifiers */
    __root_PG_InterOp_CIM_Spared_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_Dependency,
    /* features */
    _CIM_Spared_features,
};

//==============================================================================
//
// Class: CIM_StatisticalData
//
//==============================================================================

static const char*
_CIM_StatisticalData_InstanceID_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaProperty
_CIM_StatisticalData_InstanceID =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "InstanceID",
    /* qualifiers */
    _CIM_StatisticalData_InstanceID_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_StatisticalData_ElementName_qualifiers[] =
{
    /* Required */
    "\050\001",
    /* Override */
    "\044ElementName",
    0,
};

static MetaProperty
_CIM_StatisticalData_ElementName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "ElementName",
    /* qualifiers */
    _CIM_StatisticalData_ElementName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_StatisticalData_StartStatisticTime_qualifiers[] =
{
    0,
};

static MetaProperty
_CIM_StatisticalData_StartStatisticTime =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "StartStatisticTime",
    /* qualifiers */
    _CIM_StatisticalData_StartStatisticTime_qualifiers,
    /* type */
    CIMTYPE_DATETIME,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_StatisticalData_StatisticTime_qualifiers[] =
{
    0,
};

static MetaProperty
_CIM_StatisticalData_StatisticTime =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "StatisticTime",
    /* qualifiers */
    _CIM_StatisticalData_StatisticTime_qualifiers,
    /* type */
    CIMTYPE_DATETIME,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_StatisticalData_SampleInterval_qualifiers[] =
{
    0,
};

static MetaProperty
_CIM_StatisticalData_SampleInterval =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "SampleInterval",
    /* qualifiers */
    _CIM_StatisticalData_SampleInterval_qualifiers,
    /* type */
    CIMTYPE_DATETIME,
    /* subscript */
    -1,
    /* value */
    "00000000000000.000000:000",
};

static const char*
_CIM_StatisticalData_ResetSelectedStats_SelectedStatistics_qualifiers[] =
{
    /* IN */
    "\024\001",
    0,
};

static MetaProperty
_CIM_StatisticalData_ResetSelectedStats_SelectedStatistics =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_IN,
    /* name */
    "SelectedStatistics",
    /* qualifiers */
    _CIM_StatisticalData_ResetSelectedStats_SelectedStatistics_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* value */
    0,
};

static MetaFeature*
_CIM_StatisticalData_ResetSelectedStats_parameters[] =
{
    (MetaFeature*)&_CIM_StatisticalData_ResetSelectedStats_SelectedStatistics,
    0,
};

static const char*
_CIM_StatisticalData_ResetSelectedStats_qualifiers[] =
{
    0,
};

static MetaMethod
_CIM_StatisticalData_ResetSelectedStats =
{
    /* flags */
    META_FLAG_METHOD,
    /* name */
    "CIM_StatisticalData",
    /* qualifiers */
    _CIM_StatisticalData_ResetSelectedStats_qualifiers,
    /* type */
    CIMTYPE_UINT32,
    /* parameters */
    _CIM_StatisticalData_ResetSelectedStats_parameters,
};

static MetaFeature*
_CIM_StatisticalData_features[] =
{
    (MetaFeature*)&_CIM_StatisticalData_InstanceID,
    (MetaFeature*)&_CIM_StatisticalData_ElementName,
    (MetaFeature*)&_CIM_StatisticalData_StartStatisticTime,
    (MetaFeature*)&_CIM_StatisticalData_StatisticTime,
    (MetaFeature*)&_CIM_StatisticalData_SampleInterval,
    (MetaFeature*)&_CIM_StatisticalData_ResetSelectedStats,
    0,
};

static const char*
__root_PG_InterOp_CIM_StatisticalData_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Core::Statistics",
    /* Abstract */
    "\002\001",
    /* Version */
    "\0632.8.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_StatisticalData =
{
    /* flags */
    META_FLAG_CLASS|META_FLAG_ABSTRACT,
    /* name */
    "CIM_StatisticalData",
    /* qualifiers */
    __root_PG_InterOp_CIM_StatisticalData_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_ManagedElement,
    /* features */
    _CIM_StatisticalData_features,
};

//==============================================================================
//
// Class: CIM_StatisticalInformation
//
//==============================================================================

static const char*
_CIM_StatisticalInformation_Name_qualifiers[] =
{
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_StatisticalInformation_Name =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "Name",
    /* qualifiers */
    _CIM_StatisticalInformation_Name_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_StatisticalInformation_features[] =
{
    (MetaFeature*)&_CIM_StatisticalInformation_Name,
    0,
};

static const char*
__root_PG_InterOp_CIM_StatisticalInformation_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Core::Statistics",
    /* Abstract */
    "\002\001",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_StatisticalInformation =
{
    /* flags */
    META_FLAG_CLASS|META_FLAG_ABSTRACT,
    /* name */
    "CIM_StatisticalInformation",
    /* qualifiers */
    __root_PG_InterOp_CIM_StatisticalInformation_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_ManagedElement,
    /* features */
    _CIM_StatisticalInformation_features,
};

//==============================================================================
//
// Class: CIM_StatisticalSetting
//
//==============================================================================

static const char*
_CIM_StatisticalSetting_StartStatisticTime_qualifiers[] =
{
    /* Write */
    "\065\001",
    /* ModelCorrespondence */
    "\036\000\001CIM_StatisticalData.StartStatisticTime\000",
    0,
};

static MetaProperty
_CIM_StatisticalSetting_StartStatisticTime =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_WRITE,
    /* name */
    "StartStatisticTime",
    /* qualifiers */
    _CIM_StatisticalSetting_StartStatisticTime_qualifiers,
    /* type */
    CIMTYPE_DATETIME,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_StatisticalSetting_EndStatisticTime_qualifiers[] =
{
    /* Write */
    "\065\001",
    0,
};

static MetaProperty
_CIM_StatisticalSetting_EndStatisticTime =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_WRITE,
    /* name */
    "EndStatisticTime",
    /* qualifiers */
    _CIM_StatisticalSetting_EndStatisticTime_qualifiers,
    /* type */
    CIMTYPE_DATETIME,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_StatisticalSetting_SampleInterval_qualifiers[] =
{
    /* Write */
    "\065\001",
    /* ModelCorrespondence */
    "\036\000\001CIM_StatisticalData.SampleInterval\000",
    0,
};

static MetaProperty
_CIM_StatisticalSetting_SampleInterval =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_WRITE,
    /* name */
    "SampleInterval",
    /* qualifiers */
    _CIM_StatisticalSetting_SampleInterval_qualifiers,
    /* type */
    CIMTYPE_DATETIME,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_StatisticalSetting_features[] =
{
    (MetaFeature*)&_CIM_StatisticalSetting_StartStatisticTime,
    (MetaFeature*)&_CIM_StatisticalSetting_EndStatisticTime,
    (MetaFeature*)&_CIM_StatisticalSetting_SampleInterval,
    0,
};

static const char*
__root_PG_InterOp_CIM_StatisticalSetting_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Core::Statistics",
    /* Version */
    "\0632.8.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_StatisticalSetting =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "CIM_StatisticalSetting",
    /* qualifiers */
    __root_PG_InterOp_CIM_StatisticalSetting_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_SettingData,
    /* features */
    _CIM_StatisticalSetting_features,
};

//==============================================================================
//
// Class: CIM_Statistics
//
//==============================================================================

static const char*
_CIM_Statistics_Stats_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_Statistics_Stats =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Stats",
    /* qualifiers */
    _CIM_Statistics_Stats_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_StatisticalInformation,
};

static const char*
_CIM_Statistics_Element_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_Statistics_Element =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Element",
    /* qualifiers */
    _CIM_Statistics_Element_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ManagedElement,
};

static MetaFeature*
_CIM_Statistics_features[] =
{
    (MetaFeature*)&_CIM_Statistics_Stats,
    (MetaFeature*)&_CIM_Statistics_Element,
    0,
};

static const char*
__root_PG_InterOp_CIM_Statistics_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::Statistics",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_Statistics =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_Statistics",
    /* qualifiers */
    __root_PG_InterOp_CIM_Statistics_qualifiers,
    /* super */
    0,
    /* features */
    _CIM_Statistics_features,
};

//==============================================================================
//
// Class: CIM_SupportAccess
//
//==============================================================================

static const char*
_CIM_SupportAccess_SupportAccessId_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_SupportAccess_SupportAccessId =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "SupportAccessId",
    /* qualifiers */
    _CIM_SupportAccess_SupportAccessId_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_SupportAccess_Description_qualifiers[] =
{
    /* Override */
    "\044Description",
    /* MappingStrings */
    "\026\000\001MIF.DMTF|Support|001.3\000",
    0,
};

static MetaProperty
_CIM_SupportAccess_Description =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "Description",
    /* qualifiers */
    _CIM_SupportAccess_Description_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_SupportAccess_CommunicationInfo_qualifiers[] =
{
    /* MappingStrings */
    "\026\000\002MIF.DMTF|FRU|002.11\000MIF.DMTF|FRU|002.12\000",
    0,
};

static MetaProperty
_CIM_SupportAccess_CommunicationInfo =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "CommunicationInfo",
    /* qualifiers */
    _CIM_SupportAccess_CommunicationInfo_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_SupportAccess_CommunicationMode_qualifiers[] =
{
    /* ValueMap */
    "\061\000\0101\0002\0003\0004\0005\0006\0007\0008\000",
    /* Values */
    "\062\000\010Other\000Phone\000Fax\000BBS\000Online Service\000Web Page\000FTP\000E-mail\000",
    /* MappingStrings */
    "\026\000\001MIF.DMTF|Support|001.5\000",
    0,
};

static MetaProperty
_CIM_SupportAccess_CommunicationMode =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "CommunicationMode",
    /* qualifiers */
    _CIM_SupportAccess_CommunicationMode_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_SupportAccess_Locale_qualifiers[] =
{
    /* MaxLen */
    "\030\000\000\000\100",
    /* MappingStrings */
    "\026\000\001MIF.DMTF|Support|001.2\000",
    0,
};

static MetaProperty
_CIM_SupportAccess_Locale =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "Locale",
    /* qualifiers */
    _CIM_SupportAccess_Locale_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_SupportAccess_features[] =
{
    (MetaFeature*)&_CIM_SupportAccess_SupportAccessId,
    (MetaFeature*)&_CIM_SupportAccess_Description,
    (MetaFeature*)&_CIM_SupportAccess_CommunicationInfo,
    (MetaFeature*)&_CIM_SupportAccess_CommunicationMode,
    (MetaFeature*)&_CIM_SupportAccess_Locale,
    0,
};

static const char*
__root_PG_InterOp_CIM_SupportAccess_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Core::ProductFRU",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_SupportAccess =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "CIM_SupportAccess",
    /* qualifiers */
    __root_PG_InterOp_CIM_SupportAccess_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_ManagedElement,
    /* features */
    _CIM_SupportAccess_features,
};

//==============================================================================
//
// Class: CIM_Synchronized
//
//==============================================================================

static const char*
_CIM_Synchronized_SystemElement_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_Synchronized_SystemElement =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "SystemElement",
    /* qualifiers */
    _CIM_Synchronized_SystemElement_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ManagedElement,
};

static const char*
_CIM_Synchronized_SyncedElement_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_Synchronized_SyncedElement =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "SyncedElement",
    /* qualifiers */
    _CIM_Synchronized_SyncedElement_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ManagedElement,
};

static const char*
_CIM_Synchronized_WhenSynced_qualifiers[] =
{
    0,
};

static MetaProperty
_CIM_Synchronized_WhenSynced =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "WhenSynced",
    /* qualifiers */
    _CIM_Synchronized_WhenSynced_qualifiers,
    /* type */
    CIMTYPE_DATETIME,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Synchronized_SyncMaintained_qualifiers[] =
{
    0,
};

static MetaProperty
_CIM_Synchronized_SyncMaintained =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "SyncMaintained",
    /* qualifiers */
    _CIM_Synchronized_SyncMaintained_qualifiers,
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_Synchronized_features[] =
{
    (MetaFeature*)&_CIM_Synchronized_SystemElement,
    (MetaFeature*)&_CIM_Synchronized_SyncedElement,
    (MetaFeature*)&_CIM_Synchronized_WhenSynced,
    (MetaFeature*)&_CIM_Synchronized_SyncMaintained,
    0,
};

static const char*
__root_PG_InterOp_CIM_Synchronized_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::CoreElements",
    /* Version */
    "\0632.8.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_Synchronized =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_Synchronized",
    /* qualifiers */
    __root_PG_InterOp_CIM_Synchronized_qualifiers,
    /* super */
    0,
    /* features */
    _CIM_Synchronized_features,
};

//==============================================================================
//
// Class: CIM_SystemConfiguration
//
//==============================================================================

static const char*
_CIM_SystemConfiguration_SystemCreationClassName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Propagated */
    "\045CIM_System.CreationClassName",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_SystemConfiguration_SystemCreationClassName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "SystemCreationClassName",
    /* qualifiers */
    _CIM_SystemConfiguration_SystemCreationClassName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_SystemConfiguration_SystemName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Propagated */
    "\045CIM_System.Name",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_SystemConfiguration_SystemName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "SystemName",
    /* qualifiers */
    _CIM_SystemConfiguration_SystemName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_SystemConfiguration_CreationClassName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_SystemConfiguration_CreationClassName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "CreationClassName",
    /* qualifiers */
    _CIM_SystemConfiguration_CreationClassName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_SystemConfiguration_Name_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_SystemConfiguration_Name =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Name",
    /* qualifiers */
    _CIM_SystemConfiguration_Name_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_SystemConfiguration_features[] =
{
    (MetaFeature*)&_CIM_SystemConfiguration_SystemCreationClassName,
    (MetaFeature*)&_CIM_SystemConfiguration_SystemName,
    (MetaFeature*)&_CIM_SystemConfiguration_CreationClassName,
    (MetaFeature*)&_CIM_SystemConfiguration_Name,
    0,
};

static const char*
__root_PG_InterOp_CIM_SystemConfiguration_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Core::Settings",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_SystemConfiguration =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "CIM_SystemConfiguration",
    /* qualifiers */
    __root_PG_InterOp_CIM_SystemConfiguration_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_ManagedElement,
    /* features */
    _CIM_SystemConfiguration_features,
};

//==============================================================================
//
// Class: CIM_SystemSetting
//
//==============================================================================

static const char*
_CIM_SystemSetting_SystemCreationClassName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Propagated */
    "\045CIM_System.CreationClassName",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_SystemSetting_SystemCreationClassName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "SystemCreationClassName",
    /* qualifiers */
    _CIM_SystemSetting_SystemCreationClassName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_SystemSetting_SystemName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Propagated */
    "\045CIM_System.Name",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_SystemSetting_SystemName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "SystemName",
    /* qualifiers */
    _CIM_SystemSetting_SystemName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_SystemSetting_CreationClassName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_SystemSetting_CreationClassName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "CreationClassName",
    /* qualifiers */
    _CIM_SystemSetting_CreationClassName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_SystemSetting_SettingID_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Override */
    "\044SettingID",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_SystemSetting_SettingID =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "SettingID",
    /* qualifiers */
    _CIM_SystemSetting_SettingID_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_SystemSetting_features[] =
{
    (MetaFeature*)&_CIM_SystemSetting_SystemCreationClassName,
    (MetaFeature*)&_CIM_SystemSetting_SystemName,
    (MetaFeature*)&_CIM_SystemSetting_CreationClassName,
    (MetaFeature*)&_CIM_SystemSetting_SettingID,
    0,
};

static const char*
__root_PG_InterOp_CIM_SystemSetting_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Core::Settings",
    /* Abstract */
    "\002\001",
    /* Version */
    "\0632.10.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_SystemSetting =
{
    /* flags */
    META_FLAG_CLASS|META_FLAG_ABSTRACT,
    /* name */
    "CIM_SystemSetting",
    /* qualifiers */
    __root_PG_InterOp_CIM_SystemSetting_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_Setting,
    /* features */
    _CIM_SystemSetting_features,
};

//==============================================================================
//
// Class: CIM_SystemSettingContext
//
//==============================================================================

static const char*
_CIM_SystemSettingContext_Context_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Aggregate */
    "\003\001",
    0,
};

static MetaReference
_CIM_SystemSettingContext_Context =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_AGGREGATE|META_FLAG_READ,
    /* name */
    "Context",
    /* qualifiers */
    _CIM_SystemSettingContext_Context_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_SystemConfiguration,
};

static const char*
_CIM_SystemSettingContext_Setting_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_SystemSettingContext_Setting =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Setting",
    /* qualifiers */
    _CIM_SystemSettingContext_Setting_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_SystemSetting,
};

static MetaFeature*
_CIM_SystemSettingContext_features[] =
{
    (MetaFeature*)&_CIM_SystemSettingContext_Context,
    (MetaFeature*)&_CIM_SystemSettingContext_Setting,
    0,
};

static const char*
__root_PG_InterOp_CIM_SystemSettingContext_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::Settings",
    /* Aggregation */
    "\004\001",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_SystemSettingContext =
{
    /* flags */
    META_FLAG_ASSOCIATION|META_FLAG_AGGREGATION,
    /* name */
    "CIM_SystemSettingContext",
    /* qualifiers */
    __root_PG_InterOp_CIM_SystemSettingContext_qualifiers,
    /* super */
    0,
    /* features */
    _CIM_SystemSettingContext_features,
};

//==============================================================================
//
// Class: CIM_SystemSpecificCollection
//
//==============================================================================

static const char*
_CIM_SystemSpecificCollection_InstanceID_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaProperty
_CIM_SystemSpecificCollection_InstanceID =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "InstanceID",
    /* qualifiers */
    _CIM_SystemSpecificCollection_InstanceID_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_SystemSpecificCollection_features[] =
{
    (MetaFeature*)&_CIM_SystemSpecificCollection_InstanceID,
    0,
};

static const char*
__root_PG_InterOp_CIM_SystemSpecificCollection_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Core::Collection",
    /* Version */
    "\0632.10.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_SystemSpecificCollection =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "CIM_SystemSpecificCollection",
    /* qualifiers */
    __root_PG_InterOp_CIM_SystemSpecificCollection_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_Collection,
    /* features */
    _CIM_SystemSpecificCollection_features,
};

//==============================================================================
//
// Class: CIM_SystemStatisticalInformation
//
//==============================================================================

static const char*
_CIM_SystemStatisticalInformation_SystemCreationClassName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Propagated */
    "\045CIM_System.CreationClassName",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_SystemStatisticalInformation_SystemCreationClassName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "SystemCreationClassName",
    /* qualifiers */
    _CIM_SystemStatisticalInformation_SystemCreationClassName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_SystemStatisticalInformation_SystemName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Propagated */
    "\045CIM_System.Name",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_SystemStatisticalInformation_SystemName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "SystemName",
    /* qualifiers */
    _CIM_SystemStatisticalInformation_SystemName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_SystemStatisticalInformation_CreationClassName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_SystemStatisticalInformation_CreationClassName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "CreationClassName",
    /* qualifiers */
    _CIM_SystemStatisticalInformation_CreationClassName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_SystemStatisticalInformation_Name_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Override */
    "\044Name",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_SystemStatisticalInformation_Name =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Name",
    /* qualifiers */
    _CIM_SystemStatisticalInformation_Name_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_SystemStatisticalInformation_features[] =
{
    (MetaFeature*)&_CIM_SystemStatisticalInformation_SystemCreationClassName,
    (MetaFeature*)&_CIM_SystemStatisticalInformation_SystemName,
    (MetaFeature*)&_CIM_SystemStatisticalInformation_CreationClassName,
    (MetaFeature*)&_CIM_SystemStatisticalInformation_Name,
    0,
};

static const char*
__root_PG_InterOp_CIM_SystemStatisticalInformation_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Core::Statistics",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_SystemStatisticalInformation =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "CIM_SystemStatisticalInformation",
    /* qualifiers */
    __root_PG_InterOp_CIM_SystemStatisticalInformation_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_StatisticalInformation,
    /* features */
    _CIM_SystemStatisticalInformation_features,
};

//==============================================================================
//
// Class: CIM_CollectedCollections
//
//==============================================================================

static const char*
_CIM_CollectedCollections_Collection_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Aggregate */
    "\003\001",
    0,
};

static MetaReference
_CIM_CollectedCollections_Collection =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_AGGREGATE|META_FLAG_READ,
    /* name */
    "Collection",
    /* qualifiers */
    _CIM_CollectedCollections_Collection_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_CollectionOfMSEs,
};

static const char*
_CIM_CollectedCollections_CollectionInCollection_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_CollectedCollections_CollectionInCollection =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "CollectionInCollection",
    /* qualifiers */
    _CIM_CollectedCollections_CollectionInCollection_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_CollectionOfMSEs,
};

static MetaFeature*
_CIM_CollectedCollections_features[] =
{
    (MetaFeature*)&_CIM_CollectedCollections_Collection,
    (MetaFeature*)&_CIM_CollectedCollections_CollectionInCollection,
    0,
};

static const char*
__root_PG_InterOp_CIM_CollectedCollections_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::Collection",
    /* Aggregation */
    "\004\001",
    /* Version */
    "\0632.10.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_CollectedCollections =
{
    /* flags */
    META_FLAG_ASSOCIATION|META_FLAG_AGGREGATION,
    /* name */
    "CIM_CollectedCollections",
    /* qualifiers */
    __root_PG_InterOp_CIM_CollectedCollections_qualifiers,
    /* super */
    0,
    /* features */
    _CIM_CollectedCollections_features,
};

//==============================================================================
//
// Class: CIM_CollectedMSEs
//
//==============================================================================

static const char*
_CIM_CollectedMSEs_Collection_qualifiers[] =
{
    /* Aggregate */
    "\003\001",
    /* Override */
    "\044Collection",
    0,
};

static MetaReference
_CIM_CollectedMSEs_Collection =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_AGGREGATE|META_FLAG_READ,
    /* name */
    "Collection",
    /* qualifiers */
    _CIM_CollectedMSEs_Collection_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_CollectionOfMSEs,
};

static const char*
_CIM_CollectedMSEs_Member_qualifiers[] =
{
    /* Override */
    "\044Member",
    0,
};

static MetaReference
_CIM_CollectedMSEs_Member =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Member",
    /* qualifiers */
    _CIM_CollectedMSEs_Member_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ManagedSystemElement,
};

static MetaFeature*
_CIM_CollectedMSEs_features[] =
{
    (MetaFeature*)&_CIM_CollectedMSEs_Collection,
    (MetaFeature*)&_CIM_CollectedMSEs_Member,
    0,
};

static const char*
__root_PG_InterOp_CIM_CollectedMSEs_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::Collection",
    /* Aggregation */
    "\004\001",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_CollectedMSEs =
{
    /* flags */
    META_FLAG_ASSOCIATION|META_FLAG_AGGREGATION,
    /* name */
    "CIM_CollectedMSEs",
    /* qualifiers */
    __root_PG_InterOp_CIM_CollectedMSEs_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_MemberOfCollection,
    /* features */
    _CIM_CollectedMSEs_features,
};

//==============================================================================
//
// Class: CIM_CollectionConfiguration
//
//==============================================================================

static const char*
_CIM_CollectionConfiguration_Collection_qualifiers[] =
{
    /* Deprecated */
    "\013\000\002CIM_ElementSettingData.ManagedElement\000CIM_CollectionSetting.Collection\000",
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_CollectionConfiguration_Collection =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Collection",
    /* qualifiers */
    _CIM_CollectionConfiguration_Collection_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_CollectionOfMSEs,
};

static const char*
_CIM_CollectionConfiguration_Configuration_qualifiers[] =
{
    /* Deprecated */
    "\013\000\002CIM_ElementSettingData.SettingData\000CIM_CollectionSetting.Setting\000",
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_CollectionConfiguration_Configuration =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Configuration",
    /* qualifiers */
    _CIM_CollectionConfiguration_Configuration_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_Configuration,
};

static MetaFeature*
_CIM_CollectionConfiguration_features[] =
{
    (MetaFeature*)&_CIM_CollectionConfiguration_Collection,
    (MetaFeature*)&_CIM_CollectionConfiguration_Configuration,
    0,
};

static const char*
__root_PG_InterOp_CIM_CollectionConfiguration_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* Deprecated */
    "\013\000\002CIM_ElementSettingData\000CIM_CollectionSetting\000",
    /* UMLPackagePath */
    "\057CIM::Core::Settings",
    /* Version */
    "\0632.10.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_CollectionConfiguration =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_CollectionConfiguration",
    /* qualifiers */
    __root_PG_InterOp_CIM_CollectionConfiguration_qualifiers,
    /* super */
    0,
    /* features */
    _CIM_CollectionConfiguration_features,
};

//==============================================================================
//
// Class: CIM_CollectionSetting
//
//==============================================================================

static const char*
_CIM_CollectionSetting_Collection_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_CollectionSetting_Collection =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Collection",
    /* qualifiers */
    _CIM_CollectionSetting_Collection_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_CollectionOfMSEs,
};

static const char*
_CIM_CollectionSetting_Setting_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_CollectionSetting_Setting =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Setting",
    /* qualifiers */
    _CIM_CollectionSetting_Setting_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_Setting,
};

static MetaFeature*
_CIM_CollectionSetting_features[] =
{
    (MetaFeature*)&_CIM_CollectionSetting_Collection,
    (MetaFeature*)&_CIM_CollectionSetting_Setting,
    0,
};

static const char*
__root_PG_InterOp_CIM_CollectionSetting_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::Settings",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_CollectionSetting =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_CollectionSetting",
    /* qualifiers */
    __root_PG_InterOp_CIM_CollectionSetting_qualifiers,
    /* super */
    0,
    /* features */
    _CIM_CollectionSetting_features,
};

//==============================================================================
//
// Class: CIM_CompatibleProduct
//
//==============================================================================

static const char*
_CIM_CompatibleProduct_Product_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_CompatibleProduct_Product =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Product",
    /* qualifiers */
    _CIM_CompatibleProduct_Product_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_Product,
};

static const char*
_CIM_CompatibleProduct_CompatibleProduct_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_CompatibleProduct_CompatibleProduct =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "CompatibleProduct",
    /* qualifiers */
    _CIM_CompatibleProduct_CompatibleProduct_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_Product,
};

static const char*
_CIM_CompatibleProduct_CompatibilityDescription_qualifiers[] =
{
    0,
};

static MetaProperty
_CIM_CompatibleProduct_CompatibilityDescription =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "CompatibilityDescription",
    /* qualifiers */
    _CIM_CompatibleProduct_CompatibilityDescription_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_CompatibleProduct_features[] =
{
    (MetaFeature*)&_CIM_CompatibleProduct_Product,
    (MetaFeature*)&_CIM_CompatibleProduct_CompatibleProduct,
    (MetaFeature*)&_CIM_CompatibleProduct_CompatibilityDescription,
    0,
};

static const char*
__root_PG_InterOp_CIM_CompatibleProduct_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::ProductFRU",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_CompatibleProduct =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_CompatibleProduct",
    /* qualifiers */
    __root_PG_InterOp_CIM_CompatibleProduct_qualifiers,
    /* super */
    0,
    /* features */
    _CIM_CompatibleProduct_features,
};

//==============================================================================
//
// Class: CIM_ConcreteDependency
//
//==============================================================================

static const char*
_CIM_ConcreteDependency_Antecedent_qualifiers[] =
{
    /* Override */
    "\044Antecedent",
    0,
};

static MetaReference
_CIM_ConcreteDependency_Antecedent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Antecedent",
    /* qualifiers */
    _CIM_ConcreteDependency_Antecedent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ManagedElement,
};

static const char*
_CIM_ConcreteDependency_Dependent_qualifiers[] =
{
    /* Override */
    "\044Dependent",
    0,
};

static MetaReference
_CIM_ConcreteDependency_Dependent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Dependent",
    /* qualifiers */
    _CIM_ConcreteDependency_Dependent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ManagedElement,
};

static MetaFeature*
_CIM_ConcreteDependency_features[] =
{
    (MetaFeature*)&_CIM_ConcreteDependency_Antecedent,
    (MetaFeature*)&_CIM_ConcreteDependency_Dependent,
    0,
};

static const char*
__root_PG_InterOp_CIM_ConcreteDependency_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::CoreElements",
    /* Version */
    "\0632.10.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ConcreteDependency =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_ConcreteDependency",
    /* qualifiers */
    __root_PG_InterOp_CIM_ConcreteDependency_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_Dependency,
    /* features */
    _CIM_ConcreteDependency_features,
};

//==============================================================================
//
// Class: CIM_ConcreteIdentity
//
//==============================================================================

static const char*
_CIM_ConcreteIdentity_SystemElement_qualifiers[] =
{
    /* Override */
    "\044SystemElement",
    0,
};

static MetaReference
_CIM_ConcreteIdentity_SystemElement =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "SystemElement",
    /* qualifiers */
    _CIM_ConcreteIdentity_SystemElement_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ManagedElement,
};

static const char*
_CIM_ConcreteIdentity_SameElement_qualifiers[] =
{
    /* Override */
    "\044SameElement",
    0,
};

static MetaReference
_CIM_ConcreteIdentity_SameElement =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "SameElement",
    /* qualifiers */
    _CIM_ConcreteIdentity_SameElement_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ManagedElement,
};

static MetaFeature*
_CIM_ConcreteIdentity_features[] =
{
    (MetaFeature*)&_CIM_ConcreteIdentity_SystemElement,
    (MetaFeature*)&_CIM_ConcreteIdentity_SameElement,
    0,
};

static const char*
__root_PG_InterOp_CIM_ConcreteIdentity_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::CoreElements",
    /* Version */
    "\0632.10.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ConcreteIdentity =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_ConcreteIdentity",
    /* qualifiers */
    __root_PG_InterOp_CIM_ConcreteIdentity_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_LogicalIdentity,
    /* features */
    _CIM_ConcreteIdentity_features,
};

//==============================================================================
//
// Class: CIM_ContainedLocation
//
//==============================================================================

static const char*
_CIM_ContainedLocation_GroupComponent_qualifiers[] =
{
    /* Aggregate */
    "\003\001",
    /* Override */
    "\044GroupComponent",
    /* Max */
    "\027\000\000\000\001",
    0,
};

static MetaReference
_CIM_ContainedLocation_GroupComponent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_AGGREGATE|META_FLAG_READ,
    /* name */
    "GroupComponent",
    /* qualifiers */
    _CIM_ContainedLocation_GroupComponent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_Location,
};

static const char*
_CIM_ContainedLocation_PartComponent_qualifiers[] =
{
    /* Override */
    "\044PartComponent",
    0,
};

static MetaReference
_CIM_ContainedLocation_PartComponent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "PartComponent",
    /* qualifiers */
    _CIM_ContainedLocation_PartComponent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_Location,
};

static MetaFeature*
_CIM_ContainedLocation_features[] =
{
    (MetaFeature*)&_CIM_ContainedLocation_GroupComponent,
    (MetaFeature*)&_CIM_ContainedLocation_PartComponent,
    0,
};

static const char*
__root_PG_InterOp_CIM_ContainedLocation_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::Physical",
    /* Aggregation */
    "\004\001",
    /* Composition */
    "\011\001",
    /* Version */
    "\0632.7.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ContainedLocation =
{
    /* flags */
    META_FLAG_ASSOCIATION|META_FLAG_AGGREGATION,
    /* name */
    "CIM_ContainedLocation",
    /* qualifiers */
    __root_PG_InterOp_CIM_ContainedLocation_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_Component,
    /* features */
    _CIM_ContainedLocation_features,
};

//==============================================================================
//
// Class: CIM_DeviceStatisticalInformation
//
//==============================================================================

static const char*
_CIM_DeviceStatisticalInformation_SystemCreationClassName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Propagated */
    "\045CIM_LogicalDevice.SystemCreationClassName",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_DeviceStatisticalInformation_SystemCreationClassName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "SystemCreationClassName",
    /* qualifiers */
    _CIM_DeviceStatisticalInformation_SystemCreationClassName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_DeviceStatisticalInformation_SystemName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Propagated */
    "\045CIM_LogicalDevice.SystemName",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_DeviceStatisticalInformation_SystemName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "SystemName",
    /* qualifiers */
    _CIM_DeviceStatisticalInformation_SystemName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_DeviceStatisticalInformation_DeviceCreationClassName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Propagated */
    "\045CIM_LogicalDevice.CreationClassName",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_DeviceStatisticalInformation_DeviceCreationClassName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "DeviceCreationClassName",
    /* qualifiers */
    _CIM_DeviceStatisticalInformation_DeviceCreationClassName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_DeviceStatisticalInformation_DeviceID_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Propagated */
    "\045CIM_LogicalDevice.DeviceID",
    /* MaxLen */
    "\030\000\000\000\100",
    0,
};

static MetaProperty
_CIM_DeviceStatisticalInformation_DeviceID =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "DeviceID",
    /* qualifiers */
    _CIM_DeviceStatisticalInformation_DeviceID_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_DeviceStatisticalInformation_CreationClassName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_DeviceStatisticalInformation_CreationClassName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "CreationClassName",
    /* qualifiers */
    _CIM_DeviceStatisticalInformation_CreationClassName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_DeviceStatisticalInformation_Name_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Override */
    "\044Name",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_DeviceStatisticalInformation_Name =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Name",
    /* qualifiers */
    _CIM_DeviceStatisticalInformation_Name_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_DeviceStatisticalInformation_features[] =
{
    (MetaFeature*)&_CIM_DeviceStatisticalInformation_SystemCreationClassName,
    (MetaFeature*)&_CIM_DeviceStatisticalInformation_SystemName,
    (MetaFeature*)&_CIM_DeviceStatisticalInformation_DeviceCreationClassName,
    (MetaFeature*)&_CIM_DeviceStatisticalInformation_DeviceID,
    (MetaFeature*)&_CIM_DeviceStatisticalInformation_CreationClassName,
    (MetaFeature*)&_CIM_DeviceStatisticalInformation_Name,
    0,
};

static const char*
__root_PG_InterOp_CIM_DeviceStatisticalInformation_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Core::Statistics",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_DeviceStatisticalInformation =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "CIM_DeviceStatisticalInformation",
    /* qualifiers */
    __root_PG_InterOp_CIM_DeviceStatisticalInformation_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_StatisticalInformation,
    /* features */
    _CIM_DeviceStatisticalInformation_features,
};

//==============================================================================
//
// Class: CIM_ElementConfiguration
//
//==============================================================================

static const char*
_CIM_ElementConfiguration_Element_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_ElementConfiguration_Element =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Element",
    /* qualifiers */
    _CIM_ElementConfiguration_Element_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ManagedSystemElement,
};

static const char*
_CIM_ElementConfiguration_Configuration_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_ElementConfiguration_Configuration =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Configuration",
    /* qualifiers */
    _CIM_ElementConfiguration_Configuration_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_Configuration,
};

static MetaFeature*
_CIM_ElementConfiguration_features[] =
{
    (MetaFeature*)&_CIM_ElementConfiguration_Element,
    (MetaFeature*)&_CIM_ElementConfiguration_Configuration,
    0,
};

static const char*
__root_PG_InterOp_CIM_ElementConfiguration_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::Settings",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ElementConfiguration =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_ElementConfiguration",
    /* qualifiers */
    __root_PG_InterOp_CIM_ElementConfiguration_qualifiers,
    /* super */
    0,
    /* features */
    _CIM_ElementConfiguration_features,
};

//==============================================================================
//
// Class: CIM_ElementLocation
//
//==============================================================================

static const char*
_CIM_ElementLocation_Element_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_ElementLocation_Element =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Element",
    /* qualifiers */
    _CIM_ElementLocation_Element_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ManagedElement,
};

static const char*
_CIM_ElementLocation_PhysicalLocation_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_ElementLocation_PhysicalLocation =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "PhysicalLocation",
    /* qualifiers */
    _CIM_ElementLocation_PhysicalLocation_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_Location,
};

static MetaFeature*
_CIM_ElementLocation_features[] =
{
    (MetaFeature*)&_CIM_ElementLocation_Element,
    (MetaFeature*)&_CIM_ElementLocation_PhysicalLocation,
    0,
};

static const char*
__root_PG_InterOp_CIM_ElementLocation_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::Physical",
    /* Version */
    "\0632.10.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ElementLocation =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_ElementLocation",
    /* qualifiers */
    __root_PG_InterOp_CIM_ElementLocation_qualifiers,
    /* super */
    0,
    /* features */
    _CIM_ElementLocation_features,
};

//==============================================================================
//
// Class: CIM_ElementProfile
//
//==============================================================================

static const char*
_CIM_ElementProfile_ManagedElement_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_ElementProfile_ManagedElement =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "ManagedElement",
    /* qualifiers */
    _CIM_ElementProfile_ManagedElement_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ManagedElement,
};

static const char*
_CIM_ElementProfile_Profile_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_ElementProfile_Profile =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Profile",
    /* qualifiers */
    _CIM_ElementProfile_Profile_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_Profile,
};

static MetaFeature*
_CIM_ElementProfile_features[] =
{
    (MetaFeature*)&_CIM_ElementProfile_ManagedElement,
    (MetaFeature*)&_CIM_ElementProfile_Profile,
    0,
};

static const char*
__root_PG_InterOp_CIM_ElementProfile_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::Settings",
    /* Version */
    "\0632.7.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ElementProfile =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_ElementProfile",
    /* qualifiers */
    __root_PG_InterOp_CIM_ElementProfile_qualifiers,
    /* super */
    0,
    /* features */
    _CIM_ElementProfile_features,
};

//==============================================================================
//
// Class: CIM_ElementSetting
//
//==============================================================================

static const char*
_CIM_ElementSetting_Element_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_ElementSetting_Element =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Element",
    /* qualifiers */
    _CIM_ElementSetting_Element_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ManagedSystemElement,
};

static const char*
_CIM_ElementSetting_Setting_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_ElementSetting_Setting =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Setting",
    /* qualifiers */
    _CIM_ElementSetting_Setting_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_Setting,
};

static MetaFeature*
_CIM_ElementSetting_features[] =
{
    (MetaFeature*)&_CIM_ElementSetting_Element,
    (MetaFeature*)&_CIM_ElementSetting_Setting,
    0,
};

static const char*
__root_PG_InterOp_CIM_ElementSetting_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::Settings",
    /* Version */
    "\0632.10.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ElementSetting =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_ElementSetting",
    /* qualifiers */
    __root_PG_InterOp_CIM_ElementSetting_qualifiers,
    /* super */
    0,
    /* features */
    _CIM_ElementSetting_features,
};

//==============================================================================
//
// Class: CIM_ElementSettingData
//
//==============================================================================

static const char*
_CIM_ElementSettingData_ManagedElement_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_ElementSettingData_ManagedElement =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "ManagedElement",
    /* qualifiers */
    _CIM_ElementSettingData_ManagedElement_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ManagedElement,
};

static const char*
_CIM_ElementSettingData_SettingData_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_ElementSettingData_SettingData =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "SettingData",
    /* qualifiers */
    _CIM_ElementSettingData_SettingData_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_SettingData,
};

static const char*
_CIM_ElementSettingData_IsDefault_qualifiers[] =
{
    /* ValueMap */
    "\061\000\0030\0001\0002\000",
    /* Values */
    "\062\000\003Unknown\000Is Default\000Is Not Default\000",
    0,
};

static MetaProperty
_CIM_ElementSettingData_IsDefault =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "IsDefault",
    /* qualifiers */
    _CIM_ElementSettingData_IsDefault_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_ElementSettingData_IsCurrent_qualifiers[] =
{
    /* ValueMap */
    "\061\000\0030\0001\0002\000",
    /* Values */
    "\062\000\003Unknown\000Is Current\000Is Not Current\000",
    0,
};

static MetaProperty
_CIM_ElementSettingData_IsCurrent =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "IsCurrent",
    /* qualifiers */
    _CIM_ElementSettingData_IsCurrent_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_ElementSettingData_features[] =
{
    (MetaFeature*)&_CIM_ElementSettingData_ManagedElement,
    (MetaFeature*)&_CIM_ElementSettingData_SettingData,
    (MetaFeature*)&_CIM_ElementSettingData_IsDefault,
    (MetaFeature*)&_CIM_ElementSettingData_IsCurrent,
    0,
};

static const char*
__root_PG_InterOp_CIM_ElementSettingData_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::Settings",
    /* Version */
    "\0632.7.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ElementSettingData =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_ElementSettingData",
    /* qualifiers */
    __root_PG_InterOp_CIM_ElementSettingData_qualifiers,
    /* super */
    0,
    /* features */
    _CIM_ElementSettingData_features,
};

//==============================================================================
//
// Class: CIM_ElementStatisticalData
//
//==============================================================================

static const char*
_CIM_ElementStatisticalData_ManagedElement_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Min */
    "\033\000\000\000\001",
    /* Max */
    "\027\000\000\000\001",
    0,
};

static MetaReference
_CIM_ElementStatisticalData_ManagedElement =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "ManagedElement",
    /* qualifiers */
    _CIM_ElementStatisticalData_ManagedElement_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ManagedElement,
};

static const char*
_CIM_ElementStatisticalData_Stats_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_ElementStatisticalData_Stats =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Stats",
    /* qualifiers */
    _CIM_ElementStatisticalData_Stats_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_StatisticalData,
};

static MetaFeature*
_CIM_ElementStatisticalData_features[] =
{
    (MetaFeature*)&_CIM_ElementStatisticalData_ManagedElement,
    (MetaFeature*)&_CIM_ElementStatisticalData_Stats,
    0,
};

static const char*
__root_PG_InterOp_CIM_ElementStatisticalData_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::Statistics",
    /* Version */
    "\0632.7.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ElementStatisticalData =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_ElementStatisticalData",
    /* qualifiers */
    __root_PG_InterOp_CIM_ElementStatisticalData_qualifiers,
    /* super */
    0,
    /* features */
    _CIM_ElementStatisticalData_features,
};

//==============================================================================
//
// Class: CIM_FRUIncludesProduct
//
//==============================================================================

static const char*
_CIM_FRUIncludesProduct_FRU_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Aggregate */
    "\003\001",
    /* Max */
    "\027\000\000\000\001",
    0,
};

static MetaReference
_CIM_FRUIncludesProduct_FRU =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_AGGREGATE|META_FLAG_READ,
    /* name */
    "FRU",
    /* qualifiers */
    _CIM_FRUIncludesProduct_FRU_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_FRU,
};

static const char*
_CIM_FRUIncludesProduct_Component_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_FRUIncludesProduct_Component =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Component",
    /* qualifiers */
    _CIM_FRUIncludesProduct_Component_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_Product,
};

static MetaFeature*
_CIM_FRUIncludesProduct_features[] =
{
    (MetaFeature*)&_CIM_FRUIncludesProduct_FRU,
    (MetaFeature*)&_CIM_FRUIncludesProduct_Component,
    0,
};

static const char*
__root_PG_InterOp_CIM_FRUIncludesProduct_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::ProductFRU",
    /* Aggregation */
    "\004\001",
    /* Composition */
    "\011\001",
    /* Version */
    "\0632.7.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_FRUIncludesProduct =
{
    /* flags */
    META_FLAG_ASSOCIATION|META_FLAG_AGGREGATION,
    /* name */
    "CIM_FRUIncludesProduct",
    /* qualifiers */
    __root_PG_InterOp_CIM_FRUIncludesProduct_qualifiers,
    /* super */
    0,
    /* features */
    _CIM_FRUIncludesProduct_features,
};

//==============================================================================
//
// Class: CIM_FRUPhysicalElements
//
//==============================================================================

static const char*
_CIM_FRUPhysicalElements_FRU_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Aggregate */
    "\003\001",
    /* Max */
    "\027\000\000\000\001",
    0,
};

static MetaReference
_CIM_FRUPhysicalElements_FRU =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_AGGREGATE|META_FLAG_READ,
    /* name */
    "FRU",
    /* qualifiers */
    _CIM_FRUPhysicalElements_FRU_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_FRU,
};

static const char*
_CIM_FRUPhysicalElements_Component_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_FRUPhysicalElements_Component =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Component",
    /* qualifiers */
    _CIM_FRUPhysicalElements_Component_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_PhysicalElement,
};

static MetaFeature*
_CIM_FRUPhysicalElements_features[] =
{
    (MetaFeature*)&_CIM_FRUPhysicalElements_FRU,
    (MetaFeature*)&_CIM_FRUPhysicalElements_Component,
    0,
};

static const char*
__root_PG_InterOp_CIM_FRUPhysicalElements_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::ProductFRU",
    /* Aggregation */
    "\004\001",
    /* Composition */
    "\011\001",
    /* Version */
    "\0632.7.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_FRUPhysicalElements =
{
    /* flags */
    META_FLAG_ASSOCIATION|META_FLAG_AGGREGATION,
    /* name */
    "CIM_FRUPhysicalElements",
    /* qualifiers */
    __root_PG_InterOp_CIM_FRUPhysicalElements_qualifiers,
    /* super */
    0,
    /* features */
    _CIM_FRUPhysicalElements_features,
};

//==============================================================================
//
// Class: CIM_LogicalElement
//
//==============================================================================

static MetaFeature*
_CIM_LogicalElement_features[] =
{
    0,
};

static const char*
__root_PG_InterOp_CIM_LogicalElement_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Core::CoreElements",
    /* Abstract */
    "\002\001",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_LogicalElement =
{
    /* flags */
    META_FLAG_CLASS|META_FLAG_ABSTRACT,
    /* name */
    "CIM_LogicalElement",
    /* qualifiers */
    __root_PG_InterOp_CIM_LogicalElement_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_ManagedSystemElement,
    /* features */
    _CIM_LogicalElement_features,
};

//==============================================================================
//
// Class: CIM_PhysicalElementLocation
//
//==============================================================================

static const char*
_CIM_PhysicalElementLocation_Element_qualifiers[] =
{
    /* Override */
    "\044Element",
    0,
};

static MetaReference
_CIM_PhysicalElementLocation_Element =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Element",
    /* qualifiers */
    _CIM_PhysicalElementLocation_Element_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_PhysicalElement,
};

static const char*
_CIM_PhysicalElementLocation_PhysicalLocation_qualifiers[] =
{
    /* Override */
    "\044PhysicalLocation",
    /* Max */
    "\027\000\000\000\001",
    0,
};

static MetaReference
_CIM_PhysicalElementLocation_PhysicalLocation =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "PhysicalLocation",
    /* qualifiers */
    _CIM_PhysicalElementLocation_PhysicalLocation_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_Location,
};

static MetaFeature*
_CIM_PhysicalElementLocation_features[] =
{
    (MetaFeature*)&_CIM_PhysicalElementLocation_Element,
    (MetaFeature*)&_CIM_PhysicalElementLocation_PhysicalLocation,
    0,
};

static const char*
__root_PG_InterOp_CIM_PhysicalElementLocation_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::Physical",
    /* Version */
    "\0632.10.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_PhysicalElementLocation =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_PhysicalElementLocation",
    /* qualifiers */
    __root_PG_InterOp_CIM_PhysicalElementLocation_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_ElementLocation,
    /* features */
    _CIM_PhysicalElementLocation_features,
};

//==============================================================================
//
// Class: CIM_PhysicalStatisticalInformation
//
//==============================================================================

static const char*
_CIM_PhysicalStatisticalInformation_PhysicalCreationClassName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Propagated */
    "\045CIM_PhysicalElement.CreationClassName",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_PhysicalStatisticalInformation_PhysicalCreationClassName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "PhysicalCreationClassName",
    /* qualifiers */
    _CIM_PhysicalStatisticalInformation_PhysicalCreationClassName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_PhysicalStatisticalInformation_Tag_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Propagated */
    "\045CIM_PhysicalElement.Tag",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_PhysicalStatisticalInformation_Tag =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Tag",
    /* qualifiers */
    _CIM_PhysicalStatisticalInformation_Tag_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_PhysicalStatisticalInformation_CreationClassName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_PhysicalStatisticalInformation_CreationClassName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "CreationClassName",
    /* qualifiers */
    _CIM_PhysicalStatisticalInformation_CreationClassName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_PhysicalStatisticalInformation_Name_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Override */
    "\044Name",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_PhysicalStatisticalInformation_Name =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Name",
    /* qualifiers */
    _CIM_PhysicalStatisticalInformation_Name_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_PhysicalStatisticalInformation_features[] =
{
    (MetaFeature*)&_CIM_PhysicalStatisticalInformation_PhysicalCreationClassName,
    (MetaFeature*)&_CIM_PhysicalStatisticalInformation_Tag,
    (MetaFeature*)&_CIM_PhysicalStatisticalInformation_CreationClassName,
    (MetaFeature*)&_CIM_PhysicalStatisticalInformation_Name,
    0,
};

static const char*
__root_PG_InterOp_CIM_PhysicalStatisticalInformation_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Core::Statistics",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_PhysicalStatisticalInformation =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "CIM_PhysicalStatisticalInformation",
    /* qualifiers */
    __root_PG_InterOp_CIM_PhysicalStatisticalInformation_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_StatisticalInformation,
    /* features */
    _CIM_PhysicalStatisticalInformation_features,
};

//==============================================================================
//
// Class: CIM_PhysicalStatistics
//
//==============================================================================

static const char*
_CIM_PhysicalStatistics_Stats_qualifiers[] =
{
    /* Override */
    "\044Stats",
    /* Weak */
    "\064\001",
    0,
};

static MetaReference
_CIM_PhysicalStatistics_Stats =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ|META_FLAG_WEAK,
    /* name */
    "Stats",
    /* qualifiers */
    _CIM_PhysicalStatistics_Stats_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_PhysicalStatisticalInformation,
};

static const char*
_CIM_PhysicalStatistics_Element_qualifiers[] =
{
    /* Override */
    "\044Element",
    /* Min */
    "\033\000\000\000\001",
    /* Max */
    "\027\000\000\000\001",
    0,
};

static MetaReference
_CIM_PhysicalStatistics_Element =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Element",
    /* qualifiers */
    _CIM_PhysicalStatistics_Element_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_PhysicalElement,
};

static MetaFeature*
_CIM_PhysicalStatistics_features[] =
{
    (MetaFeature*)&_CIM_PhysicalStatistics_Stats,
    (MetaFeature*)&_CIM_PhysicalStatistics_Element,
    0,
};

static const char*
__root_PG_InterOp_CIM_PhysicalStatistics_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::Statistics",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_PhysicalStatistics =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_PhysicalStatistics",
    /* qualifiers */
    __root_PG_InterOp_CIM_PhysicalStatistics_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_Statistics,
    /* features */
    _CIM_PhysicalStatistics_features,
};

//==============================================================================
//
// Class: CIM_ProductSupport
//
//==============================================================================

static const char*
_CIM_ProductSupport_Product_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_ProductSupport_Product =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Product",
    /* qualifiers */
    _CIM_ProductSupport_Product_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_Product,
};

static const char*
_CIM_ProductSupport_Support_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_ProductSupport_Support =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Support",
    /* qualifiers */
    _CIM_ProductSupport_Support_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_SupportAccess,
};

static MetaFeature*
_CIM_ProductSupport_features[] =
{
    (MetaFeature*)&_CIM_ProductSupport_Product,
    (MetaFeature*)&_CIM_ProductSupport_Support,
    0,
};

static const char*
__root_PG_InterOp_CIM_ProductSupport_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::ProductFRU",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ProductSupport =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_ProductSupport",
    /* qualifiers */
    __root_PG_InterOp_CIM_ProductSupport_qualifiers,
    /* super */
    0,
    /* features */
    _CIM_ProductSupport_features,
};

//==============================================================================
//
// Class: CIM_RedundancyGroup
//
//==============================================================================

static const char*
_CIM_RedundancyGroup_CreationClassName_qualifiers[] =
{
    /* Deprecated */
    "\013\000\001No Value\000",
    /* Key */
    "\025\001",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_RedundancyGroup_CreationClassName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "CreationClassName",
    /* qualifiers */
    _CIM_RedundancyGroup_CreationClassName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_RedundancyGroup_Name_qualifiers[] =
{
    /* Deprecated */
    "\013\000\001CIM_RedundancySet.InstanceID\000",
    /* Key */
    "\025\001",
    /* Override */
    "\044Name",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_RedundancyGroup_Name =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Name",
    /* qualifiers */
    _CIM_RedundancyGroup_Name_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_RedundancyGroup_RedundancyStatus_qualifiers[] =
{
    /* Deprecated */
    "\013\000\001CIM_RedundancySet.RedundancyStatus\000",
    /* ValueMap */
    "\061\000\0050\0001\0002\0003\0004\000",
    /* Values */
    "\062\000\005Unknown\000Other\000Fully Redundant\000Degraded Redundancy\000Redundancy Lost\000",
    0,
};

static MetaProperty
_CIM_RedundancyGroup_RedundancyStatus =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "RedundancyStatus",
    /* qualifiers */
    _CIM_RedundancyGroup_RedundancyStatus_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_RedundancyGroup_features[] =
{
    (MetaFeature*)&_CIM_RedundancyGroup_CreationClassName,
    (MetaFeature*)&_CIM_RedundancyGroup_Name,
    (MetaFeature*)&_CIM_RedundancyGroup_RedundancyStatus,
    0,
};

static const char*
__root_PG_InterOp_CIM_RedundancyGroup_qualifiers[] =
{
    /* Deprecated */
    "\013\000\001CIM_RedundancySet\000",
    /* UMLPackagePath */
    "\057CIM::Core::Redundancy",
    /* Version */
    "\0632.13.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_RedundancyGroup =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "CIM_RedundancyGroup",
    /* qualifiers */
    __root_PG_InterOp_CIM_RedundancyGroup_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_LogicalElement,
    /* features */
    _CIM_RedundancyGroup_features,
};

//==============================================================================
//
// Class: CIM_RedundancySet
//
//==============================================================================

static const char*
_CIM_RedundancySet_RedundancyStatus_qualifiers[] =
{
    /* ValueMap */
    "\061\000\0060\0001\0002\0003\0004\0005\000",
    /* Values */
    "\062\000\006Unknown\000DMTF Reserved\000Fully Redundant\000Degraded Redundancy\000Redundancy Lost\000Overall Failure\000",
    /* ModelCorrespondence */
    "\036\000\001CIM_RedundancySet.MinNumberNeeded\000",
    0,
};

static MetaProperty
_CIM_RedundancySet_RedundancyStatus =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "RedundancyStatus",
    /* qualifiers */
    _CIM_RedundancySet_RedundancyStatus_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_RedundancySet_TypeOfSet_qualifiers[] =
{
    /* ValueMap */
    "\061\000\0100\0001\0002\0003\0004\0005\000..\0000x8000..\000",
    /* Values */
    "\062\000\010Unknown\000Other\000N+1\000Load Balanced\000Sparing\000Limited Sparing\000DMTF Reserved\000Vendor Reserved\000",
    /* ArrayType */
    "\005Indexed",
    /* ModelCorrespondence */
    "\036\000\001CIM_RedundancySet.OtherTypeOfSet\000",
    0,
};

static MetaProperty
_CIM_RedundancySet_TypeOfSet =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "TypeOfSet",
    /* qualifiers */
    _CIM_RedundancySet_TypeOfSet_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    0,
    /* value */
    0,
};

static const char*
_CIM_RedundancySet_MinNumberNeeded_qualifiers[] =
{
    /* MinValue */
    "\035\000\000\000\000\000\000\000\000",
    0,
};

static MetaProperty
_CIM_RedundancySet_MinNumberNeeded =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "MinNumberNeeded",
    /* qualifiers */
    _CIM_RedundancySet_MinNumberNeeded_qualifiers,
    /* type */
    CIMTYPE_UINT32,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_RedundancySet_MaxNumberSupported_qualifiers[] =
{
    0,
};

static MetaProperty
_CIM_RedundancySet_MaxNumberSupported =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "MaxNumberSupported",
    /* qualifiers */
    _CIM_RedundancySet_MaxNumberSupported_qualifiers,
    /* type */
    CIMTYPE_UINT32,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_RedundancySet_VendorIdentifyingInfo_qualifiers[] =
{
    0,
};

static MetaProperty
_CIM_RedundancySet_VendorIdentifyingInfo =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "VendorIdentifyingInfo",
    /* qualifiers */
    _CIM_RedundancySet_VendorIdentifyingInfo_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_RedundancySet_OtherTypeOfSet_qualifiers[] =
{
    /* ArrayType */
    "\005Indexed",
    /* ModelCorrespondence */
    "\036\000\001CIM_RedundancySet.TypeOfSet\000",
    0,
};

static MetaProperty
_CIM_RedundancySet_OtherTypeOfSet =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "OtherTypeOfSet",
    /* qualifiers */
    _CIM_RedundancySet_OtherTypeOfSet_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* value */
    0,
};

static const char*
_CIM_RedundancySet_LoadBalanceAlgorithm_qualifiers[] =
{
    /* Write */
    "\065\001",
    /* ValueMap */
    "\061\000\0100\0001\0002\0003\0004\0005\0006\0007\000",
    /* Values */
    "\062\000\010Unknown\000Other\000No Load Balancing\000Round Robin\000Least Blocks\000Least IO\000Address Region\000Product Specific\000",
    /* ModelCorrespondence */
    "\036\000\001CIM_RedundancySet.OtherLoadBalanceAlgorithm\000",
    0,
};

static MetaProperty
_CIM_RedundancySet_LoadBalanceAlgorithm =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_WRITE,
    /* name */
    "LoadBalanceAlgorithm",
    /* qualifiers */
    _CIM_RedundancySet_LoadBalanceAlgorithm_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    "\000\002",
};

static const char*
_CIM_RedundancySet_OtherLoadBalanceAlgorithm_qualifiers[] =
{
    /* Write */
    "\065\001",
    /* ModelCorrespondence */
    "\036\000\001CIM_RedundancySet.LoadBalanceAlgorithm\000",
    0,
};

static MetaProperty
_CIM_RedundancySet_OtherLoadBalanceAlgorithm =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_WRITE,
    /* name */
    "OtherLoadBalanceAlgorithm",
    /* qualifiers */
    _CIM_RedundancySet_OtherLoadBalanceAlgorithm_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_RedundancySet_Failover_FailoverFrom_qualifiers[] =
{
    /* IN */
    "\024\001",
    0,
};

static MetaReference
_CIM_RedundancySet_Failover_FailoverFrom =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_IN,
    /* name */
    "FailoverFrom",
    /* qualifiers */
    _CIM_RedundancySet_Failover_FailoverFrom_qualifiers,
    /* subscript */
    -1,
    /* ref */
    &__root_PG_InterOp_CIM_ManagedElement,
};

static const char*
_CIM_RedundancySet_Failover_FailoverTo_qualifiers[] =
{
    /* IN */
    "\024\001",
    0,
};

static MetaReference
_CIM_RedundancySet_Failover_FailoverTo =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_IN,
    /* name */
    "FailoverTo",
    /* qualifiers */
    _CIM_RedundancySet_Failover_FailoverTo_qualifiers,
    /* subscript */
    -1,
    /* ref */
    &__root_PG_InterOp_CIM_ManagedElement,
};

static MetaFeature*
_CIM_RedundancySet_Failover_parameters[] =
{
    (MetaFeature*)&_CIM_RedundancySet_Failover_FailoverFrom,
    (MetaFeature*)&_CIM_RedundancySet_Failover_FailoverTo,
    0,
};

static const char*
_CIM_RedundancySet_Failover_qualifiers[] =
{
    /* ValueMap */
    "\061\000\0070\0001\0002\0003\0004\000..\00032768..65535\000",
    /* Values */
    "\062\000\007Completed with No Error\000Not Supported\000Unknown/Unspecified Error\000Busy/In Use\000Paramter Error\000DMTF Reserved\000Vendor Reserved\000",
    0,
};

static MetaMethod
_CIM_RedundancySet_Failover =
{
    /* flags */
    META_FLAG_METHOD,
    /* name */
    "CIM_RedundancySet",
    /* qualifiers */
    _CIM_RedundancySet_Failover_qualifiers,
    /* type */
    CIMTYPE_UINT32,
    /* parameters */
    _CIM_RedundancySet_Failover_parameters,
};

static MetaFeature*
_CIM_RedundancySet_features[] =
{
    (MetaFeature*)&_CIM_RedundancySet_RedundancyStatus,
    (MetaFeature*)&_CIM_RedundancySet_TypeOfSet,
    (MetaFeature*)&_CIM_RedundancySet_MinNumberNeeded,
    (MetaFeature*)&_CIM_RedundancySet_MaxNumberSupported,
    (MetaFeature*)&_CIM_RedundancySet_VendorIdentifyingInfo,
    (MetaFeature*)&_CIM_RedundancySet_OtherTypeOfSet,
    (MetaFeature*)&_CIM_RedundancySet_LoadBalanceAlgorithm,
    (MetaFeature*)&_CIM_RedundancySet_OtherLoadBalanceAlgorithm,
    (MetaFeature*)&_CIM_RedundancySet_Failover,
    0,
};

static const char*
__root_PG_InterOp_CIM_RedundancySet_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Core::Redundancy",
    /* Version */
    "\0632.10.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_RedundancySet =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "CIM_RedundancySet",
    /* qualifiers */
    __root_PG_InterOp_CIM_RedundancySet_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_SystemSpecificCollection,
    /* features */
    _CIM_RedundancySet_features,
};

//==============================================================================
//
// Class: CIM_RelatedStatisticalData
//
//==============================================================================

static const char*
_CIM_RelatedStatisticalData_Stats_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_RelatedStatisticalData_Stats =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Stats",
    /* qualifiers */
    _CIM_RelatedStatisticalData_Stats_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_StatisticalData,
};

static const char*
_CIM_RelatedStatisticalData_RelatedStats_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_RelatedStatisticalData_RelatedStats =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "RelatedStats",
    /* qualifiers */
    _CIM_RelatedStatisticalData_RelatedStats_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_StatisticalData,
};

static const char*
_CIM_RelatedStatisticalData_Description_qualifiers[] =
{
    0,
};

static MetaProperty
_CIM_RelatedStatisticalData_Description =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "Description",
    /* qualifiers */
    _CIM_RelatedStatisticalData_Description_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_RelatedStatisticalData_features[] =
{
    (MetaFeature*)&_CIM_RelatedStatisticalData_Stats,
    (MetaFeature*)&_CIM_RelatedStatisticalData_RelatedStats,
    (MetaFeature*)&_CIM_RelatedStatisticalData_Description,
    0,
};

static const char*
__root_PG_InterOp_CIM_RelatedStatisticalData_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::Statistics",
    /* Version */
    "\0632.7.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_RelatedStatisticalData =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_RelatedStatisticalData",
    /* qualifiers */
    __root_PG_InterOp_CIM_RelatedStatisticalData_qualifiers,
    /* super */
    0,
    /* features */
    _CIM_RelatedStatisticalData_features,
};

//==============================================================================
//
// Class: CIM_RelatedStatistics
//
//==============================================================================

static const char*
_CIM_RelatedStatistics_Stats_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_RelatedStatistics_Stats =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Stats",
    /* qualifiers */
    _CIM_RelatedStatistics_Stats_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_StatisticalInformation,
};

static const char*
_CIM_RelatedStatistics_RelatedStats_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_RelatedStatistics_RelatedStats =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "RelatedStats",
    /* qualifiers */
    _CIM_RelatedStatistics_RelatedStats_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_StatisticalInformation,
};

static MetaFeature*
_CIM_RelatedStatistics_features[] =
{
    (MetaFeature*)&_CIM_RelatedStatistics_Stats,
    (MetaFeature*)&_CIM_RelatedStatistics_RelatedStats,
    0,
};

static const char*
__root_PG_InterOp_CIM_RelatedStatistics_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::Statistics",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_RelatedStatistics =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_RelatedStatistics",
    /* qualifiers */
    __root_PG_InterOp_CIM_RelatedStatistics_qualifiers,
    /* super */
    0,
    /* features */
    _CIM_RelatedStatistics_features,
};

//==============================================================================
//
// Class: CIM_SAPStatisticalInformation
//
//==============================================================================

static const char*
_CIM_SAPStatisticalInformation_SystemCreationClassName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Propagated */
    "\045CIM_ServiceAccessPoint.SystemCreationClassName",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_SAPStatisticalInformation_SystemCreationClassName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "SystemCreationClassName",
    /* qualifiers */
    _CIM_SAPStatisticalInformation_SystemCreationClassName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_SAPStatisticalInformation_SystemName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Propagated */
    "\045CIM_ServiceAccessPoint.SystemName",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_SAPStatisticalInformation_SystemName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "SystemName",
    /* qualifiers */
    _CIM_SAPStatisticalInformation_SystemName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_SAPStatisticalInformation_SAPCreationClassName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Propagated */
    "\045CIM_ServiceAccessPoint.CreationClassName",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_SAPStatisticalInformation_SAPCreationClassName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "SAPCreationClassName",
    /* qualifiers */
    _CIM_SAPStatisticalInformation_SAPCreationClassName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_SAPStatisticalInformation_SAPName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Propagated */
    "\045CIM_ServiceAccessPoint.Name",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_SAPStatisticalInformation_SAPName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "SAPName",
    /* qualifiers */
    _CIM_SAPStatisticalInformation_SAPName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_SAPStatisticalInformation_CreationClassName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_SAPStatisticalInformation_CreationClassName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "CreationClassName",
    /* qualifiers */
    _CIM_SAPStatisticalInformation_CreationClassName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_SAPStatisticalInformation_Name_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Override */
    "\044Name",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_SAPStatisticalInformation_Name =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Name",
    /* qualifiers */
    _CIM_SAPStatisticalInformation_Name_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_SAPStatisticalInformation_features[] =
{
    (MetaFeature*)&_CIM_SAPStatisticalInformation_SystemCreationClassName,
    (MetaFeature*)&_CIM_SAPStatisticalInformation_SystemName,
    (MetaFeature*)&_CIM_SAPStatisticalInformation_SAPCreationClassName,
    (MetaFeature*)&_CIM_SAPStatisticalInformation_SAPName,
    (MetaFeature*)&_CIM_SAPStatisticalInformation_CreationClassName,
    (MetaFeature*)&_CIM_SAPStatisticalInformation_Name,
    0,
};

static const char*
__root_PG_InterOp_CIM_SAPStatisticalInformation_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Core::Statistics",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_SAPStatisticalInformation =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "CIM_SAPStatisticalInformation",
    /* qualifiers */
    __root_PG_InterOp_CIM_SAPStatisticalInformation_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_StatisticalInformation,
    /* features */
    _CIM_SAPStatisticalInformation_features,
};

//==============================================================================
//
// Class: CIM_ScopedSettingData
//
//==============================================================================

static MetaFeature*
_CIM_ScopedSettingData_features[] =
{
    0,
};

static const char*
__root_PG_InterOp_CIM_ScopedSettingData_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Core::Settings",
    /* Abstract */
    "\002\001",
    /* Version */
    "\0632.10.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ScopedSettingData =
{
    /* flags */
    META_FLAG_CLASS|META_FLAG_ABSTRACT,
    /* name */
    "CIM_ScopedSettingData",
    /* qualifiers */
    __root_PG_InterOp_CIM_ScopedSettingData_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_SettingData,
    /* features */
    _CIM_ScopedSettingData_features,
};

//==============================================================================
//
// Class: CIM_ServiceStatisticalInformation
//
//==============================================================================

static const char*
_CIM_ServiceStatisticalInformation_SystemCreationClassName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Propagated */
    "\045CIM_Service.SystemCreationClassName",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_ServiceStatisticalInformation_SystemCreationClassName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "SystemCreationClassName",
    /* qualifiers */
    _CIM_ServiceStatisticalInformation_SystemCreationClassName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_ServiceStatisticalInformation_SystemName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Propagated */
    "\045CIM_Service.SystemName",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_ServiceStatisticalInformation_SystemName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "SystemName",
    /* qualifiers */
    _CIM_ServiceStatisticalInformation_SystemName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_ServiceStatisticalInformation_ServiceCreationClassName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Propagated */
    "\045CIM_Service.CreationClassName",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_ServiceStatisticalInformation_ServiceCreationClassName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "ServiceCreationClassName",
    /* qualifiers */
    _CIM_ServiceStatisticalInformation_ServiceCreationClassName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_ServiceStatisticalInformation_ServiceName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Propagated */
    "\045CIM_Service.Name",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_ServiceStatisticalInformation_ServiceName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "ServiceName",
    /* qualifiers */
    _CIM_ServiceStatisticalInformation_ServiceName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_ServiceStatisticalInformation_CreationClassName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_ServiceStatisticalInformation_CreationClassName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "CreationClassName",
    /* qualifiers */
    _CIM_ServiceStatisticalInformation_CreationClassName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_ServiceStatisticalInformation_Name_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Override */
    "\044Name",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_ServiceStatisticalInformation_Name =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Name",
    /* qualifiers */
    _CIM_ServiceStatisticalInformation_Name_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_ServiceStatisticalInformation_features[] =
{
    (MetaFeature*)&_CIM_ServiceStatisticalInformation_SystemCreationClassName,
    (MetaFeature*)&_CIM_ServiceStatisticalInformation_SystemName,
    (MetaFeature*)&_CIM_ServiceStatisticalInformation_ServiceCreationClassName,
    (MetaFeature*)&_CIM_ServiceStatisticalInformation_ServiceName,
    (MetaFeature*)&_CIM_ServiceStatisticalInformation_CreationClassName,
    (MetaFeature*)&_CIM_ServiceStatisticalInformation_Name,
    0,
};

static const char*
__root_PG_InterOp_CIM_ServiceStatisticalInformation_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Core::Statistics",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ServiceStatisticalInformation =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "CIM_ServiceStatisticalInformation",
    /* qualifiers */
    __root_PG_InterOp_CIM_ServiceStatisticalInformation_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_StatisticalInformation,
    /* features */
    _CIM_ServiceStatisticalInformation_features,
};

//==============================================================================
//
// Class: CIM_SettingAssociatedToCapabilities
//
//==============================================================================

static const char*
_CIM_SettingAssociatedToCapabilities_Antecedent_qualifiers[] =
{
    /* Override */
    "\044Antecedent",
    0,
};

static MetaReference
_CIM_SettingAssociatedToCapabilities_Antecedent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Antecedent",
    /* qualifiers */
    _CIM_SettingAssociatedToCapabilities_Antecedent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_Capabilities,
};

static const char*
_CIM_SettingAssociatedToCapabilities_Dependent_qualifiers[] =
{
    /* Override */
    "\044Dependent",
    0,
};

static MetaReference
_CIM_SettingAssociatedToCapabilities_Dependent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Dependent",
    /* qualifiers */
    _CIM_SettingAssociatedToCapabilities_Dependent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_SettingData,
};

static const char*
_CIM_SettingAssociatedToCapabilities_DefaultSetting_qualifiers[] =
{
    /* Required */
    "\050\001",
    0,
};

static MetaProperty
_CIM_SettingAssociatedToCapabilities_DefaultSetting =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "DefaultSetting",
    /* qualifiers */
    _CIM_SettingAssociatedToCapabilities_DefaultSetting_qualifiers,
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* value */
    "\000",
};

static MetaFeature*
_CIM_SettingAssociatedToCapabilities_features[] =
{
    (MetaFeature*)&_CIM_SettingAssociatedToCapabilities_Antecedent,
    (MetaFeature*)&_CIM_SettingAssociatedToCapabilities_Dependent,
    (MetaFeature*)&_CIM_SettingAssociatedToCapabilities_DefaultSetting,
    0,
};

static const char*
__root_PG_InterOp_CIM_SettingAssociatedToCapabilities_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::System::FilesystemServices",
    /* Version */
    "\0632.11.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_SettingAssociatedToCapabilities =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_SettingAssociatedToCapabilities",
    /* qualifiers */
    __root_PG_InterOp_CIM_SettingAssociatedToCapabilities_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_Dependency,
    /* features */
    _CIM_SettingAssociatedToCapabilities_features,
};

//==============================================================================
//
// Class: CIM_SoftwareIdentity
//
//==============================================================================

static const char*
_CIM_SoftwareIdentity_InstanceID_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaProperty
_CIM_SoftwareIdentity_InstanceID =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "InstanceID",
    /* qualifiers */
    _CIM_SoftwareIdentity_InstanceID_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_SoftwareIdentity_MajorVersion_qualifiers[] =
{
    0,
};

static MetaProperty
_CIM_SoftwareIdentity_MajorVersion =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "MajorVersion",
    /* qualifiers */
    _CIM_SoftwareIdentity_MajorVersion_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_SoftwareIdentity_MinorVersion_qualifiers[] =
{
    0,
};

static MetaProperty
_CIM_SoftwareIdentity_MinorVersion =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "MinorVersion",
    /* qualifiers */
    _CIM_SoftwareIdentity_MinorVersion_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_SoftwareIdentity_RevisionNumber_qualifiers[] =
{
    0,
};

static MetaProperty
_CIM_SoftwareIdentity_RevisionNumber =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "RevisionNumber",
    /* qualifiers */
    _CIM_SoftwareIdentity_RevisionNumber_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_SoftwareIdentity_BuildNumber_qualifiers[] =
{
    0,
};

static MetaProperty
_CIM_SoftwareIdentity_BuildNumber =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "BuildNumber",
    /* qualifiers */
    _CIM_SoftwareIdentity_BuildNumber_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_SoftwareIdentity_VersionString_qualifiers[] =
{
    0,
};

static MetaProperty
_CIM_SoftwareIdentity_VersionString =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "VersionString",
    /* qualifiers */
    _CIM_SoftwareIdentity_VersionString_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_SoftwareIdentity_TargetOperatingSystems_qualifiers[] =
{
    0,
};

static MetaProperty
_CIM_SoftwareIdentity_TargetOperatingSystems =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "TargetOperatingSystems",
    /* qualifiers */
    _CIM_SoftwareIdentity_TargetOperatingSystems_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* value */
    0,
};

static const char*
_CIM_SoftwareIdentity_Manufacturer_qualifiers[] =
{
    0,
};

static MetaProperty
_CIM_SoftwareIdentity_Manufacturer =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "Manufacturer",
    /* qualifiers */
    _CIM_SoftwareIdentity_Manufacturer_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_SoftwareIdentity_Languages_qualifiers[] =
{
    0,
};

static MetaProperty
_CIM_SoftwareIdentity_Languages =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "Languages",
    /* qualifiers */
    _CIM_SoftwareIdentity_Languages_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* value */
    0,
};

static const char*
_CIM_SoftwareIdentity_Classifications_qualifiers[] =
{
    /* ValueMap */
    "\061\000\0160\0001\0002\0003\0004\0005\0006\0007\0008\0009\00010\00011\000..\0000x8000..0xFFFF\000",
    /* Values */
    "\062\000\016Unknown\000Other\000Driver\000Configuration Software\000Application Software\000Instrumentation\000Firmware/BIOS\000Diagnostic Software\000Operating System\000Middleware\000Firmware\000BIOS/FCode\000DMTF Reserved\000Vendor Reserved\000",
    /* ArrayType */
    "\005Indexed",
    /* ModelCorrespondence */
    "\036\000\001CIM_SoftwareIdentity.ClassificationDescriptions\000",
    0,
};

static MetaProperty
_CIM_SoftwareIdentity_Classifications =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "Classifications",
    /* qualifiers */
    _CIM_SoftwareIdentity_Classifications_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    0,
    /* value */
    0,
};

static const char*
_CIM_SoftwareIdentity_ClassificationDescriptions_qualifiers[] =
{
    /* ArrayType */
    "\005Indexed",
    /* ModelCorrespondence */
    "\036\000\001CIM_SoftwareIdentity.Classifications\000",
    0,
};

static MetaProperty
_CIM_SoftwareIdentity_ClassificationDescriptions =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "ClassificationDescriptions",
    /* qualifiers */
    _CIM_SoftwareIdentity_ClassificationDescriptions_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* value */
    0,
};

static const char*
_CIM_SoftwareIdentity_SerialNumber_qualifiers[] =
{
    0,
};

static MetaProperty
_CIM_SoftwareIdentity_SerialNumber =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "SerialNumber",
    /* qualifiers */
    _CIM_SoftwareIdentity_SerialNumber_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_SoftwareIdentity_ReleaseDate_qualifiers[] =
{
    0,
};

static MetaProperty
_CIM_SoftwareIdentity_ReleaseDate =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "ReleaseDate",
    /* qualifiers */
    _CIM_SoftwareIdentity_ReleaseDate_qualifiers,
    /* type */
    CIMTYPE_DATETIME,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_SoftwareIdentity_features[] =
{
    (MetaFeature*)&_CIM_SoftwareIdentity_InstanceID,
    (MetaFeature*)&_CIM_SoftwareIdentity_MajorVersion,
    (MetaFeature*)&_CIM_SoftwareIdentity_MinorVersion,
    (MetaFeature*)&_CIM_SoftwareIdentity_RevisionNumber,
    (MetaFeature*)&_CIM_SoftwareIdentity_BuildNumber,
    (MetaFeature*)&_CIM_SoftwareIdentity_VersionString,
    (MetaFeature*)&_CIM_SoftwareIdentity_TargetOperatingSystems,
    (MetaFeature*)&_CIM_SoftwareIdentity_Manufacturer,
    (MetaFeature*)&_CIM_SoftwareIdentity_Languages,
    (MetaFeature*)&_CIM_SoftwareIdentity_Classifications,
    (MetaFeature*)&_CIM_SoftwareIdentity_ClassificationDescriptions,
    (MetaFeature*)&_CIM_SoftwareIdentity_SerialNumber,
    (MetaFeature*)&_CIM_SoftwareIdentity_ReleaseDate,
    0,
};

static const char*
__root_PG_InterOp_CIM_SoftwareIdentity_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Core::Software",
    /* Version */
    "\0632.13.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_SoftwareIdentity =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "CIM_SoftwareIdentity",
    /* qualifiers */
    __root_PG_InterOp_CIM_SoftwareIdentity_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_LogicalElement,
    /* features */
    _CIM_SoftwareIdentity_features,
};

//==============================================================================
//
// Class: CIM_SpareGroup
//
//==============================================================================

static const char*
_CIM_SpareGroup_Failover_FailoverFrom_qualifiers[] =
{
    /* IN */
    "\024\001",
    0,
};

static MetaReference
_CIM_SpareGroup_Failover_FailoverFrom =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_IN,
    /* name */
    "FailoverFrom",
    /* qualifiers */
    _CIM_SpareGroup_Failover_FailoverFrom_qualifiers,
    /* subscript */
    -1,
    /* ref */
    &__root_PG_InterOp_CIM_ManagedSystemElement,
};

static const char*
_CIM_SpareGroup_Failover_FailoverTo_qualifiers[] =
{
    /* IN */
    "\024\001",
    0,
};

static MetaReference
_CIM_SpareGroup_Failover_FailoverTo =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_IN,
    /* name */
    "FailoverTo",
    /* qualifiers */
    _CIM_SpareGroup_Failover_FailoverTo_qualifiers,
    /* subscript */
    -1,
    /* ref */
    &__root_PG_InterOp_CIM_ManagedSystemElement,
};

static MetaFeature*
_CIM_SpareGroup_Failover_parameters[] =
{
    (MetaFeature*)&_CIM_SpareGroup_Failover_FailoverFrom,
    (MetaFeature*)&_CIM_SpareGroup_Failover_FailoverTo,
    0,
};

static const char*
_CIM_SpareGroup_Failover_qualifiers[] =
{
    /* Deprecated */
    "\013\000\001CIM_RedundancySet.FailOver()\000",
    0,
};

static MetaMethod
_CIM_SpareGroup_Failover =
{
    /* flags */
    META_FLAG_METHOD,
    /* name */
    "CIM_SpareGroup",
    /* qualifiers */
    _CIM_SpareGroup_Failover_qualifiers,
    /* type */
    CIMTYPE_UINT32,
    /* parameters */
    _CIM_SpareGroup_Failover_parameters,
};

static MetaFeature*
_CIM_SpareGroup_features[] =
{
    (MetaFeature*)&_CIM_SpareGroup_Failover,
    0,
};

static const char*
__root_PG_InterOp_CIM_SpareGroup_qualifiers[] =
{
    /* Deprecated */
    "\013\000\001CIM_RedundancySet\000",
    /* UMLPackagePath */
    "\057CIM::Core::Redundancy",
    /* Version */
    "\0632.13.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_SpareGroup =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "CIM_SpareGroup",
    /* qualifiers */
    __root_PG_InterOp_CIM_SpareGroup_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_RedundancyGroup,
    /* features */
    _CIM_SpareGroup_features,
};

//==============================================================================
//
// Class: CIM_StatisticsCollection
//
//==============================================================================

static const char*
_CIM_StatisticsCollection_SampleInterval_qualifiers[] =
{
    0,
};

static MetaProperty
_CIM_StatisticsCollection_SampleInterval =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "SampleInterval",
    /* qualifiers */
    _CIM_StatisticsCollection_SampleInterval_qualifiers,
    /* type */
    CIMTYPE_DATETIME,
    /* subscript */
    -1,
    /* value */
    "00000000000000.000000:000",
};

static const char*
_CIM_StatisticsCollection_TimeLastSampled_qualifiers[] =
{
    0,
};

static MetaProperty
_CIM_StatisticsCollection_TimeLastSampled =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "TimeLastSampled",
    /* qualifiers */
    _CIM_StatisticsCollection_TimeLastSampled_qualifiers,
    /* type */
    CIMTYPE_DATETIME,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_StatisticsCollection_features[] =
{
    (MetaFeature*)&_CIM_StatisticsCollection_SampleInterval,
    (MetaFeature*)&_CIM_StatisticsCollection_TimeLastSampled,
    0,
};

static const char*
__root_PG_InterOp_CIM_StatisticsCollection_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Core::Statistics",
    /* Version */
    "\0632.8.1000",
    0,
};

MetaClass
__root_PG_InterOp_CIM_StatisticsCollection =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "CIM_StatisticsCollection",
    /* qualifiers */
    __root_PG_InterOp_CIM_StatisticsCollection_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_SystemSpecificCollection,
    /* features */
    _CIM_StatisticsCollection_features,
};

//==============================================================================
//
// Class: CIM_StorageRedundancyGroup
//
//==============================================================================

static const char*
_CIM_StorageRedundancyGroup_TypeOfAlgorithm_qualifiers[] =
{
    /* Deprecated */
    "\013\000\001CIM_StorageRedundancySet.TypeOfAlgorithm\000",
    /* ValueMap */
    "\061\000\0100\0001\0002\0003\0004\0005\0006\0007\000",
    /* Values */
    "\062\000\010None\000Other\000Unknown\000Copy\000XOR\000P+Q\000S\000P+S\000",
    /* MappingStrings */
    "\026\000\001MIF.DMTF|Redundancy Group|001.2\000",
    0,
};

static MetaProperty
_CIM_StorageRedundancyGroup_TypeOfAlgorithm =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "TypeOfAlgorithm",
    /* qualifiers */
    _CIM_StorageRedundancyGroup_TypeOfAlgorithm_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_StorageRedundancyGroup_StorageRedundancy_qualifiers[] =
{
    /* Deprecated */
    "\013\000\001No Value\000",
    /* ValueMap */
    "\061\000\0110\0001\0002\0003\0004\0005\0006\0007\0008\000",
    /* Values */
    "\062\000\011No Additional Status\000Reconfig In Progress\000Data Lost\000Not Currently Configured\000Protected Rebuild\000Redundancy Disabled\000Unprotected Rebuild\000Recalculating\000Verifying\000",
    0,
};

static MetaProperty
_CIM_StorageRedundancyGroup_StorageRedundancy =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "StorageRedundancy",
    /* qualifiers */
    _CIM_StorageRedundancyGroup_StorageRedundancy_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_StorageRedundancyGroup_IsStriped_qualifiers[] =
{
    /* Deprecated */
    "\013\000\001CIM_CompositeExtent.ExtentStripeLength\000",
    0,
};

static MetaProperty
_CIM_StorageRedundancyGroup_IsStriped =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "IsStriped",
    /* qualifiers */
    _CIM_StorageRedundancyGroup_IsStriped_qualifiers,
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_StorageRedundancyGroup_IsConcatenated_qualifiers[] =
{
    /* Deprecated */
    "\013\000\001CIM_CompositeExtent.IsConcatenated\000",
    0,
};

static MetaProperty
_CIM_StorageRedundancyGroup_IsConcatenated =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "IsConcatenated",
    /* qualifiers */
    _CIM_StorageRedundancyGroup_IsConcatenated_qualifiers,
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_StorageRedundancyGroup_features[] =
{
    (MetaFeature*)&_CIM_StorageRedundancyGroup_TypeOfAlgorithm,
    (MetaFeature*)&_CIM_StorageRedundancyGroup_StorageRedundancy,
    (MetaFeature*)&_CIM_StorageRedundancyGroup_IsStriped,
    (MetaFeature*)&_CIM_StorageRedundancyGroup_IsConcatenated,
    0,
};

static const char*
__root_PG_InterOp_CIM_StorageRedundancyGroup_qualifiers[] =
{
    /* Deprecated */
    "\013\000\001CIM_StorageRedundancySet\000",
    /* UMLPackagePath */
    "\057CIM::Core::Redundancy",
    /* Version */
    "\0632.13.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_StorageRedundancyGroup =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "CIM_StorageRedundancyGroup",
    /* qualifiers */
    __root_PG_InterOp_CIM_StorageRedundancyGroup_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_RedundancyGroup,
    /* features */
    _CIM_StorageRedundancyGroup_features,
};

//==============================================================================
//
// Class: CIM_StorageRedundancySet
//
//==============================================================================

static const char*
_CIM_StorageRedundancySet_TypeOfAlgorithm_qualifiers[] =
{
    /* ValueMap */
    "\061\000\0070\0001\0003\0004\0005\0006\0007\000",
    /* Values */
    "\062\000\007Unknown\000Other\000Copy\000XOR\000P+Q\000S\000P+S\000",
    /* MappingStrings */
    "\026\000\001MIF.DMTF|Redundancy Group|001.2\000",
    /* ModelCorrespondence */
    "\036\000\001CIM_StorageRedundancySet.OtherAlgorithm\000",
    0,
};

static MetaProperty
_CIM_StorageRedundancySet_TypeOfAlgorithm =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "TypeOfAlgorithm",
    /* qualifiers */
    _CIM_StorageRedundancySet_TypeOfAlgorithm_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_StorageRedundancySet_OtherAlgorithm_qualifiers[] =
{
    /* ModelCorrespondence */
    "\036\000\001CIM_StorageRedundancySet.TypeOfAlgorithm\000",
    0,
};

static MetaProperty
_CIM_StorageRedundancySet_OtherAlgorithm =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "OtherAlgorithm",
    /* qualifiers */
    _CIM_StorageRedundancySet_OtherAlgorithm_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_StorageRedundancySet_features[] =
{
    (MetaFeature*)&_CIM_StorageRedundancySet_TypeOfAlgorithm,
    (MetaFeature*)&_CIM_StorageRedundancySet_OtherAlgorithm,
    0,
};

static const char*
__root_PG_InterOp_CIM_StorageRedundancySet_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Core::Redundancy",
    /* Version */
    "\0632.8.1000",
    0,
};

MetaClass
__root_PG_InterOp_CIM_StorageRedundancySet =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "CIM_StorageRedundancySet",
    /* qualifiers */
    __root_PG_InterOp_CIM_StorageRedundancySet_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_RedundancySet,
    /* features */
    _CIM_StorageRedundancySet_features,
};

//==============================================================================
//
// Class: CIM_ActsAsSpare
//
//==============================================================================

static const char*
_CIM_ActsAsSpare_Group_qualifiers[] =
{
    /* Deprecated */
    "\013\000\001CIM_IsSpare.Dependent\000",
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_ActsAsSpare_Group =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Group",
    /* qualifiers */
    _CIM_ActsAsSpare_Group_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_SpareGroup,
};

static const char*
_CIM_ActsAsSpare_Spare_qualifiers[] =
{
    /* Deprecated */
    "\013\000\001CIM_IsSpare.Antecedent\000",
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_ActsAsSpare_Spare =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Spare",
    /* qualifiers */
    _CIM_ActsAsSpare_Spare_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ManagedSystemElement,
};

static const char*
_CIM_ActsAsSpare_HotStandby_qualifiers[] =
{
    /* Deprecated */
    "\013\000\001CIM_IsSpare.SpareStatus\000",
    0,
};

static MetaProperty
_CIM_ActsAsSpare_HotStandby =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "HotStandby",
    /* qualifiers */
    _CIM_ActsAsSpare_HotStandby_qualifiers,
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_ActsAsSpare_features[] =
{
    (MetaFeature*)&_CIM_ActsAsSpare_Group,
    (MetaFeature*)&_CIM_ActsAsSpare_Spare,
    (MetaFeature*)&_CIM_ActsAsSpare_HotStandby,
    0,
};

static const char*
__root_PG_InterOp_CIM_ActsAsSpare_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* Deprecated */
    "\013\000\001CIM_IsSpare\000",
    /* UMLPackagePath */
    "\057CIM::Core::Redundancy",
    /* Version */
    "\0632.13.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ActsAsSpare =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_ActsAsSpare",
    /* qualifiers */
    __root_PG_InterOp_CIM_ActsAsSpare_qualifiers,
    /* super */
    0,
    /* features */
    _CIM_ActsAsSpare_features,
};

//==============================================================================
//
// Class: CIM_DefaultSetting
//
//==============================================================================

static const char*
_CIM_DefaultSetting_Setting_qualifiers[] =
{
    /* Override */
    "\044Setting",
    /* Max */
    "\027\000\000\000\001",
    0,
};

static MetaReference
_CIM_DefaultSetting_Setting =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Setting",
    /* qualifiers */
    _CIM_DefaultSetting_Setting_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_Setting,
};

static MetaFeature*
_CIM_DefaultSetting_features[] =
{
    (MetaFeature*)&_CIM_DefaultSetting_Setting,
    0,
};

static const char*
__root_PG_InterOp_CIM_DefaultSetting_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::Settings",
    /* Version */
    "\0632.10.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_DefaultSetting =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_DefaultSetting",
    /* qualifiers */
    __root_PG_InterOp_CIM_DefaultSetting_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_ElementSetting,
    /* features */
    _CIM_DefaultSetting_features,
};

//==============================================================================
//
// Class: CIM_ElementSoftwareIdentity
//
//==============================================================================

static const char*
_CIM_ElementSoftwareIdentity_Antecedent_qualifiers[] =
{
    /* Override */
    "\044Antecedent",
    0,
};

static MetaReference
_CIM_ElementSoftwareIdentity_Antecedent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Antecedent",
    /* qualifiers */
    _CIM_ElementSoftwareIdentity_Antecedent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_SoftwareIdentity,
};

static const char*
_CIM_ElementSoftwareIdentity_Dependent_qualifiers[] =
{
    /* Override */
    "\044Dependent",
    0,
};

static MetaReference
_CIM_ElementSoftwareIdentity_Dependent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Dependent",
    /* qualifiers */
    _CIM_ElementSoftwareIdentity_Dependent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ManagedElement,
};

static const char*
_CIM_ElementSoftwareIdentity_UpgradeCondition_qualifiers[] =
{
    /* ValueMap */
    "\061\000\0100\0001\0002\0003\0004\0005\000..\0000x8000..0xFFFF\000",
    /* Values */
    "\062\000\010Unknown\000Other\000Resides off device\000Owner Upgradeable\000Factory Upgradeable\000Not Upgradeable\000DMTF Reserved\000Vendor Reserved\000",
    /* ModelCorrespondence */
    "\036\000\001CIM_ElementSoftwareIdentity.OtherUpgradeCondition\000",
    0,
};

static MetaProperty
_CIM_ElementSoftwareIdentity_UpgradeCondition =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "UpgradeCondition",
    /* qualifiers */
    _CIM_ElementSoftwareIdentity_UpgradeCondition_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_ElementSoftwareIdentity_OtherUpgradeCondition_qualifiers[] =
{
    /* ModelCorrespondence */
    "\036\000\001CIM_ElementSoftwareIdentity.UpgradeCondition\000",
    0,
};

static MetaProperty
_CIM_ElementSoftwareIdentity_OtherUpgradeCondition =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "OtherUpgradeCondition",
    /* qualifiers */
    _CIM_ElementSoftwareIdentity_OtherUpgradeCondition_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_ElementSoftwareIdentity_features[] =
{
    (MetaFeature*)&_CIM_ElementSoftwareIdentity_Antecedent,
    (MetaFeature*)&_CIM_ElementSoftwareIdentity_Dependent,
    (MetaFeature*)&_CIM_ElementSoftwareIdentity_UpgradeCondition,
    (MetaFeature*)&_CIM_ElementSoftwareIdentity_OtherUpgradeCondition,
    0,
};

static const char*
__root_PG_InterOp_CIM_ElementSoftwareIdentity_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::Software",
    /* Version */
    "\0632.8.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ElementSoftwareIdentity =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_ElementSoftwareIdentity",
    /* qualifiers */
    __root_PG_InterOp_CIM_ElementSoftwareIdentity_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_Dependency,
    /* features */
    _CIM_ElementSoftwareIdentity_features,
};

//==============================================================================
//
// Class: CIM_ExtraCapacityGroup
//
//==============================================================================

static const char*
_CIM_ExtraCapacityGroup_MinNumberNeeded_qualifiers[] =
{
    /* Deprecated */
    "\013\000\001CIM_RedundancySet.MinNumberNeeded\000",
    0,
};

static MetaProperty
_CIM_ExtraCapacityGroup_MinNumberNeeded =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "MinNumberNeeded",
    /* qualifiers */
    _CIM_ExtraCapacityGroup_MinNumberNeeded_qualifiers,
    /* type */
    CIMTYPE_UINT32,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_ExtraCapacityGroup_LoadBalancedGroup_qualifiers[] =
{
    /* Deprecated */
    "\013\000\001CIM_RedundancySet.TypeOfSet\000",
    0,
};

static MetaProperty
_CIM_ExtraCapacityGroup_LoadBalancedGroup =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "LoadBalancedGroup",
    /* qualifiers */
    _CIM_ExtraCapacityGroup_LoadBalancedGroup_qualifiers,
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_ExtraCapacityGroup_features[] =
{
    (MetaFeature*)&_CIM_ExtraCapacityGroup_MinNumberNeeded,
    (MetaFeature*)&_CIM_ExtraCapacityGroup_LoadBalancedGroup,
    0,
};

static const char*
__root_PG_InterOp_CIM_ExtraCapacityGroup_qualifiers[] =
{
    /* Deprecated */
    "\013\000\001CIM_RedundancySet\000",
    /* UMLPackagePath */
    "\057CIM::Core::Redundancy",
    /* Version */
    "\0632.13.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ExtraCapacityGroup =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "CIM_ExtraCapacityGroup",
    /* qualifiers */
    __root_PG_InterOp_CIM_ExtraCapacityGroup_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_RedundancyGroup,
    /* features */
    _CIM_ExtraCapacityGroup_features,
};

//==============================================================================
//
// Class: CIM_IsSpare
//
//==============================================================================

static const char*
_CIM_IsSpare_Antecedent_qualifiers[] =
{
    /* Override */
    "\044Antecedent",
    0,
};

static MetaReference
_CIM_IsSpare_Antecedent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Antecedent",
    /* qualifiers */
    _CIM_IsSpare_Antecedent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ManagedElement,
};

static const char*
_CIM_IsSpare_Dependent_qualifiers[] =
{
    /* Override */
    "\044Dependent",
    0,
};

static MetaReference
_CIM_IsSpare_Dependent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Dependent",
    /* qualifiers */
    _CIM_IsSpare_Dependent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_RedundancySet,
};

static const char*
_CIM_IsSpare_SpareStatus_qualifiers[] =
{
    /* ValueMap */
    "\061\000\0030\0002\0003\000",
    /* Values */
    "\062\000\003Unknown\000Hot Standby\000Cold Standby\000",
    0,
};

static MetaProperty
_CIM_IsSpare_SpareStatus =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "SpareStatus",
    /* qualifiers */
    _CIM_IsSpare_SpareStatus_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_IsSpare_FailoverSupported_qualifiers[] =
{
    /* ValueMap */
    "\061\000\0040\0002\0003\0004\000",
    /* Values */
    "\062\000\004Unknown\000Automatic\000Manual\000Both Manual and Automatic\000",
    0,
};

static MetaProperty
_CIM_IsSpare_FailoverSupported =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "FailoverSupported",
    /* qualifiers */
    _CIM_IsSpare_FailoverSupported_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_IsSpare_features[] =
{
    (MetaFeature*)&_CIM_IsSpare_Antecedent,
    (MetaFeature*)&_CIM_IsSpare_Dependent,
    (MetaFeature*)&_CIM_IsSpare_SpareStatus,
    (MetaFeature*)&_CIM_IsSpare_FailoverSupported,
    0,
};

static const char*
__root_PG_InterOp_CIM_IsSpare_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::Redundancy",
    /* Version */
    "\0632.10.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_IsSpare =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_IsSpare",
    /* qualifiers */
    __root_PG_InterOp_CIM_IsSpare_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_Dependency,
    /* features */
    _CIM_IsSpare_features,
};

//==============================================================================
//
// Class: CIM_Job
//
//==============================================================================

static const char*
_CIM_Job_JobStatus_qualifiers[] =
{
    /* ModelCorrespondence */
    "\036\000\001CIM_ManagedSystemElement.OperationalStatus\000",
    0,
};

static MetaProperty
_CIM_Job_JobStatus =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "JobStatus",
    /* qualifiers */
    _CIM_Job_JobStatus_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Job_TimeSubmitted_qualifiers[] =
{
    0,
};

static MetaProperty
_CIM_Job_TimeSubmitted =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "TimeSubmitted",
    /* qualifiers */
    _CIM_Job_TimeSubmitted_qualifiers,
    /* type */
    CIMTYPE_DATETIME,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Job_ScheduledStartTime_qualifiers[] =
{
    /* Deprecated */
    "\013\000\004CIM_Job.RunMonth\000CIM_Job.RunDay\000CIM_Job.RunDayOfWeek\000CIM_Job.RunStartInterval\000",
    /* Write */
    "\065\001",
    0,
};

static MetaProperty
_CIM_Job_ScheduledStartTime =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_WRITE,
    /* name */
    "ScheduledStartTime",
    /* qualifiers */
    _CIM_Job_ScheduledStartTime_qualifiers,
    /* type */
    CIMTYPE_DATETIME,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Job_StartTime_qualifiers[] =
{
    0,
};

static MetaProperty
_CIM_Job_StartTime =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "StartTime",
    /* qualifiers */
    _CIM_Job_StartTime_qualifiers,
    /* type */
    CIMTYPE_DATETIME,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Job_ElapsedTime_qualifiers[] =
{
    0,
};

static MetaProperty
_CIM_Job_ElapsedTime =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "ElapsedTime",
    /* qualifiers */
    _CIM_Job_ElapsedTime_qualifiers,
    /* type */
    CIMTYPE_DATETIME,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Job_JobRunTimes_qualifiers[] =
{
    /* Write */
    "\065\001",
    0,
};

static MetaProperty
_CIM_Job_JobRunTimes =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_WRITE,
    /* name */
    "JobRunTimes",
    /* qualifiers */
    _CIM_Job_JobRunTimes_qualifiers,
    /* type */
    CIMTYPE_UINT32,
    /* subscript */
    -1,
    /* value */
    "\000\000\000\001",
};

static const char*
_CIM_Job_RunMonth_qualifiers[] =
{
    /* Write */
    "\065\001",
    /* ValueMap */
    "\061\000\0140\0001\0002\0003\0004\0005\0006\0007\0008\0009\00010\00011\000",
    /* Values */
    "\062\000\014January\000February\000March\000April\000May\000June\000July\000August\000September\000October\000November\000December\000",
    /* ModelCorrespondence */
    "\036\000\003CIM_Job.RunDay\000CIM_Job.RunDayOfWeek\000CIM_Job.RunStartInterval\000",
    0,
};

static MetaProperty
_CIM_Job_RunMonth =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_WRITE,
    /* name */
    "RunMonth",
    /* qualifiers */
    _CIM_Job_RunMonth_qualifiers,
    /* type */
    CIMTYPE_UINT8,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Job_RunDay_qualifiers[] =
{
    /* Write */
    "\065\001",
    /* MinValue */
    "\035\377\377\377\377\377\377\377\341",
    /* MaxValue */
    "\031\000\000\000\000\000\000\000\037",
    /* ModelCorrespondence */
    "\036\000\003CIM_Job.RunMonth\000CIM_Job.RunDayOfWeek\000CIM_Job.RunStartInterval\000",
    0,
};

static MetaProperty
_CIM_Job_RunDay =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_WRITE,
    /* name */
    "RunDay",
    /* qualifiers */
    _CIM_Job_RunDay_qualifiers,
    /* type */
    CIMTYPE_SINT8,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Job_RunDayOfWeek_qualifiers[] =
{
    /* Write */
    "\065\001",
    /* ValueMap */
    "\061\000\017-7\000-6\000-5\000-4\000-3\000-2\000-1\0000\0001\0002\0003\0004\0005\0006\0007\000",
    /* Values */
    "\062\000\017-Saturday\000-Friday\000-Thursday\000-Wednesday\000-Tuesday\000-Monday\000-Sunday\000ExactDayOfMonth\000Sunday\000Monday\000Tuesday\000Wednesday\000Thursday\000Friday\000Saturday\000",
    /* ModelCorrespondence */
    "\036\000\003CIM_Job.RunMonth\000CIM_Job.RunDay\000CIM_Job.RunStartInterval\000",
    0,
};

static MetaProperty
_CIM_Job_RunDayOfWeek =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_WRITE,
    /* name */
    "RunDayOfWeek",
    /* qualifiers */
    _CIM_Job_RunDayOfWeek_qualifiers,
    /* type */
    CIMTYPE_SINT8,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Job_RunStartInterval_qualifiers[] =
{
    /* Write */
    "\065\001",
    /* ModelCorrespondence */
    "\036\000\004CIM_Job.RunMonth\000CIM_Job.RunDay\000CIM_Job.RunDayOfWeek\000CIM_Job.RunStartInterval\000",
    0,
};

static MetaProperty
_CIM_Job_RunStartInterval =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_WRITE,
    /* name */
    "RunStartInterval",
    /* qualifiers */
    _CIM_Job_RunStartInterval_qualifiers,
    /* type */
    CIMTYPE_DATETIME,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Job_LocalOrUtcTime_qualifiers[] =
{
    /* Write */
    "\065\001",
    /* ValueMap */
    "\061\000\0021\0002\000",
    /* Values */
    "\062\000\002Local Time\000UTC Time\000",
    0,
};

static MetaProperty
_CIM_Job_LocalOrUtcTime =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_WRITE,
    /* name */
    "LocalOrUtcTime",
    /* qualifiers */
    _CIM_Job_LocalOrUtcTime_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Job_UntilTime_qualifiers[] =
{
    /* Write */
    "\065\001",
    /* ModelCorrespondence */
    "\036\000\001CIM_Job.LocalOrUtcTime\000",
    0,
};

static MetaProperty
_CIM_Job_UntilTime =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_WRITE,
    /* name */
    "UntilTime",
    /* qualifiers */
    _CIM_Job_UntilTime_qualifiers,
    /* type */
    CIMTYPE_DATETIME,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Job_Notify_qualifiers[] =
{
    /* Write */
    "\065\001",
    0,
};

static MetaProperty
_CIM_Job_Notify =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_WRITE,
    /* name */
    "Notify",
    /* qualifiers */
    _CIM_Job_Notify_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Job_Owner_qualifiers[] =
{
    /* ModelCorrespondence */
    "\036\000\001CIM_OwningJobElement\000",
    0,
};

static MetaProperty
_CIM_Job_Owner =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "Owner",
    /* qualifiers */
    _CIM_Job_Owner_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Job_Priority_qualifiers[] =
{
    /* Write */
    "\065\001",
    0,
};

static MetaProperty
_CIM_Job_Priority =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_WRITE,
    /* name */
    "Priority",
    /* qualifiers */
    _CIM_Job_Priority_qualifiers,
    /* type */
    CIMTYPE_UINT32,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Job_PercentComplete_qualifiers[] =
{
    /* Units */
    "\060Percent",
    /* MinValue */
    "\035\000\000\000\000\000\000\000\000",
    /* MaxValue */
    "\031\000\000\000\000\000\000\000\145",
    0,
};

static MetaProperty
_CIM_Job_PercentComplete =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "PercentComplete",
    /* qualifiers */
    _CIM_Job_PercentComplete_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Job_DeleteOnCompletion_qualifiers[] =
{
    /* Write */
    "\065\001",
    0,
};

static MetaProperty
_CIM_Job_DeleteOnCompletion =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_WRITE,
    /* name */
    "DeleteOnCompletion",
    /* qualifiers */
    _CIM_Job_DeleteOnCompletion_qualifiers,
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Job_ErrorCode_qualifiers[] =
{
    /* ModelCorrespondence */
    "\036\000\001CIM_Job.ErrorDescription\000",
    0,
};

static MetaProperty
_CIM_Job_ErrorCode =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "ErrorCode",
    /* qualifiers */
    _CIM_Job_ErrorCode_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Job_ErrorDescription_qualifiers[] =
{
    /* ModelCorrespondence */
    "\036\000\001CIM_Job.ErrorCode\000",
    0,
};

static MetaProperty
_CIM_Job_ErrorDescription =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "ErrorDescription",
    /* qualifiers */
    _CIM_Job_ErrorDescription_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Job_RecoveryAction_qualifiers[] =
{
    /* ValueMap */
    "\061\000\0060\0001\0002\0003\0004\0005\000",
    /* Values */
    "\062\000\006Unknown\000Other\000Do Not Continue\000Continue With Next Job\000Re-run Job\000Run Recovery Job\000",
    /* ModelCorrespondence */
    "\036\000\001CIM_Job.OtherRecoveryAction\000",
    0,
};

static MetaProperty
_CIM_Job_RecoveryAction =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "RecoveryAction",
    /* qualifiers */
    _CIM_Job_RecoveryAction_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Job_OtherRecoveryAction_qualifiers[] =
{
    /* ModelCorrespondence */
    "\036\000\001CIM_Job.RecoveryAction\000",
    0,
};

static MetaProperty
_CIM_Job_OtherRecoveryAction =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "OtherRecoveryAction",
    /* qualifiers */
    _CIM_Job_OtherRecoveryAction_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Job_KillJob_DeleteOnKill_qualifiers[] =
{
    /* IN */
    "\024\001",
    0,
};

static MetaProperty
_CIM_Job_KillJob_DeleteOnKill =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_IN,
    /* name */
    "DeleteOnKill",
    /* qualifiers */
    _CIM_Job_KillJob_DeleteOnKill_qualifiers,
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_Job_KillJob_parameters[] =
{
    (MetaFeature*)&_CIM_Job_KillJob_DeleteOnKill,
    0,
};

static const char*
_CIM_Job_KillJob_qualifiers[] =
{
    /* Deprecated */
    "\013\000\001CIM_ConcreteJob.RequestStateChange()\000",
    /* ValueMap */
    "\061\000\0110\0001\0002\0003\0004\0006\0007\000..\00032768..65535\000",
    /* Values */
    "\062\000\011Success\000Not Supported\000Unknown\000Timeout\000Failed\000Access Denied\000Not Found\000DMTF Reserved\000Vendor Specific\000",
    0,
};

static MetaMethod
_CIM_Job_KillJob =
{
    /* flags */
    META_FLAG_METHOD,
    /* name */
    "CIM_Job",
    /* qualifiers */
    _CIM_Job_KillJob_qualifiers,
    /* type */
    CIMTYPE_UINT32,
    /* parameters */
    _CIM_Job_KillJob_parameters,
};

static MetaFeature*
_CIM_Job_features[] =
{
    (MetaFeature*)&_CIM_Job_JobStatus,
    (MetaFeature*)&_CIM_Job_TimeSubmitted,
    (MetaFeature*)&_CIM_Job_ScheduledStartTime,
    (MetaFeature*)&_CIM_Job_StartTime,
    (MetaFeature*)&_CIM_Job_ElapsedTime,
    (MetaFeature*)&_CIM_Job_JobRunTimes,
    (MetaFeature*)&_CIM_Job_RunMonth,
    (MetaFeature*)&_CIM_Job_RunDay,
    (MetaFeature*)&_CIM_Job_RunDayOfWeek,
    (MetaFeature*)&_CIM_Job_RunStartInterval,
    (MetaFeature*)&_CIM_Job_LocalOrUtcTime,
    (MetaFeature*)&_CIM_Job_UntilTime,
    (MetaFeature*)&_CIM_Job_Notify,
    (MetaFeature*)&_CIM_Job_Owner,
    (MetaFeature*)&_CIM_Job_Priority,
    (MetaFeature*)&_CIM_Job_PercentComplete,
    (MetaFeature*)&_CIM_Job_DeleteOnCompletion,
    (MetaFeature*)&_CIM_Job_ErrorCode,
    (MetaFeature*)&_CIM_Job_ErrorDescription,
    (MetaFeature*)&_CIM_Job_RecoveryAction,
    (MetaFeature*)&_CIM_Job_OtherRecoveryAction,
    (MetaFeature*)&_CIM_Job_KillJob,
    0,
};

static const char*
__root_PG_InterOp_CIM_Job_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Core::CoreElements",
    /* Abstract */
    "\002\001",
    /* Version */
    "\0632.10.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_Job =
{
    /* flags */
    META_FLAG_CLASS|META_FLAG_ABSTRACT,
    /* name */
    "CIM_Job",
    /* qualifiers */
    __root_PG_InterOp_CIM_Job_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_LogicalElement,
    /* features */
    _CIM_Job_features,
};

//==============================================================================
//
// Class: CIM_ProductSoftwareComponent
//
//==============================================================================

static const char*
_CIM_ProductSoftwareComponent_GroupComponent_qualifiers[] =
{
    /* Override */
    "\044GroupComponent",
    /* Max */
    "\027\000\000\000\001",
    0,
};

static MetaReference
_CIM_ProductSoftwareComponent_GroupComponent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "GroupComponent",
    /* qualifiers */
    _CIM_ProductSoftwareComponent_GroupComponent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_Product,
};

static const char*
_CIM_ProductSoftwareComponent_PartComponent_qualifiers[] =
{
    /* Override */
    "\044PartComponent",
    0,
};

static MetaReference
_CIM_ProductSoftwareComponent_PartComponent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "PartComponent",
    /* qualifiers */
    _CIM_ProductSoftwareComponent_PartComponent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_SoftwareIdentity,
};

static MetaFeature*
_CIM_ProductSoftwareComponent_features[] =
{
    (MetaFeature*)&_CIM_ProductSoftwareComponent_GroupComponent,
    (MetaFeature*)&_CIM_ProductSoftwareComponent_PartComponent,
    0,
};

static const char*
__root_PG_InterOp_CIM_ProductSoftwareComponent_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::ProductFRU",
    /* Aggregation */
    "\004\001",
    /* Composition */
    "\011\001",
    /* Version */
    "\0632.7.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ProductSoftwareComponent =
{
    /* flags */
    META_FLAG_ASSOCIATION|META_FLAG_AGGREGATION,
    /* name */
    "CIM_ProductSoftwareComponent",
    /* qualifiers */
    __root_PG_InterOp_CIM_ProductSoftwareComponent_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_Component,
    /* features */
    _CIM_ProductSoftwareComponent_features,
};

//==============================================================================
//
// Class: CIM_RedundancyComponent
//
//==============================================================================

static const char*
_CIM_RedundancyComponent_GroupComponent_qualifiers[] =
{
    /* Deprecated */
    "\013\000\001CIM_MemberOfCollection.Collection\000",
    /* Aggregate */
    "\003\001",
    /* Override */
    "\044GroupComponent",
    0,
};

static MetaReference
_CIM_RedundancyComponent_GroupComponent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_AGGREGATE|META_FLAG_READ,
    /* name */
    "GroupComponent",
    /* qualifiers */
    _CIM_RedundancyComponent_GroupComponent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_RedundancyGroup,
};

static const char*
_CIM_RedundancyComponent_PartComponent_qualifiers[] =
{
    /* Deprecated */
    "\013\000\001CIM_MemberOfCollection.Member\000",
    /* Override */
    "\044PartComponent",
    0,
};

static MetaReference
_CIM_RedundancyComponent_PartComponent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "PartComponent",
    /* qualifiers */
    _CIM_RedundancyComponent_PartComponent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ManagedSystemElement,
};

static MetaFeature*
_CIM_RedundancyComponent_features[] =
{
    (MetaFeature*)&_CIM_RedundancyComponent_GroupComponent,
    (MetaFeature*)&_CIM_RedundancyComponent_PartComponent,
    0,
};

static const char*
__root_PG_InterOp_CIM_RedundancyComponent_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* Deprecated */
    "\013\000\001CIM_MemberOfCollection\000",
    /* UMLPackagePath */
    "\057CIM::Core::Redundancy",
    /* Aggregation */
    "\004\001",
    /* Version */
    "\0632.13.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_RedundancyComponent =
{
    /* flags */
    META_FLAG_ASSOCIATION|META_FLAG_AGGREGATION,
    /* name */
    "CIM_RedundancyComponent",
    /* qualifiers */
    __root_PG_InterOp_CIM_RedundancyComponent_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_Component,
    /* features */
    _CIM_RedundancyComponent_features,
};

//==============================================================================
//
// Class: CIM_ScopedSetting
//
//==============================================================================

static const char*
_CIM_ScopedSetting_Antecedent_qualifiers[] =
{
    /* Override */
    "\044Antecedent",
    /* Min */
    "\033\000\000\000\001",
    /* Max */
    "\027\000\000\000\001",
    0,
};

static MetaReference
_CIM_ScopedSetting_Antecedent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Antecedent",
    /* qualifiers */
    _CIM_ScopedSetting_Antecedent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ManagedElement,
};

static const char*
_CIM_ScopedSetting_Dependent_qualifiers[] =
{
    /* Override */
    "\044Dependent",
    0,
};

static MetaReference
_CIM_ScopedSetting_Dependent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Dependent",
    /* qualifiers */
    _CIM_ScopedSetting_Dependent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ScopedSettingData,
};

static MetaFeature*
_CIM_ScopedSetting_features[] =
{
    (MetaFeature*)&_CIM_ScopedSetting_Antecedent,
    (MetaFeature*)&_CIM_ScopedSetting_Dependent,
    0,
};

static const char*
__root_PG_InterOp_CIM_ScopedSetting_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::Settings",
    /* Version */
    "\0632.10.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ScopedSetting =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_ScopedSetting",
    /* qualifiers */
    __root_PG_InterOp_CIM_ScopedSetting_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_HostedDependency,
    /* features */
    _CIM_ScopedSetting_features,
};

//==============================================================================
//
// Class: CIM_ConcreteJob
//
//==============================================================================

static const char*
_CIM_ConcreteJob_InstanceID_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaProperty
_CIM_ConcreteJob_InstanceID =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "InstanceID",
    /* qualifiers */
    _CIM_ConcreteJob_InstanceID_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_ConcreteJob_Name_qualifiers[] =
{
    /* Required */
    "\050\001",
    /* Override */
    "\044Name",
    0,
};

static MetaProperty
_CIM_ConcreteJob_Name =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "Name",
    /* qualifiers */
    _CIM_ConcreteJob_Name_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_ConcreteJob_JobState_qualifiers[] =
{
    /* ValueMap */
    "\061\000\0152\0003\0004\0005\0006\0007\0008\0009\00010\00011\00012\00013..32767\00032768..65535\000",
    /* Values */
    "\062\000\015New\000Starting\000Running\000Suspended\000Shutting Down\000Completed\000Terminated\000Killed\000Exception\000Service\000Query Pending\000DMTF Reserved\000Vendor Reserved\000",
    0,
};

static MetaProperty
_CIM_ConcreteJob_JobState =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "JobState",
    /* qualifiers */
    _CIM_ConcreteJob_JobState_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_ConcreteJob_TimeOfLastStateChange_qualifiers[] =
{
    0,
};

static MetaProperty
_CIM_ConcreteJob_TimeOfLastStateChange =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "TimeOfLastStateChange",
    /* qualifiers */
    _CIM_ConcreteJob_TimeOfLastStateChange_qualifiers,
    /* type */
    CIMTYPE_DATETIME,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_ConcreteJob_TimeBeforeRemoval_qualifiers[] =
{
    /* Required */
    "\050\001",
    /* Write */
    "\065\001",
    0,
};

static MetaProperty
_CIM_ConcreteJob_TimeBeforeRemoval =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED|META_FLAG_WRITE,
    /* name */
    "TimeBeforeRemoval",
    /* qualifiers */
    _CIM_ConcreteJob_TimeBeforeRemoval_qualifiers,
    /* type */
    CIMTYPE_DATETIME,
    /* subscript */
    -1,
    /* value */
    "00000000000500.000000:000",
};

static const char*
_CIM_ConcreteJob_RequestStateChange_RequestedState_qualifiers[] =
{
    /* IN */
    "\024\001",
    /* ValueMap */
    "\061\000\0072\0003\0004\0005\0006\0007..32767\00032768..65535\000",
    /* Values */
    "\062\000\007Start\000Suspend\000Terminate\000Kill\000Service\000DMTF Reserved\000Vendor Reserved\000",
    0,
};

static MetaProperty
_CIM_ConcreteJob_RequestStateChange_RequestedState =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_IN,
    /* name */
    "RequestedState",
    /* qualifiers */
    _CIM_ConcreteJob_RequestStateChange_RequestedState_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_ConcreteJob_RequestStateChange_TimeoutPeriod_qualifiers[] =
{
    /* IN */
    "\024\001",
    0,
};

static MetaProperty
_CIM_ConcreteJob_RequestStateChange_TimeoutPeriod =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_IN,
    /* name */
    "TimeoutPeriod",
    /* qualifiers */
    _CIM_ConcreteJob_RequestStateChange_TimeoutPeriod_qualifiers,
    /* type */
    CIMTYPE_DATETIME,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_ConcreteJob_RequestStateChange_parameters[] =
{
    (MetaFeature*)&_CIM_ConcreteJob_RequestStateChange_RequestedState,
    (MetaFeature*)&_CIM_ConcreteJob_RequestStateChange_TimeoutPeriod,
    0,
};

static const char*
_CIM_ConcreteJob_RequestStateChange_qualifiers[] =
{
    /* ValueMap */
    "\061\000\0160\0001\0002\0003\0004\0005\0006\000..\0004096\0004097\0004098\0004099\0004100..32767\00032768..65535\000",
    /* Values */
    "\062\000\016Completed with No Error\000Not Supported\000Unknown/Unspecified Error\000Can NOT complete within Timeout Period\000Failed\000Invalid Parameter\000In Use\000DMTF Reserved\000Method Parameters Checked - Transition Started\000Invalid State Transition\000Use of Timeout Parameter Not Supported\000Busy\000Method Reserved\000Vendor Specific\000",
    0,
};

static MetaMethod
_CIM_ConcreteJob_RequestStateChange =
{
    /* flags */
    META_FLAG_METHOD,
    /* name */
    "CIM_ConcreteJob",
    /* qualifiers */
    _CIM_ConcreteJob_RequestStateChange_qualifiers,
    /* type */
    CIMTYPE_UINT32,
    /* parameters */
    _CIM_ConcreteJob_RequestStateChange_parameters,
};

static MetaFeature*
_CIM_ConcreteJob_features[] =
{
    (MetaFeature*)&_CIM_ConcreteJob_InstanceID,
    (MetaFeature*)&_CIM_ConcreteJob_Name,
    (MetaFeature*)&_CIM_ConcreteJob_JobState,
    (MetaFeature*)&_CIM_ConcreteJob_TimeOfLastStateChange,
    (MetaFeature*)&_CIM_ConcreteJob_TimeBeforeRemoval,
    (MetaFeature*)&_CIM_ConcreteJob_RequestStateChange,
    0,
};

static const char*
__root_PG_InterOp_CIM_ConcreteJob_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Core::CoreElements",
    /* Version */
    "\0632.13.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ConcreteJob =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "CIM_ConcreteJob",
    /* qualifiers */
    __root_PG_InterOp_CIM_ConcreteJob_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_Job,
    /* features */
    _CIM_ConcreteJob_features,
};

//==============================================================================
//
// Class: CIM_EnabledLogicalElement
//
//==============================================================================

static const char*
_CIM_EnabledLogicalElement_EnabledState_qualifiers[] =
{
    /* ValueMap */
    "\061\000\0150\0001\0002\0003\0004\0005\0006\0007\0008\0009\00010\00011..32767\00032768..65535\000",
    /* Values */
    "\062\000\015Unknown\000Other\000Enabled\000Disabled\000Shutting Down\000Not Applicable\000Enabled but Offline\000In Test\000Deferred\000Quiesce\000Starting\000DMTF Reserved\000Vendor Reserved\000",
    /* ModelCorrespondence */
    "\036\000\001CIM_EnabledLogicalElement.OtherEnabledState\000",
    0,
};

static MetaProperty
_CIM_EnabledLogicalElement_EnabledState =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "EnabledState",
    /* qualifiers */
    _CIM_EnabledLogicalElement_EnabledState_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    "\000\005",
};

static const char*
_CIM_EnabledLogicalElement_OtherEnabledState_qualifiers[] =
{
    /* ModelCorrespondence */
    "\036\000\001CIM_EnabledLogicalElement.EnabledState\000",
    0,
};

static MetaProperty
_CIM_EnabledLogicalElement_OtherEnabledState =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "OtherEnabledState",
    /* qualifiers */
    _CIM_EnabledLogicalElement_OtherEnabledState_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_EnabledLogicalElement_RequestedState_qualifiers[] =
{
    /* ValueMap */
    "\061\000\0152\0003\0004\0005\0006\0007\0008\0009\00010\00011\00012\000..\00032768..65535\000",
    /* Values */
    "\062\000\015Enabled\000Disabled\000Shut Down\000No Change\000Offline\000Test\000Deferred\000Quiesce\000Reboot\000Reset\000Not Applicable\000DMTF Reserved\000Vendor Reserved\000",
    /* ModelCorrespondence */
    "\036\000\001CIM_EnabledLogicalElement.EnabledState\000",
    0,
};

static MetaProperty
_CIM_EnabledLogicalElement_RequestedState =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "RequestedState",
    /* qualifiers */
    _CIM_EnabledLogicalElement_RequestedState_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    "\000\014",
};

static const char*
_CIM_EnabledLogicalElement_EnabledDefault_qualifiers[] =
{
    /* Write */
    "\065\001",
    /* ValueMap */
    "\061\000\0072\0003\0005\0006\0007\0008..32767\00032768..65535\000",
    /* Values */
    "\062\000\007Enabled\000Disabled\000Not Applicable\000Enabled but Offline\000No Default\000DMTF Reserved\000Vendor Reserved\000",
    0,
};

static MetaProperty
_CIM_EnabledLogicalElement_EnabledDefault =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_WRITE,
    /* name */
    "EnabledDefault",
    /* qualifiers */
    _CIM_EnabledLogicalElement_EnabledDefault_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    "\000\002",
};

static const char*
_CIM_EnabledLogicalElement_TimeOfLastStateChange_qualifiers[] =
{
    0,
};

static MetaProperty
_CIM_EnabledLogicalElement_TimeOfLastStateChange =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "TimeOfLastStateChange",
    /* qualifiers */
    _CIM_EnabledLogicalElement_TimeOfLastStateChange_qualifiers,
    /* type */
    CIMTYPE_DATETIME,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_EnabledLogicalElement_RequestStateChange_RequestedState_qualifiers[] =
{
    /* IN */
    "\024\001",
    /* ValueMap */
    "\061\000\0132\0003\0004\0006\0007\0008\0009\00010\00011\000..\00032768..65535\000",
    /* Values */
    "\062\000\013Enabled\000Disabled\000Shut Down\000Offline\000Test\000Defer\000Quiesce\000Reboot\000Reset\000DMTF Reserved\000Vendor Reserved\000",
    /* ModelCorrespondence */
    "\036\000\001CIM_EnabledLogicalElement.RequestedState\000",
    0,
};

static MetaProperty
_CIM_EnabledLogicalElement_RequestStateChange_RequestedState =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_IN,
    /* name */
    "RequestedState",
    /* qualifiers */
    _CIM_EnabledLogicalElement_RequestStateChange_RequestedState_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_EnabledLogicalElement_RequestStateChange_Job_qualifiers[] =
{
    /* IN */
    "\024\000",
    /* OUT */
    "\043\001",
    0,
};

static MetaReference
_CIM_EnabledLogicalElement_RequestStateChange_Job =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_OUT,
    /* name */
    "Job",
    /* qualifiers */
    _CIM_EnabledLogicalElement_RequestStateChange_Job_qualifiers,
    /* subscript */
    -1,
    /* ref */
    &__root_PG_InterOp_CIM_ConcreteJob,
};

static const char*
_CIM_EnabledLogicalElement_RequestStateChange_TimeoutPeriod_qualifiers[] =
{
    /* IN */
    "\024\001",
    0,
};

static MetaProperty
_CIM_EnabledLogicalElement_RequestStateChange_TimeoutPeriod =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_IN,
    /* name */
    "TimeoutPeriod",
    /* qualifiers */
    _CIM_EnabledLogicalElement_RequestStateChange_TimeoutPeriod_qualifiers,
    /* type */
    CIMTYPE_DATETIME,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_EnabledLogicalElement_RequestStateChange_parameters[] =
{
    (MetaFeature*)&_CIM_EnabledLogicalElement_RequestStateChange_RequestedState,
    (MetaFeature*)&_CIM_EnabledLogicalElement_RequestStateChange_Job,
    (MetaFeature*)&_CIM_EnabledLogicalElement_RequestStateChange_TimeoutPeriod,
    0,
};

static const char*
_CIM_EnabledLogicalElement_RequestStateChange_qualifiers[] =
{
    /* ValueMap */
    "\061\000\0160\0001\0002\0003\0004\0005\0006\000..\0004096\0004097\0004098\0004099\0004100..32767\00032768..65535\000",
    /* Values */
    "\062\000\016Completed with No Error\000Not Supported\000Unknown or Unspecified Error\000Cannot complete within Timeout Period\000Failed\000Invalid Parameter\000In Use\000DMTF Reserved\000Method Parameters Checked - Job Started\000Invalid State Transition\000Use of Timeout Parameter Not Supported\000Busy\000Method Reserved\000Vendor Specific\000",
    /* ModelCorrespondence */
    "\036\000\001CIM_EnabledLogicalElement.RequestedState\000",
    0,
};

static MetaMethod
_CIM_EnabledLogicalElement_RequestStateChange =
{
    /* flags */
    META_FLAG_METHOD,
    /* name */
    "CIM_EnabledLogicalElement",
    /* qualifiers */
    _CIM_EnabledLogicalElement_RequestStateChange_qualifiers,
    /* type */
    CIMTYPE_UINT32,
    /* parameters */
    _CIM_EnabledLogicalElement_RequestStateChange_parameters,
};

static MetaFeature*
_CIM_EnabledLogicalElement_features[] =
{
    (MetaFeature*)&_CIM_EnabledLogicalElement_EnabledState,
    (MetaFeature*)&_CIM_EnabledLogicalElement_OtherEnabledState,
    (MetaFeature*)&_CIM_EnabledLogicalElement_RequestedState,
    (MetaFeature*)&_CIM_EnabledLogicalElement_EnabledDefault,
    (MetaFeature*)&_CIM_EnabledLogicalElement_TimeOfLastStateChange,
    (MetaFeature*)&_CIM_EnabledLogicalElement_RequestStateChange,
    0,
};

static const char*
__root_PG_InterOp_CIM_EnabledLogicalElement_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Core::CoreElements",
    /* Abstract */
    "\002\001",
    /* Version */
    "\0632.10.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_EnabledLogicalElement =
{
    /* flags */
    META_FLAG_CLASS|META_FLAG_ABSTRACT,
    /* name */
    "CIM_EnabledLogicalElement",
    /* qualifiers */
    __root_PG_InterOp_CIM_EnabledLogicalElement_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_LogicalElement,
    /* features */
    _CIM_EnabledLogicalElement_features,
};

//==============================================================================
//
// Class: CIM_LogicalDevice
//
//==============================================================================

static const char*
_CIM_LogicalDevice_SystemCreationClassName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Propagated */
    "\045CIM_System.CreationClassName",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_LogicalDevice_SystemCreationClassName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "SystemCreationClassName",
    /* qualifiers */
    _CIM_LogicalDevice_SystemCreationClassName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_LogicalDevice_SystemName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Propagated */
    "\045CIM_System.Name",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_LogicalDevice_SystemName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "SystemName",
    /* qualifiers */
    _CIM_LogicalDevice_SystemName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_LogicalDevice_CreationClassName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_LogicalDevice_CreationClassName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "CreationClassName",
    /* qualifiers */
    _CIM_LogicalDevice_CreationClassName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_LogicalDevice_DeviceID_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* MaxLen */
    "\030\000\000\000\100",
    0,
};

static MetaProperty
_CIM_LogicalDevice_DeviceID =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "DeviceID",
    /* qualifiers */
    _CIM_LogicalDevice_DeviceID_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_LogicalDevice_PowerManagementSupported_qualifiers[] =
{
    /* Deprecated */
    "\013\000\001CIM_PowerManagementCapabilities\000",
    0,
};

static MetaProperty
_CIM_LogicalDevice_PowerManagementSupported =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "PowerManagementSupported",
    /* qualifiers */
    _CIM_LogicalDevice_PowerManagementSupported_qualifiers,
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_LogicalDevice_PowerManagementCapabilities_qualifiers[] =
{
    /* Deprecated */
    "\013\000\001CIM_PowerManagementCapabilities.PowerCapabilities\000",
    /* ValueMap */
    "\061\000\0100\0001\0002\0003\0004\0005\0006\0007\000",
    /* Values */
    "\062\000\010Unknown\000Not Supported\000Disabled\000Enabled\000Power Saving Modes Entered Automatically\000Power State Settable\000Power Cycling Supported\000Timed Power On Supported\000",
    0,
};

static MetaProperty
_CIM_LogicalDevice_PowerManagementCapabilities =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "PowerManagementCapabilities",
    /* qualifiers */
    _CIM_LogicalDevice_PowerManagementCapabilities_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    0,
    /* value */
    0,
};

static const char*
_CIM_LogicalDevice_Availability_qualifiers[] =
{
    /* ValueMap */
    "\061\000\0251\0002\0003\0004\0005\0006\0007\0008\0009\00010\00011\00012\00013\00014\00015\00016\00017\00018\00019\00020\00021\000",
    /* Values */
    "\062\000\025Other\000Unknown\000Running/Full Power\000Warning\000In Test\000Not Applicable\000Power Off\000Off Line\000Off Duty\000Degraded\000Not Installed\000Install Error\000Power Save - Unknown\000Power Save - Low Power Mode\000Power Save - Standby\000Power Cycle\000Power Save - Warning\000Paused\000Not Ready\000Not Configured\000Quiesced\000",
    /* MappingStrings */
    "\026\000\003MIF.DMTF|Operational State|006.5\000MIB.IETF|HOST-RESOURCES-MIB.hrDeviceStatus\000MIF.DMTF|Host Device|001.5\000",
    /* ModelCorrespondence */
    "\036\000\001CIM_LogicalDevice.AdditionalAvailability\000",
    0,
};

static MetaProperty
_CIM_LogicalDevice_Availability =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "Availability",
    /* qualifiers */
    _CIM_LogicalDevice_Availability_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_LogicalDevice_StatusInfo_qualifiers[] =
{
    /* Deprecated */
    "\013\000\001CIM_EnabledLogicalElement.EnabledState\000",
    /* ValueMap */
    "\061\000\0051\0002\0003\0004\0005\000",
    /* Values */
    "\062\000\005Other\000Unknown\000Enabled\000Disabled\000Not Applicable\000",
    /* MappingStrings */
    "\026\000\001MIF.DMTF|Operational State|006.4\000",
    0,
};

static MetaProperty
_CIM_LogicalDevice_StatusInfo =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "StatusInfo",
    /* qualifiers */
    _CIM_LogicalDevice_StatusInfo_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_LogicalDevice_LastErrorCode_qualifiers[] =
{
    /* Deprecated */
    "\013\000\001CIM_DeviceErrorData.LastErrorCode\000",
    0,
};

static MetaProperty
_CIM_LogicalDevice_LastErrorCode =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "LastErrorCode",
    /* qualifiers */
    _CIM_LogicalDevice_LastErrorCode_qualifiers,
    /* type */
    CIMTYPE_UINT32,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_LogicalDevice_ErrorDescription_qualifiers[] =
{
    /* Deprecated */
    "\013\000\001CIM_DeviceErrorData.ErrorDescription\000",
    0,
};

static MetaProperty
_CIM_LogicalDevice_ErrorDescription =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "ErrorDescription",
    /* qualifiers */
    _CIM_LogicalDevice_ErrorDescription_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_LogicalDevice_ErrorCleared_qualifiers[] =
{
    /* Deprecated */
    "\013\000\001CIM_ManagedSystemElement.OperationalStatus\000",
    0,
};

static MetaProperty
_CIM_LogicalDevice_ErrorCleared =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "ErrorCleared",
    /* qualifiers */
    _CIM_LogicalDevice_ErrorCleared_qualifiers,
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_LogicalDevice_OtherIdentifyingInfo_qualifiers[] =
{
    /* MaxLen */
    "\030\000\000\001\000",
    /* ArrayType */
    "\005Indexed",
    /* ModelCorrespondence */
    "\036\000\001CIM_LogicalDevice.IdentifyingDescriptions\000",
    0,
};

static MetaProperty
_CIM_LogicalDevice_OtherIdentifyingInfo =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "OtherIdentifyingInfo",
    /* qualifiers */
    _CIM_LogicalDevice_OtherIdentifyingInfo_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* value */
    0,
};

static const char*
_CIM_LogicalDevice_PowerOnHours_qualifiers[] =
{
    /* Units */
    "\060Hours",
    /* Counter */
    "\012\001",
    0,
};

static MetaProperty
_CIM_LogicalDevice_PowerOnHours =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_COUNTER|META_FLAG_READ,
    /* name */
    "PowerOnHours",
    /* qualifiers */
    _CIM_LogicalDevice_PowerOnHours_qualifiers,
    /* type */
    CIMTYPE_UINT64,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_LogicalDevice_TotalPowerOnHours_qualifiers[] =
{
    /* Units */
    "\060Hours",
    /* Counter */
    "\012\001",
    0,
};

static MetaProperty
_CIM_LogicalDevice_TotalPowerOnHours =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_COUNTER|META_FLAG_READ,
    /* name */
    "TotalPowerOnHours",
    /* qualifiers */
    _CIM_LogicalDevice_TotalPowerOnHours_qualifiers,
    /* type */
    CIMTYPE_UINT64,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_LogicalDevice_IdentifyingDescriptions_qualifiers[] =
{
    /* ArrayType */
    "\005Indexed",
    /* ModelCorrespondence */
    "\036\000\001CIM_LogicalDevice.OtherIdentifyingInfo\000",
    0,
};

static MetaProperty
_CIM_LogicalDevice_IdentifyingDescriptions =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "IdentifyingDescriptions",
    /* qualifiers */
    _CIM_LogicalDevice_IdentifyingDescriptions_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* value */
    0,
};

static const char*
_CIM_LogicalDevice_AdditionalAvailability_qualifiers[] =
{
    /* ValueMap */
    "\061\000\0251\0002\0003\0004\0005\0006\0007\0008\0009\00010\00011\00012\00013\00014\00015\00016\00017\00018\00019\00020\00021\000",
    /* Values */
    "\062\000\025Other\000Unknown\000Running/Full Power\000Warning\000In Test\000Not Applicable\000Power Off\000Off Line\000Off Duty\000Degraded\000Not Installed\000Install Error\000Power Save - Unknown\000Power Save - Low Power Mode\000Power Save - Standby\000Power Cycle\000Power Save - Warning\000Paused\000Not Ready\000Not Configured\000Quiesced\000",
    /* ModelCorrespondence */
    "\036\000\001CIM_LogicalDevice.Availability\000",
    0,
};

static MetaProperty
_CIM_LogicalDevice_AdditionalAvailability =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "AdditionalAvailability",
    /* qualifiers */
    _CIM_LogicalDevice_AdditionalAvailability_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    0,
    /* value */
    0,
};

static const char*
_CIM_LogicalDevice_MaxQuiesceTime_qualifiers[] =
{
    /* Deprecated */
    "\013\000\001No value\000",
    /* Units */
    "\060MilliSeconds",
    0,
};

static MetaProperty
_CIM_LogicalDevice_MaxQuiesceTime =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "MaxQuiesceTime",
    /* qualifiers */
    _CIM_LogicalDevice_MaxQuiesceTime_qualifiers,
    /* type */
    CIMTYPE_UINT64,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_LogicalDevice_SetPowerState_PowerState_qualifiers[] =
{
    /* IN */
    "\024\001",
    /* ValueMap */
    "\061\000\0061\0002\0003\0004\0005\0006\000",
    /* Values */
    "\062\000\006Full Power\000Power Save - Low Power Mode\000Power Save - Standby\000Power Save - Other\000Power Cycle\000Power Off\000",
    0,
};

static MetaProperty
_CIM_LogicalDevice_SetPowerState_PowerState =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_IN,
    /* name */
    "PowerState",
    /* qualifiers */
    _CIM_LogicalDevice_SetPowerState_PowerState_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_LogicalDevice_SetPowerState_Time_qualifiers[] =
{
    /* IN */
    "\024\001",
    0,
};

static MetaProperty
_CIM_LogicalDevice_SetPowerState_Time =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_IN,
    /* name */
    "Time",
    /* qualifiers */
    _CIM_LogicalDevice_SetPowerState_Time_qualifiers,
    /* type */
    CIMTYPE_DATETIME,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_LogicalDevice_SetPowerState_parameters[] =
{
    (MetaFeature*)&_CIM_LogicalDevice_SetPowerState_PowerState,
    (MetaFeature*)&_CIM_LogicalDevice_SetPowerState_Time,
    0,
};

static const char*
_CIM_LogicalDevice_SetPowerState_qualifiers[] =
{
    /* Deprecated */
    "\013\000\001CIM_PowerManagementService.SetPowerState\000",
    0,
};

static MetaMethod
_CIM_LogicalDevice_SetPowerState =
{
    /* flags */
    META_FLAG_METHOD,
    /* name */
    "CIM_LogicalDevice",
    /* qualifiers */
    _CIM_LogicalDevice_SetPowerState_qualifiers,
    /* type */
    CIMTYPE_UINT32,
    /* parameters */
    _CIM_LogicalDevice_SetPowerState_parameters,
};

static MetaFeature*
_CIM_LogicalDevice_Reset_parameters[] =
{
    0,
};

static const char*
_CIM_LogicalDevice_Reset_qualifiers[] =
{
    0,
};

static MetaMethod
_CIM_LogicalDevice_Reset =
{
    /* flags */
    META_FLAG_METHOD,
    /* name */
    "CIM_LogicalDevice",
    /* qualifiers */
    _CIM_LogicalDevice_Reset_qualifiers,
    /* type */
    CIMTYPE_UINT32,
    /* parameters */
    _CIM_LogicalDevice_Reset_parameters,
};

static const char*
_CIM_LogicalDevice_EnableDevice_Enabled_qualifiers[] =
{
    /* IN */
    "\024\001",
    0,
};

static MetaProperty
_CIM_LogicalDevice_EnableDevice_Enabled =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_IN,
    /* name */
    "Enabled",
    /* qualifiers */
    _CIM_LogicalDevice_EnableDevice_Enabled_qualifiers,
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_LogicalDevice_EnableDevice_parameters[] =
{
    (MetaFeature*)&_CIM_LogicalDevice_EnableDevice_Enabled,
    0,
};

static const char*
_CIM_LogicalDevice_EnableDevice_qualifiers[] =
{
    /* Deprecated */
    "\013\000\001CIM_EnabledLogicalElement.RequestStateChange\000",
    0,
};

static MetaMethod
_CIM_LogicalDevice_EnableDevice =
{
    /* flags */
    META_FLAG_METHOD,
    /* name */
    "CIM_LogicalDevice",
    /* qualifiers */
    _CIM_LogicalDevice_EnableDevice_qualifiers,
    /* type */
    CIMTYPE_UINT32,
    /* parameters */
    _CIM_LogicalDevice_EnableDevice_parameters,
};

static const char*
_CIM_LogicalDevice_OnlineDevice_Online_qualifiers[] =
{
    /* IN */
    "\024\001",
    0,
};

static MetaProperty
_CIM_LogicalDevice_OnlineDevice_Online =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_IN,
    /* name */
    "Online",
    /* qualifiers */
    _CIM_LogicalDevice_OnlineDevice_Online_qualifiers,
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_LogicalDevice_OnlineDevice_parameters[] =
{
    (MetaFeature*)&_CIM_LogicalDevice_OnlineDevice_Online,
    0,
};

static const char*
_CIM_LogicalDevice_OnlineDevice_qualifiers[] =
{
    /* Deprecated */
    "\013\000\001CIM_EnabledLogicalElement.RequestStateChange\000",
    0,
};

static MetaMethod
_CIM_LogicalDevice_OnlineDevice =
{
    /* flags */
    META_FLAG_METHOD,
    /* name */
    "CIM_LogicalDevice",
    /* qualifiers */
    _CIM_LogicalDevice_OnlineDevice_qualifiers,
    /* type */
    CIMTYPE_UINT32,
    /* parameters */
    _CIM_LogicalDevice_OnlineDevice_parameters,
};

static const char*
_CIM_LogicalDevice_QuiesceDevice_Quiesce_qualifiers[] =
{
    /* IN */
    "\024\001",
    0,
};

static MetaProperty
_CIM_LogicalDevice_QuiesceDevice_Quiesce =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_IN,
    /* name */
    "Quiesce",
    /* qualifiers */
    _CIM_LogicalDevice_QuiesceDevice_Quiesce_qualifiers,
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_LogicalDevice_QuiesceDevice_parameters[] =
{
    (MetaFeature*)&_CIM_LogicalDevice_QuiesceDevice_Quiesce,
    0,
};

static const char*
_CIM_LogicalDevice_QuiesceDevice_qualifiers[] =
{
    /* Deprecated */
    "\013\000\001CIM_EnabledLogicalElement.RequestStateChange\000",
    0,
};

static MetaMethod
_CIM_LogicalDevice_QuiesceDevice =
{
    /* flags */
    META_FLAG_METHOD,
    /* name */
    "CIM_LogicalDevice",
    /* qualifiers */
    _CIM_LogicalDevice_QuiesceDevice_qualifiers,
    /* type */
    CIMTYPE_UINT32,
    /* parameters */
    _CIM_LogicalDevice_QuiesceDevice_parameters,
};

static MetaFeature*
_CIM_LogicalDevice_SaveProperties_parameters[] =
{
    0,
};

static const char*
_CIM_LogicalDevice_SaveProperties_qualifiers[] =
{
    0,
};

static MetaMethod
_CIM_LogicalDevice_SaveProperties =
{
    /* flags */
    META_FLAG_METHOD,
    /* name */
    "CIM_LogicalDevice",
    /* qualifiers */
    _CIM_LogicalDevice_SaveProperties_qualifiers,
    /* type */
    CIMTYPE_UINT32,
    /* parameters */
    _CIM_LogicalDevice_SaveProperties_parameters,
};

static MetaFeature*
_CIM_LogicalDevice_RestoreProperties_parameters[] =
{
    0,
};

static const char*
_CIM_LogicalDevice_RestoreProperties_qualifiers[] =
{
    0,
};

static MetaMethod
_CIM_LogicalDevice_RestoreProperties =
{
    /* flags */
    META_FLAG_METHOD,
    /* name */
    "CIM_LogicalDevice",
    /* qualifiers */
    _CIM_LogicalDevice_RestoreProperties_qualifiers,
    /* type */
    CIMTYPE_UINT32,
    /* parameters */
    _CIM_LogicalDevice_RestoreProperties_parameters,
};

static MetaFeature*
_CIM_LogicalDevice_features[] =
{
    (MetaFeature*)&_CIM_LogicalDevice_SystemCreationClassName,
    (MetaFeature*)&_CIM_LogicalDevice_SystemName,
    (MetaFeature*)&_CIM_LogicalDevice_CreationClassName,
    (MetaFeature*)&_CIM_LogicalDevice_DeviceID,
    (MetaFeature*)&_CIM_LogicalDevice_PowerManagementSupported,
    (MetaFeature*)&_CIM_LogicalDevice_PowerManagementCapabilities,
    (MetaFeature*)&_CIM_LogicalDevice_Availability,
    (MetaFeature*)&_CIM_LogicalDevice_StatusInfo,
    (MetaFeature*)&_CIM_LogicalDevice_LastErrorCode,
    (MetaFeature*)&_CIM_LogicalDevice_ErrorDescription,
    (MetaFeature*)&_CIM_LogicalDevice_ErrorCleared,
    (MetaFeature*)&_CIM_LogicalDevice_OtherIdentifyingInfo,
    (MetaFeature*)&_CIM_LogicalDevice_PowerOnHours,
    (MetaFeature*)&_CIM_LogicalDevice_TotalPowerOnHours,
    (MetaFeature*)&_CIM_LogicalDevice_IdentifyingDescriptions,
    (MetaFeature*)&_CIM_LogicalDevice_AdditionalAvailability,
    (MetaFeature*)&_CIM_LogicalDevice_MaxQuiesceTime,
    (MetaFeature*)&_CIM_LogicalDevice_SetPowerState,
    (MetaFeature*)&_CIM_LogicalDevice_Reset,
    (MetaFeature*)&_CIM_LogicalDevice_EnableDevice,
    (MetaFeature*)&_CIM_LogicalDevice_OnlineDevice,
    (MetaFeature*)&_CIM_LogicalDevice_QuiesceDevice,
    (MetaFeature*)&_CIM_LogicalDevice_SaveProperties,
    (MetaFeature*)&_CIM_LogicalDevice_RestoreProperties,
    0,
};

static const char*
__root_PG_InterOp_CIM_LogicalDevice_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Core::Device",
    /* Abstract */
    "\002\001",
    /* Version */
    "\0632.8.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_LogicalDevice =
{
    /* flags */
    META_FLAG_CLASS|META_FLAG_ABSTRACT,
    /* name */
    "CIM_LogicalDevice",
    /* qualifiers */
    __root_PG_InterOp_CIM_LogicalDevice_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_EnabledLogicalElement,
    /* features */
    _CIM_LogicalDevice_features,
};

//==============================================================================
//
// Class: CIM_Realizes
//
//==============================================================================

static const char*
_CIM_Realizes_Antecedent_qualifiers[] =
{
    /* Override */
    "\044Antecedent",
    0,
};

static MetaReference
_CIM_Realizes_Antecedent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Antecedent",
    /* qualifiers */
    _CIM_Realizes_Antecedent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_PhysicalElement,
};

static const char*
_CIM_Realizes_Dependent_qualifiers[] =
{
    /* Override */
    "\044Dependent",
    0,
};

static MetaReference
_CIM_Realizes_Dependent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Dependent",
    /* qualifiers */
    _CIM_Realizes_Dependent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_LogicalDevice,
};

static MetaFeature*
_CIM_Realizes_features[] =
{
    (MetaFeature*)&_CIM_Realizes_Antecedent,
    (MetaFeature*)&_CIM_Realizes_Dependent,
    0,
};

static const char*
__root_PG_InterOp_CIM_Realizes_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::Device",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_Realizes =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_Realizes",
    /* qualifiers */
    __root_PG_InterOp_CIM_Realizes_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_Dependency,
    /* features */
    _CIM_Realizes_features,
};

//==============================================================================
//
// Class: CIM_Service
//
//==============================================================================

static const char*
_CIM_Service_SystemCreationClassName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Propagated */
    "\045CIM_System.CreationClassName",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_Service_SystemCreationClassName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "SystemCreationClassName",
    /* qualifiers */
    _CIM_Service_SystemCreationClassName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Service_SystemName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Propagated */
    "\045CIM_System.Name",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_Service_SystemName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "SystemName",
    /* qualifiers */
    _CIM_Service_SystemName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Service_CreationClassName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_Service_CreationClassName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "CreationClassName",
    /* qualifiers */
    _CIM_Service_CreationClassName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Service_Name_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Override */
    "\044Name",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_Service_Name =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Name",
    /* qualifiers */
    _CIM_Service_Name_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Service_PrimaryOwnerName_qualifiers[] =
{
    /* Write */
    "\065\001",
    /* MaxLen */
    "\030\000\000\000\100",
    /* MappingStrings */
    "\026\000\001MIF.DMTF|General Information|001.3\000",
    0,
};

static MetaProperty
_CIM_Service_PrimaryOwnerName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_WRITE,
    /* name */
    "PrimaryOwnerName",
    /* qualifiers */
    _CIM_Service_PrimaryOwnerName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Service_PrimaryOwnerContact_qualifiers[] =
{
    /* Write */
    "\065\001",
    /* MaxLen */
    "\030\000\000\001\000",
    /* MappingStrings */
    "\026\000\001MIF.DMTF|General Information|001.4\000",
    0,
};

static MetaProperty
_CIM_Service_PrimaryOwnerContact =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_WRITE,
    /* name */
    "PrimaryOwnerContact",
    /* qualifiers */
    _CIM_Service_PrimaryOwnerContact_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Service_StartMode_qualifiers[] =
{
    /* Deprecated */
    "\013\000\001CIM_Service.EnabledDefault\000",
    /* ValueMap */
    "\061\000\002Automatic\000Manual\000",
    /* MaxLen */
    "\030\000\000\000\012",
    0,
};

static MetaProperty
_CIM_Service_StartMode =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "StartMode",
    /* qualifiers */
    _CIM_Service_StartMode_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Service_Started_qualifiers[] =
{
    0,
};

static MetaProperty
_CIM_Service_Started =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "Started",
    /* qualifiers */
    _CIM_Service_Started_qualifiers,
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_Service_StartService_parameters[] =
{
    0,
};

static const char*
_CIM_Service_StartService_qualifiers[] =
{
    0,
};

static MetaMethod
_CIM_Service_StartService =
{
    /* flags */
    META_FLAG_METHOD,
    /* name */
    "CIM_Service",
    /* qualifiers */
    _CIM_Service_StartService_qualifiers,
    /* type */
    CIMTYPE_UINT32,
    /* parameters */
    _CIM_Service_StartService_parameters,
};

static MetaFeature*
_CIM_Service_StopService_parameters[] =
{
    0,
};

static const char*
_CIM_Service_StopService_qualifiers[] =
{
    0,
};

static MetaMethod
_CIM_Service_StopService =
{
    /* flags */
    META_FLAG_METHOD,
    /* name */
    "CIM_Service",
    /* qualifiers */
    _CIM_Service_StopService_qualifiers,
    /* type */
    CIMTYPE_UINT32,
    /* parameters */
    _CIM_Service_StopService_parameters,
};

static MetaFeature*
_CIM_Service_features[] =
{
    (MetaFeature*)&_CIM_Service_SystemCreationClassName,
    (MetaFeature*)&_CIM_Service_SystemName,
    (MetaFeature*)&_CIM_Service_CreationClassName,
    (MetaFeature*)&_CIM_Service_Name,
    (MetaFeature*)&_CIM_Service_PrimaryOwnerName,
    (MetaFeature*)&_CIM_Service_PrimaryOwnerContact,
    (MetaFeature*)&_CIM_Service_StartMode,
    (MetaFeature*)&_CIM_Service_Started,
    (MetaFeature*)&_CIM_Service_StartService,
    (MetaFeature*)&_CIM_Service_StopService,
    0,
};

static const char*
__root_PG_InterOp_CIM_Service_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Core::Service",
    /* Abstract */
    "\002\001",
    /* Version */
    "\0632.10.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_Service =
{
    /* flags */
    META_FLAG_CLASS|META_FLAG_ABSTRACT,
    /* name */
    "CIM_Service",
    /* qualifiers */
    __root_PG_InterOp_CIM_Service_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_EnabledLogicalElement,
    /* features */
    _CIM_Service_features,
};

//==============================================================================
//
// Class: CIM_ServiceAccessPoint
//
//==============================================================================

static const char*
_CIM_ServiceAccessPoint_SystemCreationClassName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Propagated */
    "\045CIM_System.CreationClassName",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_ServiceAccessPoint_SystemCreationClassName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "SystemCreationClassName",
    /* qualifiers */
    _CIM_ServiceAccessPoint_SystemCreationClassName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_ServiceAccessPoint_SystemName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Propagated */
    "\045CIM_System.Name",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_ServiceAccessPoint_SystemName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "SystemName",
    /* qualifiers */
    _CIM_ServiceAccessPoint_SystemName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_ServiceAccessPoint_CreationClassName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_ServiceAccessPoint_CreationClassName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "CreationClassName",
    /* qualifiers */
    _CIM_ServiceAccessPoint_CreationClassName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_ServiceAccessPoint_Name_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Override */
    "\044Name",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_ServiceAccessPoint_Name =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Name",
    /* qualifiers */
    _CIM_ServiceAccessPoint_Name_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_ServiceAccessPoint_features[] =
{
    (MetaFeature*)&_CIM_ServiceAccessPoint_SystemCreationClassName,
    (MetaFeature*)&_CIM_ServiceAccessPoint_SystemName,
    (MetaFeature*)&_CIM_ServiceAccessPoint_CreationClassName,
    (MetaFeature*)&_CIM_ServiceAccessPoint_Name,
    0,
};

static const char*
__root_PG_InterOp_CIM_ServiceAccessPoint_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Core::Service",
    /* Abstract */
    "\002\001",
    /* Version */
    "\0632.10.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ServiceAccessPoint =
{
    /* flags */
    META_FLAG_CLASS|META_FLAG_ABSTRACT,
    /* name */
    "CIM_ServiceAccessPoint",
    /* qualifiers */
    __root_PG_InterOp_CIM_ServiceAccessPoint_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_EnabledLogicalElement,
    /* features */
    _CIM_ServiceAccessPoint_features,
};

//==============================================================================
//
// Class: CIM_ServiceAccessURI
//
//==============================================================================

static const char*
_CIM_ServiceAccessURI_LabeledURI_qualifiers[] =
{
    /* Required */
    "\050\001",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_ServiceAccessURI_LabeledURI =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "LabeledURI",
    /* qualifiers */
    _CIM_ServiceAccessURI_LabeledURI_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_ServiceAccessURI_features[] =
{
    (MetaFeature*)&_CIM_ServiceAccessURI_LabeledURI,
    0,
};

static const char*
__root_PG_InterOp_CIM_ServiceAccessURI_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Core::Service",
    /* Version */
    "\0632.10.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ServiceAccessURI =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "CIM_ServiceAccessURI",
    /* qualifiers */
    __root_PG_InterOp_CIM_ServiceAccessURI_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_ServiceAccessPoint,
    /* features */
    _CIM_ServiceAccessURI_features,
};

//==============================================================================
//
// Class: CIM_ServiceAffectsElement
//
//==============================================================================

static const char*
_CIM_ServiceAffectsElement_AffectedElement_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_ServiceAffectsElement_AffectedElement =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "AffectedElement",
    /* qualifiers */
    _CIM_ServiceAffectsElement_AffectedElement_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ManagedElement,
};

static const char*
_CIM_ServiceAffectsElement_AffectingElement_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_ServiceAffectsElement_AffectingElement =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "AffectingElement",
    /* qualifiers */
    _CIM_ServiceAffectsElement_AffectingElement_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_Service,
};

static const char*
_CIM_ServiceAffectsElement_ElementEffects_qualifiers[] =
{
    /* ValueMap */
    "\061\000\0130\0001\0002\0003\0004\0005\0006\0007\0008\0009\00010\000",
    /* Values */
    "\062\000\013Unknown\000Other\000Exclusive Use\000Performance Impact\000Element Integrity\000Manages\000Consumes\000Enhances Integrity\000Degrades Integrity\000Enhances Performance\000Degrades Performance\000",
    /* ArrayType */
    "\005Indexed",
    /* ModelCorrespondence */
    "\036\000\001CIM_ServiceAffectsElement.OtherElementEffectsDescriptions\000",
    0,
};

static MetaProperty
_CIM_ServiceAffectsElement_ElementEffects =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "ElementEffects",
    /* qualifiers */
    _CIM_ServiceAffectsElement_ElementEffects_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    0,
    /* value */
    0,
};

static const char*
_CIM_ServiceAffectsElement_OtherElementEffectsDescriptions_qualifiers[] =
{
    /* ArrayType */
    "\005Indexed",
    /* ModelCorrespondence */
    "\036\000\001CIM_ServiceAffectsElement.ElementEffects\000",
    0,
};

static MetaProperty
_CIM_ServiceAffectsElement_OtherElementEffectsDescriptions =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "OtherElementEffectsDescriptions",
    /* qualifiers */
    _CIM_ServiceAffectsElement_OtherElementEffectsDescriptions_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* value */
    0,
};

static MetaFeature*
_CIM_ServiceAffectsElement_features[] =
{
    (MetaFeature*)&_CIM_ServiceAffectsElement_AffectedElement,
    (MetaFeature*)&_CIM_ServiceAffectsElement_AffectingElement,
    (MetaFeature*)&_CIM_ServiceAffectsElement_ElementEffects,
    (MetaFeature*)&_CIM_ServiceAffectsElement_OtherElementEffectsDescriptions,
    0,
};

static const char*
__root_PG_InterOp_CIM_ServiceAffectsElement_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::Service",
    /* Version */
    "\0632.10.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ServiceAffectsElement =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_ServiceAffectsElement",
    /* qualifiers */
    __root_PG_InterOp_CIM_ServiceAffectsElement_qualifiers,
    /* super */
    0,
    /* features */
    _CIM_ServiceAffectsElement_features,
};

//==============================================================================
//
// Class: CIM_ServiceAvailableToElement
//
//==============================================================================

static const char*
_CIM_ServiceAvailableToElement_ServiceProvided_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_ServiceAvailableToElement_ServiceProvided =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "ServiceProvided",
    /* qualifiers */
    _CIM_ServiceAvailableToElement_ServiceProvided_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_Service,
};

static const char*
_CIM_ServiceAvailableToElement_UserOfService_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_ServiceAvailableToElement_UserOfService =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "UserOfService",
    /* qualifiers */
    _CIM_ServiceAvailableToElement_UserOfService_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ManagedElement,
};

static MetaFeature*
_CIM_ServiceAvailableToElement_features[] =
{
    (MetaFeature*)&_CIM_ServiceAvailableToElement_ServiceProvided,
    (MetaFeature*)&_CIM_ServiceAvailableToElement_UserOfService,
    0,
};

static const char*
__root_PG_InterOp_CIM_ServiceAvailableToElement_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::Service",
    /* Version */
    "\0632.10.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ServiceAvailableToElement =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_ServiceAvailableToElement",
    /* qualifiers */
    __root_PG_InterOp_CIM_ServiceAvailableToElement_qualifiers,
    /* super */
    0,
    /* features */
    _CIM_ServiceAvailableToElement_features,
};

//==============================================================================
//
// Class: CIM_ServiceComponent
//
//==============================================================================

static const char*
_CIM_ServiceComponent_GroupComponent_qualifiers[] =
{
    /* Aggregate */
    "\003\001",
    /* Override */
    "\044GroupComponent",
    0,
};

static MetaReference
_CIM_ServiceComponent_GroupComponent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_AGGREGATE|META_FLAG_READ,
    /* name */
    "GroupComponent",
    /* qualifiers */
    _CIM_ServiceComponent_GroupComponent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_Service,
};

static const char*
_CIM_ServiceComponent_PartComponent_qualifiers[] =
{
    /* Override */
    "\044PartComponent",
    0,
};

static MetaReference
_CIM_ServiceComponent_PartComponent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "PartComponent",
    /* qualifiers */
    _CIM_ServiceComponent_PartComponent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_Service,
};

static MetaFeature*
_CIM_ServiceComponent_features[] =
{
    (MetaFeature*)&_CIM_ServiceComponent_GroupComponent,
    (MetaFeature*)&_CIM_ServiceComponent_PartComponent,
    0,
};

static const char*
__root_PG_InterOp_CIM_ServiceComponent_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::Service",
    /* Aggregation */
    "\004\001",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ServiceComponent =
{
    /* flags */
    META_FLAG_ASSOCIATION|META_FLAG_AGGREGATION,
    /* name */
    "CIM_ServiceComponent",
    /* qualifiers */
    __root_PG_InterOp_CIM_ServiceComponent_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_Component,
    /* features */
    _CIM_ServiceComponent_features,
};

//==============================================================================
//
// Class: CIM_ServiceSAPDependency
//
//==============================================================================

static const char*
_CIM_ServiceSAPDependency_Antecedent_qualifiers[] =
{
    /* Override */
    "\044Antecedent",
    0,
};

static MetaReference
_CIM_ServiceSAPDependency_Antecedent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Antecedent",
    /* qualifiers */
    _CIM_ServiceSAPDependency_Antecedent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ServiceAccessPoint,
};

static const char*
_CIM_ServiceSAPDependency_Dependent_qualifiers[] =
{
    /* Override */
    "\044Dependent",
    0,
};

static MetaReference
_CIM_ServiceSAPDependency_Dependent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Dependent",
    /* qualifiers */
    _CIM_ServiceSAPDependency_Dependent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_Service,
};

static MetaFeature*
_CIM_ServiceSAPDependency_features[] =
{
    (MetaFeature*)&_CIM_ServiceSAPDependency_Antecedent,
    (MetaFeature*)&_CIM_ServiceSAPDependency_Dependent,
    0,
};

static const char*
__root_PG_InterOp_CIM_ServiceSAPDependency_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::Service",
    /* Version */
    "\0632.10.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ServiceSAPDependency =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_ServiceSAPDependency",
    /* qualifiers */
    __root_PG_InterOp_CIM_ServiceSAPDependency_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_Dependency,
    /* features */
    _CIM_ServiceSAPDependency_features,
};

//==============================================================================
//
// Class: CIM_ServiceStatistics
//
//==============================================================================

static const char*
_CIM_ServiceStatistics_Stats_qualifiers[] =
{
    /* Override */
    "\044Stats",
    /* Weak */
    "\064\001",
    0,
};

static MetaReference
_CIM_ServiceStatistics_Stats =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ|META_FLAG_WEAK,
    /* name */
    "Stats",
    /* qualifiers */
    _CIM_ServiceStatistics_Stats_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ServiceStatisticalInformation,
};

static const char*
_CIM_ServiceStatistics_Element_qualifiers[] =
{
    /* Override */
    "\044Element",
    /* Min */
    "\033\000\000\000\001",
    /* Max */
    "\027\000\000\000\001",
    0,
};

static MetaReference
_CIM_ServiceStatistics_Element =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Element",
    /* qualifiers */
    _CIM_ServiceStatistics_Element_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_Service,
};

static MetaFeature*
_CIM_ServiceStatistics_features[] =
{
    (MetaFeature*)&_CIM_ServiceStatistics_Stats,
    (MetaFeature*)&_CIM_ServiceStatistics_Element,
    0,
};

static const char*
__root_PG_InterOp_CIM_ServiceStatistics_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::Statistics",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ServiceStatistics =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_ServiceStatistics",
    /* qualifiers */
    __root_PG_InterOp_CIM_ServiceStatistics_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_Statistics,
    /* features */
    _CIM_ServiceStatistics_features,
};

//==============================================================================
//
// Class: CIM_StorageExtent
//
//==============================================================================

static const char*
_CIM_StorageExtent_DataOrganization_qualifiers[] =
{
    /* ValueMap */
    "\061\000\0050\0001\0002\0003\0004\000",
    /* Values */
    "\062\000\005Other\000Unknown\000Fixed Block\000Variable Block\000Count Key Data\000",
    0,
};

static MetaProperty
_CIM_StorageExtent_DataOrganization =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "DataOrganization",
    /* qualifiers */
    _CIM_StorageExtent_DataOrganization_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_StorageExtent_Purpose_qualifiers[] =
{
    /* MappingStrings */
    "\026\000\001MIB.IETF|HOST-RESOURCES-MIB.hrStorageDescr\000",
    0,
};

static MetaProperty
_CIM_StorageExtent_Purpose =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "Purpose",
    /* qualifiers */
    _CIM_StorageExtent_Purpose_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_StorageExtent_Access_qualifiers[] =
{
    /* ValueMap */
    "\061\000\0050\0001\0002\0003\0004\000",
    /* Values */
    "\062\000\005Unknown\000Readable\000Writeable\000Read/Write Supported\000Write Once\000",
    0,
};

static MetaProperty
_CIM_StorageExtent_Access =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "Access",
    /* qualifiers */
    _CIM_StorageExtent_Access_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_StorageExtent_ErrorMethodology_qualifiers[] =
{
    0,
};

static MetaProperty
_CIM_StorageExtent_ErrorMethodology =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "ErrorMethodology",
    /* qualifiers */
    _CIM_StorageExtent_ErrorMethodology_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_StorageExtent_BlockSize_qualifiers[] =
{
    /* Units */
    "\060Bytes",
    /* MappingStrings */
    "\026\000\003MIF.DMTF|Host Storage|001.4\000MIB.IETF|HOST-RESOURCES-MIB.hrStorageAllocationUnits\000MIF.DMTF|Storage Devices|001.5\000",
    0,
};

static MetaProperty
_CIM_StorageExtent_BlockSize =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "BlockSize",
    /* qualifiers */
    _CIM_StorageExtent_BlockSize_qualifiers,
    /* type */
    CIMTYPE_UINT64,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_StorageExtent_NumberOfBlocks_qualifiers[] =
{
    /* MappingStrings */
    "\026\000\002MIF.DMTF|Host Storage|001.5\000MIB.IETF|HOST-RESOURCES-MIB.hrStorageSize\000",
    0,
};

static MetaProperty
_CIM_StorageExtent_NumberOfBlocks =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "NumberOfBlocks",
    /* qualifiers */
    _CIM_StorageExtent_NumberOfBlocks_qualifiers,
    /* type */
    CIMTYPE_UINT64,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_StorageExtent_ConsumableBlocks_qualifiers[] =
{
    0,
};

static MetaProperty
_CIM_StorageExtent_ConsumableBlocks =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "ConsumableBlocks",
    /* qualifiers */
    _CIM_StorageExtent_ConsumableBlocks_qualifiers,
    /* type */
    CIMTYPE_UINT64,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_StorageExtent_IsBasedOnUnderlyingRedundancy_qualifiers[] =
{
    0,
};

static MetaProperty
_CIM_StorageExtent_IsBasedOnUnderlyingRedundancy =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "IsBasedOnUnderlyingRedundancy",
    /* qualifiers */
    _CIM_StorageExtent_IsBasedOnUnderlyingRedundancy_qualifiers,
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_StorageExtent_SequentialAccess_qualifiers[] =
{
    0,
};

static MetaProperty
_CIM_StorageExtent_SequentialAccess =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "SequentialAccess",
    /* qualifiers */
    _CIM_StorageExtent_SequentialAccess_qualifiers,
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_StorageExtent_ExtentStatus_qualifiers[] =
{
    /* ValueMap */
    "\061\000\0210\0001\0002\0003\0004\0005\0006\0007\0008\0009\00010\00011\00012\00013\00014\00015..32767\00032768..65535\000",
    /* Values */
    "\062\000\021Other\000Unknown\000None/Not Applicable\000Broken\000Data Lost\000Dynamic Reconfig\000Exposed\000Fractionally Exposed\000Partially Exposed\000Protection Disabled\000Readying\000Rebuild\000Recalculate\000Spare in Use\000Verify In Progress\000DMTF Reserved\000Vendor Reserved\000",
    0,
};

static MetaProperty
_CIM_StorageExtent_ExtentStatus =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "ExtentStatus",
    /* qualifiers */
    _CIM_StorageExtent_ExtentStatus_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    0,
    /* value */
    0,
};

static const char*
_CIM_StorageExtent_NoSinglePointOfFailure_qualifiers[] =
{
    /* ModelCorrespondence */
    "\036\000\001CIM_StorageSetting.NoSinglePointOfFailure\000",
    0,
};

static MetaProperty
_CIM_StorageExtent_NoSinglePointOfFailure =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "NoSinglePointOfFailure",
    /* qualifiers */
    _CIM_StorageExtent_NoSinglePointOfFailure_qualifiers,
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_StorageExtent_DataRedundancy_qualifiers[] =
{
    /* ModelCorrespondence */
    "\036\000\003CIM_StorageSetting.DataRedundancyGoal\000CIM_StorageSetting.DataRedundancyMax\000CIM_StorageSetting.DataRedundancyMin\000",
    0,
};

static MetaProperty
_CIM_StorageExtent_DataRedundancy =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "DataRedundancy",
    /* qualifiers */
    _CIM_StorageExtent_DataRedundancy_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_StorageExtent_PackageRedundancy_qualifiers[] =
{
    /* ModelCorrespondence */
    "\036\000\003CIM_StorageSetting.PackageRedundancyGoal\000CIM_StorageSetting.PackageRedundancyMax\000CIM_StorageSetting.PackageRedundancyMin\000",
    0,
};

static MetaProperty
_CIM_StorageExtent_PackageRedundancy =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "PackageRedundancy",
    /* qualifiers */
    _CIM_StorageExtent_PackageRedundancy_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_StorageExtent_DeltaReservation_qualifiers[] =
{
    /* Units */
    "\060Percentage",
    /* MinValue */
    "\035\000\000\000\000\000\000\000\001",
    /* MaxValue */
    "\031\000\000\000\000\000\000\000\144",
    /* ModelCorrespondence */
    "\036\000\003CIM_StorageSetting.DeltaReservationGoal\000CIM_StorageSetting.DeltaReservationMax\000CIM_StorageSetting.DeltaReservationMin\000",
    0,
};

static MetaProperty
_CIM_StorageExtent_DeltaReservation =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "DeltaReservation",
    /* qualifiers */
    _CIM_StorageExtent_DeltaReservation_qualifiers,
    /* type */
    CIMTYPE_UINT8,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_StorageExtent_Primordial_qualifiers[] =
{
    0,
};

static MetaProperty
_CIM_StorageExtent_Primordial =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "Primordial",
    /* qualifiers */
    _CIM_StorageExtent_Primordial_qualifiers,
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* value */
    "\000",
};

static MetaFeature*
_CIM_StorageExtent_features[] =
{
    (MetaFeature*)&_CIM_StorageExtent_DataOrganization,
    (MetaFeature*)&_CIM_StorageExtent_Purpose,
    (MetaFeature*)&_CIM_StorageExtent_Access,
    (MetaFeature*)&_CIM_StorageExtent_ErrorMethodology,
    (MetaFeature*)&_CIM_StorageExtent_BlockSize,
    (MetaFeature*)&_CIM_StorageExtent_NumberOfBlocks,
    (MetaFeature*)&_CIM_StorageExtent_ConsumableBlocks,
    (MetaFeature*)&_CIM_StorageExtent_IsBasedOnUnderlyingRedundancy,
    (MetaFeature*)&_CIM_StorageExtent_SequentialAccess,
    (MetaFeature*)&_CIM_StorageExtent_ExtentStatus,
    (MetaFeature*)&_CIM_StorageExtent_NoSinglePointOfFailure,
    (MetaFeature*)&_CIM_StorageExtent_DataRedundancy,
    (MetaFeature*)&_CIM_StorageExtent_PackageRedundancy,
    (MetaFeature*)&_CIM_StorageExtent_DeltaReservation,
    (MetaFeature*)&_CIM_StorageExtent_Primordial,
    0,
};

static const char*
__root_PG_InterOp_CIM_StorageExtent_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Core::StorageExtent",
    /* Version */
    "\0632.8.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_StorageExtent =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "CIM_StorageExtent",
    /* qualifiers */
    __root_PG_InterOp_CIM_StorageExtent_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_LogicalDevice,
    /* features */
    _CIM_StorageExtent_features,
};

//==============================================================================
//
// Class: CIM_System
//
//==============================================================================

static const char*
_CIM_System_CreationClassName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_System_CreationClassName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "CreationClassName",
    /* qualifiers */
    _CIM_System_CreationClassName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_System_Name_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Override */
    "\044Name",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_System_Name =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Name",
    /* qualifiers */
    _CIM_System_Name_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_System_NameFormat_qualifiers[] =
{
    /* MaxLen */
    "\030\000\000\000\100",
    0,
};

static MetaProperty
_CIM_System_NameFormat =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "NameFormat",
    /* qualifiers */
    _CIM_System_NameFormat_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_System_PrimaryOwnerName_qualifiers[] =
{
    /* Write */
    "\065\001",
    /* MaxLen */
    "\030\000\000\000\100",
    /* MappingStrings */
    "\026\000\001MIF.DMTF|General Information|001.3\000",
    0,
};

static MetaProperty
_CIM_System_PrimaryOwnerName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_WRITE,
    /* name */
    "PrimaryOwnerName",
    /* qualifiers */
    _CIM_System_PrimaryOwnerName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_System_PrimaryOwnerContact_qualifiers[] =
{
    /* Write */
    "\065\001",
    /* MaxLen */
    "\030\000\000\001\000",
    /* MappingStrings */
    "\026\000\001MIF.DMTF|General Information|001.4\000",
    0,
};

static MetaProperty
_CIM_System_PrimaryOwnerContact =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_WRITE,
    /* name */
    "PrimaryOwnerContact",
    /* qualifiers */
    _CIM_System_PrimaryOwnerContact_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_System_Roles_qualifiers[] =
{
    /* Write */
    "\065\001",
    0,
};

static MetaProperty
_CIM_System_Roles =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_WRITE,
    /* name */
    "Roles",
    /* qualifiers */
    _CIM_System_Roles_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* value */
    0,
};

static MetaFeature*
_CIM_System_features[] =
{
    (MetaFeature*)&_CIM_System_CreationClassName,
    (MetaFeature*)&_CIM_System_Name,
    (MetaFeature*)&_CIM_System_NameFormat,
    (MetaFeature*)&_CIM_System_PrimaryOwnerName,
    (MetaFeature*)&_CIM_System_PrimaryOwnerContact,
    (MetaFeature*)&_CIM_System_Roles,
    0,
};

static const char*
__root_PG_InterOp_CIM_System_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Core::CoreElements",
    /* Abstract */
    "\002\001",
    /* Version */
    "\0632.7.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_System =
{
    /* flags */
    META_FLAG_CLASS|META_FLAG_ABSTRACT,
    /* name */
    "CIM_System",
    /* qualifiers */
    __root_PG_InterOp_CIM_System_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_EnabledLogicalElement,
    /* features */
    _CIM_System_features,
};

//==============================================================================
//
// Class: CIM_SystemComponent
//
//==============================================================================

static const char*
_CIM_SystemComponent_GroupComponent_qualifiers[] =
{
    /* Aggregate */
    "\003\001",
    /* Override */
    "\044GroupComponent",
    0,
};

static MetaReference
_CIM_SystemComponent_GroupComponent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_AGGREGATE|META_FLAG_READ,
    /* name */
    "GroupComponent",
    /* qualifiers */
    _CIM_SystemComponent_GroupComponent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_System,
};

static const char*
_CIM_SystemComponent_PartComponent_qualifiers[] =
{
    /* Override */
    "\044PartComponent",
    0,
};

static MetaReference
_CIM_SystemComponent_PartComponent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "PartComponent",
    /* qualifiers */
    _CIM_SystemComponent_PartComponent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ManagedSystemElement,
};

static MetaFeature*
_CIM_SystemComponent_features[] =
{
    (MetaFeature*)&_CIM_SystemComponent_GroupComponent,
    (MetaFeature*)&_CIM_SystemComponent_PartComponent,
    0,
};

static const char*
__root_PG_InterOp_CIM_SystemComponent_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::CoreElements",
    /* Aggregation */
    "\004\001",
    /* Version */
    "\0632.10.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_SystemComponent =
{
    /* flags */
    META_FLAG_ASSOCIATION|META_FLAG_AGGREGATION,
    /* name */
    "CIM_SystemComponent",
    /* qualifiers */
    __root_PG_InterOp_CIM_SystemComponent_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_Component,
    /* features */
    _CIM_SystemComponent_features,
};

//==============================================================================
//
// Class: CIM_SystemDevice
//
//==============================================================================

static const char*
_CIM_SystemDevice_GroupComponent_qualifiers[] =
{
    /* Aggregate */
    "\003\001",
    /* Override */
    "\044GroupComponent",
    /* Min */
    "\033\000\000\000\001",
    /* Max */
    "\027\000\000\000\001",
    0,
};

static MetaReference
_CIM_SystemDevice_GroupComponent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_AGGREGATE|META_FLAG_READ,
    /* name */
    "GroupComponent",
    /* qualifiers */
    _CIM_SystemDevice_GroupComponent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_System,
};

static const char*
_CIM_SystemDevice_PartComponent_qualifiers[] =
{
    /* Override */
    "\044PartComponent",
    /* Weak */
    "\064\001",
    0,
};

static MetaReference
_CIM_SystemDevice_PartComponent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ|META_FLAG_WEAK,
    /* name */
    "PartComponent",
    /* qualifiers */
    _CIM_SystemDevice_PartComponent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_LogicalDevice,
};

static MetaFeature*
_CIM_SystemDevice_features[] =
{
    (MetaFeature*)&_CIM_SystemDevice_GroupComponent,
    (MetaFeature*)&_CIM_SystemDevice_PartComponent,
    0,
};

static const char*
__root_PG_InterOp_CIM_SystemDevice_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::Device",
    /* Aggregation */
    "\004\001",
    /* Composition */
    "\011\001",
    /* Version */
    "\0632.10.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_SystemDevice =
{
    /* flags */
    META_FLAG_ASSOCIATION|META_FLAG_AGGREGATION,
    /* name */
    "CIM_SystemDevice",
    /* qualifiers */
    __root_PG_InterOp_CIM_SystemDevice_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_SystemComponent,
    /* features */
    _CIM_SystemDevice_features,
};

//==============================================================================
//
// Class: CIM_SystemPackaging
//
//==============================================================================

static const char*
_CIM_SystemPackaging_Antecedent_qualifiers[] =
{
    /* Override */
    "\044Antecedent",
    0,
};

static MetaReference
_CIM_SystemPackaging_Antecedent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Antecedent",
    /* qualifiers */
    _CIM_SystemPackaging_Antecedent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_PhysicalElement,
};

static const char*
_CIM_SystemPackaging_Dependent_qualifiers[] =
{
    /* Override */
    "\044Dependent",
    0,
};

static MetaReference
_CIM_SystemPackaging_Dependent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Dependent",
    /* qualifiers */
    _CIM_SystemPackaging_Dependent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_System,
};

static MetaFeature*
_CIM_SystemPackaging_features[] =
{
    (MetaFeature*)&_CIM_SystemPackaging_Antecedent,
    (MetaFeature*)&_CIM_SystemPackaging_Dependent,
    0,
};

static const char*
__root_PG_InterOp_CIM_SystemPackaging_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::Physical",
    /* Version */
    "\0632.10.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_SystemPackaging =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_SystemPackaging",
    /* qualifiers */
    __root_PG_InterOp_CIM_SystemPackaging_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_Dependency,
    /* features */
    _CIM_SystemPackaging_features,
};

//==============================================================================
//
// Class: CIM_SystemStatistics
//
//==============================================================================

static const char*
_CIM_SystemStatistics_Stats_qualifiers[] =
{
    /* Override */
    "\044Stats",
    /* Weak */
    "\064\001",
    0,
};

static MetaReference
_CIM_SystemStatistics_Stats =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ|META_FLAG_WEAK,
    /* name */
    "Stats",
    /* qualifiers */
    _CIM_SystemStatistics_Stats_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_SystemStatisticalInformation,
};

static const char*
_CIM_SystemStatistics_Element_qualifiers[] =
{
    /* Override */
    "\044Element",
    /* Min */
    "\033\000\000\000\001",
    /* Max */
    "\027\000\000\000\001",
    0,
};

static MetaReference
_CIM_SystemStatistics_Element =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Element",
    /* qualifiers */
    _CIM_SystemStatistics_Element_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_System,
};

static MetaFeature*
_CIM_SystemStatistics_features[] =
{
    (MetaFeature*)&_CIM_SystemStatistics_Stats,
    (MetaFeature*)&_CIM_SystemStatistics_Element,
    0,
};

static const char*
__root_PG_InterOp_CIM_SystemStatistics_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::Statistics",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_SystemStatistics =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_SystemStatistics",
    /* qualifiers */
    __root_PG_InterOp_CIM_SystemStatistics_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_Statistics,
    /* features */
    _CIM_SystemStatistics_features,
};

//==============================================================================
//
// Class: CIM_AdminDomain
//
//==============================================================================

static const char*
_CIM_AdminDomain_NameFormat_qualifiers[] =
{
    /* Override */
    "\044NameFormat",
    /* ValueMap */
    "\061\000\024Other\000AS\000NAP\000NOC\000POP\000RNP\000IP\000IPX\000SNA\000Dial\000WAN\000LAN\000ISDN\000Frame Relay\000ATM\000E.164\000IB\000FC\000Policy Repository\000WWN\000",
    /* Values */
    "\062\000\024Other\000Autonomous System\000Network Access Provider\000Network Operations Center\000Point of Presence\000Regional Network Provider\000IP\000IPX\000SNA\000Dial\000WAN\000LAN\000ISDN\000Frame Relay\000ATM\000E.164\000Infiniband\000Fibre Channel\000Policy Repository\000Fibre Channel Worldwide Name\000",
    /* ModelCorrespondence */
    "\036\000\001CIM_AdminDomain.Name\000",
    0,
};

static MetaProperty
_CIM_AdminDomain_NameFormat =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "NameFormat",
    /* qualifiers */
    _CIM_AdminDomain_NameFormat_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_AdminDomain_features[] =
{
    (MetaFeature*)&_CIM_AdminDomain_NameFormat,
    0,
};

static const char*
__root_PG_InterOp_CIM_AdminDomain_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Core::CoreElements",
    /* Version */
    "\0632.10.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_AdminDomain =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "CIM_AdminDomain",
    /* qualifiers */
    __root_PG_InterOp_CIM_AdminDomain_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_System,
    /* features */
    _CIM_AdminDomain_features,
};

//==============================================================================
//
// Class: CIM_BasedOn
//
//==============================================================================

static const char*
_CIM_BasedOn_Antecedent_qualifiers[] =
{
    /* Override */
    "\044Antecedent",
    0,
};

static MetaReference
_CIM_BasedOn_Antecedent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Antecedent",
    /* qualifiers */
    _CIM_BasedOn_Antecedent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_StorageExtent,
};

static const char*
_CIM_BasedOn_Dependent_qualifiers[] =
{
    /* Override */
    "\044Dependent",
    0,
};

static MetaReference
_CIM_BasedOn_Dependent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Dependent",
    /* qualifiers */
    _CIM_BasedOn_Dependent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_StorageExtent,
};

static const char*
_CIM_BasedOn_StartingAddress_qualifiers[] =
{
    0,
};

static MetaProperty
_CIM_BasedOn_StartingAddress =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "StartingAddress",
    /* qualifiers */
    _CIM_BasedOn_StartingAddress_qualifiers,
    /* type */
    CIMTYPE_UINT64,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_BasedOn_EndingAddress_qualifiers[] =
{
    0,
};

static MetaProperty
_CIM_BasedOn_EndingAddress =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "EndingAddress",
    /* qualifiers */
    _CIM_BasedOn_EndingAddress_qualifiers,
    /* type */
    CIMTYPE_UINT64,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_BasedOn_OrderIndex_qualifiers[] =
{
    0,
};

static MetaProperty
_CIM_BasedOn_OrderIndex =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "OrderIndex",
    /* qualifiers */
    _CIM_BasedOn_OrderIndex_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_BasedOn_features[] =
{
    (MetaFeature*)&_CIM_BasedOn_Antecedent,
    (MetaFeature*)&_CIM_BasedOn_Dependent,
    (MetaFeature*)&_CIM_BasedOn_StartingAddress,
    (MetaFeature*)&_CIM_BasedOn_EndingAddress,
    (MetaFeature*)&_CIM_BasedOn_OrderIndex,
    0,
};

static const char*
__root_PG_InterOp_CIM_BasedOn_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::StorageExtent",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_BasedOn =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_BasedOn",
    /* qualifiers */
    __root_PG_InterOp_CIM_BasedOn_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_Dependency,
    /* features */
    _CIM_BasedOn_features,
};

//==============================================================================
//
// Class: CIM_ConfigurationForSystem
//
//==============================================================================

static const char*
_CIM_ConfigurationForSystem_Antecedent_qualifiers[] =
{
    /* Override */
    "\044Antecedent",
    /* Min */
    "\033\000\000\000\001",
    /* Max */
    "\027\000\000\000\001",
    0,
};

static MetaReference
_CIM_ConfigurationForSystem_Antecedent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Antecedent",
    /* qualifiers */
    _CIM_ConfigurationForSystem_Antecedent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_System,
};

static const char*
_CIM_ConfigurationForSystem_Dependent_qualifiers[] =
{
    /* Override */
    "\044Dependent",
    /* Weak */
    "\064\001",
    0,
};

static MetaReference
_CIM_ConfigurationForSystem_Dependent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ|META_FLAG_WEAK,
    /* name */
    "Dependent",
    /* qualifiers */
    _CIM_ConfigurationForSystem_Dependent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_SystemConfiguration,
};

static MetaFeature*
_CIM_ConfigurationForSystem_features[] =
{
    (MetaFeature*)&_CIM_ConfigurationForSystem_Antecedent,
    (MetaFeature*)&_CIM_ConfigurationForSystem_Dependent,
    0,
};

static const char*
__root_PG_InterOp_CIM_ConfigurationForSystem_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::Settings",
    /* Version */
    "\0632.8.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ConfigurationForSystem =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_ConfigurationForSystem",
    /* qualifiers */
    __root_PG_InterOp_CIM_ConfigurationForSystem_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_HostedDependency,
    /* features */
    _CIM_ConfigurationForSystem_features,
};

//==============================================================================
//
// Class: CIM_ContainedDomain
//
//==============================================================================

static const char*
_CIM_ContainedDomain_GroupComponent_qualifiers[] =
{
    /* Aggregate */
    "\003\001",
    /* Override */
    "\044GroupComponent",
    0,
};

static MetaReference
_CIM_ContainedDomain_GroupComponent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_AGGREGATE|META_FLAG_READ,
    /* name */
    "GroupComponent",
    /* qualifiers */
    _CIM_ContainedDomain_GroupComponent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_AdminDomain,
};

static const char*
_CIM_ContainedDomain_PartComponent_qualifiers[] =
{
    /* Override */
    "\044PartComponent",
    0,
};

static MetaReference
_CIM_ContainedDomain_PartComponent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "PartComponent",
    /* qualifiers */
    _CIM_ContainedDomain_PartComponent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_AdminDomain,
};

static MetaFeature*
_CIM_ContainedDomain_features[] =
{
    (MetaFeature*)&_CIM_ContainedDomain_GroupComponent,
    (MetaFeature*)&_CIM_ContainedDomain_PartComponent,
    0,
};

static const char*
__root_PG_InterOp_CIM_ContainedDomain_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::CoreElements",
    /* Aggregation */
    "\004\001",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ContainedDomain =
{
    /* flags */
    META_FLAG_ASSOCIATION|META_FLAG_AGGREGATION,
    /* name */
    "CIM_ContainedDomain",
    /* qualifiers */
    __root_PG_InterOp_CIM_ContainedDomain_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_SystemComponent,
    /* features */
    _CIM_ContainedDomain_features,
};

//==============================================================================
//
// Class: CIM_DeviceSAPImplementation
//
//==============================================================================

static const char*
_CIM_DeviceSAPImplementation_Antecedent_qualifiers[] =
{
    /* Override */
    "\044Antecedent",
    0,
};

static MetaReference
_CIM_DeviceSAPImplementation_Antecedent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Antecedent",
    /* qualifiers */
    _CIM_DeviceSAPImplementation_Antecedent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_LogicalDevice,
};

static const char*
_CIM_DeviceSAPImplementation_Dependent_qualifiers[] =
{
    /* Override */
    "\044Dependent",
    0,
};

static MetaReference
_CIM_DeviceSAPImplementation_Dependent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Dependent",
    /* qualifiers */
    _CIM_DeviceSAPImplementation_Dependent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ServiceAccessPoint,
};

static MetaFeature*
_CIM_DeviceSAPImplementation_features[] =
{
    (MetaFeature*)&_CIM_DeviceSAPImplementation_Antecedent,
    (MetaFeature*)&_CIM_DeviceSAPImplementation_Dependent,
    0,
};

static const char*
__root_PG_InterOp_CIM_DeviceSAPImplementation_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::Device",
    /* Version */
    "\0632.10.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_DeviceSAPImplementation =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_DeviceSAPImplementation",
    /* qualifiers */
    __root_PG_InterOp_CIM_DeviceSAPImplementation_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_Dependency,
    /* features */
    _CIM_DeviceSAPImplementation_features,
};

//==============================================================================
//
// Class: CIM_DeviceServiceImplementation
//
//==============================================================================

static const char*
_CIM_DeviceServiceImplementation_Antecedent_qualifiers[] =
{
    /* Override */
    "\044Antecedent",
    0,
};

static MetaReference
_CIM_DeviceServiceImplementation_Antecedent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Antecedent",
    /* qualifiers */
    _CIM_DeviceServiceImplementation_Antecedent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_LogicalDevice,
};

static const char*
_CIM_DeviceServiceImplementation_Dependent_qualifiers[] =
{
    /* Override */
    "\044Dependent",
    0,
};

static MetaReference
_CIM_DeviceServiceImplementation_Dependent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Dependent",
    /* qualifiers */
    _CIM_DeviceServiceImplementation_Dependent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_Service,
};

static MetaFeature*
_CIM_DeviceServiceImplementation_features[] =
{
    (MetaFeature*)&_CIM_DeviceServiceImplementation_Antecedent,
    (MetaFeature*)&_CIM_DeviceServiceImplementation_Dependent,
    0,
};

static const char*
__root_PG_InterOp_CIM_DeviceServiceImplementation_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::Device",
    /* Version */
    "\0632.10.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_DeviceServiceImplementation =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_DeviceServiceImplementation",
    /* qualifiers */
    __root_PG_InterOp_CIM_DeviceServiceImplementation_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_Dependency,
    /* features */
    _CIM_DeviceServiceImplementation_features,
};

//==============================================================================
//
// Class: CIM_DeviceStatistics
//
//==============================================================================

static const char*
_CIM_DeviceStatistics_Stats_qualifiers[] =
{
    /* Override */
    "\044Stats",
    /* Weak */
    "\064\001",
    0,
};

static MetaReference
_CIM_DeviceStatistics_Stats =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ|META_FLAG_WEAK,
    /* name */
    "Stats",
    /* qualifiers */
    _CIM_DeviceStatistics_Stats_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_DeviceStatisticalInformation,
};

static const char*
_CIM_DeviceStatistics_Element_qualifiers[] =
{
    /* Override */
    "\044Element",
    /* Min */
    "\033\000\000\000\001",
    /* Max */
    "\027\000\000\000\001",
    0,
};

static MetaReference
_CIM_DeviceStatistics_Element =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Element",
    /* qualifiers */
    _CIM_DeviceStatistics_Element_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_LogicalDevice,
};

static MetaFeature*
_CIM_DeviceStatistics_features[] =
{
    (MetaFeature*)&_CIM_DeviceStatistics_Stats,
    (MetaFeature*)&_CIM_DeviceStatistics_Element,
    0,
};

static const char*
__root_PG_InterOp_CIM_DeviceStatistics_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::Statistics",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_DeviceStatistics =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_DeviceStatistics",
    /* qualifiers */
    __root_PG_InterOp_CIM_DeviceStatistics_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_Statistics,
    /* features */
    _CIM_DeviceStatistics_features,
};

//==============================================================================
//
// Class: CIM_ExtentRedundancyComponent
//
//==============================================================================

static const char*
_CIM_ExtentRedundancyComponent_GroupComponent_qualifiers[] =
{
    /* Deprecated */
    "\013\000\001CIM_MemberOfCollection.Collection\000",
    /* Aggregate */
    "\003\001",
    /* Override */
    "\044GroupComponent",
    0,
};

static MetaReference
_CIM_ExtentRedundancyComponent_GroupComponent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_AGGREGATE|META_FLAG_READ,
    /* name */
    "GroupComponent",
    /* qualifiers */
    _CIM_ExtentRedundancyComponent_GroupComponent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_StorageRedundancyGroup,
};

static const char*
_CIM_ExtentRedundancyComponent_PartComponent_qualifiers[] =
{
    /* Deprecated */
    "\013\000\001CIM_MemberOfCollection.Member\000",
    /* Override */
    "\044PartComponent",
    0,
};

static MetaReference
_CIM_ExtentRedundancyComponent_PartComponent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "PartComponent",
    /* qualifiers */
    _CIM_ExtentRedundancyComponent_PartComponent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_StorageExtent,
};

static MetaFeature*
_CIM_ExtentRedundancyComponent_features[] =
{
    (MetaFeature*)&_CIM_ExtentRedundancyComponent_GroupComponent,
    (MetaFeature*)&_CIM_ExtentRedundancyComponent_PartComponent,
    0,
};

static const char*
__root_PG_InterOp_CIM_ExtentRedundancyComponent_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* Deprecated */
    "\013\000\001CIM_MemberOfCollection\000",
    /* UMLPackagePath */
    "\057CIM::Core::Redundancy",
    /* Aggregation */
    "\004\001",
    /* Version */
    "\0632.13.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ExtentRedundancyComponent =
{
    /* flags */
    META_FLAG_ASSOCIATION|META_FLAG_AGGREGATION,
    /* name */
    "CIM_ExtentRedundancyComponent",
    /* qualifiers */
    __root_PG_InterOp_CIM_ExtentRedundancyComponent_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_RedundancyComponent,
    /* features */
    _CIM_ExtentRedundancyComponent_features,
};

//==============================================================================
//
// Class: CIM_HostedAccessPoint
//
//==============================================================================

static const char*
_CIM_HostedAccessPoint_Antecedent_qualifiers[] =
{
    /* Override */
    "\044Antecedent",
    /* Min */
    "\033\000\000\000\001",
    /* Max */
    "\027\000\000\000\001",
    0,
};

static MetaReference
_CIM_HostedAccessPoint_Antecedent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Antecedent",
    /* qualifiers */
    _CIM_HostedAccessPoint_Antecedent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_System,
};

static const char*
_CIM_HostedAccessPoint_Dependent_qualifiers[] =
{
    /* Override */
    "\044Dependent",
    /* Weak */
    "\064\001",
    0,
};

static MetaReference
_CIM_HostedAccessPoint_Dependent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ|META_FLAG_WEAK,
    /* name */
    "Dependent",
    /* qualifiers */
    _CIM_HostedAccessPoint_Dependent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ServiceAccessPoint,
};

static MetaFeature*
_CIM_HostedAccessPoint_features[] =
{
    (MetaFeature*)&_CIM_HostedAccessPoint_Antecedent,
    (MetaFeature*)&_CIM_HostedAccessPoint_Dependent,
    0,
};

static const char*
__root_PG_InterOp_CIM_HostedAccessPoint_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::Service",
    /* Version */
    "\0632.10.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_HostedAccessPoint =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_HostedAccessPoint",
    /* qualifiers */
    __root_PG_InterOp_CIM_HostedAccessPoint_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_HostedDependency,
    /* features */
    _CIM_HostedAccessPoint_features,
};

//==============================================================================
//
// Class: CIM_HostedCollection
//
//==============================================================================

static const char*
_CIM_HostedCollection_Antecedent_qualifiers[] =
{
    /* Override */
    "\044Antecedent",
    /* Min */
    "\033\000\000\000\001",
    /* Max */
    "\027\000\000\000\001",
    0,
};

static MetaReference
_CIM_HostedCollection_Antecedent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Antecedent",
    /* qualifiers */
    _CIM_HostedCollection_Antecedent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_System,
};

static const char*
_CIM_HostedCollection_Dependent_qualifiers[] =
{
    /* Override */
    "\044Dependent",
    0,
};

static MetaReference
_CIM_HostedCollection_Dependent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Dependent",
    /* qualifiers */
    _CIM_HostedCollection_Dependent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_SystemSpecificCollection,
};

static MetaFeature*
_CIM_HostedCollection_features[] =
{
    (MetaFeature*)&_CIM_HostedCollection_Antecedent,
    (MetaFeature*)&_CIM_HostedCollection_Dependent,
    0,
};

static const char*
__root_PG_InterOp_CIM_HostedCollection_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::Collection",
    /* Version */
    "\0632.10.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_HostedCollection =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_HostedCollection",
    /* qualifiers */
    __root_PG_InterOp_CIM_HostedCollection_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_HostedDependency,
    /* features */
    _CIM_HostedCollection_features,
};

//==============================================================================
//
// Class: CIM_HostedService
//
//==============================================================================

static const char*
_CIM_HostedService_Antecedent_qualifiers[] =
{
    /* Override */
    "\044Antecedent",
    /* Min */
    "\033\000\000\000\001",
    /* Max */
    "\027\000\000\000\001",
    0,
};

static MetaReference
_CIM_HostedService_Antecedent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Antecedent",
    /* qualifiers */
    _CIM_HostedService_Antecedent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_System,
};

static const char*
_CIM_HostedService_Dependent_qualifiers[] =
{
    /* Override */
    "\044Dependent",
    /* Weak */
    "\064\001",
    0,
};

static MetaReference
_CIM_HostedService_Dependent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ|META_FLAG_WEAK,
    /* name */
    "Dependent",
    /* qualifiers */
    _CIM_HostedService_Dependent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_Service,
};

static MetaFeature*
_CIM_HostedService_features[] =
{
    (MetaFeature*)&_CIM_HostedService_Antecedent,
    (MetaFeature*)&_CIM_HostedService_Dependent,
    0,
};

static const char*
__root_PG_InterOp_CIM_HostedService_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::Service",
    /* Version */
    "\0632.10.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_HostedService =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_HostedService",
    /* qualifiers */
    __root_PG_InterOp_CIM_HostedService_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_HostedDependency,
    /* features */
    _CIM_HostedService_features,
};

//==============================================================================
//
// Class: CIM_InstalledSoftwareIdentity
//
//==============================================================================

static const char*
_CIM_InstalledSoftwareIdentity_System_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_InstalledSoftwareIdentity_System =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "System",
    /* qualifiers */
    _CIM_InstalledSoftwareIdentity_System_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_System,
};

static const char*
_CIM_InstalledSoftwareIdentity_InstalledSoftware_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_InstalledSoftwareIdentity_InstalledSoftware =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "InstalledSoftware",
    /* qualifiers */
    _CIM_InstalledSoftwareIdentity_InstalledSoftware_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_SoftwareIdentity,
};

static MetaFeature*
_CIM_InstalledSoftwareIdentity_features[] =
{
    (MetaFeature*)&_CIM_InstalledSoftwareIdentity_System,
    (MetaFeature*)&_CIM_InstalledSoftwareIdentity_InstalledSoftware,
    0,
};

static const char*
__root_PG_InterOp_CIM_InstalledSoftwareIdentity_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::Software",
    /* Version */
    "\0632.8.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_InstalledSoftwareIdentity =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_InstalledSoftwareIdentity",
    /* qualifiers */
    __root_PG_InterOp_CIM_InstalledSoftwareIdentity_qualifiers,
    /* super */
    0,
    /* features */
    _CIM_InstalledSoftwareIdentity_features,
};

//==============================================================================
//
// Class: CIM_PowerManagementService
//
//==============================================================================

static const char*
_CIM_PowerManagementService_SetPowerState_PowerState_qualifiers[] =
{
    /* IN */
    "\024\001",
    /* ValueMap */
    "\061\000\0101\0002\0003\0004\0005\0006\0007\0008\000",
    /* Values */
    "\062\000\010Full Power\000Power Save - Low Power Mode\000Power Save - Standby\000Power Save - Other\000Power Cycle\000Power Off\000Hibernate\000Soft Off\000",
    0,
};

static MetaProperty
_CIM_PowerManagementService_SetPowerState_PowerState =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_IN,
    /* name */
    "PowerState",
    /* qualifiers */
    _CIM_PowerManagementService_SetPowerState_PowerState_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_PowerManagementService_SetPowerState_ManagedElement_qualifiers[] =
{
    /* IN */
    "\024\001",
    0,
};

static MetaReference
_CIM_PowerManagementService_SetPowerState_ManagedElement =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_IN,
    /* name */
    "ManagedElement",
    /* qualifiers */
    _CIM_PowerManagementService_SetPowerState_ManagedElement_qualifiers,
    /* subscript */
    -1,
    /* ref */
    &__root_PG_InterOp_CIM_ManagedElement,
};

static const char*
_CIM_PowerManagementService_SetPowerState_Time_qualifiers[] =
{
    /* IN */
    "\024\001",
    0,
};

static MetaProperty
_CIM_PowerManagementService_SetPowerState_Time =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_IN,
    /* name */
    "Time",
    /* qualifiers */
    _CIM_PowerManagementService_SetPowerState_Time_qualifiers,
    /* type */
    CIMTYPE_DATETIME,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_PowerManagementService_SetPowerState_parameters[] =
{
    (MetaFeature*)&_CIM_PowerManagementService_SetPowerState_PowerState,
    (MetaFeature*)&_CIM_PowerManagementService_SetPowerState_ManagedElement,
    (MetaFeature*)&_CIM_PowerManagementService_SetPowerState_Time,
    0,
};

static const char*
_CIM_PowerManagementService_SetPowerState_qualifiers[] =
{
    0,
};

static MetaMethod
_CIM_PowerManagementService_SetPowerState =
{
    /* flags */
    META_FLAG_METHOD,
    /* name */
    "CIM_PowerManagementService",
    /* qualifiers */
    _CIM_PowerManagementService_SetPowerState_qualifiers,
    /* type */
    CIMTYPE_UINT32,
    /* parameters */
    _CIM_PowerManagementService_SetPowerState_parameters,
};

static MetaFeature*
_CIM_PowerManagementService_features[] =
{
    (MetaFeature*)&_CIM_PowerManagementService_SetPowerState,
    0,
};

static const char*
__root_PG_InterOp_CIM_PowerManagementService_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Core::PowerMgmt",
    /* Version */
    "\0632.7.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_PowerManagementService =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "CIM_PowerManagementService",
    /* qualifiers */
    __root_PG_InterOp_CIM_PowerManagementService_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_Service,
    /* features */
    _CIM_PowerManagementService_features,
};

//==============================================================================
//
// Class: CIM_ProductServiceComponent
//
//==============================================================================

static const char*
_CIM_ProductServiceComponent_GroupComponent_qualifiers[] =
{
    /* Aggregate */
    "\003\001",
    /* Override */
    "\044GroupComponent",
    /* Max */
    "\027\000\000\000\001",
    0,
};

static MetaReference
_CIM_ProductServiceComponent_GroupComponent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_AGGREGATE|META_FLAG_READ,
    /* name */
    "GroupComponent",
    /* qualifiers */
    _CIM_ProductServiceComponent_GroupComponent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_Product,
};

static const char*
_CIM_ProductServiceComponent_PartComponent_qualifiers[] =
{
    /* Override */
    "\044PartComponent",
    0,
};

static MetaReference
_CIM_ProductServiceComponent_PartComponent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "PartComponent",
    /* qualifiers */
    _CIM_ProductServiceComponent_PartComponent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_Service,
};

static MetaFeature*
_CIM_ProductServiceComponent_features[] =
{
    (MetaFeature*)&_CIM_ProductServiceComponent_GroupComponent,
    (MetaFeature*)&_CIM_ProductServiceComponent_PartComponent,
    0,
};

static const char*
__root_PG_InterOp_CIM_ProductServiceComponent_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::ProductFRU",
    /* Aggregation */
    "\004\001",
    /* Composition */
    "\011\001",
    /* Version */
    "\0632.7.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ProductServiceComponent =
{
    /* flags */
    META_FLAG_ASSOCIATION|META_FLAG_AGGREGATION,
    /* name */
    "CIM_ProductServiceComponent",
    /* qualifiers */
    __root_PG_InterOp_CIM_ProductServiceComponent_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_Component,
    /* features */
    _CIM_ProductServiceComponent_features,
};

//==============================================================================
//
// Class: CIM_ProtocolEndpoint
//
//==============================================================================

static const char*
_CIM_ProtocolEndpoint_Description_qualifiers[] =
{
    /* Override */
    "\044Description",
    /* MappingStrings */
    "\026\000\001MIB.IETF|IF-MIB.ifDescr\000",
    0,
};

static MetaProperty
_CIM_ProtocolEndpoint_Description =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "Description",
    /* qualifiers */
    _CIM_ProtocolEndpoint_Description_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_ProtocolEndpoint_OperationalStatus_qualifiers[] =
{
    /* Override */
    "\044OperationalStatus",
    /* MappingStrings */
    "\026\000\001MIB.IETF|IF-MIB.ifOperStatus\000",
    0,
};

static MetaProperty
_CIM_ProtocolEndpoint_OperationalStatus =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "OperationalStatus",
    /* qualifiers */
    _CIM_ProtocolEndpoint_OperationalStatus_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    0,
    /* value */
    0,
};

static const char*
_CIM_ProtocolEndpoint_EnabledState_qualifiers[] =
{
    /* Override */
    "\044EnabledState",
    /* MappingStrings */
    "\026\000\001MIB.IETF|IF-MIB.ifAdminStatus\000",
    0,
};

static MetaProperty
_CIM_ProtocolEndpoint_EnabledState =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "EnabledState",
    /* qualifiers */
    _CIM_ProtocolEndpoint_EnabledState_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_ProtocolEndpoint_TimeOfLastStateChange_qualifiers[] =
{
    /* Override */
    "\044TimeOfLastStateChange",
    /* MappingStrings */
    "\026\000\001MIB.IETF|IF-MIB.ifLastChange\000",
    0,
};

static MetaProperty
_CIM_ProtocolEndpoint_TimeOfLastStateChange =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "TimeOfLastStateChange",
    /* qualifiers */
    _CIM_ProtocolEndpoint_TimeOfLastStateChange_qualifiers,
    /* type */
    CIMTYPE_DATETIME,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_ProtocolEndpoint_Name_qualifiers[] =
{
    /* Override */
    "\044Name",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_ProtocolEndpoint_Name =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "Name",
    /* qualifiers */
    _CIM_ProtocolEndpoint_Name_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_ProtocolEndpoint_NameFormat_qualifiers[] =
{
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_ProtocolEndpoint_NameFormat =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "NameFormat",
    /* qualifiers */
    _CIM_ProtocolEndpoint_NameFormat_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_ProtocolEndpoint_ProtocolType_qualifiers[] =
{
    /* Deprecated */
    "\013\000\001CIM_ProtocolEndpoint.ProtocolIFType\000",
    /* ValueMap */
    "\061\000\0340\0001\0002\0003\0004\0005\0006\0007\0008\0009\00010\00011\00012\00013\00014\00015\00016\00017\00018\00019\00020\00021\00022\00023\00024\00025\00026\00027\000",
    /* Values */
    "\062\000\034Unknown\000Other\000IPv4\000IPv6\000IPX\000AppleTalk\000DECnet\000SNA\000CONP\000CLNP\000VINES\000XNS\000ATM\000Frame Relay\000Ethernet\000TokenRing\000FDDI\000Infiniband\000Fibre Channel\000ISDN BRI Endpoint\000ISDN B Channel Endpoint\000ISDN D Channel Endpoint\000IPv4/v6\000BGP\000OSPF\000MPLS\000UDP\000TCP\000",
    /* ModelCorrespondence */
    "\036\000\001CIM_ProtocolEndpoint.OtherTypeDescription\000",
    0,
};

static MetaProperty
_CIM_ProtocolEndpoint_ProtocolType =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "ProtocolType",
    /* qualifiers */
    _CIM_ProtocolEndpoint_ProtocolType_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_ProtocolEndpoint_ProtocolIFType_qualifiers[] =
{
    /* ValueMap */
    "\061\000\3700\0001\0002\0003\0004\0005\0006\0007\0008\0009\00010\00011\00012\00013\00014\00015\00016\00017\00018\00019\00020\00021\00022\00023\00024\00025\00026\00027\00028\00029\00030\00031\00032\00033\00034\00035\00036\00037\00038\00039\00040\00041\00042\00043\00044\00045\00046\00047\00048\00049\00050\00051\00052\00053\00054\00055\00056\00057\00058\00059\00060\00061\00062\00063\00064\00065\00066\00067\00068\00069\00070\00071\00072\00073\00074\00075\00076\00077\00078\00079\00080\00081\00082\00083\00084\00085\00086\00087\00088\00089\00090\00091\00092\00093\00094\00095\00096\00097\00098\00099\000100\000101\000102\000103\000104\000105\000106\000107\000108\000109\000110\000111\000112\000113\000114\000115\000116\000117\000118\000119\000120\000121\000122\000123\000124\000125\000126\000127\000128\000129\000130\000131\000132\000133\000134\000135\000136\000137\000138\000139\000140\000141\000142\000143\000144\000145\000146\000147\000148\000149\000150\000151\000152\000153\000154\000155\000156\000157\000158\000159\000160\000161\000162\000163\000164\000165\000166\000167\000168\000169\000170\000171\000172\000173\000174\000175\000176\000177\000178\000179\000180\000181\000182\000183\000184\000185\000186\000187\000188\000189\000190\000191\000192\000193\000194\000195\000196\000197\000198\000199\000200\000201\000202\000203\000204\000205\000206\000207\000208\000209\000210\000211\000212\000213\000214\000215\000216\000217\000218\000219\000220\000221\000222\000223\000224\000..4095\0004096\0004097\0004098\0004099\0004100\0004101\0004102\0004103\0004104\0004105\0004106\0004107\0004108\0004109\0004110\0004111\0004112\0004113\0004114\0004115\000..32767\00032768..\000",
    /* Values */
    "\062\000\370Unknown\000Other\000Regular 1822\000HDH 1822\000DDN X.25\000RFC877 X.25\000Ethernet CSMA/CD\000ISO 802.3 CSMA/CD\000ISO 802.4 Token Bus\000ISO 802.5 Token Ring\000ISO 802.6 MAN\000StarLAN\000Proteon 10Mbit\000Proteon 80Mbit\000HyperChannel\000FDDI\000LAP-B\000SDLC\000DS1\000E1\000Basic ISDN\000Primary ISDN\000Proprietary Point-to-Point Serial\000PPP\000Software Loopback\000EON\000Ethernet 3Mbit\000NSIP\000SLIP\000Ultra\000DS3\000SIP\000Frame Relay\000RS-232\000Parallel\000ARCNet\000ARCNet Plus\000ATM\000MIO X.25\000SONET\000X.25 PLE\000ISO 802.211c\000LocalTalk\000SMDS DXI\000Frame Relay Service\000V.35\000HSSI\000HIPPI\000Modem\000AAL5\000SONET Path\000SONET VT\000SMDS ICIP\000Proprietary Virtual/Internal\000Proprietary Multiplexor\000IEEE 802.12\000Fibre Channel\000HIPPI Interface\000Frame Relay Interconnect\000ATM Emulated LAN for 802.3\000ATM Emulated LAN for 802.5\000ATM Emulated Circuit\000Fast Ethernet (100BaseT)\000ISDN\000V.11\000V.36\000G703 at 64K\000G703 at 2Mb\000QLLC\000Fast Ethernet 100BaseFX\000Channel\000IEEE 802.11\000IBM 260/370 OEMI Channel\000ESCON\000Data Link Switching\000ISDN S/T Interface\000ISDN U Interface\000LAP-D\000IP Switch\000Remote Source Route Bridging\000ATM Logical\000DS0\000DS0 Bundle\000BSC\000Async\000Combat Net Radio\000ISO 802.5r DTR\000Ext Pos Loc Report System\000AppleTalk Remote Access Protocol\000Proprietary Connectionless\000ITU X.29 Host PAD\000ITU X.3 Terminal PAD\000Frame Relay MPI\000ITU X.213\000ADSL\000RADSL\000SDSL\000VDSL\000ISO 802.5 CRFP\000Myrinet\000Voice Receive and Transmit\000Voice Foreign Exchange Office\000Voice Foreign Exchange Service\000Voice Encapsulation\000Voice over IP\000ATM DXI\000ATM FUNI\000ATM IMA\000PPP Multilink Bundle\000IP over CDLC\000IP over CLAW\000Stack to Stack\000Virtual IP Address\000MPC\000IP over ATM\000ISO 802.5j Fibre Token Ring\000TDLC\000Gigabit Ethernet\000HDLC\000LAP-F\000V.37\000X.25 MLP\000X.25 Hunt Group\000Transp HDLC\000Interleave Channel\000FAST Channel\000IP (for APPN HPR in IP Networks)\000CATV MAC Layer\000CATV Downstream\000CATV Upstream\000Avalon 12MPP Switch\000Tunnel\000Coffee\000Circuit Emulation Service\000ATM SubInterface\000Layer 2 VLAN using 802.1Q\000Layer 3 VLAN using IP\000Layer 3 VLAN using IPX\000Digital Power Line\000Multimedia Mail over IP\000DTM\000DCN\000IP Forwarding\000MSDSL\000IEEE 1394\000IF-GSN/HIPPI-6400\000DVB-RCC MAC Layer\000DVB-RCC Downstream\000DVB-RCC Upstream\000ATM Virtual\000MPLS Tunnel\000SRP\000Voice over ATM\000Voice over Frame Relay\000ISDL\000Composite Link\000SS7 Signaling Link\000Proprietary P2P Wireless\000Frame Forward\000RFC1483 Multiprotocol over ATM\000USB\000IEEE 802.3ad Link Aggregate\000BGP Policy Accounting\000FRF .16 Multilink FR\000H.323 Gatekeeper\000H.323 Proxy\000MPLS\000Multi-Frequency Signaling Link\000HDSL-2\000S-HDSL\000DS1 Facility Data Link\000Packet over SONET/SDH\000DVB-ASI Input\000DVB-ASI Output\000Power Line\000Non Facility Associated Signaling\000TR008\000GR303 RDT\000GR303 IDT\000ISUP\000Proprietary Wireless MAC Layer\000Proprietary Wireless Downstream\000Proprietary Wireless Upstream\000HIPERLAN Type 2\000Proprietary Broadband Wireless Access Point to Mulipoint\000SONET Overhead Channel\000Digital Wrapper Overhead Channel\000ATM Adaptation Layer 2\000Radio MAC\000ATM Radio\000Inter Machine Trunk\000MVL DSL\000Long Read DSL\000Frame Relay DLCI Endpoint\000ATM VCI Endpoint\000Optical Channel\000Optical Transport\000Proprietary ATM\000Voice over Cable\000Infiniband\000TE Link\000Q.2931\000Virtual Trunk Group\000SIP Trunk Group\000SIP Signaling\000CATV Upstream Channel\000Econet\000FSAN 155Mb PON\000FSAN 622Mb PON\000Transparent Bridge\000Line Group\000Voice E&M Feature Group\000Voice FGD EANA\000Voice DID\000MPEG Transport\0006To4\000GTP\000Paradyne EtherLoop 1\000Paradyne EtherLoop 2\000Optical Channel Group\000HomePNA\000GFP\000ciscoISLvlan\000actelisMetaLOOP\000Fcip\000IANA Reserved\000IPv4\000IPv6\000IPv4/v6\000IPX\000DECnet\000SNA\000CONP\000CLNP\000VINES\000XNS\000ISDN B Channel Endpoint\000ISDN D Channel Endpoint\000BGP\000OSPF\000UDP\000TCP\000802.11a\000802.11b\000802.11g\000802.11h\000DMTF Reserved\000Vendor Reserved\000",
    /* MappingStrings */
    "\026\000\001MIB.IETF|IF-MIB.ifType\000",
    /* ModelCorrespondence */
    "\036\000\001CIM_ProtocolEndpoint.OtherTypeDescription\000",
    0,
};

static MetaProperty
_CIM_ProtocolEndpoint_ProtocolIFType =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "ProtocolIFType",
    /* qualifiers */
    _CIM_ProtocolEndpoint_ProtocolIFType_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_ProtocolEndpoint_OtherTypeDescription_qualifiers[] =
{
    /* MaxLen */
    "\030\000\000\000\100",
    /* ModelCorrespondence */
    "\036\000\002CIM_ProtocolEndpoint.ProtocolType\000CIM_ProtocolEndpoint.ProtocolIFType\000",
    0,
};

static MetaProperty
_CIM_ProtocolEndpoint_OtherTypeDescription =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "OtherTypeDescription",
    /* qualifiers */
    _CIM_ProtocolEndpoint_OtherTypeDescription_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_ProtocolEndpoint_features[] =
{
    (MetaFeature*)&_CIM_ProtocolEndpoint_Description,
    (MetaFeature*)&_CIM_ProtocolEndpoint_OperationalStatus,
    (MetaFeature*)&_CIM_ProtocolEndpoint_EnabledState,
    (MetaFeature*)&_CIM_ProtocolEndpoint_TimeOfLastStateChange,
    (MetaFeature*)&_CIM_ProtocolEndpoint_Name,
    (MetaFeature*)&_CIM_ProtocolEndpoint_NameFormat,
    (MetaFeature*)&_CIM_ProtocolEndpoint_ProtocolType,
    (MetaFeature*)&_CIM_ProtocolEndpoint_ProtocolIFType,
    (MetaFeature*)&_CIM_ProtocolEndpoint_OtherTypeDescription,
    0,
};

static const char*
__root_PG_InterOp_CIM_ProtocolEndpoint_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Core::Service",
    /* Version */
    "\0632.8.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ProtocolEndpoint =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "CIM_ProtocolEndpoint",
    /* qualifiers */
    __root_PG_InterOp_CIM_ProtocolEndpoint_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_ServiceAccessPoint,
    /* features */
    _CIM_ProtocolEndpoint_features,
};

//==============================================================================
//
// Class: CIM_ProvidesServiceToElement
//
//==============================================================================

static const char*
_CIM_ProvidesServiceToElement_Antecedent_qualifiers[] =
{
    /* Deprecated */
    "\013\000\001CIM_ServiceAvailableToElement.ServiceProvided\000",
    /* Override */
    "\044Antecedent",
    0,
};

static MetaReference
_CIM_ProvidesServiceToElement_Antecedent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Antecedent",
    /* qualifiers */
    _CIM_ProvidesServiceToElement_Antecedent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_Service,
};

static const char*
_CIM_ProvidesServiceToElement_Dependent_qualifiers[] =
{
    /* Deprecated */
    "\013\000\001CIM_ServiceAvailableToElement.UserOfService\000",
    /* Override */
    "\044Dependent",
    0,
};

static MetaReference
_CIM_ProvidesServiceToElement_Dependent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Dependent",
    /* qualifiers */
    _CIM_ProvidesServiceToElement_Dependent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ManagedElement,
};

static MetaFeature*
_CIM_ProvidesServiceToElement_features[] =
{
    (MetaFeature*)&_CIM_ProvidesServiceToElement_Antecedent,
    (MetaFeature*)&_CIM_ProvidesServiceToElement_Dependent,
    0,
};

static const char*
__root_PG_InterOp_CIM_ProvidesServiceToElement_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* Deprecated */
    "\013\000\001CIM_ServiceAvailableToElement\000",
    /* UMLPackagePath */
    "\057CIM::Core::Service",
    /* Version */
    "\0632.10.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ProvidesServiceToElement =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_ProvidesServiceToElement",
    /* qualifiers */
    __root_PG_InterOp_CIM_ProvidesServiceToElement_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_Dependency,
    /* features */
    _CIM_ProvidesServiceToElement_features,
};

//==============================================================================
//
// Class: CIM_RemoteServiceAccessPoint
//
//==============================================================================

static const char*
_CIM_RemoteServiceAccessPoint_AccessInfo_qualifiers[] =
{
    /* ModelCorrespondence */
    "\036\000\001CIM_RemoteServiceAccessPoint.InfoFormat\000",
    0,
};

static MetaProperty
_CIM_RemoteServiceAccessPoint_AccessInfo =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "AccessInfo",
    /* qualifiers */
    _CIM_RemoteServiceAccessPoint_AccessInfo_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_RemoteServiceAccessPoint_InfoFormat_qualifiers[] =
{
    /* ValueMap */
    "\061\000\0321\0002\0003\0004\0005\0006\0007\0008\0009\00010\00011\00012\00013\000100\000101\000102\000103\000104\000200\000201\000202\000203\000204\000205\000..\00032768..65535\000",
    /* Values */
    "\062\000\032Other\000Host Name\000IPv4 Address\000IPv6 Address\000IPX Address\000DECnet Address\000SNA Address\000Autonomous System Number\000MPLS Label\000IPv4 Subnet Address\000IPv6 Subnet Address\000IPv4 Address Range\000IPv6 Address Range\000Dial String\000Ethernet Address\000Token Ring Address\000ATM Address\000Frame Relay Address\000URL\000FQDN\000User FQDN\000DER ASN1 DN\000DER ASN1 GN\000Key ID\000DMTF Reserved\000Vendor Reserved\000",
    /* ModelCorrespondence */
    "\036\000\001CIM_RemoteServiceAccessPoint.OtherInfoFormatDescription\000",
    0,
};

static MetaProperty
_CIM_RemoteServiceAccessPoint_InfoFormat =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "InfoFormat",
    /* qualifiers */
    _CIM_RemoteServiceAccessPoint_InfoFormat_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_RemoteServiceAccessPoint_OtherInfoFormatDescription_qualifiers[] =
{
    /* ModelCorrespondence */
    "\036\000\001CIM_RemoteServiceAccessPoint.InfoFormat\000",
    0,
};

static MetaProperty
_CIM_RemoteServiceAccessPoint_OtherInfoFormatDescription =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "OtherInfoFormatDescription",
    /* qualifiers */
    _CIM_RemoteServiceAccessPoint_OtherInfoFormatDescription_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_RemoteServiceAccessPoint_features[] =
{
    (MetaFeature*)&_CIM_RemoteServiceAccessPoint_AccessInfo,
    (MetaFeature*)&_CIM_RemoteServiceAccessPoint_InfoFormat,
    (MetaFeature*)&_CIM_RemoteServiceAccessPoint_OtherInfoFormatDescription,
    0,
};

static const char*
__root_PG_InterOp_CIM_RemoteServiceAccessPoint_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Core::Service",
    /* Version */
    "\0632.10.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_RemoteServiceAccessPoint =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "CIM_RemoteServiceAccessPoint",
    /* qualifiers */
    __root_PG_InterOp_CIM_RemoteServiceAccessPoint_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_ServiceAccessPoint,
    /* features */
    _CIM_RemoteServiceAccessPoint_features,
};

//==============================================================================
//
// Class: CIM_SAPAvailableForElement
//
//==============================================================================

static const char*
_CIM_SAPAvailableForElement_AvailableSAP_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_SAPAvailableForElement_AvailableSAP =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "AvailableSAP",
    /* qualifiers */
    _CIM_SAPAvailableForElement_AvailableSAP_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ServiceAccessPoint,
};

static const char*
_CIM_SAPAvailableForElement_ManagedElement_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_SAPAvailableForElement_ManagedElement =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "ManagedElement",
    /* qualifiers */
    _CIM_SAPAvailableForElement_ManagedElement_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ManagedElement,
};

static MetaFeature*
_CIM_SAPAvailableForElement_features[] =
{
    (MetaFeature*)&_CIM_SAPAvailableForElement_AvailableSAP,
    (MetaFeature*)&_CIM_SAPAvailableForElement_ManagedElement,
    0,
};

static const char*
__root_PG_InterOp_CIM_SAPAvailableForElement_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::Service",
    /* Version */
    "\0632.10.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_SAPAvailableForElement =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_SAPAvailableForElement",
    /* qualifiers */
    __root_PG_InterOp_CIM_SAPAvailableForElement_qualifiers,
    /* super */
    0,
    /* features */
    _CIM_SAPAvailableForElement_features,
};

//==============================================================================
//
// Class: CIM_SAPSAPDependency
//
//==============================================================================

static const char*
_CIM_SAPSAPDependency_Antecedent_qualifiers[] =
{
    /* Override */
    "\044Antecedent",
    0,
};

static MetaReference
_CIM_SAPSAPDependency_Antecedent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Antecedent",
    /* qualifiers */
    _CIM_SAPSAPDependency_Antecedent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ServiceAccessPoint,
};

static const char*
_CIM_SAPSAPDependency_Dependent_qualifiers[] =
{
    /* Override */
    "\044Dependent",
    0,
};

static MetaReference
_CIM_SAPSAPDependency_Dependent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Dependent",
    /* qualifiers */
    _CIM_SAPSAPDependency_Dependent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ServiceAccessPoint,
};

static MetaFeature*
_CIM_SAPSAPDependency_features[] =
{
    (MetaFeature*)&_CIM_SAPSAPDependency_Antecedent,
    (MetaFeature*)&_CIM_SAPSAPDependency_Dependent,
    0,
};

static const char*
__root_PG_InterOp_CIM_SAPSAPDependency_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::Service",
    /* Version */
    "\0632.10.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_SAPSAPDependency =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_SAPSAPDependency",
    /* qualifiers */
    __root_PG_InterOp_CIM_SAPSAPDependency_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_Dependency,
    /* features */
    _CIM_SAPSAPDependency_features,
};

//==============================================================================
//
// Class: CIM_SAPStatistics
//
//==============================================================================

static const char*
_CIM_SAPStatistics_Stats_qualifiers[] =
{
    /* Override */
    "\044Stats",
    /* Weak */
    "\064\001",
    0,
};

static MetaReference
_CIM_SAPStatistics_Stats =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ|META_FLAG_WEAK,
    /* name */
    "Stats",
    /* qualifiers */
    _CIM_SAPStatistics_Stats_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_SAPStatisticalInformation,
};

static const char*
_CIM_SAPStatistics_Element_qualifiers[] =
{
    /* Override */
    "\044Element",
    /* Min */
    "\033\000\000\000\001",
    /* Max */
    "\027\000\000\000\001",
    0,
};

static MetaReference
_CIM_SAPStatistics_Element =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Element",
    /* qualifiers */
    _CIM_SAPStatistics_Element_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ServiceAccessPoint,
};

static MetaFeature*
_CIM_SAPStatistics_features[] =
{
    (MetaFeature*)&_CIM_SAPStatistics_Stats,
    (MetaFeature*)&_CIM_SAPStatistics_Element,
    0,
};

static const char*
__root_PG_InterOp_CIM_SAPStatistics_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::Statistics",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_SAPStatistics =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_SAPStatistics",
    /* qualifiers */
    __root_PG_InterOp_CIM_SAPStatistics_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_Statistics,
    /* features */
    _CIM_SAPStatistics_features,
};

//==============================================================================
//
// Class: CIM_ServiceAccessBySAP
//
//==============================================================================

static const char*
_CIM_ServiceAccessBySAP_Antecedent_qualifiers[] =
{
    /* Override */
    "\044Antecedent",
    0,
};

static MetaReference
_CIM_ServiceAccessBySAP_Antecedent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Antecedent",
    /* qualifiers */
    _CIM_ServiceAccessBySAP_Antecedent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_Service,
};

static const char*
_CIM_ServiceAccessBySAP_Dependent_qualifiers[] =
{
    /* Override */
    "\044Dependent",
    0,
};

static MetaReference
_CIM_ServiceAccessBySAP_Dependent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Dependent",
    /* qualifiers */
    _CIM_ServiceAccessBySAP_Dependent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ServiceAccessPoint,
};

static MetaFeature*
_CIM_ServiceAccessBySAP_features[] =
{
    (MetaFeature*)&_CIM_ServiceAccessBySAP_Antecedent,
    (MetaFeature*)&_CIM_ServiceAccessBySAP_Dependent,
    0,
};

static const char*
__root_PG_InterOp_CIM_ServiceAccessBySAP_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::Service",
    /* Version */
    "\0632.10.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ServiceAccessBySAP =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_ServiceAccessBySAP",
    /* qualifiers */
    __root_PG_InterOp_CIM_ServiceAccessBySAP_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_Dependency,
    /* features */
    _CIM_ServiceAccessBySAP_features,
};

//==============================================================================
//
// Class: CIM_ServiceServiceDependency
//
//==============================================================================

static const char*
_CIM_ServiceServiceDependency_Antecedent_qualifiers[] =
{
    /* Override */
    "\044Antecedent",
    0,
};

static MetaReference
_CIM_ServiceServiceDependency_Antecedent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Antecedent",
    /* qualifiers */
    _CIM_ServiceServiceDependency_Antecedent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_Service,
};

static const char*
_CIM_ServiceServiceDependency_Dependent_qualifiers[] =
{
    /* Override */
    "\044Dependent",
    0,
};

static MetaReference
_CIM_ServiceServiceDependency_Dependent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Dependent",
    /* qualifiers */
    _CIM_ServiceServiceDependency_Dependent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_Service,
};

static const char*
_CIM_ServiceServiceDependency_TypeOfDependency_qualifiers[] =
{
    /* ValueMap */
    "\061\000\0050\0001\0002\0003\0004\000",
    /* Values */
    "\062\000\005Unknown\000Other\000Service Must Have Completed\000Service Must Be Started\000Service Must Not Be Started\000",
    0,
};

static MetaProperty
_CIM_ServiceServiceDependency_TypeOfDependency =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "TypeOfDependency",
    /* qualifiers */
    _CIM_ServiceServiceDependency_TypeOfDependency_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_ServiceServiceDependency_RestartService_qualifiers[] =
{
    0,
};

static MetaProperty
_CIM_ServiceServiceDependency_RestartService =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "RestartService",
    /* qualifiers */
    _CIM_ServiceServiceDependency_RestartService_qualifiers,
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_ServiceServiceDependency_features[] =
{
    (MetaFeature*)&_CIM_ServiceServiceDependency_Antecedent,
    (MetaFeature*)&_CIM_ServiceServiceDependency_Dependent,
    (MetaFeature*)&_CIM_ServiceServiceDependency_TypeOfDependency,
    (MetaFeature*)&_CIM_ServiceServiceDependency_RestartService,
    0,
};

static const char*
__root_PG_InterOp_CIM_ServiceServiceDependency_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::Service",
    /* Version */
    "\0632.10.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ServiceServiceDependency =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_ServiceServiceDependency",
    /* qualifiers */
    __root_PG_InterOp_CIM_ServiceServiceDependency_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_ProvidesServiceToElement,
    /* features */
    _CIM_ServiceServiceDependency_features,
};

//==============================================================================
//
// Class: CIM_SettingForSystem
//
//==============================================================================

static const char*
_CIM_SettingForSystem_Antecedent_qualifiers[] =
{
    /* Override */
    "\044Antecedent",
    /* Min */
    "\033\000\000\000\001",
    /* Max */
    "\027\000\000\000\001",
    0,
};

static MetaReference
_CIM_SettingForSystem_Antecedent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Antecedent",
    /* qualifiers */
    _CIM_SettingForSystem_Antecedent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_System,
};

static const char*
_CIM_SettingForSystem_Dependent_qualifiers[] =
{
    /* Override */
    "\044Dependent",
    /* Weak */
    "\064\001",
    0,
};

static MetaReference
_CIM_SettingForSystem_Dependent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ|META_FLAG_WEAK,
    /* name */
    "Dependent",
    /* qualifiers */
    _CIM_SettingForSystem_Dependent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_SystemSetting,
};

static MetaFeature*
_CIM_SettingForSystem_features[] =
{
    (MetaFeature*)&_CIM_SettingForSystem_Antecedent,
    (MetaFeature*)&_CIM_SettingForSystem_Dependent,
    0,
};

static const char*
__root_PG_InterOp_CIM_SettingForSystem_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::Settings",
    /* Version */
    "\0632.10.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_SettingForSystem =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_SettingForSystem",
    /* qualifiers */
    __root_PG_InterOp_CIM_SettingForSystem_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_HostedDependency,
    /* features */
    _CIM_SettingForSystem_features,
};

//==============================================================================
//
// Class: CIM_ActiveConnection
//
//==============================================================================

static const char*
_CIM_ActiveConnection_Antecedent_qualifiers[] =
{
    /* Override */
    "\044Antecedent",
    0,
};

static MetaReference
_CIM_ActiveConnection_Antecedent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Antecedent",
    /* qualifiers */
    _CIM_ActiveConnection_Antecedent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ServiceAccessPoint,
};

static const char*
_CIM_ActiveConnection_Dependent_qualifiers[] =
{
    /* Override */
    "\044Dependent",
    0,
};

static MetaReference
_CIM_ActiveConnection_Dependent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Dependent",
    /* qualifiers */
    _CIM_ActiveConnection_Dependent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ServiceAccessPoint,
};

static const char*
_CIM_ActiveConnection_TrafficType_qualifiers[] =
{
    /* Deprecated */
    "\013\000\001No value\000",
    /* ValueMap */
    "\061\000\0060\0001\0002\0003\0004\0005\000",
    /* Values */
    "\062\000\006Unknown\000Other\000Unicast\000Broadcast\000Multicast\000Anycast\000",
    /* ModelCorrespondence */
    "\036\000\001CIM_ActiveConnection.OtherTrafficDescription\000",
    0,
};

static MetaProperty
_CIM_ActiveConnection_TrafficType =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "TrafficType",
    /* qualifiers */
    _CIM_ActiveConnection_TrafficType_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_ActiveConnection_OtherTrafficDescription_qualifiers[] =
{
    /* Deprecated */
    "\013\000\001No value\000",
    /* MaxLen */
    "\030\000\000\000\100",
    /* ModelCorrespondence */
    "\036\000\001CIM_ActiveConnection.TrafficType\000",
    0,
};

static MetaProperty
_CIM_ActiveConnection_OtherTrafficDescription =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "OtherTrafficDescription",
    /* qualifiers */
    _CIM_ActiveConnection_OtherTrafficDescription_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_ActiveConnection_IsUnidirectional_qualifiers[] =
{
    0,
};

static MetaProperty
_CIM_ActiveConnection_IsUnidirectional =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "IsUnidirectional",
    /* qualifiers */
    _CIM_ActiveConnection_IsUnidirectional_qualifiers,
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_ActiveConnection_features[] =
{
    (MetaFeature*)&_CIM_ActiveConnection_Antecedent,
    (MetaFeature*)&_CIM_ActiveConnection_Dependent,
    (MetaFeature*)&_CIM_ActiveConnection_TrafficType,
    (MetaFeature*)&_CIM_ActiveConnection_OtherTrafficDescription,
    (MetaFeature*)&_CIM_ActiveConnection_IsUnidirectional,
    0,
};

static const char*
__root_PG_InterOp_CIM_ActiveConnection_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::Service",
    /* Version */
    "\0632.10.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ActiveConnection =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_ActiveConnection",
    /* qualifiers */
    __root_PG_InterOp_CIM_ActiveConnection_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_SAPSAPDependency,
    /* features */
    _CIM_ActiveConnection_features,
};

//==============================================================================
//
// Class: CIM_BindsTo
//
//==============================================================================

static const char*
_CIM_BindsTo_Antecedent_qualifiers[] =
{
    /* Override */
    "\044Antecedent",
    0,
};

static MetaReference
_CIM_BindsTo_Antecedent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Antecedent",
    /* qualifiers */
    _CIM_BindsTo_Antecedent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ProtocolEndpoint,
};

static const char*
_CIM_BindsTo_Dependent_qualifiers[] =
{
    /* Override */
    "\044Dependent",
    0,
};

static MetaReference
_CIM_BindsTo_Dependent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Dependent",
    /* qualifiers */
    _CIM_BindsTo_Dependent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ServiceAccessPoint,
};

static MetaFeature*
_CIM_BindsTo_features[] =
{
    (MetaFeature*)&_CIM_BindsTo_Antecedent,
    (MetaFeature*)&_CIM_BindsTo_Dependent,
    0,
};

static const char*
__root_PG_InterOp_CIM_BindsTo_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::Service",
    /* Version */
    "\0632.10.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_BindsTo =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_BindsTo",
    /* qualifiers */
    __root_PG_InterOp_CIM_BindsTo_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_SAPSAPDependency,
    /* features */
    _CIM_BindsTo_features,
};

//==============================================================================
//
// Class: CIM_ProvidesEndpoint
//
//==============================================================================

static const char*
_CIM_ProvidesEndpoint_Antecedent_qualifiers[] =
{
    /* Override */
    "\044Antecedent",
    0,
};

static MetaReference
_CIM_ProvidesEndpoint_Antecedent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Antecedent",
    /* qualifiers */
    _CIM_ProvidesEndpoint_Antecedent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_Service,
};

static const char*
_CIM_ProvidesEndpoint_Dependent_qualifiers[] =
{
    /* Override */
    "\044Dependent",
    0,
};

static MetaReference
_CIM_ProvidesEndpoint_Dependent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Dependent",
    /* qualifiers */
    _CIM_ProvidesEndpoint_Dependent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ProtocolEndpoint,
};

static MetaFeature*
_CIM_ProvidesEndpoint_features[] =
{
    (MetaFeature*)&_CIM_ProvidesEndpoint_Antecedent,
    (MetaFeature*)&_CIM_ProvidesEndpoint_Dependent,
    0,
};

static const char*
__root_PG_InterOp_CIM_ProvidesEndpoint_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Core::Service",
    /* Version */
    "\0632.10.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ProvidesEndpoint =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_ProvidesEndpoint",
    /* qualifiers */
    __root_PG_InterOp_CIM_ProvidesEndpoint_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_ServiceAccessBySAP,
    /* features */
    _CIM_ProvidesEndpoint_features,
};

//==============================================================================
//
// Class: CIM_RemotePort
//
//==============================================================================

static const char*
_CIM_RemotePort_PortInfo_qualifiers[] =
{
    /* ModelCorrespondence */
    "\036\000\001CIM_RemotePort.PortProtocol\000",
    0,
};

static MetaProperty
_CIM_RemotePort_PortInfo =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "PortInfo",
    /* qualifiers */
    _CIM_RemotePort_PortInfo_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_RemotePort_PortProtocol_qualifiers[] =
{
    /* ValueMap */
    "\061\000\0041\0002\0003\00032768..65535\000",
    /* Values */
    "\062\000\004Other\000TCP\000UDP\000Vendor Specific\000",
    /* ModelCorrespondence */
    "\036\000\001CIM_RemotePort.OtherProtocolDescription\000",
    0,
};

static MetaProperty
_CIM_RemotePort_PortProtocol =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "PortProtocol",
    /* qualifiers */
    _CIM_RemotePort_PortProtocol_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_RemotePort_OtherProtocolDescription_qualifiers[] =
{
    /* ModelCorrespondence */
    "\036\000\001CIM_RemotePort.PortProtocol\000",
    0,
};

static MetaProperty
_CIM_RemotePort_OtherProtocolDescription =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "OtherProtocolDescription",
    /* qualifiers */
    _CIM_RemotePort_OtherProtocolDescription_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_RemotePort_features[] =
{
    (MetaFeature*)&_CIM_RemotePort_PortInfo,
    (MetaFeature*)&_CIM_RemotePort_PortProtocol,
    (MetaFeature*)&_CIM_RemotePort_OtherProtocolDescription,
    0,
};

static const char*
__root_PG_InterOp_CIM_RemotePort_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Core::Service",
    /* Version */
    "\0632.7.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_RemotePort =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "CIM_RemotePort",
    /* qualifiers */
    __root_PG_InterOp_CIM_RemotePort_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_RemoteServiceAccessPoint,
    /* features */
    _CIM_RemotePort_features,
};

//==============================================================================
//
// Class: CIM_Indication
//
//==============================================================================

static const char*
_CIM_Indication_IndicationIdentifier_qualifiers[] =
{
    /* MappingStrings */
    "\026\000\001Recommendation.ITU|X733.Notification identifier\000",
    0,
};

static MetaProperty
_CIM_Indication_IndicationIdentifier =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "IndicationIdentifier",
    /* qualifiers */
    _CIM_Indication_IndicationIdentifier_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Indication_CorrelatedIndications_qualifiers[] =
{
    /* MappingStrings */
    "\026\000\001Recommendation.ITU|X733.Correlated notifications\000",
    /* ModelCorrespondence */
    "\036\000\001CIM_Indication.IndicationIdentifier\000",
    0,
};

static MetaProperty
_CIM_Indication_CorrelatedIndications =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "CorrelatedIndications",
    /* qualifiers */
    _CIM_Indication_CorrelatedIndications_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* value */
    0,
};

static const char*
_CIM_Indication_IndicationTime_qualifiers[] =
{
    0,
};

static MetaProperty
_CIM_Indication_IndicationTime =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "IndicationTime",
    /* qualifiers */
    _CIM_Indication_IndicationTime_qualifiers,
    /* type */
    CIMTYPE_DATETIME,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Indication_PerceivedSeverity_qualifiers[] =
{
    /* ValueMap */
    "\061\000\0100\0001\0002\0003\0004\0005\0006\0007\000",
    /* Values */
    "\062\000\010Unknown\000Other\000Information\000Degraded/Warning\000Minor\000Major\000Critical\000Fatal/NonRecoverable\000",
    /* MappingStrings */
    "\026\000\001Recommendation.ITU|X733.Perceived severity\000",
    0,
};

static MetaProperty
_CIM_Indication_PerceivedSeverity =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "PerceivedSeverity",
    /* qualifiers */
    _CIM_Indication_PerceivedSeverity_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Indication_OtherSeverity_qualifiers[] =
{
    /* ModelCorrespondence */
    "\036\000\001CIM_AlertIndication.PerceivedSeverity\000",
    0,
};

static MetaProperty
_CIM_Indication_OtherSeverity =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "OtherSeverity",
    /* qualifiers */
    _CIM_Indication_OtherSeverity_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_Indication_features[] =
{
    (MetaFeature*)&_CIM_Indication_IndicationIdentifier,
    (MetaFeature*)&_CIM_Indication_CorrelatedIndications,
    (MetaFeature*)&_CIM_Indication_IndicationTime,
    (MetaFeature*)&_CIM_Indication_PerceivedSeverity,
    (MetaFeature*)&_CIM_Indication_OtherSeverity,
    0,
};

static const char*
__root_PG_InterOp_CIM_Indication_qualifiers[] =
{
    /* INDICATION */
    "\001\001",
    /* UMLPackagePath */
    "\057CIM::Event",
    /* Abstract */
    "\002\001",
    /* Version */
    "\0632.7.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_Indication =
{
    /* flags */
    META_FLAG_INDICATION|META_FLAG_ABSTRACT,
    /* name */
    "CIM_Indication",
    /* qualifiers */
    __root_PG_InterOp_CIM_Indication_qualifiers,
    /* super */
    0,
    /* features */
    _CIM_Indication_features,
};

//==============================================================================
//
// Class: CIM_ClassIndication
//
//==============================================================================

static const char*
_CIM_ClassIndication_ClassDefinition_qualifiers[] =
{
    /* Required */
    "\050\001",
    /* EmbeddedObject */
    "\020\001",
    0,
};

static MetaProperty
_CIM_ClassIndication_ClassDefinition =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_EMBEDDEDOBJECT|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "ClassDefinition",
    /* qualifiers */
    _CIM_ClassIndication_ClassDefinition_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_ClassIndication_features[] =
{
    (MetaFeature*)&_CIM_ClassIndication_ClassDefinition,
    0,
};

static const char*
__root_PG_InterOp_CIM_ClassIndication_qualifiers[] =
{
    /* INDICATION */
    "\001\001",
    /* UMLPackagePath */
    "\057CIM::Event",
    /* Abstract */
    "\002\001",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ClassIndication =
{
    /* flags */
    META_FLAG_INDICATION|META_FLAG_ABSTRACT,
    /* name */
    "CIM_ClassIndication",
    /* qualifiers */
    __root_PG_InterOp_CIM_ClassIndication_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_Indication,
    /* features */
    _CIM_ClassIndication_features,
};

//==============================================================================
//
// Class: CIM_ClassModification
//
//==============================================================================

static const char*
_CIM_ClassModification_PreviousClassDefinition_qualifiers[] =
{
    /* Required */
    "\050\001",
    /* EmbeddedObject */
    "\020\001",
    0,
};

static MetaProperty
_CIM_ClassModification_PreviousClassDefinition =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_EMBEDDEDOBJECT|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "PreviousClassDefinition",
    /* qualifiers */
    _CIM_ClassModification_PreviousClassDefinition_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_ClassModification_features[] =
{
    (MetaFeature*)&_CIM_ClassModification_PreviousClassDefinition,
    0,
};

static const char*
__root_PG_InterOp_CIM_ClassModification_qualifiers[] =
{
    /* INDICATION */
    "\001\001",
    /* UMLPackagePath */
    "\057CIM::Event",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ClassModification =
{
    /* flags */
    META_FLAG_INDICATION,
    /* name */
    "CIM_ClassModification",
    /* qualifiers */
    __root_PG_InterOp_CIM_ClassModification_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_ClassIndication,
    /* features */
    _CIM_ClassModification_features,
};

//==============================================================================
//
// Class: CIM_IndicationFilter
//
//==============================================================================

static const char*
_CIM_IndicationFilter_SystemCreationClassName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_IndicationFilter_SystemCreationClassName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "SystemCreationClassName",
    /* qualifiers */
    _CIM_IndicationFilter_SystemCreationClassName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_IndicationFilter_SystemName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_IndicationFilter_SystemName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "SystemName",
    /* qualifiers */
    _CIM_IndicationFilter_SystemName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_IndicationFilter_CreationClassName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_IndicationFilter_CreationClassName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "CreationClassName",
    /* qualifiers */
    _CIM_IndicationFilter_CreationClassName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_IndicationFilter_Name_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaProperty
_CIM_IndicationFilter_Name =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Name",
    /* qualifiers */
    _CIM_IndicationFilter_Name_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_IndicationFilter_SourceNamespace_qualifiers[] =
{
    0,
};

static MetaProperty
_CIM_IndicationFilter_SourceNamespace =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "SourceNamespace",
    /* qualifiers */
    _CIM_IndicationFilter_SourceNamespace_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_IndicationFilter_Query_qualifiers[] =
{
    /* Required */
    "\050\001",
    /* ModelCorrespondence */
    "\036\000\001CIM_IndicationFilter.QueryLanguage\000",
    0,
};

static MetaProperty
_CIM_IndicationFilter_Query =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "Query",
    /* qualifiers */
    _CIM_IndicationFilter_Query_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_IndicationFilter_QueryLanguage_qualifiers[] =
{
    /* Required */
    "\050\001",
    /* ModelCorrespondence */
    "\036\000\001CIM_IndicationFilter.Query\000",
    0,
};

static MetaProperty
_CIM_IndicationFilter_QueryLanguage =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "QueryLanguage",
    /* qualifiers */
    _CIM_IndicationFilter_QueryLanguage_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_IndicationFilter_features[] =
{
    (MetaFeature*)&_CIM_IndicationFilter_SystemCreationClassName,
    (MetaFeature*)&_CIM_IndicationFilter_SystemName,
    (MetaFeature*)&_CIM_IndicationFilter_CreationClassName,
    (MetaFeature*)&_CIM_IndicationFilter_Name,
    (MetaFeature*)&_CIM_IndicationFilter_SourceNamespace,
    (MetaFeature*)&_CIM_IndicationFilter_Query,
    (MetaFeature*)&_CIM_IndicationFilter_QueryLanguage,
    0,
};

static const char*
__root_PG_InterOp_CIM_IndicationFilter_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Event",
    /* Version */
    "\0632.12.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_IndicationFilter =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "CIM_IndicationFilter",
    /* qualifiers */
    __root_PG_InterOp_CIM_IndicationFilter_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_ManagedElement,
    /* features */
    _CIM_IndicationFilter_features,
};

//==============================================================================
//
// Class: CIM_InstIndication
//
//==============================================================================

static const char*
_CIM_InstIndication_SourceInstance_qualifiers[] =
{
    /* Required */
    "\050\001",
    /* EmbeddedObject */
    "\020\001",
    0,
};

static MetaProperty
_CIM_InstIndication_SourceInstance =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_EMBEDDEDOBJECT|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "SourceInstance",
    /* qualifiers */
    _CIM_InstIndication_SourceInstance_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_InstIndication_SourceInstanceModelPath_qualifiers[] =
{
    /* ModelCorrespondence */
    "\036\000\001CIM_InstIndication.SourceInstance\000",
    0,
};

static MetaProperty
_CIM_InstIndication_SourceInstanceModelPath =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "SourceInstanceModelPath",
    /* qualifiers */
    _CIM_InstIndication_SourceInstanceModelPath_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_InstIndication_SourceInstanceHost_qualifiers[] =
{
    /* ModelCorrespondence */
    "\036\000\001CIM_InstIndication.SourceInstance\000",
    0,
};

static MetaProperty
_CIM_InstIndication_SourceInstanceHost =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "SourceInstanceHost",
    /* qualifiers */
    _CIM_InstIndication_SourceInstanceHost_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_InstIndication_features[] =
{
    (MetaFeature*)&_CIM_InstIndication_SourceInstance,
    (MetaFeature*)&_CIM_InstIndication_SourceInstanceModelPath,
    (MetaFeature*)&_CIM_InstIndication_SourceInstanceHost,
    0,
};

static const char*
__root_PG_InterOp_CIM_InstIndication_qualifiers[] =
{
    /* INDICATION */
    "\001\001",
    /* UMLPackagePath */
    "\057CIM::Event",
    /* Abstract */
    "\002\001",
    /* Version */
    "\0632.9.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_InstIndication =
{
    /* flags */
    META_FLAG_INDICATION|META_FLAG_ABSTRACT,
    /* name */
    "CIM_InstIndication",
    /* qualifiers */
    __root_PG_InterOp_CIM_InstIndication_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_Indication,
    /* features */
    _CIM_InstIndication_features,
};

//==============================================================================
//
// Class: CIM_InstMethodCall
//
//==============================================================================

static const char*
_CIM_InstMethodCall_MethodName_qualifiers[] =
{
    /* Required */
    "\050\001",
    0,
};

static MetaProperty
_CIM_InstMethodCall_MethodName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "MethodName",
    /* qualifiers */
    _CIM_InstMethodCall_MethodName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_InstMethodCall_MethodParameters_qualifiers[] =
{
    /* EmbeddedObject */
    "\020\001",
    0,
};

static MetaProperty
_CIM_InstMethodCall_MethodParameters =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_EMBEDDEDOBJECT|META_FLAG_READ,
    /* name */
    "MethodParameters",
    /* qualifiers */
    _CIM_InstMethodCall_MethodParameters_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_InstMethodCall_ReturnValue_qualifiers[] =
{
    /* ModelCorrespondence */
    "\036\000\001CIM_InstMethodCall.PreCall\000",
    0,
};

static MetaProperty
_CIM_InstMethodCall_ReturnValue =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "ReturnValue",
    /* qualifiers */
    _CIM_InstMethodCall_ReturnValue_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_InstMethodCall_PreCall_qualifiers[] =
{
    /* Required */
    "\050\001",
    0,
};

static MetaProperty
_CIM_InstMethodCall_PreCall =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "PreCall",
    /* qualifiers */
    _CIM_InstMethodCall_PreCall_qualifiers,
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_InstMethodCall_features[] =
{
    (MetaFeature*)&_CIM_InstMethodCall_MethodName,
    (MetaFeature*)&_CIM_InstMethodCall_MethodParameters,
    (MetaFeature*)&_CIM_InstMethodCall_ReturnValue,
    (MetaFeature*)&_CIM_InstMethodCall_PreCall,
    0,
};

static const char*
__root_PG_InterOp_CIM_InstMethodCall_qualifiers[] =
{
    /* INDICATION */
    "\001\001",
    /* UMLPackagePath */
    "\057CIM::Event",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_InstMethodCall =
{
    /* flags */
    META_FLAG_INDICATION,
    /* name */
    "CIM_InstMethodCall",
    /* qualifiers */
    __root_PG_InterOp_CIM_InstMethodCall_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_InstIndication,
    /* features */
    _CIM_InstMethodCall_features,
};

//==============================================================================
//
// Class: CIM_InstModification
//
//==============================================================================

static const char*
_CIM_InstModification_PreviousInstance_qualifiers[] =
{
    /* Required */
    "\050\001",
    /* EmbeddedObject */
    "\020\001",
    0,
};

static MetaProperty
_CIM_InstModification_PreviousInstance =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_EMBEDDEDOBJECT|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "PreviousInstance",
    /* qualifiers */
    _CIM_InstModification_PreviousInstance_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_InstModification_features[] =
{
    (MetaFeature*)&_CIM_InstModification_PreviousInstance,
    0,
};

static const char*
__root_PG_InterOp_CIM_InstModification_qualifiers[] =
{
    /* INDICATION */
    "\001\001",
    /* UMLPackagePath */
    "\057CIM::Event",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_InstModification =
{
    /* flags */
    META_FLAG_INDICATION,
    /* name */
    "CIM_InstModification",
    /* qualifiers */
    __root_PG_InterOp_CIM_InstModification_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_InstIndication,
    /* features */
    _CIM_InstModification_features,
};

//==============================================================================
//
// Class: CIM_InstRead
//
//==============================================================================

static MetaFeature*
_CIM_InstRead_features[] =
{
    0,
};

static const char*
__root_PG_InterOp_CIM_InstRead_qualifiers[] =
{
    /* INDICATION */
    "\001\001",
    /* UMLPackagePath */
    "\057CIM::Event",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_InstRead =
{
    /* flags */
    META_FLAG_INDICATION,
    /* name */
    "CIM_InstRead",
    /* qualifiers */
    __root_PG_InterOp_CIM_InstRead_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_InstIndication,
    /* features */
    _CIM_InstRead_features,
};

//==============================================================================
//
// Class: CIM_ListenerDestination
//
//==============================================================================

static const char*
_CIM_ListenerDestination_SystemCreationClassName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_ListenerDestination_SystemCreationClassName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "SystemCreationClassName",
    /* qualifiers */
    _CIM_ListenerDestination_SystemCreationClassName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_ListenerDestination_SystemName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_ListenerDestination_SystemName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "SystemName",
    /* qualifiers */
    _CIM_ListenerDestination_SystemName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_ListenerDestination_CreationClassName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_ListenerDestination_CreationClassName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "CreationClassName",
    /* qualifiers */
    _CIM_ListenerDestination_CreationClassName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_ListenerDestination_Name_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_ListenerDestination_Name =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Name",
    /* qualifiers */
    _CIM_ListenerDestination_Name_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_ListenerDestination_PersistenceType_qualifiers[] =
{
    /* ValueMap */
    "\061\000\0031\0002\0003\000",
    /* Values */
    "\062\000\003Other\000Permanent\000Transient\000",
    /* ModelCorrespondence */
    "\036\000\001CIM_ListenerDestination.OtherPersistenceType\000",
    0,
};

static MetaProperty
_CIM_ListenerDestination_PersistenceType =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "PersistenceType",
    /* qualifiers */
    _CIM_ListenerDestination_PersistenceType_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_ListenerDestination_OtherPersistenceType_qualifiers[] =
{
    /* ModelCorrespondence */
    "\036\000\001CIM_ListenerDestination.PersistenceType\000",
    0,
};

static MetaProperty
_CIM_ListenerDestination_OtherPersistenceType =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "OtherPersistenceType",
    /* qualifiers */
    _CIM_ListenerDestination_OtherPersistenceType_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_ListenerDestination_features[] =
{
    (MetaFeature*)&_CIM_ListenerDestination_SystemCreationClassName,
    (MetaFeature*)&_CIM_ListenerDestination_SystemName,
    (MetaFeature*)&_CIM_ListenerDestination_CreationClassName,
    (MetaFeature*)&_CIM_ListenerDestination_Name,
    (MetaFeature*)&_CIM_ListenerDestination_PersistenceType,
    (MetaFeature*)&_CIM_ListenerDestination_OtherPersistenceType,
    0,
};

static const char*
__root_PG_InterOp_CIM_ListenerDestination_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Event",
    /* Abstract */
    "\002\001",
    /* Version */
    "\0632.8.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ListenerDestination =
{
    /* flags */
    META_FLAG_CLASS|META_FLAG_ABSTRACT,
    /* name */
    "CIM_ListenerDestination",
    /* qualifiers */
    __root_PG_InterOp_CIM_ListenerDestination_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_ManagedElement,
    /* features */
    _CIM_ListenerDestination_features,
};

//==============================================================================
//
// Class: CIM_ListenerDestinationCIMXML
//
//==============================================================================

static const char*
_CIM_ListenerDestinationCIMXML_Destination_qualifiers[] =
{
    /* Required */
    "\050\001",
    0,
};

static MetaProperty
_CIM_ListenerDestinationCIMXML_Destination =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "Destination",
    /* qualifiers */
    _CIM_ListenerDestinationCIMXML_Destination_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_ListenerDestinationCIMXML_features[] =
{
    (MetaFeature*)&_CIM_ListenerDestinationCIMXML_Destination,
    0,
};

static const char*
__root_PG_InterOp_CIM_ListenerDestinationCIMXML_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Event",
    /* Version */
    "\0632.8.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ListenerDestinationCIMXML =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "CIM_ListenerDestinationCIMXML",
    /* qualifiers */
    __root_PG_InterOp_CIM_ListenerDestinationCIMXML_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_ListenerDestination,
    /* features */
    _CIM_ListenerDestinationCIMXML_features,
};

//==============================================================================
//
// Class: CIM_ProcessIndication
//
//==============================================================================

static MetaFeature*
_CIM_ProcessIndication_features[] =
{
    0,
};

static const char*
__root_PG_InterOp_CIM_ProcessIndication_qualifiers[] =
{
    /* INDICATION */
    "\001\001",
    /* UMLPackagePath */
    "\057CIM::Event",
    /* Abstract */
    "\002\001",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ProcessIndication =
{
    /* flags */
    META_FLAG_INDICATION|META_FLAG_ABSTRACT,
    /* name */
    "CIM_ProcessIndication",
    /* qualifiers */
    __root_PG_InterOp_CIM_ProcessIndication_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_Indication,
    /* features */
    _CIM_ProcessIndication_features,
};

//==============================================================================
//
// Class: CIM_SNMPTrapIndication
//
//==============================================================================

static const char*
_CIM_SNMPTrapIndication_Enterprise_qualifiers[] =
{
    /* MappingStrings */
    "\026\000\001PDU.IETF|RFC1157-TRAP-PDU.enterprise\000",
    0,
};

static MetaProperty
_CIM_SNMPTrapIndication_Enterprise =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "Enterprise",
    /* qualifiers */
    _CIM_SNMPTrapIndication_Enterprise_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_SNMPTrapIndication_AgentAddress_qualifiers[] =
{
    /* MappingStrings */
    "\026\000\001PDU.IETF|RFC1157-TRAP-PDU.agent-addr\000",
    0,
};

static MetaProperty
_CIM_SNMPTrapIndication_AgentAddress =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "AgentAddress",
    /* qualifiers */
    _CIM_SNMPTrapIndication_AgentAddress_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_SNMPTrapIndication_GenericTrap_qualifiers[] =
{
    /* ValueMap */
    "\061\000\0070\0001\0002\0003\0004\0005\0006\000",
    /* Values */
    "\062\000\007Cold Start\000Warm Start\000Link Down\000Link Up\000Authentication Failure\000EGP Neighbor Loss\000Enterprise Specific\000",
    /* MappingStrings */
    "\026\000\001PDU.IETF|RFC1157-TRAP-PDU.generic-trap\000",
    0,
};

static MetaProperty
_CIM_SNMPTrapIndication_GenericTrap =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "GenericTrap",
    /* qualifiers */
    _CIM_SNMPTrapIndication_GenericTrap_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_SNMPTrapIndication_SpecificTrap_qualifiers[] =
{
    /* MappingStrings */
    "\026\000\001PDU.IETF|RFC1157-TRAP-PDU.specific-trap\000",
    0,
};

static MetaProperty
_CIM_SNMPTrapIndication_SpecificTrap =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "SpecificTrap",
    /* qualifiers */
    _CIM_SNMPTrapIndication_SpecificTrap_qualifiers,
    /* type */
    CIMTYPE_UINT32,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_SNMPTrapIndication_TimeStamp_qualifiers[] =
{
    /* MappingStrings */
    "\026\000\001PDU.IETF|RFC1157-TRAP-PDU.time-stamp\000",
    0,
};

static MetaProperty
_CIM_SNMPTrapIndication_TimeStamp =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "TimeStamp",
    /* qualifiers */
    _CIM_SNMPTrapIndication_TimeStamp_qualifiers,
    /* type */
    CIMTYPE_DATETIME,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_SNMPTrapIndication_VarBindNames_qualifiers[] =
{
    /* MappingStrings */
    "\026\000\001PDU.IETF|RFC1157-TRAP-PDU.variable-bindings\000",
    /* ArrayType */
    "\005Indexed",
    /* ModelCorrespondence */
    "\036\000\002CIM_SNMPTrapIndication.VarBindSyntaxes\000CIM_SNMPTrapIndication.VarBindValues\000",
    0,
};

static MetaProperty
_CIM_SNMPTrapIndication_VarBindNames =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "VarBindNames",
    /* qualifiers */
    _CIM_SNMPTrapIndication_VarBindNames_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* value */
    0,
};

static const char*
_CIM_SNMPTrapIndication_VarBindSyntaxes_qualifiers[] =
{
    /* ValueMap */
    "\061\000\0101\0002\0003\0004\0005\0006\0007\0008\000",
    /* Values */
    "\062\000\010Integer\000OctetString\000ObjectIdentifier\000NetworkAddress\000Counter\000Gauge\000TimeTicks\000Opaque\000",
    /* MappingStrings */
    "\026\000\001PDU.IETF|RFC1157-TRAP-PDU.variable-bindings\000",
    /* ArrayType */
    "\005Indexed",
    /* ModelCorrespondence */
    "\036\000\002CIM_SNMPTrapIndication.VarBindNames\000CIM_SNMPTrapIndication.VarBindValues\000",
    0,
};

static MetaProperty
_CIM_SNMPTrapIndication_VarBindSyntaxes =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "VarBindSyntaxes",
    /* qualifiers */
    _CIM_SNMPTrapIndication_VarBindSyntaxes_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    0,
    /* value */
    0,
};

static const char*
_CIM_SNMPTrapIndication_VarBindValues_qualifiers[] =
{
    /* OctetString */
    "\042\001",
    /* MappingStrings */
    "\026\000\001PDU.IETF|RFC1157-TRAP-PDU.variable-bindings\000",
    /* ArrayType */
    "\005Indexed",
    /* ModelCorrespondence */
    "\036\000\002CIM_SNMPTrapIndication.VarBindNames\000CIM_SNMPTrapIndication.VarBindSyntaxes\000",
    0,
};

static MetaProperty
_CIM_SNMPTrapIndication_VarBindValues =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_OCTETSTRING|META_FLAG_READ,
    /* name */
    "VarBindValues",
    /* qualifiers */
    _CIM_SNMPTrapIndication_VarBindValues_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* value */
    0,
};

static MetaFeature*
_CIM_SNMPTrapIndication_features[] =
{
    (MetaFeature*)&_CIM_SNMPTrapIndication_Enterprise,
    (MetaFeature*)&_CIM_SNMPTrapIndication_AgentAddress,
    (MetaFeature*)&_CIM_SNMPTrapIndication_GenericTrap,
    (MetaFeature*)&_CIM_SNMPTrapIndication_SpecificTrap,
    (MetaFeature*)&_CIM_SNMPTrapIndication_TimeStamp,
    (MetaFeature*)&_CIM_SNMPTrapIndication_VarBindNames,
    (MetaFeature*)&_CIM_SNMPTrapIndication_VarBindSyntaxes,
    (MetaFeature*)&_CIM_SNMPTrapIndication_VarBindValues,
    0,
};

static const char*
__root_PG_InterOp_CIM_SNMPTrapIndication_qualifiers[] =
{
    /* INDICATION */
    "\001\001",
    /* UMLPackagePath */
    "\057CIM::Event",
    /* Version */
    "\0632.7.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_SNMPTrapIndication =
{
    /* flags */
    META_FLAG_INDICATION,
    /* name */
    "CIM_SNMPTrapIndication",
    /* qualifiers */
    __root_PG_InterOp_CIM_SNMPTrapIndication_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_ProcessIndication,
    /* features */
    _CIM_SNMPTrapIndication_features,
};

//==============================================================================
//
// Class: CIM_AlertIndication
//
//==============================================================================

static const char*
_CIM_AlertIndication_Description_qualifiers[] =
{
    /* MappingStrings */
    "\026\000\001Recommendation.ITU|X733.Additional text\000",
    0,
};

static MetaProperty
_CIM_AlertIndication_Description =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "Description",
    /* qualifiers */
    _CIM_AlertIndication_Description_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_AlertIndication_AlertingManagedElement_qualifiers[] =
{
    /* ModelCorrespondence */
    "\036\000\001CIM_AlertIndication.AlertingElementFormat\000",
    0,
};

static MetaProperty
_CIM_AlertIndication_AlertingManagedElement =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "AlertingManagedElement",
    /* qualifiers */
    _CIM_AlertIndication_AlertingManagedElement_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_AlertIndication_AlertingElementFormat_qualifiers[] =
{
    /* ValueMap */
    "\061\000\0030\0001\0002\000",
    /* Values */
    "\062\000\003Unknown\000Other\000CIMObjectPath\000",
    /* ModelCorrespondence */
    "\036\000\002CIM_AlertIndication.AlertingManagedElement\000CIM_AlertIndication.OtherAlertingElementFormat\000",
    0,
};

static MetaProperty
_CIM_AlertIndication_AlertingElementFormat =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "AlertingElementFormat",
    /* qualifiers */
    _CIM_AlertIndication_AlertingElementFormat_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    "\000\000",
};

static const char*
_CIM_AlertIndication_OtherAlertingElementFormat_qualifiers[] =
{
    /* ModelCorrespondence */
    "\036\000\001CIM_AlertIndication.AlertingElementFormat\000",
    0,
};

static MetaProperty
_CIM_AlertIndication_OtherAlertingElementFormat =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "OtherAlertingElementFormat",
    /* qualifiers */
    _CIM_AlertIndication_OtherAlertingElementFormat_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_AlertIndication_AlertType_qualifiers[] =
{
    /* Required */
    "\050\001",
    /* ValueMap */
    "\061\000\0101\0002\0003\0004\0005\0006\0007\0008\000",
    /* Values */
    "\062\000\010Other\000Communications Alert\000Quality of Service Alert\000Processing Error\000Device Alert\000Environmental Alert\000Model Change\000Security Alert\000",
    /* MappingStrings */
    "\026\000\001Recommendation.ITU|X733.Event type\000",
    0,
};

static MetaProperty
_CIM_AlertIndication_AlertType =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "AlertType",
    /* qualifiers */
    _CIM_AlertIndication_AlertType_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_AlertIndication_OtherAlertType_qualifiers[] =
{
    /* ModelCorrespondence */
    "\036\000\001CIM_AlertIndication.AlertType\000",
    0,
};

static MetaProperty
_CIM_AlertIndication_OtherAlertType =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "OtherAlertType",
    /* qualifiers */
    _CIM_AlertIndication_OtherAlertType_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_AlertIndication_PerceivedSeverity_qualifiers[] =
{
    /* Required */
    "\050\001",
    /* Override */
    "\044PerceivedSeverity",
    /* ValueMap */
    "\061\000\0100\0001\0002\0003\0004\0005\0006\0007\000",
    /* Values */
    "\062\000\010Unknown\000Other\000Information\000Degraded/Warning\000Minor\000Major\000Critical\000Fatal/NonRecoverable\000",
    /* MappingStrings */
    "\026\000\001Recommendation.ITU|X733.Perceived severity\000",
    0,
};

static MetaProperty
_CIM_AlertIndication_PerceivedSeverity =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "PerceivedSeverity",
    /* qualifiers */
    _CIM_AlertIndication_PerceivedSeverity_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_AlertIndication_ProbableCause_qualifiers[] =
{
    /* Required */
    "\050\001",
    /* ValueMap */
    "\061\000\2030\0001\0002\0003\0004\0005\0006\0007\0008\0009\00010\00011\00012\00013\00014\00015\00016\00017\00018\00019\00020\00021\00022\00023\00024\00025\00026\00027\00028\00029\00030\00031\00032\00033\00034\00035\00036\00037\00038\00039\00040\00041\00042\00043\00044\00045\00046\00047\00048\00049\00050\00051\00052\00053\00054\00055\00056\00057\00058\00059\00060\00061\00062\00063\00064\00065\00066\00067\00068\00069\00070\00071\00072\00073\00074\00075\00076\00077\00078\00079\00080\00081\00082\00083\00084\00085\00086\00087\00088\00089\00090\00091\00092\00093\00094\00095\00096\00097\00098\00099\000100\000101\000102\000103\000104\000105\000106\000107\000108\000109\000110\000111\000112\000113\000114\000115\000116\000117\000118\000119\000120\000121\000122\000123\000124\000125\000126\000127\000128\000129\000130\000",
    /* Values */
    "\062\000\203Unknown\000Other\000Adapter/Card Error\000Application Subsystem Failure\000Bandwidth Reduced\000Connection Establishment Error\000Communications Protocol Error\000Communications Subsystem Failure\000Configuration/Customization Error\000Congestion\000Corrupt Data\000CPU Cycles Limit Exceeded\000Dataset/Modem Error\000Degraded Signal\000DTE-DCE Interface Error\000Enclosure Door Open\000Equipment Malfunction\000Excessive Vibration\000File Format Error\000Fire Detected\000Flood Detected\000Framing Error\000HVAC Problem\000Humidity Unacceptable\000I/O Device Error\000Input Device Error\000LAN Error\000Non-Toxic Leak Detected\000Local Node Transmission Error\000Loss of Frame\000Loss of Signal\000Material Supply Exhausted\000Multiplexer Problem\000Out of Memory\000Output Device Error\000Performance Degraded\000Power Problem\000Pressure Unacceptable\000Processor Problem (Internal Machine Error)\000Pump Failure\000Queue Size Exceeded\000Receive Failure\000Receiver Failure\000Remote Node Transmission Error\000Resource at or Nearing Capacity\000Response Time Excessive\000Retransmission Rate Excessive\000Software Error\000Software Program Abnormally Terminated\000Software Program Error (Incorrect Results)\000Storage Capacity Problem\000Temperature Unacceptable\000Threshold Crossed\000Timing Problem\000Toxic Leak Detected\000Transmit Failure\000Transmitter Failure\000Underlying Resource Unavailable\000Version MisMatch\000Previous Alert Cleared\000Login Attempts Failed\000Software Virus Detected\000Hardware Security Breached\000Denial of Service Detected\000Security Credential MisMatch\000Unauthorized Access\000Alarm Received\000Loss of Pointer\000Payload Mismatch\000Transmission Error\000Excessive Error Rate\000Trace Problem\000Element Unavailable\000Element Missing\000Loss of Multi Frame\000Broadcast Channel Failure\000Invalid Message Received\000Routing Failure\000Backplane Failure\000Identifier Duplication\000Protection Path Failure\000Sync Loss or Mismatch\000Terminal Problem\000Real Time Clock Failure\000Antenna Failure\000Battery Charging Failure\000Disk Failure\000Frequency Hopping Failure\000Loss of Redundancy\000Power Supply Failure\000Signal Quality Problem\000Battery Discharging\000Battery Failure\000Commercial Power Problem\000Fan Failure\000Engine Failure\000Sensor Failure\000Fuse Failure\000Generator Failure\000Low Battery\000Low Fuel\000Low Water\000Explosive Gas\000High Winds\000Ice Buildup\000Smoke\000Memory Mismatch\000Out of CPU Cycles\000Software Environment Problem\000Software Download Failure\000Element Reinitialized\000Timeout\000Logging Problems\000Leak Detected\000Protection Mechanism Failure\000Protecting Resource Failure\000Database Inconsistency\000Authentication Failure\000Breach of Confidentiality\000Cable Tamper\000Delayed Information\000Duplicate Information\000Information Missing\000Information Modification\000Information Out of Sequence\000Key Expired\000Non-Repudiation Failure\000Out of Hours Activity\000Out of Service\000Procedural Error\000Unexpected Information\000",
    /* MappingStrings */
    "\026\000\003Recommendation.ITU|X733.Probable cause\000Recommendation.ITU|M3100.probableCause\000ITU-IANA-ALARM-TC\000",
    /* ModelCorrespondence */
    "\036\000\003CIM_AlertIndication.ProbableCauseDescription\000CIM_AlertIndication.EventID\000CIM_AlertIndication.EventTime\000",
    0,
};

static MetaProperty
_CIM_AlertIndication_ProbableCause =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "ProbableCause",
    /* qualifiers */
    _CIM_AlertIndication_ProbableCause_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_AlertIndication_ProbableCauseDescription_qualifiers[] =
{
    /* ModelCorrespondence */
    "\036\000\001CIM_AlertIndication.ProbableCause\000",
    0,
};

static MetaProperty
_CIM_AlertIndication_ProbableCauseDescription =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "ProbableCauseDescription",
    /* qualifiers */
    _CIM_AlertIndication_ProbableCauseDescription_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_AlertIndication_Trending_qualifiers[] =
{
    /* ValueMap */
    "\061\000\0050\0001\0002\0003\0004\000",
    /* Values */
    "\062\000\005Unknown\000Not Applicable\000Trending Up\000Trending Down\000No Change\000",
    /* MappingStrings */
    "\026\000\001Recommendation.ITU|X733.TrendIndication\000",
    0,
};

static MetaProperty
_CIM_AlertIndication_Trending =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "Trending",
    /* qualifiers */
    _CIM_AlertIndication_Trending_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_AlertIndication_RecommendedActions_qualifiers[] =
{
    /* MappingStrings */
    "\026\000\001Recommendation.ITU|X733.Proposed repair actions\000",
    0,
};

static MetaProperty
_CIM_AlertIndication_RecommendedActions =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "RecommendedActions",
    /* qualifiers */
    _CIM_AlertIndication_RecommendedActions_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* value */
    0,
};

static const char*
_CIM_AlertIndication_EventID_qualifiers[] =
{
    /* ModelCorrespondence */
    "\036\000\001CIM_AlertIndication.ProbableCause\000",
    0,
};

static MetaProperty
_CIM_AlertIndication_EventID =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "EventID",
    /* qualifiers */
    _CIM_AlertIndication_EventID_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_AlertIndication_EventTime_qualifiers[] =
{
    /* ModelCorrespondence */
    "\036\000\001CIM_AlertIndication.ProbableCause\000",
    0,
};

static MetaProperty
_CIM_AlertIndication_EventTime =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "EventTime",
    /* qualifiers */
    _CIM_AlertIndication_EventTime_qualifiers,
    /* type */
    CIMTYPE_DATETIME,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_AlertIndication_SystemCreationClassName_qualifiers[] =
{
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_AlertIndication_SystemCreationClassName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "SystemCreationClassName",
    /* qualifiers */
    _CIM_AlertIndication_SystemCreationClassName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_AlertIndication_SystemName_qualifiers[] =
{
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_AlertIndication_SystemName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "SystemName",
    /* qualifiers */
    _CIM_AlertIndication_SystemName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_AlertIndication_ProviderName_qualifiers[] =
{
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_AlertIndication_ProviderName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "ProviderName",
    /* qualifiers */
    _CIM_AlertIndication_ProviderName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_AlertIndication_features[] =
{
    (MetaFeature*)&_CIM_AlertIndication_Description,
    (MetaFeature*)&_CIM_AlertIndication_AlertingManagedElement,
    (MetaFeature*)&_CIM_AlertIndication_AlertingElementFormat,
    (MetaFeature*)&_CIM_AlertIndication_OtherAlertingElementFormat,
    (MetaFeature*)&_CIM_AlertIndication_AlertType,
    (MetaFeature*)&_CIM_AlertIndication_OtherAlertType,
    (MetaFeature*)&_CIM_AlertIndication_PerceivedSeverity,
    (MetaFeature*)&_CIM_AlertIndication_ProbableCause,
    (MetaFeature*)&_CIM_AlertIndication_ProbableCauseDescription,
    (MetaFeature*)&_CIM_AlertIndication_Trending,
    (MetaFeature*)&_CIM_AlertIndication_RecommendedActions,
    (MetaFeature*)&_CIM_AlertIndication_EventID,
    (MetaFeature*)&_CIM_AlertIndication_EventTime,
    (MetaFeature*)&_CIM_AlertIndication_SystemCreationClassName,
    (MetaFeature*)&_CIM_AlertIndication_SystemName,
    (MetaFeature*)&_CIM_AlertIndication_ProviderName,
    0,
};

static const char*
__root_PG_InterOp_CIM_AlertIndication_qualifiers[] =
{
    /* INDICATION */
    "\001\001",
    /* UMLPackagePath */
    "\057CIM::Event",
    /* Version */
    "\0632.10.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_AlertIndication =
{
    /* flags */
    META_FLAG_INDICATION,
    /* name */
    "CIM_AlertIndication",
    /* qualifiers */
    __root_PG_InterOp_CIM_AlertIndication_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_ProcessIndication,
    /* features */
    _CIM_AlertIndication_features,
};

//==============================================================================
//
// Class: CIM_AlertInstIndication
//
//==============================================================================

static const char*
_CIM_AlertInstIndication_AlertType_qualifiers[] =
{
    /* Override */
    "\044AlertType",
    0,
};

static MetaProperty
_CIM_AlertInstIndication_AlertType =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "AlertType",
    /* qualifiers */
    _CIM_AlertInstIndication_AlertType_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    "\000\007",
};

static const char*
_CIM_AlertInstIndication_IndObject_qualifiers[] =
{
    /* Required */
    "\050\001",
    /* EmbeddedObject */
    "\020\001",
    0,
};

static MetaProperty
_CIM_AlertInstIndication_IndObject =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_EMBEDDEDOBJECT|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "IndObject",
    /* qualifiers */
    _CIM_AlertInstIndication_IndObject_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_AlertInstIndication_features[] =
{
    (MetaFeature*)&_CIM_AlertInstIndication_AlertType,
    (MetaFeature*)&_CIM_AlertInstIndication_IndObject,
    0,
};

static const char*
__root_PG_InterOp_CIM_AlertInstIndication_qualifiers[] =
{
    /* INDICATION */
    "\001\001",
    /* UMLPackagePath */
    "\057CIM::Event",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_AlertInstIndication =
{
    /* flags */
    META_FLAG_INDICATION,
    /* name */
    "CIM_AlertInstIndication",
    /* qualifiers */
    __root_PG_InterOp_CIM_AlertInstIndication_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_AlertIndication,
    /* features */
    _CIM_AlertInstIndication_features,
};

//==============================================================================
//
// Class: CIM_ClassCreation
//
//==============================================================================

static MetaFeature*
_CIM_ClassCreation_features[] =
{
    0,
};

static const char*
__root_PG_InterOp_CIM_ClassCreation_qualifiers[] =
{
    /* INDICATION */
    "\001\001",
    /* UMLPackagePath */
    "\057CIM::Event",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ClassCreation =
{
    /* flags */
    META_FLAG_INDICATION,
    /* name */
    "CIM_ClassCreation",
    /* qualifiers */
    __root_PG_InterOp_CIM_ClassCreation_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_ClassIndication,
    /* features */
    _CIM_ClassCreation_features,
};

//==============================================================================
//
// Class: CIM_ClassDeletion
//
//==============================================================================

static MetaFeature*
_CIM_ClassDeletion_features[] =
{
    0,
};

static const char*
__root_PG_InterOp_CIM_ClassDeletion_qualifiers[] =
{
    /* INDICATION */
    "\001\001",
    /* UMLPackagePath */
    "\057CIM::Event",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ClassDeletion =
{
    /* flags */
    META_FLAG_INDICATION,
    /* name */
    "CIM_ClassDeletion",
    /* qualifiers */
    __root_PG_InterOp_CIM_ClassDeletion_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_ClassIndication,
    /* features */
    _CIM_ClassDeletion_features,
};

//==============================================================================
//
// Class: CIM_IndicationHandler
//
//==============================================================================

static const char*
_CIM_IndicationHandler_Owner_qualifiers[] =
{
    /* Deprecated */
    "\013\000\001No value\000",
    0,
};

static MetaProperty
_CIM_IndicationHandler_Owner =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "Owner",
    /* qualifiers */
    _CIM_IndicationHandler_Owner_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_IndicationHandler_features[] =
{
    (MetaFeature*)&_CIM_IndicationHandler_Owner,
    0,
};

static const char*
__root_PG_InterOp_CIM_IndicationHandler_qualifiers[] =
{
    /* Deprecated */
    "\013\000\001CIM_ListenerDestination\000",
    /* UMLPackagePath */
    "\057CIM::Event",
    /* Abstract */
    "\002\001",
    /* Version */
    "\0632.8.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_IndicationHandler =
{
    /* flags */
    META_FLAG_CLASS|META_FLAG_ABSTRACT,
    /* name */
    "CIM_IndicationHandler",
    /* qualifiers */
    __root_PG_InterOp_CIM_IndicationHandler_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_ListenerDestination,
    /* features */
    _CIM_IndicationHandler_features,
};

//==============================================================================
//
// Class: CIM_IndicationHandlerCIMXML
//
//==============================================================================

static const char*
_CIM_IndicationHandlerCIMXML_Destination_qualifiers[] =
{
    /* Deprecated */
    "\013\000\001CIM_ListenerDestinationCIMXML.Destination\000",
    /* Required */
    "\050\001",
    0,
};

static MetaProperty
_CIM_IndicationHandlerCIMXML_Destination =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "Destination",
    /* qualifiers */
    _CIM_IndicationHandlerCIMXML_Destination_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_IndicationHandlerCIMXML_features[] =
{
    (MetaFeature*)&_CIM_IndicationHandlerCIMXML_Destination,
    0,
};

static const char*
__root_PG_InterOp_CIM_IndicationHandlerCIMXML_qualifiers[] =
{
    /* Deprecated */
    "\013\000\001CIM_ListenerDestinationCIMXML\000",
    /* UMLPackagePath */
    "\057CIM::Event",
    /* Version */
    "\0632.8.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_IndicationHandlerCIMXML =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "CIM_IndicationHandlerCIMXML",
    /* qualifiers */
    __root_PG_InterOp_CIM_IndicationHandlerCIMXML_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_IndicationHandler,
    /* features */
    _CIM_IndicationHandlerCIMXML_features,
};

//==============================================================================
//
// Class: CIM_IndicationSubscription
//
//==============================================================================

static const char*
_CIM_IndicationSubscription_Filter_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_IndicationSubscription_Filter =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Filter",
    /* qualifiers */
    _CIM_IndicationSubscription_Filter_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_IndicationFilter,
};

static const char*
_CIM_IndicationSubscription_Handler_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_IndicationSubscription_Handler =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Handler",
    /* qualifiers */
    _CIM_IndicationSubscription_Handler_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ListenerDestination,
};

static const char*
_CIM_IndicationSubscription_OnFatalErrorPolicy_qualifiers[] =
{
    /* ValueMap */
    "\061\000\0041\0002\0003\0004\000",
    /* Values */
    "\062\000\004Other\000Ignore\000Disable\000Remove\000",
    0,
};

static MetaProperty
_CIM_IndicationSubscription_OnFatalErrorPolicy =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "OnFatalErrorPolicy",
    /* qualifiers */
    _CIM_IndicationSubscription_OnFatalErrorPolicy_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_IndicationSubscription_OtherOnFatalErrorPolicy_qualifiers[] =
{
    0,
};

static MetaProperty
_CIM_IndicationSubscription_OtherOnFatalErrorPolicy =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "OtherOnFatalErrorPolicy",
    /* qualifiers */
    _CIM_IndicationSubscription_OtherOnFatalErrorPolicy_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_IndicationSubscription_FailureTriggerTimeInterval_qualifiers[] =
{
    /* Units */
    "\060Seconds",
    0,
};

static MetaProperty
_CIM_IndicationSubscription_FailureTriggerTimeInterval =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "FailureTriggerTimeInterval",
    /* qualifiers */
    _CIM_IndicationSubscription_FailureTriggerTimeInterval_qualifiers,
    /* type */
    CIMTYPE_UINT64,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_IndicationSubscription_SubscriptionState_qualifiers[] =
{
    /* Write */
    "\065\001",
    /* ValueMap */
    "\061\000\0050\0001\0002\0003\0004\000",
    /* Values */
    "\062\000\005Unknown\000Other\000Enabled\000Enabled Degraded\000Disabled\000",
    0,
};

static MetaProperty
_CIM_IndicationSubscription_SubscriptionState =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_WRITE,
    /* name */
    "SubscriptionState",
    /* qualifiers */
    _CIM_IndicationSubscription_SubscriptionState_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_IndicationSubscription_OtherSubscriptionState_qualifiers[] =
{
    0,
};

static MetaProperty
_CIM_IndicationSubscription_OtherSubscriptionState =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "OtherSubscriptionState",
    /* qualifiers */
    _CIM_IndicationSubscription_OtherSubscriptionState_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_IndicationSubscription_TimeOfLastStateChange_qualifiers[] =
{
    0,
};

static MetaProperty
_CIM_IndicationSubscription_TimeOfLastStateChange =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "TimeOfLastStateChange",
    /* qualifiers */
    _CIM_IndicationSubscription_TimeOfLastStateChange_qualifiers,
    /* type */
    CIMTYPE_DATETIME,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_IndicationSubscription_SubscriptionDuration_qualifiers[] =
{
    /* Write */
    "\065\001",
    /* Units */
    "\060Seconds",
    0,
};

static MetaProperty
_CIM_IndicationSubscription_SubscriptionDuration =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_WRITE,
    /* name */
    "SubscriptionDuration",
    /* qualifiers */
    _CIM_IndicationSubscription_SubscriptionDuration_qualifiers,
    /* type */
    CIMTYPE_UINT64,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_IndicationSubscription_SubscriptionStartTime_qualifiers[] =
{
    0,
};

static MetaProperty
_CIM_IndicationSubscription_SubscriptionStartTime =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "SubscriptionStartTime",
    /* qualifiers */
    _CIM_IndicationSubscription_SubscriptionStartTime_qualifiers,
    /* type */
    CIMTYPE_DATETIME,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_IndicationSubscription_SubscriptionTimeRemaining_qualifiers[] =
{
    /* Units */
    "\060Seconds",
    0,
};

static MetaProperty
_CIM_IndicationSubscription_SubscriptionTimeRemaining =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "SubscriptionTimeRemaining",
    /* qualifiers */
    _CIM_IndicationSubscription_SubscriptionTimeRemaining_qualifiers,
    /* type */
    CIMTYPE_UINT64,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_IndicationSubscription_RepeatNotificationPolicy_qualifiers[] =
{
    /* ValueMap */
    "\061\000\0050\0001\0002\0003\0004\000",
    /* Values */
    "\062\000\005Unknown\000Other\000None\000Suppress\000Delay\000",
    0,
};

static MetaProperty
_CIM_IndicationSubscription_RepeatNotificationPolicy =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "RepeatNotificationPolicy",
    /* qualifiers */
    _CIM_IndicationSubscription_RepeatNotificationPolicy_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_IndicationSubscription_OtherRepeatNotificationPolicy_qualifiers[] =
{
    0,
};

static MetaProperty
_CIM_IndicationSubscription_OtherRepeatNotificationPolicy =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "OtherRepeatNotificationPolicy",
    /* qualifiers */
    _CIM_IndicationSubscription_OtherRepeatNotificationPolicy_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_IndicationSubscription_RepeatNotificationInterval_qualifiers[] =
{
    /* Units */
    "\060Seconds",
    0,
};

static MetaProperty
_CIM_IndicationSubscription_RepeatNotificationInterval =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "RepeatNotificationInterval",
    /* qualifiers */
    _CIM_IndicationSubscription_RepeatNotificationInterval_qualifiers,
    /* type */
    CIMTYPE_UINT64,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_IndicationSubscription_RepeatNotificationGap_qualifiers[] =
{
    /* Units */
    "\060Seconds",
    0,
};

static MetaProperty
_CIM_IndicationSubscription_RepeatNotificationGap =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "RepeatNotificationGap",
    /* qualifiers */
    _CIM_IndicationSubscription_RepeatNotificationGap_qualifiers,
    /* type */
    CIMTYPE_UINT64,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_IndicationSubscription_RepeatNotificationCount_qualifiers[] =
{
    0,
};

static MetaProperty
_CIM_IndicationSubscription_RepeatNotificationCount =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "RepeatNotificationCount",
    /* qualifiers */
    _CIM_IndicationSubscription_RepeatNotificationCount_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_IndicationSubscription_features[] =
{
    (MetaFeature*)&_CIM_IndicationSubscription_Filter,
    (MetaFeature*)&_CIM_IndicationSubscription_Handler,
    (MetaFeature*)&_CIM_IndicationSubscription_OnFatalErrorPolicy,
    (MetaFeature*)&_CIM_IndicationSubscription_OtherOnFatalErrorPolicy,
    (MetaFeature*)&_CIM_IndicationSubscription_FailureTriggerTimeInterval,
    (MetaFeature*)&_CIM_IndicationSubscription_SubscriptionState,
    (MetaFeature*)&_CIM_IndicationSubscription_OtherSubscriptionState,
    (MetaFeature*)&_CIM_IndicationSubscription_TimeOfLastStateChange,
    (MetaFeature*)&_CIM_IndicationSubscription_SubscriptionDuration,
    (MetaFeature*)&_CIM_IndicationSubscription_SubscriptionStartTime,
    (MetaFeature*)&_CIM_IndicationSubscription_SubscriptionTimeRemaining,
    (MetaFeature*)&_CIM_IndicationSubscription_RepeatNotificationPolicy,
    (MetaFeature*)&_CIM_IndicationSubscription_OtherRepeatNotificationPolicy,
    (MetaFeature*)&_CIM_IndicationSubscription_RepeatNotificationInterval,
    (MetaFeature*)&_CIM_IndicationSubscription_RepeatNotificationGap,
    (MetaFeature*)&_CIM_IndicationSubscription_RepeatNotificationCount,
    0,
};

static const char*
__root_PG_InterOp_CIM_IndicationSubscription_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Event",
    /* Version */
    "\0632.7.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_IndicationSubscription =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_IndicationSubscription",
    /* qualifiers */
    __root_PG_InterOp_CIM_IndicationSubscription_qualifiers,
    /* super */
    0,
    /* features */
    _CIM_IndicationSubscription_features,
};

//==============================================================================
//
// Class: CIM_InstCreation
//
//==============================================================================

static MetaFeature*
_CIM_InstCreation_features[] =
{
    0,
};

static const char*
__root_PG_InterOp_CIM_InstCreation_qualifiers[] =
{
    /* INDICATION */
    "\001\001",
    /* UMLPackagePath */
    "\057CIM::Event",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_InstCreation =
{
    /* flags */
    META_FLAG_INDICATION,
    /* name */
    "CIM_InstCreation",
    /* qualifiers */
    __root_PG_InterOp_CIM_InstCreation_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_InstIndication,
    /* features */
    _CIM_InstCreation_features,
};

//==============================================================================
//
// Class: CIM_InstDeletion
//
//==============================================================================

static MetaFeature*
_CIM_InstDeletion_features[] =
{
    0,
};

static const char*
__root_PG_InterOp_CIM_InstDeletion_qualifiers[] =
{
    /* INDICATION */
    "\001\001",
    /* UMLPackagePath */
    "\057CIM::Event",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_InstDeletion =
{
    /* flags */
    META_FLAG_INDICATION,
    /* name */
    "CIM_InstDeletion",
    /* qualifiers */
    __root_PG_InterOp_CIM_InstDeletion_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_InstIndication,
    /* features */
    _CIM_InstDeletion_features,
};

//==============================================================================
//
// Class: CIM_ThresholdIndication
//
//==============================================================================

static const char*
_CIM_ThresholdIndication_ThresholdIdentifier_qualifiers[] =
{
    /* MappingStrings */
    "\026\000\001Recommendation.ITU|X733.Threshold information\000",
    0,
};

static MetaProperty
_CIM_ThresholdIndication_ThresholdIdentifier =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "ThresholdIdentifier",
    /* qualifiers */
    _CIM_ThresholdIndication_ThresholdIdentifier_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_ThresholdIndication_ThresholdValue_qualifiers[] =
{
    /* MappingStrings */
    "\026\000\001Recommendation.ITU|X733.Threshold information\000",
    0,
};

static MetaProperty
_CIM_ThresholdIndication_ThresholdValue =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "ThresholdValue",
    /* qualifiers */
    _CIM_ThresholdIndication_ThresholdValue_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_ThresholdIndication_ObservedValue_qualifiers[] =
{
    /* MappingStrings */
    "\026\000\001Recommendation.ITU|X733.Threshold information\000",
    0,
};

static MetaProperty
_CIM_ThresholdIndication_ObservedValue =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "ObservedValue",
    /* qualifiers */
    _CIM_ThresholdIndication_ObservedValue_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_ThresholdIndication_features[] =
{
    (MetaFeature*)&_CIM_ThresholdIndication_ThresholdIdentifier,
    (MetaFeature*)&_CIM_ThresholdIndication_ThresholdValue,
    (MetaFeature*)&_CIM_ThresholdIndication_ObservedValue,
    0,
};

static const char*
__root_PG_InterOp_CIM_ThresholdIndication_qualifiers[] =
{
    /* INDICATION */
    "\001\001",
    /* UMLPackagePath */
    "\057CIM::Event",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ThresholdIndication =
{
    /* flags */
    META_FLAG_INDICATION,
    /* name */
    "CIM_ThresholdIndication",
    /* qualifiers */
    __root_PG_InterOp_CIM_ThresholdIndication_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_AlertIndication,
    /* features */
    _CIM_ThresholdIndication_features,
};

//==============================================================================
//
// Class: CIM_FormattedIndicationSubscription
//
//==============================================================================

static const char*
_CIM_FormattedIndicationSubscription_TextFormatOwningEntity_qualifiers[] =
{
    /* ModelCorrespondence */
    "\036\000\003CIM_FormattedIndicationSubscription.TextFormatID\000CIM_FormattedIndicationSubscription.TextFormat\000CIM_FormattedIndicationSubscription.TextFormatParameters\000",
    0,
};

static MetaProperty
_CIM_FormattedIndicationSubscription_TextFormatOwningEntity =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "TextFormatOwningEntity",
    /* qualifiers */
    _CIM_FormattedIndicationSubscription_TextFormatOwningEntity_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_FormattedIndicationSubscription_TextFormatID_qualifiers[] =
{
    /* ModelCorrespondence */
    "\036\000\003CIM_FormattedIndicationSubscription.TextFormatOwningEntity\000CIM_FormattedIndicationSubscription.TextFormat\000CIM_FormattedIndicationSubscription.TextFormatParameters\000",
    0,
};

static MetaProperty
_CIM_FormattedIndicationSubscription_TextFormatID =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "TextFormatID",
    /* qualifiers */
    _CIM_FormattedIndicationSubscription_TextFormatID_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_FormattedIndicationSubscription_TextFormat_qualifiers[] =
{
    0,
};

static MetaProperty
_CIM_FormattedIndicationSubscription_TextFormat =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "TextFormat",
    /* qualifiers */
    _CIM_FormattedIndicationSubscription_TextFormat_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_FormattedIndicationSubscription_TextFormatParameters_qualifiers[] =
{
    /* ArrayType */
    "\005Indexed",
    /* ModelCorrespondence */
    "\036\000\003CIM_FormattedIndicationSubscription.TextFormatOwningEntity\000CIM_FormattedIndicationSubscription.TextFormatID\000CIM_FormattedIndicationSubscription.TextFormat\000",
    0,
};

static MetaProperty
_CIM_FormattedIndicationSubscription_TextFormatParameters =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "TextFormatParameters",
    /* qualifiers */
    _CIM_FormattedIndicationSubscription_TextFormatParameters_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* value */
    0,
};

static MetaFeature*
_CIM_FormattedIndicationSubscription_features[] =
{
    (MetaFeature*)&_CIM_FormattedIndicationSubscription_TextFormatOwningEntity,
    (MetaFeature*)&_CIM_FormattedIndicationSubscription_TextFormatID,
    (MetaFeature*)&_CIM_FormattedIndicationSubscription_TextFormat,
    (MetaFeature*)&_CIM_FormattedIndicationSubscription_TextFormatParameters,
    0,
};

static const char*
__root_PG_InterOp_CIM_FormattedIndicationSubscription_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Event",
    /* Version */
    "\0632.10.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_FormattedIndicationSubscription =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_FormattedIndicationSubscription",
    /* qualifiers */
    __root_PG_InterOp_CIM_FormattedIndicationSubscription_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_IndicationSubscription,
    /* features */
    _CIM_FormattedIndicationSubscription_features,
};

//==============================================================================
//
// Class: CIM_Namespace
//
//==============================================================================

static const char*
_CIM_Namespace_SystemCreationClassName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Propagated */
    "\045CIM_ObjectManager.SystemCreationClassName",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_Namespace_SystemCreationClassName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "SystemCreationClassName",
    /* qualifiers */
    _CIM_Namespace_SystemCreationClassName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Namespace_SystemName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Propagated */
    "\045CIM_ObjectManager.SystemName",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_Namespace_SystemName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "SystemName",
    /* qualifiers */
    _CIM_Namespace_SystemName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Namespace_ObjectManagerCreationClassName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Propagated */
    "\045CIM_ObjectManager.CreationClassName",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_Namespace_ObjectManagerCreationClassName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "ObjectManagerCreationClassName",
    /* qualifiers */
    _CIM_Namespace_ObjectManagerCreationClassName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Namespace_ObjectManagerName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Propagated */
    "\045CIM_ObjectManager.Name",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_Namespace_ObjectManagerName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "ObjectManagerName",
    /* qualifiers */
    _CIM_Namespace_ObjectManagerName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Namespace_CreationClassName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_Namespace_CreationClassName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "CreationClassName",
    /* qualifiers */
    _CIM_Namespace_CreationClassName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Namespace_Name_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_Namespace_Name =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Name",
    /* qualifiers */
    _CIM_Namespace_Name_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Namespace_ClassInfo_qualifiers[] =
{
    /* Deprecated */
    "\013\000\001CIM_Namespace.ClassType\000",
    /* Required */
    "\050\001",
    /* Write */
    "\065\001",
    /* ValueMap */
    "\061\000\0170\0001\0002\0003\0004\0005\0006\0007\0008\0009\00010\00011\000200\000201\000202\000",
    /* Values */
    "\062\000\017Unknown\000Other\000CIM 1.0\000CIM 2.0\000CIM 2.1\000CIM 2.2\000CIM 2.3\000CIM 2.4\000CIM 2.5\000CIM 2.6\000CIM 2.7\000CIM 2.8\000DMI Recast\000SNMP Recast\000CMIP Recast\000",
    /* ModelCorrespondence */
    "\036\000\001CIM_Namespace.DescriptionOfClassInfo\000",
    0,
};

static MetaProperty
_CIM_Namespace_ClassInfo =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED|META_FLAG_WRITE,
    /* name */
    "ClassInfo",
    /* qualifiers */
    _CIM_Namespace_ClassInfo_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Namespace_DescriptionOfClassInfo_qualifiers[] =
{
    /* Deprecated */
    "\013\000\001CIM_Namespace.DescriptionOfClassType\000",
    /* Write */
    "\065\001",
    /* ModelCorrespondence */
    "\036\000\001CIM_Namespace.ClassInfo\000",
    0,
};

static MetaProperty
_CIM_Namespace_DescriptionOfClassInfo =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_WRITE,
    /* name */
    "DescriptionOfClassInfo",
    /* qualifiers */
    _CIM_Namespace_DescriptionOfClassInfo_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Namespace_ClassType_qualifiers[] =
{
    /* Write */
    "\065\001",
    /* ValueMap */
    "\061\000\0060\0001\0002\000200\000201\000202\000",
    /* Values */
    "\062\000\006Unknown\000Other\000CIM\000DMI Recast\000SNMP Recast\000CMIP Recast\000",
    /* ModelCorrespondence */
    "\036\000\002CIM_Namespace.DescriptionOfClassType\000CIM_Namespace.ClassTypeVersion\000",
    0,
};

static MetaProperty
_CIM_Namespace_ClassType =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_WRITE,
    /* name */
    "ClassType",
    /* qualifiers */
    _CIM_Namespace_ClassType_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Namespace_ClassTypeVersion_qualifiers[] =
{
    /* Write */
    "\065\001",
    /* ModelCorrespondence */
    "\036\000\002CIM_Namespace.DescriptionOfClassType\000CIM_Namespace.ClassType\000",
    0,
};

static MetaProperty
_CIM_Namespace_ClassTypeVersion =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_WRITE,
    /* name */
    "ClassTypeVersion",
    /* qualifiers */
    _CIM_Namespace_ClassTypeVersion_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_Namespace_DescriptionOfClassType_qualifiers[] =
{
    /* Write */
    "\065\001",
    /* ModelCorrespondence */
    "\036\000\002CIM_Namespace.ClassType\000CIM_Namespace.ClassTypeVersion\000",
    0,
};

static MetaProperty
_CIM_Namespace_DescriptionOfClassType =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_WRITE,
    /* name */
    "DescriptionOfClassType",
    /* qualifiers */
    _CIM_Namespace_DescriptionOfClassType_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_Namespace_features[] =
{
    (MetaFeature*)&_CIM_Namespace_SystemCreationClassName,
    (MetaFeature*)&_CIM_Namespace_SystemName,
    (MetaFeature*)&_CIM_Namespace_ObjectManagerCreationClassName,
    (MetaFeature*)&_CIM_Namespace_ObjectManagerName,
    (MetaFeature*)&_CIM_Namespace_CreationClassName,
    (MetaFeature*)&_CIM_Namespace_Name,
    (MetaFeature*)&_CIM_Namespace_ClassInfo,
    (MetaFeature*)&_CIM_Namespace_DescriptionOfClassInfo,
    (MetaFeature*)&_CIM_Namespace_ClassType,
    (MetaFeature*)&_CIM_Namespace_ClassTypeVersion,
    (MetaFeature*)&_CIM_Namespace_DescriptionOfClassType,
    0,
};

static const char*
__root_PG_InterOp_CIM_Namespace_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Interop",
    /* Version */
    "\0632.8.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_Namespace =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "CIM_Namespace",
    /* qualifiers */
    __root_PG_InterOp_CIM_Namespace_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_ManagedElement,
    /* features */
    _CIM_Namespace_features,
};

//==============================================================================
//
// Class: CIM_RegisteredProfile
//
//==============================================================================

static const char*
_CIM_RegisteredProfile_InstanceID_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaProperty
_CIM_RegisteredProfile_InstanceID =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "InstanceID",
    /* qualifiers */
    _CIM_RegisteredProfile_InstanceID_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_RegisteredProfile_RegisteredOrganization_qualifiers[] =
{
    /* Required */
    "\050\001",
    /* ValueMap */
    "\061\000\0231\0002\0003\0004\0005\0006\0007\0008\0009\00010\00011\00012\00013\00014\00015\00016\00017\00018\00019\000",
    /* Values */
    "\062\000\023Other\000DMTF\000CompTIA\000Consortium for Service Innovation\000FAST\000GGF\000INTAP\000itSMF\000NAC\000Northwest Energy Efficiency Alliance\000SNIA\000TM Forum\000The Open Group\000ANSI\000IEEE\000IETF\000INCITS\000ISO\000W3C\000",
    /* ModelCorrespondence */
    "\036\000\001CIM_RegisteredProfile.OtherRegisteredOrganization\000",
    0,
};

static MetaProperty
_CIM_RegisteredProfile_RegisteredOrganization =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "RegisteredOrganization",
    /* qualifiers */
    _CIM_RegisteredProfile_RegisteredOrganization_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_RegisteredProfile_OtherRegisteredOrganization_qualifiers[] =
{
    /* MaxLen */
    "\030\000\000\001\000",
    /* ModelCorrespondence */
    "\036\000\001CIM_RegisteredProfile.RegisteredOrganization\000",
    0,
};

static MetaProperty
_CIM_RegisteredProfile_OtherRegisteredOrganization =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "OtherRegisteredOrganization",
    /* qualifiers */
    _CIM_RegisteredProfile_OtherRegisteredOrganization_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_RegisteredProfile_RegisteredName_qualifiers[] =
{
    /* Required */
    "\050\001",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_RegisteredProfile_RegisteredName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "RegisteredName",
    /* qualifiers */
    _CIM_RegisteredProfile_RegisteredName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_RegisteredProfile_RegisteredVersion_qualifiers[] =
{
    /* Required */
    "\050\001",
    0,
};

static MetaProperty
_CIM_RegisteredProfile_RegisteredVersion =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "RegisteredVersion",
    /* qualifiers */
    _CIM_RegisteredProfile_RegisteredVersion_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_RegisteredProfile_AdvertiseTypes_qualifiers[] =
{
    /* Required */
    "\050\001",
    /* ValueMap */
    "\061\000\0031\0002\0003\000",
    /* Values */
    "\062\000\003Other\000Not Advertised\000SLP\000",
    /* ArrayType */
    "\005Indexed",
    /* ModelCorrespondence */
    "\036\000\001CIM_RegisteredProfile.AdvertiseTypeDescriptions\000",
    0,
};

static MetaProperty
_CIM_RegisteredProfile_AdvertiseTypes =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "AdvertiseTypes",
    /* qualifiers */
    _CIM_RegisteredProfile_AdvertiseTypes_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    0,
    /* value */
    0,
};

static const char*
_CIM_RegisteredProfile_AdvertiseTypeDescriptions_qualifiers[] =
{
    /* ArrayType */
    "\005Indexed",
    /* ModelCorrespondence */
    "\036\000\001CIM_RegisteredProfile.AdvertiseTypes\000",
    0,
};

static MetaProperty
_CIM_RegisteredProfile_AdvertiseTypeDescriptions =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "AdvertiseTypeDescriptions",
    /* qualifiers */
    _CIM_RegisteredProfile_AdvertiseTypeDescriptions_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* value */
    0,
};

static MetaFeature*
_CIM_RegisteredProfile_features[] =
{
    (MetaFeature*)&_CIM_RegisteredProfile_InstanceID,
    (MetaFeature*)&_CIM_RegisteredProfile_RegisteredOrganization,
    (MetaFeature*)&_CIM_RegisteredProfile_OtherRegisteredOrganization,
    (MetaFeature*)&_CIM_RegisteredProfile_RegisteredName,
    (MetaFeature*)&_CIM_RegisteredProfile_RegisteredVersion,
    (MetaFeature*)&_CIM_RegisteredProfile_AdvertiseTypes,
    (MetaFeature*)&_CIM_RegisteredProfile_AdvertiseTypeDescriptions,
    0,
};

static const char*
__root_PG_InterOp_CIM_RegisteredProfile_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Interop",
    /* Version */
    "\0632.8.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_RegisteredProfile =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "CIM_RegisteredProfile",
    /* qualifiers */
    __root_PG_InterOp_CIM_RegisteredProfile_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_ManagedElement,
    /* features */
    _CIM_RegisteredProfile_features,
};

//==============================================================================
//
// Class: CIM_RegisteredSubProfile
//
//==============================================================================

static MetaFeature*
_CIM_RegisteredSubProfile_features[] =
{
    0,
};

static const char*
__root_PG_InterOp_CIM_RegisteredSubProfile_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Interop",
    /* Version */
    "\0632.8.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_RegisteredSubProfile =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "CIM_RegisteredSubProfile",
    /* qualifiers */
    __root_PG_InterOp_CIM_RegisteredSubProfile_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_RegisteredProfile,
    /* features */
    _CIM_RegisteredSubProfile_features,
};

//==============================================================================
//
// Class: CIM_SystemIdentification
//
//==============================================================================

static const char*
_CIM_SystemIdentification_SystemCreationClassName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Propagated */
    "\045CIM_Namespace.SystemCreationClassName",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_SystemIdentification_SystemCreationClassName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "SystemCreationClassName",
    /* qualifiers */
    _CIM_SystemIdentification_SystemCreationClassName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_SystemIdentification_SystemName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Propagated */
    "\045CIM_Namespace.SystemName",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_SystemIdentification_SystemName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "SystemName",
    /* qualifiers */
    _CIM_SystemIdentification_SystemName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_SystemIdentification_ObjectManagerCreationClassName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Propagated */
    "\045CIM_Namespace.ObjectManagerCreationClassName",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_SystemIdentification_ObjectManagerCreationClassName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "ObjectManagerCreationClassName",
    /* qualifiers */
    _CIM_SystemIdentification_ObjectManagerCreationClassName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_SystemIdentification_ObjectManagerName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Propagated */
    "\045CIM_Namespace.ObjectManagerName",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_SystemIdentification_ObjectManagerName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "ObjectManagerName",
    /* qualifiers */
    _CIM_SystemIdentification_ObjectManagerName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_SystemIdentification_NamespaceCreationClassName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Propagated */
    "\045CIM_Namespace.CreationClassName",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_SystemIdentification_NamespaceCreationClassName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "NamespaceCreationClassName",
    /* qualifiers */
    _CIM_SystemIdentification_NamespaceCreationClassName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_SystemIdentification_NamespaceName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Propagated */
    "\045CIM_Namespace.Name",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_SystemIdentification_NamespaceName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "NamespaceName",
    /* qualifiers */
    _CIM_SystemIdentification_NamespaceName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_SystemIdentification_CreationClassName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_SystemIdentification_CreationClassName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "CreationClassName",
    /* qualifiers */
    _CIM_SystemIdentification_CreationClassName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_SystemIdentification_Name_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* MaxLen */
    "\030\000\000\001\000",
    0,
};

static MetaProperty
_CIM_SystemIdentification_Name =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Name",
    /* qualifiers */
    _CIM_SystemIdentification_Name_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_SystemIdentification_IdentificationFormats_qualifiers[] =
{
    /* Required */
    "\050\001",
    /* Write */
    "\065\001",
    /* ValueMap */
    "\061\000\0140\0001\0002\0003\0004\0005\0006\0007\0008\0009\00010\00011\000",
    /* Values */
    "\062\000\014Unknown\000Other\000Short DNS Name\000Fully qualified DNS Name\000Windows Domain Name\000NetWare Server Name\000Unix Hostname\000Globally Unique System Identifier\000IP Address\000Modem Dialup String\000MAC Address\000IPMI Address\000",
    /* ArrayType */
    "\005Indexed",
    /* ModelCorrespondence */
    "\036\000\002CIM_SystemIdentification.IdentificationData\000CIM_SystemIdentification.FormatDescriptions\000",
    0,
};

static MetaProperty
_CIM_SystemIdentification_IdentificationFormats =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED|META_FLAG_WRITE,
    /* name */
    "IdentificationFormats",
    /* qualifiers */
    _CIM_SystemIdentification_IdentificationFormats_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    0,
    /* value */
    0,
};

static const char*
_CIM_SystemIdentification_FormatDescriptions_qualifiers[] =
{
    /* Write */
    "\065\001",
    /* ArrayType */
    "\005Indexed",
    /* ModelCorrespondence */
    "\036\000\002CIM_SystemIdentification.IdentificationFormats\000CIM_SystemIdentification.IdentificationData\000",
    0,
};

static MetaProperty
_CIM_SystemIdentification_FormatDescriptions =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_WRITE,
    /* name */
    "FormatDescriptions",
    /* qualifiers */
    _CIM_SystemIdentification_FormatDescriptions_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* value */
    0,
};

static const char*
_CIM_SystemIdentification_IdentificationData_qualifiers[] =
{
    /* Write */
    "\065\001",
    /* ArrayType */
    "\005Indexed",
    /* ModelCorrespondence */
    "\036\000\002CIM_SystemIdentification.IdentificationFormats\000CIM_SystemIdentification.FormatDescriptions\000",
    0,
};

static MetaProperty
_CIM_SystemIdentification_IdentificationData =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_WRITE,
    /* name */
    "IdentificationData",
    /* qualifiers */
    _CIM_SystemIdentification_IdentificationData_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* value */
    0,
};

static MetaFeature*
_CIM_SystemIdentification_features[] =
{
    (MetaFeature*)&_CIM_SystemIdentification_SystemCreationClassName,
    (MetaFeature*)&_CIM_SystemIdentification_SystemName,
    (MetaFeature*)&_CIM_SystemIdentification_ObjectManagerCreationClassName,
    (MetaFeature*)&_CIM_SystemIdentification_ObjectManagerName,
    (MetaFeature*)&_CIM_SystemIdentification_NamespaceCreationClassName,
    (MetaFeature*)&_CIM_SystemIdentification_NamespaceName,
    (MetaFeature*)&_CIM_SystemIdentification_CreationClassName,
    (MetaFeature*)&_CIM_SystemIdentification_Name,
    (MetaFeature*)&_CIM_SystemIdentification_IdentificationFormats,
    (MetaFeature*)&_CIM_SystemIdentification_FormatDescriptions,
    (MetaFeature*)&_CIM_SystemIdentification_IdentificationData,
    0,
};

static const char*
__root_PG_InterOp_CIM_SystemIdentification_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Interop",
    /* Version */
    "\0632.7.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_SystemIdentification =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "CIM_SystemIdentification",
    /* qualifiers */
    __root_PG_InterOp_CIM_SystemIdentification_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_ManagedElement,
    /* features */
    _CIM_SystemIdentification_features,
};

//==============================================================================
//
// Class: CIM_SystemInNamespace
//
//==============================================================================

static const char*
_CIM_SystemInNamespace_ManagedNamespace_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Min */
    "\033\000\000\000\001",
    /* Max */
    "\027\000\000\000\001",
    0,
};

static MetaReference
_CIM_SystemInNamespace_ManagedNamespace =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "ManagedNamespace",
    /* qualifiers */
    _CIM_SystemInNamespace_ManagedNamespace_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_Namespace,
};

static const char*
_CIM_SystemInNamespace_Identification_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Weak */
    "\064\001",
    0,
};

static MetaReference
_CIM_SystemInNamespace_Identification =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ|META_FLAG_WEAK,
    /* name */
    "Identification",
    /* qualifiers */
    _CIM_SystemInNamespace_Identification_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_SystemIdentification,
};

static const char*
_CIM_SystemInNamespace_ScopeOfContainedData_qualifiers[] =
{
    /* Required */
    "\050\001",
    /* Write */
    "\065\001",
    /* ValueMap */
    "\061\000\0050\0001\0002\0003\0004\000",
    /* Values */
    "\062\000\005Unknown\000Other\000Local System Data\000Data Aggregated from Multiple Systems\000Proxied Data\000",
    /* ArrayType */
    "\005Indexed",
    /* ModelCorrespondence */
    "\036\000\001CIM_SystemInNamespace.DescriptionOfContainedData\000",
    0,
};

static MetaProperty
_CIM_SystemInNamespace_ScopeOfContainedData =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED|META_FLAG_WRITE,
    /* name */
    "ScopeOfContainedData",
    /* qualifiers */
    _CIM_SystemInNamespace_ScopeOfContainedData_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    0,
    /* value */
    0,
};

static const char*
_CIM_SystemInNamespace_DescriptionOfContainedData_qualifiers[] =
{
    /* Write */
    "\065\001",
    /* ArrayType */
    "\005Indexed",
    /* ModelCorrespondence */
    "\036\000\001CIM_SystemInNamespace.ScopeOfContainedData\000",
    0,
};

static MetaProperty
_CIM_SystemInNamespace_DescriptionOfContainedData =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_WRITE,
    /* name */
    "DescriptionOfContainedData",
    /* qualifiers */
    _CIM_SystemInNamespace_DescriptionOfContainedData_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* value */
    0,
};

static MetaFeature*
_CIM_SystemInNamespace_features[] =
{
    (MetaFeature*)&_CIM_SystemInNamespace_ManagedNamespace,
    (MetaFeature*)&_CIM_SystemInNamespace_Identification,
    (MetaFeature*)&_CIM_SystemInNamespace_ScopeOfContainedData,
    (MetaFeature*)&_CIM_SystemInNamespace_DescriptionOfContainedData,
    0,
};

static const char*
__root_PG_InterOp_CIM_SystemInNamespace_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Interop",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_SystemInNamespace =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_SystemInNamespace",
    /* qualifiers */
    __root_PG_InterOp_CIM_SystemInNamespace_qualifiers,
    /* super */
    0,
    /* features */
    _CIM_SystemInNamespace_features,
};

//==============================================================================
//
// Class: CIM_CIMOMStatisticalData
//
//==============================================================================

static const char*
_CIM_CIMOMStatisticalData_InstanceID_qualifiers[] =
{
    /* Override */
    "\044InstanceID",
    0,
};

static MetaProperty
_CIM_CIMOMStatisticalData_InstanceID =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "InstanceID",
    /* qualifiers */
    _CIM_CIMOMStatisticalData_InstanceID_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_CIMOMStatisticalData_OperationType_qualifiers[] =
{
    /* Required */
    "\050\001",
    /* ValueMap */
    "\061\000\0330\0001\0002\0003\0004\0005\0006\0007\0008\0009\00010\00011\00012\00013\00014\00015\00016\00017\00018\00019\00020\00021\00022\00023\00024\00025\00026\000",
    /* Values */
    "\062\000\033Unknown\000Other\000Batched\000GetClass\000GetInstance\000DeleteClass\000DeleteInstance\000CreateClass\000CreateInstance\000ModifyClass\000ModifyInstance\000EnumerateClasses\000EnumerateClassNames\000EnumerateInstances\000EnumerateInstanceNames\000ExecQuery\000Associators\000AssociatorNames\000References\000ReferenceNames\000GetProperty\000SetProperty\000GetQualifier\000SetQualifier\000DeleteQualifier\000EnumerateQualifiers\000IndicationDelivery\000",
    /* ModelCorrespondence */
    "\036\000\001CIM_CIMOMStatisticalData.OtherOperationType\000",
    0,
};

static MetaProperty
_CIM_CIMOMStatisticalData_OperationType =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "OperationType",
    /* qualifiers */
    _CIM_CIMOMStatisticalData_OperationType_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_CIMOMStatisticalData_OtherOperationType_qualifiers[] =
{
    /* MaxLen */
    "\030\000\000\000\100",
    /* ModelCorrespondence */
    "\036\000\001CIM_CIMOMStatisticalData.OperationType\000",
    0,
};

static MetaProperty
_CIM_CIMOMStatisticalData_OtherOperationType =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "OtherOperationType",
    /* qualifiers */
    _CIM_CIMOMStatisticalData_OtherOperationType_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_CIMOMStatisticalData_NumberOfOperations_qualifiers[] =
{
    /* Required */
    "\050\001",
    /* Counter */
    "\012\001",
    0,
};

static MetaProperty
_CIM_CIMOMStatisticalData_NumberOfOperations =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_COUNTER|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "NumberOfOperations",
    /* qualifiers */
    _CIM_CIMOMStatisticalData_NumberOfOperations_qualifiers,
    /* type */
    CIMTYPE_UINT64,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_CIMOMStatisticalData_CimomElapsedTime_qualifiers[] =
{
    /* Required */
    "\050\001",
    0,
};

static MetaProperty
_CIM_CIMOMStatisticalData_CimomElapsedTime =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "CimomElapsedTime",
    /* qualifiers */
    _CIM_CIMOMStatisticalData_CimomElapsedTime_qualifiers,
    /* type */
    CIMTYPE_DATETIME,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_CIMOMStatisticalData_ProviderElapsedTime_qualifiers[] =
{
    /* Required */
    "\050\001",
    0,
};

static MetaProperty
_CIM_CIMOMStatisticalData_ProviderElapsedTime =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "ProviderElapsedTime",
    /* qualifiers */
    _CIM_CIMOMStatisticalData_ProviderElapsedTime_qualifiers,
    /* type */
    CIMTYPE_DATETIME,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_CIMOMStatisticalData_RequestSize_qualifiers[] =
{
    /* Units */
    "\060Bytes",
    /* Counter */
    "\012\001",
    0,
};

static MetaProperty
_CIM_CIMOMStatisticalData_RequestSize =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_COUNTER|META_FLAG_READ,
    /* name */
    "RequestSize",
    /* qualifiers */
    _CIM_CIMOMStatisticalData_RequestSize_qualifiers,
    /* type */
    CIMTYPE_UINT64,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_CIMOMStatisticalData_ResponseSize_qualifiers[] =
{
    /* Units */
    "\060Bytes",
    /* Counter */
    "\012\001",
    0,
};

static MetaProperty
_CIM_CIMOMStatisticalData_ResponseSize =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_COUNTER|META_FLAG_READ,
    /* name */
    "ResponseSize",
    /* qualifiers */
    _CIM_CIMOMStatisticalData_ResponseSize_qualifiers,
    /* type */
    CIMTYPE_UINT64,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_CIMOMStatisticalData_features[] =
{
    (MetaFeature*)&_CIM_CIMOMStatisticalData_InstanceID,
    (MetaFeature*)&_CIM_CIMOMStatisticalData_OperationType,
    (MetaFeature*)&_CIM_CIMOMStatisticalData_OtherOperationType,
    (MetaFeature*)&_CIM_CIMOMStatisticalData_NumberOfOperations,
    (MetaFeature*)&_CIM_CIMOMStatisticalData_CimomElapsedTime,
    (MetaFeature*)&_CIM_CIMOMStatisticalData_ProviderElapsedTime,
    (MetaFeature*)&_CIM_CIMOMStatisticalData_RequestSize,
    (MetaFeature*)&_CIM_CIMOMStatisticalData_ResponseSize,
    0,
};

static const char*
__root_PG_InterOp_CIM_CIMOMStatisticalData_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Interop",
    /* Version */
    "\0632.7.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_CIMOMStatisticalData =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "CIM_CIMOMStatisticalData",
    /* qualifiers */
    __root_PG_InterOp_CIM_CIMOMStatisticalData_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_StatisticalData,
    /* features */
    _CIM_CIMOMStatisticalData_features,
};

//==============================================================================
//
// Class: CIM_ElementConformsToProfile
//
//==============================================================================

static const char*
_CIM_ElementConformsToProfile_ConformantStandard_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_ElementConformsToProfile_ConformantStandard =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "ConformantStandard",
    /* qualifiers */
    _CIM_ElementConformsToProfile_ConformantStandard_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_RegisteredProfile,
};

static const char*
_CIM_ElementConformsToProfile_ManagedElement_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_CIM_ElementConformsToProfile_ManagedElement =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "ManagedElement",
    /* qualifiers */
    _CIM_ElementConformsToProfile_ManagedElement_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ManagedElement,
};

static MetaFeature*
_CIM_ElementConformsToProfile_features[] =
{
    (MetaFeature*)&_CIM_ElementConformsToProfile_ConformantStandard,
    (MetaFeature*)&_CIM_ElementConformsToProfile_ManagedElement,
    0,
};

static const char*
__root_PG_InterOp_CIM_ElementConformsToProfile_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Interop",
    /* Version */
    "\0632.8.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ElementConformsToProfile =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_ElementConformsToProfile",
    /* qualifiers */
    __root_PG_InterOp_CIM_ElementConformsToProfile_qualifiers,
    /* super */
    0,
    /* features */
    _CIM_ElementConformsToProfile_features,
};

//==============================================================================
//
// Class: CIM_ReferencedProfile
//
//==============================================================================

static const char*
_CIM_ReferencedProfile_Antecedent_qualifiers[] =
{
    /* Override */
    "\044Antecedent",
    0,
};

static MetaReference
_CIM_ReferencedProfile_Antecedent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Antecedent",
    /* qualifiers */
    _CIM_ReferencedProfile_Antecedent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_RegisteredProfile,
};

static const char*
_CIM_ReferencedProfile_Dependent_qualifiers[] =
{
    /* Override */
    "\044Dependent",
    0,
};

static MetaReference
_CIM_ReferencedProfile_Dependent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Dependent",
    /* qualifiers */
    _CIM_ReferencedProfile_Dependent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_RegisteredProfile,
};

static MetaFeature*
_CIM_ReferencedProfile_features[] =
{
    (MetaFeature*)&_CIM_ReferencedProfile_Antecedent,
    (MetaFeature*)&_CIM_ReferencedProfile_Dependent,
    0,
};

static const char*
__root_PG_InterOp_CIM_ReferencedProfile_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Interop",
    /* Version */
    "\0632.8.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ReferencedProfile =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_ReferencedProfile",
    /* qualifiers */
    __root_PG_InterOp_CIM_ReferencedProfile_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_Dependency,
    /* features */
    _CIM_ReferencedProfile_features,
};

//==============================================================================
//
// Class: CIM_SubProfileRequiresProfile
//
//==============================================================================

static const char*
_CIM_SubProfileRequiresProfile_Antecedent_qualifiers[] =
{
    /* Override */
    "\044Antecedent",
    /* Min */
    "\033\000\000\000\001",
    0,
};

static MetaReference
_CIM_SubProfileRequiresProfile_Antecedent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Antecedent",
    /* qualifiers */
    _CIM_SubProfileRequiresProfile_Antecedent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_RegisteredProfile,
};

static const char*
_CIM_SubProfileRequiresProfile_Dependent_qualifiers[] =
{
    /* Override */
    "\044Dependent",
    0,
};

static MetaReference
_CIM_SubProfileRequiresProfile_Dependent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Dependent",
    /* qualifiers */
    _CIM_SubProfileRequiresProfile_Dependent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_RegisteredSubProfile,
};

static MetaFeature*
_CIM_SubProfileRequiresProfile_features[] =
{
    (MetaFeature*)&_CIM_SubProfileRequiresProfile_Antecedent,
    (MetaFeature*)&_CIM_SubProfileRequiresProfile_Dependent,
    0,
};

static const char*
__root_PG_InterOp_CIM_SubProfileRequiresProfile_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Interop",
    /* Version */
    "\0632.8.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_SubProfileRequiresProfile =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_SubProfileRequiresProfile",
    /* qualifiers */
    __root_PG_InterOp_CIM_SubProfileRequiresProfile_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_ReferencedProfile,
    /* features */
    _CIM_SubProfileRequiresProfile_features,
};

//==============================================================================
//
// Class: CIM_WBEMService
//
//==============================================================================

static MetaFeature*
_CIM_WBEMService_features[] =
{
    0,
};

static const char*
__root_PG_InterOp_CIM_WBEMService_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Interop",
    /* Abstract */
    "\002\001",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_WBEMService =
{
    /* flags */
    META_FLAG_CLASS|META_FLAG_ABSTRACT,
    /* name */
    "CIM_WBEMService",
    /* qualifiers */
    __root_PG_InterOp_CIM_WBEMService_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_Service,
    /* features */
    _CIM_WBEMService_features,
};

//==============================================================================
//
// Class: CIM_IdentificationOfManagedSystem
//
//==============================================================================

static const char*
_CIM_IdentificationOfManagedSystem_Antecedent_qualifiers[] =
{
    /* Override */
    "\044Antecedent",
    /* Max */
    "\027\000\000\000\001",
    0,
};

static MetaReference
_CIM_IdentificationOfManagedSystem_Antecedent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Antecedent",
    /* qualifiers */
    _CIM_IdentificationOfManagedSystem_Antecedent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_System,
};

static const char*
_CIM_IdentificationOfManagedSystem_Dependent_qualifiers[] =
{
    /* Override */
    "\044Dependent",
    /* Max */
    "\027\000\000\000\001",
    0,
};

static MetaReference
_CIM_IdentificationOfManagedSystem_Dependent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Dependent",
    /* qualifiers */
    _CIM_IdentificationOfManagedSystem_Dependent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_SystemIdentification,
};

static MetaFeature*
_CIM_IdentificationOfManagedSystem_features[] =
{
    (MetaFeature*)&_CIM_IdentificationOfManagedSystem_Antecedent,
    (MetaFeature*)&_CIM_IdentificationOfManagedSystem_Dependent,
    0,
};

static const char*
__root_PG_InterOp_CIM_IdentificationOfManagedSystem_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Interop",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_IdentificationOfManagedSystem =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_IdentificationOfManagedSystem",
    /* qualifiers */
    __root_PG_InterOp_CIM_IdentificationOfManagedSystem_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_Dependency,
    /* features */
    _CIM_IdentificationOfManagedSystem_features,
};

//==============================================================================
//
// Class: CIM_ObjectManager
//
//==============================================================================

static const char*
_CIM_ObjectManager_GatherStatisticalData_qualifiers[] =
{
    /* Write */
    "\065\001",
    0,
};

static MetaProperty
_CIM_ObjectManager_GatherStatisticalData =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_WRITE,
    /* name */
    "GatherStatisticalData",
    /* qualifiers */
    _CIM_ObjectManager_GatherStatisticalData_qualifiers,
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* value */
    "\001",
};

static const char*
_CIM_ObjectManager_Name_qualifiers[] =
{
    /* Override */
    "\044Name",
    0,
};

static MetaProperty
_CIM_ObjectManager_Name =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "Name",
    /* qualifiers */
    _CIM_ObjectManager_Name_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_ObjectManager_ElementName_qualifiers[] =
{
    /* Override */
    "\044ElementName",
    0,
};

static MetaProperty
_CIM_ObjectManager_ElementName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "ElementName",
    /* qualifiers */
    _CIM_ObjectManager_ElementName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_ObjectManager_Description_qualifiers[] =
{
    /* Override */
    "\044Description",
    0,
};

static MetaProperty
_CIM_ObjectManager_Description =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "Description",
    /* qualifiers */
    _CIM_ObjectManager_Description_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_ObjectManager_features[] =
{
    (MetaFeature*)&_CIM_ObjectManager_GatherStatisticalData,
    (MetaFeature*)&_CIM_ObjectManager_Name,
    (MetaFeature*)&_CIM_ObjectManager_ElementName,
    (MetaFeature*)&_CIM_ObjectManager_Description,
    0,
};

static const char*
__root_PG_InterOp_CIM_ObjectManager_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Interop",
    /* Version */
    "\0632.8.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ObjectManager =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "CIM_ObjectManager",
    /* qualifiers */
    __root_PG_InterOp_CIM_ObjectManager_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_WBEMService,
    /* features */
    _CIM_ObjectManager_features,
};

//==============================================================================
//
// Class: CIM_ObjectManagerAdapter
//
//==============================================================================

static const char*
_CIM_ObjectManagerAdapter_Name_qualifiers[] =
{
    /* Override */
    "\044Name",
    0,
};

static MetaProperty
_CIM_ObjectManagerAdapter_Name =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "Name",
    /* qualifiers */
    _CIM_ObjectManagerAdapter_Name_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_ObjectManagerAdapter_ElementName_qualifiers[] =
{
    /* Override */
    "\044ElementName",
    0,
};

static MetaProperty
_CIM_ObjectManagerAdapter_ElementName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "ElementName",
    /* qualifiers */
    _CIM_ObjectManagerAdapter_ElementName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_ObjectManagerAdapter_Handle_qualifiers[] =
{
    /* Required */
    "\050\001",
    0,
};

static MetaProperty
_CIM_ObjectManagerAdapter_Handle =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "Handle",
    /* qualifiers */
    _CIM_ObjectManagerAdapter_Handle_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_ObjectManagerAdapter_AdapterType_qualifiers[] =
{
    /* Required */
    "\050\001",
    /* ValueMap */
    "\061\000\0051\0002\0003\0004\0005\000",
    /* Values */
    "\062\000\005Other\000Client\000Provider\000Indication Handler\000Repository\000",
    /* ModelCorrespondence */
    "\036\000\001CIM_ObjectManagerAdapter.OtherAdapterTypeDescription\000",
    0,
};

static MetaProperty
_CIM_ObjectManagerAdapter_AdapterType =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "AdapterType",
    /* qualifiers */
    _CIM_ObjectManagerAdapter_AdapterType_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_ObjectManagerAdapter_OtherAdapterTypeDescription_qualifiers[] =
{
    /* ModelCorrespondence */
    "\036\000\001CIM_ObjectManagerAdapter.AdapterType\000",
    0,
};

static MetaProperty
_CIM_ObjectManagerAdapter_OtherAdapterTypeDescription =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "OtherAdapterTypeDescription",
    /* qualifiers */
    _CIM_ObjectManagerAdapter_OtherAdapterTypeDescription_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_ObjectManagerAdapter_features[] =
{
    (MetaFeature*)&_CIM_ObjectManagerAdapter_Name,
    (MetaFeature*)&_CIM_ObjectManagerAdapter_ElementName,
    (MetaFeature*)&_CIM_ObjectManagerAdapter_Handle,
    (MetaFeature*)&_CIM_ObjectManagerAdapter_AdapterType,
    (MetaFeature*)&_CIM_ObjectManagerAdapter_OtherAdapterTypeDescription,
    0,
};

static const char*
__root_PG_InterOp_CIM_ObjectManagerAdapter_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Interop",
    /* Version */
    "\0632.8.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ObjectManagerAdapter =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "CIM_ObjectManagerAdapter",
    /* qualifiers */
    __root_PG_InterOp_CIM_ObjectManagerAdapter_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_WBEMService,
    /* features */
    _CIM_ObjectManagerAdapter_features,
};

//==============================================================================
//
// Class: CIM_ObjectManagerCommunicationMechanism
//
//==============================================================================

static const char*
_CIM_ObjectManagerCommunicationMechanism_CommunicationMechanism_qualifiers[] =
{
    /* Required */
    "\050\001",
    /* ValueMap */
    "\061\000\0060\0001\0002\0003\0004\0005\000",
    /* Values */
    "\062\000\006Unknown\000Other\000CIM-XML\000SM-CLP\000WS-Management\000WSDM\000",
    /* ModelCorrespondence */
    "\036\000\001CIM_ObjectManagerCommunicationMechanism.OtherCommunicationMechanismDescription\000",
    0,
};

static MetaProperty
_CIM_ObjectManagerCommunicationMechanism_CommunicationMechanism =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "CommunicationMechanism",
    /* qualifiers */
    _CIM_ObjectManagerCommunicationMechanism_CommunicationMechanism_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_ObjectManagerCommunicationMechanism_OtherCommunicationMechanismDescription_qualifiers[] =
{
    /* ModelCorrespondence */
    "\036\000\001CIM_ObjectManagerCommunicationMechanism.CommunicationMechanism\000",
    0,
};

static MetaProperty
_CIM_ObjectManagerCommunicationMechanism_OtherCommunicationMechanismDescription =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "OtherCommunicationMechanismDescription",
    /* qualifiers */
    _CIM_ObjectManagerCommunicationMechanism_OtherCommunicationMechanismDescription_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_ObjectManagerCommunicationMechanism_FunctionalProfilesSupported_qualifiers[] =
{
    /* Required */
    "\050\001",
    /* ValueMap */
    "\061\000\0120\0001\0002\0003\0004\0005\0006\0007\0008\0009\000",
    /* Values */
    "\062\000\012Unknown\000Other\000Basic Read\000Basic Write\000Schema Manipulation\000Instance Manipulation\000Association Traversal\000Query Execution\000Qualifier Declaration\000Indications\000",
    /* ArrayType */
    "\005Indexed",
    /* ModelCorrespondence */
    "\036\000\001CIM_ObjectManagerCommunicationMechanism.FunctionalProfileDescriptions\000",
    0,
};

static MetaProperty
_CIM_ObjectManagerCommunicationMechanism_FunctionalProfilesSupported =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "FunctionalProfilesSupported",
    /* qualifiers */
    _CIM_ObjectManagerCommunicationMechanism_FunctionalProfilesSupported_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    0,
    /* value */
    0,
};

static const char*
_CIM_ObjectManagerCommunicationMechanism_FunctionalProfileDescriptions_qualifiers[] =
{
    /* ArrayType */
    "\005Indexed",
    /* ModelCorrespondence */
    "\036\000\001CIM_ObjectManagerCommunicationMechanism.FunctionalProfilesSupported\000",
    0,
};

static MetaProperty
_CIM_ObjectManagerCommunicationMechanism_FunctionalProfileDescriptions =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "FunctionalProfileDescriptions",
    /* qualifiers */
    _CIM_ObjectManagerCommunicationMechanism_FunctionalProfileDescriptions_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* value */
    0,
};

static const char*
_CIM_ObjectManagerCommunicationMechanism_MultipleOperationsSupported_qualifiers[] =
{
    /* Required */
    "\050\001",
    0,
};

static MetaProperty
_CIM_ObjectManagerCommunicationMechanism_MultipleOperationsSupported =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "MultipleOperationsSupported",
    /* qualifiers */
    _CIM_ObjectManagerCommunicationMechanism_MultipleOperationsSupported_qualifiers,
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_ObjectManagerCommunicationMechanism_AuthenticationMechanismsSupported_qualifiers[] =
{
    /* Required */
    "\050\001",
    /* ValueMap */
    "\061\000\0050\0001\0002\0003\0004\000",
    /* Values */
    "\062\000\005Unknown\000Other\000None\000Basic\000Digest\000",
    /* ArrayType */
    "\005Indexed",
    /* ModelCorrespondence */
    "\036\000\001CIM_ObjectManagerCommunicationMechanism.AuthenticationMechanismDescriptions\000",
    0,
};

static MetaProperty
_CIM_ObjectManagerCommunicationMechanism_AuthenticationMechanismsSupported =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "AuthenticationMechanismsSupported",
    /* qualifiers */
    _CIM_ObjectManagerCommunicationMechanism_AuthenticationMechanismsSupported_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    0,
    /* value */
    0,
};

static const char*
_CIM_ObjectManagerCommunicationMechanism_AuthenticationMechanismDescriptions_qualifiers[] =
{
    /* ArrayType */
    "\005Indexed",
    /* ModelCorrespondence */
    "\036\000\001CIM_ObjectManagerCommunicationMechanism.AuthenticationMechanismsSupported\000",
    0,
};

static MetaProperty
_CIM_ObjectManagerCommunicationMechanism_AuthenticationMechanismDescriptions =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "AuthenticationMechanismDescriptions",
    /* qualifiers */
    _CIM_ObjectManagerCommunicationMechanism_AuthenticationMechanismDescriptions_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* value */
    0,
};

static const char*
_CIM_ObjectManagerCommunicationMechanism_Version_qualifiers[] =
{
    /* Required */
    "\050\001",
    0,
};

static MetaProperty
_CIM_ObjectManagerCommunicationMechanism_Version =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "Version",
    /* qualifiers */
    _CIM_ObjectManagerCommunicationMechanism_Version_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_ObjectManagerCommunicationMechanism_AdvertiseTypes_qualifiers[] =
{
    /* Required */
    "\050\001",
    /* ValueMap */
    "\061\000\0031\0002\0003\000",
    /* Values */
    "\062\000\003Other\000Not Advertised\000SLP\000",
    /* ArrayType */
    "\005Indexed",
    /* ModelCorrespondence */
    "\036\000\001CIM_ObjectManagerCommunicationMechanism.AdvertiseTypeDescriptions\000",
    0,
};

static MetaProperty
_CIM_ObjectManagerCommunicationMechanism_AdvertiseTypes =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "AdvertiseTypes",
    /* qualifiers */
    _CIM_ObjectManagerCommunicationMechanism_AdvertiseTypes_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    0,
    /* value */
    0,
};

static const char*
_CIM_ObjectManagerCommunicationMechanism_AdvertiseTypeDescriptions_qualifiers[] =
{
    /* ArrayType */
    "\005Indexed",
    /* ModelCorrespondence */
    "\036\000\001CIM_ObjectManagerCommunicationMechanism.AdvertiseTypes\000",
    0,
};

static MetaProperty
_CIM_ObjectManagerCommunicationMechanism_AdvertiseTypeDescriptions =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "AdvertiseTypeDescriptions",
    /* qualifiers */
    _CIM_ObjectManagerCommunicationMechanism_AdvertiseTypeDescriptions_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* value */
    0,
};

static MetaFeature*
_CIM_ObjectManagerCommunicationMechanism_features[] =
{
    (MetaFeature*)&_CIM_ObjectManagerCommunicationMechanism_CommunicationMechanism,
    (MetaFeature*)&_CIM_ObjectManagerCommunicationMechanism_OtherCommunicationMechanismDescription,
    (MetaFeature*)&_CIM_ObjectManagerCommunicationMechanism_FunctionalProfilesSupported,
    (MetaFeature*)&_CIM_ObjectManagerCommunicationMechanism_FunctionalProfileDescriptions,
    (MetaFeature*)&_CIM_ObjectManagerCommunicationMechanism_MultipleOperationsSupported,
    (MetaFeature*)&_CIM_ObjectManagerCommunicationMechanism_AuthenticationMechanismsSupported,
    (MetaFeature*)&_CIM_ObjectManagerCommunicationMechanism_AuthenticationMechanismDescriptions,
    (MetaFeature*)&_CIM_ObjectManagerCommunicationMechanism_Version,
    (MetaFeature*)&_CIM_ObjectManagerCommunicationMechanism_AdvertiseTypes,
    (MetaFeature*)&_CIM_ObjectManagerCommunicationMechanism_AdvertiseTypeDescriptions,
    0,
};

static const char*
__root_PG_InterOp_CIM_ObjectManagerCommunicationMechanism_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Interop",
    /* Version */
    "\0632.13.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ObjectManagerCommunicationMechanism =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "CIM_ObjectManagerCommunicationMechanism",
    /* qualifiers */
    __root_PG_InterOp_CIM_ObjectManagerCommunicationMechanism_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_ServiceAccessPoint,
    /* features */
    _CIM_ObjectManagerCommunicationMechanism_features,
};

//==============================================================================
//
// Class: CIM_ProtocolAdapter
//
//==============================================================================

static const char*
_CIM_ProtocolAdapter_Name_qualifiers[] =
{
    /* Deprecated */
    "\013\000\001CIM_ObjectManagerAdapter.Name\000",
    /* Override */
    "\044Name",
    0,
};

static MetaProperty
_CIM_ProtocolAdapter_Name =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "Name",
    /* qualifiers */
    _CIM_ProtocolAdapter_Name_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_ProtocolAdapter_Handle_qualifiers[] =
{
    /* Deprecated */
    "\013\000\001CIM_ObjectManagerAdapter.Handle\000",
    /* Required */
    "\050\001",
    0,
};

static MetaProperty
_CIM_ProtocolAdapter_Handle =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "Handle",
    /* qualifiers */
    _CIM_ProtocolAdapter_Handle_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_ProtocolAdapter_ProtocolAdapterType_qualifiers[] =
{
    /* Deprecated */
    "\013\000\001CIM_ObjectManagerAdapter.AdapterType\000",
    /* Required */
    "\050\001",
    /* ValueMap */
    "\061\000\0031\0002\0003\000",
    /* Values */
    "\062\000\003Other\000Client\000Provider\000",
    /* ModelCorrespondence */
    "\036\000\001CIM_ProtocolAdapter.OtherProtocolAdapterType\000",
    0,
};

static MetaProperty
_CIM_ProtocolAdapter_ProtocolAdapterType =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "ProtocolAdapterType",
    /* qualifiers */
    _CIM_ProtocolAdapter_ProtocolAdapterType_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_ProtocolAdapter_OtherProtocolAdapterType_qualifiers[] =
{
    /* Deprecated */
    "\013\000\001CIM_ObjectManagerAdapter.OtherAdapterTypeDescription\000",
    /* ModelCorrespondence */
    "\036\000\001CIM_ProtocolAdapter.ProtocolAdapterType\000",
    0,
};

static MetaProperty
_CIM_ProtocolAdapter_OtherProtocolAdapterType =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "OtherProtocolAdapterType",
    /* qualifiers */
    _CIM_ProtocolAdapter_OtherProtocolAdapterType_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_ProtocolAdapter_features[] =
{
    (MetaFeature*)&_CIM_ProtocolAdapter_Name,
    (MetaFeature*)&_CIM_ProtocolAdapter_Handle,
    (MetaFeature*)&_CIM_ProtocolAdapter_ProtocolAdapterType,
    (MetaFeature*)&_CIM_ProtocolAdapter_OtherProtocolAdapterType,
    0,
};

static const char*
__root_PG_InterOp_CIM_ProtocolAdapter_qualifiers[] =
{
    /* Deprecated */
    "\013\000\001CIM_ObjectManagerAdapter\000",
    /* UMLPackagePath */
    "\057CIM::Interop",
    /* Version */
    "\0632.8.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ProtocolAdapter =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "CIM_ProtocolAdapter",
    /* qualifiers */
    __root_PG_InterOp_CIM_ProtocolAdapter_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_WBEMService,
    /* features */
    _CIM_ProtocolAdapter_features,
};

//==============================================================================
//
// Class: CIM_CIMXMLCommunicationMechanism
//
//==============================================================================

static const char*
_CIM_CIMXMLCommunicationMechanism_CommunicationMechanism_qualifiers[] =
{
    /* Required */
    "\050\001",
    /* Override */
    "\044CommunicationMechanism",
    /* ValueMap */
    "\061\000\0012\000",
    /* Values */
    "\062\000\001CIM-XML\000",
    0,
};

static MetaProperty
_CIM_CIMXMLCommunicationMechanism_CommunicationMechanism =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "CommunicationMechanism",
    /* qualifiers */
    _CIM_CIMXMLCommunicationMechanism_CommunicationMechanism_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    "\000\002",
};

static const char*
_CIM_CIMXMLCommunicationMechanism_Version_qualifiers[] =
{
    /* Required */
    "\050\001",
    /* Override */
    "\044Version",
    /* ValueMap */
    "\061\000\0031.0\0001.1\0001.2\000",
    0,
};

static MetaProperty
_CIM_CIMXMLCommunicationMechanism_Version =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "Version",
    /* qualifiers */
    _CIM_CIMXMLCommunicationMechanism_Version_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_CIMXMLCommunicationMechanism_CIMXMLProtocolVersion_qualifiers[] =
{
    /* Deprecated */
    "\013\000\001CIM_CIMXMLCommunicationMechanism.Version\000",
    /* Required */
    "\050\001",
    /* ValueMap */
    "\061\000\0020\0001\000",
    /* Values */
    "\062\000\002Unknown\0001.0\000",
    0,
};

static MetaProperty
_CIM_CIMXMLCommunicationMechanism_CIMXMLProtocolVersion =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "CIMXMLProtocolVersion",
    /* qualifiers */
    _CIM_CIMXMLCommunicationMechanism_CIMXMLProtocolVersion_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_CIMXMLCommunicationMechanism_CIMValidated_qualifiers[] =
{
    /* Required */
    "\050\001",
    0,
};

static MetaProperty
_CIM_CIMXMLCommunicationMechanism_CIMValidated =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "CIMValidated",
    /* qualifiers */
    _CIM_CIMXMLCommunicationMechanism_CIMValidated_qualifiers,
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_CIMXMLCommunicationMechanism_features[] =
{
    (MetaFeature*)&_CIM_CIMXMLCommunicationMechanism_CommunicationMechanism,
    (MetaFeature*)&_CIM_CIMXMLCommunicationMechanism_Version,
    (MetaFeature*)&_CIM_CIMXMLCommunicationMechanism_CIMXMLProtocolVersion,
    (MetaFeature*)&_CIM_CIMXMLCommunicationMechanism_CIMValidated,
    0,
};

static const char*
__root_PG_InterOp_CIM_CIMXMLCommunicationMechanism_qualifiers[] =
{
    /* UMLPackagePath */
    "\057CIM::Interop",
    /* Version */
    "\0632.8.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_CIMXMLCommunicationMechanism =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "CIM_CIMXMLCommunicationMechanism",
    /* qualifiers */
    __root_PG_InterOp_CIM_CIMXMLCommunicationMechanism_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_ObjectManagerCommunicationMechanism,
    /* features */
    _CIM_CIMXMLCommunicationMechanism_features,
};

//==============================================================================
//
// Class: CIM_CommMechanismForAdapter
//
//==============================================================================

static const char*
_CIM_CommMechanismForAdapter_Antecedent_qualifiers[] =
{
    /* Deprecated */
    "\013\000\001CIM_CommMechanismForObjectManagerAdapter.Antecedent\000",
    /* Override */
    "\044Antecedent",
    0,
};

static MetaReference
_CIM_CommMechanismForAdapter_Antecedent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Antecedent",
    /* qualifiers */
    _CIM_CommMechanismForAdapter_Antecedent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ProtocolAdapter,
};

static const char*
_CIM_CommMechanismForAdapter_Dependent_qualifiers[] =
{
    /* Deprecated */
    "\013\000\001CIM_CommMechanismForObjectManagerAdapter.Dependent\000",
    /* Override */
    "\044Dependent",
    /* Min */
    "\033\000\000\000\001",
    0,
};

static MetaReference
_CIM_CommMechanismForAdapter_Dependent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Dependent",
    /* qualifiers */
    _CIM_CommMechanismForAdapter_Dependent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ObjectManagerCommunicationMechanism,
};

static MetaFeature*
_CIM_CommMechanismForAdapter_features[] =
{
    (MetaFeature*)&_CIM_CommMechanismForAdapter_Antecedent,
    (MetaFeature*)&_CIM_CommMechanismForAdapter_Dependent,
    0,
};

static const char*
__root_PG_InterOp_CIM_CommMechanismForAdapter_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* Deprecated */
    "\013\000\001CIM_CommMechanismForObjectManagerAdapter\000",
    /* UMLPackagePath */
    "\057CIM::Interop",
    /* Version */
    "\0632.8.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_CommMechanismForAdapter =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_CommMechanismForAdapter",
    /* qualifiers */
    __root_PG_InterOp_CIM_CommMechanismForAdapter_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_Dependency,
    /* features */
    _CIM_CommMechanismForAdapter_features,
};

//==============================================================================
//
// Class: CIM_CommMechanismForManager
//
//==============================================================================

static const char*
_CIM_CommMechanismForManager_Antecedent_qualifiers[] =
{
    /* Override */
    "\044Antecedent",
    /* Min */
    "\033\000\000\000\001",
    /* Max */
    "\027\000\000\000\001",
    0,
};

static MetaReference
_CIM_CommMechanismForManager_Antecedent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Antecedent",
    /* qualifiers */
    _CIM_CommMechanismForManager_Antecedent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ObjectManager,
};

static const char*
_CIM_CommMechanismForManager_Dependent_qualifiers[] =
{
    /* Override */
    "\044Dependent",
    /* Min */
    "\033\000\000\000\001",
    0,
};

static MetaReference
_CIM_CommMechanismForManager_Dependent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Dependent",
    /* qualifiers */
    _CIM_CommMechanismForManager_Dependent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ObjectManagerCommunicationMechanism,
};

static MetaFeature*
_CIM_CommMechanismForManager_features[] =
{
    (MetaFeature*)&_CIM_CommMechanismForManager_Antecedent,
    (MetaFeature*)&_CIM_CommMechanismForManager_Dependent,
    0,
};

static const char*
__root_PG_InterOp_CIM_CommMechanismForManager_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Interop",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_CommMechanismForManager =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_CommMechanismForManager",
    /* qualifiers */
    __root_PG_InterOp_CIM_CommMechanismForManager_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_ServiceAccessBySAP,
    /* features */
    _CIM_CommMechanismForManager_features,
};

//==============================================================================
//
// Class: CIM_CommMechanismForObjectManagerAdapter
//
//==============================================================================

static const char*
_CIM_CommMechanismForObjectManagerAdapter_Antecedent_qualifiers[] =
{
    /* Override */
    "\044Antecedent",
    0,
};

static MetaReference
_CIM_CommMechanismForObjectManagerAdapter_Antecedent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Antecedent",
    /* qualifiers */
    _CIM_CommMechanismForObjectManagerAdapter_Antecedent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ObjectManagerAdapter,
};

static const char*
_CIM_CommMechanismForObjectManagerAdapter_Dependent_qualifiers[] =
{
    /* Override */
    "\044Dependent",
    /* Min */
    "\033\000\000\000\001",
    0,
};

static MetaReference
_CIM_CommMechanismForObjectManagerAdapter_Dependent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Dependent",
    /* qualifiers */
    _CIM_CommMechanismForObjectManagerAdapter_Dependent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ObjectManagerCommunicationMechanism,
};

static MetaFeature*
_CIM_CommMechanismForObjectManagerAdapter_features[] =
{
    (MetaFeature*)&_CIM_CommMechanismForObjectManagerAdapter_Antecedent,
    (MetaFeature*)&_CIM_CommMechanismForObjectManagerAdapter_Dependent,
    0,
};

static const char*
__root_PG_InterOp_CIM_CommMechanismForObjectManagerAdapter_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Interop",
    /* Version */
    "\0632.8.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_CommMechanismForObjectManagerAdapter =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_CommMechanismForObjectManagerAdapter",
    /* qualifiers */
    __root_PG_InterOp_CIM_CommMechanismForObjectManagerAdapter_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_Dependency,
    /* features */
    _CIM_CommMechanismForObjectManagerAdapter_features,
};

//==============================================================================
//
// Class: CIM_NamespaceInManager
//
//==============================================================================

static const char*
_CIM_NamespaceInManager_Antecedent_qualifiers[] =
{
    /* Override */
    "\044Antecedent",
    /* Min */
    "\033\000\000\000\001",
    /* Max */
    "\027\000\000\000\001",
    0,
};

static MetaReference
_CIM_NamespaceInManager_Antecedent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ,
    /* name */
    "Antecedent",
    /* qualifiers */
    _CIM_NamespaceInManager_Antecedent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_ObjectManager,
};

static const char*
_CIM_NamespaceInManager_Dependent_qualifiers[] =
{
    /* Override */
    "\044Dependent",
    /* Weak */
    "\064\001",
    0,
};

static MetaReference
_CIM_NamespaceInManager_Dependent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ|META_FLAG_WEAK,
    /* name */
    "Dependent",
    /* qualifiers */
    _CIM_NamespaceInManager_Dependent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_CIM_Namespace,
};

static MetaFeature*
_CIM_NamespaceInManager_features[] =
{
    (MetaFeature*)&_CIM_NamespaceInManager_Antecedent,
    (MetaFeature*)&_CIM_NamespaceInManager_Dependent,
    0,
};

static const char*
__root_PG_InterOp_CIM_NamespaceInManager_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* UMLPackagePath */
    "\057CIM::Interop",
    /* Version */
    "\0632.7.0",
    0,
};

MetaClass
__root_PG_InterOp_CIM_NamespaceInManager =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "CIM_NamespaceInManager",
    /* qualifiers */
    __root_PG_InterOp_CIM_NamespaceInManager_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_HostedDependency,
    /* features */
    _CIM_NamespaceInManager_features,
};

//==============================================================================
//
// Class: PG_IndicationHandlerSNMPMapper
//
//==============================================================================

static const char*
_PG_IndicationHandlerSNMPMapper_TargetHost_qualifiers[] =
{
    /* Required */
    "\050\001",
    0,
};

static MetaProperty
_PG_IndicationHandlerSNMPMapper_TargetHost =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "TargetHost",
    /* qualifiers */
    _PG_IndicationHandlerSNMPMapper_TargetHost_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_PG_IndicationHandlerSNMPMapper_TargetHostFormat_qualifiers[] =
{
    /* Required */
    "\050\001",
    /* ValueMap */
    "\061\000\0051\0002\0003\0004\0005..65535\000",
    /* Values */
    "\062\000\005Other\000Host Name\000IPV4 Address\000IPV6 Address\000Pegasus Reserved\000",
    /* ModelCorrespondence */
    "\036\000\001PG_IndicationHandlerSNMPMapper.OtherTargetHostFormat\000",
    0,
};

static MetaProperty
_PG_IndicationHandlerSNMPMapper_TargetHostFormat =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "TargetHostFormat",
    /* qualifiers */
    _PG_IndicationHandlerSNMPMapper_TargetHostFormat_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_PG_IndicationHandlerSNMPMapper_OtherTargetHostFormat_qualifiers[] =
{
    /* ModelCorrespondence */
    "\036\000\001PG_IndicationHandlerSNMPMapper.TargetHostFormat\000",
    0,
};

static MetaProperty
_PG_IndicationHandlerSNMPMapper_OtherTargetHostFormat =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "OtherTargetHostFormat",
    /* qualifiers */
    _PG_IndicationHandlerSNMPMapper_OtherTargetHostFormat_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_PG_IndicationHandlerSNMPMapper_PortNumber_qualifiers[] =
{
    0,
};

static MetaProperty
_PG_IndicationHandlerSNMPMapper_PortNumber =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "PortNumber",
    /* qualifiers */
    _PG_IndicationHandlerSNMPMapper_PortNumber_qualifiers,
    /* type */
    CIMTYPE_UINT32,
    /* subscript */
    -1,
    /* value */
    "\000\000\000\242",
};

static const char*
_PG_IndicationHandlerSNMPMapper_SNMPVersion_qualifiers[] =
{
    /* Required */
    "\050\001",
    /* ValueMap */
    "\061\000\0062\0003\0004\0005\0006\0007..65535\000",
    /* Values */
    "\062\000\006SNMPv1 Trap\000SNMPv2C Trap\000SNMPv2C Inform\000SNMPv3 Trap\000SNMPv3 Inform\000Pegasus Reserved\000",
    0,
};

static MetaProperty
_PG_IndicationHandlerSNMPMapper_SNMPVersion =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "SNMPVersion",
    /* qualifiers */
    _PG_IndicationHandlerSNMPMapper_SNMPVersion_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_PG_IndicationHandlerSNMPMapper_SNMPSecurityName_qualifiers[] =
{
    /* ModelCorrespondence */
    "\036\000\001PG_IndicationHandlerSNMPMapper.SNMPVersion\000",
    0,
};

static MetaProperty
_PG_IndicationHandlerSNMPMapper_SNMPSecurityName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "SNMPSecurityName",
    /* qualifiers */
    _PG_IndicationHandlerSNMPMapper_SNMPSecurityName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_PG_IndicationHandlerSNMPMapper_SNMPEngineID_qualifiers[] =
{
    0,
};

static MetaProperty
_PG_IndicationHandlerSNMPMapper_SNMPEngineID =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "SNMPEngineID",
    /* qualifiers */
    _PG_IndicationHandlerSNMPMapper_SNMPEngineID_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_PG_IndicationHandlerSNMPMapper_features[] =
{
    (MetaFeature*)&_PG_IndicationHandlerSNMPMapper_TargetHost,
    (MetaFeature*)&_PG_IndicationHandlerSNMPMapper_TargetHostFormat,
    (MetaFeature*)&_PG_IndicationHandlerSNMPMapper_OtherTargetHostFormat,
    (MetaFeature*)&_PG_IndicationHandlerSNMPMapper_PortNumber,
    (MetaFeature*)&_PG_IndicationHandlerSNMPMapper_SNMPVersion,
    (MetaFeature*)&_PG_IndicationHandlerSNMPMapper_SNMPSecurityName,
    (MetaFeature*)&_PG_IndicationHandlerSNMPMapper_SNMPEngineID,
    0,
};

static const char*
__root_PG_InterOp_PG_IndicationHandlerSNMPMapper_qualifiers[] =
{
    0,
};

MetaClass
__root_PG_InterOp_PG_IndicationHandlerSNMPMapper =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "PG_IndicationHandlerSNMPMapper",
    /* qualifiers */
    __root_PG_InterOp_PG_IndicationHandlerSNMPMapper_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_IndicationHandler,
    /* features */
    _PG_IndicationHandlerSNMPMapper_features,
};

//==============================================================================
//
// Class: PG_ListenerDestinationSystemLog
//
//==============================================================================

static MetaFeature*
_PG_ListenerDestinationSystemLog_features[] =
{
    0,
};

static const char*
__root_PG_InterOp_PG_ListenerDestinationSystemLog_qualifiers[] =
{
    0,
};

MetaClass
__root_PG_InterOp_PG_ListenerDestinationSystemLog =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "PG_ListenerDestinationSystemLog",
    /* qualifiers */
    __root_PG_InterOp_PG_ListenerDestinationSystemLog_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_ListenerDestination,
    /* features */
    _PG_ListenerDestinationSystemLog_features,
};

//==============================================================================
//
// Class: PG_ListenerDestinationEmail
//
//==============================================================================

static const char*
_PG_ListenerDestinationEmail_MailTo_qualifiers[] =
{
    /* Required */
    "\050\001",
    0,
};

static MetaProperty
_PG_ListenerDestinationEmail_MailTo =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "MailTo",
    /* qualifiers */
    _PG_ListenerDestinationEmail_MailTo_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* value */
    0,
};

static const char*
_PG_ListenerDestinationEmail_MailCc_qualifiers[] =
{
    0,
};

static MetaProperty
_PG_ListenerDestinationEmail_MailCc =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "MailCc",
    /* qualifiers */
    _PG_ListenerDestinationEmail_MailCc_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* value */
    0,
};

static const char*
_PG_ListenerDestinationEmail_MailSubject_qualifiers[] =
{
    /* Required */
    "\050\001",
    0,
};

static MetaProperty
_PG_ListenerDestinationEmail_MailSubject =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "MailSubject",
    /* qualifiers */
    _PG_ListenerDestinationEmail_MailSubject_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_PG_ListenerDestinationEmail_features[] =
{
    (MetaFeature*)&_PG_ListenerDestinationEmail_MailTo,
    (MetaFeature*)&_PG_ListenerDestinationEmail_MailCc,
    (MetaFeature*)&_PG_ListenerDestinationEmail_MailSubject,
    0,
};

static const char*
__root_PG_InterOp_PG_ListenerDestinationEmail_qualifiers[] =
{
    0,
};

MetaClass
__root_PG_InterOp_PG_ListenerDestinationEmail =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "PG_ListenerDestinationEmail",
    /* qualifiers */
    __root_PG_InterOp_PG_ListenerDestinationEmail_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_ListenerDestination,
    /* features */
    _PG_ListenerDestinationEmail_features,
};

//==============================================================================
//
// Class: PG_ProviderModule
//
//==============================================================================

static const char*
_PG_ProviderModule_Name_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Override */
    "\044Name",
    0,
};

static MetaProperty
_PG_ProviderModule_Name =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Name",
    /* qualifiers */
    _PG_ProviderModule_Name_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_PG_ProviderModule_Vendor_qualifiers[] =
{
    /* Required */
    "\050\001",
    0,
};

static MetaProperty
_PG_ProviderModule_Vendor =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "Vendor",
    /* qualifiers */
    _PG_ProviderModule_Vendor_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_PG_ProviderModule_Version_qualifiers[] =
{
    /* Required */
    "\050\001",
    0,
};

static MetaProperty
_PG_ProviderModule_Version =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "Version",
    /* qualifiers */
    _PG_ProviderModule_Version_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_PG_ProviderModule_InterfaceType_qualifiers[] =
{
    /* Required */
    "\050\001",
    0,
};

static MetaProperty
_PG_ProviderModule_InterfaceType =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "InterfaceType",
    /* qualifiers */
    _PG_ProviderModule_InterfaceType_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_PG_ProviderModule_InterfaceVersion_qualifiers[] =
{
    /* Required */
    "\050\001",
    0,
};

static MetaProperty
_PG_ProviderModule_InterfaceVersion =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "InterfaceVersion",
    /* qualifiers */
    _PG_ProviderModule_InterfaceVersion_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_PG_ProviderModule_Location_qualifiers[] =
{
    /* Required */
    "\050\001",
    0,
};

static MetaProperty
_PG_ProviderModule_Location =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "Location",
    /* qualifiers */
    _PG_ProviderModule_Location_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_PG_ProviderModule_UserContext_qualifiers[] =
{
    /* ValueMap */
    "\061\000\0042\0003\0004\0005\000",
    /* Values */
    "\062\000\004Requestor\000Designated User\000Privileged User\000CIM Server\000",
    0,
};

static MetaProperty
_PG_ProviderModule_UserContext =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "UserContext",
    /* qualifiers */
    _PG_ProviderModule_UserContext_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_PG_ProviderModule_DesignatedUserContext_qualifiers[] =
{
    0,
};

static MetaProperty
_PG_ProviderModule_DesignatedUserContext =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "DesignatedUserContext",
    /* qualifiers */
    _PG_ProviderModule_DesignatedUserContext_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_PG_ProviderModule_OperationalStatus_qualifiers[] =
{
    /* ValueMap */
    "\061\000\0160\0001\0002\0003\0004\0005\0006\0007\0008\0009\00010\00011\00012\00013\000",
    /* Values */
    "\062\000\016Unknown\000Other\000OK\000Degraded\000Stressed\000Predictive Failure\000Error\000Non-Recoverable Error\000Starting\000Stopping\000Stopped\000In Service\000No Contact\000Lost Communication\000",
    /* ModelCorrespondence */
    "\036\000\001CIM_ManagedSystemElement.OtherStatusDescription\000",
    0,
};

static MetaProperty
_PG_ProviderModule_OperationalStatus =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "OperationalStatus",
    /* qualifiers */
    _PG_ProviderModule_OperationalStatus_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    0,
    /* value */
    0,
};

static const char*
_PG_ProviderModule_OtherStatusDescription_qualifiers[] =
{
    /* ModelCorrespondence */
    "\036\000\001CIM_ManagedSystemElement.OperationalStatus\000",
    0,
};

static MetaProperty
_PG_ProviderModule_OtherStatusDescription =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "OtherStatusDescription",
    /* qualifiers */
    _PG_ProviderModule_OtherStatusDescription_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_PG_ProviderModule_start_parameters[] =
{
    0,
};

static const char*
_PG_ProviderModule_start_qualifiers[] =
{
    0,
};

static MetaMethod
_PG_ProviderModule_start =
{
    /* flags */
    META_FLAG_METHOD,
    /* name */
    "PG_ProviderModule",
    /* qualifiers */
    _PG_ProviderModule_start_qualifiers,
    /* type */
    CIMTYPE_UINT32,
    /* parameters */
    _PG_ProviderModule_start_parameters,
};

static MetaFeature*
_PG_ProviderModule_stop_parameters[] =
{
    0,
};

static const char*
_PG_ProviderModule_stop_qualifiers[] =
{
    0,
};

static MetaMethod
_PG_ProviderModule_stop =
{
    /* flags */
    META_FLAG_METHOD,
    /* name */
    "PG_ProviderModule",
    /* qualifiers */
    _PG_ProviderModule_stop_qualifiers,
    /* type */
    CIMTYPE_UINT32,
    /* parameters */
    _PG_ProviderModule_stop_parameters,
};

static MetaFeature*
_PG_ProviderModule_features[] =
{
    (MetaFeature*)&_PG_ProviderModule_Name,
    (MetaFeature*)&_PG_ProviderModule_Vendor,
    (MetaFeature*)&_PG_ProviderModule_Version,
    (MetaFeature*)&_PG_ProviderModule_InterfaceType,
    (MetaFeature*)&_PG_ProviderModule_InterfaceVersion,
    (MetaFeature*)&_PG_ProviderModule_Location,
    (MetaFeature*)&_PG_ProviderModule_UserContext,
    (MetaFeature*)&_PG_ProviderModule_DesignatedUserContext,
    (MetaFeature*)&_PG_ProviderModule_OperationalStatus,
    (MetaFeature*)&_PG_ProviderModule_OtherStatusDescription,
    (MetaFeature*)&_PG_ProviderModule_start,
    (MetaFeature*)&_PG_ProviderModule_stop,
    0,
};

static const char*
__root_PG_InterOp_PG_ProviderModule_qualifiers[] =
{
    /* Version */
    "\0632.5.0",
    0,
};

MetaClass
__root_PG_InterOp_PG_ProviderModule =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "PG_ProviderModule",
    /* qualifiers */
    __root_PG_InterOp_PG_ProviderModule_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_LogicalElement,
    /* features */
    _PG_ProviderModule_features,
};

//==============================================================================
//
// Class: PG_Provider
//
//==============================================================================

static const char*
_PG_Provider_ProviderModuleName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Propagated */
    "\045PG_ProviderModule.Name",
    0,
};

static MetaProperty
_PG_Provider_ProviderModuleName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "ProviderModuleName",
    /* qualifiers */
    _PG_Provider_ProviderModuleName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_PG_Provider_Name_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Override */
    "\044Name",
    0,
};

static MetaProperty
_PG_Provider_Name =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Name",
    /* qualifiers */
    _PG_Provider_Name_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_PG_Provider_features[] =
{
    (MetaFeature*)&_PG_Provider_ProviderModuleName,
    (MetaFeature*)&_PG_Provider_Name,
    0,
};

static const char*
__root_PG_InterOp_PG_Provider_qualifiers[] =
{
    /* Version */
    "\0632.2.0",
    0,
};

MetaClass
__root_PG_InterOp_PG_Provider =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "PG_Provider",
    /* qualifiers */
    __root_PG_InterOp_PG_Provider_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_LogicalElement,
    /* features */
    _PG_Provider_features,
};

//==============================================================================
//
// Class: PG_Capabilities
//
//==============================================================================

static const char*
_PG_Capabilities_ProviderModuleName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Propagated */
    "\045PG_Provider.ProviderModuleName",
    0,
};

static MetaProperty
_PG_Capabilities_ProviderModuleName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "ProviderModuleName",
    /* qualifiers */
    _PG_Capabilities_ProviderModuleName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_PG_Capabilities_ProviderName_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Propagated */
    "\045PG_Provider.Name",
    0,
};

static MetaProperty
_PG_Capabilities_ProviderName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "ProviderName",
    /* qualifiers */
    _PG_Capabilities_ProviderName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_PG_Capabilities_CapabilityID_qualifiers[] =
{
    /* Key */
    "\025\001",
    0,
};

static MetaProperty
_PG_Capabilities_CapabilityID =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "CapabilityID",
    /* qualifiers */
    _PG_Capabilities_CapabilityID_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_PG_Capabilities_features[] =
{
    (MetaFeature*)&_PG_Capabilities_ProviderModuleName,
    (MetaFeature*)&_PG_Capabilities_ProviderName,
    (MetaFeature*)&_PG_Capabilities_CapabilityID,
    0,
};

static const char*
__root_PG_InterOp_PG_Capabilities_qualifiers[] =
{
    /* Abstract */
    "\002\001",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_PG_Capabilities =
{
    /* flags */
    META_FLAG_CLASS|META_FLAG_ABSTRACT,
    /* name */
    "PG_Capabilities",
    /* qualifiers */
    __root_PG_InterOp_PG_Capabilities_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_ManagedElement,
    /* features */
    _PG_Capabilities_features,
};

//==============================================================================
//
// Class: PG_CapabilitiesRegistration
//
//==============================================================================

static const char*
_PG_CapabilitiesRegistration_ProviderType_qualifiers[] =
{
    /* Required */
    "\050\001",
    /* ArrayType */
    "\005Indexed",
    /* ValueMap */
    "\061\000\0062\0003\0004\0005\0006\0007\000",
    /* Values */
    "\062\000\006Instance\000Association\000Indication\000Method\000IndicationConsumer\000InstanceQuery\000",
    0,
};

static MetaProperty
_PG_CapabilitiesRegistration_ProviderType =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "ProviderType",
    /* qualifiers */
    _PG_CapabilitiesRegistration_ProviderType_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    0,
    /* value */
    0,
};

static MetaFeature*
_PG_CapabilitiesRegistration_features[] =
{
    (MetaFeature*)&_PG_CapabilitiesRegistration_ProviderType,
    0,
};

static const char*
__root_PG_InterOp_PG_CapabilitiesRegistration_qualifiers[] =
{
    /* Abstract */
    "\002\001",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_PG_CapabilitiesRegistration =
{
    /* flags */
    META_FLAG_CLASS|META_FLAG_ABSTRACT,
    /* name */
    "PG_CapabilitiesRegistration",
    /* qualifiers */
    __root_PG_InterOp_PG_CapabilitiesRegistration_qualifiers,
    /* super */
    &__root_PG_InterOp_PG_Capabilities,
    /* features */
    _PG_CapabilitiesRegistration_features,
};

//==============================================================================
//
// Class: PG_ProviderCapabilities
//
//==============================================================================

static const char*
_PG_ProviderCapabilities_ClassName_qualifiers[] =
{
    /* Required */
    "\050\001",
    0,
};

static MetaProperty
_PG_ProviderCapabilities_ClassName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "ClassName",
    /* qualifiers */
    _PG_ProviderCapabilities_ClassName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_PG_ProviderCapabilities_Namespaces_qualifiers[] =
{
    /* Required */
    "\050\001",
    0,
};

static MetaProperty
_PG_ProviderCapabilities_Namespaces =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "Namespaces",
    /* qualifiers */
    _PG_ProviderCapabilities_Namespaces_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* value */
    0,
};

static const char*
_PG_ProviderCapabilities_SupportedProperties_qualifiers[] =
{
    /* ModelCorrespondence */
    "\036\000\001PG_ProviderCapabilities.ClassName\000",
    0,
};

static MetaProperty
_PG_ProviderCapabilities_SupportedProperties =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "SupportedProperties",
    /* qualifiers */
    _PG_ProviderCapabilities_SupportedProperties_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* value */
    0,
};

static const char*
_PG_ProviderCapabilities_SupportedMethods_qualifiers[] =
{
    /* ModelCorrespondence */
    "\036\000\001PG_ProviderCapabilities.ClassName \000",
    0,
};

static MetaProperty
_PG_ProviderCapabilities_SupportedMethods =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "SupportedMethods",
    /* qualifiers */
    _PG_ProviderCapabilities_SupportedMethods_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* value */
    0,
};

static MetaFeature*
_PG_ProviderCapabilities_features[] =
{
    (MetaFeature*)&_PG_ProviderCapabilities_ClassName,
    (MetaFeature*)&_PG_ProviderCapabilities_Namespaces,
    (MetaFeature*)&_PG_ProviderCapabilities_SupportedProperties,
    (MetaFeature*)&_PG_ProviderCapabilities_SupportedMethods,
    0,
};

static const char*
__root_PG_InterOp_PG_ProviderCapabilities_qualifiers[] =
{
    /* Version */
    "\0632.3.0",
    0,
};

MetaClass
__root_PG_InterOp_PG_ProviderCapabilities =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "PG_ProviderCapabilities",
    /* qualifiers */
    __root_PG_InterOp_PG_ProviderCapabilities_qualifiers,
    /* super */
    &__root_PG_InterOp_PG_CapabilitiesRegistration,
    /* features */
    _PG_ProviderCapabilities_features,
};

//==============================================================================
//
// Class: PG_ConsumerCapabilities
//
//==============================================================================

static const char*
_PG_ConsumerCapabilities_Destinations_qualifiers[] =
{
    /* Required */
    "\050\001",
    0,
};

static MetaProperty
_PG_ConsumerCapabilities_Destinations =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "Destinations",
    /* qualifiers */
    _PG_ConsumerCapabilities_Destinations_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* value */
    0,
};

static MetaFeature*
_PG_ConsumerCapabilities_features[] =
{
    (MetaFeature*)&_PG_ConsumerCapabilities_Destinations,
    0,
};

static const char*
__root_PG_InterOp_PG_ConsumerCapabilities_qualifiers[] =
{
    /* Version */
    "\0632.3.0",
    0,
};

MetaClass
__root_PG_InterOp_PG_ConsumerCapabilities =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "PG_ConsumerCapabilities",
    /* qualifiers */
    __root_PG_InterOp_PG_ConsumerCapabilities_qualifiers,
    /* super */
    &__root_PG_InterOp_PG_CapabilitiesRegistration,
    /* features */
    _PG_ConsumerCapabilities_features,
};

//==============================================================================
//
// Class: PG_ProviderCapabilitiesElements
//
//==============================================================================

static const char*
_PG_ProviderCapabilitiesElements_Antecedent_qualifiers[] =
{
    /* Key */
    "\025\001",
    /* Override */
    "\044Antecedent",
    /* Min */
    "\033\000\000\000\001",
    /* Max */
    "\027\000\000\000\001",
    0,
};

static MetaReference
_PG_ProviderCapabilitiesElements_Antecedent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Antecedent",
    /* qualifiers */
    _PG_ProviderCapabilitiesElements_Antecedent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_PG_Provider,
};

static const char*
_PG_ProviderCapabilitiesElements_Dependent_qualifiers[] =
{
    /* Override */
    "\044Dependent",
    /* Weak */
    "\064\001",
    0,
};

static MetaReference
_PG_ProviderCapabilitiesElements_Dependent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ|META_FLAG_WEAK,
    /* name */
    "Dependent",
    /* qualifiers */
    _PG_ProviderCapabilitiesElements_Dependent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_PG_CapabilitiesRegistration,
};

static MetaFeature*
_PG_ProviderCapabilitiesElements_features[] =
{
    (MetaFeature*)&_PG_ProviderCapabilitiesElements_Antecedent,
    (MetaFeature*)&_PG_ProviderCapabilitiesElements_Dependent,
    0,
};

static const char*
__root_PG_InterOp_PG_ProviderCapabilitiesElements_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* Version */
    "\0632.4.0",
    0,
};

MetaClass
__root_PG_InterOp_PG_ProviderCapabilitiesElements =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "PG_ProviderCapabilitiesElements",
    /* qualifiers */
    __root_PG_InterOp_PG_ProviderCapabilitiesElements_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_Dependency,
    /* features */
    _PG_ProviderCapabilitiesElements_features,
};

//==============================================================================
//
// Class: PG_ProviderModuleElements
//
//==============================================================================

static const char*
_PG_ProviderModuleElements_Antecedent_qualifiers[] =
{
    /* Override */
    "\044Antecedent",
    /* Min */
    "\033\000\000\000\001",
    /* Max */
    "\027\000\000\000\001",
    /* Key */
    "\025\001",
    0,
};

static MetaReference
_PG_ProviderModuleElements_Antecedent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_KEY|META_FLAG_READ,
    /* name */
    "Antecedent",
    /* qualifiers */
    _PG_ProviderModuleElements_Antecedent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_PG_ProviderModule,
};

static const char*
_PG_ProviderModuleElements_Dependent_qualifiers[] =
{
    /* Override */
    "\044Dependent",
    /* Weak */
    "\064\001",
    0,
};

static MetaReference
_PG_ProviderModuleElements_Dependent =
{
    /* flags */
    META_FLAG_REFERENCE|META_FLAG_READ|META_FLAG_WEAK,
    /* name */
    "Dependent",
    /* qualifiers */
    _PG_ProviderModuleElements_Dependent_qualifiers,
    /* subscript */
    -1,
    /* refId */
    &__root_PG_InterOp_PG_Provider,
};

static MetaFeature*
_PG_ProviderModuleElements_features[] =
{
    (MetaFeature*)&_PG_ProviderModuleElements_Antecedent,
    (MetaFeature*)&_PG_ProviderModuleElements_Dependent,
    0,
};

static const char*
__root_PG_InterOp_PG_ProviderModuleElements_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* Version */
    "\0632.0.0",
    0,
};

MetaClass
__root_PG_InterOp_PG_ProviderModuleElements =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "PG_ProviderModuleElements",
    /* qualifiers */
    __root_PG_InterOp_PG_ProviderModuleElements_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_Dependency,
    /* features */
    _PG_ProviderModuleElements_features,
};

//==============================================================================
//
// Class: PG_CIMXMLCommunicationMechanism
//
//==============================================================================

static const char*
_PG_CIMXMLCommunicationMechanism_namespaceType_qualifiers[] =
{
    /* Deprecated */
    "\013\000\001PG_CIMXMLCommunicationMechanism.namespaceAccessProtocol\000",
    0,
};

static MetaProperty
_PG_CIMXMLCommunicationMechanism_namespaceType =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "namespaceType",
    /* qualifiers */
    _PG_CIMXMLCommunicationMechanism_namespaceType_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_PG_CIMXMLCommunicationMechanism_namespaceAccessProtocol_qualifiers[] =
{
    /* ValueMap */
    "\061\000\0040\0002\0003\0004..100\000",
    /* Values */
    "\062\000\004Unknown\000http\000https\000Pegasus Reserved\000",
    0,
};

static MetaProperty
_PG_CIMXMLCommunicationMechanism_namespaceAccessProtocol =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "namespaceAccessProtocol",
    /* qualifiers */
    _PG_CIMXMLCommunicationMechanism_namespaceAccessProtocol_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_PG_CIMXMLCommunicationMechanism_IPAddress_qualifiers[] =
{
    0,
};

static MetaProperty
_PG_CIMXMLCommunicationMechanism_IPAddress =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "IPAddress",
    /* qualifiers */
    _PG_CIMXMLCommunicationMechanism_IPAddress_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_PG_CIMXMLCommunicationMechanism_features[] =
{
    (MetaFeature*)&_PG_CIMXMLCommunicationMechanism_namespaceType,
    (MetaFeature*)&_PG_CIMXMLCommunicationMechanism_namespaceAccessProtocol,
    (MetaFeature*)&_PG_CIMXMLCommunicationMechanism_IPAddress,
    0,
};

static const char*
__root_PG_InterOp_PG_CIMXMLCommunicationMechanism_qualifiers[] =
{
    /* Version */
    "\0632.1.0",
    0,
};

MetaClass
__root_PG_InterOp_PG_CIMXMLCommunicationMechanism =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "PG_CIMXMLCommunicationMechanism",
    /* qualifiers */
    __root_PG_InterOp_PG_CIMXMLCommunicationMechanism_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_CIMXMLCommunicationMechanism,
    /* features */
    _PG_CIMXMLCommunicationMechanism_features,
};

//==============================================================================
//
// Class: PG_NameSpace
//
//==============================================================================

static const char*
_PG_NameSpace_SchemaUpdatesAllowed_qualifiers[] =
{
    0,
};

static MetaProperty
_PG_NameSpace_SchemaUpdatesAllowed =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "SchemaUpdatesAllowed",
    /* qualifiers */
    _PG_NameSpace_SchemaUpdatesAllowed_qualifiers,
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* value */
    "\001",
};

static const char*
_PG_NameSpace_IsShareable_qualifiers[] =
{
    0,
};

static MetaProperty
_PG_NameSpace_IsShareable =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "IsShareable",
    /* qualifiers */
    _PG_NameSpace_IsShareable_qualifiers,
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* value */
    "\000",
};

static const char*
_PG_NameSpace_ParentNamespace_qualifiers[] =
{
    0,
};

static MetaProperty
_PG_NameSpace_ParentNamespace =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "ParentNamespace",
    /* qualifiers */
    _PG_NameSpace_ParentNamespace_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_PG_NameSpace_features[] =
{
    (MetaFeature*)&_PG_NameSpace_SchemaUpdatesAllowed,
    (MetaFeature*)&_PG_NameSpace_IsShareable,
    (MetaFeature*)&_PG_NameSpace_ParentNamespace,
    0,
};

static const char*
__root_PG_InterOp_PG_NameSpace_qualifiers[] =
{
    /* Version */
    "\0632.0.1",
    0,
};

MetaClass
__root_PG_InterOp_PG_NameSpace =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "PG_NameSpace",
    /* qualifiers */
    __root_PG_InterOp_PG_NameSpace_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_Namespace,
    /* features */
    _PG_NameSpace_features,
};

//==============================================================================
//
// Class: CIM_ComputerSystem
//
//==============================================================================

static const char*
_CIM_ComputerSystem_NameFormat_qualifiers[] =
{
    /* Override */
    "\044NameFormat",
    /* ValueMap */
    "\061\000\016Other\000IP\000Dial\000HID\000NWA\000HWA\000X25\000ISDN\000IPX\000DCC\000ICD\000E.164\000SNA\000OID/OSI\000",
    0,
};

static MetaProperty
_CIM_ComputerSystem_NameFormat =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "NameFormat",
    /* qualifiers */
    _CIM_ComputerSystem_NameFormat_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_ComputerSystem_OtherIdentifyingInfo_qualifiers[] =
{
    /* MaxLen */
    "\030\000\000\001\000",
    /* ArrayType */
    "\005Indexed",
    /* ModelCorrespondence */
    "\036\000\001CIM_ComputerSystem.IdentifyingDescriptions\000",
    0,
};

static MetaProperty
_CIM_ComputerSystem_OtherIdentifyingInfo =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "OtherIdentifyingInfo",
    /* qualifiers */
    _CIM_ComputerSystem_OtherIdentifyingInfo_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* value */
    0,
};

static const char*
_CIM_ComputerSystem_IdentifyingDescriptions_qualifiers[] =
{
    /* ArrayType */
    "\005Indexed",
    /* ModelCorrespondence */
    "\036\000\001CIM_ComputerSystem.OtherIdentifyingInfo\000",
    0,
};

static MetaProperty
_CIM_ComputerSystem_IdentifyingDescriptions =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "IdentifyingDescriptions",
    /* qualifiers */
    _CIM_ComputerSystem_IdentifyingDescriptions_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* value */
    0,
};

static const char*
_CIM_ComputerSystem_Dedicated_qualifiers[] =
{
    /* Values */
    "\062\000\017Not Dedicated\000Unknown\000Other\000Storage\000Router\000Switch\000Layer 3 Switch\000Central Office Switch\000Hub\000Access Server\000Firewall\000Print\000I/O\000Web Caching\000Management\000",
    0,
};

static MetaProperty
_CIM_ComputerSystem_Dedicated =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "Dedicated",
    /* qualifiers */
    _CIM_ComputerSystem_Dedicated_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    0,
    /* value */
    0,
};

static MetaFeature*
_CIM_ComputerSystem_features[] =
{
    (MetaFeature*)&_CIM_ComputerSystem_NameFormat,
    (MetaFeature*)&_CIM_ComputerSystem_OtherIdentifyingInfo,
    (MetaFeature*)&_CIM_ComputerSystem_IdentifyingDescriptions,
    (MetaFeature*)&_CIM_ComputerSystem_Dedicated,
    0,
};

static const char*
__root_PG_InterOp_CIM_ComputerSystem_qualifiers[] =
{
    /* Abstract */
    "\002\001",
    0,
};

MetaClass
__root_PG_InterOp_CIM_ComputerSystem =
{
    /* flags */
    META_FLAG_CLASS|META_FLAG_ABSTRACT,
    /* name */
    "CIM_ComputerSystem",
    /* qualifiers */
    __root_PG_InterOp_CIM_ComputerSystem_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_System,
    /* features */
    _CIM_ComputerSystem_features,
};

//==============================================================================
//
// Class: CIM_UnitaryComputerSystem
//
//==============================================================================

static const char*
_CIM_UnitaryComputerSystem_InitialLoadInfo_qualifiers[] =
{
    0,
};

static MetaProperty
_CIM_UnitaryComputerSystem_InitialLoadInfo =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "InitialLoadInfo",
    /* qualifiers */
    _CIM_UnitaryComputerSystem_InitialLoadInfo_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* value */
    0,
};

static const char*
_CIM_UnitaryComputerSystem_LastLoadInfo_qualifiers[] =
{
    /* MappingStrings */
    "\026\000\003MIB.IETF|HOST-RESOURCES-MIB.hrSystemInitialLoadDevice\000MIB.IETF|HOST-RESOURCES-MIB.hrSystemInitialLoadParameters\000MIF.DMTF|Host System|001.3\000",
    0,
};

static MetaProperty
_CIM_UnitaryComputerSystem_LastLoadInfo =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "LastLoadInfo",
    /* qualifiers */
    _CIM_UnitaryComputerSystem_LastLoadInfo_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_UnitaryComputerSystem_PowerManagementSupported_qualifiers[] =
{
    0,
};

static MetaProperty
_CIM_UnitaryComputerSystem_PowerManagementSupported =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "PowerManagementSupported",
    /* qualifiers */
    _CIM_UnitaryComputerSystem_PowerManagementSupported_qualifiers,
    /* type */
    CIMTYPE_BOOLEAN,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_UnitaryComputerSystem_PowerState_qualifiers[] =
{
    /* Values */
    "\062\000\012Unknown\000Full Power\000Power Save - Low Power Mode\000Power Save - Standby\000Power Save - Unknown\000Power Cycle\000Power Off\000Power Save - Warning\000Power Save - Hibernate\000Power Save - Soft Off\000",
    0,
};

static MetaProperty
_CIM_UnitaryComputerSystem_PowerState =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "PowerState",
    /* qualifiers */
    _CIM_UnitaryComputerSystem_PowerState_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_UnitaryComputerSystem_WakeUpType_qualifiers[] =
{
    /* Values */
    "\062\000\011Reserved\000Other\000Unknown\000APM Timer\000Modem Ring\000LAN Remote\000Power Switch\000PCI PME#\000A/C Power Restored\000",
    0,
};

static MetaProperty
_CIM_UnitaryComputerSystem_WakeUpType =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "WakeUpType",
    /* qualifiers */
    _CIM_UnitaryComputerSystem_WakeUpType_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_UnitaryComputerSystem_ResetCapability_qualifiers[] =
{
    /* ValueMap */
    "\061\000\0051\0002\0003\0004\0005\000",
    /* Values */
    "\062\000\005Other\000Unknown\000Disabled\000Enabled\000Not Implemented\000",
    /* MappingStrings */
    "\026\000\001MIF.DMTF|System Hardware Security|001.4\000",
    0,
};

static MetaProperty
_CIM_UnitaryComputerSystem_ResetCapability =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "ResetCapability",
    /* qualifiers */
    _CIM_UnitaryComputerSystem_ResetCapability_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_UnitaryComputerSystem_PowerManagementCapabilities_qualifiers[] =
{
    /* Values */
    "\062\000\010Unknown\000Not Supported\000Disabled\000Enabled\000Power Saving Modes Entered Automatically\000Power State Settable\000Power Cycling Supported\000Timed Power On Supported\000",
    /* MappingStrings */
    "\026\000\001MIF.DMTF|System Power Controls|001.2\000",
    0,
};

static MetaProperty
_CIM_UnitaryComputerSystem_PowerManagementCapabilities =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "PowerManagementCapabilities",
    /* qualifiers */
    _CIM_UnitaryComputerSystem_PowerManagementCapabilities_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    0,
    /* value */
    0,
};

static const char*
_CIM_UnitaryComputerSystem_SetPowerState_PowerState_qualifiers[] =
{
    /* IN */
    "\024\001",
    /* ValueMap */
    "\061\000\0101\0002\0003\0004\0005\0006\0007\0008\000",
    /* Values */
    "\062\000\010Full Power\000Power Save - Low Power Mode\000Power Save - Standby\000Power Save - Other\000Power Cycle\000Power Off\000Hibernate\000Soft Off\000",
    0,
};

static MetaProperty
_CIM_UnitaryComputerSystem_SetPowerState_PowerState =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_IN,
    /* name */
    "PowerState",
    /* qualifiers */
    _CIM_UnitaryComputerSystem_SetPowerState_PowerState_qualifiers,
    /* type */
    CIMTYPE_UINT32,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_CIM_UnitaryComputerSystem_SetPowerState_Time_qualifiers[] =
{
    /* IN */
    "\024\001",
    0,
};

static MetaProperty
_CIM_UnitaryComputerSystem_SetPowerState_Time =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_IN,
    /* name */
    "Time",
    /* qualifiers */
    _CIM_UnitaryComputerSystem_SetPowerState_Time_qualifiers,
    /* type */
    CIMTYPE_DATETIME,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_CIM_UnitaryComputerSystem_SetPowerState_parameters[] =
{
    (MetaFeature*)&_CIM_UnitaryComputerSystem_SetPowerState_PowerState,
    (MetaFeature*)&_CIM_UnitaryComputerSystem_SetPowerState_Time,
    0,
};

static const char*
_CIM_UnitaryComputerSystem_SetPowerState_qualifiers[] =
{
    0,
};

static MetaMethod
_CIM_UnitaryComputerSystem_SetPowerState =
{
    /* flags */
    META_FLAG_METHOD,
    /* name */
    "CIM_UnitaryComputerSystem",
    /* qualifiers */
    _CIM_UnitaryComputerSystem_SetPowerState_qualifiers,
    /* type */
    CIMTYPE_UINT32,
    /* parameters */
    _CIM_UnitaryComputerSystem_SetPowerState_parameters,
};

static MetaFeature*
_CIM_UnitaryComputerSystem_features[] =
{
    (MetaFeature*)&_CIM_UnitaryComputerSystem_InitialLoadInfo,
    (MetaFeature*)&_CIM_UnitaryComputerSystem_LastLoadInfo,
    (MetaFeature*)&_CIM_UnitaryComputerSystem_PowerManagementSupported,
    (MetaFeature*)&_CIM_UnitaryComputerSystem_PowerState,
    (MetaFeature*)&_CIM_UnitaryComputerSystem_WakeUpType,
    (MetaFeature*)&_CIM_UnitaryComputerSystem_ResetCapability,
    (MetaFeature*)&_CIM_UnitaryComputerSystem_PowerManagementCapabilities,
    (MetaFeature*)&_CIM_UnitaryComputerSystem_SetPowerState,
    0,
};

static const char*
__root_PG_InterOp_CIM_UnitaryComputerSystem_qualifiers[] =
{
    0,
};

MetaClass
__root_PG_InterOp_CIM_UnitaryComputerSystem =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "CIM_UnitaryComputerSystem",
    /* qualifiers */
    __root_PG_InterOp_CIM_UnitaryComputerSystem_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_ComputerSystem,
    /* features */
    _CIM_UnitaryComputerSystem_features,
};

//==============================================================================
//
// Class: PG_ComputerSystem
//
//==============================================================================

static const char*
_PG_ComputerSystem_PrimaryOwnerPager_qualifiers[] =
{
    /* MappingStrings */
    "\026\000\001MIF.DMTF|General Information|001.9\000",
    0,
};

static MetaProperty
_PG_ComputerSystem_PrimaryOwnerPager =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "PrimaryOwnerPager",
    /* qualifiers */
    _PG_ComputerSystem_PrimaryOwnerPager_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_PG_ComputerSystem_SecondaryOwnerName_qualifiers[] =
{
    /* MappingStrings */
    "\026\000\001MIF.DMTF|General Information|001.7\000",
    0,
};

static MetaProperty
_PG_ComputerSystem_SecondaryOwnerName =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "SecondaryOwnerName",
    /* qualifiers */
    _PG_ComputerSystem_SecondaryOwnerName_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_PG_ComputerSystem_SecondaryOwnerContact_qualifiers[] =
{
    /* MappingStrings */
    "\026\000\001MIF.DMTF|General Information|001.8\000",
    0,
};

static MetaProperty
_PG_ComputerSystem_SecondaryOwnerContact =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "SecondaryOwnerContact",
    /* qualifiers */
    _PG_ComputerSystem_SecondaryOwnerContact_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_PG_ComputerSystem_SecondaryOwnerPager_qualifiers[] =
{
    /* MappingStrings */
    "\026\000\001MIF.DMTF|General Information|001.10\000",
    0,
};

static MetaProperty
_PG_ComputerSystem_SecondaryOwnerPager =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "SecondaryOwnerPager",
    /* qualifiers */
    _PG_ComputerSystem_SecondaryOwnerPager_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_PG_ComputerSystem_SerialNumber_qualifiers[] =
{
    /* MappingStrings */
    "\026\000\001MIF.DMTF|General Information|001.13\000",
    0,
};

static MetaProperty
_PG_ComputerSystem_SerialNumber =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "SerialNumber",
    /* qualifiers */
    _PG_ComputerSystem_SerialNumber_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_PG_ComputerSystem_IdentificationNumber_qualifiers[] =
{
    /* MappingStrings */
    "\026\000\001MIF.DMTF|General Information|001.13\000",
    0,
};

static MetaProperty
_PG_ComputerSystem_IdentificationNumber =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "IdentificationNumber",
    /* qualifiers */
    _PG_ComputerSystem_IdentificationNumber_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static MetaFeature*
_PG_ComputerSystem_features[] =
{
    (MetaFeature*)&_PG_ComputerSystem_PrimaryOwnerPager,
    (MetaFeature*)&_PG_ComputerSystem_SecondaryOwnerName,
    (MetaFeature*)&_PG_ComputerSystem_SecondaryOwnerContact,
    (MetaFeature*)&_PG_ComputerSystem_SecondaryOwnerPager,
    (MetaFeature*)&_PG_ComputerSystem_SerialNumber,
    (MetaFeature*)&_PG_ComputerSystem_IdentificationNumber,
    0,
};

static const char*
__root_PG_InterOp_PG_ComputerSystem_qualifiers[] =
{
    /* Version */
    "\0632.2.0",
    0,
};

MetaClass
__root_PG_InterOp_PG_ComputerSystem =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "PG_ComputerSystem",
    /* qualifiers */
    __root_PG_InterOp_PG_ComputerSystem_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_UnitaryComputerSystem,
    /* features */
    _PG_ComputerSystem_features,
};

//==============================================================================
//
// Class: PG_ProviderProfileCapabilities
//
//==============================================================================

static const char*
_PG_ProviderProfileCapabilities_RegisteredProfile_qualifiers[] =
{
    /* Required */
    "\050\001",
    /* ValueMap */
    "\061\000\0200\0001\0002\0003\0004\0005\0006\0007\0008\0009\00010\00011\00012\00013\00014\000..\000",
    /* Values */
    "\062\000\020Other\000SNIA:Server\000SNIA:Security\000SNIA:Fabric\000SNIA:Switch\000SNIA:Extender\000SNIA:FC HBA\000SNIA:iSCSI Initiator\000SNIA:Host Discovered Resources\000SNIA:Array\000SNIA:Storage Virtualizer\000SNIA:Volume Management\000SNIA:Storage Library\000SNIA:NAS Head\000SNIA:Self-contained NAS System\000OpenPegasus Reserved\000",
    0,
};

static MetaProperty
_PG_ProviderProfileCapabilities_RegisteredProfile =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "RegisteredProfile",
    /* qualifiers */
    _PG_ProviderProfileCapabilities_RegisteredProfile_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_PG_ProviderProfileCapabilities_RegisteredSubProfiles_qualifiers[] =
{
    /* Required */
    "\050\001",
    /* ValueMap */
    "\061\000\0660\0001..1000\0001001\0001002\0001003\0001004\0001005\0001006\0001007\0001008\0001009\0001010\0001011\0001012\0001013\0001014\0001015\0001016\0001017\0001018\0001019\0001020\0001021\0001022\0001023\0001024\0001025\0001026\0001027\0001028\0001029\0001030\0001031\0001032\0001033\0001034\0001035\0001036\0001037\0001038\0001039\0001040\0001041\0001042\0001043\0001044\0001045\0001046\0001047\0001048\0001049\0001050\0001051\000..\000",
    /* Values */
    "\062\000\066Other\000OpenPegasus Reserved For Profiles\000SNIA:Access Points\000SNIA:Backend Ports\000SNIA:Blades\000SNIA:Block Server Performance\000SNIA:Block Storage Resource Ownership \000SNIA:Cascading\000SNIA:Cluster\000SNIA:Copy Services\000SNIA:Device Credentials\000SNIA:Disk Drive\000SNIA:Disk Drive Lite\000SNIA:Disk Partition\000SNIA:Disk Sparing\000SNIA:Enhanced Zoning and Enhanced Zoning Control\000SNIA:Extent Composition\000SNIA:Extent Mapping\000SNIA:Extra Capacity Set\000SNIA:Fabric Path Performance\000SNIA:FC Initiator Ports\000SNIA:FC Target Ports\000SNIA:FDMI\000SNIA:File Export Manipulation\000SNIA:File System Manipulation\000SNIA:Indication\000SNIA:iSCSI Initiator Ports\000SNIA:Job Control\000SNIA:Location\000SNIA:LUN Creation\000SNIA:LUN Mapping and Masking\000SNIA:Masking and Mapping\000SNIA:Multi System\000SNIA:Multiple Computer System\000SNIA:Object Manager Adapter\000SNIA:Pool Manipulation Capabilities and Settings\000SNIA:SCSI Multipath Management\000SNIA:Security Authorization\000SNIA:Security CredentialManagement\000SNIA:Security IdentityManagement\000SNIA:Software\000SNIA:Software Installation\000SNIA:SPI Initiator Ports\000SNIA:SPI Target Ports\000SNIA:Storage Library Capacity\000SNIA:Storage Library Element Counting\000SNIA:Storage Library InterLibraryPort Connection\000SNIA:Storage Library Limited Access Port Elements\000SNIA:Storage Library Media Movement\000SNIA:Storage Library Partitioned Library\000SNIA:Switch Configuration Data\000SNIA:Zone Control\000SNIA:iSCSI Target Ports\000OpenPegasus Reserved\000",
    0,
};

static MetaProperty
_PG_ProviderProfileCapabilities_RegisteredSubProfiles =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "RegisteredSubProfiles",
    /* qualifiers */
    _PG_ProviderProfileCapabilities_RegisteredSubProfiles_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    0,
    /* value */
    0,
};

static const char*
_PG_ProviderProfileCapabilities_ProfileVersion_qualifiers[] =
{
    /* Required */
    "\050\001",
    0,
};

static MetaProperty
_PG_ProviderProfileCapabilities_ProfileVersion =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "ProfileVersion",
    /* qualifiers */
    _PG_ProviderProfileCapabilities_ProfileVersion_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_PG_ProviderProfileCapabilities_SubProfileVersions_qualifiers[] =
{
    0,
};

static MetaProperty
_PG_ProviderProfileCapabilities_SubProfileVersions =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "SubProfileVersions",
    /* qualifiers */
    _PG_ProviderProfileCapabilities_SubProfileVersions_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* value */
    0,
};

static const char*
_PG_ProviderProfileCapabilities_OtherRegisteredProfile_qualifiers[] =
{
    0,
};

static MetaProperty
_PG_ProviderProfileCapabilities_OtherRegisteredProfile =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "OtherRegisteredProfile",
    /* qualifiers */
    _PG_ProviderProfileCapabilities_OtherRegisteredProfile_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_PG_ProviderProfileCapabilities_OtherProfileOrganization_qualifiers[] =
{
    0,
};

static MetaProperty
_PG_ProviderProfileCapabilities_OtherProfileOrganization =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "OtherProfileOrganization",
    /* qualifiers */
    _PG_ProviderProfileCapabilities_OtherProfileOrganization_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    -1,
    /* value */
    0,
};

static const char*
_PG_ProviderProfileCapabilities_OtherRegisteredSubProfiles_qualifiers[] =
{
    0,
};

static MetaProperty
_PG_ProviderProfileCapabilities_OtherRegisteredSubProfiles =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "OtherRegisteredSubProfiles",
    /* qualifiers */
    _PG_ProviderProfileCapabilities_OtherRegisteredSubProfiles_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* value */
    0,
};

static const char*
_PG_ProviderProfileCapabilities_OtherSubProfileOrganizations_qualifiers[] =
{
    0,
};

static MetaProperty
_PG_ProviderProfileCapabilities_OtherSubProfileOrganizations =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "OtherSubProfileOrganizations",
    /* qualifiers */
    _PG_ProviderProfileCapabilities_OtherSubProfileOrganizations_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* value */
    0,
};

static const char*
_PG_ProviderProfileCapabilities_ConformingElements_qualifiers[] =
{
    0,
};

static MetaProperty
_PG_ProviderProfileCapabilities_ConformingElements =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "ConformingElements",
    /* qualifiers */
    _PG_ProviderProfileCapabilities_ConformingElements_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* value */
    0,
};

static MetaFeature*
_PG_ProviderProfileCapabilities_features[] =
{
    (MetaFeature*)&_PG_ProviderProfileCapabilities_RegisteredProfile,
    (MetaFeature*)&_PG_ProviderProfileCapabilities_RegisteredSubProfiles,
    (MetaFeature*)&_PG_ProviderProfileCapabilities_ProfileVersion,
    (MetaFeature*)&_PG_ProviderProfileCapabilities_SubProfileVersions,
    (MetaFeature*)&_PG_ProviderProfileCapabilities_OtherRegisteredProfile,
    (MetaFeature*)&_PG_ProviderProfileCapabilities_OtherProfileOrganization,
    (MetaFeature*)&_PG_ProviderProfileCapabilities_OtherRegisteredSubProfiles,
    (MetaFeature*)&_PG_ProviderProfileCapabilities_OtherSubProfileOrganizations,
    (MetaFeature*)&_PG_ProviderProfileCapabilities_ConformingElements,
    0,
};

static const char*
__root_PG_InterOp_PG_ProviderProfileCapabilities_qualifiers[] =
{
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_PG_ProviderProfileCapabilities =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "PG_ProviderProfileCapabilities",
    /* qualifiers */
    __root_PG_InterOp_PG_ProviderProfileCapabilities_qualifiers,
    /* super */
    &__root_PG_InterOp_PG_Capabilities,
    /* features */
    _PG_ProviderProfileCapabilities_features,
};

//==============================================================================
//
// Class: PG_ProviderReferencedProfiles
//
//==============================================================================

static const char*
_PG_ProviderReferencedProfiles_RegisteredProfiles_qualifiers[] =
{
    /* Required */
    "\050\001",
    /* ValueMap */
    "\061\000\1040\0001\0002\0003\0004\0005\0006\0007\0008\0009\00010\00011\00012\00013\00014\00015..1000\0001001\0001002\0001003\0001004\0001005\0001006\0001007\0001008\0001009\0001010\0001011\0001012\0001013\0001014\0001015\0001016\0001017\0001018\0001019\0001020\0001021\0001022\0001023\0001024\0001025\0001026\0001027\0001028\0001029\0001030\0001031\0001032\0001033\0001034\0001035\0001036\0001037\0001038\0001039\0001040\0001041\0001042\0001043\0001044\0001045\0001046\0001047\0001048\0001049\0001050\0001051\000..\000",
    /* Values */
    "\062\000\104Other\000SNIA:Server\000SNIA:Security\000SNIA:Fabric\000SNIA:Switch\000SNIA:Extender\000SNIA:FC HBA\000SNIA:iSCSI Initiator\000SNIA:Host Discovered Resources\000SNIA:Array\000SNIA:Storage Virtualizer\000SNIA:Volume Management\000SNIA:Storage Library\000SNIA:NAS Head\000SNIA:Self-contained NAS System\000OpenPegasus Reserved For Profiles\000SNIA:Access Points\000SNIA:Backend Ports\000SNIA:Blades\000SNIA:Block Server Performance\000SNIA:Block Storage Resource Ownership \000SNIA:Cascading\000SNIA:Cluster\000SNIA:Copy Services\000SNIA:Device Credentials\000SNIA:Disk Drive\000SNIA:Disk Drive Lite\000SNIA:Disk Partition\000SNIA:Disk Sparing\000SNIA:Enhanced Zoning and Enhanced Zoning Control\000SNIA:Extent Composition\000SNIA:Extent Mapping\000SNIA:Extra Capacity Set\000SNIA:Fabric Path Performance\000SNIA:FC Initiator Ports\000SNIA:FC Target Ports\000SNIA:FDMI\000SNIA:File Export Manipulation\000SNIA:File System Manipulation\000SNIA:Indication\000SNIA:iSCSI Initiator Ports\000SNIA:Job Control\000SNIA:Location\000SNIA:LUN Creation\000SNIA:LUN Mapping and Masking\000SNIA:Masking and Mapping\000SNIA:Multi System\000SNIA:Multiple Computer System\000SNIA:Object Manager Adapter\000SNIA:Pool Manipulation Capabilities and Settings\000SNIA:SCSI Multipath Management\000SNIA:Security Authorization\000SNIA:Security CredentialManagement\000SNIA:Security IdentityManagement\000SNIA:Software\000SNIA:Software Installation\000SNIA:SPI Initiator Ports\000SNIA:SPI Target Ports\000SNIA:Storage Library Capacity\000SNIA:Storage Library Element Counting\000SNIA:Storage Library InterLibraryPort Connection\000SNIA:Storage Library Limited Access Port Elements\000SNIA:Storage Library Media Movement\000SNIA:Storage Library Partitioned Library\000SNIA:Switch Configuration Data\000SNIA:Zone Control\000SNIA:iSCSI Target Ports\000OpenPegasus Reserved\000",
    0,
};

static MetaProperty
_PG_ProviderReferencedProfiles_RegisteredProfiles =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "RegisteredProfiles",
    /* qualifiers */
    _PG_ProviderReferencedProfiles_RegisteredProfiles_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    0,
    /* value */
    0,
};

static const char*
_PG_ProviderReferencedProfiles_DependentProfiles_qualifiers[] =
{
    /* Required */
    "\050\001",
    /* ValueMap */
    "\061\000\1040\0001\0002\0003\0004\0005\0006\0007\0008\0009\00010\00011\00012\00013\00014\00015..1000\0001001\0001002\0001003\0001004\0001005\0001006\0001007\0001008\0001009\0001010\0001011\0001012\0001013\0001014\0001015\0001016\0001017\0001018\0001019\0001020\0001021\0001022\0001023\0001024\0001025\0001026\0001027\0001028\0001029\0001030\0001031\0001032\0001033\0001034\0001035\0001036\0001037\0001038\0001039\0001040\0001041\0001042\0001043\0001044\0001045\0001046\0001047\0001048\0001049\0001050\0001051\000..\000",
    /* Values */
    "\062\000\104Other\000SNIA:Server\000SNIA:Security\000SNIA:Fabric\000SNIA:Switch\000SNIA:Extender\000SNIA:FC HBA\000SNIA:iSCSI Initiator\000SNIA:Host Discovered Resources\000SNIA:Array\000SNIA:Storage Virtualizer\000SNIA:Volume Management\000SNIA:Storage Library\000SNIA:NAS Head\000SNIA:Self-contained NAS System\000OpenPegasus Reserved For Profiles\000SNIA:Access Points\000SNIA:Backend Ports\000SNIA:Blades\000SNIA:Block Server Performance\000SNIA:Block Storage Resource Ownership \000SNIA:Cascading\000SNIA:Cluster\000SNIA:Copy Services\000SNIA:Device Credentials\000SNIA:Disk Drive\000SNIA:Disk Drive Lite\000SNIA:Disk Partition\000SNIA:Disk Sparing\000SNIA:Enhanced Zoning and Enhanced Zoning Control\000SNIA:Extent Composition\000SNIA:Extent Mapping\000SNIA:Extra Capacity Set\000SNIA:Fabric Path Performance\000SNIA:FC Initiator Ports\000SNIA:FC Target Ports\000SNIA:FDMI\000SNIA:File Export Manipulation\000SNIA:File System Manipulation\000SNIA:Indication\000SNIA:iSCSI Initiator Ports\000SNIA:Job Control\000SNIA:Location\000SNIA:LUN Creation\000SNIA:LUN Mapping and Masking\000SNIA:Masking and Mapping\000SNIA:Multi System\000SNIA:Multiple Computer System\000SNIA:Object Manager Adapter\000SNIA:Pool Manipulation Capabilities and Settings\000SNIA:SCSI Multipath Management\000SNIA:Security Authorization\000SNIA:Security CredentialManagement\000SNIA:Security IdentityManagement\000SNIA:Software\000SNIA:Software Installation\000SNIA:SPI Initiator Ports\000SNIA:SPI Target Ports\000SNIA:Storage Library Capacity\000SNIA:Storage Library Element Counting\000SNIA:Storage Library InterLibraryPort Connection\000SNIA:Storage Library Limited Access Port Elements\000SNIA:Storage Library Media Movement\000SNIA:Storage Library Partitioned Library\000SNIA:Switch Configuration Data\000SNIA:Zone Control\000SNIA:iSCSI Target Ports\000OpenPegasus Reserved\000",
    0,
};

static MetaProperty
_PG_ProviderReferencedProfiles_DependentProfiles =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "DependentProfiles",
    /* qualifiers */
    _PG_ProviderReferencedProfiles_DependentProfiles_qualifiers,
    /* type */
    CIMTYPE_UINT16,
    /* subscript */
    0,
    /* value */
    0,
};

static const char*
_PG_ProviderReferencedProfiles_RegisteredProfileVersions_qualifiers[] =
{
    /* Required */
    "\050\001",
    0,
};

static MetaProperty
_PG_ProviderReferencedProfiles_RegisteredProfileVersions =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "RegisteredProfileVersions",
    /* qualifiers */
    _PG_ProviderReferencedProfiles_RegisteredProfileVersions_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* value */
    0,
};

static const char*
_PG_ProviderReferencedProfiles_DependentProfileVersions_qualifiers[] =
{
    /* Required */
    "\050\001",
    0,
};

static MetaProperty
_PG_ProviderReferencedProfiles_DependentProfileVersions =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ|META_FLAG_REQUIRED,
    /* name */
    "DependentProfileVersions",
    /* qualifiers */
    _PG_ProviderReferencedProfiles_DependentProfileVersions_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* value */
    0,
};

static const char*
_PG_ProviderReferencedProfiles_OtherRegisteredProfiles_qualifiers[] =
{
    0,
};

static MetaProperty
_PG_ProviderReferencedProfiles_OtherRegisteredProfiles =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "OtherRegisteredProfiles",
    /* qualifiers */
    _PG_ProviderReferencedProfiles_OtherRegisteredProfiles_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* value */
    0,
};

static const char*
_PG_ProviderReferencedProfiles_OtherDependentProfiles_qualifiers[] =
{
    0,
};

static MetaProperty
_PG_ProviderReferencedProfiles_OtherDependentProfiles =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "OtherDependentProfiles",
    /* qualifiers */
    _PG_ProviderReferencedProfiles_OtherDependentProfiles_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* value */
    0,
};

static const char*
_PG_ProviderReferencedProfiles_OtherRegisteredProfileOrganizations_qualifiers[] =
{
    0,
};

static MetaProperty
_PG_ProviderReferencedProfiles_OtherRegisteredProfileOrganizations =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "OtherRegisteredProfileOrganizations",
    /* qualifiers */
    _PG_ProviderReferencedProfiles_OtherRegisteredProfileOrganizations_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* value */
    0,
};

static const char*
_PG_ProviderReferencedProfiles_OtherDependentProfileOrganizations_qualifiers[] =
{
    0,
};

static MetaProperty
_PG_ProviderReferencedProfiles_OtherDependentProfileOrganizations =
{
    /* flags */
    META_FLAG_PROPERTY|META_FLAG_READ,
    /* name */
    "OtherDependentProfileOrganizations",
    /* qualifiers */
    _PG_ProviderReferencedProfiles_OtherDependentProfileOrganizations_qualifiers,
    /* type */
    CIMTYPE_STRING,
    /* subscript */
    0,
    /* value */
    0,
};

static MetaFeature*
_PG_ProviderReferencedProfiles_features[] =
{
    (MetaFeature*)&_PG_ProviderReferencedProfiles_RegisteredProfiles,
    (MetaFeature*)&_PG_ProviderReferencedProfiles_DependentProfiles,
    (MetaFeature*)&_PG_ProviderReferencedProfiles_RegisteredProfileVersions,
    (MetaFeature*)&_PG_ProviderReferencedProfiles_DependentProfileVersions,
    (MetaFeature*)&_PG_ProviderReferencedProfiles_OtherRegisteredProfiles,
    (MetaFeature*)&_PG_ProviderReferencedProfiles_OtherDependentProfiles,
    (MetaFeature*)&_PG_ProviderReferencedProfiles_OtherRegisteredProfileOrganizations,
    (MetaFeature*)&_PG_ProviderReferencedProfiles_OtherDependentProfileOrganizations,
    0,
};

static const char*
__root_PG_InterOp_PG_ProviderReferencedProfiles_qualifiers[] =
{
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_PG_ProviderReferencedProfiles =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "PG_ProviderReferencedProfiles",
    /* qualifiers */
    __root_PG_InterOp_PG_ProviderReferencedProfiles_qualifiers,
    /* super */
    &__root_PG_InterOp_PG_Capabilities,
    /* features */
    _PG_ProviderReferencedProfiles_features,
};

//==============================================================================
//
// Class: PG_RegisteredProfile
//
//==============================================================================

static MetaFeature*
_PG_RegisteredProfile_features[] =
{
    0,
};

static const char*
__root_PG_InterOp_PG_RegisteredProfile_qualifiers[] =
{
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_PG_RegisteredProfile =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "PG_RegisteredProfile",
    /* qualifiers */
    __root_PG_InterOp_PG_RegisteredProfile_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_RegisteredProfile,
    /* features */
    _PG_RegisteredProfile_features,
};

//==============================================================================
//
// Class: PG_RegisteredSubProfile
//
//==============================================================================

static MetaFeature*
_PG_RegisteredSubProfile_features[] =
{
    0,
};

static const char*
__root_PG_InterOp_PG_RegisteredSubProfile_qualifiers[] =
{
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_PG_RegisteredSubProfile =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "PG_RegisteredSubProfile",
    /* qualifiers */
    __root_PG_InterOp_PG_RegisteredSubProfile_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_RegisteredSubProfile,
    /* features */
    _PG_RegisteredSubProfile_features,
};

//==============================================================================
//
// Class: PG_ReferencedProfile
//
//==============================================================================

static MetaFeature*
_PG_ReferencedProfile_features[] =
{
    0,
};

static const char*
__root_PG_InterOp_PG_ReferencedProfile_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_PG_ReferencedProfile =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "PG_ReferencedProfile",
    /* qualifiers */
    __root_PG_InterOp_PG_ReferencedProfile_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_ReferencedProfile,
    /* features */
    _PG_ReferencedProfile_features,
};

//==============================================================================
//
// Class: PG_ElementConformsToProfile
//
//==============================================================================

static MetaFeature*
_PG_ElementConformsToProfile_features[] =
{
    0,
};

static const char*
__root_PG_InterOp_PG_ElementConformsToProfile_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_PG_ElementConformsToProfile =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "PG_ElementConformsToProfile",
    /* qualifiers */
    __root_PG_InterOp_PG_ElementConformsToProfile_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_ElementConformsToProfile,
    /* features */
    _PG_ElementConformsToProfile_features,
};

//==============================================================================
//
// Class: PG_SubProfileRequiresProfile
//
//==============================================================================

static MetaFeature*
_PG_SubProfileRequiresProfile_features[] =
{
    0,
};

static const char*
__root_PG_InterOp_PG_SubProfileRequiresProfile_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_PG_SubProfileRequiresProfile =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "PG_SubProfileRequiresProfile",
    /* qualifiers */
    __root_PG_InterOp_PG_SubProfileRequiresProfile_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_SubProfileRequiresProfile,
    /* features */
    _PG_SubProfileRequiresProfile_features,
};

//==============================================================================
//
// Class: PG_SoftwareIdentity
//
//==============================================================================

static MetaFeature*
_PG_SoftwareIdentity_features[] =
{
    0,
};

static const char*
__root_PG_InterOp_PG_SoftwareIdentity_qualifiers[] =
{
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_PG_SoftwareIdentity =
{
    /* flags */
    META_FLAG_CLASS,
    /* name */
    "PG_SoftwareIdentity",
    /* qualifiers */
    __root_PG_InterOp_PG_SoftwareIdentity_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_SoftwareIdentity,
    /* features */
    _PG_SoftwareIdentity_features,
};

//==============================================================================
//
// Class: PG_ElementSoftwareIdentity
//
//==============================================================================

static MetaFeature*
_PG_ElementSoftwareIdentity_features[] =
{
    0,
};

static const char*
__root_PG_InterOp_PG_ElementSoftwareIdentity_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_PG_ElementSoftwareIdentity =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "PG_ElementSoftwareIdentity",
    /* qualifiers */
    __root_PG_InterOp_PG_ElementSoftwareIdentity_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_ElementSoftwareIdentity,
    /* features */
    _PG_ElementSoftwareIdentity_features,
};

//==============================================================================
//
// Class: PG_ObjectManager
//
//==============================================================================

static MetaFeature*
_PG_ObjectManager_features[] =
{
    0,
};

static const char*
__root_PG_InterOp_PG_ObjectManager_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_PG_ObjectManager =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "PG_ObjectManager",
    /* qualifiers */
    __root_PG_InterOp_PG_ObjectManager_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_ObjectManager,
    /* features */
    _PG_ObjectManager_features,
};

//==============================================================================
//
// Class: PG_CommMechanismForManager
//
//==============================================================================

static MetaFeature*
_PG_CommMechanismForManager_features[] =
{
    0,
};

static const char*
__root_PG_InterOp_PG_CommMechanismForManager_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_PG_CommMechanismForManager =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "PG_CommMechanismForManager",
    /* qualifiers */
    __root_PG_InterOp_PG_CommMechanismForManager_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_CommMechanismForManager,
    /* features */
    _PG_CommMechanismForManager_features,
};

//==============================================================================
//
// Class: PG_NamespaceInManager
//
//==============================================================================

static MetaFeature*
_PG_NamespaceInManager_features[] =
{
    0,
};

static const char*
__root_PG_InterOp_PG_NamespaceInManager_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_PG_NamespaceInManager =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "PG_NamespaceInManager",
    /* qualifiers */
    __root_PG_InterOp_PG_NamespaceInManager_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_NamespaceInManager,
    /* features */
    _PG_NamespaceInManager_features,
};

//==============================================================================
//
// Class: PG_HostedObjectManager
//
//==============================================================================

static MetaFeature*
_PG_HostedObjectManager_features[] =
{
    0,
};

static const char*
__root_PG_InterOp_PG_HostedObjectManager_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_PG_HostedObjectManager =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "PG_HostedObjectManager",
    /* qualifiers */
    __root_PG_InterOp_PG_HostedObjectManager_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_HostedService,
    /* features */
    _PG_HostedObjectManager_features,
};

//==============================================================================
//
// Class: PG_HostedAccessPoint
//
//==============================================================================

static MetaFeature*
_PG_HostedAccessPoint_features[] =
{
    0,
};

static const char*
__root_PG_InterOp_PG_HostedAccessPoint_qualifiers[] =
{
    /* ASSOCIATION */
    "\000\001",
    /* Version */
    "\0632.6.0",
    0,
};

MetaClass
__root_PG_InterOp_PG_HostedAccessPoint =
{
    /* flags */
    META_FLAG_ASSOCIATION,
    /* name */
    "PG_HostedAccessPoint",
    /* qualifiers */
    __root_PG_InterOp_PG_HostedAccessPoint_qualifiers,
    /* super */
    &__root_PG_InterOp_CIM_HostedAccessPoint,
    /* features */
    _PG_HostedAccessPoint_features,
};

//==============================================================================
//
// Qualifier array
//
//==============================================================================

static MetaQualifierDecl*
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

static MetaClass*
_classes[] =
{
    &__root_PG_InterOp_CIM_ManagedElement,
    &__root_PG_InterOp_CIM_Setting,
    &__root_PG_InterOp_CIM_SettingContext,
    &__root_PG_InterOp_CIM_SettingData,
    &__root_PG_InterOp_CIM_Capabilities,
    &__root_PG_InterOp_CIM_Collection,
    &__root_PG_InterOp_CIM_CollectionOfMSEs,
    &__root_PG_InterOp_CIM_Component,
    &__root_PG_InterOp_CIM_ConcreteCollection,
    &__root_PG_InterOp_CIM_ConcreteComponent,
    &__root_PG_InterOp_CIM_Configuration,
    &__root_PG_InterOp_CIM_ConfigurationComponent,
    &__root_PG_InterOp_CIM_Dependency,
    &__root_PG_InterOp_CIM_DependencyContext,
    &__root_PG_InterOp_CIM_ElementCapabilities,
    &__root_PG_InterOp_CIM_EnabledLogicalElementCapabilities,
    &__root_PG_InterOp_CIM_FRU,
    &__root_PG_InterOp_CIM_HostedDependency,
    &__root_PG_InterOp_CIM_LocalizationCapabilities,
    &__root_PG_InterOp_CIM_Location,
    &__root_PG_InterOp_CIM_LogicalIdentity,
    &__root_PG_InterOp_CIM_ManagedSystemElement,
    &__root_PG_InterOp_CIM_MemberOfCollection,
    &__root_PG_InterOp_CIM_MethodParameters,
    &__root_PG_InterOp_CIM_OrderedMemberOfCollection,
    &__root_PG_InterOp_CIM_ParametersForMethod,
    &__root_PG_InterOp_CIM_ParameterValueSources,
    &__root_PG_InterOp_CIM_PhysicalElement,
    &__root_PG_InterOp_CIM_PowerManagementCapabilities,
    &__root_PG_InterOp_CIM_Product,
    &__root_PG_InterOp_CIM_ProductComponent,
    &__root_PG_InterOp_CIM_ProductFRU,
    &__root_PG_InterOp_CIM_ProductParentChild,
    &__root_PG_InterOp_CIM_ProductPhysicalComponent,
    &__root_PG_InterOp_CIM_ProductPhysicalElements,
    &__root_PG_InterOp_CIM_ProductProductDependency,
    &__root_PG_InterOp_CIM_Profile,
    &__root_PG_InterOp_CIM_ReplaceableProductFRU,
    &__root_PG_InterOp_CIM_Spared,
    &__root_PG_InterOp_CIM_StatisticalData,
    &__root_PG_InterOp_CIM_StatisticalInformation,
    &__root_PG_InterOp_CIM_StatisticalSetting,
    &__root_PG_InterOp_CIM_Statistics,
    &__root_PG_InterOp_CIM_SupportAccess,
    &__root_PG_InterOp_CIM_Synchronized,
    &__root_PG_InterOp_CIM_SystemConfiguration,
    &__root_PG_InterOp_CIM_SystemSetting,
    &__root_PG_InterOp_CIM_SystemSettingContext,
    &__root_PG_InterOp_CIM_SystemSpecificCollection,
    &__root_PG_InterOp_CIM_SystemStatisticalInformation,
    &__root_PG_InterOp_CIM_CollectedCollections,
    &__root_PG_InterOp_CIM_CollectedMSEs,
    &__root_PG_InterOp_CIM_CollectionConfiguration,
    &__root_PG_InterOp_CIM_CollectionSetting,
    &__root_PG_InterOp_CIM_CompatibleProduct,
    &__root_PG_InterOp_CIM_ConcreteDependency,
    &__root_PG_InterOp_CIM_ConcreteIdentity,
    &__root_PG_InterOp_CIM_ContainedLocation,
    &__root_PG_InterOp_CIM_DeviceStatisticalInformation,
    &__root_PG_InterOp_CIM_ElementConfiguration,
    &__root_PG_InterOp_CIM_ElementLocation,
    &__root_PG_InterOp_CIM_ElementProfile,
    &__root_PG_InterOp_CIM_ElementSetting,
    &__root_PG_InterOp_CIM_ElementSettingData,
    &__root_PG_InterOp_CIM_ElementStatisticalData,
    &__root_PG_InterOp_CIM_FRUIncludesProduct,
    &__root_PG_InterOp_CIM_FRUPhysicalElements,
    &__root_PG_InterOp_CIM_LogicalElement,
    &__root_PG_InterOp_CIM_PhysicalElementLocation,
    &__root_PG_InterOp_CIM_PhysicalStatisticalInformation,
    &__root_PG_InterOp_CIM_PhysicalStatistics,
    &__root_PG_InterOp_CIM_ProductSupport,
    &__root_PG_InterOp_CIM_RedundancyGroup,
    &__root_PG_InterOp_CIM_RedundancySet,
    &__root_PG_InterOp_CIM_RelatedStatisticalData,
    &__root_PG_InterOp_CIM_RelatedStatistics,
    &__root_PG_InterOp_CIM_SAPStatisticalInformation,
    &__root_PG_InterOp_CIM_ScopedSettingData,
    &__root_PG_InterOp_CIM_ServiceStatisticalInformation,
    &__root_PG_InterOp_CIM_SettingAssociatedToCapabilities,
    &__root_PG_InterOp_CIM_SoftwareIdentity,
    &__root_PG_InterOp_CIM_SpareGroup,
    &__root_PG_InterOp_CIM_StatisticsCollection,
    &__root_PG_InterOp_CIM_StorageRedundancyGroup,
    &__root_PG_InterOp_CIM_StorageRedundancySet,
    &__root_PG_InterOp_CIM_ActsAsSpare,
    &__root_PG_InterOp_CIM_DefaultSetting,
    &__root_PG_InterOp_CIM_ElementSoftwareIdentity,
    &__root_PG_InterOp_CIM_ExtraCapacityGroup,
    &__root_PG_InterOp_CIM_IsSpare,
    &__root_PG_InterOp_CIM_Job,
    &__root_PG_InterOp_CIM_ProductSoftwareComponent,
    &__root_PG_InterOp_CIM_RedundancyComponent,
    &__root_PG_InterOp_CIM_ScopedSetting,
    &__root_PG_InterOp_CIM_ConcreteJob,
    &__root_PG_InterOp_CIM_EnabledLogicalElement,
    &__root_PG_InterOp_CIM_LogicalDevice,
    &__root_PG_InterOp_CIM_Realizes,
    &__root_PG_InterOp_CIM_Service,
    &__root_PG_InterOp_CIM_ServiceAccessPoint,
    &__root_PG_InterOp_CIM_ServiceAccessURI,
    &__root_PG_InterOp_CIM_ServiceAffectsElement,
    &__root_PG_InterOp_CIM_ServiceAvailableToElement,
    &__root_PG_InterOp_CIM_ServiceComponent,
    &__root_PG_InterOp_CIM_ServiceSAPDependency,
    &__root_PG_InterOp_CIM_ServiceStatistics,
    &__root_PG_InterOp_CIM_StorageExtent,
    &__root_PG_InterOp_CIM_System,
    &__root_PG_InterOp_CIM_SystemComponent,
    &__root_PG_InterOp_CIM_SystemDevice,
    &__root_PG_InterOp_CIM_SystemPackaging,
    &__root_PG_InterOp_CIM_SystemStatistics,
    &__root_PG_InterOp_CIM_AdminDomain,
    &__root_PG_InterOp_CIM_BasedOn,
    &__root_PG_InterOp_CIM_ConfigurationForSystem,
    &__root_PG_InterOp_CIM_ContainedDomain,
    &__root_PG_InterOp_CIM_DeviceSAPImplementation,
    &__root_PG_InterOp_CIM_DeviceServiceImplementation,
    &__root_PG_InterOp_CIM_DeviceStatistics,
    &__root_PG_InterOp_CIM_ExtentRedundancyComponent,
    &__root_PG_InterOp_CIM_HostedAccessPoint,
    &__root_PG_InterOp_CIM_HostedCollection,
    &__root_PG_InterOp_CIM_HostedService,
    &__root_PG_InterOp_CIM_InstalledSoftwareIdentity,
    &__root_PG_InterOp_CIM_PowerManagementService,
    &__root_PG_InterOp_CIM_ProductServiceComponent,
    &__root_PG_InterOp_CIM_ProtocolEndpoint,
    &__root_PG_InterOp_CIM_ProvidesServiceToElement,
    &__root_PG_InterOp_CIM_RemoteServiceAccessPoint,
    &__root_PG_InterOp_CIM_SAPAvailableForElement,
    &__root_PG_InterOp_CIM_SAPSAPDependency,
    &__root_PG_InterOp_CIM_SAPStatistics,
    &__root_PG_InterOp_CIM_ServiceAccessBySAP,
    &__root_PG_InterOp_CIM_ServiceServiceDependency,
    &__root_PG_InterOp_CIM_SettingForSystem,
    &__root_PG_InterOp_CIM_ActiveConnection,
    &__root_PG_InterOp_CIM_BindsTo,
    &__root_PG_InterOp_CIM_ProvidesEndpoint,
    &__root_PG_InterOp_CIM_RemotePort,
    &__root_PG_InterOp_CIM_Indication,
    &__root_PG_InterOp_CIM_ClassIndication,
    &__root_PG_InterOp_CIM_ClassModification,
    &__root_PG_InterOp_CIM_IndicationFilter,
    &__root_PG_InterOp_CIM_InstIndication,
    &__root_PG_InterOp_CIM_InstMethodCall,
    &__root_PG_InterOp_CIM_InstModification,
    &__root_PG_InterOp_CIM_InstRead,
    &__root_PG_InterOp_CIM_ListenerDestination,
    &__root_PG_InterOp_CIM_ListenerDestinationCIMXML,
    &__root_PG_InterOp_CIM_ProcessIndication,
    &__root_PG_InterOp_CIM_SNMPTrapIndication,
    &__root_PG_InterOp_CIM_AlertIndication,
    &__root_PG_InterOp_CIM_AlertInstIndication,
    &__root_PG_InterOp_CIM_ClassCreation,
    &__root_PG_InterOp_CIM_ClassDeletion,
    &__root_PG_InterOp_CIM_IndicationHandler,
    &__root_PG_InterOp_CIM_IndicationHandlerCIMXML,
    &__root_PG_InterOp_CIM_IndicationSubscription,
    &__root_PG_InterOp_CIM_InstCreation,
    &__root_PG_InterOp_CIM_InstDeletion,
    &__root_PG_InterOp_CIM_ThresholdIndication,
    &__root_PG_InterOp_CIM_FormattedIndicationSubscription,
    &__root_PG_InterOp_CIM_Namespace,
    &__root_PG_InterOp_CIM_RegisteredProfile,
    &__root_PG_InterOp_CIM_RegisteredSubProfile,
    &__root_PG_InterOp_CIM_SystemIdentification,
    &__root_PG_InterOp_CIM_SystemInNamespace,
    &__root_PG_InterOp_CIM_CIMOMStatisticalData,
    &__root_PG_InterOp_CIM_ElementConformsToProfile,
    &__root_PG_InterOp_CIM_ReferencedProfile,
    &__root_PG_InterOp_CIM_SubProfileRequiresProfile,
    &__root_PG_InterOp_CIM_WBEMService,
    &__root_PG_InterOp_CIM_IdentificationOfManagedSystem,
    &__root_PG_InterOp_CIM_ObjectManager,
    &__root_PG_InterOp_CIM_ObjectManagerAdapter,
    &__root_PG_InterOp_CIM_ObjectManagerCommunicationMechanism,
    &__root_PG_InterOp_CIM_ProtocolAdapter,
    &__root_PG_InterOp_CIM_CIMXMLCommunicationMechanism,
    &__root_PG_InterOp_CIM_CommMechanismForAdapter,
    &__root_PG_InterOp_CIM_CommMechanismForManager,
    &__root_PG_InterOp_CIM_CommMechanismForObjectManagerAdapter,
    &__root_PG_InterOp_CIM_NamespaceInManager,
    &__root_PG_InterOp_PG_IndicationHandlerSNMPMapper,
    &__root_PG_InterOp_PG_ListenerDestinationSystemLog,
    &__root_PG_InterOp_PG_ListenerDestinationEmail,
    &__root_PG_InterOp_PG_ProviderModule,
    &__root_PG_InterOp_PG_Provider,
    &__root_PG_InterOp_PG_Capabilities,
    &__root_PG_InterOp_PG_CapabilitiesRegistration,
    &__root_PG_InterOp_PG_ProviderCapabilities,
    &__root_PG_InterOp_PG_ConsumerCapabilities,
    &__root_PG_InterOp_PG_ProviderCapabilitiesElements,
    &__root_PG_InterOp_PG_ProviderModuleElements,
    &__root_PG_InterOp_PG_CIMXMLCommunicationMechanism,
    &__root_PG_InterOp_PG_NameSpace,
    &__root_PG_InterOp_CIM_ComputerSystem,
    &__root_PG_InterOp_CIM_UnitaryComputerSystem,
    &__root_PG_InterOp_PG_ComputerSystem,
    &__root_PG_InterOp_PG_ProviderProfileCapabilities,
    &__root_PG_InterOp_PG_ProviderReferencedProfiles,
    &__root_PG_InterOp_PG_RegisteredProfile,
    &__root_PG_InterOp_PG_RegisteredSubProfile,
    &__root_PG_InterOp_PG_ReferencedProfile,
    &__root_PG_InterOp_PG_ElementConformsToProfile,
    &__root_PG_InterOp_PG_SubProfileRequiresProfile,
    &__root_PG_InterOp_PG_SoftwareIdentity,
    &__root_PG_InterOp_PG_ElementSoftwareIdentity,
    &__root_PG_InterOp_PG_ObjectManager,
    &__root_PG_InterOp_PG_CommMechanismForManager,
    &__root_PG_InterOp_PG_NamespaceInManager,
    &__root_PG_InterOp_PG_HostedObjectManager,
    &__root_PG_InterOp_PG_HostedAccessPoint,
    0,
};

const MetaNameSpace root_PG_InterOp_namespace =
{
    "root/PG_InterOp",
    _qualifiers,
    _classes,
};

PEGASUS_NAMESPACE_END
