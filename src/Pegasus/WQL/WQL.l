%option never-interactive
%{

#include "WQLYACC.h"
#include <cstring>

%}

POSITIVE_DECIMAL_DIGIT [1-9]
DECIMAL_DIGIT [0-9]
BLANK [ \t\n]
IDENT_CHAR [A-Za-z_]

%%

[Ss][Ee][Ll][Ee][Cc][Tt] {

    return TOK_SELECT;
}


[-+]?{POSITIVE_DECIMAL_DIGIT}{DECIMAL_DIGIT}* {

    WQL_lval.intValue = strtol(yytext, (char**)0, 10);
    return TOK_INTEGER;
}

[+-]?0 {

    WQL_lval.intValue = 0;
    return TOK_INTEGER;
}

[-+]?{DECIMAL_DIGIT}*\.{DECIMAL_DIGIT}+([eE][+-]?{DECIMAL_DIGIT}+)? {

    WQL_lval.doubleValue = strtod((char*)yytext, (char**)0);
    return TOK_DOUBLE;
}

\" {

#if 0
    int c;
    bmc_string8 tmp;

    while ((c = yymapch('"', '\\')) >= 0)
	tmp += (char)c;

    if (c == -2)
	WQL_error("unterminated string - unexpected end of file");
    if (c == -3)
	WQL_error("unterminated string - unexpected end of line");
    if (c < -1)
	yyterminate(); 

    WQL_lval.stringValue = strdup(tmp.c_str());

    return STRING_VALUE;
#endif
}

{IDENT_CHAR}({IDENT_CHAR}|{DECIMAL_DIGIT})*  {

    WQL_lval.strValue = strdup((char*)yytext);
    return TOK_IDENTIFIER;
}

{BLANK}+ {

}

. {

#if 0
    //cout << *yytext << endl;
    WQL_lval.stringValue = _Mof_StrDup((char*)yytext);
    if (*yytext == EOF)
	WQL_error("unexpected end of file");
    else if (*yytext == '\n')
	WQL_error("unexpected end of line");
    else
    {
	char tmp[48];
	sprintf(tmp, "unexpected symbol '%c'", *yytext);
	WQL_error(tmp);
    }
    yyterminate();
    return UNEXPECTED;
#endif

}

%%

extern "C" int WQL_wrap()
{
    return 1;
}
