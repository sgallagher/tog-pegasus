#ifndef lint
static char yysccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93";
#endif
#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define yyclearin (yychar=(-1))
#define yyerrok (yyerrflag=0)
#define YYRECOVERING (yyerrflag!=0)
#define yyparse WQL_parse
#define yylex WQL_lex
#define yyerror WQL_error
#define yychar WQL_char
#define yyval WQL_val
#define yylval WQL_lval
#define yydebug WQL_debug
#define yynerrs WQL_nerrs
#define yyerrflag WQL_errflag
#define yyss WQL_ss
#define yyssp WQL_ssp
#define yyvs WQL_vs
#define yyvsp WQL_vsp
#define yylhs WQL_lhs
#define yylen WQL_len
#define yydefred WQL_defred
#define yydgoto WQL_dgoto
#define yysindex WQL_sindex
#define yyrindex WQL_rindex
#define yygindex WQL_gindex
#define yytable WQL_table
#define yycheck WQL_check
#define yyname WQL_name
#define yyrule WQL_rule
#define YYPREFIX "WQL_"
#line 10 "WQL.y"

#include <Pegasus/Common/Config.h>
#include <Pegasus/WQL/WQLOperation.h>
#include <Pegasus/WQL/WQLOperand.h>
#include <Pegasus/WQL/WQLParserState.h>
#include <Pegasus/WQL/WQLSelectStatement.h>
#include <string.h>
#include <stdlib.h>

#ifdef PEGASUS_OS_TYPE_WINDOWS
# include <malloc.h>
#endif

#if defined(PEGASUS_COMPILER_ACC) && defined(PEGASUS_OS_HPUX)
# include <alloca.h>
#endif

#if 0
# define WQL_TRACE(X) printf X
#else
# define WQL_TRACE(X)
#endif

extern int WQL_lex();
extern int WQL_error(char*);

/**/
/* Define the global parser state object:*/
/**/

PEGASUS_USING_PEGASUS;

PEGASUS_NAMESPACE_BEGIN

extern WQLParserState* globalParserState;

PEGASUS_NAMESPACE_END

#line 58 "WQL.y"
typedef union 
{
   int intValue;
   double doubleValue;
   char* strValue;
   void* nodeValue;
} YYSTYPE;
#line 83 "y.tab.c"
#define TOK_INTEGER 257
#define TOK_DOUBLE 258
#define TOK_STRING 259
#define TOK_TRUE 260
#define TOK_FALSE 261
#define TOK_NULL 262
#define TOK_EQ 263
#define TOK_NE 264
#define TOK_LT 265
#define TOK_LE 266
#define TOK_GT 267
#define TOK_GE 268
#define TOK_NOT 269
#define TOK_OR 270
#define TOK_AND 271
#define TOK_IS 272
#define TOK_IDENTIFIER 273
#define TOK_SELECT 274
#define TOK_WHERE 275
#define TOK_FROM 276
#define TOK_UNEXPECTED_CHAR 277
#define YYERRCODE 256
short WQL_lhs[] = {                                        -1,
    0,    9,   10,   10,    2,    2,   11,   11,    7,    8,
    6,    6,    6,    6,    6,    6,    6,    3,    3,    4,
    4,    4,    4,    4,    4,    5,    5,   13,   13,    1,
   12,   14,   14,   14,   14,   14,
};
short WQL_len[] = {                                         2,
    1,    3,    1,    1,    1,    3,    2,    1,    2,    2,
    3,    3,    2,    3,    1,    3,    4,    1,    1,    3,
    3,    3,    3,    3,    3,    3,    4,    1,    1,    1,
    1,    1,    1,    1,    1,    1,
};
short WQL_defred[] = {                                      0,
    0,    0,    1,   30,    3,    5,    0,    0,    0,    0,
    0,    2,    6,   31,    9,    0,    7,   33,   34,   35,
   28,   29,    0,    0,   32,    0,   18,   19,    0,   36,
    0,   13,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   14,    0,   16,    0,   12,   24,   25,
   20,   22,   21,   23,   26,    0,   17,   27,
};
short WQL_dgoto[] = {                                       2,
   25,    7,   26,   27,   28,   29,   11,   17,    3,    8,
   12,   15,   30,   31,
};
short WQL_sindex[] = {                                   -241,
  -38,    0,    0,    0,    0,    0,   -9, -237, -232, -225,
 -228,    0,    0,    0,    0,  -40,    0,    0,    0,    0,
    0,    0,  -40,  -40,    0, -223,    0,    0, -233,    0,
 -238,    0,  -39, -229,  -40,  -40, -252, -252, -252, -252,
 -252, -252, -226,    0, -215,    0, -221,    0,    0,    0,
    0,    0,    0,    0,    0, -211,    0,    0,
};
short WQL_rindex[] = {                                      0,
    0,    0,    0,    0,    0,    0, -224,    0,    0,    0,
   53,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    1,    0,    0,   54,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    3,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,
};
short WQL_gindex[] = {                                      0,
   11,    0,    0,    0,    0,  -13,    0,    0,    0,    0,
    0,    0,  -21,  -23,
};
#define YYTABLESIZE 273
short WQL_table[] = {                                      24,
   15,   44,   11,    5,   18,   19,   20,   21,   22,   32,
   33,    6,   46,   49,   50,   51,   52,   53,   54,   13,
    4,   47,   48,   57,   37,   38,   39,   40,   41,   42,
   21,   22,    1,   43,    9,   55,   35,   36,   10,   45,
    4,   15,   56,   11,   21,   22,   16,   14,   34,   36,
   58,    4,    8,   10,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   18,   19,   20,   21,
   22,    0,    0,    0,    0,    0,    0,    0,   23,    0,
   35,   36,    4,    0,    4,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
   15,   15,   11,
};
short WQL_check[] = {                                      40,
    0,   41,    0,   42,  257,  258,  259,  260,  261,   23,
   24,    1,   34,   37,   38,   39,   40,   41,   42,    9,
  273,   35,   36,   45,  263,  264,  265,  266,  267,  268,
  260,  261,  274,  272,   44,  262,  270,  271,  276,  269,
  273,   41,  269,   41,  260,  261,  275,  273,  272,  271,
  262,  276,    0,    0,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  257,  258,  259,  260,
  261,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  269,   -1,
  270,  271,  273,   -1,  273,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
  270,  271,  270,
};
#define YYFINAL 2
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 277
#if YYDEBUG
char *WQL_name[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,"'('","')'","'*'",0,"','",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"TOK_INTEGER",
"TOK_DOUBLE","TOK_STRING","TOK_TRUE","TOK_FALSE","TOK_NULL","TOK_EQ","TOK_NE",
"TOK_LT","TOK_LE","TOK_GT","TOK_GE","TOK_NOT","TOK_OR","TOK_AND","TOK_IS",
"TOK_IDENTIFIER","TOK_SELECT","TOK_WHERE","TOK_FROM","TOK_UNEXPECTED_CHAR",
};
char *WQL_rule[] = {
"$accept : start",
"start : selectStatement",
"selectStatement : TOK_SELECT selectList selectExpression",
"selectList : '*'",
"selectList : propertyList",
"propertyList : propertyName",
"propertyList : propertyList ',' propertyName",
"selectExpression : fromClause whereClause",
"selectExpression : fromClause",
"fromClause : TOK_FROM className",
"whereClause : TOK_WHERE searchCondition",
"searchCondition : searchCondition TOK_OR searchCondition",
"searchCondition : searchCondition TOK_AND searchCondition",
"searchCondition : TOK_NOT searchCondition",
"searchCondition : '(' searchCondition ')'",
"searchCondition : predicate",
"searchCondition : predicate TOK_IS truthValue",
"searchCondition : predicate TOK_IS TOK_NOT truthValue",
"predicate : comparisonPredicate",
"predicate : nullPredicate",
"comparisonPredicate : comparisonTerm TOK_LT comparisonTerm",
"comparisonPredicate : comparisonTerm TOK_GT comparisonTerm",
"comparisonPredicate : comparisonTerm TOK_LE comparisonTerm",
"comparisonPredicate : comparisonTerm TOK_GE comparisonTerm",
"comparisonPredicate : comparisonTerm TOK_EQ comparisonTerm",
"comparisonPredicate : comparisonTerm TOK_NE comparisonTerm",
"nullPredicate : comparisonTerm TOK_IS TOK_NULL",
"nullPredicate : comparisonTerm TOK_IS TOK_NOT TOK_NULL",
"truthValue : TOK_TRUE",
"truthValue : TOK_FALSE",
"propertyName : TOK_IDENTIFIER",
"className : TOK_IDENTIFIER",
"comparisonTerm : propertyName",
"comparisonTerm : TOK_INTEGER",
"comparisonTerm : TOK_DOUBLE",
"comparisonTerm : TOK_STRING",
"comparisonTerm : truthValue",
};
#endif
#ifdef YYSTACKSIZE
#undef YYMAXDEPTH
#define YYMAXDEPTH YYSTACKSIZE
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 500
#define YYMAXDEPTH 500
#endif
#endif
int yydebug;
int yynerrs;
int yyerrflag;
int yychar;
short *yyssp;
YYSTYPE *yyvsp;
YYSTYPE yyval;
YYSTYPE yylval;
short yyss[YYSTACKSIZE];
YYSTYPE yyvs[YYSTACKSIZE];
#define yystacksize YYSTACKSIZE
#define YYABORT goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR goto yyerrlab
int
yyparse()
{
    register int yym, yyn, yystate;
#if YYDEBUG
    register char *yys;
    extern char *getenv();

    if (yys = getenv("YYDEBUG"))
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
#endif

    yynerrs = 0;
    yyerrflag = 0;
    yychar = (-1);

    yyssp = yyss;
    yyvsp = yyvs;
    *yyssp = yystate = 0;

yyloop:
    if (yyn = yydefred[yystate]) goto yyreduce;
    if (yychar < 0)
    {
        if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, reading %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
    }
    if ((yyn = yysindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: state %d, shifting to state %d\n",
                    YYPREFIX, yystate, yytable[yyn]);
#endif
        if (yyssp >= yyss + yystacksize - 1)
        {
            goto yyoverflow;
        }
        *++yyssp = yystate = yytable[yyn];
        *++yyvsp = yylval;
        yychar = (-1);
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if ((yyn = yyrindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag) goto yyinrecovery;
#ifdef lint
    goto yynewerror;
#endif
yynewerror:
    yyerror("syntax error");
#ifdef lint
    goto yyerrlab;
#endif
yyerrlab:
    ++yynerrs;
yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if ((yyn = yysindex[*yyssp]) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: state %d, error recovery shifting\
 to state %d\n", YYPREFIX, *yyssp, yytable[yyn]);
#endif
                if (yyssp >= yyss + yystacksize - 1)
                {
                    goto yyoverflow;
                }
                *++yyssp = yystate = yytable[yyn];
                *++yyvsp = yylval;
                goto yyloop;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: error recovery discarding state %d\n",
                            YYPREFIX, *yyssp);
#endif
                if (yyssp <= yyss) goto yyabort;
                --yyssp;
                --yyvsp;
            }
        }
    }
    else
    {
        if (yychar == 0) goto yyabort;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, error recovery discards token %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
        yychar = (-1);
        goto yyloop;
    }
yyreduce:
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: state %d, reducing by rule %d (%s)\n",
                YYPREFIX, yystate, yyn, yyrule[yyn]);
#endif
    yym = yylen[yyn];
    yyval = yyvsp[1-yym];
    switch (yyn)
    {
case 1:
#line 131 "WQL.y"
{
	WQL_TRACE(("YACC: start\n"));
    }
break;
case 2:
#line 137 "WQL.y"
{

    }
break;
case 3:
#line 143 "WQL.y"
{
	globalParserState->statement->appendPropertyName("*");
    }
break;
case 4:
#line 147 "WQL.y"
{

    }
break;
case 5:
#line 153 "WQL.y"
{
	globalParserState->statement->appendPropertyName(yyvsp[0].strValue);
    }
break;
case 6:
#line 157 "WQL.y"
{
	globalParserState->statement->appendPropertyName(yyvsp[0].strValue);
    }
break;
case 7:
#line 163 "WQL.y"
{

    }
break;
case 8:
#line 167 "WQL.y"
{

    }
break;
case 9:
#line 173 "WQL.y"
{
	WQL_TRACE(("YACC: fromClause: TOK_FROM className(%s)\n", yyvsp[0].strValue));
	globalParserState->statement->setClassName(yyvsp[0].strValue);
    }
break;
case 10:
#line 180 "WQL.y"
{

    }
break;
case 11:
#line 186 "WQL.y"
{
	WQL_TRACE(("YACC: TOK_OR\n"));
	globalParserState->statement->appendOperation(WQL_OR);
    }
break;
case 12:
#line 191 "WQL.y"
{
	WQL_TRACE(("YACC: TOK_AND\n"));
	globalParserState->statement->appendOperation(WQL_AND);
    }
break;
case 13:
#line 196 "WQL.y"
{
	WQL_TRACE(("YACC: TOK_NOT\n"));

	globalParserState->statement->appendOperation(WQL_NOT);
    }
break;
case 14:
#line 202 "WQL.y"
{

    }
break;
case 15:
#line 206 "WQL.y"
{

    }
break;
case 16:
#line 210 "WQL.y"
{
	WQLOperation op = yyvsp[0].intValue ? WQL_IS_TRUE : WQL_IS_FALSE;
	globalParserState->statement->appendOperation(op);
    }
break;
case 17:
#line 215 "WQL.y"
{
	WQLOperation op = yyvsp[0].intValue ? WQL_IS_NOT_TRUE : WQL_IS_NOT_FALSE;
	globalParserState->statement->appendOperation(op);
    }
break;
case 18:
#line 224 "WQL.y"
{

    }
break;
case 19:
#line 228 "WQL.y"
{

    }
break;
case 20:
#line 234 "WQL.y"
{
	WQL_TRACE(("YACC: TOK_LT\n"));
	globalParserState->statement->appendOperation(WQL_LT);
    }
break;
case 21:
#line 239 "WQL.y"
{
	WQL_TRACE(("YACC: TOK_GT\n"));
	globalParserState->statement->appendOperation(WQL_GT);
    }
break;
case 22:
#line 244 "WQL.y"
{
	WQL_TRACE(("YACC: TOK_LE\n"));
	globalParserState->statement->appendOperation(WQL_LE);
    }
break;
case 23:
#line 249 "WQL.y"
{
	WQL_TRACE(("YACC: TOK_GE\n"));
	globalParserState->statement->appendOperation(WQL_GE);
    }
break;
case 24:
#line 254 "WQL.y"
{
	WQL_TRACE(("YACC: TOK_EQ\n"));
	globalParserState->statement->appendOperation(WQL_EQ);
    }
break;
case 25:
#line 259 "WQL.y"
{
	WQL_TRACE(("YACC: TOK_NE\n"));
	globalParserState->statement->appendOperation(WQL_NE);
    }
break;
case 26:
#line 266 "WQL.y"
{
	WQL_TRACE(("YACC: nullPredicate : comparisonTerm IS NULL\n"));
	globalParserState->statement->appendOperation(WQL_IS_NULL);
    }
break;
case 27:
#line 271 "WQL.y"
{
	WQL_TRACE(("YACC: nullPredicate : comparisonTerm IS NOT NULL\n"));
	globalParserState->statement->appendOperation(WQL_IS_NOT_NULL);
    }
break;
case 28:
#line 278 "WQL.y"
{
	yyval.intValue = 1;
    }
break;
case 29:
#line 282 "WQL.y"
{
	yyval.intValue = 0;
    }
break;
case 30:
#line 288 "WQL.y"
{
	WQL_TRACE(("YACC: propertyName : TOK_IDENTIFIER(%s)\n", yyvsp[0].strValue));
	yyval.strValue = yyvsp[0].strValue;
    }
break;
case 31:
#line 294 "WQL.y"
{
	WQL_TRACE(("YACC: TOK_IDENTIFIER %s\n", yyvsp[0].strValue));
	yyval.strValue = yyvsp[0].strValue;
    }
break;
case 32:
#line 301 "WQL.y"
{
	globalParserState->statement->appendOperand(
	    WQLOperand(yyvsp[0].strValue, WQLOperand::PROPERTY_NAME_TAG));
    }
break;
case 33:
#line 306 "WQL.y"
{
	globalParserState->statement->appendOperand(
	    WQLOperand(yyvsp[0].intValue, WQLOperand::INTEGER_VALUE_TAG));
    }
break;
case 34:
#line 311 "WQL.y"
{
	globalParserState->statement->appendOperand(
	    WQLOperand(yyvsp[0].doubleValue, WQLOperand::DOUBLE_VALUE_TAG));
    }
break;
case 35:
#line 316 "WQL.y"
{
	globalParserState->statement->appendOperand(
	    WQLOperand(yyvsp[0].strValue, WQLOperand::STRING_VALUE_TAG));
    }
break;
case 36:
#line 321 "WQL.y"
{
	globalParserState->statement->appendOperand(
	    WQLOperand(yyvsp[0].intValue != 0, WQLOperand::BOOLEAN_VALUE_TAG));
    }
break;
#line 669 "y.tab.c"
    }
    yyssp -= yym;
    yystate = *yyssp;
    yyvsp -= yym;
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: after reduction, shifting from state 0 to\
 state %d\n", YYPREFIX, YYFINAL);
#endif
        yystate = YYFINAL;
        *++yyssp = YYFINAL;
        *++yyvsp = yyval;
        if (yychar < 0)
        {
            if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
            if (yydebug)
            {
                yys = 0;
                if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
                if (!yys) yys = "illegal-symbol";
                printf("%sdebug: state %d, reading %d (%s)\n",
                        YYPREFIX, YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == 0) goto yyaccept;
        goto yyloop;
    }
    if ((yyn = yygindex[yym]) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: after reduction, shifting from state %d \
to state %d\n", YYPREFIX, *yyssp, yystate);
#endif
    if (yyssp >= yyss + yystacksize - 1)
    {
        goto yyoverflow;
    }
    *++yyssp = yystate;
    *++yyvsp = yyval;
    goto yyloop;
yyoverflow:
    yyerror("yacc stack overflow");
yyabort:
    return (1);
yyaccept:
    return (0);
}
