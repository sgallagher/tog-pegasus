typedef union 
{
   int intValue;
   double doubleValue;
   char* strValue;
   void* nodeValue;
} YYSTYPE;
#define	TOK_INTEGER	258
#define	TOK_DOUBLE	259
#define	STRING_LITERAL	260
#define	EQ	261
#define	NE	262
#define	LT	263
#define	LE	264
#define	GT	265
#define	GE	266
#define	TOK_SELECT	267
#define	WHERE	268
#define	FROM	269
#define	TOK_IDENTIFIER	270
#define	NOT	271
#define	OR	272
#define	AND	273
#define	ISA	274
#define	WQL_TRUE	275
#define	WQL_FALSE	276


extern YYSTYPE WQL_lval;
