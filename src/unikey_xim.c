#include <stddef.h>
#include "unikey_xim.h"

#define BUFFER_LENTH 1024
static char * preEditText = NULL;
static int preEditAction = PREEDIT_ACTION_NONE;

int getPreEditAction() {
    return preEditAction;  
}

const char * getPreEditText() {
    // strcpy(preEditText, "Bong");
    return preEditText;
}

void UnikeyProcessKey(unsigned char key, int keysym, int state) {
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
        preEditAction = PREEDIT_ACTION_COMMIT;
        return;
    }
    preEditAction = PREEDIT_ACTION_DRAW;
}