%{
#include <Pegasus/Common/Config.h>
#include "CQLParserState.h"
#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include "memobjs.h"
#include "CQLYACC.h"


%}
SIGN [+-]
BINARY_DIGIT [01]
HEX_DIGIT [A-Fa-f0-9]
HEX_IDENT 0[Xx]
POSITIVE_DECIMAL_DIGIT [1-9]
DECIMAL_DIGIT [0-9]
BLANK [ \t]
IDENT_CHAR [A-Za-z_\x80-\xFF]

A [Aa]
B [Bb]
C [Cc]
D [Dd]
E [Ee]
F [Ff]
G [Gg]
H [Hh]
I [Ii]
J [Jj]
K [Kk]
L [Ll]
M [Mm]
N [Nn]
O [Oo]
P [Pp]
Q [Qq]
R [Rr]
S [Ss]
T [Tt]
U [Uu]
V [Vv]
W [Ww]
X [Xx]
Y [Yy]
Z [Zz]

%%

{S}{E}{L}{E}{C}{T} {

    printf("LEX: %s [SELECT] ", yytext);
    return SELECT;
}

{F}{R}{O}{M} {

    printf("LEX: %s [FROM] ", yytext);
    return FROM;
}

{W}{H}{E}{R}{E} {

    printf("LEX: %s [WHERE] ", yytext);
    return WHERE;
}

{A}{N}{Y} {

    printf("LEX: %s [ANY] ", yytext);
    return ANY;
}

{A}{S} {
                                                                                                                                                             
    printf("LEX: %s [AS] ", yytext);
    return AS;
}

{A}{S}{C} {
                                                                                                                                                             
    printf("LEX: %s [ASC] ", yytext);
    return ASC;
}

{B}{Y} {
                                                                                                                                                             
    printf("LEX: %s [BY] ", yytext);
    return BY;
}

{D}{E}{S}{C} {
                                                                                                                                                             
    printf("LEX: %s [DESC] ", yytext);
    return DESC;
}


{D}{I}{S}{T}{I}{N}{C}{T} {
                                                                                                                                                             
    printf("LEX: %s [DISTINCT] ", yytext);
    return DISTINCT;
}

{E}{V}{E}{R}{Y} {
                                                                                                                                                             
    printf("LEX: %s [EVERY] ", yytext);
    return EVERY;
}

{F}{I}{R}{S}{T} {
                                                                                                                                                             
    printf("LEX: %s [FIRST] ", yytext);
    return FIRST;
}

{I}{N} {
                                                                                                                                                             
    printf("LEX: %s [IN] ", yytext);
    return IN;
}

{I}{S} {
                                                                                                                                                             
    printf("LEX: %s [IS] ", yytext);
    return IS;
}

{I}{S}{A} {
                                                                                                                                                             
    printf("LEX: %s [ISA] ", yytext);
    return ISA;
}

{L}{I}{K}{E} {
                                                                                                                                                             
    printf("LEX: %s [LIKE] ", yytext);
    return LIKE;
}

{O}{R}{D}{E}{R} {
                                                                                                                                                             
    printf("LEX: %s [ORDER] ", yytext);
    return ORDER;
}

{S}{A}{T}{I}{S}{F}{I}{E}{S} {
                                                                                                                                                             
    printf("LEX: %s [SATISFIES] ", yytext);
    return SATISFIES;
}

{S}{C}{O}{P}{I}{N}{G}{T}{Y}{P}{E} {
                                                                                                                                                             
    printf("LEX: %s [SCOPINGTYPE] ", yytext);
    return SCOPINGTYPE;
}

{T}{R}{U}{E} {

    printf("LEX: %s [TRUE] ", yytext);
    return TRUE;
}

{F}{A}{L}{S}{E} {

    printf("LEX: %s [FALSE] ", yytext);
    return FALSE;
}

{N}{U}{L}{L} {

    printf("LEX: %s [_NULL] ", yytext);
    return _NULL;
}

{N}{O}{T} {

    printf("LEX: %s [NOT] ", yytext);
    return NOT;
}

{A}{N}{D} {

    printf("LEX: %s [AND] ", yytext);
    return AND;
}

{O}{R} {

    printf("LEX: %s [OR] ", yytext);
    return OR;
}

\.{IDENT_CHAR}*\:\:{IDENT_CHAR}*[.]? {
    printf("LEX: %s [SCOPED_PROPERTY] ", yytext);
    return SCOPED_PROPERTY;
}

[-]{1}[0][Xx]{HEX_DIGIT}{HEX_DIGIT}* {
    printf("LEX: %s [NEGATIVE_HEXADECIMAL] ", yytext);
    /* copy the hex value */
    {
        size_t n = strlen(yytext);
	if(CQL_lval.strValue)
		delete [] CQL_lval.strValue;
        CQL_lval.strValue = new char[n + 1];
        memcpy(CQL_lval.strValue, yytext, n);
        CQL_lval.strValue[n] = '\0';
    }
    return NEGATIVE_HEXADECIMAL;
}

[+]?[0][Xx]{HEX_DIGIT}{HEX_DIGIT}* {
    printf("LEX: %s [HEXADECIMAL] ", yytext);
    /* copy the hex value */
    {
        size_t n = strlen(yytext);
        if(CQL_lval.strValue)
                delete [] CQL_lval.strValue;
        CQL_lval.strValue = new char[n + 1];
        memcpy(CQL_lval.strValue, yytext, n);
        CQL_lval.strValue[n] = '\0';
    }
    return HEXADECIMAL;
}

[-]{1}{BINARY_DIGIT}{BINARY_DIGIT}*{B} {
    printf("LEX: %s [NEGATIVE_BINARY] ", yytext);
    /* copy the bin value */ 
    {
        size_t n = strlen(yytext);
	if(CQL_lval.strValue)
                delete [] CQL_lval.strValue;
        CQL_lval.strValue = new char[n + 1];
        memcpy(CQL_lval.strValue, yytext, n);
        CQL_lval.strValue[n] = '\0';
    }
    return NEGATIVE_BINARY;
}

[+]?{BINARY_DIGIT}{BINARY_DIGIT}*{B} {
    printf("LEX: %s [BINARY] ", yytext);
    /* copy the bin value */
    {
        size_t n = strlen(yytext);
        if(CQL_lval.strValue)
                delete [] CQL_lval.strValue;
        CQL_lval.strValue = new char[n + 1];
        memcpy(CQL_lval.strValue, yytext, n);
        CQL_lval.strValue[n] = '\0';
    }
    return BINARY;
}


[-]{1}{POSITIVE_DECIMAL_DIGIT}{DECIMAL_DIGIT}* {

    printf("LEX: %s [NEGATIVE_INTEGER] ", yytext);
    size_t n = strlen(yytext);
    if(CQL_lval.strValue)
	    delete [] CQL_lval.strValue;
    CQL_lval.strValue = new char[n + 1];
    memcpy(CQL_lval.strValue, yytext, n);
    CQL_lval.strValue[n] = '\0';

    /*CQL_lval.intValue = strtol(yytext, (char**)0, 10);*/
    return NEGATIVE_INTEGER;
}

[+]?{POSITIVE_DECIMAL_DIGIT}{DECIMAL_DIGIT}* {
                                                                                                                                       
    printf("LEX: %s [INTEGER] ", yytext);
    size_t n = strlen(yytext);
    if(CQL_lval.strValue)
            delete [] CQL_lval.strValue;
    CQL_lval.strValue = new char[n + 1];
    memcpy(CQL_lval.strValue, yytext, n);
    CQL_lval.strValue[n] = '\0';
                                                                                                                                       
    /*CQL_lval.intValue = strtol(yytext, (char**)0, 10);*/
    return INTEGER;
}


[+-]?0 {

    printf("LEX: %s [INTEGER] ", yytext);
    size_t n = strlen(yytext);
    if(CQL_lval.strValue)
	    delete [] CQL_lval.strValue;
    CQL_lval.strValue = new char[n + 1];
    memcpy(CQL_lval.strValue, yytext, n);
    CQL_lval.strValue[n] = '\0';

    return INTEGER;
}

[-]{1}{DECIMAL_DIGIT}*\.{DECIMAL_DIGIT}+([eE][+-]?{DECIMAL_DIGIT}+)? {

    printf("LEX: %s [NEGATIVE_REAL] ", yytext);
    size_t n = strlen(yytext);
    if(CQL_lval.strValue)
            delete [] CQL_lval.strValue;
    CQL_lval.strValue = new char[n + 1];
    memcpy(CQL_lval.strValue, yytext, n);
    CQL_lval.strValue[n] = '\0';

    /*CQL_lval.realValue = strtod((char*)yytext, (char**)0);*/
    return NEGATIVE_REAL;
}

[+]?{DECIMAL_DIGIT}*\.{DECIMAL_DIGIT}+([eE][+-]?{DECIMAL_DIGIT}+)? {
                                                                                                                                       
    printf("LEX: %s [REAL] ", yytext);
    size_t n = strlen(yytext);
    if(CQL_lval.strValue)
            delete [] CQL_lval.strValue;
    CQL_lval.strValue = new char[n + 1];
    memcpy(CQL_lval.strValue, yytext, n);
    CQL_lval.strValue[n] = '\0';
                                                                                                                                       
    /*CQL_lval.realValue = strtod((char*)yytext, (char**)0);*/
    return REAL;
}


\'[^\'\n]*\' {

    /* ATTN-B: handle long literals by using yyinput(). */
    /* ATTN-B: Handle expansion of special characters */

    printf("LEX: %s [STRING] ", yytext);

    /* Copy the string (but remove the surrounding quotes */

    {
	size_t n = strlen(yytext) - 2;
	if(CQL_lval.strValue)
		delete [] CQL_lval.strValue;
	CQL_lval.strValue = new char[n + 1];
	memcpy(CQL_lval.strValue, yytext + 1, n);
	CQL_lval.strValue[n] = '\0';
    }

    return STRING_LITERAL;
}

\'[^\'\n]*$ {

    printf("Unterminated string");
}

\*{1} {
   printf("LEX: [STAR] ");
   return STAR;
}

\/{1} {
   printf("LEX: [DIV] ");
   return DIV;
}

\+{1} {
   printf("LEX: [PLUS] ");
   return PLUS;
}

\-{1} {
   printf("LEX: [MINUS] ");
   return MINUS;
}

\,{1} {
   printf("LEX: [COMMA] ");
   return COMMA;
}

\.{2} {
    printf("LEX: [DOTDOT] ");
    return DOTDOT;
}

\.{1} {
    printf("LEX: [DOT] ");
    return DOT;
}

\[{1} {
    printf("LEX: [LBRKT] ");
    return LBRKT;
}

\]{1} {
    printf("LEX: [RBRKT] ");
    return RBRKT;
}

\({1} {
    printf("LEX: [LPAR] ");
    return LPAR;
}

\){1} {
    printf("LEX: [RPAR] ");
    return RPAR;
}

"::" {
    printf("LEX: %s [SCOPE] ", yytext);
    return SCOPE;
}

"||" {
    printf("LEX: %s [DBL_PIPE] ", yytext);
    return DBL_PIPE;
}

"_" {
    printf("LEX: %s [UNDERSCORE] ", yytext);
    return UNDERSCORE;
}

"=" { 
    printf("LEX: %s [EQ] ", yytext);
    return EQ; 
}

"!=" { 

    printf("LEX: %s [NE] ", yytext);
    return NE; 
}

"<=" { 

    printf("LEX: %s [LE] ", yytext);
    return LE; 
}

"<" { 

    printf("LEX: %s [LT] ", yytext);
    return LT; 
}

">=" { 

    printf("LEX: %s [GE] ", yytext);
    return GE; 
}

">" { 

    printf("LEX: %s [GT] ", yytext);
    return GT; 
}

{IDENT_CHAR}({IDENT_CHAR}|{DECIMAL_DIGIT})*  {

    printf("LEX: %s [IDENTIFIER] ", yytext);

    {
	size_t n = strlen(yytext);
	if(CQL_lval.strValue)
		delete [] CQL_lval.strValue;
	CQL_lval.strValue = new char[n + 1];
	memcpy(CQL_lval.strValue, yytext, n);
	CQL_lval.strValue[n] = '\0';
    }

    return IDENTIFIER;
}

{BLANK}+ {

    /* Ignore blanks */
}

\n {
	return 0;
   }

<<EOF>> {
             exit(0);
        }

. {
    CQL_lval.strValue = 0;
    printf("LEX::UNEXPECTED_CHAR\n");
    return UNEXPECTED_CHAR;
}

%%
extern "C" int CQL_wrap()
{
    return 1;
}


