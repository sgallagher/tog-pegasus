/*
**==============================================================================
**
** Includes
**
**==============================================================================
*/

%{

#include <string.h>
#include <stdlib.h>

#ifdef PEGASUS_OS_TYPE_WINDOWS
# include <malloc.h>
#endif

#if defined(PEGASUS_COMPILER_ACC) && defined(PEGASUS_OS_HPUX)
# include <alloca.h>
#endif

#if 0
# define WQL_TRACE(X) printf(X)
#else
# define WQL_TRACE(X)
#endif

extern int WQL_lex();
extern int WQL_error(char*);

%}

/*
**==============================================================================
**
** Union used to pass tokens from Lexer to this Parser.
**
**==============================================================================
*/

%union 
{
   int intValue;
   double doubleValue;
   char* strValue;
   void* nodeValue;
}

/*
**==============================================================================
**
** Tokens, types, and associative rules.
**
**==============================================================================
*/

%token <intValue> TOK_INTEGER
%token <doubleValue> TOK_DOUBLE
%token <strValue> TOK_STRING
%token <intValue> TOK_TRUE
%token <intValue> TOK_FALSE

%token <intValue> TOK_EQ
%token <intValue> TOK_NE
%token <intValue> TOK_LT
%token <intValue> TOK_LE
%token <intValue> TOK_GT
%token <intValue> TOK_GE

%token <intValue> TOK_NOT
%token <intValue> TOK_OR
%token <intValue> TOK_AND
%token <intValue> TOK_ISA

%token <strValue> TOK_IDENTIFIER
%token <intValue> TOK_SELECT
%token <intValue> TOK_WHERE
%token <intValue> TOK_FROM

%token <strValue> TOK_UNEXPECTED_CHAR

%type <nodeValue> constant
%type <nodeValue> property
%type <nodeValue> propertyListOrStar
%type <nodeValue> propertyList
%type <nodeValue> expressionTerm
%type <nodeValue> expression
%type <nodeValue> whereClause
%type <strValue> fromClass
%type <nodeValue> selectStatement
%type <strValue> className
%type <nodeValue> function
%type <nodeValue> functionParameterList
%type <nodeValue> functionParameter

%left TOK_OR
%left TOK_AND
%nonassoc TOK_NOT

%%

/*
**==============================================================================
**
** The grammar itself.
**
**==============================================================================
*/

start
    : selectStatement
    {
	WQL_TRACE(("YACC: start: selectStatement\n"));
    }

selectStatement
    : TOK_SELECT propertyListOrStar fromClass
    {
	WQL_TRACE(("YACC: selectStatement\n"));
    } 
    | TOK_SELECT propertyListOrStar fromClass whereClause
    {
	WQL_TRACE(("YACC: selectStatement\n"));
    }

fromClass : TOK_FROM className
    {
	WQL_TRACE(("YACC: fromClass : TOK_FROM %s\n", $2));
	$$ = $2;
    }

className : TOK_IDENTIFIER
    {
	WQL_TRACE(("YACC: className : %s\n", $1));
	$$ = $1
    }

whereClause : TOK_WHERE expression
    {
	WQL_TRACE(("YACC: whereClause : TOK_WHERE expression\n"));
    }

propertyListOrStar 
    : propertyList
    {

    }
    | '*'
    {
	WQL_TRACE(("YACC: propertyListOrStar: '*'\n"));
    }

propertyList : property
    {

    }
    | propertyList ',' property
    {

    }

property 
    : TOK_IDENTIFIER
    {

    }
    | TOK_IDENTIFIER '.' TOK_IDENTIFIER
    {

    }

expression 
    : expression TOK_OR expression
    {

    }
    | expression TOK_AND expression
    {

    }
    | TOK_NOT expression
    {

    }
    | '(' expression ')'
    {

    }
    | expressionTerm
    {

    }

expressionTerm
    : property TOK_LT constant
    {

    }
    | property TOK_GT constant
    {

    }
    | property TOK_LE constant
    {

    }
    | property TOK_GE constant
    {

    }
    | property TOK_EQ constant
    {

    }
    | property TOK_NE constant
    {

    }
    | constant TOK_LT property
    {

    }
    | constant TOK_GT property
    {

    }
    | constant TOK_LE property
    {

    }
    | constant TOK_GE property
    {

    }
    | constant TOK_EQ property
    {

    }
    | constant TOK_NE property
    {

    }
    | function TOK_LT constant
    {

    }
    | function TOK_GT constant
    {

    }
    | function TOK_LE constant
    {

    }
    | function TOK_GE constant
    {

    }
    | function TOK_EQ constant
    {

    }
    | function TOK_NE constant
    {

    }
    | constant TOK_LT function
    {

    }
    | constant TOK_GT function
    {

    }
    | constant TOK_LE function
    {

    }
    | constant TOK_GE function
    {

    }
    | constant TOK_EQ function
    {

    }
    | constant TOK_NE function
    {

    }
    | className TOK_ISA className
    {

    }

function 
    : TOK_IDENTIFIER '(' ')'
    {

    }
    | TOK_IDENTIFIER '(' functionParameterList ')'
    {

    }

functionParameterList
    : functionParameter
    {

    }
    | functionParameterList ',' functionParameter
    {

    }

functionParameter
    : property
    | constant
    | function
    ;

constant 
    : TOK_INTEGER
    {

    }
    | TOK_DOUBLE
    {

    }
    | TOK_STRING
    {
	WQL_TRACE(("YACC: TOK_STRING: %s\n", $1));
	$$ = $1;
    }

%%

int WQL_error(char* errorMessage)
{
    fprintf(stderr, "WQL_error: %s\n", errorMessage);
    return -1;
}
