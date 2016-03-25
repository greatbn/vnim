#include <stddef.h>
// #include <X11/Xlocale.h>
// #include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
// #include "IMdkit.h"
    // #include <Xi18n.h>
#include "wrapper.h"
#include "vi-engine.h"

#define BUFFER_LENTH 1024
static wchar_t preEditText[BUFFER_LENTH];
static int preEditLength = 0;
static Bool engineEnabled = False;

const wchar_t * XIMGetPreeditText() {
    return preEditText;
}

#define STRBUFLEN 64
int XIMProcessKey(XKeyEvent * keyEvent) {
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
        
        if (preEditLength > 0) {
            return PREEDIT_ACTION_COMMIT_FORWARD;
        } else {
            return PREEDIT_ACTION_FORWARD;
        }
    }
    
    if (! engineEnabled) {
        return PREEDIT_ACTION_FORWARD;        
    }    
        
    if (keysym == XK_BackSpace && preEditLength > 0) {
        printf("backspace pressed\n");
        
        //preediting
        ViProcessBackspace();
        ViGetCurrentWord(preEditText, &preEditLength);
        
        if (preEditLength > 0) {
            return PREEDIT_ACTION_DRAW;
        } else {
            return PREEDIT_ACTION_DISCARD;
        }
    } else if (keysym == XK_Shift_L || keysym == XK_Shift_R) {
        //ignore shift
        return PREEDIT_ACTION_NONE;
    } else if (count > 0) {
        keyval = strbuf[0]; 
        if (keyval > ' ' && keyval <= '~' ) {
            printf("processKey [%c]. shift %d, caplock %d\n", keyval, keyEvent->state & ShiftMask, keyEvent->state & LockMask);
            if (ViProcessKey(keyval, ((keyEvent->state & ShiftMask) > 0) != ((keyEvent->state & LockMask) > 0))){
                ViGetCurrentWord(preEditText, &preEditLength);
                return PREEDIT_ACTION_DRAW;
            }                        
        }
    }
    
    if (preEditLength > 0) {
        return PREEDIT_ACTION_COMMIT_FORWARD;
    } else {
        return PREEDIT_ACTION_FORWARD;
    }
}

void XIMCommitDone() {
    ViResetEngine();
    preEditLength = 0;
    preEditText[0] = 0;
}

void XIMFocusIn() {
    printf("Focus In\n");
    ViResetEngine();
}

void XIMFocusOut() {
    printf("Focus out\n");
    ViResetEngine();
}

void XIMInit() {
    ViInitEngine();    
    engineEnabled = True;      
}

void XIMDestroy() {
    ViDestroyEngine();
}