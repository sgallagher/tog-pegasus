%option never-interactive
%{ 

#include "cimmofParser.h"
#include "memobjs.h"
#include "cimmof_tab.h"

/*---------------------------------------------------------------*/
/* These routines wrap certain flex functions, keeping down the  */
/* number of external dependencies                               */
/*---------------------------------------------------------------*/
int get_yy_buf_size_wrapper() { return YY_BUF_SIZE; }

void * get_cimmof__current_buffer_wrapper() { 
  return (void *)YY_CURRENT_BUFFER; 
}

void * create_cimmof_buffer_wrapper(const FILE *f, int size) {
  return (void *)yy_create_buffer((FILE *)f, size);
}

int switch_to_buffer_wrapper(void *buffstate) {
  yy_switch_to_buffer((YY_BUFFER_STATE)buffstate);
  return 0;
}



%}


%%


\n                                  { 
                                cimmofParser::Instance()->increment_lineno(); }
\/\/.*[\r\n]?                       {
                                cimmofParser::Instance()->increment_lineno(); }
[ \t\r]                             { /* skip white space */ }
class                                 { return(TOK_CLASS); }
\{                                  { return(TOK_LEFTCURLYBRACE); }
\}                                  { return(TOK_RIGHTCURLYBRACE); }
\[                                  { return(TOK_LEFTSQUAREBRACKET); }
\]                                  { return(TOK_RIGHTSQUAREBRACKET); }
\(                                  { return(TOK_LEFTPAREN); }
\)                                  { return(TOK_RIGHTPAREN); }
\:                                  { return(TOK_COLON); }
;                                   { return(TOK_SEMICOLON); }
[+-]?[0-9]*\.[0-9]+([eE][+-]?[0-9]*)? { 
                                      cimmof_lval.strval = new String(yytext);
                                      return(TOK_REAL_VALUE); }
[+-]0[0-7]+                         { cimmof_lval.strval = new String(yytext);
                                     return(TOK_OCTAL_VALUE); }
0x[0-9A-Fa-f]+                      { cimmof_lval.strval = new String(yytext);
                                     return(TOK_HEX_VALUE); }
[+-][01]+[Bb]                       { cimmof_lval.strval = new String(yytext);
                                     return(TOK_BINARY_VALUE); }
[0-9]+                              { cimmof_lval.strval = new String(yytext);
                                     return(TOK_POSITIVE_DECIMAL_VALUE); }
[+-][0-9]+                          { cimmof_lval.strval = new String(yytext);
                                     return(TOK_SIGNED_DECIMAL_VALUE); }
\=                                  { return(TOK_EQUAL); }
,                                   { return(TOK_COMMA); }
\'.+\'                              { cimmof_lval.strval = new String(yytext);
                                      return(TOK_CHAR_VALUE); }
\"[^\"\n]*[\"\n]                    { cimmof_lval.strval = new String(yytext);
                                      return(TOK_STRING_VALUE); }
[Nn][Uu][Ll][Ll]                    { return(TOK_NULL_VALUE); }
[Tt][Rr][Uu][Ee]                    { return(TOK_TRUE); }
[Ff][Aa][Ll][Ss][Ee]                { return(TOK_FALSE); }
\"                                  { return(TOK_DQUOTE); }
\.                                  { return(TOK_PERIOD); }
\#[Pp][Rr][Aa][Gg][Mm][Aa]          { return(TOK_PRAGMA); }
[Aa][Ss]                            { return(TOK_AS); }
[Ii][Nn][Ss][Tt][Aa][Nn][Cc][Ee]    {
                                      return(TOK_INSTANCE); }
[Oo][Ff]                            {
                                      return(TOK_OF); }
[Qq][Uu][Aa][Ll][Ii][Ff][Ii][Ee][Rr] {return(TOK_QUALIFIER); }
[Ss][Cc][Oo][Pp][Ee]                { return(TOK_SCOPE); }
[Ss][Cc][Hh][Ee][Mm][Aa]            { return(TOK_SCHEMA); }
[Aa][Ss][Ss][Oo][Cc][Ii][Aa][Tt][Ii][Oo][Nn] {
                                      return(TOK_ASSOCIATION); }
[Ii][Nn][Dd][Ii][Cc][Aa][Tt][Ii][Oo][Nn] {
                                      return(TOK_INDICATION); }
[Pp][Rr][Oo][Pp][Ee][Rr][Tt][Yy]    { return(TOK_PROPERTY); }
[Rr][Ee][Ff][Ee][Rr][Ee][Nn][Cc][Ee] { return(TOK_REFERENCE); }
[Mm][Ee][Tt][Hh][Oo][Dd]            { return(TOK_METHOD); }
[Pp][Aa][Rr][Aa][Mm][Ee][Tt][Ee][Rr] {return(TOK_PARAMETER); }
[Aa][Nn][Yy]                        { return(TOK_ANY); }
[Rr][Ee][Ff]                        { return(TOK_REF); }
[Ff][Ll][Aa][Vv][Oo][Rr]            { return(TOK_FLAVOR); }
[Ee][Nn][Aa][Bb][Ll][Ee][Oo][Vv][Ee][Rr][Rr][Ii][Dd][Ee] {
                                      return(TOK_ENABLEOVERRIDE); }
[Dd][Ii][Ss][Aa][Bb][Ll][Ee][Oo][Vv][Ee][Rr][Rr][Ii][Dd][Ee] {
                                      return(TOK_DISABLEOVERRIDE); }
[Rr][Ee][Ss][Tt][Rr][Ii][Cc][Tt][Ee][Dd] {
                                      return(TOK_RESTRICTED); }
[Tt][Oo][Ss][Uu][Bb][Cc][Ll][Aa][Ss][Ss] {return(TOK_TOSUBCLASS); }
[Tt][Rr][Aa][Nn][Ss][Ll][Aa][Tt][Aa][Bb][Ll][Ee] {
                                      return(TOK_TRANSLATABLE); }
[Ii][Nn][Cc][Ll][Uu][Dd][Ee]        { return(TOK_INCLUDE); }
[Ss][Tt][Rr][Ii][Nn][Gg]            { return(TOK_DT_STR); }
[Bb][Oo][Oo][Ll][Ee][Aa][Nn]        { return(TOK_DT_BOOL); }
[Dd][Aa][Tt][Ee][Tt][Ii][Mm][Ee]    { return(TOK_DT_DATETIME); }
[Uu][Ii][Nn][Tt]8                   { return(TOK_DT_UINT8); }
[Uu][Ii][Nn][Tt]16                  { return(TOK_DT_UINT16); }
[Uu][Ii][Nn][Tt]32                  { return(TOK_DT_UINT32); }
[Uu][Ii][Nn][Tt]64                  { return(TOK_DT_UINT64); }
[Ss][Ii][Nn][Tt]8                   { return(TOK_DT_SINT8); }
[Ss][Ii][Nn][Tt]16                  { return(TOK_DT_SINT16); }
[Ss][Ii][Nn][Tt]32                  { return(TOK_DT_SINT32); }
[Ss][Ii][Nn][Tt]64                  { return(TOK_DT_SINT64); }
[Rr][Ee][Aa][Ll]32                  { return(TOK_DT_REAL32); }
[Rr][Ee][Aa][Ll]64                  { return(TOK_DT_REAL64); }
[Cc][Hh][Aa][Rr]8                   { return(TOK_DT_CHAR8); }
[Cc][Hh][Aa][Rr]16                  { return(TOK_DT_CHAR16); }
$[_$0-9a-zA-Z]+                     { return(TOK_ALIAS_IDENTIFIER); }
[_a-zA-Z][_$0-9a-zA-Z]*             { cimmof_lval.strval = new String(yytext);
                                      return(TOK_SIMPLE_IDENTIFIER); }
.                                   { /* error */ }

%%

void cimmof_yy_less(int n) {
  yyless(n);
}
