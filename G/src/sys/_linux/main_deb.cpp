//--------------------------------------------------------------------------//
// iq . 2003 . code for the Paradise 64 kb intro by RGBA                    //
//--------------------------------------------------------------------------//

#define XRES        800
#define YRES        600

#define ERRORMSG            // activa printado mensajes de error
#define LOADING             // activa pintado "wait while loading..."
#define AUTOCENTER          // activa centrado de la ventana


#include <X11/X.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <string.h>
#include <stdio.h>
#include "../../intro.hh"
#include "../../sys/mmalloc.h"
#include "../../sys/font.h"

//----------------------------------------------------------------------------

typedef struct
{
    //---------------
    Display     *hDisplay;
    GLXContext  hRC;
    Window      hWnd;
    //---------------
    int         full;
    //---------------
    char        wndclass[11];	// window class and title :)
    //---------------
} win_info_t;

static int doubleBufferVisual[]  =
{
        GLX_RGBA,           // Needs to support OpenGL
        GLX_DEPTH_SIZE, 24, // Needs to support a 16 bit depth buffer
        GLX_DOUBLEBUFFER,   // Needs to support double-buffering
        None                // end of list
};


static win_info_t wininfo = {  0,0,0,
							1,	// full
							{'r','g','b','a','_','i','n','t','r','o',0}
                            };

#ifdef LOADING
static char fnt_wait[6]    = "arial";
static char msg_wait[22]   = "wait while loading...";
#endif
#ifdef ERRORMSG
static char msg_error[207] = "intro_init()!\n\n"\
                             "  no memory?\n"\
                             "  no music?\n"\
                             "  no GL_ARB_multitexture?\n"\
                             "  no GL_ARB_vertex_program?\n"\
                             "  no GL_ARB_fragment_program?\n"\
							 "  no GL_EXT_texture_lod_bias?\n"\
                             "  no GL_EXT_texture3D?\n"\
                             "  no GL_SGIS_generate_mipmap?";

static char tlt_error[6]   = "error";
#endif




//----------------------------------------------------------------------------

// // n = [0..224]
// void loadbar( int  n )
// {
// /*
//     win_info_t *info = &wininfo;

//     int     xo = 16*(XRES)>>8;
//     int     y1 = (240*YRES)>>8;
//     int     yo = y1-8;

//     // draw background
//     SelectObject( wininfo.hDC, CreateSolidBrush(0x0045302c) );
//     Rectangle( wininfo.hDC, 0, 0, XRES, YRES );

//     // draw text
//     SetBkMode( wininfo.hDC, TRANSPARENT );
//     SetTextColor( wininfo.hDC, 0x00ffffff );
//     SelectObject( wininfo.hDC, CreateFont( 44,0,0,0,0,0,0,0,0,0,0,ANTIALIASED_QUALITY,0,fnt_wait) );
//     TextOut( wininfo.hDC, (XRES-318)>>1, (YRES-38)>>1, msg_wait, 21 );
//     //DrawText( info->hDC, msg_wait, 21, &rec, DT_CENTER|DT_VCENTER|DT_SINGLELINE );

//     // draw bar
//     SelectObject( wininfo.hDC, CreateSolidBrush(0x00705030) );
//     Rectangle( wininfo.hDC, xo, yo, (240*XRES)>>8, y1 );
//     SelectObject( wininfo.hDC, CreateSolidBrush(0x00f0d0b0) );
//     Rectangle( wininfo.hDC, xo, yo, ((16+n)*XRES)>>8, y1 );
//     */
//     //iqiq TODO
// }


//----------------------------------------------------------------------------

static void window_end( win_info_t *info )
{
    XDestroyWindow( info->hDisplay, info->hWnd );
    XCloseDisplay( info->hDisplay );
}

static int window_init( win_info_t *info )
{
    XVisualInfo *visualInfo;
    int         errorBase;
    int         eventBase;

    info->hDisplay = XOpenDisplay( NULL );
    if( !info->hDisplay )
        return( 0 );

    // Make sure OpenGL's GLX extension supported
    if( !glXQueryExtension( info->hDisplay, &errorBase, &eventBase ) )
        return( 0 );

    // Try for the double-bufferd visual first
    visualInfo = glXChooseVisual( info->hDisplay, DefaultScreen(info->hDisplay), doubleBufferVisual );
    if( visualInfo == NULL )
        return( 0 );

    // Create an OpenGL rendering context
    info->hRC = glXCreateContext( info->hDisplay, visualInfo, NULL, GL_TRUE );
    if( info->hRC == NULL )
        return( 0 );

    // Create an X colormap since we're probably not using the default visual
    Colormap colorMap;
    colorMap = XCreateColormap( info->hDisplay, RootWindow(info->hDisplay, visualInfo->screen),
                                visualInfo->visual, AllocNone );

    XSetWindowAttributes winAttr;
    winAttr.colormap     = colorMap;
    winAttr.border_pixel = 0;
    winAttr.event_mask   = ExposureMask           |
                           VisibilityChangeMask   |
                           KeyPressMask           |
                           KeyReleaseMask         |
                           ButtonPressMask        |
                           ButtonReleaseMask      |
                           PointerMotionMask      |
                           StructureNotifyMask    |
                           SubstructureNotifyMask |
                           FocusChangeMask;

    // Create an X window with the selected visual
    info->hWnd = XCreateWindow( info->hDisplay, RootWindow(info->hDisplay, visualInfo->screen),
                              0, 0, XRES, YRES, 0, visualInfo->depth, InputOutput,
				visualInfo->visual, CWBorderPixel | CWColormap | CWEventMask,
                              &winAttr );

    if( !info->hWnd )
        return( 0 );

    //XSetStandardProperties( info->hDisplay, info->hWnd, info->wndclass, info->wndclass, None, argv, argc, NULL );
    char *argv[] = { "hola", 0 };
    XSetStandardProperties( info->hDisplay, info->hWnd, info->wndclass,
    info->wndclass, None, argv, 1, NULL );

    glXMakeCurrent( info->hDisplay, info->hWnd, info->hRC );


    XMapWindow( info->hDisplay, info->hWnd );


    return( 1 );
}


//----------------------------------------------------------------------------

//void entrypoint( void )
int main( void )
{
    XEvent      event;
    int         done=0;
    win_info_t     *info = &wininfo;


    if( !smalloc_init() )
    {
        #ifdef ERRORMSG
        printf( "window_init()!" );
        #endif
        return( 1 );
    }

/*
    if( MessageBox( 0, "fullscreen?", info->wndclass, MB_YESNO|MB_ICONQUESTION)==IDYES )
        info->full++;
*/
    if( !window_init(info) )
    {
        window_end( info );
        #ifdef ERRORMSG
        printf( "window_init()!" );
        #endif
        return( 2 );
    }

    FONT_Init( (long)info->hDisplay );

    if( !intro_init( XRES, YRES, 0, (int)info->hWnd ) )
    {
        window_end( info );
        #ifdef ERRORMSG
        printf( msg_error );
        #endif
        return( 3 );
    }

    while( !done )
    {
        done = intro_do();

	while( XPending(info->hDisplay) )
	{
	  XNextEvent( info->hDisplay, &event );
	  switch( event.type )
	  {
	  case KeyPress:
	    switch( XKeycodeToKeysym( info->hDisplay, event.xkey.keycode, 0 ) )
	    {
	    case XK_Escape:
	      done = 1;
	      break;
	    }

	    break;
	  case DestroyNotify:
	    done = 1;
	    break;
	  }
	}
        glXSwapBuffers( info->hDisplay, info->hWnd );
    }

    intro_end();

    window_end( info );

    smalloc_end();

    return( 0 );
}
