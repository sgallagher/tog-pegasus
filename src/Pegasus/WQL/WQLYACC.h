#define TOK_INTEGER 257
#define TOK_DOUBLE 258
#define TOK_STRING 259
#define TOK_TRUE 260
#define TOK_FALSE 261
#define TOK_NULL 262
#define TOK_EQ 263
#define TOK_NE 264
#define TOK_LT 265
#define TOK_LE 266
#define TOK_GT 267
#define TOK_GE 268
#define TOK_NOT 269
#define TOK_OR 270
#define TOK_AND 271
#define TOK_IS 272
#define TOK_IDENTIFIER 273
#define TOK_SELECT 274
#define TOK_WHERE 275
#define TOK_FROM 276
#define TOK_UNEXPECTED_CHAR 277
typedef union 
{
   int intValue;
   double doubleValue;
   char* strValue;
   void* nodeValue;
} YYSTYPE;
extern YYSTYPE WQL_lval;
