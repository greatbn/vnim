#ifndef __VNIM_PRIVATE_H__
#define __VNIM_PRIVATE_H__

#define CharTransform1 (0) //a(
#define CharTransform2 (1) //a^, e^, o^
#define CharTransform3 (2) //o*, u*
#define CharTransform4 (3) //d+
#define CharTransform5 (4) //w->u*
#define CharTransformMax (5)

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
#define VNCharI ('i')
#define VNCharJ ('j')
#define VNCharO ('o')
#define VNCharR ('r')
#define VNCharS ('s')
#define VNCharU ('u')
#define VNCharW ('w')
#define VNCharX ('X')
#define VNCharY ('y')
#define VNCharZ ('z') 

typedef unsigned char UChar;

static const UChar TelexCharTransformIndex[][2] = {
    {VNCharW, CharTransform1},   
    {VNCharA, CharTransform2},   
    {VNCharE, CharTransform2},
    {VNCharO, CharTransform2},
    {VNCharW, CharTransform3},
    {VNCharD, CharTransform4},
    {VNCharW, CharTransform5}
};
static const int TelexCharTransformNumber = 7;
    
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
    VNChar chars[WORD_MAX_LENGTH];
    UChar transform;
    int length;
} VNWord;

#define VNFalse (0)
#define VNTrue (1)

 #endif //__VNIM_PRIVATE_H__