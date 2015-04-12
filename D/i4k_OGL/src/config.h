//--------------------------------------------------------------------------//
// iq / rgba  .  tiny codes  .  2008                                        //
//--------------------------------------------------------------------------//

#ifndef _CONFIG_H_
#define _CONFIG_H_

#define NOINLINER

// #define INLINER			// 3950
// #define INLINER inline		// 3950
// #define INLINER inline static	// 3938
// #define INLINER static		// 3933
// #define INLINER __forceinline	// 3883
#define INLINER __forceinline static	// 3883

// #define LOOP_(i, n) for (unsigned int i = 0; i < n; ++i)
// #define LOOP_(i, n) for (int i = n - 1; i >= 0; --i)
#define LOOP_(i, n) int i = n; while(i-- > 0)

#ifndef DEBUG

#ifdef RESOLUTION_1920_1200
# define XRES 1920
# define YRES 1200
# define TRES 512
# define VPY  60
# define VPH  1080
#endif

#ifdef RESOLUTION_1920_1080
# define XRES 1920
# define YRES 1080
# define TRES 512
# define VPY  0
# define VPH  1080
#endif

#ifdef RESOLUTION_1280_1024
# define XRES 1280
# define YRES 1024
# define TRES 512
# define VPY  152
# define VPH  720
#endif

#ifdef RESOLUTION_1280_800
# define XRES 1280
# define YRES 800
# define TRES 512
# define VPY  40
# define VPH  720
#endif

#ifdef RESOLUTION_1280_720
# define XRES 1280
# define YRES 720
# define TRES 512
# define VPY  0
# define VPH  720
#endif

#ifdef RESOLUTION_1024_768
# define XRES 1024
# define YRES 768
# define TRES 512
# define VPY  96
# define VPH  576
#endif

#else
//#define USEDSOUND
//#define CLEANDESTROY          // destroy stuff (windows, glContext, ...)

#define XRES         1024
#define YRES         576
#define TRES         512
#define VPY          0
#define VPH          576

#endif

#endif
