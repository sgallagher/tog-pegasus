//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
#ifndef YYERRCODE
#define YYERRCODE 256
#endif

#define IDENTIFIER 257
#define STRING_LITERAL 258
#define BINARY 259
#define NEGATIVE_BINARY 260
#define HEXADECIMAL 261
#define NEGATIVE_HEXADECIMAL 262
#define INTEGER 263
#define NEGATIVE_INTEGER 264
#define REAL 265
#define NEGATIVE_REAL 266
#define _TRUE 267
#define _FALSE 268
#define SCOPED_PROPERTY 269
#define LPAR 270
#define RPAR 271
#define HASH 272
#define DOT 273
#define LBRKT 274
#define RBRKT 275
#define UNDERSCORE 276
#define COMMA 277
#define CONCAT 278
#define DBL_PIPE 279
#define PLUS 280
#define MINUS 281
#define TIMES 282
#define DIV 283
#define IS 284
#define _NULL 285
#define _EQ 286
#define _NE 287
#define _GT 288
#define _LT 289
#define _GE 290
#define _LE 291
#define _ISA 292
#define _LIKE 293
#define NOT 294
#define _AND 295
#define _OR 296
#define SCOPE 297
#define ANY 298
#define EVERY 299
#define IN 300
#define SATISFIES 301
#define STAR 302
#define DOTDOT 303
#define SHARP 304
#define DISTINCT 305
#define SELECT 306
#define FIRST 307
#define FROM 308
#define WHERE 309
#define ORDER 310
#define BY 311
#define ASC 312
#define DESC 313
#define AS 314
#define UNEXPECTED_CHAR 315
typedef union {
   char * strValue;
   String * _string;
   CQLValue * _value;
   CQLIdentifier * _identifier;
   CQLChainedIdentifier * _chainedIdentifier;
   CQLPredicate * _predicate;
   ExpressionOpType _opType;
   void * _node;
} YYSTYPE;
extern YYSTYPE CQL_lval;
