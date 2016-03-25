#ifndef __VI_ENGINE_H__
#define __VI_ENGINE_H__

#include "wchar.h"

typedef unsigned char UChar;
#define VNFalse (0)
#define VNTrue (1)

void VNIMResetWord();
void VNIMInitVNIM();
void VNIMDestroyVNIM();
int VNIMProcessKey(UChar keyCode, int capStatus);
int VNIMConvertWordToWChar(wchar_t* outBuffer, int* outLength);

 #endif //__VI_ENGINE_H__