#ifndef __UNIKEY_XIM_H__
#define __UNIKEY_XIM_H__

#define PREEDIT_ACTION_NONE (0)
#define PREEDIT_ACTION_START (PREEDIT_ACTION_NONE + 1)
#define PREEDIT_ACTION_COMMIT (PREEDIT_ACTION_START + 1)
#define PREEDIT_ACTION_DRAW (PREEDIT_ACTION_COMMIT + 1)
#define PREEDIT_ACTION_HIDE (PREEDIT_ACTION_DRAW +1)
 
int getPreEditAction();
const char* getPreEditText();
void processKey(unsigned char key);
#endif