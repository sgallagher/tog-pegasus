%option never-interactive
%{

extern int WQLInput(char* buffer, int& numRead, int numRequested);
extern int WQL_error(char*);

/* ATTN: works with Flex only */
#define YY_INPUT(BUF, NREAD, NREQUESTED) WQLInput(BUF, NREAD, NREQUESTED)

#include "WQLYACC.h"
#include <cstring>

#define WQL_TRACE(X) printf X

%}

POSITIVE_DECIMAL_DIGIT [1-9]
DECIMAL_DIGIT [0-9]
BLANK [ \t\n]
IDENT_CHAR [A-Za-z_]

%%

[Ss][Ee][Ll][Ee][Cc][Tt] {

    WQL_TRACE(("LEX: %s [TOK_SELECT]\n", yytext));
    return TOK_SELECT;
}

[Ff][Rr][Oo][Mm] {

    WQL_TRACE(("LEX: %s [TOK_FROM]\n", yytext));
    return TOK_FROM;
}

[Ww][Hh][Ee][Rr][Ee] {

    WQL_TRACE(("LEX: %s [TOK_WHERE]\n", yytext));
    return TOK_WHERE;
}

[Tt][Rr][Uu][Ee] {

    WQL_TRACE(("LEX: %s [TOK_TRUE]\n", yytext));
    return TOK_TRUE;
}

[Ff][Aa][Ll][Ss][Ee] {

    WQL_TRACE(("LEX: %s [TOK_FALSE]\n", yytext));
    return TOK_FALSE;
}

[Nn][Ul][Ul] {

    WQL_TRACE(("LEX: %s [TOK_NULL]\n", yytext));
    return TOK_NULL;
}

[Nn][Oo][Tt] {

    WQL_TRACE(("LEX: %s [TOK_NOT]\n", yytext));
    return TOK_NOT;
}

[Aa][Nn][Dd] {

    WQL_TRACE(("LEX: %s [TOK_AND]\n", yytext));
    return TOK_AND;
}

[Oo][Rr] {

    WQL_TRACE(("LEX: %s [TOK_OR]\n", yytext));
    return TOK_OR;
}

[Ii][Ss] {

    WQL_TRACE(("LEX: %s [TOK_IS]\n", yytext));
    return TOK_IS;
}

[-+]?{POSITIVE_DECIMAL_DIGIT}{DECIMAL_DIGIT}* {

    WQL_TRACE(("LEX: %s [TOK_INTEGER]\n", yytext));
    WQL_lval.intValue = strtol(yytext, (char**)0, 10);
    return TOK_INTEGER;
}

[+-]?0 {

    WQL_TRACE(("LEX: %s [TOK_INTEGER]\n", yytext));
    WQL_lval.intValue = 0;
    return TOK_INTEGER;
}

[-+]?{DECIMAL_DIGIT}*\.{DECIMAL_DIGIT}+([eE][+-]?{DECIMAL_DIGIT}+)? {

    WQL_TRACE(("LEX: %s [TOK_DOUBLE]\n", yytext));
    WQL_lval.doubleValue = strtod((char*)yytext, (char**)0);
    return TOK_DOUBLE;
}

\"[^\"\n]*\" {

    /* ATTN: Can long string literals overflow the buffer? */

    WQL_TRACE(("LEX: %s [TOK_STRING]\n", yytext));
    WQL_lval.strValue = strdup(yytext);
    return TOK_STRING;
}

\"[^\"\n]*$ {

    WQL_error("Unterminated string");
}

[\*(),] { 

    WQL_TRACE(("LEX: %c\n", yytext[0]));
    return yytext[0];
}

"=" { 
    WQL_TRACE(("LEX: %s [TOK_EQ]\n", yytext));
    return TOK_EQ; 
}

"!=" { 

    WQL_TRACE(("LEX: %s [TOK_NE]\n", yytext));
    return TOK_NE; 
}

"<=" { 

    WQL_TRACE(("LEX: %s [TOK_LE]\n", yytext));
    return TOK_LE; 
}

"<" { 

    WQL_TRACE(("LEX: %s [TOK_LT]\n", yytext));
    return TOK_LT; 
}

">=" { 

    WQL_TRACE(("LEX: %s [TOK_GE]\n", yytext));
    return TOK_GE; 
}

">" { 

    WQL_TRACE(("LEX: %s [TOK_GT]\n", yytext));
    return TOK_GT; 
}

{IDENT_CHAR}({IDENT_CHAR}|{DECIMAL_DIGIT})*  {

    WQL_TRACE(("LEX: %s [TOK_IDENTIFIER]\n", yytext));
    WQL_lval.strValue = strdup((char*)yytext);
    return TOK_IDENTIFIER;
}

{BLANK}+ {

    /* Ignore blanks */
}

. {
    WQL_lval.strValue = strdup((char*)yytext);
    return TOK_UNEXPECTED_CHAR;
}

%%

extern "C" int WQL_wrap()
{
    return 1;
}
