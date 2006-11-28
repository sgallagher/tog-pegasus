//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/MessageLoader.h>
#include <Pegasus/Common/Thread.h>
#include <Pegasus/Common/Tracer.h>
#include <Pegasus/Common/Constants.h>
#include <Pegasus/Common/CommonUTF.h>
#include <Pegasus/Common/FileSystem.h>
#include <Pegasus/Common/LanguageParser.h>
#include <iostream>
#ifdef PEGASUS_OS_OS400
#include "EBCDIC_OS400.h"
#endif
PEGASUS_NAMESPACE_BEGIN
PEGASUS_USING_STD;

static const int ID_INVALID = -1;
static const String server_resbundl_name = "pegasus/pegasusServer";
String MessageLoader::pegasus_MSG_HOME;
Boolean MessageLoader::_useProcessLocale = false;
Boolean MessageLoader::_useDefaultMsg = false;
AcceptLanguageList MessageLoader::_acceptlanguages;

String MessageLoader::getMessage(MessageLoaderParms& parms)
{
    PEG_METHOD_ENTER(TRC_L10N, "MessageLoader::getMessage");
    String msg;

    try
    {
        openMessageFile(parms);
        msg = getMessage2(parms);
        closeMessageFile(parms);
    }
    catch (Exception&)
    {
        msg = String("AN INTERNAL ERROR OCCURED IN MESSAGELOADER: ").append(
            parms.default_msg);
    }
    PEG_METHOD_EXIT();
    return msg;
}

String MessageLoader::getMessage2(MessageLoaderParms& parms)
{
    PEG_METHOD_ENTER(TRC_L10N, "MessageLoader::getMessage2");
    String msg;

    try
    {
#if defined (PEGASUS_HAS_MESSAGES) && defined (PEGASUS_HAS_ICU)
        if (!_useDefaultMsg && (parms._resbundl != NO_ICU_MAGIC))
        {
            msg = extractICUMessage(parms._resbundl,parms);
            if (msg.size() > 0)
            {
                PEG_METHOD_EXIT();
                return msg;
            }
        }
#endif
        // NOTE: the default message is returned if:
        // 1) The previous call to openMessageFile() set _useDefaultMsg
        // 2) parms._resbundl is set to NO_ICU_MAGIC from a previous
        //    call to openMessageFile() indicating an error
        //    (including InitializeICU::initICUSuccessful() failed)
        // 3) Message loading is DISABLED
        // 4) Did not get a message from ICU

        msg = formatDefaultMessage(parms);
    }
    catch (Exception&)
    {
        msg = String("AN INTERNAL ERROR OCCURED IN MESSAGELOADER: ").append(
            parms.default_msg);
    }

    PEG_METHOD_EXIT();
    return msg;
}

void MessageLoader::openMessageFile(MessageLoaderParms& parms)
{
    PEG_METHOD_ENTER(TRC_L10N, "MessageLoader::openMessageFile");

    try
    {
        parms.contentlanguages.clear();

#if defined (PEGASUS_HAS_MESSAGES) && defined (PEGASUS_HAS_ICU)
        if (InitializeICU::initICUSuccessful())
        {
            openICUMessageFile(parms);
        }
        // If initICUSuccessful() has returned false, then
        // _resbundl is still set to NO_ICU_MAGIC and
        // subsequent calls to getMessage2 will call
        // formatDefaultMessage() instead of using ICU.
#else
        // Set _resbundl to the "magic number" indicating there is no ICU
        // resource bundle to use
        parms._resbundl = NO_ICU_MAGIC;
#endif
    }
    catch (Exception&)
    {
        // Set _resbundl to the "magic number" indicating there was trouble
        // during openICUMessageFile(), so there is no ICU resource bundle
        // to use.
        parms._resbundl = NO_ICU_MAGIC;
    }

    PEG_METHOD_EXIT();
    return;
}

void MessageLoader::closeMessageFile(MessageLoaderParms& parms)
{
    PEG_METHOD_ENTER(TRC_L10N, "MessageLoader::closeMessageFile");

#if defined (PEGASUS_HAS_MESSAGES) && defined (PEGASUS_HAS_ICU)
    if (parms._resbundl != NO_ICU_MAGIC)
    {
        ures_close(parms._resbundl);
    }
#endif

    PEG_METHOD_EXIT();
    return;
}

#ifdef PEGASUS_HAS_ICU

void MessageLoader::openICUMessageFile(MessageLoaderParms &parms)
{
    PEG_METHOD_ENTER(TRC_L10N, "MessageLoader::openICUMessageFile");
    UErrorCode status = U_ZERO_ERROR;
    CString resbundl_path_ICU;

    const int size_locale_ICU = 50;

    // the static AcceptLangauges takes precedence over what
    // parms.acceptlangauges has
    AcceptLanguageList acceptlanguages;
    acceptlanguages = (_acceptlanguages.size() > 0) ?
        _acceptlanguages : parms.acceptlanguages;

    // get the correct path to the resource bundles
    resbundl_path_ICU = getQualifiedMsgPath(parms.msg_src_path).getCString();

    //cout << "USING PACKAGE PATH: " << endl;
    //cout << resbundl_path_ICU << endl;
    PEG_TRACE_STRING(TRC_L10N, Tracer::LEVEL4, "Using resource bundle path:");
    PEG_TRACE_STRING(TRC_L10N, Tracer::LEVEL4, resbundl_path_ICU);
#ifdef PEGASUS_OS_OS400
    const char *atoe = resbundl_path_ICU;
    AtoE((char*)atoe);
#endif

    if (_useProcessLocale ||
        (acceptlanguages.size() == 0 && parms.useProcessLocale))
    {
        // use the system default resource bundle

        parms._resbundl = ures_open(
            (const char*)resbundl_path_ICU, uloc_getDefault(), &status);

        if (U_SUCCESS(status))
        {
            UErrorCode _status = U_ZERO_ERROR;
            PEG_TRACE_STRING(TRC_L10N, Tracer::LEVEL4, "Using process locale:");
            PEG_TRACE_STRING(TRC_L10N, Tracer::LEVEL4,
                ures_getLocale(parms._resbundl, &_status));
            if (status == U_USING_FALLBACK_WARNING ||
                status == U_USING_DEFAULT_WARNING)
            {
                PEG_TRACE_STRING(TRC_L10N, Tracer::LEVEL4,
                    "Using process locale fallback or default");
            }
            const char * _locale = ures_getLocale(parms._resbundl,&status);
#ifdef PEGASUS_OS_OS400
            char tmplcl[size_locale_ICU];
            strcpy(tmplcl, _locale);
            EtoA(tmplcl);
            String localeStr(tmplcl);
#else
            String localeStr(_locale);
#endif
			// The "root" locale indicates that an ICU message bundle is not present
			// for the current locale setting.
            if (localeStr != "root")
            {
                parms.contentlanguages.append(LanguageTag(
                    LanguageParser::convertLocaleIdToLanguageTag(localeStr)));
            }
        }
        else
        {
            PEG_TRACE_STRING(TRC_L10N, Tracer::LEVEL4,
                "Using process locale.  Could not open resource, "
                    "formatting default message.");
            // magic number to indicate no ICU resource bundle to use
            parms._resbundl = NO_ICU_MAGIC;
        }
        PEG_METHOD_EXIT();
        return;
    }
    else if (acceptlanguages.size() == 0 && parms.useThreadLocale)
    {
        // get AcceptLanguageList from the current Thread
        AcceptLanguageList *al = Thread::getLanguages();
        if (al != NULL)
        {
            acceptlanguages = *al;
            PEG_TRACE_STRING(TRC_L10N, Tracer::LEVEL4,
                "Using thread locale: got AcceptLanguageList from thread.");
        }
        else
        {
            PEG_TRACE_STRING(TRC_L10N, Tracer::LEVEL4,
                "Using thread locale: thread returned NULL for "
                    "AcceptLanguageList.");
        }
    }

    char locale_ICU[size_locale_ICU];
    LanguageTag languageTag;

    // iterate through AcceptLanguageList, use the first resource bundle match
    PEG_TRACE_STRING(TRC_L10N, Tracer::LEVEL4,
        "Looping through AcceptLanguageList...");
    for (Uint32 index = 0; index < acceptlanguages.size(); index++)
    {
        languageTag = acceptlanguages.getLanguageTag(index);
#ifdef PEGASUS_OS_OS400
        CString cstr = languageTag.toString().getCString();
        const char *atoe = cstr;
        AtoE((char*)atoe);

        uloc_getName(atoe, locale_ICU, size_locale_ICU, &status);
#else
        uloc_getName(
            (const char*)(languageTag.toString()).getCString(),
            locale_ICU, size_locale_ICU, &status);
#endif
        // TODO: check to see if we have previously cached the resource bundle

        parms._resbundl =
            ures_open((const char*)resbundl_path_ICU, locale_ICU, &status);

        if (U_SUCCESS(status))
        {
            PEG_TRACE_STRING(TRC_L10N, Tracer::LEVEL4,
                "ACCEPTLANGUAGES LOOP: opened resource bundle with:");
            PEG_TRACE_STRING(TRC_L10N, Tracer::LEVEL4, languageTag.toString());
            if (status == U_USING_FALLBACK_WARNING ||
                status == U_USING_DEFAULT_WARNING)
            {
                // we want to use the ICU fallback behaviour in the following
                // cases ONLY
                PEG_TRACE_STRING(TRC_L10N, Tracer::LEVEL4,
                    "ACCEPTLANGUAGES LOOP: ICU warns using FALLBACK or "
                        "DEFAULT");
                if ((acceptlanguages.size() == 1) && (!parms.useICUfallback) &&
                    (status == U_USING_DEFAULT_WARNING))
                {
                    // in this case we want to return messages from the root
                    // bundle
                    status = U_ZERO_ERROR;
                    PEG_TRACE_STRING(TRC_L10N, Tracer::LEVEL4,
                        "acceptlang.size =1 && !parms.useICUfallback && "
                            "U_USING_DEFAULT_WARNING, using root bundle.");

                    // Reopen the bundle in the root locale
                    ures_close(parms._resbundl);
                    parms._resbundl = ures_open(
                        (const char*)resbundl_path_ICU, "", &status);
                    if (U_SUCCESS(status))
                    {
                        PEG_METHOD_EXIT();
                        return;
                    }
                }
                else if (acceptlanguages.size() == 1 || parms.useICUfallback)
                {
                    PEG_TRACE_STRING(TRC_L10N, Tracer::LEVEL4,
                        "ACCEPTLANGUAGES LOOP: acceptlanguages.size == 1 or "
                            "useICUfallback true, using ICU fallback "
                            "behaviour...");
                    const char* _locale =
                        ures_getLocale(parms._resbundl, &status);

#ifdef PEGASUS_OS_OS400
                    char tmplcl[size_locale_ICU];
                    strcpy(tmplcl, _locale);
                    EtoA(tmplcl);
                    String localeStr(tmplcl);
#else
                    String localeStr(_locale);
#endif

					// The "root" locale indicates that an ICU message bundle is not present
					// for the current locale setting.
                    if (localeStr != "root")
                    {
                        parms.contentlanguages.append(LanguageTag(
                            LanguageParser::convertLocaleIdToLanguageTag(
                                localeStr)));
                    }
                    PEG_METHOD_EXIT();
                    return;
                }
            }
            else
            {
                // we found an exact resource bundle match, extract, and set
                // ContentLanguage

                PEG_TRACE_STRING(TRC_L10N, Tracer::LEVEL4,
                    "ACCEPTLANGUAGES LOOP: found an EXACT resource bundle "
                        "MATCH:");
                PEG_TRACE_STRING(TRC_L10N, Tracer::LEVEL4,
                    languageTag.toString());
                parms.contentlanguages.append(
                    LanguageTag(languageTag.toString()));
                PEG_METHOD_EXIT();
                return;
            }
        }
        else
        {
            // possible errors, ex: message path incorrect
            // for now do nothing, let the while loop continue
        }
        status = U_ZERO_ERROR;  // reset status
    }

    // now if we DIDN'T open a resource bundle, we want to enable ICU
    // fallback for the highest priority language
    if (acceptlanguages.size() > 0)
    {
        PEG_TRACE_STRING(TRC_L10N, Tracer::LEVEL4,
            "No message was loaded, using ICU fallback behaviour.");
        languageTag = acceptlanguages.getLanguageTag(0);

#ifdef PEGASUS_OS_OS400
        CString cstr = languageTag.toString().getCString();
        const char *atoe = cstr;
        AtoE((char*)atoe);

        uloc_getName(atoe, locale_ICU, size_locale_ICU, &status);
#else
        uloc_getName(
            (const char*)(languageTag.toString()).getCString(),
            locale_ICU, size_locale_ICU, &status);
#endif
        status = U_ZERO_ERROR;
        parms._resbundl =
            ures_open((const char*)resbundl_path_ICU, locale_ICU, &status);
        const char* _locale = NULL;
        if (U_SUCCESS(status))
        {
            if (status == U_USING_DEFAULT_WARNING)
            {
                PEG_TRACE_STRING(TRC_L10N, Tracer::LEVEL4,
                    "PRIORITY ICU FALLBACK: using default resource bundle "
                    "with ");
                PEG_TRACE_STRING(TRC_L10N, Tracer::LEVEL4,
                     languageTag.toString());
                status = U_ZERO_ERROR;

                // Reopen the bundle in the root locale
                ures_close(parms._resbundl);
                parms._resbundl =
                    ures_open((const char*)resbundl_path_ICU, "", &status);
                if (U_SUCCESS(status))
                {
                    _locale = ures_getLocale(parms._resbundl,&status);
                }
            }
            else
            {
                _locale = ures_getLocale(parms._resbundl,&status);
            }

            String localeStr;

            if (_locale != NULL)
            {
#ifdef PEGASUS_OS_OS400
                char tmplcl[size_locale_ICU];
                strcpy(tmplcl, _locale);
                EtoA(tmplcl);
                localeStr.assign(tmplcl);
#else
                localeStr.assign(_locale);
#endif
            }

			// The "root" locale indicates that an ICU message bundle is not present
			// for the current locale setting.
            if (localeStr != "root")
            {
                parms.contentlanguages.append(LanguageTag(
                    LanguageParser::convertLocaleIdToLanguageTag(
                        localeStr)));
            }

            if (_locale != NULL)
            {
                PEG_METHOD_EXIT();
                return;
            }
        }
    }

    {
        // else if no message, load message from root bundle explicitly
        PEG_TRACE_STRING(TRC_L10N, Tracer::LEVEL4,
            "EXHAUSTED ACCEPTLANGUAGES: using root bundle to extract message");
        status = U_ZERO_ERROR;
        parms._resbundl =
            ures_open((const char*)resbundl_path_ICU, "", &status);
        if (U_SUCCESS(status))
        {
            PEG_TRACE_STRING(TRC_L10N, Tracer::LEVEL4,
                "EXHAUSTED ACCEPTLANGUAGES: opened root resource bundle");
        }
        else
        {
            PEG_TRACE_STRING(TRC_L10N, Tracer::LEVEL4,
                "EXHAUSTED ACCEPTLANGUAGES: could NOT open root resource "
                    "bundle");
            parms._resbundl = NO_ICU_MAGIC;
        }
    }

    PEG_METHOD_EXIT();
    return;
}

String MessageLoader::extractICUMessage(
    UResourceBundle* resbundl,
    MessageLoaderParms& parms)
{
    UErrorCode status = U_ZERO_ERROR;
    int32_t msgLen = 0;

#ifdef PEGASUS_OS_OS400
    CString cstr = parms.msg_id.getCString();
    const char* atoe = cstr;
    AtoE((char*)atoe);
    const UChar* msg =
        ures_getStringByKey(resbundl, (const char*)atoe, &msgLen, &status);
#else
    const UChar* msg = ures_getStringByKey(
        resbundl, (const char*)parms.msg_id.getCString(), &msgLen, &status);
#endif

    if (U_FAILURE(status))
    {
        return String::EMPTY;
    }

    return formatICUMessage(resbundl, msg, msgLen, parms);
}

String MessageLoader::uChar2String(UChar* uchar_str)
{
    return String((const Char16 *)uchar_str);
}

String MessageLoader::uChar2String(UChar* uchar_str, int len)
{
    return String((const Char16 *)uchar_str, len);
}

String MessageLoader::formatICUMessage(
    UResourceBundle* resbundl,
    const UChar* msg,
    int msg_len,
    MessageLoaderParms& parms)
{
    // format the message
    UnicodeString msg_pattern(msg, msg_len);
    UnicodeString msg_formatted;
    UErrorCode status = U_ZERO_ERROR;
    const int arg_count = 10;
    const char *locale;
    if (resbundl == NULL)
         locale = ULOC_US;
    else
         locale = ures_getLocale(resbundl, &status);

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

    return uChar2String(
        const_cast<UChar*>(msg_formatted.getBuffer()), msg_formatted.length());
}

void MessageLoader::xferFormattable(
    Formatter::Arg &arg,
    Formattable& formattable)
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
                sprintf(buffer, "%" PEGASUS_64BIT_CONVERSION_WIDTH "u",
                    arg._lUInteger);
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

String MessageLoader::formatDefaultMessage(MessageLoaderParms& parms)
{
    PEG_METHOD_ENTER(TRC_L10N, "MessageLoader::formatDefaultMessage");

    // NOTE TO PROGRAMMERS: using native substitution functions
    // ie. calling Formatter::format()
    // can result in incorrect locale handling of substitutions

    // locale INSENSITIVE formatting code

    // this could have previously been set by ICU
    parms.contentlanguages.clear();

    PEG_METHOD_EXIT();
    return Formatter::format(
        parms.default_msg,
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

String MessageLoader::getQualifiedMsgPath(String path)
{
    PEG_METHOD_ENTER(TRC_L10N, "MessageLoader::getQualifiedMsgPath");

    if (pegasus_MSG_HOME.size() == 0)
        initPegasusMsgHome(String::EMPTY);

    if (path.size() == 0)
    {
        PEG_METHOD_EXIT();
        return pegasus_MSG_HOME + server_resbundl_name;
    }

    // NOTE TO PROGRAMMERS: WINDOWS and non-UNIX platforms should
    // redefine delim here
    Char16 delim = '/';
    Uint32 i;
    if ((i = path.find(delim)) != PEG_NOT_FOUND && i == 0)
    {
        // fully qualified package name
        PEG_METHOD_EXIT();
        return path;
    }

    PEG_METHOD_EXIT();
    return pegasus_MSG_HOME + path;  // relative path and package name
}

void MessageLoader::setPegasusMsgHome(String home)
{
    PEG_METHOD_ENTER(TRC_L10N, "MessageLoader::setPegasusMsgHome");
    pegasus_MSG_HOME = home + "/";

    // TODO: remove the next call once test cases are compatible with ICU
    // messages
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
        if (PEG_NOT_FOUND == argv0.find('\\'))
        {
            char exeDir[_MAX_PATH];
            HMODULE module = GetModuleHandle(NULL);
            if (NULL != module )
            {
                DWORD filename =
                    GetModuleFileName(module,exeDir ,sizeof(exeDir));
                if (0 != filename)
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
        if (PEG_NOT_FOUND  != argv0.find('/'))
        {
            Uint32 command = argv0.reverseFind('/');
            startingDir = argv0.subString(0, command+1);
        }
        else
        {
            if (FileSystem::existsNoCase(argv0))
            {
                FileSystem::getCurrentDirectory(startingDir);
                startingDir.append("/");
            }
            else
            {
                String path;
# ifdef PEGASUS_PLATFORM_OS400_ISERIES_IBM
#  pragma convert(37)
                const char* env = getenv("PATH");
                if (env != NULL)
                    EtoA((char*)env);
#  pragma convert(0)
# else
                const char* env = getenv("PATH");
# endif
                if (env != NULL)
                    path.assign(env);
                String pathDelim = FileSystem::getPathDelimiter();
                Uint32 size = path.size();
                while (path.size() > 0)
                {
                    try
                    {
                        Uint32 delim = path.find(pathDelim);
                        if (delim != PEG_NOT_FOUND)
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
                        Boolean dirContent =
                            FileSystem::existsNoCase(commandPath);
                        if (dirContent)
                        {
                            startingDir = pathDir;
                            break;
                        }
                    }
                    catch (Exception& e)
                    {
                        // Have to do nothing.
                        // Catching the exception if there is any exception
                        // while searching in the path variable
                    }
                }
            }
        }
#endif
        initPegasusMsgHome(startingDir);
    }
    catch (Exception& e)
    {
        // Have to do nothing.
        // Catching the exception if there is any exception while searching
        // in the path variable
    }
#endif
}


void MessageLoader::initPegasusMsgHome(const String& startDir)
{
    String startingDir = startDir;
    if (startingDir.size() == 0)
    {
#ifdef PEGASUS_PLATFORM_OS400_ISERIES_IBM
# pragma convert(37)
        const char* env = getenv("PEGASUS_MSG_HOME");
        if (env != NULL)
            EtoA((char*)env);
# pragma convert(0)
#else
        const char* env = getenv("PEGASUS_MSG_HOME");
#endif

        if (env != NULL)
            startingDir.assign(env);
    }

#ifdef PEGASUS_DEFAULT_MESSAGE_SOURCE
    if (System::is_absolute_path(
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

void MessageLoader::checkDefaultMsgLoading()
{
    // Note: this function is a special hook for the automated tests
    // (poststarttests).  Since the automated tests expect the old hardcoded
    // default messages, an env var will be used to tell this code to ignore
    // ICU and return the default message.
    // This will allow poststarttests to run with ICU installed.
    // TODO: remove this function once test cases are compatible with ICU
    // messages
#ifdef PEGASUS_OS_OS400
# pragma convert(37)
#endif
    const char* env = getenv("PEGASUS_USE_DEFAULT_MESSAGES");
#ifdef PEGASUS_OS_OS400
# pragma convert(0)
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

    acceptlanguages = AcceptLanguageList();
    contentlanguages = ContentLanguageList();

    _resbundl = NO_ICU_MAGIC;
}

MessageLoaderParms::MessageLoaderParms(
    const String& id,
    const String& msg,
    const Formatter::Arg& arg0_,
    const Formatter::Arg& arg1_,
    const Formatter::Arg& arg2_,
    const Formatter::Arg& arg3_,
    const Formatter::Arg& arg4_,
    const Formatter::Arg& arg5_,
    const Formatter::Arg& arg6_,
    const Formatter::Arg& arg7_,
    const Formatter::Arg& arg8_,
    const Formatter::Arg& arg9_)
{
    msg_id = id;
    default_msg = msg;
    _init();
    arg0 = arg0_;
    arg1 = arg1_;
    arg2 = arg2_;
    arg3 = arg3_;
    arg4 = arg4_;
    arg5 = arg5_;
    arg6 = arg6_;
    arg7 = arg7_;
    arg8 = arg8_;
    arg9 = arg9_;
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
    const Formatter::Arg& arg0_)
{
    msg_id = id;
    default_msg = msg;
    _init();
    arg0 = arg0_;
}

MessageLoaderParms::MessageLoaderParms(
    const String& id,
    const String& msg,
    const Formatter::Arg& arg0_,
    const Formatter::Arg& arg1_)
{
    msg_id = id;
    default_msg = msg;
    _init();
    arg0 = arg0_;
    arg1 = arg1_;
}

MessageLoaderParms::MessageLoaderParms(
    const String& id,
    const String& msg,
    const Formatter::Arg& arg0_,
    const Formatter::Arg& arg1_,
    const Formatter::Arg& arg2_)
{
    msg_id = id;
    default_msg = msg;
    _init();
    arg0 = arg0_;
    arg1 = arg1_;
    arg2 = arg2_;
}

MessageLoaderParms::MessageLoaderParms(
    const String& id,
    const String& msg,
    const Formatter::Arg& arg0_,
    const Formatter::Arg& arg1_,
    const Formatter::Arg& arg2_,
    const Formatter::Arg& arg3_)
{
    msg_id = id;
    default_msg = msg;
    _init();
    arg0 = arg0_;
    arg1 = arg1_;
    arg2 = arg2_;
    arg3 = arg3_;
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
    const String& arg0_)
{
    msg_id = id;
    default_msg = msg;
    _init();
    arg0 = arg0_;
}

MessageLoaderParms::MessageLoaderParms(
    const char* id,
    const char* msg,
    const String& arg0_,
    const String& arg1_)
{
    msg_id = id;
    default_msg = msg;
    _init();
    arg0 = arg0_;
    arg1 = arg1_;
}

void MessageLoaderParms::_init()
{
    useProcessLocale = false;
    useThreadLocale = true;

#ifdef PEGASUS_HAS_ICU
    useICUfallback = false;
#endif

    acceptlanguages.clear();
    contentlanguages.clear();

    arg0 = Formatter::DEFAULT_ARG;
    arg1 = Formatter::DEFAULT_ARG;
    arg2 = Formatter::DEFAULT_ARG;
    arg3 = Formatter::DEFAULT_ARG;
    arg4 = Formatter::DEFAULT_ARG;
    arg5 = Formatter::DEFAULT_ARG;
    arg6 = Formatter::DEFAULT_ARG;
    arg7 = Formatter::DEFAULT_ARG;
    arg8 = Formatter::DEFAULT_ARG;
    arg9 = Formatter::DEFAULT_ARG;

    _resbundl = NO_ICU_MAGIC;
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
    s.append("acceptlanguages = " +
        LanguageParser::buildAcceptLanguageHeader(acceptlanguages) + "\n");
    s.append("contentlanguages = " +
        LanguageParser::buildContentLanguageHeader(contentlanguages) + "\n");

    s.append("useProcessLocale = " + processLoc + "\n");
    s.append("useThreadLocale = " + threadLoc + "\n");
#ifdef PEGASUS_HAS_ICU
    s.append("useICUfallback = " + ICUfallback + "\n");
#endif
    s.append(
        "arg0 = " + arg0.toString() + "\n" +
        "arg1 = " + arg1.toString() + "\n" +
        "arg2 = " + arg2.toString() + "\n" +
        "arg3 = " + arg3.toString() + "\n" +
        "arg4 = " + arg4.toString() + "\n" +
        "arg5 = " + arg5.toString() + "\n" +
        "arg6 = " + arg6.toString() + "\n" +
        "arg7 = " + arg7.toString() + "\n" +
        "arg8 = " + arg8.toString() + "\n" +
        "arg9 = " + arg9.toString() + "\n\n");

    return s;
}

MessageLoaderParms::~MessageLoaderParms()
{
}

PEGASUS_NAMESPACE_END
