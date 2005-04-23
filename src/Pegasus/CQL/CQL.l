/*
//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN
// ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
// "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//==============================================================================
*/
/* 
   This file describes the language tokens possible for CQL. When a token is matched,
	the token postion is updated in the CQL_globalParserState, copying from the lex buffer
	to a shared buffer (shared with the lexer and parser) may happen, then a token identifier
	is returned to the parser.

*/	

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
#include <Pegasus/Common/CommonUTF.h>
#include <Pegasus/Common/MessageLoader.h>
#include "CQLParserState.h"
#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include "CQLObjects.h"
#include "CQLYACC.h"
PEGASUS_NAMESPACE_BEGIN
                                                                                
extern CQLParserState* CQL_globalParserState;
                                                                                
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
IDENT_CHAR_1 [0-9A-Za-z_\x80-\xFF]
IDENT_CHAR_NO_NUM [A-Za-z_\x80-\xFF]
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
    CQL_globalParserState->currentTokenPos+=yyleng;    
    CQL_globalParserState->tokenCount++;
    return SELECT;
}

{F}{R}{O}{M} {

    sprintf(msg_,"LEX: %s [FROM] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return FROM;
}

{W}{H}{E}{R}{E} {

    sprintf(msg_,"LEX: %s [WHERE] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return WHERE;
}

{A}{N}{Y} {

    sprintf(msg_,"LEX: %s [ANY] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return ANY;
}

{A}{S} {
    sprintf(msg_,"LEX: %s [AS] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return AS;
}

{A}{S}{C} {
    sprintf(msg_,"LEX: %s [ASC] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return ASC;
}

{B}{Y} {
    sprintf(msg_,"LEX: %s [BY] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return BY;
}

{D}{E}{S}{C} {
    sprintf(msg_,"LEX: %s [DESC] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return DESC;
}


{D}{I}{S}{T}{I}{N}{C}{T} {
    sprintf(msg_,"LEX: %s [DISTINCT] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return DISTINCT;
}

{E}{V}{E}{R}{Y} {
    sprintf(msg_,"LEX: %s [EVERY] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return EVERY;
}

{F}{I}{R}{S}{T} {
    sprintf(msg_,"LEX: %s [FIRST] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return FIRST;
}

{I}{N} {
    sprintf(msg_,"LEX: %s [IN] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return IN;
}

{I}{S} {
    sprintf(msg_,"LEX: %s [IS] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return IS;
}

{I}{S}{A} {
    sprintf(msg_,"LEX: %s [ISA] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return _ISA;
}

{L}{I}{K}{E} {
    sprintf(msg_,"LEX: %s [LIKE] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return _LIKE;
}

{O}{R}{D}{E}{R} {
    sprintf(msg_,"LEX: %s [ORDER] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return ORDER;
}

{S}{A}{T}{I}{S}{F}{I}{E}{S} {
    sprintf(msg_,"LEX: %s [SATISFIES] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return SATISFIES;
}

{T}{R}{U}{E} {
    sprintf(msg_,"LEX: %s [_TRUE] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return _TRUE;
}

{F}{A}{L}{S}{E} {
    sprintf(msg_,"LEX: %s [_FALSE] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return _FALSE;
}

{N}{U}{L}{L} {
    sprintf(msg_,"LEX: %s [_NULL] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return _NULL;
}

{N}{O}{T} {
    sprintf(msg_,"LEX: %s [NOT] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return NOT;
}

{A}{N}{D} {
    sprintf(msg_,"LEX: %s [AND] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return _AND;
}

{O}{R} {
    sprintf(msg_,"LEX: %s [OR] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return _OR;
}

{IDENT_CHAR_NO_NUM}{IDENT_CHAR}*\:\:{IDENT_CHAR_NO_NUM}({IDENT_CHAR}*|{IDENT_CHAR}*\[.*\]|{IDENT_CHAR}*#\'.*\') {
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
	 /* remove any single quotes surrounding embedded literals */
	 size_t n;
	 if(CQL_lval.strValue)
	         delete [] CQL_lval.strValue;

    // chuck
    if (!isUTF8Str(yytext))
    {	
        sprintf(msg_,"LEX: [STRING]-> BAD UTF\n");
        printf__(msg_);
        throw CQLSyntaxErrorException(
                                      MessageLoaderParms(String("CQL.CQL_y.BAD_UTF8"),
                                      String("Bad UTF8 encountered parsing rule $0 in position $1."),
                                      String("identifier"),
                                      CQL_globalParserState->currentTokenPos));
    }
				
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
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
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
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
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
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
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
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
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
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    size_t n = strlen(yytext);
    if(CQL_lval.strValue)
	    delete [] CQL_lval.strValue;
    CQL_lval.strValue = new char[n + 1];
    memcpy(CQL_lval.strValue, yytext, n);
    CQL_lval.strValue[n] = '\0';

    /*CQL_lval.intValue = strtol(yytext, (char**)0, 10);*/
    return NEGATIVE_INTEGER;
}

[+]?{DECIMAL_DIGIT}+ {
                                                                                                                                       
    sprintf(msg_,"LEX: %s [INTEGER] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    size_t n = strlen(yytext);
    if(CQL_lval.strValue)
            delete [] CQL_lval.strValue;
    CQL_lval.strValue = new char[n + 1];
    memcpy(CQL_lval.strValue, yytext, n);
    CQL_lval.strValue[n] = '\0';
                                                                                                                                       
    /*CQL_lval.intValue = strtol(yytext, (char**)0, 10);*/
    return INTEGER;
}

[-]{1}{DECIMAL_DIGIT}*\.{DECIMAL_DIGIT}+([eE][+-]?{DECIMAL_DIGIT}+)? {

    sprintf(msg_,"LEX: %s [NEGATIVE_REAL] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    size_t n = strlen(yytext);
    if(CQL_lval.strValue)
            delete [] CQL_lval.strValue;
    CQL_lval.strValue = new char[n + 1];
    memcpy(CQL_lval.strValue, yytext, n);
    CQL_lval.strValue[n] = '\0';

    /*CQL_lval.realValue = strtod((char*)yytext, (char**)0);*/
    return NEGATIVE_REAL;
}

([+]?{DECIMAL_DIGIT}*\.{DECIMAL_DIGIT}+([eE][+-]?{DECIMAL_DIGIT}+)?)|([+]?{DECIMAL_DIGIT}+\.{DECIMAL_DIGIT}*([eE][+-]?{DECIMAL_DIGIT}+)?) {
                                                                                                                                       
    sprintf(msg_,"LEX: %s [REAL] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    size_t n = strlen(yytext);
    if(CQL_lval.strValue)
            delete [] CQL_lval.strValue;
    CQL_lval.strValue = new char[n + 1];
    memcpy(CQL_lval.strValue, yytext, n);
    CQL_lval.strValue[n] = '\0';
                                                                                                                                       
    /*CQL_lval.realValue = strtod((char*)yytext, (char**)0);*/
    return REAL;
}

\'((\\')|(\\\\)|[^'(\\')])*\' {
	/* \'[^\'\n]*\' */
    /* ATTN-B: handle long literals by using yyinput(). */
    /* ATTN-B: Handle expansion of special characters */

    sprintf(msg_,"LEX: %s [STRING] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    /* remove any escaped \ OR escaped ' */

    String _esc_doubleslash = "\\\\";
    String _esc_singlequote = "\\'";

    // chuck
    if (!isUTF8Str(yytext))
    {	
        sprintf(msg_,"LEX: [STRING]-> BAD UTF\n");
        printf__(msg_);
        throw CQLSyntaxErrorException(
                                      MessageLoaderParms(String("CQL.CQL_y.BAD_UTF8"),
                                      String("Bad UTF8 encountered parsing rule $0 in position $1."),
                                      String("literal_string"),
                                      CQL_globalParserState->currentTokenPos));
    }

    String s(yytext);

    Uint32 index = 1;
    while((index = s.find(_esc_doubleslash)) != PEG_NOT_FOUND || 
	       (index = s.find(_esc_singlequote)) != PEG_NOT_FOUND)
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
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
}

\*{1} {
   sprintf(msg_,"LEX: [STAR] ");
   printf__(msg_);
   CQL_globalParserState->currentTokenPos+=yyleng;
   CQL_globalParserState->tokenCount++;
   return STAR;
}

\/{1} {
   sprintf(msg_,"LEX: [DIV] ");
   printf__(msg_);
   CQL_globalParserState->currentTokenPos+=yyleng;
   CQL_globalParserState->tokenCount++;
   return DIV;
}

\+{1} {
   sprintf(msg_,"LEX: [PLUS] ");
   printf__(msg_);
   CQL_globalParserState->currentTokenPos+=yyleng;
   CQL_globalParserState->tokenCount++;
   return PLUS;
}

\-{1} {
   sprintf(msg_,"LEX: [MINUS] ");
   printf__(msg_);
   CQL_globalParserState->currentTokenPos+=yyleng;
   CQL_globalParserState->tokenCount++;
   return MINUS;
}

\,{1} {
   sprintf(msg_,"LEX: [COMMA] ");
   printf__(msg_);
   CQL_globalParserState->currentTokenPos+=yyleng;
   CQL_globalParserState->tokenCount++;
   return COMMA;
}

\.{2} {
    sprintf(msg_,"LEX: [DOTDOT] ");
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return DOTDOT;
}

\#{1} {
    sprintf(msg_,"LEX: [HASH] ");
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;                                                                   
    CQL_globalParserState->tokenCount++;
    return HASH;
}

\.{1} {
    sprintf(msg_,"LEX: [DOT] ");
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return DOT;
}

\[{1} {
    sprintf(msg_,"LEX: [LBRKT] ");
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return LBRKT;
}

\]{1} {
    sprintf(msg_,"LEX: [RBRKT] ");
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return RBRKT;
}

\({1} {
    sprintf(msg_,"LEX: [LPAR] ");
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return LPAR;
}

\){1} {
    sprintf(msg_,"LEX: [RPAR] ");
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return RPAR;
}

"||" {
    sprintf(msg_,"LEX: %s [DBL_PIPE] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return DBL_PIPE;
}

"_" {
    sprintf(msg_,"LEX: %s [UNDERSCORE] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return UNDERSCORE;
}

"=" { 
    sprintf(msg_,"LEX: %s [_EQ] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return _EQ; 
}


"<>" { 

    sprintf(msg_,"LEX: %s [_NE] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return _NE; 
}


"<=" { 

    sprintf(msg_,"LEX: %s [_LE] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return _LE; 
}

"<" { 

    sprintf(msg_,"LEX: %s [_LT] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return _LT; 
}

">=" { 

    sprintf(msg_,"LEX: %s [_GE] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return _GE; 
}

">" { 

    sprintf(msg_,"LEX: %s [_GT] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return _GT; 
}

{IDENT_CHAR_NO_NUM}({IDENT_CHAR})*  {

    sprintf(msg_,"LEX: %s [IDENTIFIER] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
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
    CQL_globalParserState->currentTokenPos+=yyleng;
}

\n {
	CQL_globalParserState->currentTokenPos=0;
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


