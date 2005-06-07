//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
// Modified By: Josephine Eskaline Joyce, IBM (jojustin@in.ibm.com) for Bug#3032
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/MessageLoader.h>
#include <Pegasus/Common/Thread.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/CommonUTF.h>
#include <Pegasus/Common/FileSystem.h>
#include <iostream>
#ifdef PEGASUS_OS_OS400
#include "OS400ConvertChar.h"
#endif
PEGASUS_NAMESPACE_BEGIN
PEGASUS_USING_STD;

static const int ID_INVALID = -1;
static const String server_resbundl_name = "pegasus/pegasusServer";
String MessageLoader::pegasus_MSG_HOME = String();
Boolean MessageLoader::_useProcessLocale = false;
Boolean MessageLoader::_useDefaultMsg = false;
AcceptLanguages MessageLoader::_acceptlanguages = AcceptLanguages();

	String MessageLoader::getMessage(MessageLoaderParms &parms){
		PEG_METHOD_ENTER(TRC_L10N, "MessageLoader::getMessage");
		try{
		        parms.contentlanguages.clear();

#if defined (PEGASUS_HAS_MESSAGES) && defined (PEGASUS_HAS_ICU)
		        if (InitializeICU::initICUSuccessful())
                        {
                            String msg = loadICUMessage(parms);

                            if (msg.size() > 0)
                            {
		 	        PEG_METHOD_EXIT();
			        return (msg);
                            }
                        }

#endif
			// NOTE: the default message is returned if:
			// Message loading is DIABLED or
			// A non-ICU message loading function is called that has
			// non-ICU process locale discovery code and non-ICU 
			// message loading or
			// InitializeICU::initICUSuccessful() is failed or
			// Did not get a message from ICU

			PEG_METHOD_EXIT();
			return formatDefaultMessage(parms);

		}catch(Exception&){
			PEG_METHOD_EXIT();
			return String("AN INTERNAL ERROR OCCURED IN MESSAGELOADER: ").append(parms.default_msg);
		}
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

		// if someone has set the global behaviour to use default messages
		// Note: this must be after the call to getQualifiedMsgPath
		if (_useDefaultMsg)
		{
			PEG_METHOD_EXIT();
			return formatDefaultMessage(parms);
		}

		//cout << "USING PACKAGE PATH: " << endl;
		//cout << resbundl_path_ICU << endl;
		PEG_TRACE_STRING(TRC_L10N, Tracer::LEVEL4, "Using resource bundle path:");
		PEG_TRACE_STRING(TRC_L10N, Tracer::LEVEL4, resbundl_path_ICU);
#ifdef PEGASUS_OS_OS400
		const char *atoe = resbundl_path_ICU;
		AtoE((char*)atoe);
#endif



		if(_useProcessLocale || (acceptlanguages.size() == 0 && parms.useProcessLocale)){ // use the system default resource bundle

			resbundl = ures_open((const char*)resbundl_path_ICU, uloc_getDefault() , &status);

			if(U_SUCCESS(status)) {
				//cout << "PROCESS_LOCALE: opened resource bundle" << endl;
				UErrorCode _status = U_ZERO_ERROR;
				//cout << "PROCESS_LOCALE = " << ures_getLocale(resbundl, &_status) << endl;
				PEG_TRACE_STRING(TRC_L10N, Tracer::LEVEL4, "Using process locale:");
				PEG_TRACE_STRING(TRC_L10N, Tracer::LEVEL4, ures_getLocale(resbundl, &_status));
				if(status == U_USING_FALLBACK_WARNING || status == U_USING_DEFAULT_WARNING){
					//cout << "PROCESS_LOCALE: using fallback or default" << endl;
					PEG_TRACE_STRING(TRC_L10N, Tracer::LEVEL4, "Using process locale fallback or default");
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
				PEG_TRACE_STRING(TRC_L10N, Tracer::LEVEL4, "Using process locale.  Could not open resource, formatting default message.");
    			msg = formatDefaultMessage(parms);
			}
			PEG_METHOD_EXIT();
			return msg;
		} else if (acceptlanguages.size() == 0 && parms.useThreadLocale){ // get AcceptLanguages from the current Thread
			AcceptLanguages *al = Thread::getLanguages();
			if(al != NULL){
				acceptlanguages = *al;
				//cout << "THREAD_LOCALE: got acceptlanguages from thread" << endl;
				PEG_TRACE_STRING(TRC_L10N, Tracer::LEVEL4, "Using thread locale: got AcceptLanguages from thread.");
			}else {
				//cout << "THREAD_LOCALE: thread returned NULL for acceptlanguages" << endl;
				PEG_TRACE_STRING(TRC_L10N, Tracer::LEVEL4, "Using thread locale: thread returned NULL for AcceptLanguages.");
			 }
		}

		const int size_locale_ICU = 50;
		char locale_ICU[size_locale_ICU];
		AcceptLanguageElement language_element = AcceptLanguageElement::EMPTY;

		// iterate through AcceptLanguages, use the first resource bundle match
		acceptlanguages.itrStart();
		//cout << "LOOPING THROUGH ACCEPTLANGUAGES..." << endl;
		PEG_TRACE_STRING(TRC_L10N, Tracer::LEVEL4, "Looping through AcceptLanguages...");
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
				PEG_TRACE_STRING(TRC_L10N, Tracer::LEVEL4, "ACCEPTLANGUAGES LOOP: opened resource bundle with:");
				PEG_TRACE_STRING(TRC_L10N, Tracer::LEVEL4, language_element.getTag());
				if(status == U_USING_FALLBACK_WARNING || status == U_USING_DEFAULT_WARNING){
					//we want to use the ICU fallback behaviour in the following cases ONLY
					//cout << "ACCEPTLANGUAGES LOOP: ICU warns using FALLBACK or DEFAULT" << endl;
					PEG_TRACE_STRING(TRC_L10N, Tracer::LEVEL4,"ACCEPTLANGUAGES LOOP: ICU warns using FALLBACK or DEFAULT");
					if((acceptlanguages.size() == 1) && (!parms.useICUfallback) && (status == U_USING_DEFAULT_WARNING)){
						// in this case we want to return messages from the root bundle
						status = U_ZERO_ERROR;
						//cout << "ML::acceptlang.size =1 && !parms.useICUfallback && U_USING_DEFAULT_WARNING" << endl;
						PEG_TRACE_STRING(TRC_L10N, Tracer::LEVEL4,"acceptlang.size =1 && !parms.useICUfallback && U_USING_DEFAULT_WARNING, using root bundle.");

                  // Reopen the bundle in the root locale
                  ures_close(resbundl);
                  resbundl = ures_open((const char*)resbundl_path_ICU, "", &status);
                  if(U_SUCCESS(status)) {
                    msg = extractICUMessage(resbundl,parms);
                    ures_close(resbundl);
                    break;
                  }
					}
					else if(acceptlanguages.size() == 1 || parms.useICUfallback){
						//cout << "ACCEPTLANGUAGES LOOP: acceptlanguages.size == 1 or useICUfallback true, using ICU fallback behaviour..." << endl;
						PEG_TRACE_STRING(TRC_L10N, Tracer::LEVEL4,"ACCEPTLANGUAGES LOOP: acceptlanguages.size == 1 or useICUfallback true, using ICU fallback behaviour...");
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
					PEG_TRACE_STRING(TRC_L10N, Tracer::LEVEL4,"ACCEPTLANGUAGES LOOP: found an EXACT resource bundle MATCH:");
					PEG_TRACE_STRING(TRC_L10N, Tracer::LEVEL4,language_element.getTag());
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
			PEG_TRACE_STRING(TRC_L10N, Tracer::LEVEL4,"No message was loaded, using ICU fallback behaviour.");
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
					PEG_TRACE_STRING(TRC_L10N, Tracer::LEVEL4,"PRIORITY ICU FALLBACK: using default resource bundle with ");
					PEG_TRACE_STRING(TRC_L10N, Tracer::LEVEL4, language_element.getTag());
					status = U_ZERO_ERROR;

               // Reopen the bundle in the root locale
               ures_close(resbundl);
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
			PEG_TRACE_STRING(TRC_L10N, Tracer::LEVEL4,"EXHAUSTED ACCEPTLANGUAGES: using root bundle to extract message");
			status = U_ZERO_ERROR;
			resbundl = ures_open((const char*)resbundl_path_ICU, "", &status);
			if(U_SUCCESS(status)){
				//cout << "EXHAUSTED ACCEPTLANGUAGES: opened root resource bundle" << endl;
				PEG_TRACE_STRING(TRC_L10N, Tracer::LEVEL4,"EXHAUSTED ACCEPTLANGUAGES: opened root resource bundle");
				msg = extractICUMessage(resbundl,parms);
				//parms.contentlanguages.append(ContentLanguageElement(String(ures_getLocale(resbundl,&status))));
            ures_close(resbundl);
			}else {
				//cout << "EXHAUSTED ACCEPTLANGUAGES: could NOT open root resource bundle" << endl;
				PEG_TRACE_STRING(TRC_L10N, Tracer::LEVEL4,"EXHAUSTED ACCEPTLANGUAGES: could NOT open root resource bundle");
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

	String MessageLoader::formatICUMessage(UResourceBundle * resbundl, const UChar * msg, int msg_len, MessageLoaderParms &parms){

		// format the message
		UnicodeString msg_pattern(msg, msg_len);
		UnicodeString msg_formatted;
		UErrorCode status = U_ZERO_ERROR;
		const int arg_count = 10;
		const char *locale;
		if(resbundl == NULL)
			 locale = ULOC_US;
		else
			 locale = ures_getLocale(resbundl, &status);

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

      if (parms.arg0._type != Formatter::Arg::VOIDT)
        xferFormattable(parms.arg0, args[0]);
      if (parms.arg1._type != Formatter::Arg::VOIDT)
        xferFormattable(parms.arg1, args[1]);
      if (parms.arg2._type != Formatter::Arg::VOIDT)
        xferFormattable(parms.arg2, args[2]);
      if (parms.arg3._type != Formatter::Arg::VOIDT)
        xferFormattable(parms.arg3, args[3]);
      if (parms.arg4._type != Formatter::Arg::VOIDT)
        xferFormattable(parms.arg4, args[4]);
      if (parms.arg5._type != Formatter::Arg::VOIDT)
        xferFormattable(parms.arg5, args[5]);
      if (parms.arg6._type != Formatter::Arg::VOIDT)
        xferFormattable(parms.arg6, args[6]);
      if (parms.arg7._type != Formatter::Arg::VOIDT)
        xferFormattable(parms.arg7, args[7]);
      if (parms.arg8._type != Formatter::Arg::VOIDT)
        xferFormattable(parms.arg8, args[8]);
      if (parms.arg9._type != Formatter::Arg::VOIDT)
        xferFormattable(parms.arg9, args[9]);

		Formattable args_obj(args,arg_count);
		status = U_ZERO_ERROR;
		msg_formatted = formatter.format(args_obj, msg_formatted, status);

		return uChar2String(const_cast<UChar *>(msg_formatted.getBuffer()), msg_formatted.length());
	}

	void MessageLoader::xferFormattable(Formatter::Arg& arg, Formattable &formattable)
   {
			//cout << "arg" << " = " << arg.toString() << endl;
			switch (arg._type)
    		{
				case Formatter::Arg::INTEGER:
                    formattable = (int32_t)arg._integer;
                    break;
				case Formatter::Arg::UINTEGER:
                    // Note: the ICU Formattable class doesn't support
                    // unsigned 32.  Cast to signed 64.
                    formattable = (int64_t)arg._uinteger;
                    break;
				case Formatter::Arg::BOOLEAN:
                    // Note: the ICU formattable class doesn't support
                    // boolean.  Turn it into a string.
                    if (!arg._boolean)
                        formattable = Formattable("false");
                    else
                        formattable = Formattable("true");
                    break;
				case Formatter::Arg::REAL:
                    formattable = (double)arg._real;
                    break;
				case Formatter::Arg::LINTEGER:
                    // Note: this uses a Formattable constructor that is
                    // labelled ICU 2.8 draft.  Assumes that Pegasus uses
                    // at least ICU 2.8.
                    formattable = (int64_t)arg._lInteger;
                    break;
				case Formatter::Arg::ULINTEGER:
                    // Note: the ICU Formattable class doesn't support
                    // unsigned 64.  If the number is too big for signed 64
                    // then turn it into a string.  This string will  
                    // not be formatted for the locale, but at least the number
                    // will appear in the message.
                    if (arg._lUInteger >  PEGASUS_UINT64_LITERAL(0x7FFFFFFFFFFFFFFF))
                    {
                        char buffer[32];  // Should need 21 chars max
                        sprintf(buffer, "%" PEGASUS_64BIT_CONVERSION_WIDTH "u", arg._lUInteger);
                        formattable = Formattable(buffer);
                    }
                    else
                    {
                        formattable = (int64_t)arg._lUInteger;
                    }
                    break;
				case Formatter::Arg::STRING:
                    formattable = Formattable((UChar*)arg._string.getChar16Data());
                    break;
	    		case Formatter::Arg::VOIDT:
            default:
              formattable = "";
              break;
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
		PEG_METHOD_EXIT();
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


	}

	String MessageLoader::getQualifiedMsgPath(String path){
		PEG_METHOD_ENTER(TRC_L10N, "MessageLoader::getQualifiedMsgPath");

		if(pegasus_MSG_HOME.size() == 0)
			initPegasusMsgHome(String::EMPTY);

		if(path.size() == 0)
		{
			PEG_METHOD_EXIT();
			return pegasus_MSG_HOME + server_resbundl_name;
		}

		Char16 delim = '/'; // NOTE TO PROGRAMMERS: WINDOWS and non-UNIX platforms should redefine delim here
		Uint32 i;
		if(( i = path.find(delim)) != PEG_NOT_FOUND && i == 0){ //  fully qualified package name
			PEG_METHOD_EXIT();
			return path;
		}

		PEG_METHOD_EXIT();
		return pegasus_MSG_HOME	+ path;  // relative path and package name

	}

	void MessageLoader::setPegasusMsgHome(String home){
		PEG_METHOD_ENTER(TRC_L10N, "MessageLoader::setPegasusMsgHome");
			//cout << "MessageLoader::setPegasusMsgHome()" << endl;
			pegasus_MSG_HOME = home + "/";
			// TODO: remove the next call once test cases are compatible with ICU messages
			checkDefaultMsgLoading();
		PEG_METHOD_EXIT();
	}
    void MessageLoader::setPegasusMsgHomeRelative(const String& argv0)
    {
#ifdef PEGASUS_HAS_MESSAGES
      try
      {
        String startingDir, pathDir;

#ifdef PEGASUS_OS_TYPE_WINDOWS
        if(PEG_NOT_FOUND == argv0.find('\\'))
        {
          char exeDir[_MAX_PATH];
          HMODULE module = GetModuleHandle(NULL);
          if(NULL != module )
          {
              DWORD filename = GetModuleFileName(module,exeDir ,sizeof(exeDir));
               if(0 != filename)
               {
                   String path(exeDir);
                   Uint32 command = path.reverseFind('\\');
                   startingDir = path.subString(0, command+1);
               }
          }
        }
        else
        {
            Uint32 command = argv0.reverseFind('\\');
            startingDir = argv0.subString(0, command+1);
        }
#endif

#ifdef PEGASUS_OS_TYPE_UNIX
        if(PEG_NOT_FOUND  != argv0.find('/'))
        {
            Uint32 command = argv0.reverseFind('/');
            startingDir = argv0.subString(0, command+1);
        }
        else
        {
          if(FileSystem::existsNoCase(argv0))
          {
             FileSystem::getCurrentDirectory(startingDir);
             startingDir.append("/");
          }
          else
          {
           String path;
           #ifdef PEGASUS_PLATFORM_OS400_ISERIES_IBM
             #pragma convert(37)
                const char* env = getenv("PATH");
                if(env != NULL)
                  EtoA((char*)env);
             #pragma convert(0)
           #else
                const char* env = getenv("PATH");
           #endif
           if (env != NULL)
              path.assign(env);
           String pathDelim = FileSystem::getPathDelimiter();
           Uint32 size = path.size();
           while(path.size() > 0)
           {
             try
             {
              Uint32 delim = path.find(pathDelim);
              if(delim != PEG_NOT_FOUND)
              {
                 pathDir = path.subString(0,delim);
                 path.remove(0,(delim+1));
               }
               else
               {
                     pathDir = path;
                     path = "";
               }
               String commandPath = pathDir.append('/');
               commandPath = commandPath.append(argv0) ;
               Boolean dirContent = FileSystem::existsNoCase(commandPath);
               if(dirContent)
               {
                    startingDir = pathDir;
                    break;
               }
             }
             catch(Exception &e)
             {
                //Have to do nothing. 
                //Catching the exception if there is any exception while searching in the path variable
             }
           }
          }
        }
#endif
         initPegasusMsgHome(startingDir);
      }
      catch(Exception &e)
      {
                //Have to do nothing.
                //Catching the exception if there is any exception while searching in the path variable
      }
#endif
}


void MessageLoader::initPegasusMsgHome(const String & startDir)
{
    String startingDir = startDir; 
    if (startingDir.size() == 0)
    {
#ifdef PEGASUS_PLATFORM_OS400_ISERIES_IBM
#pragma convert(37)
         const char* env = getenv("PEGASUS_MSG_HOME");
            if(env != NULL)
               EtoA((char*)env);
#pragma convert(0)
#else
        const char* env = getenv("PEGASUS_MSG_HOME");
#endif

        if (env != NULL)
            startingDir.assign(env);
    }

#ifdef PEGASUS_DEFAULT_MESSAGE_SOURCE
        if(System::is_absolute_path(
                (const char *)PEGASUS_DEFAULT_MESSAGE_SOURCE))
        {
            pegasus_MSG_HOME = PEGASUS_DEFAULT_MESSAGE_SOURCE;
            pegasus_MSG_HOME.append('/');
        }
        else
        {
            if (startingDir.size() != 0)
            {
               pegasus_MSG_HOME = startingDir;
                pegasus_MSG_HOME.append('/');
            }
            pegasus_MSG_HOME.append(PEGASUS_DEFAULT_MESSAGE_SOURCE);
            pegasus_MSG_HOME.append('/');
        }
#else
        if (startingDir.size() != 0)
        {
            pegasus_MSG_HOME = startingDir;
            pegasus_MSG_HOME.append("/");
        }
        else
        {
             // Will use current working directory
        }
#endif
        checkDefaultMsgLoading();
     }

	void MessageLoader::checkDefaultMsgLoading(){
	  // Note: this function is a special hook for the automated tests (poststarttests)
	  // Since the automated tests expect the old hardcoded default messages, an env var
	  // will be used to tell this code to ignore ICU and return the default message.
	  // This will allow poststarttests to run with ICU installed.
	  // TODO: remove this function once test cases are compatible with ICU messages
#ifdef PEGASUS_OS_OS400
#pragma convert(37)
#endif
	  const char* env = getenv("PEGASUS_USE_DEFAULT_MESSAGES");
#ifdef PEGASUS_OS_OS400
#pragma convert(0)
#endif
	  if (env != NULL)
	    _useDefaultMsg = true;
	}

MessageLoaderParms::MessageLoaderParms()
{
	useProcessLocale = false;
	useThreadLocale = true;

#ifdef PEGASUS_HAS_ICU
	useICUfallback = false;
#endif

	acceptlanguages = AcceptLanguages();
	contentlanguages = ContentLanguages();
}

MessageLoaderParms::MessageLoaderParms(
    const String& id,
    const String& msg,
    const Formatter::Arg& arg0,
    const Formatter::Arg& arg1,
    const Formatter::Arg& arg2,
    const Formatter::Arg& arg3,
    const Formatter::Arg& arg4,
    const Formatter::Arg& arg5,
    const Formatter::Arg& arg6,
    const Formatter::Arg& arg7,
    const Formatter::Arg& arg8,
    const Formatter::Arg& arg9)
{
    msg_id = id;
    default_msg = msg;
    _init();
    this->arg0 = arg0;
    this->arg1 = arg1;
    this->arg2 = arg2;
    this->arg3 = arg3;
    this->arg4 = arg4;
    this->arg5 = arg5;
    this->arg6 = arg6;
    this->arg7 = arg7;
    this->arg8 = arg8;
    this->arg9 = arg9;
}

MessageLoaderParms::MessageLoaderParms(
    const String& id,
    const String& msg)
{
    msg_id = id;
    default_msg = msg;
    _init();
}

MessageLoaderParms::MessageLoaderParms(
    const String& id,
    const String& msg,
    const Formatter::Arg& arg0)
{
    msg_id = id;
    default_msg = msg;
    _init();
    this->arg0 = arg0;
}

MessageLoaderParms::MessageLoaderParms(
    const String& id,
    const String& msg,
    const Formatter::Arg& arg0,
    const Formatter::Arg& arg1)
{
    msg_id = id;
    default_msg = msg;
    _init();
    this->arg0 = arg0;
    this->arg1 = arg1;
}

MessageLoaderParms::MessageLoaderParms(
    const String& id,
    const String& msg,
    const Formatter::Arg& arg0,
    const Formatter::Arg& arg1,
    const Formatter::Arg& arg2)
{
    msg_id = id;
    default_msg = msg;
    _init();
    this->arg0 = arg0;
    this->arg1 = arg1;
    this->arg2 = arg2;
}

MessageLoaderParms::MessageLoaderParms(
    const String& id,
    const String& msg,
    const Formatter::Arg& arg0,
    const Formatter::Arg& arg1,
    const Formatter::Arg& arg2,
    const Formatter::Arg& arg3)
{
    msg_id = id;
    default_msg = msg;
    _init();
    this->arg0 = arg0;
    this->arg1 = arg1;
    this->arg2 = arg2;
    this->arg3 = arg3;
}

MessageLoaderParms::MessageLoaderParms(
    const char* id,
    const char* msg)
{
    msg_id = id;
    default_msg = msg;
    _init();
}

MessageLoaderParms::MessageLoaderParms(
    const char* id,
    const char* msg,
    const String& arg0)
{
    msg_id = id;
    default_msg = msg;
    _init();
    this->arg0 = arg0;
}

MessageLoaderParms::MessageLoaderParms(
    const char* id,
    const char* msg,
    const String& arg0,
    const String& arg1)
{
    msg_id = id;
    default_msg = msg;
    _init();
    this->arg0 = arg0;
    this->arg1 = arg1;
}

void MessageLoaderParms::_init()
{
    useProcessLocale = false;
    useThreadLocale = true;

#ifdef PEGASUS_HAS_ICU
    useICUfallback = false;
#endif

    acceptlanguages = AcceptLanguages::EMPTY;
    contentlanguages = ContentLanguages::EMPTY;

    this->arg0 = Formatter::DEFAULT_ARG;
    this->arg1 = Formatter::DEFAULT_ARG;
    this->arg2 = Formatter::DEFAULT_ARG;
    this->arg3 = Formatter::DEFAULT_ARG;
    this->arg4 = Formatter::DEFAULT_ARG;
    this->arg5 = Formatter::DEFAULT_ARG;
    this->arg6 = Formatter::DEFAULT_ARG;
    this->arg7 = Formatter::DEFAULT_ARG;
    this->arg8 = Formatter::DEFAULT_ARG;
    this->arg9 = Formatter::DEFAULT_ARG;
}

String MessageLoaderParms::toString()
{
    String s;
    String processLoc,threadLoc,ICUfallback;
    processLoc = (useProcessLocale) ? "true" : "false";
    threadLoc = (useThreadLocale) ? "true" : "false";
#ifdef PEGASUS_HAS_ICU
    ICUfallback = (useICUfallback) ? "true" : "false";
#endif

    s.append("msg_id = " + msg_id + "\n");
    s.append("default_msg = " + default_msg + "\n");
    s.append("msg_src_path = " + msg_src_path + "\n");
    s.append("acceptlanguages = " + acceptlanguages.toString() + "\n");
    s.append("contentlanguages = " + contentlanguages.toString() + "\n");

    s.append("useProcessLocale = " + processLoc + "\n");
    s.append("useThreadLocale = " + threadLoc + "\n");
#ifdef PEGASUS_HAS_ICU
    s.append("useICUfallback = " + ICUfallback + "\n");
#endif
    s.append("arg0 = " + arg0.toString() + "\n" + "arg1 = " + arg1.toString() + "\n" + "arg2 = " + arg2.toString() + "\n" + "arg3 = " + arg3.toString() + "\n" +
	      "arg4 = " + arg4.toString() + "\n" + "arg5 = " + arg5.toString() + "\n" + "arg6 = " + arg6.toString() + "\n" + "arg7 = " + arg7.toString() + "\n" +
	      "arg8 = " + arg8.toString() + "\n" + "arg9 = " + arg9.toString() + "\n\n");

    return s;
}

MessageLoaderParms::~MessageLoaderParms()
{
}

PEGASUS_NAMESPACE_END
