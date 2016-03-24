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

//charset output
//#define CONV_CHARSET_UNICODE	0
//#define CONV_CHARSET_UNIUTF8    1
#define CONV_CHARSET_UNIREF     2  //&#D;
#define CONV_CHARSET_UNIREF_HEX 3
//#define CONV_CHARSET_UNIDECOMPOSED 4
//#define CONV_CHARSET_WINCP1258	5
#define CONV_CHARSET_UNI_CSTRING 6
//#define CONV_CHARSET_VNSTANDARD 7

#define CONV_CHARSET_VIQR		10
//#define CONV_CHARSET_UTF8VIQR 11
#define CONV_CHARSET_XUTF8  12

#define CONV_CHARSET_TCVN3		20
//#define CONV_CHARSET_VPS		21
//#define CONV_CHARSET_VISCII		22
//#define CONV_CHARSET_BKHCM1		23
//#define CONV_CHARSET_VIETWAREF	24
//#define CONV_CHARSET_ISC        25

#define CONV_CHARSET_VNIWIN		40
#define CONV_CHARSET_BKHCM2		41
//#define CONV_CHARSET_VIETWAREX	42
//#define CONV_CHARSET_VNIMAC		43

#define BUFFER_LENTH 1024
static wchar_t * preEditText = NULL;
static int preEditLength = 0;
static int preEditAction = PREEDIT_ACTION_NONE;
static Bool engineEnabled = False;

int getPreEditAction() {
    if (preEditAction == PREEDIT_ACTION_FORWARD && preEditLength > 0) {
        return PREEDIT_ACTION_COMMIT_FORWARD;
    }
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
        UnikeyBuf[UnikeyBufChars] = 0;
        static wchar_t buffer[100];
        
        int newLen;
        
        if (1) {
            newLen = mbstowcs(buffer, UnikeyBuf, 100);  
        }  else {
            newLen = UnikeyBufChars;
            for (newLen=0; newLen< UnikeyBufChars; newLen++) {
                buffer[newLen] = UnikeyBuf[newLen];
            }
            buffer[newLen] = 0;
        }
        printf("newLen = %d\n",newLen);
        if (newLen > 0) {
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
    // printf("ProcessKey\n");
    static char strbuf[STRBUFLEN];
    static unsigned char keyval;
    static KeySym keysym;
    int count;

    // fprintf(stderr, "Processing \n");
    memset(strbuf, 0, STRBUFLEN);
    count = XLookupString(keyEvent, strbuf, STRBUFLEN, &keysym, NULL);
    printf("keysym = %d, keycode = %d, modifiers = %d\n",keysym,keyEvent->keycode, keyEvent->state);    
    
    if ((keyEvent->state & ControlMask) || (keyEvent->state & Mod1Mask)) {
        printf("special key, hot key\n");
        if ((keyEvent->state & ControlMask) && (keysym == XK_Shift_L)
            || (keyEvent->state & Mod1Mask) && (keysym == XK_z) ){
            engineEnabled = (engineEnabled == True)?False:True;
            printf("engineEnabled: %d\n", engineEnabled); 
        }
        preEditAction = PREEDIT_ACTION_FORWARD;
        return;
    }
    
    if (engineEnabled == False) {
        preEditAction = PREEDIT_ACTION_FORWARD;
        return;        
    }
        
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
            }
            
            wprintf("preEditLength = %d, preEditText = %ls\n", preEditLength, preEditText);
            if (AtWordEnd == 1) {
                preEditAction = PREEDIT_ACTION_COMMIT;
            } else {
                preEditAction = PREEDIT_ACTION_DRAW;
            } 
            return;
        }
    }
    
    if (keysym == XK_Shift_L || keysym == XK_Shift_R) {
        preEditAction = PREEDIT_ACTION_NONE;
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
    //UnikeySetInputMethod(UkVni);
    //UnikeySetOutputCharset(CONV_CHARSET_TCVN3);
    
    engineEnabled = True;  
}

void UnikeyDestroy() {
    free(preEditText);
    preEditText = NULL;
}