//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN
// ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
// "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//==============================================================================
// 
// Author: Humberto Rivero (hurivero@us.ibm.com)
//
// Modified By:
// 
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/MessageLoader.h>
#include <Pegasus/Common/Thread.h>
#include <Pegasus/Common/Tracer.h>
#include <iostream>
#ifdef PEGASUS_OS_OS400
#include "OS400ConvertChar.h"
#endif
PEGASUS_NAMESPACE_BEGIN 
PEGASUS_USING_STD;

static const int ID_INVALID = -1;
static const String server_resbundl_name = "pegasus/pegasusServer";
String MessageLoader::pegasus_MSG_HOME = String::EMPTY;
Boolean MessageLoader::_useProcessLocale = false;
Boolean MessageLoader::_useDefaultMsg = false;
AcceptLanguages MessageLoader::_acceptlanguages = AcceptLanguages();

	String MessageLoader::getMessage(MessageLoaderParms &parms){
		PEG_METHOD_ENTER(TRC_L10N, "MessageLoader::getMessage");
		
		String msg;
		parms.contentlanguages.clear();
		
		// if message loading is DISABLED return the default message
		#ifndef PEGASUS_HAS_MESSAGES
			return formatDefaultMessage(parms);
		#endif	
		
		// if someone has set the global behaviour to use default messages
		if(_useDefaultMsg) return formatDefaultMessage(parms);	 
		
		#ifdef PEGASUS_HAS_ICU
			//cout << "PEGASUS_HAS_ICU" << endl;
			msg = loadICUMessage(parms);
			if(msg.size() == 0){ // we didnt get a message from ICU for some reason, return the default message
				//cout << "didnt get a message from ICU, using default message" << endl;
				return formatDefaultMessage(parms);
			}
			return msg;
		#else
			// NOTE TO PROGRAMMERS:
			// call a non-ICU message loading function here that has
			// non-ICU process locale discovery code and non-ICU message loading
			// otherwise the default message is always returned
			//cout << "PEGASUS_HAS_ICU is NOT defined, using default message" << endl;
			return formatDefaultMessage(parms);
		#endif	
				
		PEG_METHOD_EXIT();			
	}
	
	#ifdef PEGASUS_HAS_ICU
	  
	String MessageLoader::loadICUMessage(MessageLoaderParms &parms){
		
		PEG_METHOD_ENTER(TRC_L10N, "MessageLoader::loadICUMessage");
		String msg;
		UResourceBundle* resbundl;
		UErrorCode status = U_ZERO_ERROR;
		//String resbundl_path_ICU; 
		CString resbundl_path_ICU;
		
		// the static AcceptLangauges takes precedence over what parms.acceptlangauges has
		AcceptLanguages acceptlanguages;
		acceptlanguages = (_acceptlanguages.size() > 0) ? _acceptlanguages : parms.acceptlanguages;
		
		// get the correct path to the resource bundles
		resbundl_path_ICU = getQualifiedMsgPath(parms.msg_src_path).getCString();
		//cout << "USING PACKAGE PATH: " << endl;
		//cout << resbundl_path_ICU << endl;
		#ifdef PEGASUS_OS_OS400
		const char *atoe = resbundl_path_ICU;
		AtoE((char*)atoe);
		#endif
			
		
		
		if(_useProcessLocale || (acceptlanguages.size() == 0 && parms.useProcessLocale)){ // use the system default resource bundle
		        
			resbundl = ures_open((const char*)resbundl_path_ICU, uloc_getDefault() , &status); 
			
			if(U_SUCCESS(status)) {
				//cout << "PROCESS_LOCALE: opened resource bundle" << endl;
				UErrorCode cout_status = U_ZERO_ERROR;
				//cout << "PROCESS_LOCALE = " << ures_getLocale(resbundl, &cout_status) << endl;
				if(status == U_USING_FALLBACK_WARNING || status == U_USING_DEFAULT_WARNING){
					//cout << "PROCESS_LOCALE: using fallback or default" << endl;	
				}
    			msg = extractICUMessage(resbundl,parms);

			String cl_str(ures_getLocale(resbundl,&status));
			#ifdef PEGASUS_OS_OS400
			   CString cstr = cl_str.getCString();
		           const char *etoa = cstr;
			   EtoA((char*)etoa);
			   cl_str = etoa;
		        #endif
    			parms.contentlanguages.append(ContentLanguageElement(cl_str));
    			ures_close(resbundl); 
			} else {
				//cout << "PROCESS_LOCALE: could not open resouce, formatting default message" << endl;
    			msg = formatDefaultMessage(parms);
			}
			return msg;
		} else if (acceptlanguages.size() == 0 && parms.useThreadLocale){ // get AcceptLanguages from the current Thread
			AcceptLanguages *al = Thread::getLanguages();
			if(al != NULL){			
				acceptlanguages = *al;
				//cout << "THREAD_LOCALE: got acceptlanguages from thread" << endl;	
			}else { 
				//cout << "THREAD_LOCALE: thread returned NULL for acceptlanguages" << endl;
			 }		
		}
				
		const int size_locale_ICU = 50;
		char locale_ICU[size_locale_ICU];
		AcceptLanguageElement language_element = AcceptLanguageElement::EMPTY;
		
		// iterate through AcceptLanguages, use the first resource bundle match
		acceptlanguages.itrStart();
		//cout << "LOOPING THROUGH ACCEPTLANGUAGES..." << endl;
		while((language_element = acceptlanguages.itrNext()) != AcceptLanguageElement::EMPTY_REF){
			#ifdef PEGASUS_OS_OS400
		        CString cstr = language_element.getTag().getCString();
			const char *atoe = cstr;
			AtoE((char*)atoe);
			String tag(atoe);
		        uloc_getName((const char*)tag.getCString(), locale_ICU, size_locale_ICU, &status);
		        #else
			uloc_getName((const char*)(language_element.getTag()).getCString(), locale_ICU, size_locale_ICU, &status);
			#endif
			//cout << "locale_ICU = " << locale_ICU << endl;	
			// TODO: check to see if we have previously cached the resource bundle
			
			resbundl = ures_open((const char*)resbundl_path_ICU, locale_ICU, &status);
			
			if(U_SUCCESS(status)) {
				//cout << "ACCEPTLANGUAGES LOOP: opened resource bundle with " << language_element.getTag() << endl;
				if(status == U_USING_FALLBACK_WARNING || status == U_USING_DEFAULT_WARNING){
					//we want to use the ICU fallback behaviour in the following cases ONLY
					//cout << "ACCEPTLANGUAGES LOOP: ICU warns using FALLBACK or DEFAULT" << endl;
					if(acceptlanguages.size() == 1 || parms.useICUfallback){
						//cout << "ACCEPTLANGUAGES LOOP: acceptlanguages.size == 1 or useICUfallback true, using ICU fallback behaviour..." << endl;
						msg = extractICUMessage(resbundl,parms);

						String cl_str(ures_getLocale(resbundl,&status));
                                                #ifdef PEGASUS_OS_OS400
			                           CString cstr = cl_str.getCString();
		                                   const char *etoa = cstr;
			                           EtoA((char*)etoa);
			                           cl_str = etoa;
		                                #endif
						parms.contentlanguages.append(ContentLanguageElement(cl_str));
						ures_close(resbundl);
						break;
					}	
				}
				else{  // we found an exact resource bundle match, extract, and set ContentLanguage 
					//cout << "ACCEPTLANGUAGES LOOP: found an EXACT resource bundle MATCH" << endl;
					msg = extractICUMessage(resbundl,parms);
					parms.contentlanguages.append(ContentLanguageElement(language_element.getTag()));	
    				ures_close(resbundl); 
    				break;
				}	
			} else { // possible errors, ex: message path incorrect
    			// for now do nothing, let the while loop continue
    			//cout << "ACCEPTLANGUAGES LOOP: could NOT open a resource for: " << language_element.getTag() << endl;			 	   	
			}
			status = U_ZERO_ERROR;  // reset status
		}
		// now if we DIDNT get a message, we want to enable ICU fallback for the highest priority language
		if(msg.size() == 0 && acceptlanguages.size() > 0){
			//cout << "USING ICU FALLBACK" << endl;
			language_element = acceptlanguages.getLanguageElement(0);

			#ifdef PEGASUS_OS_OS400
		        CString cstr = language_element.getTag().getCString();
			const char *atoe = cstr;
			AtoE((char*)atoe);
			String tag(atoe);
		        uloc_getName((const char*)tag.getCString(), locale_ICU, size_locale_ICU, &status);
		        #else
			uloc_getName((const char*)(language_element.getTag()).getCString(), locale_ICU, size_locale_ICU, &status);
			#endif
			//cout << "locale_ICU in fallback = " << locale_ICU << endl;
			status = U_ZERO_ERROR;
			resbundl = ures_open((const char*)resbundl_path_ICU, locale_ICU, &status);
			String cl_str_;
			if(U_SUCCESS(status)) {
				if(status == U_USING_DEFAULT_WARNING){
					//cout << "PRIORITY ICU FALLBACK: using default resource bundle with " << language_element.getTag() << endl;
					status = U_ZERO_ERROR;
					resbundl = ures_open((const char*)resbundl_path_ICU, "", &status);
					if(U_SUCCESS(status)) {
						msg = extractICUMessage(resbundl,parms);
			    		cl_str_ = ures_getLocale(resbundl,&status);
					}
				}else{
			    	msg = extractICUMessage(resbundl,parms);
			    	cl_str_ = ures_getLocale(resbundl,&status);
			    }
			#ifdef PEGASUS_OS_OS400
			    CString cstr_ = cl_str_.getCString();
			    const char *etoa = cstr_;
			    EtoA((char*)etoa);
			    cl_str_ = etoa;
		        #endif
			    if(cl_str_ != "root")
				parms.contentlanguages.append(ContentLanguageElement(cl_str_));
			    ures_close(resbundl);
			}
		}
		if(msg.size() == 0){ 			// else if no message, load message from root bundle explicitly
			//cout << "EXHAUSTED ACCEPTLANGUAGES: using root bundle to extract message" << endl;
			status = U_ZERO_ERROR;	
			resbundl = ures_open((const char*)resbundl_path_ICU, "", &status);
			if(U_SUCCESS(status)){
				//cout << "EXHAUSTED ACCEPTLANGUAGES: opened root resource bundle" << endl;
				msg = extractICUMessage(resbundl,parms);
				//parms.contentlanguages.append(ContentLanguageElement(String(ures_getLocale(resbundl,&status))));
			}else { 
				//cout << "EXHAUSTED ACCEPTLANGUAGES: could NOT open root resource bundle" << endl; 
			}

		}
		PEG_METHOD_EXIT();
		return msg; 
	}
	
	String MessageLoader::extractICUMessage(UResourceBundle * resbundl, MessageLoaderParms &parms){
		UErrorCode status = U_ZERO_ERROR;	
		int32_t msgLen = 0;

		#ifdef PEGASUS_OS_OS400
		CString cstr = parms.msg_id.getCString();
		const char *atoe = cstr;
		AtoE((char*)atoe);
		const UChar *msg = ures_getStringByKey(resbundl, (const char*)atoe, &msgLen, &status);
		#else
		const UChar *msg = ures_getStringByKey(resbundl, (const char*)parms.msg_id.getCString(), &msgLen, &status);
		#endif
   		if(U_FAILURE(status)) {	
	  		//cout << "could not extract ICU Message" << endl;
	  		return String::EMPTY;
		}		

		return formatICUMessage(resbundl, msg, msgLen, parms);
	}
	
	String MessageLoader::uChar2String(UChar * uchar_str){
		return String((const Char16 *)uchar_str);
	} 
	
	String MessageLoader::uChar2String(UChar * uchar_str, int len){
		return String((const Char16 *)uchar_str, len);
	}
	
	UChar* MessageLoader::string2UChar(String s){
		return (UChar*)((uint16_t *)s.getChar16Data());
	}
	
	
	String MessageLoader::formatICUMessage(UResourceBundle * resbundl, const UChar * msg, int msg_len, MessageLoaderParms &parms){
		
		// format the message
		UnicodeString msg_pattern(msg, msg_len);
		UnicodeString msg_formatted;
		UErrorCode status = U_ZERO_ERROR;
		const int arg_count = 10;
		char * locale;
		if(resbundl == NULL)
			 locale = ULOC_US;
		else
			 locale = const_cast<char *>(ures_getLocale(resbundl, &status));
		
		//cout << "FORMAT ICU MESSAGE: using locale = " << locale << endl;
		
		char lang[4]; 
		char cc[4];
		char var[arg_count];
		uloc_getLanguage(locale, lang, 4, &status);
		uloc_getCountry(locale, cc, 4, &status);
		uloc_getVariant(locale, var, 10, &status);
		Locale localeID(lang,cc,var);
		
		status = U_ZERO_ERROR;
		MessageFormat formatter(msg_pattern, localeID, status);
		Formattable args[arg_count];
		xferFormattables(parms, args);
		Formattable args_obj(args,arg_count);
		status = U_ZERO_ERROR;
		msg_formatted = formatter.format(args_obj, msg_formatted, status);
       	
		return uChar2String(const_cast<UChar *>(msg_formatted.getBuffer()), msg_formatted.length());
	} 
	
	void MessageLoader::xferFormattables(MessageLoaderParms &parms, Formattable *formattables){	
		
		Formatter::Arg arg_arr[10] = {parms.arg0,parms.arg1,parms.arg2,parms.arg3,parms.arg4,parms.arg5,
								 	  parms.arg6,parms.arg7,parms.arg8,parms.arg9};
		//cout << "XFERFORMATTABLES" << endl;
		for(int i = 0; i < 10; i++){  
			//cout << "arg" << i << " = " << arg_arr[i].toString() << endl;
			switch (arg_arr[i]._type)
    		{
				case Formatter::Arg::INTEGER:   formattables[i] = (int32_t)arg_arr[i]._integer;break;
				case Formatter::Arg::UINTEGER:  formattables[i] = (int32_t)arg_arr[i]._uinteger;break;
				case Formatter::Arg::BOOLEAN:   formattables[i] = (int32_t)arg_arr[i]._boolean;break;
				case Formatter::Arg::REAL:      formattables[i] = (float)arg_arr[i]._real;break;
				case Formatter::Arg::LINTEGER:  formattables[i] = (double)arg_arr[i]._lInteger;break;
				case Formatter::Arg::ULINTEGER: formattables[i] = (double)arg_arr[i]._lUInteger;break;
				case Formatter::Arg::STRING:    formattables[i] = Formattable(string2UChar(arg_arr[i]._string));break;
	    		case Formatter::Arg::VOIDT:     formattables[i] = "";break;
			}	
		}
	}
	
	#endif
	
	String MessageLoader::formatDefaultMessage(MessageLoaderParms &parms){
	
		PEG_METHOD_ENTER(TRC_L10N, "MessageLoader::formatDefaultMessage");
		// NOTE TO PROGRAMMERS: using native substitution functions
		// ie. calling Formatter::format()
		// can result in incorrect locale handling of substitutions	
		
		// locale INSENSITIVE formatting code
		//cout << "using locale INSENSITIVE formatting" << endl;
		parms.contentlanguages.clear(); // this could have previously been set by ICU
		//cout << parms.toString() << endl;
		//cout << "ml:" << parms.default_msg << endl;
		return Formatter::format(parms.default_msg,
		 						 	parms.arg0,
								 	parms.arg1,
								 	parms.arg2, 
								 	parms.arg3,
								 	parms.arg4,
								 	parms.arg5,
								 	parms.arg6,
								 	parms.arg7,
								 	parms.arg8,
								 	parms.arg9);
								 
		
		PEG_METHOD_EXIT(); 
	}
	
	String MessageLoader::getQualifiedMsgPath(String path){
		PEG_METHOD_ENTER(TRC_L10N, "MessageLoader::getQualifiedPackageName");
		
		if(pegasus_MSG_HOME.size() == 0)
				initPegasusMsgHome();
		
		if(path.size() == 0)	 
			return pegasus_MSG_HOME + server_resbundl_name;
		
		Char16 delim = '/'; // NOTE TO PROGRAMMERS: WINDOWS and non-UNIX platforms should redefine delim here
		Uint32 i;
		if(( i = path.find(delim)) != PEG_NOT_FOUND && i == 0){ //  fully qualified package name
			return path;
		}
		
		return pegasus_MSG_HOME	+ path;  // relative path and package name
		
		PEG_METHOD_EXIT(); 
	}
	
	void MessageLoader::setPegasusMsgHome(String home){
		PEG_METHOD_ENTER(TRC_L10N, "MessageLoader::setPegasusMsgHome");
			//cout << "MessageLoader::setPegasusMsgHome()" << endl;
			pegasus_MSG_HOME = home + "/";
			// TODO: remove the next call once test cases are compatible with ICU messages
			checkDefaultMsgLoading();
		PEG_METHOD_EXIT();
	} 
	
	void MessageLoader::initPegasusMsgHome(){
		//cout << "INITPEGASUSMSGHOME:" << endl;
		#ifdef PEGASUS_OS_OS400
			pegasus_MSG_HOME = OS400_DEFAULT_MESSAGE_SOURCE;
		#else
			const char* env = getenv("PEGASUS_HOME");
    		if (env != NULL){
				pegasus_MSG_HOME = env;
				pegasus_MSG_HOME.append("/msg/");
    		}
				
		#endif 
	}
	
	void MessageLoader::checkDefaultMsgLoading(){
			// TODO: remove this function once test cases are compatible with ICU messages
			const char* env = getenv("PEGASUS_USE_DEFAULT_MESSAGES");
    		if (env != NULL)
				_useDefaultMsg = true;	
	}

PEGASUS_NAMESPACE_END


