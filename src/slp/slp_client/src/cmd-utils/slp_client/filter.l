/*****************************************************************************
 *  Description: encode/decode/eval LDAP Filters
 *
 *  Originated: April 04, 2002
 *	Original Author: Mike Day md@soft-hackle.net
 *                                mdd@us.ibm.com
 *
 *  $Header: /cvs/MSB/pegasus/src/slp/slp_client/src/cmd-utils/slp_client/filter.l,v 1.2 2005/02/26 05:36:54 david.dillard Exp $ 	                                                            
 *               					                    
 *  Copyright (c) 2001 - 2003  IBM                                          
 *  Copyright (c) 2000 - 2003 Michael Day                                    
 *                                                                           
 *  Permission is hereby granted, free of charge, to any person obtaining a  
 *  copy of this software and associated documentation files (the "Software"),
 *  to deal in the Software without restriction, including without limitation 
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 *  and/or sell copies of the Software, and to permit persons to whom the     
 *  Software is furnished to do so, subject to the following conditions:       
 * 
 *  The above copyright notice and this permission notice shall be included in 
 *  all copies or substantial portions of the Software.
 * 
 * 
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 *  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 *  DEALINGS IN THE SOFTWARE.
 *
 *****************************************************************************/



%{

#undef YYLMAX
#define YYLMAX 2048
#define YY_NEVER_INTERACTIVE 1
#undef YY_INPUT
#define YY_INPUT(b, r, m) abort()
#define exit(i) abort()
#include "slp_client.h"
#include "y_filter.h"
#include <stdarg.h>

#define filterless yyless
void filtererror(char *, ...);

/*
extern char *lslp_logPath;

#define yyerror(s) 	_lslpMsgOut(lslp_logPath, lslpPrintSem, LSLP_LOG_ERRORS, NULL, 0, (s), __FILE__, __LINE__);
*/
static int16 heapIndex;
static char heap[2052];
static char buf[2052];

static char *_lslp_strdup(const char *s);
void filter_close_lexer(uint32 handle);
size_t filter_init_lexer(const char *s);


%}

/* regex macros for the lexer */
hexdigit		[0-9a-fA-F]
reserved                [()\&|!=<>~\n] /* newline in reserved set for convinience */
not_reserved            [^()\&|!=<>~\n] 


/* special lexer states */
%x QUOTED_STRING

/* table size directives */

%option noyywrap prefix="filter"
%%

"\""                          { BEGIN QUOTED_STRING; filterless(0); }
<QUOTED_STRING>[^()\&|!=<>~\n]+  {
                                 BEGIN INITIAL; 
                                 if(NULL != (filterlval.filter_string =  _lslp_strdup(filtertext)))
                                    return(OPERAND) ; 
                                 else 
                                    return(0L) ;
                              }


[ \t\v\f]*                    { ; }
"("                           { filterlval.filter_int = L_PAREN; return L_PAREN ; }
")"                           { filterlval.filter_int = R_PAREN; return R_PAREN ; }
"&"                           { filterlval.filter_int = OP_AND; return OP_AND ; }
"|"                           { filterlval.filter_int = OP_OR; return OP_OR ; }
"!"                           { filterlval.filter_int = OP_NOT; return OP_NOT ; }
"="                           { filterlval.filter_int = OP_EQU; return OP_EQU ; }
">="                          { filterlval.filter_int = OP_GT; return OP_GT ; }
"<="                          { filterlval.filter_int = OP_LT; return OP_LT ; }
"=*"                          { filterlval.filter_int = OP_PRESENT; return OP_PRESENT ; }
"~="                          { filterlval.filter_int = OP_APPROX; return OP_APPROX ;  }


[-+][0-9]+                  |
[-+]"0"[xX]{hexdigit}+      { 
                              filterlval.filter_int = strtol(filtertext, (char **) 0, 0) ;
                              return VAL_INT;
                            }

[0-9]+                      |
"0"[xX]{hexdigit}+          { 
                              filterlval.filter_int = strtoul(filtertext, (char **) 0, 0);
                              return VAL_INT;
                            }

[tT][rR][uU][eE]            { 
                               filterlval.filter_int = 1; return VAL_BOOL; 
                            }


[fF][aA][lL][sS][eE]        { 
                               filterlval.filter_int = 0; return VAL_BOOL; 
                            }


[^()\&|!=<>~ \t\v\f]+       {
                                 if(NULL != (filterlval.filter_string =  _lslp_strdup(filtertext)))
                                    return(OPERAND) ; 
                                 else 
                                    return(0L) ;
                            }


 
%%

void filter_close_lexer(uint32 handle)
{
  assert(handle != 0);
  filter_delete_buffer((YY_BUFFER_STATE)handle);
}


size_t filter_init_lexer(const char *s)
{
  memset(&buf[0], 0x00, 2052);
  memset(&heap[0], 0x00, 2052);
  heapIndex = 0;
  strncpy(&buf[0], s, 2048);
  return((size_t) filter_scan_buffer(&buf[0], strlen(s) + 2));
}


static char *_lslp_strdup(const char *s)
{
  char *p = &heap[heapIndex];
  do { heap[heapIndex++] = *s; } while ((*s != 0x00) && (heapIndex < 2049) && (++s));
  return(p);
}

/*****
static char *_lslp_strdup_strip_quotes(char *s)
{
  char *p = &heap[heapIndex];
  do { if(*s != '"') heap[heapIndex++] = *s; } while ((*s != 0x00) && (heapIndex < 2049) && (++s));
  return(p);
}

******/

void filtererror(char *s, ...)
{
	return;
}	

/*
int main(int argc, char *argv[])
{

}	

*/

