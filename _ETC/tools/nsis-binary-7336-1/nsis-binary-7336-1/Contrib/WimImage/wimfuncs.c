/*
 * WimImage plugin for NSIS 
 *
 * By Jason Ross aka JasonFriday13 on the forums.
 * Copyright 2014 MouseHelmet Software
 *
 * wimgapi.c
 * Contains the wim functions for using .wim files.
 */

#include <windows.h>
#include <commctrl.h> /* For ProgressBar control only. */
#include <shlwapi.h>
#include "nsis\pluginapi.h"
#include "wimgapi\Include\wimgapi.h"
#include "wimfuncs.h"
#include "errorcodes.h"

HWND InstBar = 0, DetailText = 0, HwndParent = 0;
LARGE_INTEGER ChunkSize;
PBRANGE Range;
WCHAR NextDiscText[MAX_PATH] = L"Please insert disc %d to continue.";

/* This function allows us to use the progress bar on the InstFiles page.
   This function doesn't change the parent controls it reads from. */
void HookUI(HWND hwndParent)
{
  HwndParent = hwndParent;
  /* Store the reqired handles so that it is easier to code. */
  InstBar = GetDlgItem(FindWindowEx(hwndParent, 0, _T("#32770"), 0), 1004);
  DetailText = GetDlgItem(FindWindowEx(hwndParent, 0, _T("#32770"), 0), 1006);
  /* Set to 0, and find the range of the progress bar. */
  SendMessage(InstBar, PBM_SETPOS, 0, 0);
  SendMessage(InstBar, PBM_GETRANGE, FALSE, (LPARAM)&Range);
  /* Find out what 1% is. Unlikely to be less than 1 aka !(Range.iHigh < 100).
  The low part is used to store this multiplier for the progressbar. */
  Range.iLow = Range.iHigh/100;
}

/* These functions do the magic on the InstFiles page. */
void SetPos(WPARAM wParam)
{
  if (InstBar) SendMessage(InstBar, PBM_SETPOS, wParam * Range.iLow, 0);
}

void SetText(LPCWSTR Text)
{
  if (DetailText) SetWindowTextW(DetailText, Text);
}

/* Callback function. */
DWORD WINAPI WIMCallback(DWORD dwMsg, WPARAM wParam, LPARAM lParam, PVOID pvIgnored)
/* Message ID; usually a filename; usually an error code; used to maintain caller context. */
{
  PLARGE_INTEGER pliChunkSize = NULL;
  PWSTR *ppszFileName = NULL;

  switch (dwMsg)
  {
    case WIM_MSG_PROGRESS:
      /* Sends the current percentage. */
      SetPos(wParam);
      break;
    case WIM_MSG_PROCESS:
      /* Sends the file name being processed. */
      SetText((LPCWSTR)wParam);
      break;
    case WIM_MSG_SPLIT:
      /* This message is sent when each split .wim file is successful.
         You can change a split .wim file name or size by using this message. */
      ppszFileName = (PWSTR*)wParam;
      pliChunkSize = (PLARGE_INTEGER)lParam;
      /* Change the chunk size */
      pliChunkSize->QuadPart = ChunkSize.QuadPart;
      /* Display the next file to be created. */
      SetText((LPCWSTR)*ppszFileName);
      break;
/* Unused messages. */
#if 0
    case WIM_MSG_ERROR:
      /* This message is sent when an error is reported. */
      MessageBox(HwndParent, _T("WIM_MSG_ERROR"), _T("WimImage"), MB_OK);
      break;
    case WIM_MSG_RETRY:
      /* This message is sent when the file is being reapplied because of a
         network timeout. Retry is attempted up to five times. */
      MessageBox(HwndParent, _T("WIM_MSG_RETRY"), _T("WimImage"), MB_OK);
      break;
    case WIM_MSG_INFO:
      /* This message is sent when an informational message is available. */
      MessageBox(HwndParent, _T("WIM_MSG_INFO"), _T("WimImage"), MB_OK);
      break;
    case WIM_MSG_WARNING:
      /* This message is sent when a warning message is available. */
      MessageBox(HwndParent, _T("WIM_MSG_WARNING"), _T("WimImage"), MB_OK);
      break;
#endif
  }
  /* To abort image processing, return WIM_MSG_ABORT_IMAGE. */
  return WIM_MSG_SUCCESS;
}

LPPROGRESS_ROUTINE CopyProgressCallback(
  LARGE_INTEGER TotalFileSize,
  LARGE_INTEGER TotalBytesTransferred,
  LARGE_INTEGER StreamSize,
  LARGE_INTEGER StreamBytesTransferred,
  DWORD dwStreamNumber,
  DWORD dwCallbackReason,
  HANDLE hSourceFile,
  HANDLE hDestinationFile,
  LPVOID lpData)
{
  LARGE_INTEGER tmp;
  
  /* Calculate the percent relative to the progress bar range.
  tmp.LowPart should be the percent, which is multiplied in SetPos(). */
  tmp.QuadPart = TotalFileSize.QuadPart / 100;
  tmp.QuadPart = TotalBytesTransferred.QuadPart / tmp.QuadPart;
  SetPos(tmp.LowPart);

  return PROGRESS_CONTINUE;
}

/* This function writes a .wim file based on it's argument. */
DWORD WriteWIMFile(PWIMINFO Data)
{
  HANDLE hWim = NULL, hImage = NULL;
  DWORD RetVal = 0,
    dwDisposition = 0,
    dwDesiredAccess = 0,
    dwCreateFlags = 0,
    dwCaptureFlags = 0,
    dwCreationResult = 0;

  if (Data->Append)
    /* Use "append" mode. Add a new image index to a pre-existing .wim file if one is found. */
    dwDisposition = WIM_OPEN_EXISTING, dwDesiredAccess = WIM_GENERIC_WRITE | WIM_GENERIC_READ;
  else
    /* Use "capture" mode. Overwrite a pre-existing .wim file. */
    dwDisposition = WIM_CREATE_ALWAYS, dwDesiredAccess = WIM_GENERIC_WRITE;
  /* Check the Verify flag. */
  if (Data->Verify) dwCreateFlags |= WIM_FLAG_VERIFY;
  /* Open the .wim file. */
  hWim = WIMCreateFile(Data->OutFile, dwDesiredAccess, dwDisposition, dwCreateFlags, Data->Compress, &dwCreationResult);
  if (!hWim)
    return ERR_WIMCREATEFILE_FAIL;
  /* Register the Callback function. */
  if (WIMRegisterMessageCallback(hWim, (FARPROC)WIMCallback, NULL) == INVALID_CALLBACK_VALUE)
  {
    if (hWim) WIMCloseHandle(hWim);
    return ERR_WIMREGISTERMESSAGECALLBACK_FAIL;
  }
  /* WIMGAPI uses temporary files. You must specify where to store the files. */
  RetVal = WIMSetTemporaryPath(hWim, Data->TempDir);
  if (!RetVal)
  {
    WIMUnregisterMessageCallback(hWim, (FARPROC)WIMCallback);
    if (hWim) WIMCloseHandle(hWim);
    return ERR_WIMSETTEMPORARYPATH_FAIL;
  }
  /* Check the Verify flag. */
  if (Data->Verify) dwCaptureFlags |= WIM_FLAG_VERIFY;
  /* Finally, perform the capture operation. */
  hImage = WIMCaptureImage(hWim, Data->SrcDir, dwCaptureFlags);
  if (!hImage)
  {
    WIMUnregisterMessageCallback(hWim, (FARPROC)WIMCallback);
    if (hWim) WIMCloseHandle(hWim);
    return ERR_WIMCAPTUREIMAGE_FAIL;
  }
  /* Close the handles we used. */
  WIMUnregisterMessageCallback(hWim, (FARPROC)WIMCallback);
  if (hImage) WIMCloseHandle(hImage);
  if (hWim) WIMCloseHandle(hWim);

  return ERR_SUCCESS;
}

/* This function splits a .wim file based on it's argument. */
DWORD SplitWIMFile(PWIMINFO Data)
{
  HANDLE hWim = NULL;
  DWORD RetVal = 0, dwCreateFlags = 0, dwCreateResult = 0;

  /* Check the Verify flag. */
  if (Data->Verify) dwCreateFlags |= WIM_FLAG_VERIFY;
  /* Open the .wim file. */
  hWim = WIMCreateFile(Data->SrcFile, WIM_GENERIC_READ, WIM_OPEN_EXISTING, dwCreateFlags, 0, &dwCreateResult);
  if (!hWim)
    return ERR_WIMCREATEFILE_FAIL;
  /* Register the Callback function. */
  if (WIMRegisterMessageCallback(hWim, (FARPROC)WIMCallback, NULL) == INVALID_CALLBACK_VALUE)
  {
    if (hWim) WIMCloseHandle(hWim);
    return ERR_WIMREGISTERMESSAGECALLBACK_FAIL;
  }
  /* Get the attributes from the current .wim file. */
  RetVal = WIMGetAttributes(hWim, &Data->Wim_Info, sizeof(Data->Wim_Info));
  if (!RetVal)
  {
    WIMUnregisterMessageCallback(hWim, (FARPROC)WIMCallback);
    if (hWim) WIMCloseHandle(hWim);
    return ERR_WIMGETATTRIBUTES_FAIL;
  }
  /* Verify if the .wim file has already been split. */
  if (Data->Wim_Info.TotalParts != 1)
  {
    WIMUnregisterMessageCallback(hWim, (FARPROC)WIMCallback);
    if (hWim) WIMCloseHandle(hWim);
    return ERR_WIMALREADY_SPLIT;
  }
  /* Set the chunk size so that it's global. */
  ChunkSize = Data->ChunkSize;
  /* Display the text of the first file to be created. */
  SetText(Data->OutFile);
  /* Split the .wim file. */
  RetVal = WIMSplitFile(hWim, Data->OutFile, &Data->FirstChunkSize, 0);
  if (!RetVal || RetVal == ERROR_MORE_DATA)
  {
    WIMUnregisterMessageCallback(hWim, (FARPROC)WIMCallback);
    if (hWim) WIMCloseHandle(hWim);
    if (RetVal == ERROR_MORE_DATA)
      return ERR_WIMSPLITFILE_MOREDATA;
    else
      return ERR_WIMSPLITFILE_FAIL;
  }
  /* When you are finished, close the handle that you created in the previous steps. */
  WIMUnregisterMessageCallback(hWim, (FARPROC)WIMCallback);
  if (hWim) WIMCloseHandle(hWim);

  return ERR_SUCCESS;
}

/* This function copies a wim file based on it's argument. */
DWORD CopyWIMFile(PWIMINFO Data)
{
  HANDLE hWim = NULL;
  DWORD RetVal = 0, dwCreateFlags = 0, dwApplyFlags = 0, dwCreateResult = 0;

  /* Open the .wim file. */
  hWim = WIMCreateFile(Data->SrcFile, WIM_GENERIC_READ, WIM_OPEN_EXISTING, dwCreateFlags, 0, &dwCreateResult);
  if (!hWim)
    return ERR_WIMCREATEFILE_FAIL;
  /* Get the attributes of the .wim file. */
  RetVal = WIMGetAttributes(hWim, &Data->Wim_Info, sizeof(Data->Wim_Info));
  if (!RetVal)
  {
    if (hWim) WIMCloseHandle(hWim);
    return ERR_WIMGETATTRIBUTES_FAIL;
  }
  /* Check the next disc option. */
  if (Data->NextDiscTextSet) wcscpy(NextDiscText, Data->NextDiscText); //StringCchCopyW(NextDiscText, sizeof(NextDiscText), Data->NextDiscText);
  /* Check for a split .wim and copy the files if it is true. */
  if (Data->Wim_Info.WimAttributes & WIM_ATTRIBUTE_SPANNED)
  {
    UINT i = 0;
    PWSTR pSrcFile, pOutFile, pFileOnly;
    PWSTR pBuffer1, pBuffer2, pNextDiscText, pTitleText;

    pSrcFile = GlobalAlloc(GPTR, sizeof(WCHAR)*MAX_PATH);
    pOutFile = GlobalAlloc(GPTR, sizeof(WCHAR)*MAX_PATH);
    pFileOnly = GlobalAlloc(GPTR, sizeof(WCHAR)*MAX_PATH);

    /* Close handle so that we can copy the file. */
    WIMCloseHandle(hWim);
    /* Duplicate the strings for use. */
    /*StringCchCopyW*/wcscpy(pSrcFile, Data->SrcFile);
    /*StringCchCopyW*/wcscpy(pOutFile, Data->OutFile);
    /*StringCchCopyW*/wcscpy(pFileOnly, Data->OutFile);
    /* Strip the extensions off the files so we can count them. */
    PathRemoveExtensionW(pSrcFile);
    PathRemoveExtensionW(pOutFile);
    /* Strip the path off for display purposes. */
    PathStripPathW(pFileOnly);
    SetText(pFileOnly);
    /* Attempt to copy first part. */
    if (!WIMCopyFile(Data->SrcFile, Data->OutFile, (LPPROGRESS_ROUTINE)CopyProgressCallback, 0, 0, WIM_COPY_FILE_RETRY))
    {
      GlobalFree(pSrcFile);
      GlobalFree(pOutFile);
      GlobalFree(pFileOnly);
      return ERR_WIMCOPYFILE_FAIL;
    }
    pBuffer1 = GlobalAlloc(GPTR, sizeof(WCHAR)*MAX_PATH);
    pBuffer2 = GlobalAlloc(GPTR, sizeof(WCHAR)*MAX_PATH);
    pNextDiscText = GlobalAlloc(GPTR, sizeof(WCHAR)*MAX_PATH);
    pTitleText = GlobalAlloc(GPTR, sizeof(WCHAR)*MAX_PATH);
    GetWindowTextW(HwndParent, pTitleText, MAX_PATH);
    /* Count our way though the files to be copied. */
    for (i = 2; i <= Data->Wim_Info.TotalParts; i++)
    {
      wsprintfW(pBuffer1, L"%s%d.swm", pSrcFile, i);
      wsprintfW(pBuffer2, L"%s%d.swm", pOutFile, i);
      wsprintfW(pNextDiscText, NextDiscText, i);
      wsprintfW(pFileOnly, L"%s%d.swm", pOutFile, i);
/*      StringCbPrintfW(szBuffer1, sizeof(szBuffer1), L"%s%d.swm", szSrcFile, i);
      StringCbPrintfW(szBuffer2, sizeof(szBuffer2), L"%s%d.swm", szOutFile, i);
      StringCbPrintfW(szNextDiscText, sizeof(szNextDiscText), NextDiscText, i);
      StringCbPrintfW(szFileOnly, sizeof(szFileOnly), L"%s%d.swm", szOutFile, i);*/
      PathStripPathW(pFileOnly);
      while (!PathFileExistsW(pBuffer1))
      {
        if (MessageBoxW(HwndParent, pNextDiscText, pTitleText, MB_OKCANCEL|MB_ICONINFORMATION) & IDCANCEL)
        {
          GlobalFree(pSrcFile);
          GlobalFree(pOutFile);
          GlobalFree(pFileOnly);

          GlobalFree(pBuffer1);
          GlobalFree(pBuffer2);
          GlobalFree(pNextDiscText);

          GlobalFree(pTitleText);
          return ERR_WIMCOPYFILEPART_ABORT;
        }
      }
      SetText(pFileOnly);
      if (!WIMCopyFile(pBuffer1, pBuffer2, (LPPROGRESS_ROUTINE)CopyProgressCallback, 0, 0, WIM_COPY_FILE_RETRY))
      {
        GlobalFree(pSrcFile);
        GlobalFree(pOutFile);
        GlobalFree(pFileOnly);

        GlobalFree(pBuffer1);
        GlobalFree(pBuffer2);
        GlobalFree(pNextDiscText);

        GlobalFree(pTitleText);
        return ERR_WIMCOPYFILEPART_FAIL;
      }
    }
    GlobalFree(pSrcFile);
    GlobalFree(pOutFile);
    GlobalFree(pFileOnly);

    GlobalFree(pBuffer1);
    GlobalFree(pBuffer2);
    GlobalFree(pNextDiscText);

    GlobalFree(pTitleText);
  }
  else
  {
    PWSTR pFileOnly;

    pFileOnly = GlobalAlloc(GPTR, sizeof(WCHAR)*MAX_PATH);
    /* Close handle so that we can copy the file. */
    WIMCloseHandle(hWim);
    /* Set display text for the out file. */
    /*StringCchCopyW*/wcscpy(pFileOnly, Data->OutFile);
    PathStripPathW(pFileOnly);
    SetText(pFileOnly);
    GlobalFree(pFileOnly);
    /* Attempt to copy file. */
    if (!WIMCopyFile(Data->SrcFile, Data->OutFile, (LPPROGRESS_ROUTINE)CopyProgressCallback, 0, 0, WIM_COPY_FILE_RETRY))
      return ERR_WIMCOPYFILE_FAIL;
  }

  return ERR_SUCCESS;
}

/* This function extracts an image from a wim file based on it's argument. */
DWORD ExtractWIMFile(WIMINFO *Data)
{
  HANDLE hWim = NULL, hImage = NULL;
  
  DWORD RetVal = 0, dwCreateFlags = 0, dwApplyFlags = 0, dwCreateResult = 0;

  /* Check the Verify flag. */
  if (Data->Verify) dwCreateFlags |= WIM_FLAG_VERIFY;
  /* Open the .wim file. */
  hWim = WIMCreateFile(Data->SrcFile, WIM_GENERIC_READ, WIM_OPEN_EXISTING, dwCreateFlags, 0, &dwCreateResult);
  if (!hWim)
    return ERR_WIMCREATEFILE_FAIL;
  /* Register the callback function. */
  if (WIMRegisterMessageCallback(hWim, (FARPROC)WIMCallback, NULL) == INVALID_CALLBACK_VALUE)
  {
    if (hWim) WIMCloseHandle(hWim);
    return ERR_WIMREGISTERMESSAGECALLBACK_FAIL;
  }
  /* Note: To attach split .wim files (SWM) to this session, use:
     WIMSetReferenceFile(hWim, pszFileName, dwFlags) */
  RetVal = WIMGetAttributes(hWim, &Data->Wim_Info, sizeof(Data->Wim_Info));
  if (!RetVal)
  {
    WIMUnregisterMessageCallback(hWim, (FARPROC)WIMCallback);
    if (hWim) WIMCloseHandle(hWim);
    return ERR_WIMGETATTRIBUTES_FAIL;
  }
  /* Check the image count. */
  if (Data->Wim_Info.ImageCount < Data->ImageIndex)
  {
    WIMUnregisterMessageCallback(hWim, (FARPROC)WIMCallback);
    if (hWim) WIMCloseHandle(hWim);
    return ERR_INVALID_IMAGEINDEX;
  }
  /* Check for a split .wim and reference the files if it is true. */
  if (Data->Wim_Info.WimAttributes & WIM_ATTRIBUTE_SPANNED)
  {
    UINT i = 0;
    PWSTR pBuffer, pTmp;

    pBuffer = GlobalAlloc(GPTR, sizeof(WCHAR)*MAX_PATH);
    pTmp = GlobalAlloc(GPTR, sizeof(WCHAR)*MAX_PATH);

    wcscpy(pBuffer, Data->SrcFile);
    PathRemoveExtensionW(pBuffer);
    for (i = 2; i <= Data->Wim_Info.TotalParts; i++)
    {
      wsprintfW(pTmp, L"%s%d.swm", pBuffer, i);
      if (!PathFileExistsW(pTmp) || !WIMSetReferenceFile(hWim, pTmp, WIM_REFERENCE_APPEND))
      {
        WIMUnregisterMessageCallback(hWim, (FARPROC)WIMCallback);
        if (hWim) WIMCloseHandle(hWim);
        GlobalFree(pBuffer);
        GlobalFree(pTmp);
        return ERR_WIMSETREFERENCEFILE_FAIL;
      }
    }
    GlobalFree(pBuffer);
    GlobalFree(pTmp);
  }
  /* Set the temporary working directory. */
  RetVal = WIMSetTemporaryPath(hWim, Data->TempDir);
  if (!RetVal)
  {
    WIMUnregisterMessageCallback(hWim, (FARPROC)WIMCallback);
    if (hWim) WIMCloseHandle(hWim);
    return ERR_WIMSETTEMPORARYPATH_FAIL;
  }
  /* Retrieve the image handle for the image at the given index. */
  hImage = WIMLoadImage(hWim, Data->ImageIndex);
  if (!hImage)
  {
    WIMUnregisterMessageCallback(hWim, (FARPROC)WIMCallback);
    if (hWim) WIMCloseHandle(hWim);
    return ERR_WIMLOADIMAGE_FAIL;
  }
  /* Check the Verify flag. */
  if (Data->Verify) dwApplyFlags |= WIM_FLAG_VERIFY;
  /* Check the ReadSequential flag. */
  if (Data->ReadSequential) dwApplyFlags |= WIM_FLAG_INDEX;
  /* Finally, apply the image. */
  RetVal = WIMApplyImage(hImage, Data->OutDir, dwApplyFlags);
  if (!RetVal)
  {
    WIMUnregisterMessageCallback(hWim, (FARPROC)WIMCallback);
    if (hImage) WIMCloseHandle(hImage);
    if (hWim) WIMCloseHandle(hWim);
    return ERR_WIMAPPLYIMAGE_FAIL;
  }
  /* Close the handles we used. */
  WIMUnregisterMessageCallback(hWim, (FARPROC)WIMCallback);
  if (hImage) WIMCloseHandle(hImage);
  if (hWim) WIMCloseHandle(hWim);

  return ERR_SUCCESS;
}