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

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     L_PAREN = 258,
     R_PAREN = 259,
     OP_AND = 260,
     OP_OR = 261,
     OP_NOT = 262,
     OP_EQU = 263,
     OP_GT = 264,
     OP_LT = 265,
     OP_PRESENT = 266,
     OP_APPROX = 267,
     VAL_INT = 268,
     VAL_BOOL = 269,
     OPERAND = 270
   };
#endif
#define L_PAREN 258
#define R_PAREN 259
#define OP_AND 260
#define OP_OR 261
#define OP_NOT 262
#define OP_EQU 263
#define OP_GT 264
#define OP_LT 265
#define OP_PRESENT 266
#define OP_APPROX 267
#define VAL_INT 268
#define VAL_BOOL 269
#define OPERAND 270




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 91 "filter.y"
typedef union YYSTYPE {
  int32 filter_int;
  char *filter_string;
  lslpLDAPFilter *filter_filter;
} YYSTYPE;
/* Line 1275 of yacc.c.  */
#line 73 "y_filter.h"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE filterlval;



