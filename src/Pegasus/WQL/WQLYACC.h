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


extern YYSTYPE WQL_lval;
