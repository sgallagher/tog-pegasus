#ifndef BISON_Y_FILTER_H
# define BISON_Y_FILTER_H

#ifndef YYSTYPE
typedef union {
  int32 filter_int;
  char *filter_string;
  lslpLDAPFilter *filter_filter;
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
# define	L_PAREN	257
# define	R_PAREN	258
# define	OP_AND	259
# define	OP_OR	260
# define	OP_NOT	261
# define	OP_EQU	262
# define	OP_GT	263
# define	OP_LT	264
# define	OP_PRESENT	265
# define	OP_APPROX	266
# define	VAL_INT	267
# define	VAL_BOOL	268
# define	OPERAND	269


extern YYSTYPE filterlval;

#endif /* not BISON_Y_FILTER_H */
