#ifndef __VNIM_PRIVATE_H__
#define __VNIM_PRIVATE_H__

#include "vi-engine.h"

#define CharTransform0 (0) //
#define CharTransform1 (1) //a^, e^, o^, d+, u*
#define CharTransform2 (2) //a(, o*
#define CharTransformMax (3)

#define WordTransform0 (0) //z
#define WordTransform1 (1) //s
#define WordTransform2 (2) //f
#define WordTransform3 (3) //r
#define WordTransform4 (4) //x 
#define WordTransform5 (5) //j
#define WordTransformMax (6)

#define VNCharA ('a')
#define VNCharD ('d')
#define VNCharE ('e')
#define VNCharF ('f')
#define VNCharG ('g')
#define VNCharI ('i')
#define VNCharJ ('j')
#define VNCharO ('o')
#define VNCharR ('r')
#define VNCharS ('s')
#define VNCharU ('u')
#define VNCharW ('w')
#define VNCharX ('x')
#define VNCharY ('y')
#define VNCharZ ('z')

static const UChar TelexCharTransformIndex[][3] = {
    {VNCharA, CharTransform1, VNCharA},
    {VNCharW, CharTransform2, VNCharA},   
    {VNCharE, CharTransform1, VNCharE},   
    {VNCharO, CharTransform1, VNCharO},
    {VNCharW, CharTransform2, VNCharO},
    {VNCharW, CharTransform1, VNCharU},
    {VNCharD, CharTransform1, VNCharD},
};
static const int TelexCharTransformNumber = 7;

static const UChar TelexCharConversionIndex[][2] = {
    {VNCharW, VNCharU, CharTransform1}
};
static const int TelexCharConversionNumber = 1;
    
static const UChar TelexWordTransformIndex[][2] = {    
    {VNCharS, WordTransform1},
    {VNCharF, WordTransform2},
    {VNCharR, WordTransform3},
    {VNCharX, WordTransform4},
    {VNCharJ, WordTransform5},
    {VNCharZ, WordTransform0}
};
static const int TelexWordTransformNumber = 6;

typedef struct {
    UChar origin;
    UChar transform;
    UChar isUpper;
} VNChar;

#define WORD_MAX_LENGTH 10
typedef struct {
    VNChar chars[WORD_MAX_LENGTH + 1];
    UChar transform;
    int length;
} VNWord;

 #endif //__VNIM_PRIVATE_H__