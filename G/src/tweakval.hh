#ifndef TWEAKVAL_H
#define TWEAKVAL_H
//=========================================================
// tweakval.hh
//
// Author: Joel Davis (joeld42@yahoo.com)
// 
// Based on a discussion on this thread:
// https://mollyrocket.com/forums/viewtopic.php?t=556
// See that thread for details.
//
// USAGE: Anywhere you're using a constant value that 
// you want to be tweakable, wrap it in a _TV() macro. 
// In a release build (with -DNDEBUG defined) this 
// will simply compile out to the constant, but in a 
// debug build this will scan the source file for 
// a modified value, providing a "live update" in 
// the game.
//
// Simply edit the value in the source file, and save the file
// while the game is running, and you'll see the change.
//
//  EXAMPLES:
//     // Move at constant (tweakable) speed
//     float newPos = oldPos + _TV(5.2f) * dt;
//
//     // Clear to a solid color
//     glClearColor( _TV( 0.5 ), _TV( 0.2 ), _TV( 0.9 ), 1.0 );
//     glClear( GL_COLOR_BUFFER_BIT );
//
//     // initial monster health
//     Monster *m = new Monster( _TV( 10 ) );
//
//   WARNING: 
//   This is currently in a very rough state, it doesn't
//   handle errors, c-style comments, and adding and removing or
//   rearranging TV's will probably confuse it.
//   Also, putting _TV's in header files is not a good idea.
//=========================================================




// Note: gcc supports __COUNTER__ in 4.3+




#ifdef __GNUC__
#  define GCC_VERSION (__GNUC__ * 10000 \
                       + __GNUC_MINOR__ * 100 \
                       + __GNUC_PATCHLEVEL__ )


#  define TV_HAS_COUNTER (GCC_VERSION > 40300)
#else
// msvc supports __COUNTER__ in 6.0+ which is old
// enough to assume... not sure about other compilers
#  define TV_HAS_COUNTER 1
#endif




// Do only in debug builds
#ifndef DEBUG
#  define TV_USE_TWEAKVAL 0
#else
#  define TV_USE_TWEAKVAL 1
#endif


#if LINUX
#  include <stddef.h>
#endif



//
// See the thread referenced above for idea of how to implement
// this without the __COUNTER__ macro.


// If we are in a build modethat wants tweakval, and the compiler
// supports it, use it
#if TV_USE_TWEAKVAL && TV_HAS_COUNTER
#  define _TV(Val) _TweakValue( __FILE__, __COUNTER__, Val )

float _TweakValue( const char *file, size_t counter, float origVal );
int _TweakValue( const char *file, size_t counter, int origVal );
char *_TweakValue( const char *file, size_t counter, char *origVal );

void ReloadChangedTweakableValues();


#else
// don't use it
#  define _TV(Val) (Val)
#  define ReloadChangedTweakableValues()


#endif

namespace tweakval { extern bool isEnable; }


#endif
