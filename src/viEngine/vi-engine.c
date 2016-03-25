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
    );
}

/**
 * Check if this is a latin character or not
 */
int ViIsLatinChar(UChar keyCode) {
    if (keyCode >= VNCharA && keyCode <= VNCharZ) {
        return VNTrue;
    } else {
        printf("keyCode %c, %c, %c\n",keyCode,VNCharA,VNCharZ);
        return VNFalse;
    }
}

/**
 * Check if this character can be used to transform other characters
 */
int ViIsCharTransformer(UChar keyCode) {
    int i;
    for (i = 0; i < sCharTransformNumber; i++) {
        // printf("checking char index %d\n",i);
        if (sCharTransformIndex[i][0] == keyCode) {
            return VNTrue;
        }
    }
    
    return VNFalse;
}

/*
 * Check if this character can be used to transform a word
 */
int ViIsWordTransformer(UChar keyCode) {
    int i;
    for (i = 0; i < sWordTransformNumber; i++) {
        if (sWordTransformIndex[i][0] == keyCode) {
            return VNTrue;
        }            
    }     
}

/*
 * Check if this can be used to transform
 */
int ViIsATransformer(UChar keyCode) {
    //check if this keyCode is in the transformIndex
    return (ViIsCharTransformer(keyCode) || ViIsWordTransformer(keyCode));
}

/*
 * Apend a character into a word
 */
int ViAppendWord(VNWord* vnWord, UChar keyCode, int capStatus) {
    printf("ViAppendWord [%c]\n", keyCode);
    if ((! ViIsLatinChar(keyCode)) || (vnWord->length >= WORD_MAX_LENGTH)) {
        return VNFalse;
    }
    
    VNChar* vnChar = &(vnWord->chars[vnWord->length++]);
    vnChar->origin = keyCode;
    vnChar->transform = CharTransform0;
    vnChar->isUpper = capStatus?(VNTrue):(VNFalse);    
    return VNTrue;
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
int VNIMCharTransform(VNChar* vnChar, UChar transformInfo[3]) {
    if (transformInfo[2] != vnChar->origin) {
        //transform char does not match
        return NOTHING;
    }
    
    if (vnChar->transform == transformInfo[1]) {
        //revert since this transform is already used
        vnChar->transform = CharTransform0;
        return REVERTED;
    } else {
        //do a transform overridely
        vnChar->transform = transformInfo[1];
        return PROCESSED;
    }    
}

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
            index += vnChar->transform * 2;
        } else {
            index += vnChar->transform * 12;
            index += wordTransformShift * 2;
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

//public methods
void ViResetEngine() {
    sCurrentWord->length = 0;
    sCurrentWord->transform = WordTransform0;
}

void ViInitEngine() {
    printf("VNIMInitVNIM\n");
    sCurrentWord = (VNWord*)malloc(sizeof(VNWord));
    ViResetEngine();
    
    sWordTransformIndex =TelexWordTransformIndex;
    sWordTransformNumber = TelexWordTransformNumber;
    sCharTransformIndex = TelexCharTransformIndex;
    sCharTransformNumber = TelexCharTransformNumber;
}

void ViDestroyEngine() {
    free(sCurrentWord);
    sCurrentWord = NULL;
}

int ViGetCurrentWord(wchar_t* outBuffer, int* outLength) {
    (*outLength) = sCurrentWord->length;
    int needWordTransform = (sCurrentWord->transform != WordTransform0);
    UChar wordTransformShift = 0;
    //process word first
    int i, index;
    for (i = (*outLength) - 1; i >= 0 ; i--) {
        if (needWordTransform && ViIsVowel(sCurrentWord->chars[i].origin)) {
            wordTransformShift = sCurrentWord->transform;
            needWordTransform = 0;
            outBuffer[i] = VNCharToWChar(&(sCurrentWord->chars[i]), wordTransformShift);
        } else {
            outBuffer[i] = VNCharToWChar(&(sCurrentWord->chars[i]), 0);
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
    
    // a char transformer?
    if (ViIsCharTransformer(keyCode)) {
        for (i = sCurrentWord->length-1; i>=0; i--) {
            printf("checking %d\n",i);
            if (ViCanTransform(&(sCurrentWord->chars[i]))) {
                for (j = 0; j < sCharTransformNumber; j++ ) {
                    if (sCharTransformIndex[j][0] == keyCode) {
                        int retVal = VNIMCharTransform(&(sCurrentWord->chars[i]), sCharTransformIndex[j]);
                        if (retVal == REVERTED) {
                            if (ViAppendWord(sCurrentWord, keyCode, capStatus)) {                               
                                return VNTrue;
                            } else {
                                return VNFalse;
                            }
                        }  else if (retVal == PROCESSED) {
                            //transform done
                            return VNTrue;
                        } //endif
                    } //endif
                } //endfor
            } //endif
        } //endfor
        
        // //still cannot transform, check for w-> u*
        // for (i = 0; i < sCharTransformNumber; i++) {
        //     if (sCharTransformIndex[i][0] == keyCode && sCharTransformIndex[i][1] == CharTransform5) {
        //         VNIMAppendWord(sCurrentWord, VNCharU, capStatus);
        //         return VNTrue;   
        //     }
        // } //endfor
    } else if (ViIsWordTransformer(keyCode)) {
        for (i = 0; i < sWordTransformNumber; i++) {
            if (sWordTransformIndex[i][0] == keyCode) {
                if (sCurrentWord->transform == sWordTransformIndex[i][1]) {
                    //revert
                    sCurrentWord->transform = WordTransform0;
                    if (ViAppendWord(sCurrentWord, keyCode, capStatus)) {
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
        
    if (ViAppendWord(sCurrentWord, keyCode, capStatus)) {
        return VNTrue;
    }
    
    return VNFalse;
}

/*
 * process backspace
 */
int ViProcessBackspace() {
    if (sCurrentWord->length > 0){
        sCurrentWord->length--;
        return VNTrue;
    } else {
        return VNFalse;
    }
}
