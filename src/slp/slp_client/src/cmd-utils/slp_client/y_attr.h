#ifndef BISON_Y_ATTR_H
# define BISON_Y_ATTR_H

#ifndef YYSTYPE
typedef union {
	int32 _i;
	char *_s;
	lslpAttrList *_atl;
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
# define	_TRUE	257
# define	_FALSE	258
# define	_MULTIVAL	259
# define	_INT	260
# define	_ESCAPED	261
# define	_TAG	262
# define	_STRING	263
# define	_OPAQUE	264


extern YYSTYPE attrlval;

#endif /* not BISON_Y_ATTR_H */
