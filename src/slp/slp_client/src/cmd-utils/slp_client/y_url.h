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




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 95 "url.y"
typedef union YYSTYPE {
	int32 _i;
	char *_s;
	lslpAtomList *_atl;
	lslpAtomizedURL *_aturl;
} YYSTYPE;
/* Line 1275 of yacc.c.  */
#line 68 "y_url.h"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE urllval;



