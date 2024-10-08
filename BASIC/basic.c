/* $Id$ */

/* Krakow BASIC - Multi-platform simple BASIC */

#if defined(PLATFORM_X11)
#define PLATFORM_UNIX
#endif

#if defined(PLATFORM_SHIROI)

#include "dri/text.h"
#include "dri/video.h"
#include "dri/math.h"

#include "mem.h"
#include "char.h"
#define PLATFORM "Shiroi"
#define NEWLINE "\r\n"
#define BREAKKEY

#elif defined(PLATFORM_MSX)

#define PLATFORM "MSX"
#define NEWLINE "\r\n"
#define BREAKKEY

#define mull(x, y) ((x) * (y))
#define divl(x, y) ((x) / (y))

#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define agetch(x) oggetch(0)

void basic(void);

void main(void){
	basic();
}

char _wgetch(char wait) __naked {
	__asm
		ld hl, #2
		add hl, sp

		ld a, (hl)

		cp #1
		jp z, wait

		call 0x9c
		jp nz, wait
		ld l, a
		ld h, #0
		ret

wait:
		call 0x9f
		ld l, a
		ld h, #0
		ret
	__endasm;
}

char oggetch(char wait){
	char c = _wgetch(wait);
	if(c == '\r') return '\n';
	if(c == 3) return 1;
	return c;
}

void putchar(char c) __naked {
	__asm
		ld hl, #2
		add hl, sp
		ld a, (hl)
		call 0xa2
		ret
	__endasm;
}

void putstr(const char* str){
	int i;
	for(i = 0; str[i] != 0; i++) putchar(str[i]);
}

void putnum(int a){
	char numbuf[64];
	int incr = 63;
	numbuf[incr--] = 0;
	int i;
	while(1){
		numbuf[incr--] = (a % 10) + '0';
		a /= 10;
		if(a == 0) break;
	}
	putstr(numbuf + incr + 1);
}

char strcaseequ(const char* a, const char* b){
	int i;
	if(strlen(a) != strlen(b)) return 0;
	for(i = 0; a[i] != 0; i++){
		if(toupper(a[i]) != toupper(b[i])) return 0;
	}
	return 1;
}

void change_color(unsigned char c) __naked {
	__asm
		ld hl, #2
		add hl, sp

		ld a, (hl)
		and a, #0xf
		ld (0xf3ea), a

		ld a, (hl)
		and a, #0xf0
		sra a
		sra a
		sra a
		sra a
		ld (0xf3e9), a

		ld a, #1
		call 0x62

		ret
	__endasm;
}

#define strnum atoi

void clear(void) __naked {
	__asm
		call 0x6f
		ld a, #15
		ld (0xf3e9), a
		ld a, #1
		ld (0xf3ea), a
		ld a, #1
		call 0x62
		ret
	__endasm;
}

#define killcursor(x)
#define cursor(x)

#elif defined(PLATFORM_UNIX) || defined(PLATFORM_WINDOWS) || defined(PLATFORM_ARDUINO) || defined(PLATFORM_C64) || defined(PLATFORM_A800XL) || defined(PLATFORM_APPLE2) || defined(PLATFORM_PET)

#if defined(PLATFORM_WINDOWS)
#define PLATFORM "Windows"
#define NO_PEEKPOKE
#elif defined(PLATFORM_UNIX)
#define PLATFORM "Unix"
#define NO_PEEKPOKE
#elif defined(PLATFORM_ARDUINO)
#define PLATFORM "Arduino"
#define NEWLINE "\r\n"
#define BREAKKEY
#elif defined(PLATFORM_C64)
#define PLATFORM "Commodore-64"
#define NEWLINE "\r\n"
#define BREAKKEY
#include <conio.h>
#elif defined(PLATFORM_PET)
#define PLATFORM "Commodore-PET"
#define NEWLINE "\r\n"
#define BREAKKEY
#include <conio.h>
#include <peekpoke.h>
#elif defined(PLATFORM_A800XL)
#define PLATFORM "Atari-800XL"
#define NEWLINE "\n"
#define BREAKKEY
#include <conio.h>
#elif defined(PLATFORM_APPLE2)
#define PLATFORM "Apple2"
#define NEWLINE "\n"
#define BREAKKEY
#include <conio.h>
#endif

#define mull(x, y) ((x) * (y))
#define divl(x, y) ((x) / (y))
#define killcursor(x)
#define cursor(x)
#define strnum atoi

#ifndef NEWLINE
#define NEWLINE "\n"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#if defined(__MINGW32__)
#include <conio.h>
#include <windows.h>
#elif defined(PLATFORM_ARDUINO)
#define BAUD 9600
#include <avr/io.h>
#include <util/delay.h>
#include <util/setbaud.h>
#define BUFFER_SIZE (1024)
#define LINE_BUFFER_SIZE (128)
#define LINES (32)
#undef putchar
#define putchar uart_putchar
#elif defined(PLATFORM_UNIX)
#if defined(PLATFORM_X11)
#include <X11/Xlib.h>
#include <pthread.h>

void* x11_thread(void* arg);
char x11_putchar(char c);
void x11_init(void);
#else
#include <termios.h>
#include <sys/ioctl.h>
#endif
#elif defined(PLATFORM_PET)
#define BUFFER_SIZE (4 * 1024)
#undef killcursor
#undef cursor
#define killcursor(x) cursor(0)
#define cursor(x) cursor(1)
#elif defined(PLATFORM_C64)
#define BUFFER_SIZE (16 * 1024)
#undef killcursor
#undef cursor
#define killcursor(x) cursor(0)
#define cursor(x) cursor(1)
#elif defined(PLATFORM_A800XL)
#define BUFFER_SIZE (16 * 1024)
#undef killcursor
#undef cursor
#define killcursor(x) cursor(0)
#define cursor(x) cursor(1)
#elif defined(PLATFORM_APPLE2)
#define BUFFER_SIZE (8 * 1024)
#undef killcursor
#undef cursor
#define killcursor(x) cursor(0)
#define cursor(x) cursor(1)
#endif

#if defined(PLATFORM_ARDUINO)
int uart_putchar(char c) {
	while(!(UCSR0A & _BV(UDRE0)))
		;
	UDR0 = c;
	return 0;
}

void uart_init(void) {
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;

	UCSR0B |= _BV(TXEN0) | _BV(RXEN0);

	UCSR0C |= _BV(UCSZ00) | _BV(UCSZ01);
}
#endif

#if defined(PLATFORM_X11)
#undef putchar
#define putchar(x) x11_putchar(x)
#endif

#define agetch() oggetch(0)
char oggetch(char wait)
#if defined(PLATFORM_X11)
;
#else
{
	int c;
#if defined(PLATFORM_WINDOWS)
rescan:
	if(!wait){
		if(!_kbhit()) return 0;
	}
	c = _getch();
	if(c == '\r') return '\n';
	if(c == '\n') goto rescan;
#elif defined(PLATFORM_UNIX)
	if(!wait){
		int b;
		ioctl(0, FIONREAD, &b);
		if(b == 0) return 0;
	}
	c = getchar();
	if(c == EOF) return -1;
	if(c == '\r') return '\n';
#elif defined(PLATFORM_ARDUINO)
rescan:
	if(!wait) {
		if(!(UCSR0A & _BV(RXC0))) return 0;
	} else {
		while(!(UCSR0A & _BV(RXC0)))
			;
	}
	c = UDR0;
	if(c == '\r') return '\n';
	if(c == '\n') goto rescan;
	if(c == 3) return 1;
#elif defined(PLATFORM_C64) || defined(PLATFORM_PET)
	if(!wait) {
		if(!kbhit()) return 0;
	}
	c = cgetc();
	if(c == EOF) return -1;
	if(c == '\r') return '\n';
	if(c == 20) return 8;
	if(c == 3) return 1;
#elif defined(PLATFORM_A800XL)
	if(!wait) {
		if(!kbhit()) return 0;
	}
	c = cgetc();
	if(c == EOF) return -1;
	if(c == '\r') return '\n';
	if(c == 126) return 8;
	if(c == 3) return 1;
#elif defined(PLATFORM_APPLE2)
	if(!wait) {
		if(!kbhit()) return 0;
	}
	c = cgetc();
	if(c == EOF) return -1;
	if(c == '\r') return '\n';
	if(c == 3) return 1;
#endif
	return c;
}
#endif

bool strcaseequ(const char* a, const char* b) { return strcasecmp(a, b) == 0; }

#if defined(PLATFORM_ARDUINO)
void putstr(const char* n) {
	int i;
	for(i = 0; n[i] != 0; i++) {
		uart_putchar(n[i]);
	}
}

void putnum(int n) {
	char buf[64];
	int incr = 63;
	buf[incr--] = 0;
	while(1) {
		buf[incr--] = (n % 10) + '0';
		n /= 10;
		if(n == 0) break;
	}
	putstr(buf + incr + 1);
}
#else
void putnum(int n)
#if defined(PLATFORM_X11)
;
#else
{
	printf("%d", n);
	fflush(stdout);
}
#endif

void putstr(const char* n)
#if defined(PLATFORM_X11)
;
#else
{
	printf("%s", n);
	fflush(stdout);
}
#endif

#endif

void change_color(int a)
#if defined(PLATFORM_X11)
;
#else
{
#if defined(PLATFORM_ARDUINO) || defined(PLATFORM_UNIX)
	int fg = (a >> 4) & 0xf;
	int bg = (a & 0xf);
	char color[2];
	if(!(0 <= fg && fg <= 15)) return;
	if(!(0 <= bg && bg <= 15)) return;
	color[1] = 0;
	if(bg < 8) {
		color[0] = bg + '0';
		putstr("\x1b[4");
	} else {
		color[0] = (bg - 8) + '0';
		putstr("\x1b[10");
	}
	putstr(color);
	putstr("m");
	if(fg < 8) {
		color[0] = fg + '0';
		putstr("\x1b[3");
	} else {
		color[0] = (fg - 8) + '0';
		putstr("\x1b[9");
	}
	putstr(color);
	putstr("m");
	putstr("\x1b[2J\x1b[1;1H");
#elif defined(PLATFORM_C64) || defined(PLATFORM_A800XL) || defined(PLATFORM_APPLE2) || defined(PLATFORM_PET)
	int fg = (a >> 4) & 0xf;
	int bg = (a & 0xf);
	bgcolor(bg);
	textcolor(fg);
#endif
}
#endif

void clear(void)
#if defined(PLATFORM_X11)
;
#else
{
#if defined(PLATFORM_WINDOWS)
	system("cls");
#elif defined(PLATFORM_UNIX) || defined(PLATFORM_ARDUINO)
	putstr("\x1b[0m\x1b[2J\x1b[1;1H");
#elif defined(PLATFORM_C64) || defined(PLATFORM_A800XL) || defined(PLATFORM_APPLE2) || defined(PLATFORM_PET)
	clrscr();
#endif
}
#endif

void basic(void);

int main() {
#if defined(PLATFORM_PET)
	if(PEEK(0x9000) == POKE(0x9000, PEEK(0x9000) + 1)) {
		_heapadd((void*)0x9000, 0x2000);
	}
#endif
#if defined(PLATFORM_WINDOWS)
	HANDLE winstdout = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD mode = 0;
	GetConsoleMode(winstdout, &mode);
	const DWORD origmode = mode;
	mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(winstdout, mode);
#elif defined(PLATFORM_UNIX)
#if defined(PLATFORM_X11)
	pthread_t thread;
	x11_init();
	pthread_create(&thread, NULL, x11_thread, NULL);
#else
	struct termios old, new;
	tcgetattr(0, &old);
	new = old;
	new.c_lflag &= ~(ECHO | ICANON);
	tcsetattr(0, TCSANOW, &new);
#endif
#elif defined(PLATFORM_ARDUINO)
	uart_init();
	DDRB |= _BV(DDB5);
	PORTB |= _BV(PORT5);
#endif
#if defined(PLATFORM_C64) || defined(PLATFORM_A800XL) || defined(PLATFORM_APPLE2) || defined(PLATFORM_PET)
	change_color((1 << 4) | 0);
	bordercolor(0);
#endif
	basic();
#if defined(PLATFORM_WINDOWS)
	SetConsoleMode(winstdout, origmode);
#elif defined(PLATFORM_UNIX)
#if defined(PLATFORM_X11)
	pthread_join(thread, NULL);
#else
	tcsetattr(0, TCSANOW, &old);
#endif
#endif
}

#else
#error "Define PLATFORM_*"
#endif

#define VERSION "0.0"

#ifndef LINE_BUFFER_SIZE
#define LINE_BUFFER_SIZE (512)
#endif

#ifndef BUFFER_SIZE
#define BUFFER_SIZE (1024 * 24)
#endif

#ifndef LINES
#define LINES (1024)
#endif

unsigned char basicbuffer[BUFFER_SIZE / 2];
unsigned char varbuffer[BUFFER_SIZE / 2];
char linebuf[LINE_BUFFER_SIZE];

int hexnum(char c) {
	if('0' <= c && c <= '9') {
		return c - '0';
	} else if('A' <= c && c <= 'F') {
		return c - 'A' + 10;
	} else if('a' <= c && c <= 'f') {
		return c - 'a' + 10;
	}
	return 0;
}

int pexpr(char* expr, char* buffer, int* number) {
	char ownbuf[128];
	int i;
	int start = 0;
	int br = 0;
	int result = 0;
	int stack[32];
	int sp = 0;
	char put = 0;
	char hex = 0;
	if(expr[0] == '"' && expr[strlen(expr) - 1] == '"'){
		for(i = 1; expr[i + 1] != 0; i++){
			buffer[i - 1] = expr[i];
		}
		buffer[i - 1] = 0;
		return 0;
	}
	for(i = 0; expr[i] != 0; i++) ownbuf[i] = expr[i];
	ownbuf[i] = 0;
	for(i = 0; i < 32; i++) stack[i] = 0;
	for(i = 0;; i++) {
		if(ownbuf[i] == 0) {
			break;
		} else if(ownbuf[i] == '&' && put == 0) {
			put = 1;
			hex = 1;
		} else if(('0' <= ownbuf[i] && ownbuf[i] <= '9') || (hex ? ('A' <= ownbuf[i] && ownbuf[i] <= 'F') : 0) || (hex ? ('a' <= ownbuf[i] && ownbuf[i] <= 'f') : 0)) {
			stack[sp] *= hex ? 16 : 10;
			if(hex == 1) {
				stack[sp] += hexnum(ownbuf[i]);
			} else {
				stack[sp] += ownbuf[i] - '0';
			}
			put = 1;
#ifndef NO_PEEKPOKE
		} else if(ownbuf[i] == 'R') {
			put = 0;
			hex = 0;
			if(sp < 1) {
				return -1;
			} else {
				int top = stack[--sp];
				stack[sp++] = (int)*(unsigned char*)top;
			}
			stack[sp] = 0;
#endif
		} else if(ownbuf[i] == '+' || ownbuf[i] == '-' || ownbuf[i] == '*' || ownbuf[i] == '/') {
			put = 0;
			hex = 0;
			if(sp < 2) {
				return -1;
			} else {
				int top = stack[--sp];
				int bottom = stack[--sp];
				int value = 0;
				if(ownbuf[i] == '+') {
					value = top + bottom;
				} else if(ownbuf[i] == '-') {
					value = bottom + top;
				} else if(ownbuf[i] == '*') {
					value = mull(top, bottom);
				} else if(ownbuf[i] == '/') {
					value = divl(bottom, top);
				}
				stack[sp++] = value;
			}
			stack[sp] = 0;
		} else if(ownbuf[i] == ' ' && put == 1) {
			stack[++sp] = 0;
		}
	}
	result = stack[0];
	*number = result;
	return 1;
}

/* arr gets sorted, arr2 index gets replaced */
void sort(int* arr, int* arr2, int size) {
	int i;
redo:
	for(i = 1; i < size; i++) {
		if(arr[i - 1] > arr[i]) {
			int tmp = arr[i];
			arr[i] = arr[i - 1];
			arr[i - 1] = tmp;
			tmp = arr2[i];
			arr2[i] = arr2[i - 1];
			arr2[i - 1] = tmp;
		}
	}
	for(i = 1; i < size; i++) {
		if(arr[i - 1] > arr[i]) {
			goto redo;
		}
	}
}

int run(char* cmd, int linenum, char num, int* lgoto) {
	char line[LINE_BUFFER_SIZE];
	char rcmd[32];
	int i;
	char* arg;
	int incr = 0;
#ifdef BREAKKEY
	if(agetch() == 1) return -1;
#endif
	if(lgoto != 0) *lgoto = 0;
	for(i = 0; cmd[i] != 0; i++) line[i] = cmd[i];
	line[i] = 0;
	rcmd[0] = 0;
	for(i = 0;; i++) {
		if(line[i] == ' ' || line[i] == '\t' || line[i] == 0 || line[i] == '"') {
			break;
		} else {
			rcmd[incr++] = line[i];
			if(incr == 32) {
				putstr("! Command too long");
				if(linenum != -1) {
					putstr(" in line ");
					putnum(linenum);
				}
				putstr(NEWLINE);
				return 1;
			}
			rcmd[incr] = 0;
		}
	}
	arg = line + 1 + strlen(rcmd);
	if(strcaseequ(rcmd, "COLOR")) {
		int argc = 0;
		char* farg = 0;
		char* sarg = 0;
		int fgcolor, bgcolor, ret0, ret1;
		if(arg[0] != 0) argc++;
		for(i = 0; arg[i] != 0; i++) {
			if(arg[i] == ',') {
				arg[i] = 0;
				farg = arg;
				sarg = arg + i + 1;
				for(; *sarg != 0 && (*sarg == '\t' || *sarg == ' '); sarg++)
					;
				argc++;
			}
		}
		if(argc != 2) {
			putstr("! Invalid argument");
			if(linenum != -1) {
				putstr(" in line ");
				putnum(linenum);
			}
			putstr(NEWLINE);
			return 1;
		}
		bgcolor = 0;
		fgcolor = 0;
		ret0 = pexpr(farg, 0, &bgcolor);
		ret1 = pexpr(sarg, 0, &fgcolor);
		if(ret0 == 0) {
			putstr("! Invalid argument");
			if(linenum != -1) {
				putstr(" in line ");
				putnum(linenum);
			}
			putstr(NEWLINE);
			return 1;
		} else if(ret0 == -1) {
			putstr("! Syntax error");
			if(linenum != -1) {
				putstr(" in line ");
				putnum(linenum);
			}
			putstr(NEWLINE);
			return 1;
		}
		if(ret1 == 1) {
			change_color((fgcolor << 4) | bgcolor);
		} else if(ret1 == 0) {
			putstr("! Invalid argument");
			if(linenum != -1) {
				putstr(" in line ");
				putnum(linenum);
			}
			putstr(NEWLINE);
			return 1;
		} else if(ret1 == -1) {
			putstr("! Syntax error");
			if(linenum != -1) {
				putstr(" in line ");
				putnum(linenum);
			}
			putstr(NEWLINE);
			return 1;
		}
	} else if(strcaseequ(rcmd, "PRINT")){
		if(arg[0] != 0){
			char sc = 0;
			int incr = 0;
			for(i = 0;; i++) {
				if(arg[i] != 0) sc = 0;
				if(arg[i] == ';' || arg[i] == 0) {
					char oldc = arg[i];
					arg[i] = 0;

					if(strlen(arg + incr) > 0){	
						char buffer[128];
	
						int number;
						int ret = pexpr(arg + incr, buffer, &number);
	
						if(ret == 0){
							putstr(buffer);
						}else if(ret == 1){
							putstr(" ");
							putnum(number);
						}else if(ret == -1){
							putstr("! Syntax error");
							if(linenum != -1) {
								putstr(" in line ");
								putnum(linenum);
							}
							putstr(NEWLINE);
							return 1;
						}
					}
					
					incr = i + 1;
					if(oldc == ';') sc = 1;
					if(oldc == 0) break;
				}
			}
			if(!sc) putstr(NEWLINE);
		}else{
			putstr(NEWLINE);
		}
#ifndef NO_PEEKPOKE
	} else if(strcaseequ(rcmd, "POKE")) {
		int argc = 0;
		char* farg = 0;
		char* sarg = 0;
		int addr, data, ret0, ret1;
		if(arg[0] != 0) argc++;
		for(i = 0; arg[i] != 0; i++) {
			if(arg[i] == ',') {
				arg[i] = 0;
				farg = arg;
				sarg = arg + i + 1;
				for(; *sarg != 0 && (*sarg == '\t' || *sarg == ' '); sarg++)
					;
				argc++;
			}
		}
		if(argc != 2) {
			putstr("! Invalid argument");
			if(linenum != -1) {
				putstr(" in line ");
				putnum(linenum);
			}
			putstr(NEWLINE);
			return 1;
		}
		addr = 0;
		data = 0;
		ret0 = pexpr(farg, 0, &addr);
		ret1 = pexpr(sarg, 0, &data);
		if(ret0 == 0) {
			putstr("! Invalid argument");
			if(linenum != -1) {
				putstr(" in line ");
				putnum(linenum);
			}
			putstr(NEWLINE);
			return 1;
		} else if(ret0 == -1) {
			putstr("! Syntax error");
			if(linenum != -1) {
				putstr(" in line ");
				putnum(linenum);
			}
			putstr(NEWLINE);
			return 1;
		}
		if(ret1 == 1) {
			unsigned char* a = (unsigned char*)addr;
			*a = data;
		} else if(ret1 == 0) {
			putstr("! Invalid argument");
			if(linenum != -1) {
				putstr(" in line ");
				putnum(linenum);
			}
			putstr(NEWLINE);
			return 1;
		} else if(ret1 == -1) {
			putstr("! Syntax error");
			if(linenum != -1) {
				putstr(" in line ");
				putnum(linenum);
			}
			putstr(NEWLINE);
			return 1;
		}
#endif
	} else if(num == 0 && strcaseequ(rcmd, "LIST")) {
		int addr = BUFFER_SIZE - 1;
		int saddr = 0;
		int lbuf[LINES];
		int shift[LINES];
		int cnt = 0;
		int i;
		while(1) {
			unsigned long ln = 0;
			for(i = 0; i < 4; i++) {
				ln >>= 8;
				ln |= (unsigned long)basicbuffer[addr--] << 24;
			}
			if(ln == 0) break;
			lbuf[cnt] = ln;
			shift[cnt++] = saddr;
			saddr += strlen(basicbuffer + saddr) + 1;
		}
		sort(lbuf, shift, cnt);
		for(i = 0; i < cnt; i++) {
			putnum(lbuf[i]);
			putstr(" ");
			putstr(basicbuffer + shift[i]);
			putstr(NEWLINE);
		}
	} else if(num == 1 && strcaseequ(rcmd, "GOTO")) {
		int ret = pexpr(arg, 0, lgoto);
		if(ret == 0) {
			putstr("! Invalid argument");
			if(linenum != -1) {
				putstr(" in line ");
				putnum(linenum);
			}
			putstr(NEWLINE);
			return 1;
		} else if(ret == -1) {
			putstr("! Syntax error");
			if(linenum != -1) {
				putstr(" in line ");
				putnum(linenum);
			}
			putstr(NEWLINE);
			return 1;
		}
	} else if(num == 0 && strcaseequ(rcmd, "RUN")) {
		int addr = BUFFER_SIZE - 1;
		int saddr = 0;
		int lbuf[LINES];
		int shift[LINES];
		int cnt = 0;
		int gt, i;
		while(1) {
			unsigned long ln = 0;
			for(i = 0; i < 4; i++) {
				ln >>= 8;
				ln |= (unsigned long)basicbuffer[addr--] << 24;
			}
			if(ln == 0) break;
			lbuf[cnt] = ln;
			shift[cnt++] = saddr;
			saddr += strlen(basicbuffer + saddr) + 1;
		}
		sort(lbuf, shift, cnt);
		for(i = 0; i < cnt; i++) {
			int ret = run(basicbuffer + shift[i], lbuf[i], 1, &gt);
			if(ret != 0) return ret;
			if(gt != 0) {
				char found;
				for(i = 0; i < cnt; i++) {
					if(lbuf[i] == gt) {
						found = 1;
						break;
					}
				}
				if(found) {
					i--;
				} else {
					putstr("! GOTO no such line");
					if(linenum != -1) {
						putstr(" in line ");
						putnum(linenum);
					}
					putstr(NEWLINE);
					return 1;
				}
			}
		}
	} else {
		putstr("! Unknown command");
		if(linenum != -1) {
			putstr(" in line ");
			putnum(linenum);
		}
		putstr(NEWLINE);
		return 1;
	}
	return 0;
}

int execute(int linenum, char* cmd, char num) {
	int i;
	char line[LINE_BUFFER_SIZE];
	int incr = 0;
	char dq = 0;
	for(i = 0; cmd[i] != 0; i++) {
		if(cmd[i] == ' ' || cmd[i] == '\t') {
			if(!dq) {
				for(; cmd[i] != 0 && (cmd[i] == ' ' || cmd[i] == '\t'); i++)
					;
				i--;
			}
			line[incr++] = cmd[i];
		} else if(cmd[i] == '"') {
			line[incr++] = '"';
			dq = dq == 0 ? 1 : 0;
		} else {
			line[incr++] = dq == 0 ? toupper(cmd[i]) : cmd[i];
		}
	}
	line[incr] = 0;
	if(num == 0) {
		int ret = run(line, -1, 0, 0);
		putstr("Ready");
		putstr(NEWLINE);
		return ret;
	} else {
		int addr = BUFFER_SIZE - 1;
		int i;
		int shf = 0;
		int cnt = 0;
		int len;
		while(1) {
			unsigned long ln = 0;
			for(i = 0; i < 4; i++) {
				ln >>= 8;
				ln |= (unsigned long)basicbuffer[addr--] << 24;
			}
			cnt++;
			if(ln == linenum) shf = cnt;
			if(shf != 0) {
				addr += 4;
				for(i = 0; i < 4; i++) {
					basicbuffer[addr] = basicbuffer[addr - 4 * shf];
					addr--;
				}
				addr += 4;
				ln = 0;
				for(i = 0; i < 4; i++) {
					ln >>= 8;
					ln |= (unsigned long)basicbuffer[addr--] << 24;
				}
			}
			if(ln == 0) break;
		}
		if(line[0] != 0) {
			addr += 4;
			for(i = 0; i < 4; i++) {
				basicbuffer[addr--] = linenum & 0xff;
				linenum >>= 8;
			}
		}
		len = 0;
		cnt = 0;
		while(1) {
			int slen = strlen(basicbuffer + len);
			if(slen == 0) break;
			len += slen + 1;
			cnt++;
		}
		if(line[0] != 0) {
			for(i = 0; line[i] != 0; i++) {
				basicbuffer[len + i] = line[i];
			}
			basicbuffer[len + i] = 0;
			basicbuffer[len + i + 1] = 0;
		}
		if(shf != 0) {
			cnt = 0;
			len = 0;
			while(1) {
				int slen = strlen(basicbuffer + len);
				if(slen == 0) break;

				len += slen + 1;
				cnt++;

				if(cnt == shf) {
					int i;
					int nc = 0;
					len -= slen + 1;
					for(i = len;; i++) {
						basicbuffer[i] = basicbuffer[i + slen + 1];
						if(basicbuffer[i] == 0) {
							nc++;
							if(nc == 2) break;
						} else {
							nc = 0;
						}
					}
					break;
				}
			}
		}
		return 0;
	}
}

void basic(void) {
	int i;
	int lineind;
	clear();

#ifdef SMALL
	putstr("Krakow BASIC  Ver. ");
	putstr(VERSION);
	putstr(NEWLINE);
	putstr(NEWLINE);
#else
	putstr(PLATFORM);
	putstr("   Krakow BASIC V");
	putstr(VERSION);
	putstr(NEWLINE);
	putstr("Copyright 2024 by: Nishi.");
	putstr(NEWLINE);
	putstr("                   penguin2233.");
	putstr(NEWLINE);
	putstr(NEWLINE);
	putstr(" Max ");
	putnum(LINE_BUFFER_SIZE);
	putstr(" characters per line");
	putstr(NEWLINE);
	putstr(" Max ");
	putnum(LINES);
	putstr(" lines");
	putstr(NEWLINE);
	putstr(" ");
	putstr("Variable buffer ");
	putnum(BUFFER_SIZE / 2);
	putstr(" bytes");
	putstr(NEWLINE);
	putstr(" ");
#endif
	putnum(BUFFER_SIZE / 2);
	putstr(" bytes free");
	putstr(NEWLINE);
	putstr(NEWLINE);

	for(i = 0; i < BUFFER_SIZE / 2; i++) {
		basicbuffer[i] = 0;
	}
	putstr("Ready");
	putstr(NEWLINE);

	cursor();
	linebuf[0] = 0;
	lineind = 0;
	while(1) {
		char c;
#if defined(PLATFORM_C64) || defined(PLATFORM_A800XL) || defined(PLATFORM_APPLE2) || defined(PLATFORM_PET) || defined(PLATFORM_MSX)
		c = oggetch(1);
#else
		c = agetch();
#endif
		if(c != 0) {
			killcursor();
		}
		if(c == 1) {
			putstr("Break");
			putstr(NEWLINE);
			lineind = 0;
		} else if(c == '\n') {
			int i;
			char num = 1;
			char* cmd = linebuf;

			linebuf[lineind] = 0;
			putstr(NEWLINE);
			if(lineind == 0) goto skip;

			for(i = 0; linebuf[i] != 0; i++) {
				if(linebuf[i] == ' ') {
					linebuf[i] = 0;
					cmd = linebuf + i + 1;
					break;
				} else if(!('0' <= linebuf[i] && linebuf[i] <= '9')) {
					num = 0;
					break;
				}
			}

			if(num == 1 && strnum(linebuf) == 0) {
				putstr("! Line number 0 is illegal");
				putstr(NEWLINE);
			} else {
				int ret = execute(num == 1 ? strnum(linebuf) : -1, (num == 1 && cmd == linebuf) ? "" : cmd, num);
				if(ret == -1) {
					putstr("! Break");
					putstr(NEWLINE);
				}
			}

		skip:
			lineind = 0;
		} else if(c == 0x8 || c == 0x7f) {
			if(lineind > 0) {
#if defined(PLATFORM_A800XL)
				putstr("\x7e \x7e");
#else
				putstr("\x08 \x08");
#endif
				linebuf[--lineind] = 0;
			}
		} else if(c == -1) {
			break;
		} else if(c != 0) {
			putchar(c);
			if(lineind <= LINE_BUFFER_SIZE){
				linebuf[lineind++] = c;
			}
		}
		cursor();
	}
}
