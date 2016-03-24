#ifndef __VNIM_H__
#define __VNIM_H__

#define Transform11 (0) //a(
#define Transform12 (1) //a^
#define Transform13 (2) //o*
#define Transform14 (3) //d+
#define Transform20 (4) //z
#define Transform21 (5) //s
#define Transform22 (6) //f
#define Transform23 (7) //r
#define Transform24 (8) //x 
#define Transform25 (9) //j
#define Transform30 (10) //w ->u*
#define TransformMax (11)
#define TransformLower (0)
#define TransformUpper (1)

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
#define VNCharZ ('Z') 

typedef unsigned char UChar;

static const UChar TelexTransformIndex[][2] = {
    {VNCharW, Transform11},   
    {VNCharA, Transform12},   
    {VNCharE, Transform12},
    {VNCharO, Transform12},
    {VNCharW, Transform13},
    {VNCharD, Transform14},
    {VNCharS, Transform21},
    {VNCharF, Transform22},
    {VNCharR, Transform23},
    {vnCharX, Transform24},
    {VNCharJ, Transform25},
    {VNCharZ, Transform20},
    (VNCharW, Transform30),
};

static const int TelexTransformNumber = 13;

typedef struct {
    UChar origin;
    UChar zeroTrans;
    UChar firstTrans;
    UChar secondTrans;
} VNChar;

#define VNFalse (0)
#define VNTrue (1)

 #endif