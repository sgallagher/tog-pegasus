typedef union 
{
   int intValue;
   double doubleValue;
   char* strValue;
   void* nodeValue;
} YYSTYPE;
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


extern YYSTYPE WQL_lval;
