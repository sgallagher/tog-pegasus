
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
#define	TOK_EQ	262
#define	TOK_NE	263
#define	TOK_LT	264
#define	TOK_LE	265
#define	TOK_GT	266
#define	TOK_GE	267
#define	TOK_NOT	268
#define	TOK_OR	269
#define	TOK_AND	270
#define	TOK_ISA	271
#define	TOK_IDENTIFIER	272
#define	TOK_SELECT	273
#define	TOK_WHERE	274
#define	TOK_FROM	275
#define	TOK_UNEXPECTED_CHAR	276

#line 9 "wql.y"


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


#line 41 "wql.y"
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



#define	YYFINAL		94
#define	YYFLAG		-32768
#define	YYNTBASE	28

#define YYTRANSLATE(x) ((unsigned)(x) <= 276 ? yytranslate[x] : 42)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,    26,
    27,    23,     2,    24,     2,    25,     2,     2,     2,     2,
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
    17,    18,    19,    20,    21,    22
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     6,    11,    14,    16,    19,    21,    23,    25,
    29,    31,    35,    39,    43,    46,    50,    52,    56,    60,
    64,    68,    72,    76,    80,    84,    88,    92,    96,   100,
   104,   108,   112,   116,   120,   124,   128,   132,   136,   140,
   144,   148,   152,   156,   161,   163,   167,   169,   171,   173,
   175,   177
};

static const short yyrhs[] = {    29,
     0,    19,    33,    30,     0,    19,    33,    30,    32,     0,
    21,    31,     0,    18,     0,    20,    36,     0,    34,     0,
    23,     0,    35,     0,    34,    24,    35,     0,    18,     0,
    18,    25,    18,     0,    36,    15,    36,     0,    36,    16,
    36,     0,    14,    36,     0,    26,    36,    27,     0,    37,
     0,    35,    10,    41,     0,    35,    12,    41,     0,    35,
    11,    41,     0,    35,    13,    41,     0,    35,     8,    41,
     0,    35,     9,    41,     0,    41,    10,    35,     0,    41,
    12,    35,     0,    41,    11,    35,     0,    41,    13,    35,
     0,    41,     8,    35,     0,    41,     9,    35,     0,    38,
    10,    41,     0,    38,    12,    41,     0,    38,    11,    41,
     0,    38,    13,    41,     0,    38,     8,    41,     0,    38,
     9,    41,     0,    41,    10,    38,     0,    41,    12,    38,
     0,    41,    11,    38,     0,    41,    13,    38,     0,    41,
     8,    38,     0,    41,     9,    38,     0,    31,    17,    31,
     0,    18,    26,    27,     0,    18,    26,    39,    27,     0,
    40,     0,    39,    24,    40,     0,    35,     0,    41,     0,
    38,     0,     3,     0,     4,     0,     5,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   111,   117,   121,   126,   132,   138,   144,   148,   153,   157,
   163,   167,   173,   177,   181,   185,   189,   195,   199,   203,
   207,   211,   215,   219,   223,   227,   231,   235,   239,   243,
   247,   251,   255,   259,   263,   267,   271,   275,   279,   283,
   287,   291,   297,   301,   307,   311,   317,   318,   319,   323,
   327,   331
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","TOK_INTEGER",
"TOK_DOUBLE","TOK_STRING","TOK_TRUE","TOK_FALSE","TOK_EQ","TOK_NE","TOK_LT",
"TOK_LE","TOK_GT","TOK_GE","TOK_NOT","TOK_OR","TOK_AND","TOK_ISA","TOK_IDENTIFIER",
"TOK_SELECT","TOK_WHERE","TOK_FROM","TOK_UNEXPECTED_CHAR","'*'","','","'.'",
"'('","')'","start","selectStatement","fromClass","className","whereClause",
"propertyListOrStar","propertyList","property","expression","expressionTerm",
"function","functionParameterList","functionParameter","constant", NULL
};
#endif

static const short yyr1[] = {     0,
    28,    29,    29,    30,    31,    32,    33,    33,    34,    34,
    35,    35,    36,    36,    36,    36,    36,    37,    37,    37,
    37,    37,    37,    37,    37,    37,    37,    37,    37,    37,
    37,    37,    37,    37,    37,    37,    37,    37,    37,    37,
    37,    37,    38,    38,    39,    39,    40,    40,    40,    41,
    41,    41
};

static const short yyr2[] = {     0,
     1,     3,     4,     2,     1,     2,     1,     1,     1,     3,
     1,     3,     3,     3,     2,     3,     1,     3,     3,     3,
     3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
     3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
     3,     3,     3,     4,     1,     3,     1,     1,     1,     1,
     1,     1
};

static const short yydefact[] = {     0,
     0,     1,    11,     8,     0,     7,     9,     0,     0,     2,
     0,    12,     5,     4,     0,     3,    10,    50,    51,    52,
     0,    11,     0,     0,     0,     6,    17,     0,     0,    15,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,    11,    43,    47,    49,     0,    45,    48,
    16,    42,    22,    23,    18,    20,    19,    21,    13,    14,
    34,    35,    30,    32,    31,    33,    28,    40,    29,    41,
    24,    36,    26,    38,    25,    37,    27,    39,     0,    44,
    46,     0,     0,     0
};

static const short yydefgoto[] = {    92,
     2,    10,    24,    16,     5,     6,    25,    26,    27,    28,
    58,    59,    29
};

static const short yypact[] = {    -8,
    11,-32768,    -7,-32768,     7,    14,-32768,    17,    27,    44,
    57,-32768,-32768,-32768,    39,-32768,-32768,-32768,-32768,-32768,
    39,    41,    39,    66,    69,    25,-32768,    81,    87,-32768,
    28,    10,    27,    65,    65,    65,    65,    65,    65,    39,
    39,    65,    65,    65,    65,    65,    65,    67,    67,    67,
    67,    67,    67,    46,-32768,-32768,-32768,    12,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,    68,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,    58,-32768,
-32768,    86,   101,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,    -6,-32768,-32768,-32768,    -1,    33,-32768,   -29,
-32768,    -2,   -30
};


#define	YYLAST		101


static const short yytable[] = {     7,
    60,    57,    14,    63,    64,    65,    66,    67,    68,    17,
     1,    71,    72,    73,    74,    75,    76,     8,    78,    80,
    82,    84,    86,    88,    40,    41,    62,     9,     3,    56,
    18,    19,    20,     4,    12,    89,    61,    11,    90,    40,
    41,    18,    19,    20,    13,    54,    77,    79,    81,    83,
    85,    87,    21,    30,    55,    32,    22,    -5,    60,    57,
    18,    19,    20,    15,    23,     8,    31,    18,    19,    20,
     8,    31,    69,    70,     3,    54,    34,    35,    36,    37,
    38,    39,    33,    41,    54,    93,    91,    56,    42,    43,
    44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
    94
};

static const short yycheck[] = {     1,
    31,    31,     9,    34,    35,    36,    37,    38,    39,    11,
    19,    42,    43,    44,    45,    46,    47,    25,    48,    49,
    50,    51,    52,    53,    15,    16,    33,    21,    18,    31,
     3,     4,     5,    23,    18,    24,    27,    24,    27,    15,
    16,     3,     4,     5,    18,    18,    48,    49,    50,    51,
    52,    53,    14,    21,    27,    23,    18,    17,    89,    89,
     3,     4,     5,    20,    26,    25,    26,     3,     4,     5,
    25,    26,    40,    41,    18,    18,     8,     9,    10,    11,
    12,    13,    17,    16,    18,     0,    89,    89,     8,     9,
    10,    11,    12,    13,     8,     9,    10,    11,    12,    13,
     0
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
#line 112 "wql.y"
{
	WQL_TRACE(("YACC: start: selectStatement\n"));
    ;
    break;}
case 2:
#line 118 "wql.y"
{
	WQL_TRACE(("YACC: selectStatement\n"));
    ;
    break;}
case 3:
#line 122 "wql.y"
{
	WQL_TRACE(("YACC: selectStatement\n"));
    ;
    break;}
case 4:
#line 127 "wql.y"
{
	WQL_TRACE(("YACC: fromClass : TOK_FROM %s\n", yyvsp[0].strValue));
	yyval.strValue = yyvsp[0].strValue;
    ;
    break;}
case 5:
#line 133 "wql.y"
{
	WQL_TRACE(("YACC: className : %s\n", yyvsp[0].strValue));
	yyval.strValue = yyvsp[0].strValue
    ;
    break;}
case 6:
#line 139 "wql.y"
{
	WQL_TRACE(("YACC: whereClause : TOK_WHERE expression\n"));
    ;
    break;}
case 7:
#line 145 "wql.y"
{

    ;
    break;}
case 8:
#line 149 "wql.y"
{
	WQL_TRACE(("YACC: propertyListOrStar: '*'\n"));
    ;
    break;}
case 9:
#line 154 "wql.y"
{

    ;
    break;}
case 10:
#line 158 "wql.y"
{

    ;
    break;}
case 11:
#line 164 "wql.y"
{

    ;
    break;}
case 12:
#line 168 "wql.y"
{

    ;
    break;}
case 13:
#line 174 "wql.y"
{

    ;
    break;}
case 14:
#line 178 "wql.y"
{

    ;
    break;}
case 15:
#line 182 "wql.y"
{

    ;
    break;}
case 16:
#line 186 "wql.y"
{

    ;
    break;}
case 17:
#line 190 "wql.y"
{

    ;
    break;}
case 18:
#line 196 "wql.y"
{

    ;
    break;}
case 19:
#line 200 "wql.y"
{

    ;
    break;}
case 20:
#line 204 "wql.y"
{

    ;
    break;}
case 21:
#line 208 "wql.y"
{

    ;
    break;}
case 22:
#line 212 "wql.y"
{

    ;
    break;}
case 23:
#line 216 "wql.y"
{

    ;
    break;}
case 24:
#line 220 "wql.y"
{

    ;
    break;}
case 25:
#line 224 "wql.y"
{

    ;
    break;}
case 26:
#line 228 "wql.y"
{

    ;
    break;}
case 27:
#line 232 "wql.y"
{

    ;
    break;}
case 28:
#line 236 "wql.y"
{

    ;
    break;}
case 29:
#line 240 "wql.y"
{

    ;
    break;}
case 30:
#line 244 "wql.y"
{

    ;
    break;}
case 31:
#line 248 "wql.y"
{

    ;
    break;}
case 32:
#line 252 "wql.y"
{

    ;
    break;}
case 33:
#line 256 "wql.y"
{

    ;
    break;}
case 34:
#line 260 "wql.y"
{

    ;
    break;}
case 35:
#line 264 "wql.y"
{

    ;
    break;}
case 36:
#line 268 "wql.y"
{

    ;
    break;}
case 37:
#line 272 "wql.y"
{

    ;
    break;}
case 38:
#line 276 "wql.y"
{

    ;
    break;}
case 39:
#line 280 "wql.y"
{

    ;
    break;}
case 40:
#line 284 "wql.y"
{

    ;
    break;}
case 41:
#line 288 "wql.y"
{

    ;
    break;}
case 42:
#line 292 "wql.y"
{

    ;
    break;}
case 43:
#line 298 "wql.y"
{

    ;
    break;}
case 44:
#line 302 "wql.y"
{

    ;
    break;}
case 45:
#line 308 "wql.y"
{

    ;
    break;}
case 46:
#line 312 "wql.y"
{

    ;
    break;}
case 50:
#line 324 "wql.y"
{

    ;
    break;}
case 51:
#line 328 "wql.y"
{

    ;
    break;}
case 52:
#line 332 "wql.y"
{
	WQL_TRACE(("YACC: TOK_STRING: %s\n", yyvsp[0].strValue));
	yyval.nodeValue = yyvsp[0].strValue;
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
#line 337 "wql.y"


int WQL_error(char* errorMessage)
{
    fprintf(stderr, "WQL_error: %s\n", errorMessage);
    return -1;
}
