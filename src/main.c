#include <stdio.h>
#include <stddef.h>
#include <X11/Xlocale.h>
// #include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
// #include "IMdkit.h"
    // #include <Xi18n.h>
#include "locales.h"
#include "wrapper.h"
#include "vi-engine.h"

#define PROG_NAME "Vietnam XIM"

int main(int argc, char** argv) {
    int inputEngine = TELEX_INPUT;
    Bool enableVerbose = False;
    int i;    
    for (i = 1; i < argc; i++) {
	    if (!strcmp(argv[i], "--vni")) {
	        inputEngine = VNI_INPUT;
	    } else if (!strcmp(argv[i], "--verbose")) {
	        enableVerbose = True;
	// } else if (!strcmp(argv[i], "-dynamic")) {
	//     use_trigger = True;
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
    
    Display *dpy;    
   Window imWindow;
    if (!setlocale(LC_CTYPE, "en_US.UTF-8") && !setlocale(LC_CTYPE, "vi_VN.UTF-8")) {
        fputs("Cannot load either en_US.UTF-8 or vi_VN.UTF-8 locale\n"
              "To use this program you must have one of these locales installed\n", stderr);
        exit(1);
    }
        
    if ((dpy = XOpenDisplay(NULL)) == NULL) {
	    fprintf(stderr, "Can't Open Display\n");
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
    
    InitXIM(dpy, &imWindow);
        
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