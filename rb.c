#ifdef GLX
#include <GL/glx.h>
#include <sys/unistd.h>
#define EV(y) ev.x##y
#else
#include <SDL.h>
#include <SDL_opengl.h>
#define ButtonPress SDL_MOUSEBUTTONDOWN
#define ButtonRelease SDL_MOUSEBUTTONUP
#define MotionNotify SDL_MOUSEMOTION
#define ClientMessage SDL_QUIT
#define EV(y) ev.y
#endif
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define case(x) break;case x:;
int main(int argc,char**argv){
	#ifdef SDL
	if(SDL_Init(SDL_INIT_VIDEO)==-1){
		fputs(SDL_GetError(),stderr);
		return 1;
	}
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);
	SDL_Surface*dpy=SDL_SetVideoMode(512,512,0,SDL_OPENGL);
	#else
	Display*dpy=XOpenDisplay(0);
	XVisualInfo*vi=glXChooseVisual(dpy,DefaultScreen(dpy),(int[]){GLX_RGBA,GLX_DOUBLEBUFFER,None});
	Window Wdo=XCreateWindow(dpy,RootWindow(dpy,vi->screen),0,0,512,512,0,vi->depth,InputOutput,vi->visual,CWColormap|CWEventMask,(XSetWindowAttributes[]){{.colormap=XCreateColormap(dpy,RootWindow(dpy,vi->screen),vi->visual,AllocNone),.event_mask=PointerMotionMask|ButtonPressMask|ButtonReleaseMask}});
	XSetWMProtocols(dpy,Wdo,(Atom[]){XInternAtom(dpy,"WM_DELETE_WINDOW",False)},1);
	XMapWindow(dpy,Wdo);
	glXMakeCurrent(dpy,Wdo,glXCreateContext(dpy,vi,0,GL_TRUE));
	#endif
	srand(time(0));
	_Bool mb=0;
	unsigned short mx1,my1,mx2,my2,exy[64][4];
	unsigned char ep=0;
	int en=16384;
	glEnable(GL_COLOR_LOGIC_OP);
	glLogicOp(GL_XOR);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2,GL_SHORT,8,exy);
	glOrtho(0,512,512,0,1,-1);
	glColor3ub(255,255,255);
	for(;;){
		#ifdef GLX
		glXSwapBuffers(dpy,Wdo);
		XEvent ev;
		while(XPending(dpy)){
			XNextEvent(dpy,&ev);
		#else
		SDL_GL_SwapBuffers();
		SDL_Event ev;
		while(SDL_PollEvent(&ev)){
		#endif
			switch(ev.type){
			case(MotionNotify)
				mx2=EV(motion.x);
				my2=EV(motion.y);
			case(ButtonPress)
				mb=1;
				mx1=EV(motion.x);
				my1=EV(motion.y);
			case(ButtonRelease)
				mb=0;
				mx2=EV(motion.x);
				my2=EV(motion.y);
				if(mx1>mx2){
					short t=mx1;
					mx1=mx2;
					mx2=t;
				}
				if(my1>my2){
					short t=my1;
					my1=my2;
					my2=t;
				}
				int b=0;
				for(int i=0;i<ep;i++)b+=exy[i][0]<=mx2&&exy[i][0]>=mx1&&exy[i][1]<=my2&&exy[i][1]>=my1;
				if(b){
					en+=(b*(b-1)<<6)+b*96<<2;
					for(int i=0;i<ep;){
						en+=(exy[i][2]+!!exy[i][3]>>1)+1<<3;
						if(exy[i][0]<=mx2+b*b*b&&exy[i][0]>=mx1-b*b*b&&exy[i][1]<=my2+b*b*b&&exy[i][1]>=my1-b*b*b){
							memmove(exy[i],exy[i+1],ep-i<<3);
							ep--;
							continue;
						}
						i++;
					}
				}
				en-=(mx2-mx1)*(my2-my1);
			case(ClientMessage)return 0;
			}
		}
		if(ep<64&&!(rand()&3)){
			exy[ep][0]=0;
			exy[ep][1]=rand()&511;
			exy[ep][2]=1+(rand()&3);
			exy[ep][3]=rand()&3?(exy[ep][1]<240?1:-1):0;
			ep++;
		}
		glClear(GL_COLOR_BUFFER_BIT);
		glRecti(0,0,512,en>>9);
		glBegin(GL_LINES);
		glVertex2i(0,en>>9);
		glVertex2i(en&511,en>>9);
		glEnd();
		for(int i=0;i<ep;){
			exy[i][0]+=exy[i][2];
			exy[i][1]+=exy[i][3];
			if(exy[i][0]>512){
				memmove(exy[i],exy[i+1],ep-i<<3);
				ep--;
				en-=exy[i][2];
				continue;
			}
			i++;
		}
		glDrawArrays(GL_POINTS,0,ep);
		if(mb){
			glBegin(GL_LINE_LOOP);
			glVertex2i(mx1,my1);
			glVertex2i(mx2,my1);
			glVertex2i(mx2,my2);
			glVertex2i(mx1,my2);
			glEnd();
		}
		#ifdef GLX
		usleep(60000-en/5);
		#else
		SDL_Delay(60-en/5000);
		#endif
	}
}