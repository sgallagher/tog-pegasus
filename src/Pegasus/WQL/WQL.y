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
%token <intValue> TOK_NULL

%token <intValue> TOK_EQ
%token <intValue> TOK_NE
%token <intValue> TOK_LT
%token <intValue> TOK_LE
%token <intValue> TOK_GT
%token <intValue> TOK_GE

%token <intValue> TOK_NOT
%token <intValue> TOK_OR
%token <intValue> TOK_AND
%token <intValue> TOK_IS

%token <strValue> TOK_IDENTIFIER
%token <intValue> TOK_SELECT
%token <intValue> TOK_WHERE
%token <intValue> TOK_FROM

%token <strValue> TOK_UNEXPECTED_CHAR

%type <nodeValue> propertyName
%type <nodeValue> propertyList
%type <nodeValue> predicate
%type <nodeValue> comparisonPredicate
%type <nodeValue> comparisonTerm
%type <nodeValue> nullPredicate
%type <nodeValue> searchCondition
%type <nodeValue> fromClause
%type <nodeValue> whereClause
%type <nodeValue> selectStatement
%type <nodeValue> selectList
%type <nodeValue> selectExpression
%type <strValue> className

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
	printf("YACC: start\n");
    }

selectStatement
    : TOK_SELECT selectList selectExpression
    {

    }

selectList
    : '*'
    {

    }
    | propertyList
    {

    }

propertyList 
    : propertyName
    {

    }
    | propertyList ',' propertyName
    {

    }

selectExpression
    : fromClause whereClause
    {

    }
    | fromClause
    {

    }

fromClause
    : TOK_FROM className
    {

    }

whereClause 
    : TOK_WHERE searchCondition
    {

    }

searchCondition 
    : searchCondition TOK_OR searchCondition
    {

    }
    | searchCondition TOK_AND searchCondition
    {

    }
    | TOK_NOT searchCondition
    {

    }
    | '(' searchCondition ')'
    {

    }
    | predicate
    {

    }
    | predicate TOK_IS truthValue
    {

    }
    | predicate TOK_IS TOK_NOT truthValue
    {

    }

predicate
    : comparisonPredicate
    {

    }
    | nullPredicate
    {

    }

comparisonPredicate
    : comparisonTerm TOK_LT comparisonTerm 
    {

    }
    | comparisonTerm TOK_GT comparisonTerm
    {

    }
    | comparisonTerm TOK_LE comparisonTerm
    {

    }
    | comparisonTerm TOK_GE comparisonTerm
    {

    }
    | comparisonTerm TOK_EQ comparisonTerm
    {

    }
    | comparisonTerm TOK_NE comparisonTerm
    {

    }

nullPredicate
    : comparisonTerm TOK_IS TOK_NULL
    {

    }
    | comparisonTerm TOK_IS TOK_NOT TOK_NULL
    {

    }

truthValue 
    : TOK_TRUE 
    {

    }
    | TOK_FALSE
    {

    }

propertyName 
    : TOK_IDENTIFIER
    {

    }

className : TOK_IDENTIFIER
    {

    }

comparisonTerm
    : propertyName
    {

    }
    | TOK_INTEGER
    {

    }
    | TOK_DOUBLE
    {

    }
    | TOK_STRING
    {

    }
    | truthValue
    {
    }

%%

int WQL_error(char* errorMessage)
{
    fprintf(stderr, "WQL_error: %s\n", errorMessage);
    return -1;
}
