#include <stddef.h>
#include "unikey_xim.h"

#define BUFFER_LENTH 1024
static char * preEditText = NULL;
static int preEditShow = 0;
static int needCommit = 0;

int getPreEditAction() {
    if (preEditShow == 0) {
        preEditShow = 1;
        return PREEDIT_ACTION_START;
    } else if (needCommit == 1) {
        return PREEDIT_ACTION_COMMIT;
    } else {
        return PREEDIT_ACTION_DRAW;
    }
    
    return PREEDIT_ACTION_NONE;
}

const char * getPreEditText() {
    // strcpy(preEditText, "Bong");
    return preEditText;
}

void processKey(unsigned char key) {
    if (preEditText == NULL) {
        preEditText = (char *)malloc(BUFFER_LENTH);
    }
    needCommit = 0;
    printf("processKey [%c]\n", key);
    static int length = 0;
    preEditText[length] = key;
    length ++;
    preEditText[length] = '\0';
    printf("preedit text %s\n",preEditText);
    if (length > 4) {
        length = 0;
        needCommit = 1;
    }
}