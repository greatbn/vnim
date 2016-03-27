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
#include <X11/Xlib.h>
#include <IMdkit.h>
#include <Xi18n.h>
#include "IC.h"
#include <stdio.h>

static IC *ic_list = (IC *)NULL;
static IC *free_list = (IC *)NULL;

static IC
*NewIC()
{
    static CARD16 icid = 0;
    IC *rec;

    if (free_list != NULL) {
	rec = free_list;
	free_list = free_list->next;
    } else {
	rec = (IC *)malloc(sizeof(IC));
    }
    memset(rec, 0, sizeof(IC));
    rec->id = ++icid;

    rec->next = ic_list;
    ic_list = rec;
    rec->preedit_len = 0;
    rec->preedit_enabled = False;
    return rec;
}

IC
*FindIC(icid)
CARD16 icid;
{
    IC *rec = ic_list;

    while (rec != NULL) {
	if (rec->id == icid)
	  return rec;
	rec = rec->next;
    }

    return NULL;
}

static void
DeleteIC(icid)
CARD16 icid;
{
    IC *rec, *last;

    last = NULL;
    for (rec = ic_list; rec != NULL; last = rec, rec = rec->next) {
        if (rec->id == icid) {
          if (last != NULL)
            last->next = rec->next;
          else
            ic_list = rec->next;

          rec->next = free_list;
          free_list = rec;
          return;
	}
    }
    return;
}

static int Is(char *attr, XICAttribute *attr_list) {
	return !strcmp(attr, attr_list->name);
}

static void
StoreIC(rec, call_data)
IC *rec;
IMChangeICStruct *call_data;
{
	XICAttribute *ic_attr = call_data->ic_attr;
	XICAttribute *pre_attr = call_data->preedit_attr;
	XICAttribute *sts_attr = call_data->status_attr;
	register int i;
	
	for (i = 0; i < (int)call_data->ic_attr_num; i++, ic_attr++) {
		if (Is (XNInputStyle, ic_attr))
		    rec->input_style = *(INT32*)ic_attr->value;

		else if (Is (XNClientWindow, ic_attr))
		    rec->client_win = *(Window*)ic_attr->value;
		
		else if (Is (XNFocusWindow, ic_attr))
		    rec->focus_win = *(Window*)ic_attr->value;

		else 
		    fprintf(stderr, "Unknown attr: %s\n", ic_attr->name);
	}
	
	for (i = 0; i < (int)call_data->preedit_attr_num; i++, pre_attr++) {
		    fprintf(stderr, "Unknown attr: %s\n", ic_attr->name);
	}
	
	for (i = 0; i < (int)call_data->status_attr_num; i++, sts_attr++) {
		    fprintf(stderr, "Unknown attr: %s\n", ic_attr->name);
	}
	
}

void
CreateIC(call_data)
IMChangeICStruct *call_data;
{
    IC *rec;

    rec = NewIC();
    if (rec == NULL)
      return;
    StoreIC(rec, call_data);
    call_data->icid = rec->id;
    return;
}

void
DestroyIC(call_data)
IMChangeICStruct *call_data;
{
    DeleteIC(call_data->icid);
    return;
}

void
SetIC(call_data)
IMChangeICStruct *call_data;
{
    IC *rec = FindIC(call_data->icid);

    if (rec == NULL)
      return;
    StoreIC(rec, call_data);
    return;
}

void
GetIC(call_data)
IMChangeICStruct *call_data;
{
    XICAttribute *ic_attr = call_data->ic_attr;
    XICAttribute *pre_attr = call_data->preedit_attr;
    XICAttribute *sts_attr = call_data->status_attr;
    register int i;
    IC *rec = FindIC(call_data->icid);

    if (rec == NULL)
      return;
    for (i = 0; i < (int)call_data->ic_attr_num; i++, ic_attr++) {
	if (Is (XNFilterEvents, ic_attr)) {
	    ic_attr->value = (void *)malloc(sizeof(CARD32));
	    *(CARD32*)ic_attr->value = KeyPressMask|KeyReleaseMask;
	    ic_attr->value_length = sizeof(CARD32);
	}
    }


}
