%{
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CommonUTF.h>
#include "memobjs.h"
#include <stdio.h>

#define yyparse CQL_parse

int yylex();
int yyerror(char * err){printf("%s\n", err); return 1;}
extern char * yytext;

PEGASUS_NAMESPACE_BEGIN
                                                                                
extern CQLParserState* globalParserState;
                                                                                
PEGASUS_NAMESPACE_END


%}
%union {
   char * strValue;
   String * _string;
   CQLValue * _value;
   CQLSelectStatement * _ss;
   CQLIdentifier * _id;
   CQLChainedIdentifier * _cid;
   CQLTerm * _term;
   CQLFactor * _factor;
   CQLPredicate * _predicate;
}

/* terminals */
%token <strValue> IDENTIFIER 
%token <strValue> STRING_LITERAL
%token <strValue> BINARY
%token <strValue> NEGATIVE_BINARY
%token <strValue> HEXADECIMAL
%token <strValue> NEGATIVE_HEXADECIMAL
%token <strValue> INTEGER 
%token <strValue> NEGATIVE_INTEGER
%token <strValue> REAL 
%token <strValue> NEGATIVE_REAL
%token <strValue> TRUE 
%token <strValue> FALSE 
%token <strValue> SCOPED_PROPERTY
%token <strValue> LPAR 
%token <strValue> RPAR 
%token <strValue> HASH
%token <strValue> DOT 
%token <strValue> LBRKT 
%token <strValue> RBRKT 
%token <strValue> SCOPE
%token <strValue> UNDERSCORE
%token <strValue> COMMA 
%token <strValue> CONCAT 
%token <strValue> DBL_PIPE
%token <strValue> PLUS
%token <strValue> MINUS 
%token <strValue> TIMES 
%token <strValue> DIV 
%token <strValue> IS 
%token <strValue> _NULL 
%token <strValue> EQ 
%token <strValue> NE 
%token <strValue> GT 
%token <strValue> LT 
%token <strValue> GE 
%token <strValue> LE 
%token <strValue> ISA 
%token <strValue> LIKE 
%token <strValue> NOT AND OR 
%token <strValue> SCOPINGTYPE 
%token <strValue> ANY EVERY IN SATISFIES 
%token <strValue> STAR 
%token <strValue> DOTDOT 
%token <strValue> SHARP DISTINCT 
%token <strValue> SELECT 
%token <strValue> FIRST 
%token <strValue> FROM 
%token <strValue> WHERE 
%token <strValue> ORDER 
%token <strValue> BY 
%token <strValue> ASC 
%token <strValue> DESC 
%token <strValue> AS 
%token <strValue> UNEXPECTED_CHAR


/* grammar - non terminals */
%type <strValue> identifier
%type <strValue> class_name
%type <strValue> class_path
%type <strValue> property_scope
%type <strValue> literal_string
%type <strValue> binary_value
%type <strValue> hex_value
%type <strValue> decimal_value
%type <strValue> real_value
%type <strValue> literal
%type <strValue> array_index
%type <strValue> array_index_list
%type <strValue> chain
%type <strValue> concat
%type <strValue> factor
%type <strValue> term
%type <strValue> arith
%type <strValue> value_symbol
%type <strValue> arith_or_value_symbol
%type <strValue> comp_op
%type <strValue> comp
%type <strValue> optional_not
%type <strValue> expr_factor
%type <strValue> expr_term
%type <strValue> expr
%type <strValue> arg_list
%type <strValue> arg_list_sub
%type <strValue> from_specifier
%type <strValue> from_criteria
%type <strValue> star_expr
%type <strValue> selected_entry
%type <strValue> select_list
%type <strValue> select_list_tail
%type <strValue> search_condition
%type <strValue> optional_where
%type <strValue> select_statement

%start select_statement

%%
identifier : IDENTIFIER 
             { 
                 printf("BISON::identifier\n"); 
             }
;

class_name : identifier  
             { 
                 printf("BISON::class_namei = %s\n", $1); 
             }
;

class_path : class_name 
             { 
                 printf("BISON::class_path\n"); 
             }
;

property_scope : class_path SCOPE 
             { 
                 printf("BISON::property_scope = %s\n",$1); 
             }
;

literal_string : STRING_LITERAL 
             { 
                 printf("BISON::literal_string-> %s\n",CQL_lval.strValue); 
		 if(isUTF8Str(CQL_lval.strValue)){
		     CQLValue _value(String(CQL_lval.strValue));
		 }else{
		     printf("BISON::literal_string-> BAD UTF\n");
		 }
             }
;

binary_value : BINARY 
	       { 
                   printf("BISON::binary_value-> %s\n",CQL_lval.strValue); 
                   CQLValue _value(CQL_lval.strValue, Binary); 
               }
             | NEGATIVE_BINARY 
               { 
                   printf("BISON::binary_value-> %s\n",CQL_lval.strValue); 
                   CQLValue _value(CQL_lval.strValue, Binary, false); 
               }
;

hex_value : HEXADECIMAL 
            { 
                printf("BISON::hex_value-> %s\n",CQL_lval.strValue); 
                CQLValue _value(CQL_lval.strValue, Hex);
            }
          | NEGATIVE_HEXADECIMAL 
            { 
                printf("BISON::hex_value-> %s\n",CQL_lval.strValue); 
                CQLValue _value(CQL_lval.strValue, Hex, false);
            }
;

decimal_value : INTEGER 
                { 
                    printf("BISON::decimal_value-> %s\n",CQL_lval.strValue); 
                    CQLValue _value(CQL_lval.strValue, Decimal); 
                }
              | NEGATIVE_INTEGER 
                { 
                    printf("BISON::decimal_value-> %s\n",CQL_lval.strValue); 
                    CQLValue _value(CQL_lval.strValue, Decimal, false);
                }
;

real_value : REAL 
             { 
                 printf("BISON::real_value-> %s\n",CQL_lval.strValue); 
                 CQLValue _value(CQL_lval.strValue, Real);
             }
           | NEGATIVE_REAL 
             { 
                 printf("BISON::real_value-> %s\n",CQL_lval.strValue); 
                 CQLValue _value(CQL_lval.strValue, Real, false);
             }
;

literal : literal_string 
          {
              printf("BISON::literal->literal_string\n");
          }
        | decimal_value
          {
              printf("BISON::literal->decimal_value\n");
          }
        | binary_value
          {
              printf("BISON::literal->binary_value\n");
          }
        | hex_value
          {
              printf("BISON::literal->hex_value\n");
          }
        | real_value
          {
              printf("BISON::literal->real_value\n");
          }
        | TRUE
          {
              printf("BISON::literal->TRUE\n");
          }
        | FALSE
          {
              printf("BISON::literal->FALSE\n");
          }
;

array_index : expr
              {
                  printf("BISON::array_index->expr\n");
              }
;

array_index_list : array_index
                   {
                       printf("BISON::array_index_list->array_index\n");
                   }
;

chain : literal
        {
            printf("BISON::chain->literal\n");
        }
      | LPAR expr RPAR
        {
            printf("BISON::chain-> ( expr )\n");
        }
      | identifier
        {
            printf("BISON::chain->identifier\n");
        }
      | identifier HASH literal_string
        {
            printf("BISON::chain->identifier#literal_string\n");
        }
      | identifier LPAR arg_list RPAR
        {
            printf("BISON::chain-> identifier( arg_list )\n");
        }
      | chain DOT property_scope identifier
        {
            printf("BISON::chain->chain.property_scope identifier\n");
        }
      | chain DOT property_scope identifier HASH literal_string
        {
            printf("BISON::chain->chain.property_scope identifier#literal_string\n");
        }
      | chain DOT identifier
        {
            printf("BISON::chain->chain.identifier\n");
        }
      | chain DOT identifier HASH literal_string
        {
            printf("BISON::chain->chain.identifier#literal_string\n");
        }
      | chain LBRKT array_index_list RBRKT
        {
            printf("BISON::chain->chain[ array_index_list ]\n");
        }
;

concat : chain
         {
             printf("BISON::concat->chain = %s\n",$1);
         }
       | concat DBL_PIPE chain
         {
             printf("BISON::concat||chain\n");
         }
;

factor : concat
         {
             printf("BISON::factor->concat\n");
         }         
       | PLUS concat
         {
             printf("BISON::factor->PLUS concat\n");
         }
       | MINUS concat
         {
             printf("BISON::factor->MINUS concat\n");
         }
;

term : factor
       {
           printf("BISON::term->factor\n");
       }
     | term STAR factor
       {
           printf("BISON::term->term STAR factor\n");
       }
     | term DIV factor
       {
           printf("BISON::term->term DIV factor\n");
       }
;

arith : term
        {
            printf("BISON::arith->term\n");
        }
      | arith PLUS term
        {
            printf("BISON::arith->arith PLUS term\n");
        }
      | arith MINUS term
        {
            printf("BISON::arith->arith MINUS term\n");
        }
;

value_symbol : HASH literal_string
               {
                   printf("BISON::value_symbol->#literal_string\n");
               }
;

arith_or_value_symbol : arith
                        {
                            printf("BISON::arith_or_value_symbol->arith\n");
                        }
                      | value_symbol
                        {
                            printf("BISON::arith_or_value_symbol->value_symbol\n");
                        }
;

comp_op : EQ 
          {
              printf("BISON::comp_op->EQ\n");
          }
        | NE 
          {
              printf("BISON::comp_op->NE\n");
          }
        | GT 
          {
              printf("BISON::comp_op->GT\n");
          }
        | LT 
          {
              printf("BISON::comp_op->LT\n");
          }
        | GE 
          {
              printf("BISON::comp_op->GE\n");
          }
        | LE 
          {
              printf("BISON::comp_op->LE\n");
          }
;

comp : arith
       {
           printf("BISON::comp->arith\n");
       }
     | arith IS optional_not _NULL
       {
           printf("BISON::comp->arith IS optional_not _NULL\n");
       }
     | arith comp_op arith_or_value_symbol
       {
           printf("BISON::comp->arith comp_op arith_or_value_symbol\n");
       }
     | value_symbol comp_op arith
       {
           printf("BISON::comp->value_symbol comp_op arith\n");
       }
     | arith ISA identifier
       {
           printf("BISON::comp->arith ISA identifier\n");
       }
     | arith LIKE literal_string
       {
           printf("BISON::comp->arith LIKE literal_string\n");
       }
;

optional_not : {;} /* nothing */
             | NOT
               {
                   printf("BISON::optional_not->NOT\n");
               }
;

expr_factor : comp
              {
                  printf("BISON::expr_factor->comp\n");
              }
            | NOT comp
              {
                  printf("BISON::expr_factor->NOT comp\n");
              }
;

expr_term : expr_factor
            {
                printf("BISON::expr_term->expr_factor\n");
            }
          | expr_term AND expr_factor
            {
                printf("BISON::expr_term->expr_term AND expr_factor\n");
            }
;

expr : expr_term 
       {
           printf("BISON::expr->expr_term\n");
       }
     | expr OR expr_term
       {
           printf("BISON::expr->expr OR expr_term\n");
       }
;

arg_list : {;}
         | STAR
           {
               printf("BISON::arg_list->STAR\n");
           }
         | DISTINCT STAR
           {
               printf("BISON::arg_list->DISTINCT STAR\n");
           }
         | arg_list_sub arg_list_tail
           {
               printf("BISON::arg_list->arg_list_sub arg_list_tail\n");
           }
;

arg_list_sub : expr
               {
                   printf("BISON::arg_list_sub->expr\n");
               }   
             | DISTINCT expr
               {
                   printf("BISON::arg_list_sub->DISTINCT expr\n");
               }
;

arg_list_tail : {;}
              | COMMA arg_list_sub arg_list_tail
              {
                  printf("BISON::arg_list_tail->COMMA arg_list_sub arg_list_tail\n");
              }
;

from_specifier : class_path
                 {
                     printf("BISON::from_specifier->class_path\n");
                 } 
;

from_criteria : from_specifier
                {
                    printf("BISON::from_criteria->from_specifier\n");
                }
;

star_expr : STAR
            {
                printf("BISON::star_expr->STAR\n");
            }
;

selected_entry : expr 
                 {
                     printf("BISON::selected_entry->expr\n");
                 }
               | star_expr
                 {
                     printf("BISON::selected_entry->star_expr\n");
                 }
;

select_list : selected_entry select_list_tail
            {
                printf("BISON::select_list->selected_entry select_list_tail\n");
            }
;

select_list_tail : {;} /* empty */
                 | COMMA selected_entry select_list_tail
                   {
                       printf("BISON::select_list_tail->COMMA selected_entry select_list_tail\n");
                   }
;

search_condition : expr
                   {
                       printf("BISON::search_condition->expr\n");
                   }
;

optional_where : {;}
               | WHERE search_condition
                 {
                     printf("BISON::optional_where->WHERE search_condition\n");
                 }
;

select_statement : SELECT select_list FROM from_criteria optional_where 
                   {
                       printf("select_statement\n\n");
                   }
;

%%
