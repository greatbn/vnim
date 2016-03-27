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

// static const UChar TelexCharTransformIndex[][3] = {
//     {VNCharW, CharTransform2, VNCharO},
//     {VNCharW, CharTransform1, VNCharU},
//     {VNCharW, CharTransform2, VNCharA},
//     {VNCharA, CharTransform1, VNCharA},
//     {VNCharE, CharTransform1, VNCharE},   
//     {VNCharO, CharTransform1, VNCharO},
//     {VNCharD, CharTransform1, VNCharD},
// };
// static const int TelexCharTransformNumber = 7;

// static const UChar TelexCharConversionIndex[][3] = {
//     {VNCharW, VNCharU, CharTransform1}
// };
// static const int TelexCharConversionNumber = 1;
    
// static const UChar TelexWordTransformIndex[][2] = {    
//     {VNCharS, WordTransform1},
//     {VNCharF, WordTransform2},
//     {VNCharR, WordTransform3},
//     {VNCharX, WordTransform4},
//     {VNCharJ, WordTransform5},
//     {VNCharZ, WordTransform0}
// };
// static const int TelexWordTransformNumber = 6;

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

#define TELEX_RULES (0)

typedef struct {
    UChar *** charShift;
    UChar *** wordShift;
    int charShiftNum;
    int wordShiftNum;
    int type;
} Transformation;

extern const UChar sTelexRules[][2];
extern const int sTelexRulesNum;
#endif //__VNIM_PRIVATE_H__