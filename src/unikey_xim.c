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

#define BUFFER_LENTH 1024
static char * preEditText = NULL;
static int preEditLength = 0;
static int preEditAction = PREEDIT_ACTION_NONE;

int getPreEditAction() {
    return preEditAction;  
}

const char * getPreEditText() {
    // strcpy(preEditText, "Bong");
    return preEditText;
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
    KeySym keysym;
    int count;

    // fprintf(stderr, "Processing \n");
    memset(strbuf, 0, STRBUFLEN);
    count = XLookupString(keyEvent, strbuf, STRBUFLEN, &keysym, NULL);
    printf("keysym = %d, keycode = %d\n",keysym,keyEvent->keycode);
    
    if (count > 0) {
        if (strbuf[0] >= ' ' && strbuf[0] <= '~' ) {
            printf("processKey [%c]\n", strbuf[0]);
            preEditText[preEditLength++] = strbuf[0];
            preEditText[preEditLength] = '\0';
            printf("preedit text %s\n",preEditText);
            if (preEditLength > 4) {
                preEditLength = 0;
                //needCommit = 1;
                preEditAction = PREEDIT_ACTION_COMMIT;
                return;
            }
            preEditAction = PREEDIT_ACTION_DRAW;
            return;
        }
    }
    
    if (preEditLength > 0) {
        preEditAction = PREEDIT_ACTION_COMMIT_FORWARD;
    } else {
        preEditAction = PREEDIT_ACTION_DISCARD_FORWARD;
    }
}

void UnikeyReset() {
    preEditText[preEditLength = 0] = 0;
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
        preEditText = (char *)malloc(BUFFER_LENTH);
    }
    
    UnikeySetup();  
}

void UnikeyDestroy() {
    free(preEditText);
    preEditText = NULL;
}