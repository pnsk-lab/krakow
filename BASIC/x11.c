/* $Id$ */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

bool stop = false;
bool dorender = false;
bool renderall = false;

XExposeEvent ex;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

#define FONTWIDTH 7
#define FONTHEIGHT 14
#define TEXTWIDTH 80
#define TEXTHEIGHT 25
int width;
int height;

uint8_t tram[TEXTWIDTH * TEXTHEIGHT];
uint8_t old_tram[TEXTWIDTH * TEXTHEIGHT];

GC gc;
Window w;
Display* d;
XFontStruct* font;

int bgc = 0;
int fgc = 15;

#define RGB(r,g,b) (((r) << 16) | ((g) << 8) | ((b) << 0))

uint32_t colors[] = {
	RGB(0, 0, 0),
	RGB(170, 0, 0),
	RGB(0, 170, 0),
	RGB(170, 85, 0),
	RGB(0, 0, 170),
	RGB(170, 0, 170),
	RGB(0, 170, 170),
	RGB(170, 170, 170),
	RGB(85, 85, 85),
	RGB(255, 85, 85),
	RGB(85, 255, 85),
	RGB(255, 255, 85),
	RGB(85, 85, 255),
	RGB(255, 85, 255),
	RGB(85, 255, 255),
	RGB(255, 255, 255)
};

/* NOTE: Called from outside */
void change_color(int a){
	pthread_mutex_lock(&mutex);
	fgc = (a >> 4) & 0xf;
	bgc = (a & 0xf);
	renderall = true;
	dorender = true;
	pthread_mutex_unlock(&mutex);
}

/* NOTE: Called from outside */
void clear(void){
	pthread_mutex_lock(&mutex);
	int i;
	for(i = 0; i < TEXTWIDTH * TEXTHEIGHT; i++){
		tram[i] = 0x20;
	}
	renderall = true;
	dorender = true;
	pthread_mutex_unlock(&mutex);
}

int cx = 0;
int cy = 0;

void scroll_y(void){
	cy = TEXTHEIGHT - 1;
	int i;
	for(i = TEXTWIDTH; i < TEXTWIDTH * TEXTHEIGHT; i++){
		tram[i - TEXTWIDTH] = tram[i];
	}
	for(i = 0; i < TEXTWIDTH; i++){
		tram[TEXTWIDTH * TEXTHEIGHT - TEXTWIDTH + i] = 0x20;
	}
}

#define maxmacro(a,b) ((a) > (b) ? (a) : (b))
#define minmacro(a,b) ((a) < (b) ? (a) : (b))

void putstr(const char* n){
	pthread_mutex_lock(&mutex);
	int i;
	ex.x = cx * TEXTWIDTH;
	ex.y = cy * TEXTHEIGHT;
	ex.width = TEXTWIDTH * FONTWIDTH;
	ex.height = 0;
	for(i = 0; n[i] != 0; i++){
		if(n[i] == '\n'){
			cy++;
			cx = 0;
			if(cy == TEXTHEIGHT){
				scroll_y();
				ex.height += FONTHEIGHT;
			}
		}else if(n[i] == 8){
			if(cx > 0) cx--;
		}else{
			tram[cy * TEXTWIDTH + cx++] = n[i];
			if(cx == TEXTWIDTH){
				cx = 0;
				cy++;
				if(cy == TEXTHEIGHT){
					scroll_y();
					ex.height += FONTHEIGHT;
				}
			}
		}
		ex.x = minmacro(ex.x, cx * TEXTWIDTH);
		ex.y = maxmacro(ex.y, cy * TEXTHEIGHT);
	}
	dorender = true;
	pthread_mutex_unlock(&mutex);
}

char x11_putchar(char c){
	char cbuf[2];
	cbuf[0] = c;
	cbuf[1] = 0;
	putstr(cbuf);
}

void putnum(int n){
	char number[512];
	sprintf(number, "%d", n);
	putstr(number);
}

char keybuffer[64];

char oggetch(char wait){
	char c;
	if(wait){
		while((c = keybuffer[0]) == 0);
	}else{
		c = keybuffer[0];
		if(c == 0) return 0;
	}
	pthread_mutex_lock(&mutex);
	int i;
	for(i = 1;; i++){
		keybuffer[i - 1] = keybuffer[i];
		if(keybuffer[i] == 0) break;
	}
	pthread_mutex_unlock(&mutex);
	return c;
}

void render(void){
	pthread_mutex_lock(&mutex);
	if(renderall){
		XSetForeground(d, gc, colors[bgc]);
		XFillRectangle(d, w, gc, 0, 0, width, height);
	}


	int y, x;
	char cbuf[2];
	cbuf[1] = 0;
	for(y = 0; y < TEXTHEIGHT; y++){
		for(x = 0; x < TEXTWIDTH; x++){
			cbuf[0] = tram[y * TEXTWIDTH + x];
			bool rnd = !renderall && old_tram[y * TEXTWIDTH + x] != cbuf[0];
			if(renderall || rnd){
				XSetForeground(d, gc, colors[bgc]);
				XFillRectangle(d, w, gc, x * FONTWIDTH, y * FONTHEIGHT, FONTWIDTH + 2, FONTHEIGHT + 2);
				XSetForeground(d, gc, colors[fgc]);
				XDrawString(d, w, gc, x * FONTWIDTH, y * FONTHEIGHT + FONTHEIGHT, cbuf, 1);
				if(rnd) old_tram[y * TEXTWIDTH + x] = cbuf[0];
			}
		}
	}

	XFlush(d);
	pthread_mutex_unlock(&mutex);

	if(renderall) renderall = false;
}

void x11_init(void){
	int i;
	for(i = 0; i < TEXTWIDTH * TEXTHEIGHT; i++){
		tram[i] = 0x20;
		old_tram[i] = 0xff;
	}
}

void* x11_thread(void* arg){
	width = FONTWIDTH * TEXTWIDTH;
	height = FONTHEIGHT * TEXTHEIGHT;

	int i;
	for(i = 0; i < 64; i++) keybuffer[i] = 0;

	d = XOpenDisplay(NULL);
	w = XCreateSimpleWindow(d, RootWindow(d, 0), 0, 0, width, height, 3, WhitePixel(d, 0), BlackPixel(d, 0));
	XStoreName(d, w, "Krakow BASIC");

	XSizeHints* size = XAllocSizeHints();
	size->flags = PMinSize | PMaxSize;
	size->min_width = size->max_width = width;
	size->min_height = size->max_height = height;
	XSetWMNormalHints(d, w, size);
	XFree(size);

	font = XLoadQueryFont(d, "7x14");

	XEvent ev;

	XSelectInput(d, w, ExposureMask | KeyPressMask | KeyReleaseMask);
	XMapWindow(d, w);
	XFlush(d);

	gc = XCreateGC(d, DefaultRootWindow(d), 0, 0);
	XSetFont(d, gc, font->fid);

	renderall = true;
	render();

	while(1){
		if(dorender){
			render();
			dorender = false;
		}else if(XPending(d) > 0){
			XNextEvent(d, &ev);
			if(ev.type == Expose){
				ex = ev.xexpose;
				renderall = true;
				render();
			}else if(ev.type == KeyPress){
				for(i = 0; keybuffer[i] != 0; i++);
				char* k_ = XKeysymToString(XLookupKeysym(&ev.xkey, 0));
				char* k = malloc(strlen(k_) + 1);
				memcpy(k, k_, strlen(k_));
				k[strlen(k_)] = 0;
				if(strcmp(k, "Return") == 0){
					k[0] = '\n';
				}else if(strcmp(k, "BackSpace") == 0){
					k[0] = '\x08';
				}else if(strcmp(k, "space") == 0){
					k[0] = ' ';
				}else if(strcmp(k, "comma") == 0){
					k[0] = ',';
				}else if(strcmp(k, "semicolon") == 0){
					k[0] = ';';
				}else if(strcmp(k, "c") == 0 && ev.xkey.state & ControlMask){
					k[0] = '\x01';
				}else if(strcmp(k, "1") == 0 && ev.xkey.state & ShiftMask){
					k[0] = '!';
				}else if(strcmp(k, "2") == 0 && ev.xkey.state & ShiftMask){
					k[0] = '"';
				}else if(strcmp(k, "3") == 0 && ev.xkey.state & ShiftMask){
					k[0] = '#';
				}else if(strcmp(k, "4") == 0 && ev.xkey.state & ShiftMask){
					k[0] = '$';
				}else if(strcmp(k, "5") == 0 && ev.xkey.state & ShiftMask){
					k[0] = '%';
				}else if(strcmp(k, "6") == 0 && ev.xkey.state & ShiftMask){
					k[0] = '&';
				}else if(strcmp(k, "7") == 0 && ev.xkey.state & ShiftMask){
					k[0] = '\'';
				}else if(strcmp(k, "8") == 0 && ev.xkey.state & ShiftMask){
					k[0] = '(';
				}else if(strcmp(k, "9") == 0 && ev.xkey.state & ShiftMask){
					k[0] = ')';
				}else if(strcmp(k, "0") == 0 && ev.xkey.state & ShiftMask){
					k[0] = '~';
				}else if(strcmp(k, "Control_L") == 0){
					k[0] = '\x00';
				}else if(strcmp(k, "Shift_L") == 0){
					k[0] = '\x00';
				}else if(strcmp(k, "Control_R") == 0){
					k[0] = '\x00';
				}else if(strcmp(k, "Shift_R") == 0){
					k[0] = '\x00';
				}else if(ev.xkey.state & ShiftMask){
					k[0] = toupper(k[0]);
				}
				keybuffer[i] = k[0];
				free(k);
			}
		}
	}
	stop = true;
	return arg;
}
