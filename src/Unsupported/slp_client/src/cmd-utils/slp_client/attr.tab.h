#ifndef BISON_ATTR_TAB_H
# define BISON_ATTR_TAB_H

#ifndef YYSTYPE
typedef union {
	int32 _i;
	int8 *_s;
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

#endif /* not BISON_ATTR_TAB_H */
