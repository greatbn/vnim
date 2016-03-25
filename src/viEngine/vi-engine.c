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
static UChar (*sWordTransformIndex)[2];
static int sWordTransformNumber;
static UChar (*sCharTransformIndex)[3];
static int sCharTransformNumber;

void VNIMResetWord() {
    sCurrentWord->length = 0;
    sCurrentWord->transform = WordTransform0;
}

void VNIMInitVNIM() {
    printf("VNIMInitVNIM\n");
    sCurrentWord = (VNWord*)malloc(sizeof(VNWord));
    VNIMResetWord();
    
    sWordTransformIndex =TelexWordTransformIndex;
    sWordTransformNumber = TelexWordTransformNumber;
    sCharTransformIndex = TelexCharTransformIndex;
    sCharTransformNumber = TelexCharTransformNumber;
}

void VNIMDestroyVNIM() {
    free(sCurrentWord);
    sCurrentWord = NULL;
}

int VNIMCanTransform(VNChar* vnChar) {
    return (
        vnChar->origin == VNCharA
        || vnChar->origin == VNCharD
        || vnChar->origin == VNCharE
        || vnChar->origin == VNCharO
        || vnChar->origin == VNCharU
    );
}

int VEIsVowel(UChar keyCode) {
    return (
        keyCode == VNCharA
        || keyCode == VNCharE
        || keyCode == VNCharO
        || keyCode == VNCharU
        || keyCode == VNCharI
        || keyCode == VNCharY
    );
}

int VNIMIsVNChar(UChar keyCode) {
    if (keyCode >= VNCharA && keyCode <= VNCharZ) {
        return VNTrue;
    } else {
        printf("keyCode %c, %c, %c\n",keyCode,VNCharA,VNCharZ);
        return VNFalse;
    }
}

int VNIMIsCharTransformer(UChar keyCode) {
    printf("VNIMIsCharTransformer\n");
    int i;
    for (i = 0; i < sCharTransformNumber; i++) {
        // printf("checking char index %d\n",i);
        if (sCharTransformIndex[i][0] == keyCode) {
            return VNTrue;
        }
    }
    
    return VNFalse;
}

int VNIMIsWordTransformer(UChar keyCode) {
    printf("VNIMIsWordTransformer\n");
    int i;
    for (i = 0; i < sWordTransformNumber; i++) {
        if (sWordTransformIndex[i][0] == keyCode) {
            return VNTrue;
        }            
    }     
}

int VNIMIsATransformer(UChar keyCode) {
    //check if this keyCode is in the transformIndex
    return (VNIMIsCharTransformer(keyCode) || VNIMIsWordTransformer(keyCode));
}

int VNIMAppendWord(VNWord* vnWord, UChar keyCode, int capStatus) {
    printf("VNIMAppendWord\n");
    if ((! VNIMIsVNChar(keyCode)) || (vnWord->length > 8)) {
        printf("why return false?\n");
        return VNFalse;
    }
    
    VNChar* vnChar = &(vnWord->chars[vnWord->length++]);
    vnChar->origin = keyCode;
    vnChar->transform = CharTransform0;
    vnChar->isUpper = capStatus?(VNTrue):(VNFalse);    
    return VNTrue;
}

int VNIMProcessable(UChar keyCode) {
    return (VNIMIsVNChar(keyCode) || VNIMIsATransformer(keyCode));        
}

int VNIMCharTransform(VNChar* vnChar, UChar transformInfo[3]) {
    if (transformInfo[2] != vnChar->origin) {
        //transform char does not match
        return NOTHING;
    }
    
    if (vnChar->transform == transformInfo[1]) {
        //revert
        vnChar->transform = CharTransform0;
        return REVERTED;
    } else {
        vnChar->transform = transformInfo[1];
        return PROCESSED;
    }    
}

int VNIMProcessBackspace() {
    if (sCurrentWord->length > 0){
        sCurrentWord->length--;
        return VNTrue;
    } else {
        return VNFalse;
    }
}

int VNIMProcessKey(UChar keyCode, int capStatus) {
    printf("received [%c], capStatus = %d\n",keyCode, capStatus);
    keyCode = tolower(keyCode);
    printf("processing [%c]\n", keyCode);
    if (! VNIMProcessable(keyCode)) {
        return VNFalse; 
    }
    
    printf("processing [%c]\n", keyCode);
    int i,j;
    
    // a char transformer?
    if (VNIMIsCharTransformer(keyCode)) {
        for (i = sCurrentWord->length-1; i>=0; i--) {
            printf("checking %d\n",i);
            if (VNIMCanTransform(&(sCurrentWord->chars[i]))) {
                for (j = 0; j < sCharTransformNumber; j++ ) {
                    if (sCharTransformIndex[j][0] == keyCode) {
                        int retVal = VNIMCharTransform(&(sCurrentWord->chars[i]), sCharTransformIndex[j]);
                        if (retVal == REVERTED) {
                            if (VNIMAppendWord(sCurrentWord, keyCode, capStatus)) {                               
                                return VNTrue;
                            } else {
                                return VNFalse;
                            }
                        }  else if (retVal == PROCESSED) {
                            //transform done
                            return VNTrue;                        
                        } else {
                            
                        }
                    }                
                }            
            }
        } //endfor
        
        // //still cannot transform, check for w-> u*
        // for (i = 0; i < sCharTransformNumber; i++) {
        //     if (sCharTransformIndex[i][0] == keyCode && sCharTransformIndex[i][1] == CharTransform5) {
        //         VNIMAppendWord(sCurrentWord, VNCharU, capStatus);
        //         return VNTrue;   
        //     }
        // } //endfor
    } else if (VNIMIsWordTransformer(keyCode)) {
        for (i = 0; i < sWordTransformNumber; i++) {
            if (sWordTransformIndex[i][0] == keyCode) {
                if (sCurrentWord->transform == sWordTransformIndex[i][1]) {
                    //revert
                    sCurrentWord->transform = WordTransform0;
                    if (VNIMAppendWord(sCurrentWord, keyCode, capStatus)) {
                        return VNTrue;    
                    } else {
                        return VNFalse;
                    }
                } else {
                    sCurrentWord->transform = sWordTransformIndex[i][1];
                    return VNTrue;
                }
            }
        }
    } //endif
        
    if (VNIMAppendWord(sCurrentWord, keyCode, capStatus)) {
        return VNTrue;
    }
    
    return VNFalse;    
}

int FindCharsetIndex(UChar keyCode) {
    int i;
    for (i = 0; i < CharSetIndexMax; i++) {
        if (CharSetIndex[i][0] == keyCode) {
            return CharSetIndex[i][1];
        }
    }
    return -1;
}

wchar_t VNCharToWChar(VNChar* vnChar, UChar wordTransformShift) {
    int index = FindCharsetIndex(vnChar->origin);
    if (index >= 0) {
        if (vnChar->origin == VNCharD) {
            index += vnChar->transform * 2;
        } else {
            index += vnChar->transform * 12;
            index += wordTransformShift * 2;
        }
        if (! vnChar->isUpper) {
            index += 1;
        }
        return UnicodeVNCharset[index];            
    }
    printf("not converting [%c]\n",vnChar->origin);
    if (vnChar->isUpper) {
        return (wchar_t)toupper(vnChar->origin);
    } else {
        return (wchar_t)vnChar->origin;        
    }
}

int VNIMConvertWordToWChar(wchar_t* outBuffer, int* outLength) {
    (*outLength) = sCurrentWord->length;
    int needWordTransform = (sCurrentWord->transform != WordTransform0);
    UChar wordTransformShift = 0;
    //process word first
    int i, index;
    for (i = 0; i < *outLength; i++) {
        if (needWordTransform && VEIsVowel(sCurrentWord->chars[i].origin)) {
            wordTransformShift = sCurrentWord->transform;
            needWordTransform = 0;
            outBuffer[i] = VNCharToWChar(&(sCurrentWord->chars[i]), wordTransformShift);
        } else {
            outBuffer[i] = VNCharToWChar(&(sCurrentWord->chars[i]), 0);
        }
    }
    outBuffer[*outLength] = 0;    
    // for (i =0; i< sCurrentWord->length; i++) {
    //     outBuffer[i] = sCurrentWord->chars[i].origin;
    // }    
}