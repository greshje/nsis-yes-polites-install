/*
 * WimImage plugin for NSIS 
 *
 * By Jason Ross aka JasonFriday13 on the forums.
 * Copyright 2014 MouseHelmet Software
 *
 * errorcodes.h
 * Contains the error codes in an enum format.
 */

#ifndef ERRORCODES
#define ERRORCODES

enum
{
  ERR_SUCCESS, /* Function finished successfully. */
  ERR_POPSTACK_FAIL, /* Pop failed (is stack empty?) */
  ERR_INVALID_SWITCH, /* Invalid switch used. */
  ERR_STR_TOOLONG, /* String length exceeds MAX_PATH (260 chars). */
  ERR_INVALID_SRCDIR, /* Invalid source directory. */
  ERR_INVALID_SRCFILE, /* Invalid source file. */
  ERR_INVALID_OUTDIR, /* Invalid out directory. */
  ERR_INVALID_OUTFILE, /* Invalid out file. */
  ERR_INVALID_TEMPDIR, /* Invalid temp directory. */
  ERR_INVALID_COMPRESSION, /* Invalid compression type. */
  ERR_INVALID_SPLITSIZE, /* Invalid split size. */
  ERR_INVALID_RESERVESIZE, /* Invalid reserve size. */
  ERR_GETTEMPPATH_FAIL, /* GetTempPath failed. */
  /* WIM related error codes. */
  ERR_WIMREGISTERMESSAGECALLBACK_FAIL, /* WIMRegisterMessageCallback failed. */
  ERR_WIMCREATEFILE_FAIL, /* WIMCreateFile failed. */ 
  ERR_WIMSETTEMPORARYPATH_FAIL, /* WIMSetTemporaryPath failed. */
  ERR_WIMCAPTUREIMAGE_FAIL, /* WIMCaptureImage failed. */
  ERR_WIMALREADY_SPLIT, /* The source file is already split. */
  ERR_WIMSPLITFILE_FAIL, /* WIMSplitFile failed. */
  ERR_WIMSPLITFILE_MOREDATA, /* Not enough data to split. */
  ERR_WIMGETATTRIBUTES_FAIL, /* WIMGetAttributes failed. */
  ERR_WIMCOPYFILE_FAIL, /* WIMCopyFile failed. */
  ERR_WIMCOPYFILEPART_ABORT, /* WIMCopyFile file part aborted. */
  ERR_WIMCOPYFILEPART_FAIL, /* WIMCopyFile file part failed. */
  ERR_WIMSETREFERENCEFILE_FAIL, /* WIMSetReferenceFile failed. */
  ERR_INVALID_IMAGEINDEX, /* Invalid image number for extraction. */
  ERR_WIMLOADIMAGE_FAIL, /* WIMLoadImage failed. */ 
  ERR_WIMAPPLYIMAGE_FAIL /* WIMApplyImage failed. */
};
#endif