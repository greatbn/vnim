#ifndef __WRAPPER_H__
#define __WRAPPER_H__

#include "wchar.h"

#define PREEDIT_ACTION_NONE (0)
#define PREEDIT_ACTION_START (PREEDIT_ACTION_NONE + 1)
#define PREEDIT_ACTION_COMMIT (PREEDIT_ACTION_START + 1)
#define PREEDIT_ACTION_DRAW (PREEDIT_ACTION_COMMIT + 1)
#define PREEDIT_ACTION_COMMIT_FORWARD (PREEDIT_ACTION_DRAW + 1)
#define PREEDIT_ACTION_FORWARD (PREEDIT_ACTION_COMMIT_FORWARD + 1)
#define PREEDIT_ACTION_DISCARD (PREEDIT_ACTION_FORWARD + 1)

void XIMInit();
void XIMDestroy();
void XIMFocusOut();
void XIMFocusIn();
void XIMCommitDone();
void XIMResetFocus();
const wchar_t* XIMGetPreeditText();
int XIMProcessKey(XKeyEvent * keyEvent);

#endif //__WRAPPER_H__