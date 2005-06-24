/* A Bison parser, made by GNU Bison 1.875.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002 Free Software Foundation, Inc.

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
     TOK_ALIAS_IDENTIFIER = 258,
     TOK_ANY = 259,
     TOK_AS = 260,
     TOK_ASSOCIATION = 261,
     TOK_BINARY_VALUE = 262,
     TOK_CHAR_VALUE = 263,
     TOK_CLASS = 264,
     TOK_COLON = 265,
     TOK_COMMA = 266,
     TOK_DISABLEOVERRIDE = 267,
     TOK_DQUOTE = 268,
     TOK_DT_BOOL = 269,
     TOK_DT_CHAR16 = 270,
     TOK_DT_CHAR8 = 271,
     TOK_DT_DATETIME = 272,
     TOK_DT_REAL32 = 273,
     TOK_DT_REAL64 = 274,
     TOK_DT_SINT16 = 275,
     TOK_DT_SINT32 = 276,
     TOK_DT_SINT64 = 277,
     TOK_DT_SINT8 = 278,
     TOK_DT_STR = 279,
     TOK_DT_UINT16 = 280,
     TOK_DT_UINT32 = 281,
     TOK_DT_UINT64 = 282,
     TOK_DT_UINT8 = 283,
     TOK_ENABLEOVERRIDE = 284,
     TOK_END_OF_FILE = 285,
     TOK_EQUAL = 286,
     TOK_FALSE = 287,
     TOK_FLAVOR = 288,
     TOK_HEX_VALUE = 289,
     TOK_INCLUDE = 290,
     TOK_INDICATION = 291,
     TOK_INSTANCE = 292,
     TOK_LEFTCURLYBRACE = 293,
     TOK_LEFTPAREN = 294,
     TOK_LEFTSQUAREBRACKET = 295,
     TOK_METHOD = 296,
     TOK_NULL_VALUE = 297,
     TOK_OCTAL_VALUE = 298,
     TOK_OF = 299,
     TOK_PARAMETER = 300,
     TOK_PERIOD = 301,
     TOK_POSITIVE_DECIMAL_VALUE = 302,
     TOK_PRAGMA = 303,
     TOK_PROPERTY = 304,
     TOK_QUALIFIER = 305,
     TOK_REAL_VALUE = 306,
     TOK_REF = 307,
     TOK_REFERENCE = 308,
     TOK_RESTRICTED = 309,
     TOK_RIGHTCURLYBRACE = 310,
     TOK_RIGHTPAREN = 311,
     TOK_RIGHTSQUAREBRACKET = 312,
     TOK_SCHEMA = 313,
     TOK_SCOPE = 314,
     TOK_SEMICOLON = 315,
     TOK_SIGNED_DECIMAL_VALUE = 316,
     TOK_SIMPLE_IDENTIFIER = 317,
     TOK_STRING_VALUE = 318,
     TOK_TOSUBCLASS = 319,
     TOK_TRANSLATABLE = 320,
     TOK_TRUE = 321,
     TOK_UNEXPECTED_CHAR = 322
   };
#endif
#define TOK_ALIAS_IDENTIFIER 258
#define TOK_ANY 259
#define TOK_AS 260
#define TOK_ASSOCIATION 261
#define TOK_BINARY_VALUE 262
#define TOK_CHAR_VALUE 263
#define TOK_CLASS 264
#define TOK_COLON 265
#define TOK_COMMA 266
#define TOK_DISABLEOVERRIDE 267
#define TOK_DQUOTE 268
#define TOK_DT_BOOL 269
#define TOK_DT_CHAR16 270
#define TOK_DT_CHAR8 271
#define TOK_DT_DATETIME 272
#define TOK_DT_REAL32 273
#define TOK_DT_REAL64 274
#define TOK_DT_SINT16 275
#define TOK_DT_SINT32 276
#define TOK_DT_SINT64 277
#define TOK_DT_SINT8 278
#define TOK_DT_STR 279
#define TOK_DT_UINT16 280
#define TOK_DT_UINT32 281
#define TOK_DT_UINT64 282
#define TOK_DT_UINT8 283
#define TOK_ENABLEOVERRIDE 284
#define TOK_END_OF_FILE 285
#define TOK_EQUAL 286
#define TOK_FALSE 287
#define TOK_FLAVOR 288
#define TOK_HEX_VALUE 289
#define TOK_INCLUDE 290
#define TOK_INDICATION 291
#define TOK_INSTANCE 292
#define TOK_LEFTCURLYBRACE 293
#define TOK_LEFTPAREN 294
#define TOK_LEFTSQUAREBRACKET 295
#define TOK_METHOD 296
#define TOK_NULL_VALUE 297
#define TOK_OCTAL_VALUE 298
#define TOK_OF 299
#define TOK_PARAMETER 300
#define TOK_PERIOD 301
#define TOK_POSITIVE_DECIMAL_VALUE 302
#define TOK_PRAGMA 303
#define TOK_PROPERTY 304
#define TOK_QUALIFIER 305
#define TOK_REAL_VALUE 306
#define TOK_REF 307
#define TOK_REFERENCE 308
#define TOK_RESTRICTED 309
#define TOK_RIGHTCURLYBRACE 310
#define TOK_RIGHTPAREN 311
#define TOK_RIGHTSQUAREBRACKET 312
#define TOK_SCHEMA 313
#define TOK_SCOPE 314
#define TOK_SEMICOLON 315
#define TOK_SIGNED_DECIMAL_VALUE 316
#define TOK_SIMPLE_IDENTIFIER 317
#define TOK_STRING_VALUE 318
#define TOK_TOSUBCLASS 319
#define TOK_TRANSLATABLE 320
#define TOK_TRUE 321
#define TOK_UNEXPECTED_CHAR 322




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 163 "cimmof.y"
typedef union YYSTYPE {
  //char                     *strval;
  CIMClass                 *mofclass;
  CIMFlavor                *flavor;
  CIMInstance              *instance;
  CIMKeyBinding            *keybinding;
  CIMMethod                *method;
  CIMName                  *cimnameval;
  CIMObjectPath            *reference;
  CIMProperty              *property;
  CIMQualifier             *qualifier;
  CIMQualifierDecl         *mofqualifier;
  CIMScope                 *scope;
  CIMType                   datatype;
  CIMValue                 *value;
  int                       ival;
  modelPath                *modelpath;
  String                   *strptr;
  String                   *strval;
  struct pragma            *pragma;
  TYPED_INITIALIZER_VALUE  *typedinitializer;
} YYSTYPE;
/* Line 1250 of yacc.c.  */
#line 193 "cimmoftemp.h"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE cimmof_lval;



