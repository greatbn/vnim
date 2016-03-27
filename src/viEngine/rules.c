#include "vi-engine-private.h"

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

// const UChar sTelexWordShiftIndex[][2] = {
//     {VNCharS, IndexShift2},
//     {VNCharF, IndexShift4},
//     {VNCharR, IndexShift6},
//     {VNCharX, IndexShift8},
//     {VNCharJ, IndexShift10},
//     {VNCharZ, IndexShift0}    
// };
// const int sTelexWordShiftNum = 6;