/*
 * WimImage plugin for NSIS 
 *
 * By Jason Ross aka JasonFriday13 on the forums.
 * Copyright 2014 MouseHelmet Software
 *
 * wimfuncs.h
 * Contains the wim function declarations from wimfuncs.c.
 */

#ifndef WIMFUNCS
#define WIMFUNCS

#include "wimgapi\Include\wimgapi.h"

typedef struct
{
  WIM_INFO Wim_Info;
  WCHAR SrcDir[MAX_PATH];
  WCHAR SrcFile[MAX_PATH];
  WCHAR OutDir[MAX_PATH];
  WCHAR OutFile[MAX_PATH];
  WCHAR TempDir[MAX_PATH];
  WCHAR NextDiscText[MAX_PATH];
  LARGE_INTEGER ChunkSize;
  LARGE_INTEGER FirstChunkSize;
  DWORD Compress;
  DWORD ImageIndex;
  BOOL Append;
  BOOL NextDiscTextSet;
  BOOL ReadSequential;
  BOOL Verify;
} WIMINFO, *PWIMINFO;

DWORD WriteWIMFile(PWIMINFO Data);
DWORD SplitWIMFile(PWIMINFO Data);
DWORD CopyWIMFile(PWIMINFO Data);
DWORD ExtractWIMFile(PWIMINFO Data); 

void HookUI(HWND hwndParent);

#endif