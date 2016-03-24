#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "vnim.h"

#define BUFFER_LENGTH 100

#define PROCESSED (1)
#define REVERTED (2)
#define NOTHING (0)

static VNChar* sBuffer;
static int sBufferLength;
static UChar** sTransformIndex;
static int sTransformNumber;

void VNIMInitVNIM() {
    sBuffer = (VNChar*)malloc(sizeof(VNChar) * BUFFER_LENGTH);
    sBufferLength = 0;
    sTransformIndex = TelexTransformIndex;
    sTransformNumber = TelexTransformNumber;
}

void VNIMResetBuffer() {
    sBufferLength = 0;
}

void VNIMDestroyBuffer() {
    free(sBuffer);
    sBuffer = NULL;
}

int VNIMCanTransform(UChar keyCode) {
    return (
        keyCode == VNCharA
        || keyCode == VNCharD
        || keyCode == VNCharE
        || keyCode == VNCharI
        || keyCode == VNCharO
        || keyCode == VNCharU
        || keyCode == VNCharY
    );
}

void VNIMInitVNChar(VNChar* vnChar, UChar keyCode, int capStatus) {
    vnChar->firstTrans = TransformMax;
    vnChar->secondTrans = TransformMax;
    if (capStatus == True) {
        vnChar->zeroTrans = TransformUpper;
    } else {
        vnChar->zeroTrans = TransformLower;
    }
    vnChar->origin = keyCode;
}

int VNIMProcessable(UChar keyCode) {
    if (keyCode >= VNCharA && keyCode <= VNCharZ) return VNTrue;
    
    int i;
    for (i = 0; i < sTransformNumber; i++) {
        if (sTransformIndex[i][0] == tolower(keyCode)) {
            return VNTrue;
        }            
    } 
    return VNFalse;
}

int VNIMTransform(VNChar* vnChar, UChar transform) {
    if (transform > Transform20) {
        if (vnChar->secondTrans == transform) {
            //revert
            vnChar->secondTrans = TransformMax;
            return REVERTED;
        } else {
            if (vnChar->origin != VNCharD) {
                vnChar->secondTrans = transform;
                return PROCESSED;                
            } else {
                return NOTHING;
            }
        }
    } else if (transform < Transform20) {
        if (vnChar->firstTrans == transform) {
            //revert
            vnChar->firstTrans = TransformMax;
            return REVERTED;
        }
        
        switch (transform) {
            case Transform11:
                if (vnChar->origin == 'a') {
                    vnChar->firstTrans = transform;
                    return VNTrue;
                }
            case Transform12:
                if (vnChar->origin == 'a'
                    || vnchar->origin == 'e')
                
        }
    } else {
        // processing Z
        if (vnChar->secondTrans != TransformMax) {
            //revert
            vnChar->secondTrans = TransformMax;
            return PROCESSED;
        } else {
            return NOTHING;
        }
    }
}

int VNIMProcessKey(UChar keyCode, int capStatus) {
    keyCode = tolower(keyCode);
    if (! VNIMProcessable(keyCode)) {
        return VNFalse; 
    }
    
    printf("process [%c]\n",keyCode);
    UChar transformType = TransformMax;
    int i,j;
    
    for (i = sBufferLength-1; i>=0; i--) {
        if (VNIMCanTransform(keyCode) == ) {
            for (j = 0; j < sTransformNumber; j++ ) {
                if (sTransformIndex[j][0] == keyCode) {
                    int retVal = VNIMTransform(&sBuffer[i], sTransformIndex[j][1]);
                    if (retVal == REVERTED) {
                        VNIMInitVNChar(sBuffer[sBufferLength++], keyCode));
                        return VNTrue;
                    }  else if (retVal == PROCESSED) {
                        //transform done
                        return VNTrue;                        
                    } else {
                        
                    }
                }                
            }            
        }
    }
    
    //still cannot transform, check for w-> u*
    for (i = 0; i < TransformNumber; i++) {
        if (sTransformIndex[i][0] == keyCode && sTransformIndex[i][1] == Transform30) {
            VNIMInitVNChar(sBuffer[sBufferLength++], VNCharU));
            return VNTrue;   
        }
    }
    
    if (keyCode >= VNCharA && keyCode <= VNCharZ) {
        VNIMInitVNChar(sBuffer[sBufferLength++], keyCode);
        return VNTrue;
    }
    
    return VNFalse;    
}