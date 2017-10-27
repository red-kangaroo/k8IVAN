/*
 *
 *  Iter Vehemens ad Necem (IVAN)
 *  Copyright (C) Timo Kiviluoto
 *  Released under the GNU General
 *  Public License
 *
 *  See LICENSING which should be included
 *  along with this file for more details
 *
 */
#ifndef _GNU_SOURCE
# define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cctype>

#include "fesave.h"
#include "femath.h"


#ifdef USE_ZLIB
# define Xgetc  gzgetc
# define Xfeof  gzeof
# define Xungc  gzungetc
# define Xseek  gzseek
# define Xtell  gztell
# define Xclre  gzclearerr
#else
# define Xgetc  fgetc
# define Xfeof  feof
# define Xungc  ungetc
# define Xseek  fseek
# define Xtell  ftell
# define Xclre  clearerr
#endif


// ////////////////////////////////////////////////////////////////////////// //
outputfile::outputfile (cfestring &FileName, truth maxcomp, truth AbortOnErr) :
#ifdef USE_ZLIB
  Buffer(gzopen(FileName.CStr(), (maxcomp?"wb9":"wb1"))),
#else
  Buffer(fopen(FileName.CStr(), "wb")),
#endif
  FileName(FileName)
{
  if (AbortOnErr && !IsOpen()) ABORT("Can't open %s for output!", FileName.CStr());
}


outputfile::~outputfile () {
  Close();
}


void outputfile::Close () {
  if (Buffer) {
#ifdef USE_ZLIB
    gzclose(Buffer);
#else
    fclose(Buffer);
#endif
    Buffer = 0;
  }
}


void outputfile::Flush () {
#ifdef USE_ZLIB
  gzflush(Buffer, Z_FINISH);
#else
  fflush(Buffer);
#endif
}


void outputfile::Put (char What) {
#ifdef USE_ZLIB
  gzputc(Buffer, What);
#else
  fputc(What, Buffer);
#endif
}


void outputfile::Write (cchar *Offset, sLong Size) {
#ifdef USE_ZLIB
  gzwrite(Buffer, Offset, Size);
#else
  fwrite(Offset, 1, Size, Buffer);
#endif
}


// ////////////////////////////////////////////////////////////////////////// //
truth inputfile::fileExists (const festring &fname) {
  struct stat st;
  if (stat(fname.CStr(), &st)) return false;
  if (!S_ISREG(st.st_mode)) return false;
  return access(fname.CStr(), R_OK) == 0;
}


festring inputfile::GetMyDir () {
#ifdef IVAN_DATA_DIR
  return IVAN_DATA_DIR;
#else
  char myDir[8192];
  char buf[128];
  pid_t mypid = getpid();
  memset(myDir, 0, sizeof(myDir));
  sprintf(buf, "/proc/%u/exe", (unsigned int)mypid);
  if (readlink(buf, myDir, sizeof(myDir)-1) < 0) {
    strcpy(myDir, ".");
  } else {
    char *p = (char *)strrchr(myDir, '/');
    if (!p) strcpy(myDir, "."); else *p = '\0';
  }
  if (myDir[strlen(myDir)-1] == '/') myDir[strlen(myDir)-1] = '\0';
  return myDir;
#endif
}


////////////////////////////////////////////////////////////////////////////////
inputfile::inputfile () :
  Buffer(0),
  FileName("")
#ifdef USE_ZLIB
  , mFileSize(-1)
#endif
{
}


inputfile::inputfile (cfestring &aFileName, truth AbortOnErr) :
#ifdef USE_ZLIB
  Buffer(gzopen(aFileName.CStr(), "rb")),
#else
  Buffer(fopen(aFileName.CStr(), "rb")),
#endif
  FileName(aFileName)
#ifdef USE_ZLIB
  , mFileSize(-1)
#endif
{
  if (AbortOnErr && !IsOpen()) ABORT("File %s not found!", FileName.CStr());
}


inputfile::~inputfile () {
  Close();
}


bool inputfile::Open (cfestring &aFileName, truth AbortOnErr) {
  Close();
#ifdef USE_ZLIB
  mFileSize = -1;
  Buffer = gzopen(aFileName.CStr(), "rb");
#else
  Buffer = fopen(aFileName.CStr(), "rb");
#endif
  FileName = aFileName;
  mRetFName = aFileName;
  auto mydir = GetMyDir();
  if (mydir.GetSize() > 0 && mRetFName.startsWith(mydir)) {
    mRetFName.Erase(0, mydir.GetSize()+(mydir[mydir.GetSize()-1] != '/' ? 1 : 0));
  }
  if (AbortOnErr && !IsOpen()) ABORT("File %s not found!", aFileName.CStr());
  return IsOpen();
}


void inputfile::Close () {
  if (Buffer) {
#ifdef USE_ZLIB
    gzclose(Buffer);
#else
    fclose(Buffer);
#endif
    Buffer = 0;
  }
}


int inputfile::Get () {
  if (Buffer) {
    int ch = Xgetc(Buffer);
    return (ch < 0 ? EOF : ch);
  } else {
    return EOF;
  }
}


truth inputfile::Eof () {
  return (Buffer ? Xfeof(Buffer) : true);
}


void inputfile::Read (char *Offset, sLong Size) {
  if (!Buffer) ABORT("Trying to read from unopened file '%s'!", FileName.CStr());
#ifdef USE_ZLIB
  if (gzread(Buffer, Offset, Size) != Size) ABORT("File '%s' read error!", FileName.CStr());
#else
  if (fread(Offset, Size, 1, Buffer) != 1) ABORT("File '%s' read error!", FileName.CStr());
#endif
}


void inputfile::SeekPosBegin (sLong Offset) {
  if (Xseek(Buffer, Offset, SEEK_SET) < 0) ABORT("File '%s': seek error!", FileName.CStr());
}


void inputfile::SeekPosCurrent (sLong Offset) {
  if (Xseek(Buffer, Offset, SEEK_CUR) < 0) ABORT("File '%s': seek error!", FileName.CStr());
}


#ifdef USE_ZLIB
void inputfile::SeekPosEnd (sLong Offset) {
  //HACKHACK: emulate this
  if (mFileSize < 0) {
    //SLOOOW, but we have to do that
    int opos;
    char *buffer, buf[512];
    int bufsize;
    for (bufsize = 256*1024; bufsize > (int)sizeof(buf); bufsize /= 2) {
      if ((buffer = (char *)malloc(bufsize)) != NULL) break;
    }
    if (buffer == NULL) { buffer = buf; bufsize = sizeof(buf); }
    //fprintf(stderr, "determining file size...\n");
    mFileSize = opos = gztell(Buffer);
    for (;;) {
      int len = gzread(Buffer, buffer, bufsize);
      if (len < 0) { mFileSize = -1; break; } // error
      mFileSize += len;
      if (len < bufsize) break; // eof reached
    }
    if (buffer != buf) free(buffer);
    //fprintf(stderr, "file size: %d\n", ctx->filesize);
  }
  if (mFileSize < 0) ABORT("File '%s': seek error!", FileName.CStr());
  if (gzseek(Buffer, mFileSize+Offset, SEEK_SET) < 0) ABORT("File '%s': seek error!", FileName.CStr());
}

#else

void inputfile::SeekPosEnd (sLong Offset) {
  if (fseek(Buffer, Offset, SEEK_END) < 0) ABORT("File '%s': seek error!", FileName.CStr());
}
#endif


sLong inputfile::TellPos () {
  return Xtell(Buffer);
}


void inputfile::ClearFlags () {
  Xclre(Buffer);
}


// ////////////////////////////////////////////////////////////////////////// //
outputfile &operator << (outputfile &SaveFile, cfestring &String) {
  uShort Length = String.GetSize();
  SaveFile << Length;
  if (Length) SaveFile.Write(String.CStr(), Length);
  return SaveFile;
}


inputfile &operator >> (inputfile &SaveFile, festring &String) {
  char *RealBuffer, StackBuffer[1024];
  uShort Length;
  SaveFile >> Length;
  RealBuffer = (Length < 1024 ? StackBuffer : new char[Length+1]);
  String.Empty();
  if (Length) {
    SaveFile.Read(RealBuffer, Length);
    RealBuffer[Length] = 0;
    String << RealBuffer;
  }
  if (RealBuffer != StackBuffer) delete [] RealBuffer;
  return SaveFile;
}


outputfile &operator << (outputfile &SaveFile, cchar *String) {
  uShort Length = (String ? strlen(String) : 0);
  SaveFile << Length;
  if (Length) SaveFile.Write(String, Length);
  return SaveFile;
}


inputfile &operator >> (inputfile &SaveFile, char *&String) {
  uShort Length;
  SaveFile >> Length;
  if (Length) {
    String = new char[Length+1];
    SaveFile.Read(String, Length);
    String[Length] = 0;
  } else {
    String = 0;
  }
  return SaveFile;
}
