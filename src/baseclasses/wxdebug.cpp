//------------------------------------------------------------------------------
// File: WXDebug.cpp
//
// Desc: DirectShow base classes - implements ActiveX system debugging
//       facilities.
//
// Copyright (c) 1992-2001 Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

#include "stdafx.h"

#ifdef DEBUG
#ifdef UNICODE
#ifndef _UNICODE
#define _UNICODE
#endif // _UNICODE
#endif // UNICODE
#endif // DEBUG

#include <tchar.h>

#ifdef DEBUG

// The Win32 wsprintf() function writes a maximum of 1024 characters to it's output buffer.
// See the documentation for wsprintf()'s lpOut parameter for more information.
const INT iDEBUGINFO = 1024;                 // Used to format strings

/* For every module and executable we store a debugging level for each of
   the five categories (eg LOG_ERROR and LOG_TIMING). This makes it easy
   to isolate and debug individual modules without seeing everybody elses
   spurious debug output. The keys are stored in the registry under the
   HKEY_LOCAL_MACHINE\SOFTWARE\Debug\<Module Name>\<KeyName> key values
   NOTE these must be in the same order as their enumeration definition */

TCHAR *pKeyNames[] = {
    TEXT("TIMING"),      // Timing and performance measurements
    TEXT("TRACE"),       // General step point call tracing
    TEXT("MEMORY"),      // Memory and object allocation/destruction
    TEXT("LOCKING"),     // Locking/unlocking of critical sections
    TEXT("ERROR"),       // Debug error notification
    TEXT("CUSTOM1"),
    TEXT("CUSTOM2"),
    TEXT("CUSTOM3"),
    TEXT("CUSTOM4"),
    TEXT("CUSTOM5")
    };

const TCHAR CAutoTrace::_szEntering[] = TEXT("Entering: %s");
const TCHAR CAutoTrace::_szLeaving[]  = TEXT("Leaving: %s");

const INT iMAXLEVELS = NUMELMS(pKeyNames);  // Maximum debug categories

HINSTANCE m_hInst;                          // Module instance handle
TCHAR m_ModuleName[iDEBUGINFO];             // Cut down module name
DWORD m_Levels[iMAXLEVELS];                 // Debug level per category
CRITICAL_SECTION m_CSDebug;                 // Controls access to list
DWORD m_dwNextCookie;                       // Next active object ID
ObjectDesc *pListHead = NULL;               // First active object
DWORD m_dwObjectCount;                      // Active object count
BOOL m_bInit = FALSE;                       // Have we been initialised
HANDLE m_hOutput = INVALID_HANDLE_VALUE;    // Optional output written here
DWORD dwWaitTimeout = INFINITE;             // Default timeout value
DWORD dwTimeOffset;                // Time of first DbgLog call
bool g_fUseKASSERT = false;                 // don't create messagebox
bool g_fDbgInDllEntryPoint = false;
bool g_fAutoRefreshLevels = false;

const TCHAR *pBaseKey = TEXT("SOFTWARE\\Debug");
const TCHAR *pGlobalKey = TEXT("GLOBAL");
static CHAR *pUnknownName = "UNKNOWN";

TCHAR *TimeoutName = TEXT("TIMEOUT");

/* This sets the instance handle that the debug library uses to find
   the module's file name from the Win32 GetModuleFileName function */

void WINAPI DbgInitialise(HINSTANCE hInst)
{
    InitializeCriticalSection(&m_CSDebug);
    m_bInit = TRUE;

    m_hInst = hInst;
    DbgInitModuleName();
    if (GetProfileInt(m_ModuleName, TEXT("BreakOnLoad"), 0))
       DebugBreak();
    DbgInitModuleSettings(false);
    DbgInitGlobalSettings(true);
    dwTimeOffset = timeGetTime();
}


/* This is called to clear up any resources the debug library uses - at the
   moment we delete our critical section and the object list. The values we
   retrieve from the registry are all done during initialisation but we don't
   go looking for update notifications while we are running, if the values
   are changed then the application has to be restarted to pick them up */

void WINAPI DbgTerminate()
{
    if (m_hOutput != INVALID_HANDLE_VALUE) {
       EXECUTE_ASSERT(CloseHandle(m_hOutput));
       m_hOutput = INVALID_HANDLE_VALUE;
    }
    DeleteCriticalSection(&m_CSDebug);
    m_bInit = FALSE;
}


/* This is called by DbgInitLogLevels to read the debug settings
   for each logging category for this module from the registry */

void WINAPI DbgInitKeyLevels(HKEY hKey, bool fTakeMax)
{
    LONG lReturn;               // Create key return value
    LONG lKeyPos;               // Current key category
    DWORD dwKeySize;            // Size of the key value
    DWORD dwKeyType;            // Receives it's type
    DWORD dwKeyValue;           // This fields value

    /* Try and read a value for each key position in turn */
    for (lKeyPos = 0;lKeyPos < iMAXLEVELS;lKeyPos++) {

        dwKeySize = sizeof(DWORD);
        lReturn = RegQueryValueEx(
            hKey,                       // Handle to an open key
            pKeyNames[lKeyPos],         // Subkey name derivation
            NULL,                       // Reserved field
            &dwKeyType,                 // Returns the field type
            (LPBYTE) &dwKeyValue,       // Returns the field's value
            &dwKeySize );               // Number of bytes transferred

        /* If either the key was not available or it was not a DWORD value
           then we ensure only the high priority debug logging is output
           but we try and update the field to a zero filled DWORD value */

        if (lReturn != ERROR_SUCCESS || dwKeyType != REG_DWORD)  {

            dwKeyValue = 0;
            lReturn = RegSetValueEx(
                hKey,                   // Handle of an open key
                pKeyNames[lKeyPos],     // Address of subkey name
                (DWORD) 0,              // Reserved field
                REG_DWORD,              // Type of the key field
                (PBYTE) &dwKeyValue,    // Value for the field
                sizeof(DWORD));         // Size of the field buffer

            if (lReturn != ERROR_SUCCESS) {
                DbgLog((LOG_ERROR,0,TEXT("Could not create subkey %s"),pKeyNames[lKeyPos]));
                dwKeyValue = 0;
            }
        }
        if(fTakeMax)
        {
            m_Levels[lKeyPos] = std::max(dwKeyValue,m_Levels[lKeyPos]); /*ffdshow custom*/
        }
        else
        {
            if((m_Levels[lKeyPos] & LOG_FORCIBLY_SET) == 0) {
                m_Levels[lKeyPos] = dwKeyValue;
            }
        }
    }

    /*  Read the timeout value for catching hangs */
    dwKeySize = sizeof(DWORD);
    lReturn = RegQueryValueEx(
        hKey,                       // Handle to an open key
        TimeoutName,                // Subkey name derivation
        NULL,                       // Reserved field
        &dwKeyType,                 // Returns the field type
        (LPBYTE) &dwWaitTimeout,    // Returns the field's value
        &dwKeySize );               // Number of bytes transferred

    /* If either the key was not available or it was not a DWORD value
       then we ensure only the high priority debug logging is output
       but we try and update the field to a zero filled DWORD value */

    if (lReturn != ERROR_SUCCESS || dwKeyType != REG_DWORD)  {

        dwWaitTimeout = INFINITE;
        lReturn = RegSetValueEx(
            hKey,                   // Handle of an open key
            TimeoutName,            // Address of subkey name
            (DWORD) 0,              // Reserved field
            REG_DWORD,              // Type of the key field
            (PBYTE) &dwWaitTimeout, // Value for the field
            sizeof(DWORD));         // Size of the field buffer

        if (lReturn != ERROR_SUCCESS) {
            DbgLog((LOG_ERROR,0,TEXT("Could not create subkey %s"),pKeyNames[lKeyPos]));
            dwWaitTimeout = INFINITE;
        }
    }
}

void WINAPI DbgOutString(LPCTSTR psz)
{
    if (m_hOutput != INVALID_HANDLE_VALUE) {
        UINT  cb = lstrlen(psz);
        DWORD dw;
#ifdef UNICODE
        CHAR szDest[2048];
        WideCharToMultiByte(CP_ACP, 0, psz, -1, szDest, NUMELMS(szDest), 0, 0);
        WriteFile (m_hOutput, szDest, cb, &dw, NULL);
#else
        WriteFile (m_hOutput, psz, cb, &dw, NULL);
#endif
    } else {
        OutputDebugString (psz);
    }
}

/* Called by DbgInitGlobalSettings to setup alternate logging destinations
 */

void WINAPI DbgInitLogTo (
    HKEY hKey)
{
    LONG  lReturn;
    DWORD dwKeyType;
    DWORD dwKeySize;
    TCHAR szFile[MAX_PATH] = {0};
    static const TCHAR cszKey[] = TEXT("LogToFile");

    dwKeySize = MAX_PATH;
    lReturn = RegQueryValueEx(
        hKey,                       // Handle to an open key
        cszKey,                     // Subkey name derivation
        NULL,                       // Reserved field
        &dwKeyType,                 // Returns the field type
        (LPBYTE) szFile,            // Returns the field's value
        &dwKeySize);                // Number of bytes transferred

    // create an empty key if it does not already exist
    //
    if (lReturn != ERROR_SUCCESS || dwKeyType != REG_SZ)
       {
       dwKeySize = sizeof(TCHAR);
       lReturn = RegSetValueEx(
            hKey,                   // Handle of an open key
            cszKey,                 // Address of subkey name
            (DWORD) 0,              // Reserved field
            REG_SZ,                 // Type of the key field
            (PBYTE)szFile,          // Value for the field
            dwKeySize);            // Size of the field buffer
       }

    // if an output-to was specified.  try to open it.
    //
    if (m_hOutput != INVALID_HANDLE_VALUE) {
       EXECUTE_ASSERT(CloseHandle (m_hOutput));
       m_hOutput = INVALID_HANDLE_VALUE;
    }
    if (szFile[0] != 0)
       {
       if (!lstrcmpi(szFile, TEXT("Console"))) {
          m_hOutput = GetStdHandle (STD_OUTPUT_HANDLE);
          if (m_hOutput == INVALID_HANDLE_VALUE) {
             AllocConsole ();
             m_hOutput = GetStdHandle (STD_OUTPUT_HANDLE);
          }
          SetConsoleTitle (TEXT("ActiveX Debug Output"));
       } else if (szFile[0] &&
                lstrcmpi(szFile, TEXT("Debug")) &&
                lstrcmpi(szFile, TEXT("Debugger")) &&
                lstrcmpi(szFile, TEXT("Deb")))
          {
            m_hOutput = CreateFile(szFile, GENERIC_WRITE,
                                 FILE_SHARE_READ,
                                 NULL, OPEN_ALWAYS,
                                 FILE_ATTRIBUTE_NORMAL,
                                 NULL);
          if (INVALID_HANDLE_VALUE != m_hOutput)
              {
              static const TCHAR cszBar[] = TEXT("\r\n\r\n=====DbgInitialize()=====\r\n\r\n");
              SetFilePointer (m_hOutput, 0, NULL, FILE_END);
              DbgOutString (cszBar);
              }
          }
       }
}



/* This is called by DbgInitLogLevels to read the global debug settings for
   each logging category for this module from the registry. Normally each
   module has it's own values set for it's different debug categories but
   setting the global SOFTWARE\Debug\Global applies them to ALL modules */

void WINAPI DbgInitGlobalSettings(bool fTakeMax)
{
    LONG lReturn;               // Create key return value
    TCHAR szInfo[iDEBUGINFO];   // Constructs key names
    HKEY hGlobalKey;            // Global override key

    /* Construct the global base key name */
    wsprintf(szInfo,TEXT("%s\\%s"),pBaseKey,pGlobalKey);

    /* Create or open the key for this module */
    lReturn = RegCreateKeyEx(HKEY_LOCAL_MACHINE,   // Handle of an open key
                             szInfo,               // Address of subkey name
                             (DWORD) 0,            // Reserved value
                             NULL,                 // Address of class name
                             (DWORD) 0,            // Special options flags
                             KEY_ALL_ACCESS,       // Desired security access
                             NULL,                 // Key security descriptor
                             &hGlobalKey,          // Opened handle buffer
                             NULL);                // What really happened

    if (lReturn != ERROR_SUCCESS) {
        DbgLog((LOG_ERROR,0,TEXT("Could not access GLOBAL module key")));
        return;
    }

    DbgInitKeyLevels(hGlobalKey, fTakeMax);
    RegCloseKey(hGlobalKey);
}


/* This sets the debugging log levels for the different categories. We start
   by opening (or creating if not already available) the SOFTWARE\Debug key
   that all these settings live under. We then look at the global values
   set under SOFTWARE\Debug\Global which apply on top of the individual
   module settings. We then load the individual module registry settings */

void WINAPI DbgInitModuleSettings(bool fTakeMax)
{
    LONG lReturn;               // Create key return value
    TCHAR szInfo[iDEBUGINFO];   // Constructs key names
    HKEY hModuleKey;            // Module key handle

    /* Construct the base key name */
    wsprintf(szInfo,TEXT("%s\\%s"),pBaseKey,m_ModuleName);

    /* Create or open the key for this module */
    lReturn = RegCreateKeyEx(HKEY_LOCAL_MACHINE,   // Handle of an open key
                             szInfo,               // Address of subkey name
                             (DWORD) 0,            // Reserved value
                             NULL,                 // Address of class name
                             (DWORD) 0,            // Special options flags
                             KEY_ALL_ACCESS,       // Desired security access
                             NULL,                 // Key security descriptor
                             &hModuleKey,          // Opened handle buffer
                             NULL);                // What really happened

    if (lReturn != ERROR_SUCCESS) {
        DbgLog((LOG_ERROR,0,TEXT("Could not access module key")));
        return;
    }

    DbgInitLogTo(hModuleKey);
    DbgInitKeyLevels(hModuleKey, fTakeMax);
    RegCloseKey(hModuleKey);
}


/* Initialise the module file name */

void WINAPI DbgInitModuleName()
{
    TCHAR FullName[iDEBUGINFO];     // Load the full path and module name
    TCHAR *pName;                   // Searches from the end for a backslash

    GetModuleFileName(m_hInst,FullName,iDEBUGINFO);
    pName = _tcsrchr(FullName,'\\');
    if (pName == NULL) {
        pName = FullName;
    } else {
        pName++;
    }
    lstrcpy(m_ModuleName,pName);
}

struct MsgBoxMsg
{
    HWND hwnd;
    TCHAR *szTitle;
    TCHAR *szMessage;
    DWORD dwFlags;
    INT iResult;
};

//
// create a thread to call MessageBox(). calling MessageBox() on
// random threads at bad times can confuse the host (eg IE).
//
unsigned int WINAPI MsgBoxThread(
  LPVOID lpParameter   // thread data
  )
{
    MsgBoxMsg *pmsg = (MsgBoxMsg *)lpParameter;
    pmsg->iResult = MessageBox(
        pmsg->hwnd,
        pmsg->szTitle,
        pmsg->szMessage,
        pmsg->dwFlags);

    return 0;
}

INT MessageBoxOtherThread(
    HWND hwnd,
    TCHAR *szTitle,
    TCHAR *szMessage,
    DWORD dwFlags)
{
    if(g_fDbgInDllEntryPoint)
    {
        // can't wait on another thread because we have the loader
        // lock held in the dll entry point.
        // This can crash sometimes so just skip it
        // return MessageBox(hwnd, szTitle, szMessage, dwFlags);
        return IDCANCEL;
    }
    else
    {
        MsgBoxMsg msg = {hwnd, szTitle, szMessage, dwFlags, 0};

        HANDLE hThread = (HANDLE)_beginthreadex( NULL,              /* Security */
                                                0,                  /* Stack Size */
                                                MsgBoxThread,       /* Thread process */
                                                (LPVOID)&msg,       /* Arguments */
                                                0,                  /* 0 = Start Immediately */
                                                NULL                /* Thread Address */
                                                );

        if(hThread)
        {
            WaitForSingleObject(hThread, INFINITE);
            CloseHandle(hThread);
            return msg.iResult;
        }

        // break into debugger on failure.
        return IDCANCEL;
    }
}

/* Displays a message box if the condition evaluated to FALSE */

void WINAPI DbgAssert(const TCHAR *pCondition,const TCHAR *pFileName,INT iLine)
{
    if(g_fUseKASSERT)
    {
        DbgKernelAssert(pCondition, pFileName, iLine);
    }
    else
    {

        TCHAR szInfo[iDEBUGINFO];

        wsprintf(szInfo, TEXT("%s \nAt line %d of %s\nContinue? (Cancel to debug)"),
                 pCondition, iLine, pFileName);

        INT MsgId = MessageBoxOtherThread(NULL,szInfo,TEXT("ASSERT Failed"),
                                          MB_SYSTEMMODAL |
                                          MB_ICONHAND |
                                          MB_YESNOCANCEL |
                                          MB_SETFOREGROUND);
        switch (MsgId)
        {
          case IDNO:              /* Kill the application */

              FatalAppExit(FALSE, TEXT("Application terminated"));
              break;

          case IDCANCEL:          /* Break into the debugger */

              DebugBreak();
              break;

          case IDYES:             /* Ignore assertion continue execution */
              break;
        }
    }
}

/* Displays a message box at a break point */

void WINAPI DbgBreakPoint(const TCHAR *pCondition,const TCHAR *pFileName,INT iLine)
{
    if(g_fUseKASSERT)
    {
        DbgKernelAssert(pCondition, pFileName, iLine);
    }
    else
    {
        TCHAR szInfo[iDEBUGINFO];

        wsprintf(szInfo, TEXT("%s \nAt line %d of %s\nContinue? (Cancel to debug)"),
                 pCondition, iLine, pFileName);

        INT MsgId = MessageBoxOtherThread(NULL,szInfo,TEXT("Hard coded break point"),
                                          MB_SYSTEMMODAL |
                                          MB_ICONHAND |
                                          MB_YESNOCANCEL |
                                          MB_SETFOREGROUND);
        switch (MsgId)
        {
          case IDNO:              /* Kill the application */

              FatalAppExit(FALSE, TEXT("Application terminated"));
              break;

          case IDCANCEL:          /* Break into the debugger */

              DebugBreak();
              break;

          case IDYES:             /* Ignore break point continue execution */
              break;
        }
    }
}

void WINAPI DbgBreakPoint(const TCHAR *pFileName,INT iLine,const TCHAR* szFormatString,...)
{
    // A debug break point message can have at most 2000 characters if
    // ANSI or UNICODE characters are being used.  A debug break point message
    // can have between 1000 and 2000 double byte characters in it.  If a
    // particular message needs more characters, then the value of this constant
    // should be increased.
    const DWORD MAX_BREAK_POINT_MESSAGE_SIZE = 2000;

    TCHAR szBreakPointMessage[MAX_BREAK_POINT_MESSAGE_SIZE];

    const DWORD MAX_CHARS_IN_BREAK_POINT_MESSAGE = sizeof(szBreakPointMessage) / sizeof(TCHAR);

    va_list va;
    va_start( va, szFormatString );

    int nReturnValue = _vsntprintf( szBreakPointMessage, MAX_CHARS_IN_BREAK_POINT_MESSAGE, szFormatString, va );

    va_end(va);

    // _vsnprintf() returns -1 if an error occurs.
    if( -1 == nReturnValue ) {
        DbgBreak( "ERROR in DbgBreakPoint().  The variable length debug message could not be displayed because _vsnprintf() failed." );
        return;
    }

    ::DbgBreakPoint( szBreakPointMessage, pFileName, iLine );
}


/* When we initialised the library we stored in the m_Levels array the current
   debug output level for this module for each of the five categories. When
   some debug logging is sent to us it can be sent with a combination of the
   categories (if it is applicable to many for example) in which case we map
   the type's categories into their current debug levels and see if any of
   them can be accepted. The function looks at each bit position in turn from
   the input type field and then compares it's debug level with the modules.

   A level of 0 means that output is always sent to the debugger.  This is
   due to producing output if the input level is <= m_Levels.
*/


BOOL WINAPI DbgCheckModuleLevel(DWORD Type,DWORD Level)
{
    if(g_fAutoRefreshLevels)
    {
        // re-read the registry every second. We cannot use RegNotify() to
        // notice registry changes because it's not available on win9x.
        static DWORD g_dwLastRefresh = 0; /*ffdshow custom*/
        DWORD dwTime = timeGetTime();
        if(dwTime - g_dwLastRefresh > 1000) {
            g_dwLastRefresh = dwTime;

            // there's a race condition: multiple threads could update the
            // values. plus read and write not synchronized. no harm
            // though.
            DbgInitModuleSettings(false);
        }
    }


    DWORD Mask = 0x01;

    // If no valid bits are set return FALSE
    if ((Type & ((1<<iMAXLEVELS)-1))) {

    // speed up unconditional output.
    if (0==Level)
        return(TRUE);

        for (LONG lKeyPos = 0;lKeyPos < iMAXLEVELS;lKeyPos++) {
            if (Type & Mask) {
                if (Level <= (m_Levels[lKeyPos] & ~LOG_FORCIBLY_SET)) {
                    return TRUE;
                }
            }
            Mask <<= 1;
        }
    }
    return FALSE;
}


/* Set debug levels to a given value */

void WINAPI DbgSetModuleLevel(DWORD Type, DWORD Level)
{
    DWORD Mask = 0x01;

    for (LONG lKeyPos = 0;lKeyPos < iMAXLEVELS;lKeyPos++) {
        if (Type & Mask) {
            m_Levels[lKeyPos] = Level | LOG_FORCIBLY_SET;
        }
        Mask <<= 1;
    }
}

/* whether to check registry values periodically. this isn't turned
   automatically because of the potential performance hit. */
void WINAPI DbgSetAutoRefreshLevels(bool fAuto)
{
    g_fAutoRefreshLevels = fAuto;
}

#ifdef UNICODE
//
// warning -- this function is implemented twice for ansi applications
// linking to the unicode library
//
void WINAPI DbgLogInfo(DWORD Type,DWORD Level,const CHAR *pFormat,...)
{
    /* Check the current level for this type combination */

    BOOL bAccept = DbgCheckModuleLevel(Type,Level);
    if (bAccept == FALSE) {
        return;
    }

    TCHAR szInfo[2000];

    /* Format the variable length parameter list */

    va_list va;
    va_start(va, pFormat);

    lstrcpy(szInfo,m_ModuleName);
    wsprintf(szInfo + lstrlen(szInfo),
             TEXT("(tid %x) %8d : "),
             GetCurrentThreadId(), timeGetTime() - dwTimeOffset);

    CHAR szInfoA[2000];
    WideCharToMultiByte(CP_ACP, 0, szInfo, -1, szInfoA, NUMELMS(szInfoA), 0, 0);

    wvsprintfA(szInfoA + lstrlenA(szInfoA), pFormat, va);
    lstrcatA(szInfoA, "\r\n");

    WCHAR wszOutString[2000];
    MultiByteToWideChar(CP_ACP, 0, szInfoA, -1, wszOutString, NUMELMS(wszOutString));
    DbgOutString(wszOutString);

    va_end(va);
}

void WINAPI DbgAssert(const CHAR *pCondition,const CHAR *pFileName,INT iLine)
{
    if(g_fUseKASSERT)
    {
        DbgKernelAssert(pCondition, pFileName, iLine);
    }
    else
    {

        TCHAR szInfo[iDEBUGINFO];

        wsprintf(szInfo, TEXT("%hs \nAt line %d of %hs\nContinue? (Cancel to debug)"),
                 pCondition, iLine, pFileName);

        INT MsgId = MessageBoxOtherThread(NULL,szInfo,TEXT("ASSERT Failed"),
                                          MB_SYSTEMMODAL |
                                          MB_ICONHAND |
                                          MB_YESNOCANCEL |
                                          MB_SETFOREGROUND);
        switch (MsgId)
        {
          case IDNO:              /* Kill the application */

              FatalAppExit(FALSE, TEXT("Application terminated"));
              break;

          case IDCANCEL:          /* Break into the debugger */

              DebugBreak();
              break;

          case IDYES:             /* Ignore assertion continue execution */
              break;
        }
    }
}

/* Displays a message box at a break point */

void WINAPI DbgBreakPoint(const CHAR *pCondition,const CHAR *pFileName,INT iLine)
{
    if(g_fUseKASSERT)
    {
        DbgKernelAssert(pCondition, pFileName, iLine);
    }
    else
    {
        TCHAR szInfo[iDEBUGINFO];

        wsprintf(szInfo, TEXT("%hs \nAt line %d of %hs\nContinue? (Cancel to debug)"),
                 pCondition, iLine, pFileName);

        INT MsgId = MessageBoxOtherThread(NULL,szInfo,TEXT("Hard coded break point"),
                                          MB_SYSTEMMODAL |
                                          MB_ICONHAND |
                                          MB_YESNOCANCEL |
                                          MB_SETFOREGROUND);
        switch (MsgId)
        {
          case IDNO:              /* Kill the application */

              FatalAppExit(FALSE, TEXT("Application terminated"));
              break;

          case IDCANCEL:          /* Break into the debugger */

              DebugBreak();
              break;

          case IDYES:             /* Ignore break point continue execution */
              break;
        }
    }
}

void WINAPI DbgKernelAssert(const CHAR *pCondition,const CHAR *pFileName,INT iLine)
{
    DbgLog((LOG_ERROR,0,TEXT("Assertion FAILED (%hs) at line %d in file %hs"),
           pCondition, iLine, pFileName));
    DebugBreak();
}

#endif

/* Print a formatted string to the debugger prefixed with this module's name
   Because the COMBASE classes are linked statically every module loaded will
   have their own copy of this code. It therefore helps if the module name is
   included on the output so that the offending code can be easily found */

//
// warning -- this function is implemented twice for ansi applications
// linking to the unicode library
//
void WINAPI DbgLogInfo(DWORD Type,DWORD Level,const TCHAR *pFormat,...)
{

    /* Check the current level for this type combination */

    BOOL bAccept = DbgCheckModuleLevel(Type,Level);
    if (bAccept == FALSE) {
        return;
    }

    TCHAR szInfo[2000];

    /* Format the variable length parameter list */

    va_list va;
    va_start(va, pFormat);

    lstrcpy(szInfo,m_ModuleName);
    wsprintf(szInfo + lstrlen(szInfo),
             TEXT("(tid %x) %8d : "),
             GetCurrentThreadId(), timeGetTime() - dwTimeOffset);

    _vstprintf(szInfo + lstrlen(szInfo), pFormat, va);
    lstrcat(szInfo, TEXT("\r\n"));
    DbgOutString(szInfo);

    va_end(va);
}


/* If we are executing as a pure kernel filter we cannot display message
   boxes to the user, this provides an alternative which puts the error
   condition on the debugger output with a suitable eye catching message */

void WINAPI DbgKernelAssert(const TCHAR *pCondition,const TCHAR *pFileName,INT iLine)
{
    DbgLog((LOG_ERROR,0,TEXT("Assertion FAILED (%s) at line %d in file %s"),
           pCondition, iLine, pFileName));
    DebugBreak();
}



/* Each time we create an object derived from CBaseObject the constructor will
   call us to register the creation of the new object. We are passed a string
   description which we store away. We return a cookie that the constructor
   uses to identify the object when it is destroyed later on. We update the
   total number of active objects in the DLL mainly for debugging purposes */

DWORD WINAPI DbgRegisterObjectCreation(const CHAR *szObjectName,
                                       const WCHAR *wszObjectName)
{
    /* If this fires you have a mixed DEBUG/RETAIL build */

    ASSERT(!!szObjectName ^ !!wszObjectName);

    /* Create a place holder for this object description */

    ObjectDesc *pObject = new ObjectDesc;
    ASSERT(pObject);

    /* It is valid to pass a NULL object name */
    if (pObject == NULL) {
        return FALSE;
    }

    /* Check we have been initialised - we may not be initialised when we are
       being pulled in from an executable which has globally defined objects
       as they are created by the C++ run time before WinMain is called */

    if (m_bInit == FALSE) {
        DbgInitialise(GetModuleHandle(NULL));
    }

    /* Grab the list critical section */
    EnterCriticalSection(&m_CSDebug);

    /* If no name then default to UNKNOWN */
    if (!szObjectName && !wszObjectName) {
        szObjectName = pUnknownName;
    }

    /* Put the new description at the head of the list */

    pObject->m_szName = szObjectName;
    pObject->m_wszName = wszObjectName;
    pObject->m_dwCookie = ++m_dwNextCookie;
    pObject->m_pNext = pListHead;

    pListHead = pObject;
    m_dwObjectCount++;

    DWORD ObjectCookie = pObject->m_dwCookie;
    ASSERT(ObjectCookie);

    if(wszObjectName) {
        DbgLog((LOG_MEMORY,2,TEXT("Object created   %d (%ls) %d Active"),
                pObject->m_dwCookie, wszObjectName, m_dwObjectCount));
    } else {
        DbgLog((LOG_MEMORY,2,TEXT("Object created   %d (%hs) %d Active"),
                pObject->m_dwCookie, szObjectName, m_dwObjectCount));
    }

    LeaveCriticalSection(&m_CSDebug);
    return ObjectCookie;
}


/* This is called by the CBaseObject destructor when an object is about to be
   destroyed, we are passed the cookie we returned during construction that
   identifies this object. We scan the object list for a matching cookie and
   remove the object if successful. We also update the active object count */

BOOL WINAPI DbgRegisterObjectDestruction(DWORD dwCookie)
{
    /* Grab the list critical section */
    EnterCriticalSection(&m_CSDebug);

    ObjectDesc *pObject = pListHead;
    ObjectDesc *pPrevious = NULL;

    /* Scan the object list looking for a cookie match */

    while (pObject) {
        if (pObject->m_dwCookie == dwCookie) {
            break;
        }
        pPrevious = pObject;
        pObject = pObject->m_pNext;
    }

    if (pObject == NULL) {
        DbgBreak("Apparently destroying a bogus object");
        LeaveCriticalSection(&m_CSDebug);
        return FALSE;
    }

    /* Is the object at the head of the list */

    if (pPrevious == NULL) {
        pListHead = pObject->m_pNext;
    } else {
        pPrevious->m_pNext = pObject->m_pNext;
    }

    /* Delete the object and update the housekeeping information */

    m_dwObjectCount--;

    if(pObject->m_wszName) {
        DbgLog((LOG_MEMORY,2,TEXT("Object destroyed %d (%ls) %d Active"),
                pObject->m_dwCookie, pObject->m_wszName, m_dwObjectCount));
    } else {
        DbgLog((LOG_MEMORY,2,TEXT("Object destroyed %d (%hs) %d Active"),
                pObject->m_dwCookie, pObject->m_szName, m_dwObjectCount));
    }

    delete pObject;
    LeaveCriticalSection(&m_CSDebug);
    return TRUE;
}


/* This runs through the active object list displaying their details */

void WINAPI DbgDumpObjectRegister()
{
    TCHAR szInfo[iDEBUGINFO];

    /* Grab the list critical section */

    EnterCriticalSection(&m_CSDebug);
    ObjectDesc *pObject = pListHead;

    /* Scan the object list displaying the name and cookie */

    DbgLog((LOG_MEMORY,2,TEXT("")));
    DbgLog((LOG_MEMORY,2,TEXT("   ID             Object Description")));
    DbgLog((LOG_MEMORY,2,TEXT("")));

    while (pObject) {
        if(pObject->m_wszName) {
            wsprintf(szInfo,TEXT("%5d (%8x) %30ls"),pObject->m_dwCookie, &pObject, pObject->m_wszName);
        } else {
            wsprintf(szInfo,TEXT("%5d (%8x) %30hs"),pObject->m_dwCookie, &pObject, pObject->m_szName);
        }
        DbgLog((LOG_MEMORY,2,szInfo));
        pObject = pObject->m_pNext;
    }

    wsprintf(szInfo,TEXT("Total object count %5d"),m_dwObjectCount);
    DbgLog((LOG_MEMORY,2,TEXT("")));
    DbgLog((LOG_MEMORY,1,szInfo));
    LeaveCriticalSection(&m_CSDebug);
}

/*  Debug infinite wait stuff */
DWORD WINAPI DbgWaitForSingleObject(HANDLE h)
{
    DWORD dwWaitResult;
    do {
        dwWaitResult = WaitForSingleObject(h, dwWaitTimeout);
        ASSERT(dwWaitResult == WAIT_OBJECT_0);
    } while (dwWaitResult == WAIT_TIMEOUT);
    return dwWaitResult;
}
DWORD WINAPI DbgWaitForMultipleObjects(DWORD nCount,
                                CONST HANDLE *lpHandles,
                                BOOL bWaitAll)
{
    DWORD dwWaitResult;
    do {
        dwWaitResult = WaitForMultipleObjects(nCount,
                                              lpHandles,
                                              bWaitAll,
                                              dwWaitTimeout);
        ASSERT((DWORD)(dwWaitResult - WAIT_OBJECT_0) < MAXIMUM_WAIT_OBJECTS);
    } while (dwWaitResult == WAIT_TIMEOUT);
    return dwWaitResult;
}

void WINAPI DbgSetWaitTimeout(DWORD dwTimeout)
{
    dwWaitTimeout = dwTimeout;
}

#endif /* DEBUG */

/*  CDisp class - display our data types */

