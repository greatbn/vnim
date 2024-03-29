/******************************************************************
 
         Copyright 1994, 1995 by Sun Microsystems, Inc.
         Copyright 1993, 1994 by Hewlett-Packard Company
 
Permission to use, copy, modify, distribute, and sell this software
and its documentation for any purpose is hereby granted without fee,
provided that the above copyright notice appear in all copies and
that both that copyright notice and this permission notice appear
in supporting documentation, and that the name of Sun Microsystems, Inc.
and Hewlett-Packard not be used in advertising or publicity pertaining to
distribution of the software without specific, written prior permission.
Sun Microsystems, Inc. and Hewlett-Packard make no representations about
the suitability of this software for any purpose.  It is provided "as is"
without express or implied warranty.
 
SUN MICROSYSTEMS INC. AND HEWLETT-PACKARD COMPANY DISCLAIMS ALL
WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
SUN MICROSYSTEMS, INC. AND HEWLETT-PACKARD COMPANY BE LIABLE FOR ANY
SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 
  Author: Hidetoshi Tajima(tajima@Eng.Sun.COM) Sun Microsystems, Inc.
 
******************************************************************/
#include <stdio.h>
#include <X11/Xlocale.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <IMdkit.h>
#include <Xi18n.h>
#include "locales.h"
#include "wrapper.h"
#include "IC.h"

#define DEFAULT_IMNAME "vnim"
#define PROG_NAME "Vietnam XIM"

/* Supported Inputstyles */
static XIMStyle Styles[] = {
    XIMPreeditPosition  | XIMStatusNothing,
    XIMPreeditCallbacks | XIMStatusNothing,
    XIMPreeditNothing   | XIMStatusNothing,
    XIMPreeditPosition  | XIMStatusCallbacks,
    XIMPreeditCallbacks | XIMStatusCallbacks,
    XIMPreeditNothing   | XIMStatusCallbacks,
    0
};

static XIMEncoding SupportedEncodings[] = {
    "COMPOUND_TEXT",
    NULL
};

// queue implementations
#define MAX_BUFFER 100
static XEvent Queue[MAX_BUFFER];
static int FirstIndex = 0;
static int LastIndex = 0;

void Push(XEvent* xEvent) {
    Queue[LastIndex] = *xEvent;
    LastIndex = (LastIndex +1 ) % MAX_BUFFER;
}

Bool Pop(XEvent* xEvent) {
    if (FirstIndex == LastIndex) {
        printf("queue empty\n");
        return False;
    }
    (*xEvent) = Queue[FirstIndex];
    FirstIndex = (FirstIndex + 1) % MAX_BUFFER;
    return True;
}
//queue implementation done

static void IMPreeditShow(XIMS ims, IMForwardEventStruct *call_data, IC* ic) {
    printf("IMPreeditShow\n");
    IMPreeditCBStruct pcb;

    pcb.major_code        = XIM_PREEDIT_START;
    pcb.minor_code        = 0;
    pcb.connect_id        = call_data->connect_id;
    pcb.icid              = call_data->icid;
    pcb.todo.return_value = 0;
    IMCallCallback (ims, (XPointer) & pcb);
    ic->preedit_enabled = True;
    ic->preedit_len = 0;    
}

static void IMPreeditDraw(XIMS ims, IMForwardEventStruct *call_data, const wchar_t * buffer)
{
    IC* ic = FindIC(call_data->icid);
    if (ic == NULL) {
        fprintf(stderr, "cannot find any ic\n");
        return;
    }
    printf("id: %d, len = %d, enabled = %d\n",call_data->icid, ic->preedit_len, ic->preedit_enabled);
    if (! ic->preedit_enabled) {
        if (buffer == NULL) {
            printf("No need to enable preedit\n");
            return;
        }
        IMPreeditShow(ims, call_data, ic);
    }
    
    IMPreeditCBStruct pcb;
    XIMText text;
    static XIMFeedback feedback[128] = {0};
    pcb.major_code = XIM_PREEDIT_DRAW;        
    pcb.connect_id = call_data->connect_id;
    pcb.icid = call_data->icid;
    pcb.todo.draw.chg_first = 0;
    pcb.todo.draw.chg_length = ic->preedit_len;
    pcb.todo.draw.text = &text;
    text.feedback = feedback;    
    text.encoding_is_wchar = 0;
    
    if (buffer != NULL) {
    
        printf("IMPreeditDraw\n");
        XTextProperty tp;

        unsigned int j, i, len;
        
        len = wcslen(buffer);

        pcb.todo.draw.caret = len;
        ic->preedit_len = len;

        for (i = 0; i < len; i++) {
            feedback[i] = XIMUnderline;
        }
        feedback[len] = 0;
            
        XwcTextListToTextProperty (ims->core.display,
                                     (wchar_t **)&buffer,
                                     1, XCompoundTextStyle, &tp);
        text.length = strlen ((char*)tp.value);        
        text.string.multi_byte = (char*)tp.value;
        IMCallCallback (ims, (XPointer) &pcb);
        XFree (tp.value);
    } else if (ic->preedit_len > 0) {
            //these following code is used to clean preedit text
            printf("draw nothing\n");
            pcb.todo.draw.caret = 0;
            feedback[0] = 0;
                    
            text.length = 0;
            text.string.multi_byte = "";                    
            IMCallCallback (ims, (XPointer) &pcb);
            ic->preedit_len = 0;
    }
}

static void IMPreeditSoftHide(XIMS ims, IMForwardEventStruct* call_data){
        IMPreeditDraw(ims, call_data, NULL);
}

static void IMPreeditHide (XIMS ims, IMForwardEventStruct *call_data) {
    IC* ic = FindIC(call_data->icid);
    if (ic == NULL) {
        fprintf(stderr, "cannot find any ic\n");
        return;
    }
    if (ic->preedit_enabled) {
        IMPreeditSoftHide(ims, call_data);
        printf("IMPreeditHide\n");        
        
        IMPreeditCBStruct pcb;

        pcb.major_code        = XIM_PREEDIT_DONE;
        pcb.minor_code        = 0;
        pcb.connect_id        = call_data->connect_id;
        pcb.icid              = call_data->icid;
        pcb.todo.return_value = 0;
        IMCallCallback (ims, (XPointer) & pcb);
        // gIsPreeditShowing = False;
        ic->preedit_enabled = False;
    }
}

Bool IMPreeditCommit(XIMS ims, IMForwardEventStruct *call_data, const wchar_t *buffer)
{
    if (buffer != NULL) {
        IMPreeditSoftHide(ims,call_data);
        
        IMCommitStruct commitInfo;
        XIMText text;
        XTextProperty tp;

        XwcTextListToTextProperty (ims->core.display,
                                        (wchar_t **)&buffer,
                                        1, XCompoundTextStyle, &tp);    
        
        *((IMAnyStruct *)&commitInfo) = *((IMAnyStruct *)call_data);
        commitInfo.major_code = XIM_COMMIT;
        commitInfo.icid = call_data->icid;
        commitInfo.connect_id = call_data->connect_id;
        commitInfo.flag = XimLookupChars;
        commitInfo.commit_string = (unsigned char*)tp.value;
        IMCommitString(ims, (XPointer)&commitInfo);
        XFree (tp.value);
        XIMCommitDone(); //callback
        return True;
    }
    return False;
}

void ProcessKey(XIMS ims, IMForwardEventStruct *call_data)
{
    static wchar_t *buffer;
    //IMPreeditHide(ims, call_data);    
    switch (XIMProcessKey((XKeyEvent*)&call_data->event)) {
        case PREEDIT_ACTION_DRAW:
            printf("PREEDIT_ACTION_DRAW\n");
            IMPreeditDraw(ims, call_data, XIMGetPreeditText());
            break;
        case PREEDIT_ACTION_COMMIT:
            printf("PREEDIT_ACTION_COMMIT\n");
            IMPreeditCommit(ims, call_data, XIMGetPreeditText());
            break;
        case PREEDIT_ACTION_COMMIT_FORWARD:
            buffer = XIMGetPreeditText();
            printf("PREEDIT_ACTION_COMMIT_FORWARD %d\n",call_data->sync_bit);
            if (buffer != NULL) {
                ims->sync = True;
                IMPreeditCommit(ims, call_data, buffer);
                Push(&(call_data->event));
            } else {
                printf("IMForwardEvent only\n");
                IMForwardEvent(ims, call_data);
            }
            break;
        case PREEDIT_ACTION_FORWARD:
            printf("PREEDIT_ACTION_FORWARD\n");
//            IMPreeditSoftHide(ims, call_data);
            IMForwardEvent(ims, call_data);
            break;
        case PREEDIT_ACTION_DISCARD:
            printf("PREEDIT_ACTION_DISCARD\n");
            IMPreeditCommit(ims, call_data, "");
            break;
    }
        
    XSync(ims->core.display, False);    
}

Bool MyProtoHandler(XIMS ims, IMProtocol* call_data)
{
    switch (call_data->major_code) {
      case XIM_OPEN:
        printf("XIM_OPEN: %d\n",  (int)call_data->any.connect_id);
//     printf("new_client lang = %s\n", call_data->lang.name);
	    // return MyOpenHandler(ims, call_data);
        return True;
      case XIM_CLOSE:
        printf("XIM_CLOSE: %d\n",  (int)call_data->any.connect_id);
	    // return MyCloseHandler(ims, call_data);
        return True;
      case XIM_CREATE_IC:
        printf("XIM_CREATE_IC:\n");
        CreateIC(call_data);
        return True;
	    // return MyCreateICHandler(ims, call_data);
      case XIM_DESTROY_IC:
        printf("XIM_DESTROY_IC.\n");
        DestroyIC(call_data);
        return True;
        //return MyDestroyICHandler(ims, call_data);
      case XIM_SET_IC_VALUES:
        printf("XIM_SET_IC_VALUES:\n");
        SetIC(call_data);
        return True;
	    //return MySetICValuesHandler(ims, call_data);
      case XIM_GET_IC_VALUES:
        printf("XIM_GET_IC_VALUES:\n");
        GetIC(call_data);
	    return True;
      case XIM_FORWARD_EVENT:
	    ProcessKey(ims, call_data);
        return True;
      case XIM_SET_IC_FOCUS:
        printf("XIM_SET_IC_FOCUS()\n");
        IMPreeditSoftHide(ims, call_data);
        FirstIndex = LastIndex = 0;
        XIMFocusIn();
	    return True;
      case XIM_UNSET_IC_FOCUS:
        printf("XIM_UNSET_IC_FOCUS:\n");
        IMPreeditCommit(ims,call_data, XIMGetPreeditText());
        IMPreeditHide(ims,call_data);
        FirstIndex = LastIndex = 0;        
        XIMFocusOut();
	    return True;
      case XIM_RESET_IC:
        printf("XIM_RESET_IC_FOCUS:\n");
        IMPreeditCommit(ims,call_data, XIMGetPreeditText());
        IMPreeditHide(ims,call_data);
        FirstIndex = LastIndex = 0;
        XIMResetFocus();
	    return True;
    //   case XIM_TRIGGER_NOTIFY:
    //     printf("XIM_TRIGGER_NOTIFY:\n");
	//     return MyTriggerNotifyHandler(ims, call_data);
      case XIM_PREEDIT_START_REPLY:
        printf("XIM_PREEDIT_START_REPLY:\n");
        return True;
	    //return MyPreeditStartReplyHandler(ims, call_data);
    //   case XIM_PREEDIT_CARET_REPLY:
    //     printf("XIM_PREEDIT_CARET_REPLY:\n");
	//     return MyPreeditCaretReplyHandler(ims, call_data);
        case XIM_SYNC_REPLY:
            printf("sync received\n");
            XEvent event;
            if (Pop(&event)) {
                printf("do syncing...\n");
                IMForwardEventStruct* fwdata = (IMForwardEventStruct*)call_data;
                fwdata->event = event;
                IMForwardEvent(ims, fwdata);
            }
            return True;
      default:
	    fprintf(stderr, "Unknown IMDKit Protocol message type %d\n", call_data->major_code);
	break;
    }
}

XIMS ims;
//------------------------------------------------------
void MyXEventHandler(Window im_window, XEvent *event)
{
    switch (event->type) {
    case DestroyNotify:
        XDestroyWindow(event->xbutton.display, im_window);
        IMCloseIM(ims);
        exit(0);    
        break;
    case ButtonPress:
        switch (event->xbutton.button) {
        case Button1:
            break;
        }
        break;

    case PropertyNotify:
        break;
    default:
        break;
    }
}

int InitXIM() {
    Window imWindow;    
    char *display_name = NULL;
    Display *dpy;
    char *imname = DEFAULT_IMNAME;
    XIMStyles *input_styles/*, *styles2*/;
    //XIMTriggerKeys *on_keys, *trigger2;
    XIMEncodings *encodings/*, *encoding2*/;
    register int i;

    if (!setlocale(LC_CTYPE, "en_US.UTF-8") && !setlocale(LC_CTYPE, "vi_VN.UTF-8")) {
        fputs("Cannot load either en_US.UTF-8 or vi_VN.UTF-8 locale\n"
              "To use this program you must have one of these locales installed\n", stderr);
        exit(1);
    }
    
    if ((dpy = XOpenDisplay(display_name)) == NULL) {
	    fprintf(stderr, "Can't Open Display: %s\n", display_name);
	    exit(1);
    }
    
    imWindow = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy),
				    0, 700, 400, 800-700,
				    0, WhitePixel(dpy, DefaultScreen(dpy)),
				    WhitePixel(dpy, DefaultScreen(dpy)));

    if (imWindow == (Window)NULL) {
	    fprintf(stderr, "Can't Create Window\n");
	    exit(1);
    }
    XStoreName(dpy, imWindow, PROG_NAME);
    XSetTransientForHint(dpy, imWindow, imWindow);

    if ((input_styles = (XIMStyles *)malloc(sizeof(XIMStyles))) == NULL) {
	    fprintf(stderr, "Can't allocate\n");
	    exit(1);
    }
    input_styles->count_styles = sizeof(Styles)/sizeof(XIMStyle) - 1;
    input_styles->supported_styles = Styles;

    if ((encodings = (XIMEncodings *)malloc(sizeof(XIMEncodings))) == NULL) {
	    fprintf(stderr, "Can't allocate\n");
	    exit(1);
    }
    encodings->count_encodings = sizeof(SupportedEncodings)/sizeof(XIMEncoding) - 1;
    encodings->supported_encodings = SupportedEncodings;
    
    ims = IMOpenIM(dpy,
		   IMModifiers, "Xi18n",
		   IMServerWindow, imWindow,
		   IMServerName, imname,
		   IMLocale, LOCALES_STRING,
		   IMServerTransport, "X/",
		   IMInputStyles, input_styles,
		   NULL);
    if (ims == (XIMS)NULL) {
        fprintf(stderr, "Can't Open Input Method Service:\n");
        fprintf(stderr, "\tInput Method Name :%s\n", imname);
        exit(1);
    }

    IMSetIMValues(ims,
		  IMEncodingList, encodings,
		  IMProtocolHandler, MyProtoHandler,
		  IMFilterEventMask, KeyPressMask|KeyReleaseMask,
		  NULL);

    XSelectInput(dpy, imWindow, StructureNotifyMask|ButtonPressMask);
    // XMapWindow(dpy, im_window);
    XFlush(dpy);
    
    while (1) {
        XEvent event;
        XNextEvent(dpy, &event);
        if (XFilterEvent(&event, None) == True) {
            continue;
        }
        MyXEventHandler(imWindow, &event);
    }           
}