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
%token <strValue> STRING_LITERAL
%token <intValue> EQ
%token <intValue> NE
%token <intValue> LT
%token <intValue> LE
%token <intValue> GT
%token <intValue> GE
%token <intValue> TOK_SELECT
%token <intValue> WHERE
%token <intValue> FROM
%token <strValue> TOK_IDENTIFIER
%token <intValue> NOT
%token <intValue> OR
%token <intValue> AND
%token <intValue> ISA
%token <intValue> WQL_TRUE
%token <intValue> WQL_FALSE

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

%left OR
%left AND
%nonassoc NOT

/*
**==============================================================================
**
** The grammar itself.
**
**==============================================================================
*/

%%

/*
**------------------------------------------------------------------------------
**
** start rule:
**
**------------------------------------------------------------------------------
*/

start
    : selectStatement
    {
    }

/* BOOKMARK */

/*
**------------------------------------------------------------------------------
**
** Select Statement
**
**------------------------------------------------------------------------------
*/

selectStatement
    : TOK_SELECT propertyListOrStar fromClass
    {

    } 
    | TOK_SELECT propertyListOrStar fromClass whereClause
    {

    }

fromClass : FROM className
    {
	$$ = $2;
    }

className : TOK_IDENTIFIER
    {
	$$ = $1
    }

whereClause : WHERE expression
    {

    }

propertyListOrStar 
    : propertyList
    {

    }
    | '*'
    {

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
    : expression OR expression
    {

    }
    | expression AND expression
    {

    }
    | NOT expression
    {

    }
    | '(' expression ')'
    {

    }
    | expressionTerm
    {

    }

expressionTerm
    : property LT constant
    {

    }
    | property GT constant
    {

    }
    | property LE constant
    {

    }
    | property GE constant
    {

    }
    | property EQ constant
    {

    }
    | property NE constant
    {

    }
    | constant LT property
    {

    }
    | constant GT property
    {

    }
    | constant LE property
    {

    }
    | constant GE property
    {

    }
    | constant EQ property
    {

    }
    | constant NE property
    {

    }
    | function LT constant
    {

    }
    | function GT constant
    {

    }
    | function LE constant
    {

    }
    | function GE constant
    {

    }
    | function EQ constant
    {

    }
    | function NE constant
    {

    }
    | constant LT function
    {

    }
    | constant GT function
    {

    }
    | constant LE function
    {

    }
    | constant GE function
    {

    }
    | constant EQ function
    {

    }
    | constant NE function
    {

    }
    | className ISA className
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
    | STRING_LITERAL
    {

    }

%%

int WQL_error(char* errorMessage)
{
    fprintf(stderr, "WQL_error: %s\n", errorMessage);
    return -1;
}
