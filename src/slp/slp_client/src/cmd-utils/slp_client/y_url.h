#ifndef BISON_Y_URL_H
# define BISON_Y_URL_H

#ifndef YYSTYPE
typedef union {
	int32 _i;
	char *_s;
	lslpAtomList *_atl;
	lslpAtomizedURL *_aturl;
} yystype;
# define YYSTYPE yystype
# define YYSTYPE_IS_TRIVIAL 1
#endif
# define	_RESERVED	257
# define	_HEXDIG	258
# define	_STAG	259
# define	_RESNAME	260
# define	_NAME	261
# define	_ELEMENT	262
# define	_IPADDR	263
# define	_IPX	264
# define	_AT	265
# define	_ZONE	266


extern YYSTYPE urllval;

#endif /* not BISON_Y_URL_H */
