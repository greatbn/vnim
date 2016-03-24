#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "vnim.h"
#include "vnim_private.h"
#include <stddef.h> //testing
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include "unikey_xim.h"

//ukengine
#include "wchar.h"

#define BUFFER_LENGTH 100

#define PROCESSED (1)
#define REVERTED (2)
#define NOTHING (0)

static VNWord* sCurrentWord;
static UChar (*sWordTransformIndex)[2];
static int sWordTransformNumber;
static UChar (*sCharTransformIndex)[2];
static int sCharTransformNumber;

void VNIMResetWord() {
    sCurrentWord->length = 0;
    sCurrentWord->transform = WordTransformMax;
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
    vnChar->transform = CharTransformMax;
    vnChar->isUpper = capStatus?(VNTrue):(VNFalse);    
    return VNTrue;
}

int VNIMProcessable(UChar keyCode) {
    return (VNIMIsVNChar(keyCode) || VNIMIsATransformer(keyCode));        
}

int VNIMCharTransform(VNChar* vnChar, UChar transform) {
    if (vnChar->transform == transform) {
        //revert
        vnChar->transform = CharTransformMax;
        return REVERTED;
    }
    
    switch (transform) {
        case CharTransform1:
            if (vnChar->origin == VNCharA) {
                vnChar->transform = transform;
                return PROCESSED;
            }
            return NOTHING;
        case CharTransform2:
            if (vnChar->origin == VNCharA
                || vnChar->origin == VNCharE
                || vnChar->origin == VNCharO) {
                    vnChar->transform = transform;
                    return PROCESSED;
                }
            return NOTHING;
        case CharTransform3:
            if (vnChar->origin == VNCharO
                || vnChar->origin == VNCharU) {
                    vnChar->transform == transform;
                    return PROCESSED;
                }
            return NOTHING;
        case CharTransform4:
            if (vnChar->origin == VNCharD) {
                vnChar->transform == transform;
                return PROCESSED;
            }
            return NOTHING;            
    }
    return NOTHING;
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
                        int retVal = VNIMCharTransform(&(sCurrentWord->chars[i]), sCharTransformIndex[j][1]);
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
        
        //still cannot transform, check for w-> u*
        for (i = 0; i < sCharTransformNumber; i++) {
            if (sCharTransformIndex[i][0] == keyCode && sCharTransformIndex[i][1] == CharTransform5) {
                VNIMAppendWord(sCurrentWord, VNCharU, capStatus);
                return VNTrue;   
            }
        } //endfor
    } else if (VNIMIsWordTransformer(keyCode)) {
        for (i = 0; i < sWordTransformNumber; i++) {
            if (sWordTransformIndex[i][0] == keyCode) {
                if (sCurrentWord->transform == sWordTransformIndex[i][1]) {
                    //revert
                    sCurrentWord->transform = WordTransformMax;
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


//testing
static int preEditAction;
static wchar_t preEditText[100];
int getPreEditAction() {
    return preEditAction;  
}

const wchar_t* getPreEditText() {
    return &preEditText;
}

#define STRBUFLEN 100
void UnikeyProcessKey(XKeyEvent * keyEvent) {
        // printf("ProcessKey\n");
    static char strbuf[STRBUFLEN];
    static unsigned char keyval;
    static KeySym keysym;
    int count;

    // fprintf(stderr, "Processing \n");
    memset(strbuf, 0, STRBUFLEN);
    count = XLookupString(keyEvent, strbuf, STRBUFLEN, &keysym, NULL);
    printf("keysym = %d, keycode = %c, modifiers = %d\n",keysym,keyEvent->keycode, keyEvent->state);    
    
    if ((keyEvent->state & ControlMask) || (keyEvent->state & Mod1Mask)) {
        printf("special key, hot key\n");
        preEditAction = PREEDIT_ACTION_FORWARD;
        return;
    }
    
    
    if (count > 0) {
        keyval = strbuf[0]; 
        if (keyval >= ' ' && keyval <= '~' ) {
            printf("processKey [%c]. shift %d, caplock %d\n", keyval, keyEvent->state & ShiftMask, keyEvent->state & LockMask);
            int capStatus = ((keyEvent->state & ShiftMask) && (keyEvent->state & LockMask));
            int retVal = VNIMProcessKey(keyval, capStatus);
            
            int i;
            for (i =0; i< sCurrentWord->length; i++) {
                preEditText[i] = sCurrentWord->chars[i].origin;
            }
            preEditText[sCurrentWord->length] = 0;
            printf("assign done: %d\n",sCurrentWord->length);
            
            wprintf("preEditLength = %d, preEditText = %ls\n", sCurrentWord->length, preEditText);
            if (retVal == VNFalse) {
                preEditAction = PREEDIT_ACTION_COMMIT_FORWARD;
            } else {
                preEditAction = PREEDIT_ACTION_DRAW;
            } 
            return;
        }
    }
    return preEditAction = PREEDIT_ACTION_FORWARD;    
}

void UnikeyCommitDone() {
    VNIMResetWord();
}

void UnikeyFocusIn() {
    VNIMResetWord();
}

void UnikeyFocusOut() {
    VNIMResetWord();    
}

void UnikeyInit() {
    VNIMInitVNIM();
}
void UnikeyDestroy() {
    VNIMDestroyVNIM();
}