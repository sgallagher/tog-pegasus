//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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
typedef union {
  struct pragma *pragma;
  int              ival;
  //  char             *strval;
  String *         strval;
  CIMName *         cimnameval;
  CIMType        datatype;
  CIMValue *          value;
  String *         strptr;
  CIMQualifier *      qualifier;
  CIMScope *          scope;
  CIMFlavor *         flavor;
  CIMProperty *       property;
  CIMMethod *         method;
  CIMClass *      mofclass;
  CIMQualifierDecl *   mofqualifier;
  CIMInstance *   instance;
  CIMObjectPath *  reference;
  modelPath *     modelpath;
  CIMKeyBinding *    keybinding;
  TYPED_INITIALIZER_VALUE * typedinitializer;
} YYSTYPE;
#define	TOK_LEFTCURLYBRACE	257
#define	TOK_RIGHTCURLYBRACE	258
#define	TOK_RIGHTSQUAREBRACKET	259
#define	TOK_LEFTSQUAREBRACKET	260
#define	TOK_LEFTPAREN	261
#define	TOK_RIGHTPAREN	262
#define	TOK_COLON	263
#define	TOK_SEMICOLON	264
#define	TOK_POSITIVE_DECIMAL_VALUE	265
#define	TOK_SIGNED_DECIMAL_VALUE	266
#define	TOK_EQUAL	267
#define	TOK_COMMA	268
#define	TOK_CLASS	269
#define	TOK_REAL_VALUE	270
#define	TOK_CHAR_VALUE	271
#define	TOK_STRING_VALUE	272
#define	TOK_NULL_VALUE	273
#define	TOK_OCTAL_VALUE	274
#define	TOK_HEX_VALUE	275
#define	TOK_BINARY_VALUE	276
#define	TOK_TRUE	277
#define	TOK_FALSE	278
#define	TOK_DQUOTE	279
#define	TOK_PERIOD	280
#define	TOK_SIMPLE_IDENTIFIER	281
#define	TOK_ALIAS_IDENTIFIER	282
#define	TOK_PRAGMA	283
#define	TOK_INCLUDE	284
#define	TOK_AS	285
#define	TOK_INSTANCE	286
#define	TOK_OF	287
#define	TOK_QUALIFIER	288
#define	TOK_SCOPE	289
#define	TOK_SCHEMA	290
#define	TOK_ASSOCIATION	291
#define	TOK_INDICATION	292
#define	TOK_PROPERTY	293
#define	TOK_REFERENCE	294
#define	TOK_METHOD	295
#define	TOK_PARAMETER	296
#define	TOK_ANY	297
#define	TOK_REF	298
#define	TOK_FLAVOR	299
#define	TOK_ENABLEOVERRIDE	300
#define	TOK_DISABLEOVERRIDE	301
#define	TOK_RESTRICTED	302
#define	TOK_TOSUBCLASS	303
#define	TOK_TRANSLATABLE	304
#define	TOK_DT_STR	305
#define	TOK_DT_BOOL	306
#define	TOK_DT_DATETIME	307
#define	TOK_DT_UINT8	308
#define	TOK_DT_SINT8	309
#define	TOK_DT_UINT16	310
#define	TOK_DT_SINT16	311
#define	TOK_DT_UINT32	312
#define	TOK_DT_SINT32	313
#define	TOK_DT_UINT64	314
#define	TOK_DT_SINT64	315
#define	TOK_DT_CHAR8	316
#define	TOK_DT_CHAR16	317
#define	TOK_DT_REAL32	318
#define	TOK_DT_REAL64	319
#define	TOK_UNEXPECTED_CHAR	320
#define	TOK_END_OF_FILE	321


extern YYSTYPE cimmof_lval;
