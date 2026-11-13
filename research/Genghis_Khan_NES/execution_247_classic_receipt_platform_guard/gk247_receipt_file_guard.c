#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "gk247_receipt_file_guard.h"
#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <stdint.h>
int gk247_receipt_file_guard(int fd,const char *path,unsigned expected_size){
 if(fd<0||!path||!*path||!expected_size)return 0;
 const intptr_t raw=_get_osfhandle(fd);
 if(raw==-1)return 0;
 const HANDLE h=(HANDLE)raw;
 BY_HANDLE_FILE_INFORMATION info;
 LARGE_INTEGER sz;
 DWORD attr=GetFileAttributesA(path);
 if(attr==INVALID_FILE_ATTRIBUTES ||
    (attr&(FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_REPARSE_POINT)) ||
    GetFileType(h)!=FILE_TYPE_DISK ||
    !GetFileInformationByHandle(h,&info) ||
    info.nNumberOfLinks!=1u ||
    (info.dwFileAttributes&(FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_REPARSE_POINT)) ||
    !GetFileSizeEx(h,&sz) ||
    sz.QuadPart!=(LONGLONG)expected_size)return 0;
 return 1;
}
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
int gk247_receipt_file_guard(int fd,const char *path,unsigned expected_size){
 struct stat st;
 return fd>=0&&path&&*path&&expected_size>0&&fstat(fd,&st)==0&&S_ISREG(st.st_mode)&&
        st.st_uid==geteuid()&&st.st_nlink==1&&
        (st.st_mode&077)==0&&st.st_size==(off_t)expected_size;
}
#endif
