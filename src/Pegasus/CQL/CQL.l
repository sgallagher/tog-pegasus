%option never-interactive
%{
extern int CQLInput(char* buffer, int& numRead, int numRequested);
extern int CQL_error(const char*);

#ifdef CQLINPUT
#define YY_INPUT(BUF, NREAD, NREQUESTED) CQLInput(BUF, NREAD, NREQUESTED)
#endif

#ifdef CQL_DEBUG_LEXER
#define DEBUG_LEX 1
#else
#define DEBUG_LEX 0
#endif

#include <Pegasus/Common/Config.h>
#include "CQLParserState.h"
#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include "CQLObjects.h"
#include "CQLYACC.h"
PEGASUS_NAMESPACE_BEGIN
                                                                                
extern CQLParserState* globalParserState;
                                                                                
PEGASUS_NAMESPACE_END
char msg_[100];
void printf__(char * msg){
        if(DEBUG_LEX == 1)
                printf("%s\n",msg);
}

int lineno;

%}
SIGN [+-]
BINARY_DIGIT [01]
HEX_DIGIT [A-Fa-f0-9]
HEX_IDENT 0[Xx]
POSITIVE_DECIMAL_DIGIT [1-9]
DECIMAL_DIGIT [0-9]
BLANK [ \t]
IDENT_CHAR [0-9A-Za-z_\x80-\xFF]
PROP_CHAR [A-Za-z0-9_\[\]\-\#\']

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

    sprintf(msg_,"LEX: %s [SELECT] ", yytext);
    printf__(msg_);
    globalParserState->currentTokenPos+=yyleng;    
    globalParserState->tokenCount++;
    return SELECT;
}

{F}{R}{O}{M} {

    sprintf(msg_,"LEX: %s [FROM] ", yytext);
    printf__(msg_);
    globalParserState->currentTokenPos+=yyleng;
    globalParserState->tokenCount++;
    return FROM;
}

{W}{H}{E}{R}{E} {

    sprintf(msg_,"LEX: %s [WHERE] ", yytext);
    printf__(msg_);
    globalParserState->currentTokenPos+=yyleng;
    globalParserState->tokenCount++;
    return WHERE;
}

{A}{N}{Y} {

    sprintf(msg_,"LEX: %s [ANY] ", yytext);
    printf__(msg_);
    globalParserState->currentTokenPos+=yyleng;
    globalParserState->tokenCount++;
    return ANY;
}

{A}{S} {
    sprintf(msg_,"LEX: %s [AS] ", yytext);
    printf__(msg_);
    globalParserState->currentTokenPos+=yyleng;
    globalParserState->tokenCount++;
    return AS;
}

{A}{S}{C} {
    sprintf(msg_,"LEX: %s [ASC] ", yytext);
    printf__(msg_);
    globalParserState->currentTokenPos+=yyleng;
    globalParserState->tokenCount++;
    return ASC;
}

{B}{Y} {
    sprintf(msg_,"LEX: %s [BY] ", yytext);
    printf__(msg_);
    globalParserState->currentTokenPos+=yyleng;
    globalParserState->tokenCount++;
    return BY;
}

{D}{E}{S}{C} {
    sprintf(msg_,"LEX: %s [DESC] ", yytext);
    printf__(msg_);
    globalParserState->currentTokenPos+=yyleng;
    globalParserState->tokenCount++;
    return DESC;
}


{D}{I}{S}{T}{I}{N}{C}{T} {
    sprintf(msg_,"LEX: %s [DISTINCT] ", yytext);
    printf__(msg_);
    globalParserState->currentTokenPos+=yyleng;
    globalParserState->tokenCount++;
    return DISTINCT;
}

{E}{V}{E}{R}{Y} {
    sprintf(msg_,"LEX: %s [EVERY] ", yytext);
    printf__(msg_);
    globalParserState->currentTokenPos+=yyleng;
    globalParserState->tokenCount++;
    return EVERY;
}

{F}{I}{R}{S}{T} {
    sprintf(msg_,"LEX: %s [FIRST] ", yytext);
    printf__(msg_);
    globalParserState->currentTokenPos+=yyleng;
    globalParserState->tokenCount++;
    return FIRST;
}

{I}{N} {
    sprintf(msg_,"LEX: %s [IN] ", yytext);
    printf__(msg_);
    globalParserState->currentTokenPos+=yyleng;
    globalParserState->tokenCount++;
    return IN;
}

{I}{S} {
    sprintf(msg_,"LEX: %s [IS] ", yytext);
    printf__(msg_);
    globalParserState->currentTokenPos+=yyleng;
    globalParserState->tokenCount++;
    return IS;
}

{I}{S}{A} {
    sprintf(msg_,"LEX: %s [ISA] ", yytext);
    printf__(msg_);
    globalParserState->currentTokenPos+=yyleng;
    globalParserState->tokenCount++;
    return _ISA;
}

{L}{I}{K}{E} {
    sprintf(msg_,"LEX: %s [LIKE] ", yytext);
    printf__(msg_);
    globalParserState->currentTokenPos+=yyleng;
    globalParserState->tokenCount++;
    return _LIKE;
}

{O}{R}{D}{E}{R} {
    sprintf(msg_,"LEX: %s [ORDER] ", yytext);
    printf__(msg_);
    globalParserState->currentTokenPos+=yyleng;
    globalParserState->tokenCount++;
    return ORDER;
}

{S}{A}{T}{I}{S}{F}{I}{E}{S} {
    sprintf(msg_,"LEX: %s [SATISFIES] ", yytext);
    printf__(msg_);
    globalParserState->currentTokenPos+=yyleng;
    globalParserState->tokenCount++;
    return SATISFIES;
}

{T}{R}{U}{E} {
    sprintf(msg_,"LEX: %s [_TRUE] ", yytext);
    printf__(msg_);
    globalParserState->currentTokenPos+=yyleng;
    globalParserState->tokenCount++;
    return _TRUE;
}

{F}{A}{L}{S}{E} {
    sprintf(msg_,"LEX: %s [_FALSE] ", yytext);
    printf__(msg_);
    globalParserState->currentTokenPos+=yyleng;
    globalParserState->tokenCount++;
    return _FALSE;
}

{N}{U}{L}{L} {
    sprintf(msg_,"LEX: %s [_NULL] ", yytext);
    printf__(msg_);
    globalParserState->currentTokenPos+=yyleng;
    globalParserState->tokenCount++;
    return _NULL;
}

{N}{O}{T} {
    sprintf(msg_,"LEX: %s [NOT] ", yytext);
    printf__(msg_);
    globalParserState->currentTokenPos+=yyleng;
    globalParserState->tokenCount++;
    return NOT;
}

{A}{N}{D} {
    sprintf(msg_,"LEX: %s [AND] ", yytext);
    printf__(msg_);
    globalParserState->currentTokenPos+=yyleng;
    globalParserState->tokenCount++;
    return _AND;
}

{O}{R} {
    sprintf(msg_,"LEX: %s [OR] ", yytext);
    printf__(msg_);
    globalParserState->currentTokenPos+=yyleng;
    globalParserState->tokenCount++;
    return _OR;
}

{IDENT_CHAR}*\:\:{PROP_CHAR}* {
    globalParserState->currentTokenPos+=yyleng;
    globalParserState->tokenCount++;
	 /* remove any single quotes surrounding embedded literals */
	 size_t n;
	 if(CQL_lval.strValue)
	         delete [] CQL_lval.strValue;
				
	 String s(yytext);
	 Uint32 index = s.find("'");
	 if(index != PEG_NOT_FOUND){
	 	s.remove(index,1);
		s.remove(s.size()-1,1);
		CString cstr = s.getCString();
	 	const char* string = (const char*)cstr;
		n = strlen(string);
		CQL_lval.strValue = new char[n+1];
		memcpy(CQL_lval.strValue, string, n);
	 }else{
	 	n = strlen(yytext);
		CQL_lval.strValue = new char[n+1];
      memcpy(CQL_lval.strValue, yytext, n);
	 }
    CQL_lval.strValue[n] = '\0';
    sprintf(msg_,"LEX: %s [SCOPED_PROPERTY] ", CQL_lval.strValue);
    printf__(msg_);

    return SCOPED_PROPERTY;
}

[-]{1}[0][Xx]{HEX_DIGIT}{HEX_DIGIT}* {
    sprintf(msg_,"LEX: %s [NEGATIVE_HEXADECIMAL] ", yytext);
    printf__(msg_);
    globalParserState->currentTokenPos+=yyleng;
    globalParserState->tokenCount++;
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
    sprintf(msg_,"LEX: %s [HEXADECIMAL] ", yytext);
    printf__(msg_);
    globalParserState->currentTokenPos+=yyleng;
    globalParserState->tokenCount++;
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
    sprintf(msg_,"LEX: %s [NEGATIVE_BINARY] ", yytext);
    printf__(msg_);
    globalParserState->currentTokenPos+=yyleng;
    globalParserState->tokenCount++;
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
    sprintf(msg_,"LEX: %s [BINARY] ", yytext);
    printf__(msg_);
    globalParserState->currentTokenPos+=yyleng;
    globalParserState->tokenCount++;
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

    sprintf(msg_,"LEX: %s [NEGATIVE_INTEGER] ", yytext);
    printf__(msg_);
    globalParserState->currentTokenPos+=yyleng;
    globalParserState->tokenCount++;
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
                                                                                                                                       
    sprintf(msg_,"LEX: %s [INTEGER] ", yytext);
    printf__(msg_);
    globalParserState->currentTokenPos+=yyleng;
    globalParserState->tokenCount++;
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

    sprintf(msg_,"LEX: %s [INTEGER] ", yytext);
    printf__(msg_);
    globalParserState->currentTokenPos+=yyleng;
    globalParserState->tokenCount++;
    size_t n = strlen(yytext);
    if(CQL_lval.strValue)
	    delete [] CQL_lval.strValue;
    CQL_lval.strValue = new char[n + 1];
    memcpy(CQL_lval.strValue, yytext, n);
    CQL_lval.strValue[n] = '\0';

    return INTEGER;
}

[-]{1}{DECIMAL_DIGIT}*\.{DECIMAL_DIGIT}+([eE][+-]?{DECIMAL_DIGIT}+)? {

    sprintf(msg_,"LEX: %s [NEGATIVE_REAL] ", yytext);
    printf__(msg_);
    globalParserState->currentTokenPos+=yyleng;
    globalParserState->tokenCount++;
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
                                                                                                                                       
    sprintf(msg_,"LEX: %s [REAL] ", yytext);
    printf__(msg_);
    globalParserState->currentTokenPos+=yyleng;
    globalParserState->tokenCount++;
    size_t n = strlen(yytext);
    if(CQL_lval.strValue)
            delete [] CQL_lval.strValue;
    CQL_lval.strValue = new char[n + 1];
    memcpy(CQL_lval.strValue, yytext, n);
    CQL_lval.strValue[n] = '\0';
                                                                                                                                       
    /*CQL_lval.realValue = strtod((char*)yytext, (char**)0);*/
    return REAL;
}

\'((\\')|(\\)|[^'(\\')])*\' {
	/* \'[^\'\n]*\' */
    /* ATTN-B: handle long literals by using yyinput(). */
    /* ATTN-B: Handle expansion of special characters */

    sprintf(msg_,"LEX: %s [STRING] ", yytext);
    printf__(msg_);
    globalParserState->currentTokenPos+=yyleng;
    globalParserState->tokenCount++;
    /* remove any escaped \ OR escaped ' */

    String _esc_doubleslash = "\\\\";
    String _esc_singlequote = "\\'";
	 String _esc_doublequote = "\\\"";
    String s(yytext);

    Uint32 index = 1;
    while((index = s.find(_esc_doubleslash)) != PEG_NOT_FOUND || 
	       (index = s.find(_esc_singlequote)) != PEG_NOT_FOUND ||
			 (index = s.find(_esc_doublequote)) != PEG_NOT_FOUND)
	 {
				if(index == s.size() - 2) // make sure we dont remove the slash from say 'abc\'
				break;
				s.remove(index,1);
    }
    CString cstr = s.getCString();
    const char* string = (const char*)cstr;

    /* Copy the string (but remove the surrounding quotes */

    {
	size_t n = strlen(string) - 2;
	if(CQL_lval.strValue)
		delete [] CQL_lval.strValue;
	CQL_lval.strValue = new char[n + 1];
	memcpy(CQL_lval.strValue, string + 1, n);
	CQL_lval.strValue[n] = '\0';
    }
    return STRING_LITERAL;
}

\'[^\'\n]*$ {

    sprintf(msg_,"Unterminated string");
    printf__(msg_);
    globalParserState->currentTokenPos+=yyleng;
    globalParserState->tokenCount++;
}

\*{1} {
   sprintf(msg_,"LEX: [STAR] ");
   printf__(msg_);
   globalParserState->currentTokenPos+=yyleng;
   globalParserState->tokenCount++;
   return STAR;
}

\/{1} {
   sprintf(msg_,"LEX: [DIV] ");
   printf__(msg_);
   globalParserState->currentTokenPos+=yyleng;
   globalParserState->tokenCount++;
   return DIV;
}

\+{1} {
   sprintf(msg_,"LEX: [PLUS] ");
   printf__(msg_);
   globalParserState->currentTokenPos+=yyleng;
   globalParserState->tokenCount++;
   return PLUS;
}

\-{1} {
   sprintf(msg_,"LEX: [MINUS] ");
   printf__(msg_);
   globalParserState->currentTokenPos+=yyleng;
   globalParserState->tokenCount++;
   return MINUS;
}

\,{1} {
   sprintf(msg_,"LEX: [COMMA] ");
   printf__(msg_);
   globalParserState->currentTokenPos+=yyleng;
   globalParserState->tokenCount++;
   return COMMA;
}

\.{2} {
    sprintf(msg_,"LEX: [DOTDOT] ");
    printf__(msg_);
    globalParserState->currentTokenPos+=yyleng;
    globalParserState->tokenCount++;
    return DOTDOT;
}

\#{1} {
    sprintf(msg_,"LEX: [HASH] ");
    printf__(msg_);
    globalParserState->currentTokenPos+=yyleng;                                                                   
    globalParserState->tokenCount++;
    return HASH;
}

\.{1} {
    sprintf(msg_,"LEX: [DOT] ");
    printf__(msg_);
    globalParserState->currentTokenPos+=yyleng;
    globalParserState->tokenCount++;
    return DOT;
}

\[{1} {
    sprintf(msg_,"LEX: [LBRKT] ");
    printf__(msg_);
    globalParserState->currentTokenPos+=yyleng;
    globalParserState->tokenCount++;
    return LBRKT;
}

\]{1} {
    sprintf(msg_,"LEX: [RBRKT] ");
    printf__(msg_);
    globalParserState->currentTokenPos+=yyleng;
    globalParserState->tokenCount++;
    return RBRKT;
}

\({1} {
    sprintf(msg_,"LEX: [LPAR] ");
    printf__(msg_);
    globalParserState->currentTokenPos+=yyleng;
    globalParserState->tokenCount++;
    return LPAR;
}

\){1} {
    sprintf(msg_,"LEX: [RPAR] ");
    printf__(msg_);
    globalParserState->currentTokenPos+=yyleng;
    globalParserState->tokenCount++;
    return RPAR;
}

"||" {
    sprintf(msg_,"LEX: %s [DBL_PIPE] ", yytext);
    printf__(msg_);
    globalParserState->currentTokenPos+=yyleng;
    globalParserState->tokenCount++;
    return DBL_PIPE;
}

"_" {
    sprintf(msg_,"LEX: %s [UNDERSCORE] ", yytext);
    printf__(msg_);
    globalParserState->currentTokenPos+=yyleng;
    globalParserState->tokenCount++;
    return UNDERSCORE;
}

"=" { 
    sprintf(msg_,"LEX: %s [_EQ] ", yytext);
    printf__(msg_);
    globalParserState->currentTokenPos+=yyleng;
    globalParserState->tokenCount++;
    return _EQ; 
}


"<>" { 

    sprintf(msg_,"LEX: %s [_NE] ", yytext);
    printf__(msg_);
    globalParserState->currentTokenPos+=yyleng;
    globalParserState->tokenCount++;
    return _NE; 
}


"<=" { 

    sprintf(msg_,"LEX: %s [_LE] ", yytext);
    printf__(msg_);
    globalParserState->currentTokenPos+=yyleng;
    globalParserState->tokenCount++;
    return _LE; 
}

"<" { 

    sprintf(msg_,"LEX: %s [_LT] ", yytext);
    printf__(msg_);
    globalParserState->currentTokenPos+=yyleng;
    globalParserState->tokenCount++;
    return _LT; 
}

">=" { 

    sprintf(msg_,"LEX: %s [_GE] ", yytext);
    printf__(msg_);
    globalParserState->currentTokenPos+=yyleng;
    globalParserState->tokenCount++;
    return _GE; 
}

">" { 

    sprintf(msg_,"LEX: %s [_GT] ", yytext);
    printf__(msg_);
    globalParserState->currentTokenPos+=yyleng;
    globalParserState->tokenCount++;
    return _GT; 
}

{IDENT_CHAR}({IDENT_CHAR}|{DECIMAL_DIGIT})*  {

    sprintf(msg_,"LEX: %s [IDENTIFIER] ", yytext);
    printf__(msg_);
    globalParserState->currentTokenPos+=yyleng;
    globalParserState->tokenCount++;
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
    globalParserState->currentTokenPos+=yyleng;
}

\n {
	globalParserState->currentTokenPos=0;
	return 0;
   }

<<EOF>> {
             return 0;
        }

. {
    CQL_lval.strValue = 0;
    sprintf(msg_,"LEX::UNEXPECTED_CHAR\n");
    printf__(msg_);

    return UNEXPECTED_CHAR;
}

%%
extern "C" int CQL_wrap()
{
    return 1;
}


