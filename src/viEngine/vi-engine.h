#ifndef __VI_ENGINE_H__
#define __VI_ENGINE_H__

#include "wchar.h"

typedef unsigned char UChar;
#define VNFalse (0)
#define VNTrue (1)

#define TELEX_INPUT 0
#define VNI_INPUT 1
/**
 Reset the engine
*/
void ViResetEngine();
void ViInitEngine();
void ViDestroyEngine();
void SetInputEngine(int input);
int ViProcessKey(UChar keyCode, int capStatus);
void ViGetCurrentWord(wchar_t* outBuffer, int* outLength);
int ViProcessBackspace();

#endif //__VI_ENGINE_H__