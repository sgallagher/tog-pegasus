
/*  A Bison parser, made from wql.y
    by GNU Bison version 1.28  */

#define YYBISON 1  /* Identify Bison output.  */

#define yyparse WQL_parse
#define yylex WQL_lex
#define yyerror WQL_error
#define yylval WQL_lval
#define yychar WQL_char
#define yydebug WQL_debug
#define yynerrs WQL_nerrs
#define	TOK_INTEGER	257
#define	TOK_DOUBLE	258
#define	TOK_STRING	259
#define	TOK_TRUE	260
#define	TOK_FALSE	261
#define	TOK_NULL	262
#define	TOK_ISA	263
#define	TOK_DOT	264
#define	TOK_EQ	265
#define	TOK_NE	266
#define	TOK_LT	267
#define	TOK_LE	268
#define	TOK_GT	269
#define	TOK_GE	270
#define	TOK_NOT	271
#define	TOK_OR	272
#define	TOK_AND	273
#define	TOK_IS	274
#define	TOK_IDENTIFIER	275
#define	TOK_SELECT	276
#define	TOK_WHERE	277
#define	TOK_FROM	278
#define	TOK_UNEXPECTED_CHAR	279

#line 37 "wql.y"


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
extern int WQL_error(const char*);

//
// Define the global parser state object:
//

PEGASUS_USING_PEGASUS;

PEGASUS_NAMESPACE_BEGIN

extern WQLParserState* globalParserState;

PEGASUS_NAMESPACE_END


#line 86 "wql.y"
typedef union 
{
   int intValue;
   double doubleValue;
   char* strValue;
   void* nodeValue;
} YYSTYPE;
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		65
#define	YYFLAG		-32768
#define	YYNTBASE	30

#define YYTRANSLATE(x) ((unsigned)(x) <= 279 ? yytranslate[x] : 45)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,    28,
    29,    26,     2,    27,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     3,     4,     5,     6,
     7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
    17,    18,    19,    20,    21,    22,    23,    24,    25
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     6,     8,    10,    12,    16,    19,    21,    24,
    27,    31,    35,    38,    42,    44,    48,    53,    55,    57,
    61,    65,    69,    73,    77,    81,    85,    89,    94,    96,
    98,   100,   104,   106,   108,   110,   112,   114
};

static const short yyrhs[] = {    31,
     0,    22,    32,    34,     0,    26,     0,    33,     0,    42,
     0,    33,    27,    42,     0,    35,    36,     0,    35,     0,
    24,    43,     0,    23,    37,     0,    37,    18,    37,     0,
    37,    19,    37,     0,    17,    37,     0,    28,    37,    29,
     0,    38,     0,    38,    20,    41,     0,    38,    20,    17,
    41,     0,    39,     0,    40,     0,    44,    13,    44,     0,
    44,    15,    44,     0,    44,    14,    44,     0,    44,    16,
    44,     0,    44,    11,    44,     0,    44,    12,    44,     0,
    42,     9,    43,     0,    44,    20,     8,     0,    44,    20,
    17,     8,     0,     6,     0,     7,     0,    21,     0,    21,
    10,    21,     0,    21,     0,    42,     0,     3,     0,     4,
     0,     5,     0,    41,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   160,   166,   172,   176,   182,   186,   192,   196,   202,   209,
   215,   220,   225,   231,   235,   239,   244,   253,   257,   263,
   268,   273,   278,   283,   288,   293,   303,   308,   315,   319,
   332,   337,   349,   356,   362,   367,   372,   377
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","TOK_INTEGER",
"TOK_DOUBLE","TOK_STRING","TOK_TRUE","TOK_FALSE","TOK_NULL","TOK_ISA","TOK_DOT",
"TOK_EQ","TOK_NE","TOK_LT","TOK_LE","TOK_GT","TOK_GE","TOK_NOT","TOK_OR","TOK_AND",
"TOK_IS","TOK_IDENTIFIER","TOK_SELECT","TOK_WHERE","TOK_FROM","TOK_UNEXPECTED_CHAR",
"'*'","','","'('","')'","start","selectStatement","selectList","propertyList",
"selectExpression","fromClause","whereClause","searchCondition","predicate",
"comparisonPredicate","nullPredicate","truthValue","propertyName","className",
"comparisonTerm", NULL
};
#endif

static const short yyr1[] = {     0,
    30,    31,    32,    32,    33,    33,    34,    34,    35,    36,
    37,    37,    37,    37,    37,    37,    37,    38,    38,    39,
    39,    39,    39,    39,    39,    39,    40,    40,    41,    41,
    42,    42,    43,    44,    44,    44,    44,    44
};

static const short yyr2[] = {     0,
     1,     3,     1,     1,     1,     3,     2,     1,     2,     2,
     3,     3,     2,     3,     1,     3,     4,     1,     1,     3,
     3,     3,     3,     3,     3,     3,     3,     4,     1,     1,
     1,     3,     1,     1,     1,     1,     1,     1
};

static const short yydefact[] = {     0,
     0,     1,    31,     3,     0,     4,     5,     0,     0,     2,
     8,     0,    32,    33,     9,     0,     7,     6,    35,    36,
    37,    29,    30,     0,     0,    10,    15,    18,    19,    38,
    34,     0,    13,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,    14,    11,    12,     0,    16,
    26,    34,    24,    25,    20,    22,    21,    23,    27,     0,
    17,    28,     0,     0,     0
};

static const short yydefgoto[] = {    63,
     2,     5,     6,    10,    11,    17,    26,    27,    28,    29,
    30,    52,    15,    32
};

static const short yypact[] = {   -12,
    -7,-32768,     6,-32768,    -2,    12,-32768,    25,    39,-32768,
     4,    40,-32768,-32768,-32768,     0,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,     0,     0,    38,    42,-32768,-32768,-32768,
    54,    29,-32768,     7,     0,     0,    31,    39,    26,    26,
    26,    26,    26,    26,     1,-32768,    45,-32768,    52,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,    57,
-32768,-32768,    66,    67,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,-32768,-32768,-32768,-32768,   -23,-32768,-32768,-32768,
   -29,    -1,    30,    11
};


#define	YYLAST		68


static const short yytable[] = {     7,
    33,    34,    19,    20,    21,    22,    23,    50,    59,     1,
    18,    47,    48,     3,    31,     8,    24,    60,     4,    61,
     3,     9,    31,    31,    35,    36,    16,    25,    19,    20,
    21,    22,    23,    31,    31,    46,    22,    23,    12,    39,
    40,    41,    42,    43,    44,    13,     3,    49,    45,    53,
    54,    55,    56,    57,    58,    35,    36,    22,    23,    14,
     3,    37,    38,    36,    62,    64,    65,    51
};

static const short yycheck[] = {     1,
    24,    25,     3,     4,     5,     6,     7,    37,     8,    22,
    12,    35,    36,    21,    16,    10,    17,    17,    26,    49,
    21,    24,    24,    25,    18,    19,    23,    28,     3,     4,
     5,     6,     7,    35,    36,    29,     6,     7,    27,    11,
    12,    13,    14,    15,    16,    21,    21,    17,    20,    39,
    40,    41,    42,    43,    44,    18,    19,     6,     7,    21,
    21,    20,     9,    19,     8,     0,     0,    38
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "bison.simple"
/* This file comes from bison-@bison_version@.  */

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

#ifndef YYSTACK_USE_ALLOCA
#ifdef alloca
#define YYSTACK_USE_ALLOCA
#else /* alloca not defined */
#ifdef __GNUC__
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi) || (defined (__sun) && defined (__i386))
#define YYSTACK_USE_ALLOCA
#include <alloca.h>
#else /* not sparc */
/* We think this test detects Watcom and Microsoft C.  */
/* This used to test MSDOS, but that is a bad idea
   since that symbol is in the user namespace.  */
#if (defined (_MSDOS) || defined (_MSDOS_)) && !defined (__TURBOC__)
#if 0 /* No need for malloc.h, which pollutes the namespace;
	 instead, just don't use alloca.  */
#include <malloc.h>
#endif
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
/* I don't know what this was needed for, but it pollutes the namespace.
   So I turned it off.   rms, 2 May 1997.  */
/* #include <malloc.h>  */
 #pragma alloca
#define YYSTACK_USE_ALLOCA
#else /* not MSDOS, or __TURBOC__, or _AIX */
#if 0
#ifdef __hpux /* haible@ilog.fr says this works for HPUX 9.05 and up,
		 and on HPUX 10.  Eventually we can turn this on.  */
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#endif /* __hpux */
#endif
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc */
#endif /* not GNU C */
#endif /* alloca not defined */
#endif /* YYSTACK_USE_ALLOCA not defined */

#ifdef YYSTACK_USE_ALLOCA
#define YYSTACK_ALLOC alloca
#else
#define YYSTACK_ALLOC malloc
#endif

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Define __yy_memcpy.  Note that the size argument
   should be passed with type unsigned int, because that is what the non-GCC
   definitions require.  With GCC, __builtin_memcpy takes an arg
   of type size_t, but it can handle unsigned int.  */

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (to, from, count)
     char *to;
     char *from;
     unsigned int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *to, char *from, unsigned int count)
{
  register char *t = to;
  register char *f = from;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 217 "bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
#ifdef YYPARSE_PARAM
int yyparse (void *);
#else
int yyparse (void);
#endif
#endif

int
yyparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;
  int yyfree_stacks = 0;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  if (yyfree_stacks)
	    {
	      free (yyss);
	      free (yyvs);
#ifdef YYLSP_NEEDED
	      free (yyls);
#endif
	    }
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
#ifndef YYSTACK_USE_ALLOCA
      yyfree_stacks = 1;
#endif
      yyss = (short *) YYSTACK_ALLOC (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1,
		   size * (unsigned int) sizeof (*yyssp));
      yyvs = (YYSTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1,
		   size * (unsigned int) sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1,
		   size * (unsigned int) sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 1:
#line 161 "wql.y"
{
	WQL_TRACE(("YACC: start\n"));
    ;
    break;}
case 2:
#line 167 "wql.y"
{

    ;
    break;}
case 3:
#line 173 "wql.y"
{
	globalParserState->statement->setAllProperties(true);
    ;
    break;}
case 4:
#line 177 "wql.y"
{

    ;
    break;}
case 5:
#line 183 "wql.y"
{
	globalParserState->statement->appendSelectPropertyName(CIMName(yyvsp[0].strValue));
    ;
    break;}
case 6:
#line 187 "wql.y"
{
	globalParserState->statement->appendSelectPropertyName(CIMName(yyvsp[0].strValue));
    ;
    break;}
case 7:
#line 193 "wql.y"
{

    ;
    break;}
case 8:
#line 197 "wql.y"
{

    ;
    break;}
case 9:
#line 203 "wql.y"
{
	WQL_TRACE(("YACC: fromClause: TOK_FROM className(%s)\n", yyvsp[0].strValue));
	globalParserState->statement->setClassName(CIMName(yyvsp[0].strValue));
    ;
    break;}
case 10:
#line 210 "wql.y"
{

    ;
    break;}
case 11:
#line 216 "wql.y"
{
	WQL_TRACE(("YACC: TOK_OR\n"));
	globalParserState->statement->appendOperation(WQL_OR);
    ;
    break;}
case 12:
#line 221 "wql.y"
{
	WQL_TRACE(("YACC: TOK_AND\n"));
	globalParserState->statement->appendOperation(WQL_AND);
    ;
    break;}
case 13:
#line 226 "wql.y"
{
	WQL_TRACE(("YACC: TOK_NOT\n"));

	globalParserState->statement->appendOperation(WQL_NOT);
    ;
    break;}
case 14:
#line 232 "wql.y"
{

    ;
    break;}
case 15:
#line 236 "wql.y"
{

    ;
    break;}
case 16:
#line 240 "wql.y"
{
	WQLOperation op = yyvsp[0].intValue ? WQL_IS_TRUE : WQL_IS_FALSE;
	globalParserState->statement->appendOperation(op);
    ;
    break;}
case 17:
#line 245 "wql.y"
{
	WQLOperation op = yyvsp[0].intValue ? WQL_IS_NOT_TRUE : WQL_IS_NOT_FALSE;
	globalParserState->statement->appendOperation(op);
    ;
    break;}
case 18:
#line 254 "wql.y"
{

    ;
    break;}
case 19:
#line 258 "wql.y"
{

    ;
    break;}
case 20:
#line 264 "wql.y"
{
	WQL_TRACE(("YACC: TOK_LT\n"));
	globalParserState->statement->appendOperation(WQL_LT);
    ;
    break;}
case 21:
#line 269 "wql.y"
{
	WQL_TRACE(("YACC: TOK_GT\n"));
	globalParserState->statement->appendOperation(WQL_GT);
    ;
    break;}
case 22:
#line 274 "wql.y"
{
	WQL_TRACE(("YACC: TOK_LE\n"));
	globalParserState->statement->appendOperation(WQL_LE);
    ;
    break;}
case 23:
#line 279 "wql.y"
{
	WQL_TRACE(("YACC: TOK_GE\n"));
	globalParserState->statement->appendOperation(WQL_GE);
    ;
    break;}
case 24:
#line 284 "wql.y"
{
	WQL_TRACE(("YACC: TOK_EQ\n"));
	globalParserState->statement->appendOperation(WQL_EQ);
    ;
    break;}
case 25:
#line 289 "wql.y"
{
	WQL_TRACE(("YACC: TOK_NE\n"));
	globalParserState->statement->appendOperation(WQL_NE);
    ;
    break;}
case 26:
#line 294 "wql.y"
{
    WQL_TRACE(("YACC: TOK_ISA\n"));
#ifndef PEGASUS_SNIA_EXTENSIONS
        // If SNIA tests, allow the ISA but do not pass className
        yyerror("ISA Token Not Supported");
#endif
    ;
    break;}
case 27:
#line 304 "wql.y"
{
	WQL_TRACE(("YACC: nullPredicate : comparisonTerm IS NULL\n"));
	globalParserState->statement->appendOperation(WQL_IS_NULL);
    ;
    break;}
case 28:
#line 309 "wql.y"
{
	WQL_TRACE(("YACC: nullPredicate : comparisonTerm IS NOT NULL\n"));
	globalParserState->statement->appendOperation(WQL_IS_NOT_NULL);
    ;
    break;}
case 29:
#line 316 "wql.y"
{
	yyval.intValue = 1;
    ;
    break;}
case 30:
#line 320 "wql.y"
{
	yyval.intValue = 0;
    ;
    break;}
case 31:
#line 333 "wql.y"
{
        WQL_TRACE(("YACC: propertyName : TOK_IDENTIFIER(%s)\n", yyvsp[0].strValue));
        yyval.strValue = yyvsp[0].strValue;
    ;
    break;}
case 32:
#line 338 "wql.y"
{
        WQL_TRACE(("YACC: propertyName : TOK_IDENTIFIER(%s.%s)\n", yyvsp[-2].strValue, yyvsp[0].strValue));
#ifdef PEGASUS_SNIA_EXTENSIONS
        // Pass anything as a property name to fool parser for SNIA testing
        yyval.strValue = strdup("dummy");
#else
        yyerror("Scoped (dotted) property names not supported");
#endif
    ;
    break;}
case 33:
#line 350 "wql.y"
{
	WQL_TRACE(("YACC: TOK_IDENTIFIER %s\n", yyvsp[0].strValue));
	yyval.strValue = yyvsp[0].strValue;
    ;
    break;}
case 34:
#line 357 "wql.y"
{
	globalParserState->statement->appendOperand(
	    WQLOperand(yyvsp[0].strValue, WQL_PROPERTY_NAME_TAG));
	globalParserState->statement->appendWherePropertyName(CIMName(yyvsp[0].strValue));
    ;
    break;}
case 35:
#line 363 "wql.y"
{
	globalParserState->statement->appendOperand(
	    WQLOperand(yyvsp[0].intValue, WQL_INTEGER_VALUE_TAG));
    ;
    break;}
case 36:
#line 368 "wql.y"
{
	globalParserState->statement->appendOperand(
	    WQLOperand(yyvsp[0].doubleValue, WQL_DOUBLE_VALUE_TAG));
    ;
    break;}
case 37:
#line 373 "wql.y"
{
	globalParserState->statement->appendOperand(
	    WQLOperand(yyvsp[0].strValue, WQL_STRING_VALUE_TAG));
    ;
    break;}
case 38:
#line 378 "wql.y"
{
	globalParserState->statement->appendOperand(
	    WQLOperand(yyvsp[0].intValue != 0, WQL_BOOLEAN_VALUE_TAG));
    ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 543 "bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;

 yyacceptlab:
  /* YYACCEPT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 0;

 yyabortlab:
  /* YYABORT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 1;
}
#line 383 "wql.y"

