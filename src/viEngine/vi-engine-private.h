#ifndef __VNIM_PRIVATE_H__
#define __VNIM_PRIVATE_H__

#include "vi-engine.h"

// #define CharTransform0 (0) //
// #define CharTransform1 (1) //a^, e^, o^, d+, u*
// #define CharTransform2 (2) //a(, o*
// #define CharTransformMax (3)

// #define WordTransform0 (0) //z
// #define WordTransform1 (1) //s
// #define WordTransform2 (2) //f
// #define WordTransform3 (3) //r
// #define WordTransform4 (4) //x 
// #define WordTransform5 (5) //j
// #define WordTransformMax (6)

#define VNCharA ('a')
#define VNCharD ('d')
#define VNCharE ('e')
#define VNCharF ('f')
#define VNCharG ('g')
#define VNCharI ('i')
#define VNCharJ ('j')
#define VNCharO ('o')
#define VNCharQ ('q')
#define VNCharR ('r')
#define VNCharS ('s')
#define VNCharU ('u')
#define VNCharW ('w')
#define VNCharX ('x')
#define VNCharY ('y')
#define VNCharZ ('z')
#define VNCharZero 0

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

#define IndexShift0 (0)
#define IndexShift2 (2)
#define IndexShift4 (4)
#define IndexShift6 (6)
#define IndexShift8 (8)
#define IndexShift10 (10)
#define IndexShift12 (12)
#define IndexShift24 (24)
#define IndexShift154 (154)

#define VNTriggerWFull (0)
#define VNTriggerW (1)
#define VNTriggerA (2)
#define VNTriggerD (3)
#define VNTriggerE (4)
#define VNTriggerO (5)
#define VNTriggerF (6)
#define VNTriggerJ (7)
#define VNTriggerR (8)
#define VNTriggerS (9)
#define VNTriggerX (10)
#define VNTriggerZ (11)
#define VNTriggerAW (12)
#define VNTriggerAEO (13)
#define VNTriggerOUW (14)

const UChar sTelexRules[][2] = {
    {VNCharW, VNTriggerWFull},
    {VNCharA, VNTriggerA},
    {VNCharE, VNTriggerE},
    {VNCharO, VNTriggerO},
    {VNCharD, VNTriggerD},
    {VNCharS, VNTriggerS},
    {VNCharF, VNTriggerF},
    {VNCharJ, VNTriggerJ},
    {VNCharR, VNTriggerR},
    {VNCharX, VNTriggerX},
    {VNCharZ, VNTriggerZ},
};
const int sTelexRulesNum = 11;

const UChar sVNIRules[][2] = {
    {'0', VNTriggerZ},
    {'1', VNTriggerS},
    {'2', VNTriggerF},
    {'3', VNTriggerR},
    {'4', VNTriggerX},
    {'5', VNTriggerJ},
    {'6', VNTriggerAEO},
    {'7', VNTriggerOUW},    
    {'8', VNTriggerAW},
    {'9', VNTriggerD},    
};
const int sVNIRulesNum = 10;

#endif //__VNIM_PRIVATE_H__