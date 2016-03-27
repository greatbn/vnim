#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "vi-engine-private.h"

#include "charset.h"

#define BUFFER_LENGTH 100

#define PROCESSED (1)
#define REVERTED (2)
#define NOTHING (0)

static VNWord* sCurrentWord;
// static UChar (*sWordTransformIndex)[2];
// static int sWordTransformNumber;
// static UChar (*sCharTransformIndex)[3];
// static int sCharTransformNumber;
// static UChar (*sCharConversionIndex)[3];
// static int sCharConversionNumber;
static UChar (*sRules)[2];
static int sRulesNum;

//private methods
/**
 * Check if we can transform this character
 */
int ViCanTransform(VNChar* vnChar) {
    return (
        vnChar->origin == VNCharA
        || vnChar->origin == VNCharD
        || vnChar->origin == VNCharE
        || vnChar->origin == VNCharO
        || vnChar->origin == VNCharU
    );
}

/**
 * Check if a character is a vowel or not
 */
int ViIsVowel(UChar keyCode) {
    return (
        keyCode == VNCharA
        || keyCode == VNCharE
        || keyCode == VNCharO
        || keyCode == VNCharU
        || keyCode == VNCharI
        || keyCode == VNCharY
        || keyCode == VNCharZero
    );
}

int ViCanTransformWord(VNWord* vnWord) {
    int i;
    for (i = 0; i < vnWord->length; i++) {
        if (ViIsVowel(vnWord->chars[i].origin)) {
            return VNTrue;
        }
    }
    return VNFalse;
}

/**
 * Check if this is a latin character or not
 */
int ViIsLatinChar(UChar keyCode) {
    return (keyCode >= VNCharA && keyCode <= VNCharZ);
}

/**
 * Check if this character can be used to transform other characters
 */
// int ViIsCharTransformer(UChar keyCode) {
//     int i;
//     for (i = 0; i < sRulesNum; i++) {
//          printf("checking char index %d\n",i);
//         if ((*sTransformationRules->charShift)[i][0] == keyCode) {
//             return VNTrue;
//         }
//     }
    
//     return VNFalse;
// }

/*
 * Check if this character can be used to transform a word
 */
// int ViIsWordTransformer(UChar keyCode) {
//     int i;
//     for (i = 0; i < sTransformationRules->wordShiftNum; i++) {
//         if ((*sTransformationRules->wordShift)[i][0] == keyCode) {
//             return VNTrue;
//         }            
//     }
    
//     return VNFalse;
// }

/*
 * Check if this can be used to transform
 */
int ViIsATransformer(UChar keyCode) {
    printf("ViIsATransformer\n");
    //check if this keyCode is in the transformIndex
    // return (ViIsCharTransformer(keyCode) || ViIsWordTransformer(keyCode));
    int i;
    for (i = 0; i < sRulesNum; i++) {
        if (sRules[i][0] == keyCode) {
            return VNTrue;
        }
    }
    
    return VNFalse;
}

/**
 * Check if can handle this key or not
 */
int ViProcessable(UChar keyCode) {
    return (ViIsLatinChar(keyCode) || ViIsATransformer(keyCode));        
}

/*
 * Do transform a character
 */
int ViCharTransform(VNChar* vnChar, UChar transform) {
    if (vnChar->transform == transform) {
        //revert since this transform is already used
        vnChar->transform = IndexShift0;
        return REVERTED;
    } else {
        //do a transform overridely
        vnChar->transform = transform;
        return PROCESSED;
    }    
}

/**
 *  Find & do a char transform
 */
int ViDoCharTransform(VNWord* vnWord, UChar keyCode, UChar transform) {
    int i;
    for (i=vnWord->length -1; i>=0; i--) {
        if (vnWord->chars[i].origin == keyCode) {
            return ViCharTransform(&(vnWord->chars[i]), transform);
        }
    }
    return NOTHING;
}

/*
 * Apend a character into a word
 */
int ViAppendWord(VNWord* vnWord, UChar keyCode, UChar transform, int capStatus) {
    printf("ViAppendWord [%c]\n", keyCode);
    if ((vnWord->length >= WORD_MAX_LENGTH)) {
        return VNFalse;
    }
    
    VNChar* vnChar = &(vnWord->chars[vnWord->length++]);
    vnChar->origin = keyCode;
    vnChar->transform = transform;
    vnChar->isUpper = capStatus?(VNTrue):(VNFalse);
    
    ViCorrection(sCurrentWord);
    return VNTrue;
}

int ViDoProcessOUW(VNWord* vnWord) {
    int i;
    //process u*o*
    for (i=vnWord->length-1; i>=0; i--) {
        if (vnWord->chars[i].origin == VNCharO) {
            if (i>0 && vnWord->chars[i-1].origin == VNCharU) { //uo
                if (vnWord->chars[i-1].transform == IndexShift12 && vnWord->chars[i].transform == IndexShift24) {
                    //revert
                    vnWord->chars[i-1].transform = IndexShift0;
                    vnWord->chars[i].transform = IndexShift0;
                    return REVERTED;
                } else {
                    // u*o*
                    vnWord->chars[i-1].transform = IndexShift12;
                    vnWord->chars[i].transform = IndexShift24;
                    return PROCESSED;
                }
            } else { //o* only
                return ViCharTransform(&(vnWord->chars[i]), IndexShift24);
            }
        } else if (vnWord->chars[i].origin == VNCharU) { //u only
            return ViCharTransform(&(vnWord->chars[i]), IndexShift12); 
        }
    }
    return NOTHING;    
}

/**
 * Do process w
 */
 int ViDoProcessW(VNWord* vnWord) {
    printf("ViDoProcessW\n");
     //revert u*o*
    int retVal = ViDoProcessOUW(vnWord);
    if (retVal != NOTHING) return retVal;
     //process a
    return ViDoCharTransform(vnWord, VNCharA, IndexShift24);     
 }
 
int ViDoProcessWFull(VNWord* vnWord, int capStatus) {
    printf("ViDoProcessWFull\n");
    int i;
    if (vnWord->length > 0) {
        //revert w->u* first
        for (i = vnWord->length -1; i>=0; i--) {
            VNChar* lastChar = &(vnWord->chars[i]);
            if (lastChar->origin == VNCharZero) {
                if (vnWord->transform == IndexShift0 && i == vnWord->length-1) {
                    //revert it
                    vnWord->length--;
                    return REVERTED;
                } else {
                    lastChar->origin = VNCharU;
                    lastChar->transform = IndexShift12;
                    printf("converted to U?\n");
                }
            } //endif
        }
            
        int retVal = ViDoProcessW(vnWord);
        if (retVal != NOTHING) {
            return retVal;
        } //endif
    }
    
    if (ViAppendWord(vnWord, VNCharZero, IndexShift154, capStatus)) {
        return PROCESSED;
    } else {
        return NOTHING;
    } //endif
}

int ViDoProcessAOE(VNWord* vnWord) {
    printf("ViDoProcessAOE\n");
    int i;
    //revert ^ first
    for (i = vnWord->length -1; i>=0; i--) {
        if (vnWord->chars[i].origin == VNCharA
            || vnWord->chars[i].origin == VNCharO
            || vnWord->chars[i].origin == VNCharE) {
                if (vnWord->chars[i].transform == IndexShift12) {
                    vnWord->chars[i].transform = IndexShift0;
                    printf("reverted ^\n");
                    return REVERTED;
                }
            }
    } //end for
    
    int retVal = ViDoCharTransform(sCurrentWord, VNCharO, IndexShift12);
    if (retVal != NOTHING) return retVal;
    retVal = ViDoCharTransform(sCurrentWord, VNCharA, IndexShift12);
    if (retVal != NOTHING) return retVal;
    return ViDoCharTransform(sCurrentWord, VNCharE, IndexShift12);    
}

void ViCorrection(VNWord* vnWord) {
    printf("ViCorrection\n");
    //currently doing nothing
    // int i;
    // for (i=1; i< vnWord->length; i++) {        
    //     VNChar* thisChar = &(vnWord->chars[i]);
    //     VNChar* preChar = &(vnWord->chars[i-1]);
        
    //     // check uo
    //     if (preChar->origin == VNCharU && thisChar->origin == VNCharO) {
    //         if (preChar->transform == IndexShift12 || thisChar->transform == IndexShift24) {
    //             preChar->transform = IndexShift12;
    //             thisChar->transform = IndexShift24;
    //             printf("Corrected uo\n");
    //         } 
    //     }
    // }
}



// int ViAppendWordVNChar(VNWord* vnWord, VNChar* vnChar) {
//     if (vnWord->length >=WORD_MAX_LENGTH) {
//         return VNFalse;
//     }
    
//     vnWord->chars[vnWord->length++] = (*vnChar);
//     return VNTrue;
// }

/*
 * Do transform a character
 */
// int ViCharTransform(VNChar* vnChar, UChar transformInfo[3]) {
//     if (vnChar->transform == transformInfo[1]) {
//         //revert since this transform is already used
//         vnChar->transform = IndexShift0;
//         return REVERTED;
//     } else {
//         //do a transform overridely
//         vnChar->transform = transformInfo[1];
//         return PROCESSED;
//     }    
// }

/*
 * find charset index of a character
 * return -1 if cannot find any
 */
int ViFindCharsetIndex(UChar keyCode) {
    int i;
    for (i = 0; i < CharSetIndexMax; i++) {
        if (CharSetIndex[i][0] == keyCode) {
            return CharSetIndex[i][1];
        }
    }
    return -1;
}

/*
 * Convert a VNChar -> a widechar (UTF8)
 */
wchar_t VNCharToWChar(VNChar* vnChar, UChar wordTransformShift) {
    int index = ViFindCharsetIndex(vnChar->origin);
    if (index >= 0) {
        if (vnChar->origin == VNCharD) {
            index += vnChar->transform;
        } else {
            index += vnChar->transform;
            index += wordTransformShift;
        }
        if (! vnChar->isUpper) {
            index += 1;
        }
        return (wchar_t)UnicodeVNCharset[index];            
    }

    if (vnChar->isUpper) {
        return (wchar_t)toupper(vnChar->origin);
    } else {
        return (wchar_t)vnChar->origin;        
    }
}

/**
 * find the type of trigger when received a key
 */
 int ViFindTriggerType(UChar keyCode) {
     int i;
     for (i = 0; i < sRulesNum; i++) {
         if (sRules[i][0] == keyCode) {
             return sRules[i][1];
         }
     }
     return -1;
 }
 
 /**
  * do word transform
  */
int ViDoWordTransform(VNWord *vnWord, UChar transform) {
    printf("ViDoWordTransform\n");
    if (ViCanTransformWord(vnWord)) {
        if (vnWord->transform == transform) {
            //revert because this transform is already applied
            printf("reverted\n");
            vnWord->transform = IndexShift0;
            return REVERTED;
        } else {
            sCurrentWord->transform = transform;
            return PROCESSED;
        }
    } else {
        return NOTHING;
    }
}

/*********************************************************/
//public methods
/*********************************************************/
void ViResetEngine() {
    sCurrentWord->length = 0;
    sCurrentWord->transform = IndexShift0;
}

void ViInitEngine() {
    printf("VNIMInitVNIM\n");
    sCurrentWord = (VNWord*)malloc(sizeof(VNWord));
    ViResetEngine();
    
    //sRules = sTelexRules;
    //sRulesNum = sTelexRulesNum;
    sRules = sVNIRules;
    sRulesNum = sVNIRulesNum;
    
    //printf("sizeall %d, size 1 %d\n",sizeof sRules,sizeof(sRules[0]));
    printf("sRulesNum = %d\n",sRulesNum); 
    
}

void ViDestroyEngine() {
    free(sCurrentWord);
    sCurrentWord = NULL;
}

void ViGetCurrentWord(wchar_t* outBuffer, int* outLength) {
    (*outLength) = sCurrentWord->length;
    
    if (sCurrentWord->length > 0) {
        int needWordTransform = (sCurrentWord->transform != IndexShift0);
        UChar wordTransformShift = 0;
        //process word first
        int i, index;
        for (i = (*outLength) - 1; i >= 0 ; i--) {
            wordTransformShift = 0;
            if (needWordTransform && ViIsVowel(sCurrentWord->chars[i].origin)) {
                wordTransformShift = sCurrentWord->transform;
                printf("adding dau\n");
                if (i > 0 && ViIsVowel(sCurrentWord->chars[i-1].origin)) {
                    wordTransformShift = 0; 
                    
                    if (i + 1< (*outLength)) {
                        wordTransformShift = sCurrentWord->transform;
                    }
                    
                    //GI
                    if (i > 1 && sCurrentWord->chars[i-2].origin == VNCharG && sCurrentWord->chars[i-1].origin == VNCharI) {
                        wordTransformShift = sCurrentWord->transform;
                    }
                    
                    //QU
                    if (i > 1 && sCurrentWord->chars[i-2].origin == VNCharQ && sCurrentWord->chars[i-1].origin == VNCharU) {
                        wordTransformShift = sCurrentWord->transform;
                    }
                }
                if (wordTransformShift > 0) {
                    needWordTransform = VNFalse; //word is transformed
                }
            }
            outBuffer[i] = VNCharToWChar(&(sCurrentWord->chars[i]), wordTransformShift);
        }        
    }
    outBuffer[*outLength] = 0;
}

int ViProcessKey(UChar keyCode, int capStatus) {
    printf("received [%c], capStatus = %d\n",keyCode, capStatus);
    keyCode = tolower(keyCode);
    if (! ViProcessable(keyCode)) {
        return VNFalse; 
    }
    
    printf("processing [%c]\n", keyCode);
    int i,j;
    
    int retVal = NOTHING; 
    switch (ViFindTriggerType(keyCode)) {
        case VNTriggerWFull:
            retVal = ViDoProcessWFull(sCurrentWord, capStatus);
            break;
        case VNTriggerW:
            retVal = ViDoProcessW(sCurrentWord);
            break;
        case VNTriggerA:
            retVal = ViDoCharTransform(sCurrentWord, VNCharA, IndexShift12);
            break;
        case VNTriggerD:
            retVal = ViDoCharTransform(sCurrentWord, VNCharD, IndexShift2);
            break;
        case VNTriggerE:
            retVal = ViDoCharTransform(sCurrentWord, VNCharE, IndexShift12);
            break;
        case VNTriggerO:
            retVal = ViDoCharTransform(sCurrentWord, VNCharO, IndexShift12);
            break;
        case VNTriggerS:
            retVal = ViDoWordTransform(sCurrentWord, IndexShift2);
            break;
        case VNTriggerF:
            retVal = ViDoWordTransform(sCurrentWord, IndexShift4);
            break;
        case VNTriggerR:
            retVal = ViDoWordTransform(sCurrentWord, IndexShift6);
            break;
        case VNTriggerX:
            retVal = ViDoWordTransform(sCurrentWord, IndexShift8);
            break;
        case VNTriggerJ:
            printf("VNTriggerJ\n");
            retVal = ViDoWordTransform(sCurrentWord, IndexShift10);
            break;
        case VNTriggerZ:
            retVal = ViDoWordTransform(sCurrentWord, IndexShift0);
            break;
        case VNTriggerAEO:
            retVal = ViDoProcessAOE(sCurrentWord);
            break;
        case VNTriggerAW:
            retVal = ViDoCharTransform(sCurrentWord, VNCharA, IndexShift24);
            break;
        case VNTriggerOUW:
            retVal = ViDoProcessOUW(sCurrentWord);
            break;
    }
    
    return (retVal == PROCESSED || ViAppendWord(sCurrentWord, keyCode, IndexShift0, capStatus));
}

/*
 * process backspace
 */
int ViProcessBackspace() {
    if (sCurrentWord->length > 0){
        sCurrentWord->length--;
        if (sCurrentWord->transform != IndexShift0 && (!ViCanTransformWord(sCurrentWord))){
            sCurrentWord->transform = IndexShift0;
        }
        return VNTrue;
    } else {
        return VNFalse;
    }
}
