/* $Id$ */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

bool stop = false;
bool dorender = false;

const int fontwidth = 7;
const int fontheight = 14;
const int textwidth = 80;
const int textheight = 25;
int width;
int height;

GC gc;
Window w;
Display* d;

int bgc = 0;
int fgc = 15;

uint32_t colors[] = {
	0x000000
};

void render(void){
	XSetForeground(d, gc, colors[bgc]);
	XFillRectangle(d, w, gc, 0, 0, width, height);
	XFlush(d);
}

void* x11_thread(void* arg){
	width = fontwidth * textwidth;
	height = fontheight * textheight;

	d = XOpenDisplay(NULL);
	w = XCreateSimpleWindow(d, RootWindow(d, 0), 0, 0, width, height, 3, WhitePixel(d, 0), BlackPixel(d, 0));
	XStoreName(d, w, "Krakow BASIC");

	XSizeHints* size = XAllocSizeHints();
	size->flags = PMinSize | PMaxSize;
	size->min_width = size->max_width = width;
	size->min_height = size->max_height = height;
	XSetWMNormalHints(d, w, size);
	XFree(size);

	XEvent ev;

	XSelectInput(d, w, ExposureMask);
	XMapWindow(d, w);
	XFlush(d);

	gc = XCreateGC(d, DefaultRootWindow(d), 0, 0);

	render();

	while(1){
		if(dorender){
			render();
			dorender = false;
		}else if(XPending(d) > 0){
			XNextEvent(d, &ev);
			if(ev.type == Expose){
				render();
			}
		}
	}
	stop = true;
	return arg;
}
