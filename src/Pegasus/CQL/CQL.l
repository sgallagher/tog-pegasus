/*//%2006///////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
//============================================================================*/

/* 
   This file describes the language tokens possible for CQL. When a token is
   matched, the token postion is updated in the CQL_globalParserState, copying
   from the lex buffer  to a shared buffer (shared with the lexer and parser)
   may happen, then a token identifier is returned to the parser.

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
static char msg_[100];
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

    sprintf(msg_,"LEX: %s [TOK_SELECT] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;    
    CQL_globalParserState->tokenCount++;
    return TOK_SELECT;
}

{F}{R}{O}{M} {

    sprintf(msg_,"LEX: %s [TOK_FROM] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return TOK_FROM;
}

{W}{H}{E}{R}{E} {

    sprintf(msg_,"LEX: %s [TOK_WHERE] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return TOK_WHERE;
}

{A}{N}{Y} {

    sprintf(msg_,"LEX: %s [TOK_ANY] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return TOK_ANY;
}

{A}{S} {
    sprintf(msg_,"LEX: %s [TOK_AS] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return TOK_AS;
}

{A}{S}{C} {
    sprintf(msg_,"LEX: %s [TOK_ASC] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return TOK_ASC;
}

{B}{Y} {
    sprintf(msg_,"LEX: %s [TOK_BY] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return TOK_BY;
}

{D}{E}{S}{C} {
    sprintf(msg_,"LEX: %s [TOK_DESC] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return TOK_DESC;
}


{D}{I}{S}{T}{I}{N}{C}{T} {
    sprintf(msg_,"LEX: %s [TOK_DISTINCT] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return TOK_DISTINCT;
}

{E}{V}{E}{R}{Y} {
    sprintf(msg_,"LEX: %s [TOK_EVERY] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return TOK_EVERY;
}

{F}{I}{R}{S}{T} {
    sprintf(msg_,"LEX: %s [TOK_FIRST] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return TOK_FIRST;
}

{I}{N} {
    sprintf(msg_,"LEX: %s [TOK_IN] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return TOK_IN;
}

{I}{S} {
    sprintf(msg_,"LEX: %s [TOK_IS] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return TOK_IS;
}

{I}{S}{A} {
    sprintf(msg_,"LEX: %s [ISA] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return TOK_ISA;
}

{L}{I}{K}{E} {
    sprintf(msg_,"LEX: %s [TOKE_LIKE] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return TOK_LIKE;
}

{O}{R}{D}{E}{R} {
    sprintf(msg_,"LEX: %s [TOK_ORDER] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return TOK_ORDER;
}

{S}{A}{T}{I}{S}{F}{I}{E}{S} {
    sprintf(msg_,"LEX: %s [TOK_SATISFIES] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return TOK_SATISFIES;
}

{T}{R}{U}{E} {
    sprintf(msg_,"LEX: %s [TOK_TRUE] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return TOK_TRUE;
}

{F}{A}{L}{S}{E} {
    sprintf(msg_,"LEX: %s [TOK_FALSE] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return TOK_FALSE;
}

{N}{U}{L}{L} {
    sprintf(msg_,"LEX: %s [TOK_NULL] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return TOK_NULL;
}

{N}{O}{T} {
    sprintf(msg_,"LEX: %s [TOK_NOT] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return TOK_NOT;
}

{A}{N}{D} {
    sprintf(msg_,"LEX: %s [TOK_AND] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return TOK_AND;
}

{O}{R} {
    sprintf(msg_,"LEX: %s [TOK_OR] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return TOK_OR;
    /* NOCHKSRC*/
}

{IDENT_CHAR_NO_NUM}{IDENT_CHAR}*\:\:{IDENT_CHAR_NO_NUM}({IDENT_CHAR}*|{IDENT_CHAR}*\[.*\]|{IDENT_CHAR}*#\'.*\') {
    /* DOCHKSRC */
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
            MessageLoaderParms("CQL.CQL_y.BAD_UTF8",
            "Bad UTF8 encountered parsing rule $0 in position $1.",
            "identifier",
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

    return TOK_SCOPED_PROPERTY;
}

[-]{1}[0][Xx]{HEX_DIGIT}{HEX_DIGIT}* {
    sprintf(msg_,"LEX: %s [TOK_NEGATIVE_HEXADECIMAL] ", yytext);
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
    return TOK_NEGATIVE_HEXADECIMAL;
}

[+]?[0][Xx]{HEX_DIGIT}{HEX_DIGIT}* {
    sprintf(msg_,"LEX: %s [TOK_HEXADECIMAL] ", yytext);
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
    return TOK_HEXADECIMAL;
}

[-]{1}{BINARY_DIGIT}{BINARY_DIGIT}*{B} {
    sprintf(msg_,"LEX: %s [TOK_NEGATIVE_BINARY] ", yytext);
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
    return TOK_NEGATIVE_BINARY;
}

[+]?{BINARY_DIGIT}{BINARY_DIGIT}*{B} {
    sprintf(msg_,"LEX: %s [TOK_BINARY] ", yytext);
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
    return TOK_BINARY;
}


[-]{1}{POSITIVE_DECIMAL_DIGIT}{DECIMAL_DIGIT}* {

    sprintf(msg_,"LEX: %s [TOK_NEGATIVE_INTEGER] ", yytext);
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
    return TOK_NEGATIVE_INTEGER;
}

[+]?{DECIMAL_DIGIT}+ {

    sprintf(msg_,"LEX: %s [TOK_INTEGER] ", yytext);
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
    return TOK_INTEGER;
}

[-]{1}{DECIMAL_DIGIT}*\.{DECIMAL_DIGIT}+([eE][+-]?{DECIMAL_DIGIT}+)? {

    sprintf(msg_,"LEX: %s [TOK_NEGATIVE_REAL] ", yytext);
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
    return TOK_NEGATIVE_REAL;
    /*NOCHKSRC*/
}

([+]?{DECIMAL_DIGIT}*\.{DECIMAL_DIGIT}+([eE][+-]?{DECIMAL_DIGIT}+)?)|([+]?{DECIMAL_DIGIT}+\.{DECIMAL_DIGIT}*([eE][+-]?{DECIMAL_DIGIT}+)?) {
    /*DOCHKSRC*/
    sprintf(msg_,"LEX: %s [TOK_REAL] ", yytext);
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
    return TOK_REAL;
}

\'((\\')|(\\\\)|[^'(\\')])*\' {
    /* \'[^\'\n]*\' */
    /* ATTN-B: handle long literals by using yyinput(). */
    /* ATTN-B: Handle expansion of special characters */

    sprintf(msg_,"LEX: %s [TOK_STRING_LITERAL] ", yytext);
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
         // make sure we don't remove the slasy from say 'abc\'
         if(index == s.size() - 2)
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
    return TOK_STRING_LITERAL;
}

\'[^\'\n]*$ {

    sprintf(msg_,"Unterminated string");
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
}

\*{1} {
   sprintf(msg_,"LEX: [TOK_STAR] ");
   printf__(msg_);
   CQL_globalParserState->currentTokenPos+=yyleng;
   CQL_globalParserState->tokenCount++;
   return TOK_STAR;
}

\/{1} {
   sprintf(msg_,"LEX: [TOK_DIV] ");
   printf__(msg_);
   CQL_globalParserState->currentTokenPos+=yyleng;
   CQL_globalParserState->tokenCount++;
   return TOK_DIV;
}

\+{1} {
   sprintf(msg_,"LEX: [TOK_PLUS] ");
   printf__(msg_);
   CQL_globalParserState->currentTokenPos+=yyleng;
   CQL_globalParserState->tokenCount++;
   return TOK_PLUS;
}

\-{1} {
   sprintf(msg_,"LEX: [TOK_MINUS] ");
   printf__(msg_);
   CQL_globalParserState->currentTokenPos+=yyleng;
   CQL_globalParserState->tokenCount++;
   return TOK_MINUS;
}

\,{1} {
   sprintf(msg_,"LEX: [TOK_COMMA] ");
   printf__(msg_);
   CQL_globalParserState->currentTokenPos+=yyleng;
   CQL_globalParserState->tokenCount++;
   return TOK_COMMA;
}

\.{2} {
    sprintf(msg_,"LEX: [TOK_DOTDOT] ");
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return TOK_DOTDOT;
}

\#{1} {
    sprintf(msg_,"LEX: [TOK_HASH] ");
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return TOK_HASH;
}

\.{1} {
    sprintf(msg_,"LEX: [TOK_DOT] ");
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return TOK_DOT;
}

\[{1} {
    sprintf(msg_,"LEX: [TOK_LBRKT] ");
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return TOK_LBRKT;
}

\]{1} {
    sprintf(msg_,"LEX: [TOK_RBRKT] ");
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return TOK_RBRKT;
}

\({1} {
    sprintf(msg_,"LEX: [TOK_LPAR] ");
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return TOK_LPAR;
}

\){1} {
    sprintf(msg_,"LEX: [TOK_RPAR] ");
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return TOK_RPAR;
}

"||" {
    sprintf(msg_,"LEX: %s [TOK_DBL_PIPE] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return TOK_DBL_PIPE;
}

"_" {
    sprintf(msg_,"LEX: %s [TOK_UNDERSCORE] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return TOK_UNDERSCORE;
}

"=" { 
    sprintf(msg_,"LEX: %s [TOK_EQ] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return TOK_EQ; 
}


"<>" { 

    sprintf(msg_,"LEX: %s [TOK_NE] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return TOK_NE; 
}


"<=" { 

    sprintf(msg_,"LEX: %s [TOK_LE] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return TOK_LE; 
}

"<" { 

    sprintf(msg_,"LEX: %s [TOK_LT] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return TOK_LT; 
}

">=" { 

    sprintf(msg_,"LEX: %s [TOK_GE] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return TOK_GE; 
}

">" { 

    sprintf(msg_,"LEX: %s [TOK_GT] ", yytext);
    printf__(msg_);
    CQL_globalParserState->currentTokenPos+=yyleng;
    CQL_globalParserState->tokenCount++;
    return TOK_GT; 
}

{IDENT_CHAR_NO_NUM}({IDENT_CHAR})*  {

    sprintf(msg_,"LEX: %s [TOK_IDENTIFIER] ", yytext);
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

    return TOK_IDENTIFIER;
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
    sprintf(msg_,"LEX::TOK_UNEXPECTED_CHAR\n");
    printf__(msg_);

    return TOK_UNEXPECTED_CHAR;
}

%%
extern "C" int CQL_wrap()
{
    return 1;
}


