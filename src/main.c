#include <stdio.h>
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
static Bool engineEnabled = True;

const wchar_t * XIMGetPreeditText() {
    return (preEditLength > 0)?preEditText:NULL;
}

void ProcessHotKey(XKeyEvent * keyEvent, KeySym keysym) {
    static int triggerNum = 0;
    if (triggerNum == 0 && (keyEvent->type == KeyPress) && (keysym == XK_Control_L)) {
        triggerNum = 1;
    } else if (triggerNum == 1 && (keyEvent->type == KeyPress) && (keysym == XK_Shift_L)) {
        triggerNum = 2;
    } else if (triggerNum == 2 && (keyEvent->type == KeyRelease)) {
        engineEnabled = (engineEnabled == True)?False:True;
        printf("engineEnabled: %d, %d\n", engineEnabled, triggerNum);
        triggerNum = 0;
	} else if (triggerNum == 0 && (keyEvent->type == KeyPress) && (keysym ==  XK_Alt_L)) {
	    triggerNum = 11;
	} else if (triggerNum == 11 && (keyEvent->type == KeyPress) && (keysym == XK_z)) {
		triggerNum = 12;
	} else if (triggerNum == 12 && keyEvent->type == KeyRelease) {
        engineEnabled = (engineEnabled == True)?False:True;
        printf("engineEnabled: %d, %d\n", engineEnabled,triggerNum);
        triggerNum = 0;		
    } else {
        triggerNum = 0;
    }
}

#define STRBUFLEN 64
int XIMProcessKey(XKeyEvent * keyEvent) {
    // printf("ProcessKey\n");
    static char strbuf[STRBUFLEN];
    static unsigned char keyval;
    static KeySym keysym;
    int count;
    
    memset(strbuf, 0, STRBUFLEN);
    count = XLookupString(keyEvent, strbuf, STRBUFLEN, &keysym, NULL);
    printf("XIMProcessKey keysym = %d, keycode = %d, modifiers = %d\n",keysym,keyEvent->keycode, keyEvent->state);
        
    ProcessHotKey(keyEvent, keysym);
    
    if ((!engineEnabled)
        || (keyEvent->state & ControlMask)
        || (keyEvent->state & Mod1Mask)) {
        printf("special key or engine disabled\n");
        return PREEDIT_ACTION_COMMIT_FORWARD;        
    }
        
    if (keyEvent->type != KeyPress
        || keysym == XK_Shift_L
        || keysym == XK_Shift_R) {
            printf("ignore shift, key release event\n");
        return PREEDIT_ACTION_FORWARD;
    }
            
    if (keysym == XK_BackSpace && preEditLength > 0) {
        printf("backspace pressed\n");
        
        //preediting
        ViProcessBackspace();
        ViGetCurrentWord(preEditText, &preEditLength);
        
        return (preEditLength > 0)?PREEDIT_ACTION_DRAW:PREEDIT_ACTION_DISCARD;
    } else if (count > 0) {
        keyval = strbuf[0]; 
        if (keyval >=' ' && keyval <= '~' ) {
            printf("processKey [%c]. shift %d, caplock %d\n", keyval, keyEvent->state & ShiftMask, keyEvent->state & LockMask);
            if (ViProcessKey(keyval, ((keyEvent->state & ShiftMask) > 0) != ((keyEvent->state & LockMask) > 0))){
                ViGetCurrentWord(preEditText, &preEditLength);
                return PREEDIT_ACTION_DRAW;
            } else {
                ViGetCurrentWord(preEditText, &preEditLength);
            }
        }
    }

    return PREEDIT_ACTION_COMMIT_FORWARD;
}

void EngineReset() {
    ViResetEngine();
    preEditLength = 0;
    preEditText[0] = 0;    
}

void XIMCommitDone() {
    printf("XIMCommitDone\n");
    EngineReset();
}

void XIMFocusIn() {
    printf("Focus In\n");
    EngineReset();
}

void XIMFocusOut() {
    printf("Focus out\n");
    EngineReset();
}

void XIMResetFocus() {
    printf("XIMResetFocus\n");
    EngineReset();
}

void XIMInit() {
    ViInitEngine();
    //engineEnabled = True;
}

void XIMDestroy() {
    EngineReset();
    ViDestroyEngine();
}

extern void InitXIM();

int main(int argc, char** argv) {
    int inputEngine = TELEX_INPUT;
    Bool enableVerbose = False;
    int i;    
    for (i = 1; i < argc; i++) {
	    if (!strcmp(argv[i], "--vni")) {
	        inputEngine = VNI_INPUT;
	    } else if (!strcmp(argv[i], "--verbose")) {
	        enableVerbose = True;
        } else if (!strcmp(argv[i], "--silent")) {
            engineEnabled = False;
	// } else if (!strcmp(argv[i], "-static")) {
	//     use_trigger = False;
	// } else if (!strcmp(argv[i], "-tcp")) {
	//     use_tcp = True;
	// } else if (!strcmp(argv[i], "-local")) {
	//     use_local = True;
	// } else if (!strcmp(argv[i], "-offkey")) {
	//     use_offkey = True;
	// } else if (!strcmp(argv[i], "-kl")) {
	//     filter_mask = (KeyPressMask|KeyReleaseMask);
	    }
    }
    
    if (! enableVerbose) { //disable log
        fclose(stdout);
    }    
    
    XIMInit();
    SetInputEngine(inputEngine);
    InitXIM();
}