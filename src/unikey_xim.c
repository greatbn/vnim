#include <stddef.h>
#include "unikey_xim.h"

#define BUFFER_LENTH 1024
static char * preEditText = NULL;
static int preEditShow = 0;
static int preEditAction = PREEDIT_ACTION_NONE;

int getPreEditAction() {
    if (preEditAction == PREEDIT_ACTION_DRAW && preEditShow == 0) {
        preEditShow = 1;
        return PREEDIT_ACTION_START;
    }
    
    if (preEditAction == PREEDIT_ACTION_HIDE && preEditShow == 1) {
        preEditShow = 0;
    }
    
    return preEditAction;  
}

const char * getPreEditText() {
    // strcpy(preEditText, "Bong");
    return preEditText;
}

void processKey(unsigned char key) {
    if (preEditText == NULL) {
        preEditText = (char *)malloc(BUFFER_LENTH);
    }
    printf("processKey [%c]\n", key);
    static int length = 0;
    preEditText[length] = key;
    length ++;
    preEditText[length] = '\0';
    printf("preedit text %s\n",preEditText);
    if (length > 4) {
        length = 0;
        //needCommit = 1;
        preEditAction = PREEDIT_ACTION_HIDE;
        return;
    }
    preEditAction = PREEDIT_ACTION_DRAW;
}