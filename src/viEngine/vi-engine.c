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

/*
 * Check if this can be used to transform
 */
int ViIsATransformer(UChar keyCode) {
    // printf("ViIsATransformer\n");
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
VNBoolean ViProcessable(UChar keyCode) {
    return (ViIsLatinChar(keyCode) || ViIsATransformer(keyCode));        
}

int ViDoCharRevert(VNWord* word, UChar keyCode, UChar transform) {
    int i;
    for (i=word->length -1; i>=0; i--) {
        if (word->chars[i].origin == keyCode && word->chars[i].transform == transform) {
            word->chars[i].transform = IndexShift0;
            return REVERTED;
        }
    }
    return NOTHING;
}

/*
 * Do transform a character
 */

int ViDoCharRevertTransform(VNWord* word, int transformsNum, UChar transformList[][2]) {
    printf("ViDoCharRevertTransform\n");
    int i,j;
    //revert first
    for (i =0; i<transformsNum; i++) {
        if (ViDoCharRevert(word, transformList[i][0], transformList[i][1]))
            return REVERTED;  //return false to let other handle the character
    }
    
    //transform
    for (i=word->length -1; i > word->wordBreak; i--){
        for (j=0; j < transformsNum; j++) {
            if (word->chars[i].origin == transformList[j][0]) {
                word->chars[i].transform = transformList[j][1];
                // printf("transformed %d\n", transformList[j][1]);
                return PROCESSED;
            }
        }
    }
    
    return NOTHING;
}

void ViCorrection(VNWord* vnWord) {
    printf("ViCorrection\n");
    //currently doing nothing
    int i;
    for (i=1; i< vnWord->length; i++) {        
        VNChar* thisChar = &(vnWord->chars[i]);
        VNChar* preChar = &(vnWord->chars[i-1]);
        
    //     // check oa
        if (preChar->origin == VNCharU && thisChar->origin == VNCharA) {
            if (thisChar->transform == IndexShift24) {
                preChar->transform = IndexShift12;
                thisChar->transform = IndexShift0;
                printf("Corrected oa\n");
            } 
        }
    }
}

/*
 * Apend a character into a word
 */
VNBoolean ViAppendWord(VNWord* vnWord, UChar keyCode, UChar transform, int capStatus) {
    printf("ViAppendWord [%c]\n", keyCode);
    if ((vnWord->length >= WORD_MAX_LENGTH)) {
        return VNFalse;
    }
    
    VNChar* vnChar = &(vnWord->chars[vnWord->length++]);
    vnChar->origin = keyCode;
    vnChar->transform = transform;
    vnChar->isUpper = capStatus?(VNTrue):(VNFalse);
    return VNTrue;
}

int ViDoProcessUO(VNWord* vnWord) {
    int i;
    //process u*o*
    for (i=vnWord->length-1; i > 0; i--) {
        if (vnWord->chars[i].origin == VNCharO && vnWord->chars[i-1].origin == VNCharU) {
            if (vnWord->chars[i-1].transform == IndexShift12 && vnWord->chars[i].transform == IndexShift24) {
                //revert
                vnWord->chars[i-1].transform = IndexShift0;
                vnWord->chars[i].transform = IndexShift0;
                return REVERTED;
            } else if (i > vnWord->wordBreak) {
                vnWord->chars[i-1].transform = IndexShift12;
                vnWord->chars[i].transform = IndexShift24;
                return PROCESSED;                
            }
        }
    }
    return NOTHING;    
}
int ViDoProcessOUW(VNWord* vnWord) {
    int i;
    //process u*o*
    switch (ViDoProcessUO(vnWord)){
        case REVERTED:
            return REVERTED;
        case PROCESSED:
            return PROCESSED;
    }
    
    UChar params [][2]= {{VNCharO, IndexShift24},{VNCharU,IndexShift12}};
    return ViDoCharRevertTransform(vnWord, 2, params);
}

/**
 * Do process w
 */
 VNBoolean ViDoProcessW(VNWord* vnWord) {
    printf("ViDoProcessW\n");
    //process u*o*
    switch (ViDoProcessUO(vnWord)){
        case REVERTED:
            return REVERTED;
        case PROCESSED:
            return PROCESSED;
    }
    
    //return ViDoCharTransform(vnWord, VNCharA, IndexShift24);
    UChar params [][2] = {{VNCharO,IndexShift24},{VNCharU, IndexShift12},{VNCharA, IndexShift24}};
    return ViDoCharRevertTransform(vnWord, 3, params);     
 }
 
int ViDoProcessWFull(VNWord* vnWord, int capStatus) {
    printf("ViDoProcessWFull\n");
    int i;
    if (vnWord->length > 0) {
        //revert w->u* first
        for (i = vnWord->length -1; i >= 0; i--) {
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
            
        switch (ViDoProcessW(vnWord)){
            case REVERTED:
                return REVERTED;
            case PROCESSED:
                return PROCESSED;
        }
    }
    
    if (ViAppendWord(vnWord, VNCharZero, IndexShift154, capStatus)) {
        return PROCESSED;
    } //endif
    
    return NOTHING;
}

int ViDoProcessAOE(VNWord* vnWord) {
    printf("ViDoProcessAOE\n");

    UChar params[][2] = {{VNCharA, IndexShift12},{VNCharO, IndexShift12},{VNCharE, IndexShift12}};
    return ViDoCharRevertTransform(vnWord, 3, params);
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
    }
    
    return NOTHING;
}

void ViUpdateWordBreak(VNWord *word) {
    int i = word->length-1;
    while (i >= 0 && !ViIsVowel(word->chars[i--].origin));
    while (i >= 0 && ViIsVowel(word->chars[i--].origin));
    word->wordBreak = i;
    printf("wordBreak = %d\n",word->wordBreak);
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
    
    SetInputEngine(TELEX_INPUT);
    
    //printf("sizeall %d, size 1 %d\n",sizeof sRules,sizeof(sRules[0]));
    printf("sRulesNum = %d\n",sRulesNum); 
    
}

void ViDestroyEngine() {
    free(sCurrentWord);
    sCurrentWord = NULL;
}

void SetInputEngine(int input) {
    if (input == VNI_INPUT) {
        sRules = sVNIRules;
        sRulesNum = sVNIRulesNum;        
    } else if (input == TELEX_INPUT) {
        sRules = sTelexRules;
        sRulesNum = sTelexRulesNum;        
    }
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
                
                if ((sCurrentWord->chars[i].transform > 0)
                    || (i + 1< (*outLength))
                    || (i > 1 && sCurrentWord->chars[i-2].origin == VNCharG && sCurrentWord->chars[i-1].origin == VNCharI) //GI
                    || (i > 1 && sCurrentWord->chars[i-2].origin == VNCharQ && sCurrentWord->chars[i-1].origin == VNCharU)){ //QU 
                    //this is a transformed letter
                } else if (i > 0 && ViIsVowel(sCurrentWord->chars[i-1].origin)) {
                    wordTransformShift = 0; 
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
    ViUpdateWordBreak(sCurrentWord);
    int i,j;
    
    int retVal = VNFalse; 
    switch (ViFindTriggerType(keyCode)) {
        case VNTriggerWFull:
            retVal = ViDoProcessWFull(sCurrentWord, capStatus);
            break;
        case VNTriggerW:
            retVal = ViDoProcessW(sCurrentWord);
            break;
        case VNTriggerA:
        {
            UChar params[][2] ={{VNCharA, IndexShift12}};
            retVal = ViDoCharRevertTransform(sCurrentWord, 1, params);
            break;
        }
        case VNTriggerD:
        {
            UChar params[][2] = {{VNCharD, IndexShift2}};
            retVal = ViDoCharRevertTransform(sCurrentWord, 1, params);
            break;
        }
        case VNTriggerE:
        {
            UChar params[][2] = {{VNCharE, IndexShift12}};
            retVal = ViDoCharRevertTransform(sCurrentWord, 1, params);
            break;
        }
        case VNTriggerO:
        {
            UChar params[][2] = {{VNCharO, IndexShift12}};
            retVal = ViDoCharRevertTransform(sCurrentWord, 1, params);
            break;
        }
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
            retVal = ViDoWordTransform(sCurrentWord, IndexShift10);
            break;
        case VNTriggerZ:
            retVal = ViDoWordTransform(sCurrentWord, IndexShift0);
            break;
        case VNTriggerAEO:
            retVal = ViDoProcessAOE(sCurrentWord);
            break;
        case VNTriggerAW:
        {
            UChar params[][2] = {{VNCharA, IndexShift24}};
            retVal = ViDoCharRevertTransform(sCurrentWord, 1, params);
            break;
        }
        case VNTriggerOUW:
            retVal = ViDoProcessOUW(sCurrentWord);
            break;
    }
    
    if (retVal != PROCESSED && ViAppendWord(sCurrentWord, keyCode, IndexShift0, capStatus)) {
        retVal = PROCESSED;
    }
    ViCorrection(sCurrentWord);
    return retVal;
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
