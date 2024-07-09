/*
 * WimImage plugin for NSIS 
 *
 * By Jason Ross aka JasonFriday13 on the forums.
 * Copyright 2014 MouseHelmet Software
 *
 * main.c
 * Contains the NSIS related calls and main entry point.
 */

#include <windows.h>
#include <shlwapi.h>
#include "nsis\pluginapi.h" /* This means NSIS 2.42 or higher is required. */
#include "wimfuncs.h"
#include "errorcodes.h"

/*  Our defines. */
#define swSourceDir L"/S"
#define swSourceFile L"/S"
#define swOutFile L"/O"
#define swOutDir L"/O"
#define swTempDir L"/T"
#define swCompressionType L"/C"
#define swSplitSize L"/SS"
#define swReserve L"/R"
#define swImageIndex L"/I"
#define swReadSequential L"/RS"
#define swVerify L"/V"
#define swNextDiscText L"/M"

#define swCD650 L"CD650"
#define swCD700 L"CD700"
#define swDVD4GB L"DVD4GB"
#define swDVD5 L"DVD5"
#define swDVD9 L"DVD9"

HINSTANCE hInstance;
BOOL Initialized = FALSE;

/*  Our plugin callback so that our dll stays loaded. */
UINT_PTR __cdecl NSISPluginCallback(enum NSPIM Event) 
{
  return 0;
}

void Initialize(extra_parameters* xp)
{
  if (!Initialized)
  {
    xp->RegisterPluginCallback(hInstance, NSISPluginCallback);
    Initialized = TRUE;
  }
}

/*	This routine gets the data from the installer and applies it. */
DWORD GetWriteParams(PWIMINFO pData, PWSTR pBuf)
{
  DWORD RetVal = 0;

  /*  Pop, pop, pop it off the stack. */
  if (!PopStringW(pBuf))
    if (lstrcmpiW(pBuf, swOutFile) == 0)
      if (!PopStringW(pBuf))
        if (lstrlenW(pBuf) < MAX_PATH)
          wcscpy(pData->OutFile, pBuf);
        else
          return ERR_STR_TOOLONG;
      else
        return ERR_POPSTACK_FAIL;
    else
      return ERR_INVALID_SWITCH;
  else
    return ERR_POPSTACK_FAIL;
  /*  Pop, pop, pop it off the stack. */
  if (!PopStringW(pBuf))
    if (lstrcmpiW(pBuf, swSourceDir) == 0)
      if (!PopStringW(pBuf))
        if (lstrlenW(pBuf) < MAX_PATH)
          if (PathIsDirectoryW(pBuf))
            wcscpy(pData->SrcDir, pBuf);
          else
            return ERR_INVALID_SRCDIR;
        else
          return ERR_STR_TOOLONG;
      else
        return ERR_POPSTACK_FAIL;
    else
      return ERR_INVALID_SWITCH;
  else
    return ERR_POPSTACK_FAIL;
  /*  Pop, pop, pop it off the stack. */
  if (!PopStringW(pBuf))
    if (lstrcmpiW(pBuf, swTempDir) == 0)
      if (!PopStringW(pBuf))
        if (lstrlenW(pBuf) < MAX_PATH)
          if (PathIsDirectoryW(pBuf))
            wcscpy(pData->TempDir, pBuf);
          else
            return ERR_INVALID_TEMPDIR;
        else
          return ERR_STR_TOOLONG;
      else
        return ERR_POPSTACK_FAIL;
    else
    {
      PushStringW(pBuf);
      RetVal = GetTempPathW(MAX_PATH, pData->TempDir);
      if (RetVal > MAX_PATH || (RetVal == 0))
        return ERR_GETTEMPPATH_FAIL;
      else
        if (!PathIsDirectoryW(pData->TempDir))
          return ERR_INVALID_TEMPDIR;
    }
  else
  {
    RetVal = GetTempPathW(MAX_PATH, pData->TempDir);
    if (!RetVal || RetVal > MAX_PATH)
      return ERR_GETTEMPPATH_FAIL;
    else
      if (!PathIsDirectoryW(pData->TempDir))
        return ERR_INVALID_TEMPDIR;
  }
  /*  Pop, pop, pop it off the stack. */
  if (!PopStringW(pBuf))
    if (lstrcmpiW(pBuf, swCompressionType) == 0)
      if (!PopStringW(pBuf))
      {
        DWORD dwCompress = (DWORD)myatou(pBuf);
        if (dwCompress <= 2)
          pData->Compress = dwCompress;
        else
          return ERR_INVALID_COMPRESSION;
      }
      else
        return ERR_POPSTACK_FAIL;
    else
      PushStringW(pBuf), pData->Compress = 0;
  else
    pData->Compress = 0;
  /*  Pop, pop, pop it off the stack. */
  if (!PopStringW(pBuf))
    if (lstrcmpiW(pBuf, swVerify) == 0)
      pData->Verify = TRUE;
    else
      PushStringW(pBuf), pData->Verify = FALSE;
  else
    pData->Verify = FALSE;

  return ERR_SUCCESS;
}

/*	This routine gets the data from the installer and applies it. */
DWORD GetSplitParams(PWIMINFO pData, PWSTR pBuf)
{
  /*  Pop, pop, pop it off the stack. */
  if (!PopStringW(pBuf))
    if (lstrcmpiW(pBuf, swOutFile) == 0)
      if (!PopStringW(pBuf))
        if (lstrlenW(pBuf) < MAX_PATH)
          wcscpy(pData->OutFile, pBuf);
        else
          return ERR_STR_TOOLONG;
      else
        return ERR_POPSTACK_FAIL;
    else
      return ERR_INVALID_SWITCH;
  else
    return ERR_POPSTACK_FAIL;
  /*  Pop, pop, pop it off the stack. */
  if (!PopStringW(pBuf))
    if (lstrcmpiW(pBuf, swSourceFile) == 0)
      if (!PopStringW(pBuf))
        if (lstrlenW(pBuf) < MAX_PATH)
          if (PathFileExistsW(pBuf))
            wcscpy(pData->SrcFile, pBuf);
          else
            return ERR_INVALID_SRCFILE;
        else
          return ERR_STR_TOOLONG;
      else
        return ERR_POPSTACK_FAIL;
    else
      return ERR_INVALID_SWITCH;
  else
    return ERR_POPSTACK_FAIL;
  /*  Pop, pop, pop it off the stack. */
  if (!PopStringW(pBuf))
    if (lstrcmpiW(pBuf, swSplitSize) == 0)
      if (!PopStringW(pBuf))
        if (lstrcmpiW(pBuf, swCD650) == 0)
          pData->ChunkSize.QuadPart = 681574400; /* 650 * 1024 * 1024 */
        else
          if (lstrcmpiW(pBuf, swCD700) == 0)
            pData->ChunkSize.QuadPart = 734003200; /* 700 * 1024 * 1024 */
          else
            if (lstrcmpiW(pBuf, swDVD4GB) == 0)
              pData->ChunkSize.QuadPart = 4194304000; /* 4000 * 1024 * 1024 */
            else
              if (lstrcmpiW(pBuf, swDVD5) == 0)
                pData->ChunkSize.QuadPart = 4697620480; /* 4480 * 1024 * 1024 */
              else
                if (lstrcmpiW(pBuf, swDVD9) == 0)
                  pData->ChunkSize.QuadPart = 8540651520; /* 8145 * 1024 * 1024 */
                else
                  return ERR_INVALID_SPLITSIZE;
      else
        return ERR_POPSTACK_FAIL;
    else
      return ERR_INVALID_SWITCH;
  else
    return ERR_POPSTACK_FAIL;
  /*  Pop, pop, pop it off the stack. */
  if (!PopStringW(pBuf))
    if (lstrcmpiW(pBuf, swReserve) == 0)
      if (!PopStringW(pBuf))
      {
        DWORD rsize = (DWORD)myatou(pBuf);
        /* Check for a reserve size bigger than the chunk size. */
        if (rsize > pData->ChunkSize.QuadPart)
          return ERR_INVALID_RESERVESIZE;
        else
          /* Check for ample space for the first split. 100MB is a fair size. */
          if (rsize > pData->ChunkSize.QuadPart - 104857600)
            return ERR_INVALID_RESERVESIZE;
          else
            if (rsize < 1048576) rsize = 1048576;
        pData->FirstChunkSize.QuadPart = pData->ChunkSize.QuadPart - rsize;
      }
      else
        return ERR_POPSTACK_FAIL;
    else
      PushStringW(pBuf), pData->FirstChunkSize.QuadPart = pData->ChunkSize.QuadPart - 1048576;
  else
    pData->FirstChunkSize.QuadPart = pData->ChunkSize.QuadPart - 1048576;
  /*  Pop, pop, pop it off the stack. */
  if (!PopStringW(pBuf))
    if (lstrcmpiW(pBuf, swVerify) == 0)
      pData->Verify = TRUE;
    else
      PushStringW(pBuf), pData->Verify = FALSE;
  else
    pData->Verify = FALSE;

  return ERR_SUCCESS;
}

/*	This routine gets the data from the installer and copies it. */
DWORD GetCopyParams(PWIMINFO pData, PWSTR pBuf)
{
  /*  Pop, pop, pop it off the stack. */
  if (!PopStringW(pBuf))
    if (lstrcmpiW(pBuf, swOutFile) == 0)
      if (!PopStringW(pBuf))
        if (lstrlenW(pBuf) < MAX_PATH)
          wcscpy(pData->OutFile, pBuf);
        else
          return ERR_STR_TOOLONG;
      else
        return ERR_POPSTACK_FAIL;
    else
      return ERR_INVALID_SWITCH;
  else
    return ERR_POPSTACK_FAIL;
  /*  Pop, pop, pop it off the stack. */
  if (!PopStringW(pBuf))
    if (lstrcmpiW(pBuf, swSourceFile) == 0)
      if (!PopStringW(pBuf))
        if (lstrlenW(pBuf) < MAX_PATH)
          if (PathFileExistsW(pBuf))
            wcscpy(pData->SrcFile, pBuf);
          else
            return ERR_INVALID_SRCFILE;
        else
          return ERR_STR_TOOLONG;
      else
        return ERR_POPSTACK_FAIL;
    else
      return ERR_INVALID_SWITCH;
  else
    return ERR_POPSTACK_FAIL;
  /*  Pop, pop, pop it off the stack. */
  if (!PopStringW(pBuf))
    if (lstrcmpiW(pBuf, swNextDiscText) == 0)
      if (!PopStringW(pData->NextDiscText))
        pData->NextDiscTextSet = TRUE;
      else
        return ERR_POPSTACK_FAIL;        
    else
      PushStringW(pBuf), pData->NextDiscTextSet = FALSE;
  else
    pData->NextDiscTextSet = FALSE;

  return ERR_SUCCESS;
}

/*	This routine gets the data from the installer and applies it. */
DWORD GetExtractParams(PWIMINFO pData, PWSTR pBuf)
{
  DWORD RetVal = 0;
    
  /*  Pop, pop, pop it off the stack. */
  if (!PopStringW(pBuf))
    if (lstrcmpiW(pBuf, swOutDir) == 0)
      if (!PopStringW(pBuf))
        if (lstrlenW(pBuf) < MAX_PATH)
          if (PathIsDirectoryW(pBuf))
            wcscpy(pData->OutDir, pBuf);
          else
            return ERR_INVALID_OUTDIR;
        else
          return ERR_STR_TOOLONG;
      else
        return ERR_POPSTACK_FAIL;
    else
      return ERR_INVALID_SWITCH;
  else
    return ERR_POPSTACK_FAIL;
  /*  Pop, pop, pop it off the stack. */
  if (!PopStringW(pBuf))
    if (lstrcmpiW(pBuf, swSourceFile) == 0)
      if (!PopStringW(pBuf))
        if (lstrlenW(pBuf) < MAX_PATH)
          if (PathFileExistsW(pBuf))
            wcscpy(pData->SrcFile, pBuf);
          else
            return ERR_INVALID_SRCFILE;
        else
          return ERR_STR_TOOLONG;
      else
        return ERR_POPSTACK_FAIL;
    else
      return ERR_INVALID_SWITCH;
  else
    return ERR_POPSTACK_FAIL;
  /*  Pop, pop, pop it off the stack. */
  if (!PopStringW(pBuf))
    if (lstrcmpiW(pBuf, swTempDir) == 0)
      if (!PopStringW(pBuf))
        if (lstrlenW(pBuf) < MAX_PATH)
          if (PathIsDirectoryW(pBuf))
            wcscpy(pData->TempDir, pBuf);
          else
            return ERR_INVALID_TEMPDIR;
        else
          return ERR_STR_TOOLONG;
      else
        return ERR_POPSTACK_FAIL;
    else
    {
      PushStringW(pBuf);
      RetVal = GetTempPathW(MAX_PATH, pData->TempDir);
      if (RetVal > MAX_PATH || (RetVal == 0))
        return ERR_GETTEMPPATH_FAIL;
      else
        if (!PathIsDirectoryW(pData->TempDir))
          return ERR_INVALID_TEMPDIR;
    }
  else
  {
    RetVal = GetTempPathW(MAX_PATH, pData->TempDir);
    if (RetVal > MAX_PATH || (RetVal == 0))
      return ERR_GETTEMPPATH_FAIL;
    else
      if (!PathIsDirectoryW(pData->TempDir))
        return ERR_INVALID_TEMPDIR;
  }
  /*  Pop, pop, pop it off the stack. */
  if (!PopStringW(pBuf))
    if (lstrcmpiW(pBuf, swImageIndex) == 0)
      if (!PopStringW(pBuf))
      {
        DWORD dwIndex = (DWORD)myatou(pBuf);
        if (dwIndex)
          pData->ImageIndex = dwIndex;
        else
          pData->ImageIndex = 1;
      }
      else
        return ERR_POPSTACK_FAIL;
    else
      PushStringW(pBuf), pData->ImageIndex = 1;
  else
    pData->ImageIndex = 1;
  /*  Pop, pop, pop it off the stack. */
  if (!PopStringW(pBuf))
    if (lstrcmpiW(pBuf, swReadSequential) == 0)
      pData->ReadSequential = TRUE;
    else
      PushStringW(pBuf), pData->ReadSequential = FALSE;
  else
    pData->ReadSequential = FALSE;
  /*  Pop, pop, pop it off the stack. */
  if (!PopStringW(pBuf))
    if (lstrcmpiW(pBuf, swVerify) == 0)
      pData->Verify = TRUE;
    else
      PushStringW(pBuf), pData->Verify = FALSE;
  else
    pData->Verify = FALSE;

  return ERR_SUCCESS;
}

void AddDirPrivate(HWND hwndParent, BOOL Append)
{
  DWORD RetVal;
  PWIMINFO pData;
  PWSTR pBuf;

  pData = GlobalAlloc(GPTR, sizeof(WIMINFO));
  pBuf = GlobalAlloc(GPTR, sizeof(WCHAR)*g_stringsize);

  RetVal = GetWriteParams(pData, pBuf);
  GlobalFree(pBuf);
  if (RetVal)
  {
    GlobalFree(pData);
    pushint(RetVal);
    return;
  }
  pData->Append = Append;
  HookUI(hwndParent);
  RetVal = WriteWIMFile(pData);
  GlobalFree(pData);
  if (RetVal)
  {
    pushint(RetVal);
    return;
  }
  pushint(ERR_SUCCESS);
}

void SplitPrivate(HWND hwndParent)
{
  DWORD RetVal;
  PWIMINFO pData;
  PWSTR pBuf;

  pData = GlobalAlloc(GPTR, sizeof(WIMINFO));
  pBuf = GlobalAlloc(GPTR, sizeof(WCHAR)*g_stringsize);

  RetVal = GetSplitParams(pData, pBuf);
  GlobalFree(pBuf);
  if (RetVal)
  {
    GlobalFree(pData);
    pushint(RetVal);
    return;
  }
  HookUI(hwndParent);
  RetVal = SplitWIMFile(pData);
  GlobalFree(pData);
  if (RetVal)
  {
    pushint(RetVal);
    return;
  }
  pushint(ERR_SUCCESS);
}

void CopyPrivate(HWND hwndParent)
{
  DWORD RetVal;
  PWIMINFO pData;
  PWSTR pBuf;

  pData = GlobalAlloc(GPTR, sizeof(WIMINFO));
  pBuf = GlobalAlloc(GPTR, sizeof(WCHAR)*g_stringsize);

  RetVal = GetCopyParams(pData, pBuf);
  GlobalFree(pBuf);
  if (RetVal)
  {
    GlobalFree(pData);
    pushint(RetVal);
    return;
  }
  HookUI(hwndParent);
  RetVal = CopyWIMFile(pData);
  GlobalFree(pData);
  if (RetVal)
  {
    pushint(RetVal);
    return;
  }
  pushint(ERR_SUCCESS);
}

void ExtractPrivate(HWND hwndParent)
{
  DWORD RetVal;
  PWIMINFO pData;
  PWSTR pBuf;

  pData = GlobalAlloc(GPTR, sizeof(WIMINFO));
  pBuf = GlobalAlloc(GPTR, sizeof(WCHAR)*g_stringsize);

  RetVal = GetExtractParams(pData, pBuf);
  GlobalFree(pBuf);
  if (RetVal)
  {
    GlobalFree(pData);
    pushint(RetVal);
    return;
  }
  HookUI(hwndParent);
  RetVal = ExtractWIMFile(pData);
  GlobalFree(pData);
  if (RetVal)
  {
    pushint(RetVal);
    return;
  }
  pushint(ERR_SUCCESS);
}

/*  Exported functions. */
void __declspec(dllexport) __cdecl AddDir(HWND hwndParent, int string_size, TCHAR *variables, stack_t **stacktop, extra_parameters* xp)
{
  /*  Initialize the stack so we can access it from our DLL. */
  EXDLL_INIT();
  Initialize(xp);

  AddDirPrivate(hwndParent, FALSE);
}

void __declspec(dllexport) __cdecl AppendDir(HWND hwndParent, int string_size, TCHAR *variables, stack_t **stacktop, extra_parameters* xp)
{
  /*  Initialize the stack so we can access it from our DLL. */
  EXDLL_INIT();
  Initialize(xp);

  AddDirPrivate(hwndParent, TRUE);
}

void __declspec(dllexport) __cdecl Split(HWND hwndParent, int string_size, TCHAR *variables, stack_t **stacktop, extra_parameters* xp)
{
  /*  Initialize the stack so we can access it from our DLL. */
  EXDLL_INIT();
  Initialize(xp);

  SplitPrivate(hwndParent);
}

void __declspec(dllexport) __cdecl Copy(HWND hwndParent, int string_size, TCHAR *variables, stack_t **stacktop, extra_parameters* xp)
{
  /*  Initialize the stack so we can access it from our DLL. */
  EXDLL_INIT();
  Initialize(xp);

  CopyPrivate(hwndParent);
}

void __declspec(dllexport) __cdecl Extract(HWND hwndParent, int string_size, TCHAR *variables, stack_t **stacktop, extra_parameters* xp)
{
  /*  Initialize the stack so we can access it from our DLL. */
  EXDLL_INIT();
  Initialize(xp);

  ExtractPrivate(hwndParent);
}

/*  Our DLL entry point, this is called when we first load up our DLL. */
BOOL WINAPI _DllMainCRTStartup(HANDLE hInst, ULONG ul_reason_for_call, LPVOID lpReserved)
{
  hInstance = hInst;

  return TRUE;
}