#include <stdlib.h>
#include <stdio.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

static char *title = "xbar" ;
static float prevval=1. , val = 1. ;
static XWindowAttributes wa;
static unsigned long black, white;
static XEvent ev;

enum{
	InitWinWidth = 200,
	InitWinHeight = 300,
	ExitKey = 'q',
	PressIteration = 0.05,
	UpKey = 'k',
	DownKey = 'j',
};

Display *dpy;
int screen;
Window win, root;
/* Graphical context. */
GC gc;

void
xinit(void)
{

	dpy = XOpenDisplay(0) ;
	screen = DefaultScreen(dpy) ;
	root = DefaultRootWindow(dpy) ;
	black = BlackPixel(dpy, screen) ;
	white = WhitePixel(dpy, screen) ;
	win = XCreateSimpleWindow(dpy, root,
			0, 0,
			InitWinWidth, InitWinHeight,
			0,
			white, black) ;
	gc = XCreateGC(dpy, win, 0, 0) ;

	XSetStandardProperties(dpy, win, title, PROGNAME, None, 0, 0, 0);
	XSelectInput(dpy, win, ExposureMask|ButtonPressMask|KeyPressMask|Button1MotionMask);
	XSetBackground(dpy, gc, black);
	XSetForeground(dpy, gc, white);
	XClearWindow(dpy, win);
	XMapRaised(dpy, win);
}

void
xexit(void)
{
	XFreeGC(dpy, gc);
	XDestroyWindow(dpy, win);
	XCloseDisplay(dpy);
}

void
xredraw(void)
{

	XClearArea(dpy, win,
		0, 0,
		wa.width, wa.height - wa.height*val, False);
	XFillRectangle(dpy, win, gc,
		0, wa.height - wa.height*val,
		wa.width, wa.height*val);
}

void
xwork(void)
{
	KeySym key;
	char buf[BUFSIZ];
	char str[BUFSIZ];
	while(1){
		XNextEvent(dpy, &ev);
		if( ev.type==Expose && ev.xexpose.count==0){
			XGetWindowAttributes(dpy, win, &wa);
			xredraw();
		}else if( ev.type==KeyPress
			&& XLookupString( &ev.xkey, buf, sizeof(buf), &key, 0) == 1
				){
			if(*buf == ExitKey) return ;
		} else if (ev.type==ButtonPress || ev.type==MotionNotify){
			if(ev.xbutton.y < 0 || ev.xbutton.y > wa.height)
				continue ;
			val = (float)(wa.height - ev.xbutton.y) / wa.height ;
			xredraw();

			if(val != prevval){
				sprintf(str, "%f\n", val);
				write(1, str, strlen(str));
				prevval = val ;
			}
		} 
	}
}

int
main(int argc, char *argv[])
{
	xinit();
	xwork();
	xexit();
	return 0 ;
}

