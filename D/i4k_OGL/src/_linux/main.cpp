
#include <stdlib.h>
#include <string.h>
#include <SDL/SDL.h>
#include "syna.h"

int quit=0;
extern char biisi[];
extern void analyysi(void *);
extern int osa(int);

void pakki(void *udata, Uint8 *stream, int len)
{
    syna_play((short *)stream,len/4);
    analyysi(stream);
}

int main(int argc,char *argv[])
{
    int orig,flags=SDL_OPENGL|SDL_FULLSCREEN;
    SDL_Event e;
    SDL_AudioSpec w;

    if(argc>1)
        if(!strcmp(argv[1],"-w"))
            flags-=SDL_FULLSCREEN;

    if(argv[0][strlen(argv[0])-1]=='w')
            flags-=SDL_FULLSCREEN;

    SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_TIMER);

    w.freq=44100;
    w.format=AUDIO_S16SYS;
    w.channels=2;
#ifdef BLKSIZE
    w.samples=BLKSIZE;
#else
    w.samples=1024;
#endif
    w.callback=pakki;
    w.userdata=NULL;
    SDL_OpenAudio(&w,NULL);
    syna_init(44100);
    if(syna_get(biisi)<0)
        return(EXIT_FAILURE);

    SDL_SetVideoMode(1024,768,32,flags);

    SDL_WM_SetCaption("Yellow Rose of Texas",0);
    SDL_ShowCursor(0);
    SDL_PauseAudio(0);

    orig=SDL_GetTicks();
    while(!quit)
    {
        if(osa(SDL_GetTicks()-orig)==0)
            SDL_GL_SwapBuffers();

        while(SDL_PollEvent(&e)>0)
        {
            if(e.type==SDL_MOUSEBUTTONDOWN)
                quit=1;
            if(e.type==SDL_KEYDOWN)
                quit=1;
        }
    }

    SDL_Quit();
    return(0);
}
