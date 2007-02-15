//%2006////////////////////////////////////////////////////////////////////////
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
//==============================================================================
/* A Bison parser, made by GNU Bison 1.875c.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003 Free Software Foundation, Inc.

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

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0

/* If NAME_PREFIX is specified substitute the variables and functions
   names.  */
#define yyparse urlparse
#define yylex   urllex
#define yyerror urlerror
#define yylval  urllval
#define yychar  urlchar
#define yydebug urldebug
#define yynerrs urlnerrs


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     _RESERVED = 258,
     _HEXDIG = 259,
     _STAG = 260,
     _RESNAME = 261,
     _NAME = 262,
     _ELEMENT = 263,
     _IPADDR = 264,
     _IPX = 265,
     _AT = 266,
     _ZONE = 267
   };
#endif
#define _RESERVED 258
#define _HEXDIG 259
#define _STAG 260
#define _RESNAME 261
#define _NAME 262
#define _ELEMENT 263
#define _IPADDR 264
#define _IPX 265
#define _AT 266
#define _ZONE 267




/* Copy the first part of user declarations.  */
#line 66 "url.y"


#include "slp_client.h"



/* prototypes and globals go here */
void urlerror(char *, ...);
int32 urlwrap(void);
int32 urllex(void);   
int32 urlparse(void);
void url_close_lexer(uint32 handle);
size_t url_init_lexer(const char *s);

lslpAtomizedURL urlHead = 
{
	&urlHead, &urlHead, TRUE, NULL, 0
};

static lslpAtomList srvcHead = {&srvcHead, &srvcHead, TRUE, NULL, 0};
static lslpAtomList siteHead = {&siteHead, &siteHead, TRUE, NULL, 0};
static lslpAtomList pathHead = {&pathHead, &pathHead, TRUE, NULL, 0};
static lslpAtomList attrHead = {&attrHead, &attrHead, TRUE, NULL, 0};



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 95 "url.y"
typedef union YYSTYPE {
	int32 _i;
	char *_s;
	lslpAtomList *_atl;
	lslpAtomizedURL *_aturl;
} YYSTYPE;
/* Line 191 of yacc.c.  */
#line 142 "y_url.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 214 of yacc.c.  */
#line 154 "y_url.c"

#if ! defined (yyoverflow) || YYERROR_VERBOSE

# ifndef YYFREE
#  define YYFREE free
# endif
# ifndef YYMALLOC
#  define YYMALLOC malloc
# endif

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   define YYSTACK_ALLOC alloca
#  endif
# else
#  if defined (alloca) || defined (_ALLOCA_H)
#   define YYSTACK_ALLOC alloca
#  else
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (defined (YYSTYPE_IS_TRIVIAL) && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined (__GNUC__) && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  7
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   43

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  19
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  17
/* YYNRULES -- Number of rules. */
#define YYNRULES  35
/* YYNRULES -- Number of states. */
#define YYNSTATES  52

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   267

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,    14,    15,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    13,    17,
       2,    18,     2,     2,    16,     2,     2,     2,     2,     2,
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
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned char yyprhs[] =
{
       0,     0,     3,     6,     8,    11,    14,    19,    21,    24,
      26,    28,    30,    32,    35,    39,    45,    47,    49,    56,
      58,    62,    64,    66,    68,    70,    73,    75,    78,    80,
      83,    86,    89,    91,    94,    97
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      20,     0,    -1,    21,    23,    -1,    22,    -1,    21,    22,
      -1,     6,    13,    -1,     6,    14,     6,    13,    -1,    24,
      -1,    24,    31,    -1,    25,    -1,    27,    -1,    28,    -1,
      26,    -1,    15,    15,    -1,    15,    15,    29,    -1,    15,
      15,     6,    16,    29,    -1,     6,    -1,    10,    -1,    11,
      12,    13,    12,    13,    12,    -1,    30,    -1,    30,    13,
       4,    -1,     6,    -1,     9,    -1,    32,    -1,    34,    -1,
      32,    34,    -1,    33,    -1,    32,    33,    -1,    15,    -1,
      15,     8,    -1,    15,     6,    -1,    15,     4,    -1,    35,
      -1,    34,    35,    -1,    17,     8,    -1,    17,     8,    18,
       8,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   115,   115,   209,   217,   226,   244,   266,   273,   282,
     295,   308,   322,   340,   343,   352,   363,   368,   373,   386,
     389,   404,   407,   412,   415,   418,   423,   430,   440,   444,
     462,   479,   497,   504,   513,   530
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "_RESERVED", "_HEXDIG", "_STAG",
  "_RESNAME", "_NAME", "_ELEMENT", "_IPADDR", "_IPX", "_AT", "_ZONE",
  "':'", "'.'", "'/'", "'@'", "';'", "'='", "$accept", "url",
  "service_list", "service", "sap", "site", "ip_site", "service_id",
  "ipx_site", "at_site", "hostport", "host", "url_part", "path_list",
  "path_el", "attr_list", "attr_el", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,    58,    46,    47,    64,    59,    61
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    19,    20,    21,    21,    22,    22,    23,    23,    24,
      24,    24,    24,    25,    25,    25,    26,    27,    28,    29,
      29,    30,    30,    31,    31,    31,    32,    32,    33,    33,
      33,    33,    34,    34,    35,    35
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     2,     1,     2,     2,     4,     1,     2,     1,
       1,     1,     1,     2,     3,     5,     1,     1,     6,     1,
       3,     1,     1,     1,     1,     2,     1,     2,     1,     2,
       2,     2,     1,     2,     2,     4
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       0,     0,     0,     0,     3,     5,     0,     1,    16,    17,
       0,     0,     4,     2,     7,     9,    12,    10,    11,     0,
       0,    13,    28,     0,     8,    23,    26,    24,    32,     6,
       0,    21,    22,    14,    19,    31,    30,    29,    34,    27,
      25,    33,     0,     0,     0,     0,     0,    21,    15,    20,
      35,    18
};

/* YYDEFGOTO[NTERM-NUM]. */
static const yysigned_char yydefgoto[] =
{
      -1,     2,     3,     4,    13,    14,    15,    16,    17,    18,
      33,    34,    24,    25,    26,    27,    28
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -28
static const yysigned_char yypact[] =
{
      -3,     5,    16,    -5,   -28,   -28,     2,   -28,     5,   -28,
       9,     7,   -28,   -28,   -13,   -28,   -28,   -28,   -28,    10,
      11,     6,     3,    12,   -28,   -13,   -28,    13,   -28,   -28,
      14,    15,   -28,   -28,    19,   -28,   -28,   -28,    17,   -28,
      13,   -28,    20,     8,    21,    26,    24,   -28,   -28,   -28,
     -28,   -28
};

/* YYPGOTO[NTERM-NUM].  */
static const yysigned_char yypgoto[] =
{
     -28,   -28,   -28,    25,   -28,   -28,   -28,   -28,   -28,   -28,
     -16,   -28,   -28,   -28,     4,    18,   -27
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const unsigned char yytable[] =
{
      41,     8,    22,     1,    23,     9,    10,    35,    19,    36,
      11,    37,    31,    41,    47,    32,     7,    32,     5,     6,
      38,    20,    21,    29,    30,    49,    42,    48,    12,    39,
      23,    43,    44,    46,    50,    45,    51,     0,     0,     0,
       0,     0,     0,    40
};

static const yysigned_char yycheck[] =
{
      27,     6,    15,     6,    17,    10,    11,     4,     6,     6,
      15,     8,     6,    40,     6,     9,     0,     9,    13,    14,
       8,    12,    15,    13,    13,     4,    12,    43,     3,    25,
      17,    16,    13,    13,     8,    18,    12,    -1,    -1,    -1,
      -1,    -1,    -1,    25
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     6,    20,    21,    22,    13,    14,     0,     6,    10,
      11,    15,    22,    23,    24,    25,    26,    27,    28,     6,
      12,    15,    15,    17,    31,    32,    33,    34,    35,    13,
      13,     6,     9,    29,    30,     4,     6,     8,     8,    33,
      34,    35,    12,    16,    13,    18,    13,     6,    29,     4,
       8,    12
};

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)		\
   ((Current).first_line   = (Rhs)[1].first_line,	\
    (Current).first_column = (Rhs)[1].first_column,	\
    (Current).last_line    = (Rhs)[N].last_line,	\
    (Current).last_column  = (Rhs)[N].last_column)
#endif

/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)

# define YYDSYMPRINT(Args)			\
do {						\
  if (yydebug)					\
    yysymprint Args;				\
} while (0)

# define YYDSYMPRINTF(Title, Token, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr, 					\
                  Token, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short *bottom, short *top)
#else
static void
yy_stack_print (bottom, top)
    short *bottom;
    short *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (/* Nothing. */; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_reduce_print (int yyrule)
#else
static void
yy_reduce_print (yyrule)
    int yyrule;
#endif
{
  int yyi;
  unsigned int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %u), ",
             yyrule - 1, yylno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname [yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname [yyr1[yyrule]]);
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (Rule);		\
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YYDSYMPRINT(Args)
# define YYDSYMPRINTF(Title, Token, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if defined (YYMAXDEPTH) && YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

#endif /* !YYERROR_VERBOSE */



#if YYDEBUG
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (yytype < YYNTOKENS)
    {
      YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
# ifdef YYPRINT
      YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
    }
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyoutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yydestruct (int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yytype, yyvaluep)
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  switch (yytype)
    {

      default:
        break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM);
# else
int yyparse ();
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM)
# else
int yyparse (YYPARSE_PARAM)
  void *YYPARSE_PARAM;
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YYDSYMPRINTF ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %s, ", yytname[yytoken]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 115 "url.y"
    {
			if (NULL != (yyval._aturl = (lslpAtomizedURL *)calloc(1, sizeof(lslpAtomizedURL))))
			{
				int32 urlLen = 1;
				lslpAtomList *temp = srvcHead.next;
				while (! _LSLP_IS_HEAD(temp) && temp->str != NULL)
				{
					urlLen += strlen(temp->str);
					temp = temp->next;
				}
				temp = siteHead.next;
				while (! _LSLP_IS_HEAD(temp) && temp->str != NULL)
				{
					urlLen += strlen(temp->str);
					temp = temp->next;
				}
				temp = pathHead.next;
				while (! _LSLP_IS_HEAD(temp) && temp->str != NULL)
				{
					urlLen += strlen(temp->str) + 1;
					temp = temp->next;
				}
				temp = attrHead.next;
				while (! _LSLP_IS_HEAD(temp) && temp->str != NULL)
				{
					urlLen += strlen(temp->str) + 1;
					temp = temp->next;
				}
				if (NULL != (yyval._aturl->url = (char *)calloc(urlLen, sizeof(char))))
				{
					temp = srvcHead.next;
					if (! _LSLP_IS_HEAD(temp) && temp->str != NULL)
					{
						strcpy(yyval._aturl->url, temp->str);
						temp = temp->next;
					}
					while (! _LSLP_IS_HEAD(temp) && temp->str != NULL)
					{
						strcat(yyval._aturl->url, temp->str);
						temp = temp->next;
					}
					temp = siteHead.next;
					while (! _LSLP_IS_HEAD(temp) && temp->str != NULL)
					{
						strcat(yyval._aturl->url, temp->str);
						temp = temp->next;
					}
					temp = pathHead.next;
					while (! _LSLP_IS_HEAD(temp) && temp->str != NULL)
					{
						strcat(yyval._aturl->url, "/");
						strcat(yyval._aturl->url, temp->str);
						temp = temp->next;
					}
					temp = attrHead.next;
					while (! _LSLP_IS_HEAD(temp) && temp->str != NULL)
					{
						strcat(yyval._aturl->url, ";");
						strcat(yyval._aturl->url, temp->str);
						temp = temp->next;
					}
					yyval._aturl->urlHash = lslpCheckSum(yyval._aturl->url, (int16)strlen(yyval._aturl->url));
				}

				/* make certain the listheads are initialized */
				yyval._aturl->srvcs.next = yyval._aturl->srvcs.prev = &(yyval._aturl->srvcs);
				yyval._aturl->srvcs.isHead = TRUE;
				yyval._aturl->site.next = yyval._aturl->site.prev = &(yyval._aturl->site);
				yyval._aturl->site.isHead = TRUE;
				yyval._aturl->path.next = yyval._aturl->path.prev = &(yyval._aturl->path);
				yyval._aturl->path.isHead = TRUE;
				yyval._aturl->attrs.next = yyval._aturl->attrs.prev = &(yyval._aturl->attrs);
				yyval._aturl->attrs.isHead = TRUE;
				if (! _LSLP_IS_EMPTY(&srvcHead ))
				{
					_LSLP_LINK_HEAD(&(yyval._aturl->srvcs), &srvcHead);
				}
				if (! _LSLP_IS_EMPTY(&siteHead))
				{
					_LSLP_LINK_HEAD(&(yyval._aturl->site), &siteHead);
				}
				if (! _LSLP_IS_EMPTY(&pathHead))
				{
					_LSLP_LINK_HEAD(&(yyval._aturl->path), &pathHead);
				}
				if (! _LSLP_IS_EMPTY(&attrHead))
				{
					_LSLP_LINK_HEAD(&(yyval._aturl->attrs), &attrHead);
				}
				_LSLP_INSERT_BEFORE(yyval._aturl, &urlHead);
				lslpInitAtomLists();
			}
		;}
    break;

  case 3:
#line 209 "url.y"
    {
			yyval._atl = &srvcHead;
			if (yyvsp[0]._atl != NULL)
			{
				_LSLP_INSERT_BEFORE(yyvsp[0]._atl, yyval._atl);
			}
		;}
    break;

  case 4:
#line 217 "url.y"
    {
			yyval._atl = &srvcHead;
			if (yyvsp[0]._atl != NULL)
			{
				_LSLP_INSERT_BEFORE(yyvsp[0]._atl, yyval._atl);
			}
		;}
    break;

  case 5:
#line 226 "url.y"
    {
			if (NULL != (yyval._atl = (lslpAtomList *)calloc(1, sizeof(lslpAtomList))))
			{
				yyval._atl->next = yyval._atl->prev = yyval._atl;
				if (NULL != (yyval._atl->str = (char *)calloc(2 + strlen(yyvsp[-1]._s), sizeof(char))))
				{
					strcpy(yyval._atl->str, yyvsp[-1]._s);
					strcat(yyval._atl->str, ":");	
					yyval._atl->hash = lslpCheckSum(yyval._atl->str, (int16)strlen(yyval._atl->str));
				}
				else
				{
					free(yyval._atl);
					yyval._atl = NULL;
				}
			}

		;}
    break;

  case 6:
#line 244 "url.y"
    {
			if (NULL != (yyval._atl = (lslpAtomList *)calloc(1, sizeof(lslpAtomList))))
			{
				yyval._atl->next = yyval._atl->prev = yyval._atl;
				if (NULL != (yyval._atl->str = (char *)calloc(3 + strlen(yyvsp[-3]._s) + strlen(yyvsp[-1]._s), sizeof(char))))
				{
					strcpy(yyval._atl->str, yyvsp[-3]._s);
					strcat(yyval._atl->str, ".");
					strcat(yyval._atl->str, yyvsp[-1]._s);
					strcat(yyval._atl->str, ":");	
					yyval._atl->hash = lslpCheckSum(yyval._atl->str, (int16)strlen(yyval._atl->str));
				}
				else
				{
					free(yyval._atl);
					yyval._atl = NULL;
				}
			}

		;}
    break;

  case 7:
#line 266 "url.y"
    {
			yyval._atl = &siteHead;
			if (yyvsp[0]._atl != NULL)
			{
				_LSLP_INSERT_BEFORE(yyvsp[0]._atl, yyval._atl);
			}
		;}
    break;

  case 8:
#line 273 "url.y"
    {
			yyval._atl = &siteHead;
			if (yyvsp[-1]._atl != NULL)
			{
					_LSLP_INSERT_BEFORE(yyvsp[-1]._atl, yyval._atl);
			}
		;}
    break;

  case 9:
#line 282 "url.y"
    {
			if (yyvsp[0]._s != NULL)
			{
				if(NULL != (yyval._atl = (lslpAtomList *)calloc(1, sizeof(lslpAtomList))))
				{
					yyval._atl->next = yyval._atl->prev = yyval._atl;
					yyval._atl->str = yyvsp[0]._s;
					yyval._atl->hash = lslpCheckSum(yyval._atl->str, (int16)strlen(yyval._atl->str));
				}
			}
			else
				yyval._atl = NULL;
		;}
    break;

  case 10:
#line 295 "url.y"
    {
			if (yyvsp[0]._s != NULL)
			{
				if(NULL != (yyval._atl = (lslpAtomList *)calloc(1, sizeof(lslpAtomList))))
				{
					yyval._atl->next = yyval._atl->prev = yyval._atl;
					yyval._atl->str = yyvsp[0]._s;
					yyval._atl->hash = lslpCheckSum(yyval._atl->str, (int16)strlen(yyval._atl->str));
				}
			}
			else
				yyval._atl = NULL;
		;}
    break;

  case 11:
#line 308 "url.y"
    {
			if (yyvsp[0]._s != NULL)
			{
				if(NULL != (yyval._atl = (lslpAtomList *)calloc(1, sizeof(lslpAtomList))))
				{
					yyval._atl->next = yyval._atl->prev = yyval._atl;
					yyval._atl->str = yyvsp[0]._s;
					yyval._atl->hash = lslpCheckSum(yyval._atl->str, (int16)strlen(yyval._atl->str));
				}
			}
			else
				yyval._atl = NULL;
		;}
    break;

  case 12:
#line 322 "url.y"
    {
  
			if (yyvsp[0]._s != NULL)
			{
				if(NULL != (yyval._atl = (lslpAtomList *)calloc(1, sizeof(lslpAtomList))))
				{
					yyval._atl->next = yyval._atl->prev = yyval._atl;
					yyval._atl->str = yyvsp[0]._s;
					yyval._atl->hash = lslpCheckSum(yyval._atl->str, (int16)strlen(yyval._atl->str));
				}
			}
			else
				yyval._atl = NULL;
  
;}
    break;

  case 13:
#line 340 "url.y"
    {
			yyval._s = strdup("//");
		;}
    break;

  case 14:
#line 343 "url.y"
    {
			if(NULL != yyvsp[0]._s && (NULL !=(yyval._s = (char *)calloc(3 + strlen(yyvsp[0]._s), sizeof(char)))))
			{
				strcpy(yyval._s, "//");
				strcat(yyval._s, yyvsp[0]._s);
				free(yyvsp[0]._s);
			}

		;}
    break;

  case 15:
#line 352 "url.y"
    {
			if(NULL != yyvsp[0]._s && (NULL !=(yyval._s = (char *)calloc(4 + strlen(yyvsp[-2]._s) + strlen(yyvsp[0]._s), sizeof(char)))))
			{
				strcpy(yyval._s, "//");
				strcat(yyval._s, yyvsp[-2]._s);
				strcat(yyval._s, "@");
				strcat(yyval._s, yyvsp[0]._s);
			}
		;}
    break;

  case 16:
#line 363 "url.y"
    {
	  yyval._s = strdup(yyvsp[0]._s);
	;}
    break;

  case 17:
#line 368 "url.y"
    {
			yyval._s = yyvsp[0]._s;
		;}
    break;

  case 18:
#line 373 "url.y"
    {
			if(NULL != (yyval._s = (char *)calloc(strlen(yyvsp[-5]._s) + strlen(yyvsp[-4]._s) + strlen(yyvsp[-2]._s) + strlen(yyvsp[0]._s) + 3, sizeof(char))))
			{
				strcpy(yyval._s, yyvsp[-5]._s);
				strcat(yyval._s, yyvsp[-4]._s);
				strcat(yyval._s, ":");
				strcat(yyval._s, yyvsp[-2]._s);
				strcat(yyval._s, ":");
				strcat(yyval._s, yyvsp[0]._s);
			}
		;}
    break;

  case 19:
#line 386 "url.y"
    {
			yyval._s = yyvsp[0]._s;	
		;}
    break;

  case 20:
#line 389 "url.y"
    {
			if (yyvsp[-2]._s != NULL)
			{
				if(NULL != (yyval._s = (char *)calloc(strlen(yyvsp[-2]._s) + strlen(yyvsp[0]._s) + 2, sizeof(char))))
				{
					strcpy(yyval._s, yyvsp[-2]._s);
					strcat(yyval._s, ":");
					strcat(yyval._s, yyvsp[0]._s);
				}
			}
			else
				yyval._s = NULL;	
		;}
    break;

  case 21:
#line 404 "url.y"
    {
			yyval._s = yyvsp[0]._s;
		;}
    break;

  case 22:
#line 407 "url.y"
    {
			yyval._s = yyvsp[0]._s;
		;}
    break;

  case 23:
#line 412 "url.y"
    {
			;	
		;}
    break;

  case 24:
#line 415 "url.y"
    {
			;		
		;}
    break;

  case 25:
#line 418 "url.y"
    {
			;
		;}
    break;

  case 26:
#line 423 "url.y"
    {
			yyval._atl = &pathHead;
			if (yyvsp[0]._atl != NULL)
			{
				_LSLP_INSERT_BEFORE(yyvsp[0]._atl, yyval._atl);
			}
		;}
    break;

  case 27:
#line 430 "url.y"
    {
			yyval._atl = &pathHead;
			if (yyvsp[0]._atl != NULL)
			{
				_LSLP_INSERT_BEFORE(yyvsp[0]._atl, yyval._atl);
			}
		;}
    break;

  case 28:
#line 440 "url.y"
    {
			 /* dangling path slash - do nothing */
			 yyval._atl = NULL;
			 ;}
    break;

  case 29:
#line 444 "url.y"
    {
			if(NULL != (yyval._atl = (lslpAtomList *)calloc(1, sizeof(lslpAtomList))))
			{
				yyval._atl->prev = yyval._atl->next = yyval._atl;
				if(NULL != (yyval._atl->str = (char *)calloc(1 + strlen(yyvsp[0]._s), sizeof(char))))
				{
					strcpy(yyval._atl->str, yyvsp[0]._s);
					yyval._atl->hash = lslpCheckSum(yyval._atl->str, (int16)strlen(yyval._atl->str));

				}
				else
				{
					free(yyval._atl);
					yyval._atl = NULL;
				}
			}

		;}
    break;

  case 30:
#line 462 "url.y"
    {
			if(NULL != (yyval._atl = (lslpAtomList *)calloc(1, sizeof(lslpAtomList))))
			{
				yyval._atl->prev = yyval._atl->next = yyval._atl;
				if(NULL != (yyval._atl->str = (char *)calloc(1 + strlen(yyvsp[0]._s), sizeof(char))))
				{
					strcpy(yyval._atl->str, yyvsp[0]._s);
					yyval._atl->hash = lslpCheckSum(yyval._atl->str, (int16)strlen(yyval._atl->str));
				}
				else
				{
					free(yyval._atl);
					yyval._atl = NULL;
				}
			}

		;}
    break;

  case 31:
#line 479 "url.y"
    {
			if(NULL != (yyval._atl = (lslpAtomList *)calloc(1, sizeof(lslpAtomList))))
			{
				yyval._atl->prev = yyval._atl->next = yyval._atl;
				if(NULL != (yyval._atl->str = (char *)calloc(1 + strlen(yyvsp[0]._s), sizeof(char))))
				{
					strcpy(yyval._atl->str, yyvsp[0]._s);
					yyval._atl->hash = lslpCheckSum(yyval._atl->str, (int16)strlen(yyval._atl->str));
				}
				else
				{
					free(yyval._atl);
					yyval._atl = NULL;
				}
			}
		;}
    break;

  case 32:
#line 497 "url.y"
    {
			yyval._atl = &attrHead;
			if (yyvsp[0]._atl != NULL)
			{
				_LSLP_INSERT_BEFORE(yyvsp[0]._atl, yyval._atl);
			}
		;}
    break;

  case 33:
#line 504 "url.y"
    {
			yyval._atl = &attrHead;
			if (yyvsp[0]._atl != NULL)
			{
				_LSLP_INSERT_BEFORE(yyvsp[0]._atl, yyval._atl);
			}
		;}
    break;

  case 34:
#line 513 "url.y"
    {
			if(NULL != (yyval._atl = (lslpAtomList *)calloc(1, sizeof(lslpAtomList))))
			{
				yyval._atl->prev = yyval._atl->next = yyval._atl;
				if(NULL != (yyval._atl->str = (char *)calloc(1 + strlen(yyvsp[0]._s), sizeof(char))))
				{
					strcpy(yyval._atl->str, yyvsp[0]._s);
					yyval._atl->hash = lslpCheckSum(yyval._atl->str, (int16)strlen(yyval._atl->str));
				}
				else
				{
					free(yyval._atl);
					yyval._atl = NULL;
				}
			}
		;}
    break;

  case 35:
#line 530 "url.y"
    {
			if(NULL != (yyval._atl = (lslpAtomList *)calloc(1, sizeof(lslpAtomList))))
			{
				yyval._atl->prev = yyval._atl->next = yyval._atl;
				if(NULL != (yyval._atl->str = (char *)calloc(2 + strlen(yyvsp[-2]._s) + strlen(yyvsp[0]._s), sizeof(char))))
				{
					strcpy(yyval._atl->str, yyvsp[-2]._s);
					strcat(yyval._atl->str, "=");
					strcat(yyval._atl->str, yyvsp[0]._s);
					yyval._atl->hash = lslpCheckSum(yyval._atl->str, (int16)strlen(yyval._atl->str));
				}
				else
				{
					free(yyval._atl);
					yyval._atl = NULL;
				}
			}
		;}
    break;


    }

/* Line 1000 of yacc.c.  */
#line 1610 "y_url.c"

  yyvsp -= yylen;
  yyssp -= yylen;


  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  int yytype = YYTRANSLATE (yychar);
	  const char* yyprefix;
	  char *yymsg;
	  int yyx;

	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  int yyxbegin = yyn < 0 ? -yyn : 0;

	  /* Stay within bounds of both yycheck and yytname.  */
	  int yychecklim = YYLAST - yyn;
	  int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
	  int yycount = 0;

	  yyprefix = ", expecting ";
	  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      {
		yysize += yystrlen (yyprefix) + yystrlen (yytname [yyx]);
		yycount += 1;
		if (yycount == 5)
		  {
		    yysize = 0;
		    break;
		  }
	      }
	  yysize += (sizeof ("syntax error, unexpected ")
		     + yystrlen (yytname[yytype]));
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "syntax error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[yytype]);

	      if (yycount < 5)
		{
		  yyprefix = ", expecting ";
		  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
		    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
		      {
			yyp = yystpcpy (yyp, yyprefix);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yyprefix = " or ";
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("syntax error; also virtual memory exhausted");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror ("syntax error");
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* If at end of input, pop the error token,
	     then the rest of the stack, then return failure.  */
	  if (yychar == YYEOF)
	     for (;;)
	       {
		 YYPOPSTACK;
		 if (yyssp == yyss)
		   YYABORT;
		 YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
		 yydestruct (yystos[*yyssp], yyvsp);
	       }
        }
      else
	{
	  YYDSYMPRINTF ("Error: discarding", yytoken, &yylval, &yylloc);
	  yydestruct (yytoken, &yylval);
	  yychar = YYEMPTY;

	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

#ifdef __GNUC__
  /* Pacify GCC when the user code never invokes YYERROR and the label
     yyerrorlab therefore never appears in user code.  */
  if (0)
     goto yyerrorlab;
#endif

  yyvsp -= yylen;
  yyssp -= yylen;
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
      yydestruct (yystos[yystate], yyvsp);
      YYPOPSTACK;
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;


  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*----------------------------------------------.
| yyoverflowlab -- parser overflow comes here.  |
`----------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}


#line 550 "url.y"
 


lslpAtomList *lslpAllocAtom(void)
{
	lslpAtomList *temp = (lslpAtomList *)calloc(1, sizeof(lslpAtomList));
	if (temp != NULL)
	{
		temp->next = temp->prev = temp;
	}
	return(temp);
}	

lslpAtomList *lslpAllocAtomList(void)
{
	lslpAtomList *temp =lslpAllocAtom(); 
	if (temp != NULL)
		temp->isHead = TRUE;
	return(temp);
}	

void lslpFreeAtom(lslpAtomList *a )
{
	assert(a != NULL);
	if (a->str != NULL)
		free(a->str);
	free(a);
	return;
}	

void lslpFreeAtomList(lslpAtomList *l, int32 flag)
{
	lslpAtomList *temp;
	assert(l != NULL);
	assert(_LSLP_IS_HEAD(l));
	while (! _LSLP_IS_EMPTY(l))
	{
		temp = l->next;
		temp->prev->next = temp->next;
		temp->next->prev = temp->prev; 
		lslpFreeAtom(temp);
	}
	if (flag)
		lslpFreeAtom(l);
	return;
}	

lslpAtomizedURL *lslpAllocAtomizedURL(void)
{
	lslpAtomizedURL *temp = (lslpAtomizedURL *)calloc(1, sizeof(lslpAtomizedURL));
	if (temp != NULL)
	{
		temp->prev = temp->next = temp;
	}
	return(temp);
}	

lslpAtomizedURL *lslpAllocAtomizedURLList(void)
{
	lslpAtomizedURL *temp = lslpAllocAtomizedURL();
	if (temp != NULL)
	{
		temp->isHead = TRUE;
	}
	return(temp);
}	

void lslpFreeAtomizedURL(lslpAtomizedURL *u)
{
	assert(u != NULL);
	if (u->url != NULL)
		free(u->url);
	if (! _LSLP_IS_EMPTY(&(u->srvcs)))
		lslpFreeAtomList(&(u->srvcs), 0);
	if (! _LSLP_IS_EMPTY(&(u->site)))
		lslpFreeAtomList(&(u->site), 0);
	if (! _LSLP_IS_EMPTY(&(u->path)))
		lslpFreeAtomList(&(u->path), 0);
	if (!  _LSLP_IS_EMPTY(&(u->attrs)))
		lslpFreeAtomList(&(u->attrs), 0);
	free(u);
	return;
}	

void lslpFreeAtomizedURLList(lslpAtomizedURL *l, int32 flag)
{
	lslpAtomizedURL *temp;
	assert(l != NULL);
	assert(_LSLP_IS_HEAD(l));
	while (! (_LSLP_IS_HEAD(l->next)))
	{
		temp = l->next;
		temp->prev->next = temp->next;
		temp->next->prev = temp->prev;
		lslpFreeAtomizedURL(temp);
	}
	if (flag)
		free(l);
	return;
}	

void lslpInitAtomLists(void)
{
	srvcHead.next = srvcHead.prev = &srvcHead;
	siteHead.next = siteHead.prev = &siteHead;
	pathHead.next = pathHead.prev = &pathHead;
	attrHead.next = attrHead.prev = &attrHead;
	return;
}	

void lslpInitURLList(void)
{
	urlHead.next = urlHead.prev = &urlHead;
	return;
}	



void lslpCleanUpURLLists(void)
{
  
  lslpFreeAtomList(&srvcHead, 0);
  lslpFreeAtomList(&siteHead, 0);
  lslpFreeAtomList(&pathHead, 0);
  lslpFreeAtomList(&attrHead, 0);
  lslpFreeAtomizedURLList(&urlHead, 0);
}

lslpAtomizedURL *_lslpDecodeURLs(char *u[], int32 count)
{
  int32 i;
  
  uint32 lexer = 0;
  lslpAtomizedURL *temp = NULL;
  assert(u != NULL && u[count - 1] != NULL);
  lslpInitURLList();
  lslpInitAtomLists();
  
  for (i = 0; i < count; i++) {
    if (NULL == u[i])
      break;
    if((0 != (lexer = url_init_lexer(u[i])))) {
      if(urlparse())
	lslpCleanUpURLLists();
      url_close_lexer(lexer);
    }
  }
  if (! _LSLP_IS_EMPTY(&urlHead)) {
    if(NULL != (temp = lslpAllocAtomizedURLList())) {
      _LSLP_LINK_HEAD(temp, &urlHead);
    }
  }
  
  return(temp);	
}	 


