#ifndef __UNIKEY_XIM_H__
#define __UNIKEY_XIM_H__

#define PREEDIT_ACTION_NONE (0)
#define PREEDIT_ACTION_START (PREEDIT_ACTION_NONE + 1)
#define PREEDIT_ACTION_COMMIT (PREEDIT_ACTION_START + 1)
#define PREEDIT_ACTION_DRAW (PREEDIT_ACTION_COMMIT + 1)
#define PREEDIT_ACTION_COMMIT_FORWARD (PREEDIT_ACTION_DRAW + 1)
#define PREEDIT_ACTION_FORWARD (PREEDIT_ACTION_COMMIT_FORWARD + 1)
#define PREEDIT_ACTION_DISCARD (PREEDIT_ACTION_FORWARD + 1)
 
int getPreEditAction();
const wchar_t* getPreEditText();
void UnikeyProcessKey(XKeyEvent * keyEvent);
void UnikeyCommitDone();
void UnikeyFocusIn();
void UnikeyFocusOut();
void UnikeyInit();
void UnikeyDestroy();
#endif