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

#define TOK_ALIAS_IDENTIFIER 257
#define TOK_ANY 258
#define TOK_AS 259
#define TOK_ASSOCIATION 260
#define TOK_BINARY_VALUE 261
#define TOK_CHAR_VALUE 262
#define TOK_CLASS 263
#define TOK_COLON 264
#define TOK_COMMA 265
#define TOK_DISABLEOVERRIDE 266
#define TOK_DQUOTE 267
#define TOK_DT_BOOL 268
#define TOK_DT_CHAR16 269
#define TOK_DT_CHAR8 270
#define TOK_DT_DATETIME 271
#define TOK_DT_REAL32 272
#define TOK_DT_REAL64 273
#define TOK_DT_SINT16 274
#define TOK_DT_SINT32 275
#define TOK_DT_SINT64 276
#define TOK_DT_SINT8 277
#define TOK_DT_STR 278
#define TOK_DT_UINT16 279
#define TOK_DT_UINT32 280
#define TOK_DT_UINT64 281
#define TOK_DT_UINT8 282
#define TOK_ENABLEOVERRIDE 283
#define TOK_END_OF_FILE 284
#define TOK_EQUAL 285
#define TOK_FALSE 286
#define TOK_FLAVOR 287
#define TOK_HEX_VALUE 288
#define TOK_INCLUDE 289
#define TOK_INDICATION 290
#define TOK_INSTANCE 291
#define TOK_LEFTCURLYBRACE 292
#define TOK_LEFTPAREN 293
#define TOK_LEFTSQUAREBRACKET 294
#define TOK_METHOD 295
#define TOK_NULL_VALUE 296
#define TOK_OCTAL_VALUE 297
#define TOK_OF 298
#define TOK_PARAMETER 299
#define TOK_PERIOD 300
#define TOK_POSITIVE_DECIMAL_VALUE 301
#define TOK_PRAGMA 302
#define TOK_PROPERTY 303
#define TOK_QUALIFIER 304
#define TOK_REAL_VALUE 305
#define TOK_REF 306
#define TOK_REFERENCE 307
#define TOK_RESTRICTED 308
#define TOK_RIGHTCURLYBRACE 309
#define TOK_RIGHTPAREN 310
#define TOK_RIGHTSQUAREBRACKET 311
#define TOK_SCHEMA 312
#define TOK_SCOPE 313
#define TOK_SEMICOLON 314
#define TOK_SIGNED_DECIMAL_VALUE 315
#define TOK_SIMPLE_IDENTIFIER 316
#define TOK_STRING_VALUE 317
#define TOK_TOSUBCLASS 318
#define TOK_TRANSLATABLE 319
#define TOK_TRUE 320
#define TOK_UNEXPECTED_CHAR 321
typedef union {
  /*char                     *strval;*/
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
extern YYSTYPE cimmof_lval;
