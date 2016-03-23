#include <stddef.h>
// #include <X11/Xlocale.h>
// #include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
// #include "IMdkit.h"
    // #include <Xi18n.h>
#include "unikey_xim.h"

//ukengine
#include "unikey.h"
#include "wchar.h"

#define BUFFER_LENTH 1024
static wchar_t * preEditText = NULL;
static int preEditLength = 0;
static int preEditAction = PREEDIT_ACTION_NONE;

int getPreEditAction() {
    return preEditAction;  
}

const wchar_t * getPreEditText() {
    // strcpy(preEditText, "Bong");
    return preEditText;
}

void handleEngineResult() {
    printf("handleEngineResult %d, %d\n",UnikeyBackspaces, UnikeyBufChars);
    if (UnikeyBackspaces > 0) {
        int i;
        for (i = 0; i < UnikeyBackspaces; i++) {
            preEditText[--preEditLength] = 0;
        }
    }
    
    printf("delete done\n");
    
    if (UnikeyBufChars > 0)
    {
        printf("UnikeyBuf = %s\n",UnikeyBuf);
        UnikeyBuf[UnikeyBufChars] = 0;
        static wchar_t buffer[100];
        int newLen = mbstowcs(buffer, UnikeyBuf, 100);
        printf("newLen = %d\n",newLen);
        if (newLen > 0) {
            //memcpy(preEditText + preEditLength, UnikeyBuf, UnikeyBufChars);        
            wcsncpy((wchar_t*)(preEditText + preEditLength), buffer, newLen);
            preEditLength += newLen;
            printf("new length = %d\n",preEditLength);
            preEditText[preEditLength] = 0;
        } else {
            printf("ERROR \n");
            preEditText[preEditLength = 0] = 0;
        }
    }
    printf("handleEngineResult done\n");
}

#define STRBUFLEN 64
void UnikeyProcessKey(XKeyEvent * keyEvent) {
    if ((keyEvent->state & ControlMask) == ControlMask) {
        printf("special key, hot key\n");
        preEditAction = PREEDIT_ACTION_COMMIT_FORWARD;
        return;
    }
    
    
    // printf("ProcessKey\n");
    static char strbuf[STRBUFLEN];
    static unsigned char keyval;
    static KeySym keysym;
    int count;

    // fprintf(stderr, "Processing \n");
    memset(strbuf, 0, STRBUFLEN);
    count = XLookupString(keyEvent, strbuf, STRBUFLEN, &keysym, NULL);
    printf("keysym = %d, keycode = %d\n",keysym,keyEvent->keycode);
    
    if (keysym == XK_BackSpace && preEditLength > 0) {
        printf("backspace pressed\n");
        
        //preediting
        UnikeyBackspacePress();
        handleEngineResult(True);
        
        if (preEditLength > 0) {
            preEditAction = PREEDIT_ACTION_DRAW;
        } else {
            preEditAction = PREEDIT_ACTION_DISCARD;
        }
        return;
    }
    
    if (count > 0) {
        keyval = strbuf[0]; 
        if (keyval > ' ' && keyval <= '~' ) {
            printf("processKey [%c]. shift %d, caplock %d\n", keyval, keyEvent->state & ShiftMask, keyEvent->state & LockMask);
            UnikeySetCapsState(keyEvent->state & ShiftMask, keyEvent->state & LockMask);
            UnikeyFilter(keyval);
            handleEngineResult(False);
            
            if (UnikeyBufChars == 0) {
                //add key manually
                preEditText[preEditLength++] = keyval;
                //swprintf(preEditText, 100, L"%hs", keyval);
            }
            
            wprintf("preEditLength = %d, preEditText = %ls\n", preEditLength, preEditText);
            
            preEditAction = PREEDIT_ACTION_DRAW;
            return;
        }
    }
    
    if (preEditLength > 0) {
        preEditAction = PREEDIT_ACTION_COMMIT_FORWARD;
    } else {
        preEditAction = PREEDIT_ACTION_FORWARD;
    }
}

void UnikeyReset() {
    preEditLength = 0;
    memset(preEditText, 0, BUFFER_LENTH);
    UnikeyResetBuf();
}

void UnikeyCommitDone() {
    UnikeyReset();
}

void UnikeyFocusIn() {
    printf("Focus In\n");
    UnikeyReset();
}

void UnikeyFocusOut() {
    printf("Focus out\n");
    UnikeyReset();
}

void UnikeyInit() {
    if (preEditText == NULL) {
        preEditText = (wchar_t *)malloc(sizeof(wchar_t) * BUFFER_LENTH);
    }
    
    UnikeySetup();  
}

void UnikeyDestroy() {
    free(preEditText);
    preEditText = NULL;
}