//--------------------------------------------------------------------------//
// iq / rgba  .  tiny codes  .  2008                                        //
//--------------------------------------------------------------------------//

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "glext.h" // Shaders
// #include "exwt.h" // Shaders
#include "intro.h"
#include "config.h"
#include "mzk.h"
#include "words.h"

//---------------------------------------------------------------------

// From IQ: http://iquilezles.org/www/articles/float4k/float4k.htm
#define p0d01 0.0100097656f     // 0.01f    0x3c240000
#define p0d70 0.6992187500f     // 0.70f    0x3f330000
#define p0d90 0.8984375000f     // 0.90f    0x3f660000
#define p1d01 1.0078125000f     // 1.01f    0x3f810000
#define p1d78 1.7812500000f     // 1.78f    0x3fe40000


#pragma data_seg(".projection_matrix")
static const float projectionMatrix[16] = {
  1.f, 0.f, 0.f,  0.f,
  0.f, p1d78, 0.f,  0.f, // 16/9 // 1.778f
  0.f, 0.f, -1.f, -1.f,
  0.f, 0.f, 0.f,  1.f };

#pragma data_seg(".vertices")
static const float faceVerts[] = {
  1.f, -1.f,
  1.f,  1.f,
 -1.f,  1.f,
 -1.f, -1.f };

#pragma data_seg(".texcoords")
static const float faceCoords[] = {
  1.f,  0.f,
  1.f,  1.f,
  0.f,  1.f,
  0.f,  0.f };

static int pid;

static PFNGLUSEPROGRAMPROC glUseProgram = NULL;

#pragma data_seg(".bg_colors")
struct bgColor
{
  short beat;
  unsigned char r;
  unsigned char g;
  unsigned char b;
};
static bgColor bgColors[] = {
  {  0,   0,   0,   0}, // Fondu noir de début
  { 32,  30,  87, 156}, // Nuit
  { 94, 106, 154, 228}, // Jour
  { 95, 255, 180, 160}, // Aube
  {159, 191, 177, 162}, // Teint atone
  {160, 116, 160, 202}, // Bleu SSII
  {232,  15,  48,  91}, // Bleu des yeux
  {255, 133,   3,  91}, // PlayBoy
  {256, 199,   8,   0}, // Giffle
  {272, 128, 128, 128}, // Remise en question
  {354, 255, 180, 160}, // Rose (pareil que l'aube)
  {372,   0,   0,   0}, // Fondu noir final
  {1024,  0,   0,   0}, // Borne supérieure
};

// Mots en string :
#pragma data_seg(".words")
static const char words[] =
#define WORD_(x) #x
#include "words.h"
#undef WORD_
  "????"
  ;
const unsigned int numberOfWords = sizeof(words) / (4 * sizeof(char));

static GLuint wordTextures[sizeof(words) / 4];

#define FIRST_GLYPH 1
#define GLYPHS 'z'

#define WORD_WIDTHS 1
#if WORD_WIDTHS
static float wordWidths[sizeof(words) / 4];
#endif

// Enum pour faire référence aux mots
enum wi
  {
#define WORD_(x) x##_,
#include "words.h"
#undef WORD_
    QMARK,
    EMPTY = 0xff
  };

#pragma code_seg(".mix")
static float mix(int start, int end, int x)
{
  return (float)(x - start) / (float)(end - start);
}

#pragma code_seg(".falloff")
INLINER float falloff(int start, int end, int x)
{
  float y = mix(start, end, x);
  y = y * y * (3.f - 2.f * y);
  y *= y;
  return 1.f - y; // Smoothstep au carré pour décentrer
}

#pragma code_seg(".beat_to_time")
INLINER int beatToTime(int beat)
{
  return (beat * 60000)/136;
}

// ============================================================================

#pragma code_seg(".reset_matrix")
INLINER void resetMatrix()
{
  glLoadMatrixf(projectionMatrix);
}

#pragma code_seg(".clear_screen")
INLINER void clearScreen(long itime)
{
  glClear(GL_COLOR_BUFFER_BIT);
//   glColor3ubv(bgColors + 3 * ((itime / 14000) % (sizeof(bgColors) / 3)));
  int i = 0;
  while (itime > beatToTime(bgColors[i].beat)) ++i;

  float w = mix(beatToTime(bgColors[i - 1].beat), beatToTime(bgColors[i].beat), itime);
  float r = (bgColors[i - 1].r * (1.f - w) + bgColors[i].r * w)/255.f;
  float g = (bgColors[i - 1].g * (1.f - w) + bgColors[i].g * w)/255.f;
  float b = (bgColors[i - 1].b * (1.f - w) + bgColors[i].b * w)/255.f;
  glColor3f(r, g, b);

  glUseProgram(0);
  glDisable(GL_TEXTURE_2D);
  glDrawArrays(GL_QUADS, 0, 4);
  glEnable(GL_TEXTURE_2D);
}

#pragma code_seg(".show_word")
NOINLINER void showWord(unsigned char word, float x, float y, float size, float alpha)
{
  if (word == EMPTY)
    return;
  resetMatrix();
  glTranslatef(x, y, 0);
  size *= 1.01f - 0.01f * alpha * alpha;
  glScalef(size, size, 1.f);

  glColor4f(1.f, 1.f, 1.f, p0d90 * alpha);
  glBindTexture(GL_TEXTURE_2D, wordTextures[word]);

  glDrawArrays(GL_QUADS, 0, 4);
}

struct wordPosition
{
  unsigned char duration;
  unsigned char dt;
  unsigned char size;
  char startx;
  char starty;
  char dp321;
};

#define RIGHT_RIGHT_RIGHT 0x00 // 00 00 00 00
#define LEFT_LEFT_LEFT    0x15 // 00 01 01 01
#define UP_UP_UP          0x2a // 00 10 10 10
#define DOWN_DOWN_DOWN    0x3f // 00 11 11 11

#define RIGHT_DOWN_LEFT   0x1c // 00 01 11 00
#define UP_RIGHT_DOWN     0x32 // 00 11 00 10
#define RIGHT_DOWN_RIGHT  0x0c // 00 00 11 00
#define RIGHT_UP_RIGHT    0x08 // 00 00 10 00

#pragma data_seg(".words_timestamps")
const char wordDeltaEncodedTimestamps[] =
  {
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 12, 4, 4, 4, 4, 4,
    -28, 7, 8, 8, 8, 8, 8, 8, 8, 4, 4, 8, 4, 4, 9, 4, 4, 4, 4, 4, 4,
    4, -28, 7, 8, 8, 8, 8, 8, 8, 8, 4, 4, 8, 4, 3, 1, 8, 17, 8, 4, 4,
    4, 4, 4, 4, 4, 4, -4, 8, 4, 4, 2, 4, 2, 4, 4, 10, 2, 4, 4, 6, 1
  };
const unsigned int numberOfDisplays = sizeof(wordDeltaEncodedTimestamps) / sizeof(char);

#pragma data_seg(".displayed_words")
const unsigned char displayedWords[] =
  {
    FOUR_, FOUR_, FOUR_, FOUR_,   FOUR_, FOUR_, FOUR_, FOUR_,
    FOUR_, FOUR_, FOUR_, FOUR_,   FOUR_, FOUR_, FOUR_, FOUR_,
    FOUR_, KILO_, BYTE_, DEMO_,   CTRL_, META_, TEST_, EMPTY,
    FOUR_, FOUR_, FOUR_, FOUR_,   DAYS_, DAYS_, DAYS_, DAYS_,
    DAYS_, DAYS_, DAYS_, DAYS_,   DAYS_, DAYS_, DAYS_, DAYS_,
    DAYS_, DAYS_, DAYS_, DAYS_,   DAYS_, DAYS_, DAYS_, DAYS_,
    DAYS_, DAYS_, DAYS_, DAYS_,   DAYS_, DAYS_, DAYS_, DAYS_,
    DAYS_, DAYS_, DAYS_, DAYS_,   WEEK_, WEEK_, WEEK_, WEEK_,
    WEEK_, WEEK_, WEEK_, WEEK_,   WEEK_, WEEK_, WEEK_, WEEK_,
    WEEK_, WEEK_, WEEK_, WEEK_,   WEEK_, WEEK_, WEEK_, WEEK_,
    WEEK_, WEEK_, WEEK_, WEEK_,   DAYS_, EMPTY, EMPTY, EMPTY,
    DAYS_, THEN_, TURN_, WEEK_,   EACH_, WEEK_, EMPTY, EMPTY,
    JUST_, LIKE_, LAST_, WEEK_,   EACH_, DAWN_, EMPTY, EMPTY,
    WAKE_, WASH_, SLAM_, JAVA_,   READ_, NEWS_, EMPTY, EMPTY,
    SAME_, WARS_, RAPE_, BOMB_,   TONE_, DEAF_, FLAT_, EMPTY,
    TONE_, DEAF_, LIFE_, EMPTY,   WILL_, THAT_, EVER_, STOP_,
    WILL_, THIS_, LIFE_, EMPTY,   TURN_, INTO_, LIFE_, EMPTY,
    TURN_, INTO_, REAL_, LIFE_,   WORK_, WORK_, WORK_, WORK_,
    WORK_, WORK_, WORK_, WORK_,   WORK_, WORK_, WORK_, WORK_,
    WORK_, WORK_, WORK_, WORK_,   WORK_, WORK_, WORK_, WORK_,
    WORK_, WORK_, WORK_, WORK_,   WORK_, WORK_, WORK_, WORK_,
    WORK_, WORK_, WORK_, WORK_,   WORK_, EMPTY, EMPTY, EMPTY,
    WORK_, TILL_, LATE_, HOUR_,   MEET_, BOSS_, EMPTY, EMPTY,
    ALSO_, MEET_, DEAD_, LINE_,   MEET_, TEAM_, EMPTY, EMPTY,
    HAVE_, BEER_, WITH_, THEM_,   LOOK_, BACK_, EMPTY, EMPTY,
    THAT_, BLUE_, EYED_, GIRL_,   DEEP_, BLUE_, EYES_, EMPTY,
    FINE_, SOFT_, FACE_, EMPTY,   BEST_, GIRL_, EVER_, EMPTY,
    MUST_, TALK_, EMPTY, EMPTY,   TELL_, WHAT_, EMPTY, EMPTY,
    YEAH_, BABY_, WANT_, FUCK_,   LIKE_, PORN_, STAR_, QMARK,
    SLAM_, EMPTY, EMPTY, EMPTY,   OKAY_, THAT_, WONT_, WORK_,
    MUST_, JUST_, TALK_, EMPTY,   TELL_, SOME_, NICE_, WORD_,
    JUST_, TELL_, WHAT_, FEEL_,   THAT_, GIRL_, EMPTY, EMPTY,
    SUCH_, CUTE_, WARM_, FACE_,   SUCH_, NAVY_, BLUE_, EYES_,
    SUCH_, LONG_, DARK_, HAIR_,   SUCH_, SOFT_, SKIN_, EMPTY,
    SUCH_, FINE_, ROSE_, LIPS_,   BEST_, GIRL_, EVER_, EMPTY,
    WISH_, KISS_, THEM_, EMPTY,   THAT_, GIRL_, EMPTY, EMPTY,
    LIFE_, FEEL_, MORE_, REAL_,   WITH_, THAT_, GIRL_, EMPTY,
    THAT_, GIRL_, JUST_, GETS_,   TONE_, BACK_, INTO_, LIFE_,
    JUST_, TELL_, TRUE_, FEEL_,   JUST_, THAT_, EASY_, EMPTY,
    MATT_, GETS_, NEXT_, LUCY_,   GAZE_, GAZE_, GAZE_, GAZE_,
    GAZE_, GAZE_, GAZE_, GAZE_,   HIYA_, EMPTY, EMPTY, EMPTY,
    FROM_, ASIA_, WITH_, LOVE_,   CTRL_, META_, TEST_, EMPTY
  };

#pragma data_seg(".words_positions")
const wordPosition wordPositions[] =
  {
    {  4, 1, 4,  -8,  -4, RIGHT_RIGHT_RIGHT},
    {  4, 1, 4,   8,   4, LEFT_LEFT_LEFT},
    {  4, 1, 4,   6,  -4, UP_UP_UP},
    {  4, 1, 4,  -6,   4, DOWN_DOWN_DOWN},
    {  4, 1, 5, -12,   0, RIGHT_RIGHT_RIGHT},
    {  5, 1, 6,  -8,   0, RIGHT_RIGHT_RIGHT},
    {  5, 1, 8,  -4,  -4, UP_RIGHT_DOWN},
    {  5, 2, 8, -16,   8, DOWN_DOWN_DOWN},
    {  5, 2, 8, -16,   8, DOWN_DOWN_DOWN},
    {  5, 2, 8, -16,   8, DOWN_DOWN_DOWN},
    {  5, 2, 8, -16,   8, DOWN_DOWN_DOWN},
    {  5, 2, 8, -16,   8, DOWN_DOWN_DOWN},
    {  5, 2, 8, -16,   8, DOWN_DOWN_DOWN},
    {  5, 2, 8, -16,   8, DOWN_DOWN_DOWN},
    {  5, 2, 8, -16,   8, DOWN_DOWN_DOWN},
    {  5, 2, 6,  20, -16, LEFT_LEFT_LEFT},
    {  5, 2, 6,  20, -16, LEFT_LEFT_LEFT},
    {  5, 2, 6,  20, -16, LEFT_LEFT_LEFT},
    {  5, 2, 6,  20, -16, LEFT_LEFT_LEFT},
    {  5, 2, 6,  20, -16, LEFT_LEFT_LEFT},
    {  5, 2, 6,  20, -16, LEFT_LEFT_LEFT},
    {  8, 1, 8, -16,   4, RIGHT_RIGHT_RIGHT},
    {  8, 1, 8, -16,   4, RIGHT_RIGHT_RIGHT},
    {  8, 1, 8, -16,   4, RIGHT_RIGHT_RIGHT},
    {  8, 1, 8, -16,   0, RIGHT_RIGHT_RIGHT},
    {  8, 1, 8, -16,   4, RIGHT_RIGHT_RIGHT},
    {  8, 1, 8, -16,   0, RIGHT_RIGHT_RIGHT},
    {  8, 1, 8, -16,   4, RIGHT_RIGHT_RIGHT},
    {  8, 1, 8, -16,   0, RIGHT_RIGHT_RIGHT},
    { 12, 1, 8,   0,  10, RIGHT_RIGHT_RIGHT},
    { 12, 1, 8,   0,   5, RIGHT_RIGHT_RIGHT},
    { 12, 1, 8, -12,   0, RIGHT_RIGHT_RIGHT},
    { 12, 1, 8,   0,  10, RIGHT_RIGHT_RIGHT},
    { 12, 1, 8,   0,   5, RIGHT_RIGHT_RIGHT},
    { 12, 1, 8, -12,   0, RIGHT_RIGHT_RIGHT},
    {  5, 2, 6,  20, -16, LEFT_LEFT_LEFT},
    {  5, 2, 6,  20, -16, LEFT_LEFT_LEFT},
    {  5, 2, 6,  20, -16, LEFT_LEFT_LEFT},
    {  5, 2, 6,  20, -16, LEFT_LEFT_LEFT},
    {  5, 2, 6,  20, -16, LEFT_LEFT_LEFT},
    {  5, 2, 6,  20, -16, LEFT_LEFT_LEFT},
    {  5, 2, 6,  20, -16, LEFT_LEFT_LEFT},
    {  5, 2, 6,  20, -16, LEFT_LEFT_LEFT},
    {  8, 1, 8, -16,   4, RIGHT_RIGHT_RIGHT},
    {  8, 1, 8, -16,   0, RIGHT_RIGHT_RIGHT},
    {  8, 1, 8, -16,   4, RIGHT_RIGHT_RIGHT},
    {  8, 1, 8, -16,   0, RIGHT_RIGHT_RIGHT},
    {  8, 1, 8, -16,   4, RIGHT_RIGHT_RIGHT},
    {  8, 1, 8, -16,   0, RIGHT_RIGHT_RIGHT},
    {  8, 1, 8, -16,   4, RIGHT_RIGHT_RIGHT},
    {  8, 1, 8, -16,   0, RIGHT_RIGHT_RIGHT},
    { 12, 1, 8,   0,  10, RIGHT_RIGHT_RIGHT},
    { 12, 1, 8,   0,   5, RIGHT_RIGHT_RIGHT},
    { 12, 1, 8,   0,   0, RIGHT_RIGHT_RIGHT},
    { 12, 1, 8,   4,  10, RIGHT_RIGHT_RIGHT},
    { 12, 1, 8,   4,   5, RIGHT_RIGHT_RIGHT},
    {  8, 1, 8, -16,   0, RIGHT_RIGHT_RIGHT},
    {  8, 1, 8, -16,  -5, RIGHT_RIGHT_RIGHT},
    {  5, 0,16,   4,  -4, RIGHT_RIGHT_RIGHT},
    {  5, 1, 8, -16,  -4, RIGHT_RIGHT_RIGHT},
    {  5, 1, 6,   0,  -4, RIGHT_RIGHT_RIGHT},
    {  5, 1, 6,   0,   0, RIGHT_RIGHT_RIGHT},
    {  5, 1, 8, -16,   0, RIGHT_RIGHT_RIGHT},
    {  5, 1, 6, -16,   4, RIGHT_RIGHT_RIGHT},
    {  5, 1, 6,  -4,  -4, RIGHT_RIGHT_RIGHT},
    {  5, 1, 6,  -4,  -4, RIGHT_RIGHT_RIGHT},
    {  5, 1, 6,  -4,  -4, RIGHT_RIGHT_RIGHT},
    {  5, 1, 6,  -4,  -4, RIGHT_RIGHT_RIGHT},
    { 10, 1, 6,  -4,  -4, RIGHT_RIGHT_RIGHT},
    {  5, 1, 6, -16,   4, RIGHT_RIGHT_RIGHT},
    {  5, 2, 8, -12,   0, RIGHT_RIGHT_RIGHT},
    {  5, 1, 6, -16,   4, RIGHT_RIGHT_RIGHT},
    {  5, 1, 6,   0,   5, RIGHT_RIGHT_RIGHT},
    {  5, 1, 6,   0,   0, RIGHT_RIGHT_RIGHT},
    {  5, 1, 6, -16,   4, RIGHT_RIGHT_RIGHT},
    {  5, 1, 6, -16,   0, RIGHT_RIGHT_RIGHT},
    { 12, 1, 8, -16,   0, RIGHT_RIGHT_RIGHT},
    { 12, 4, 8,  -8,   0, RIGHT_RIGHT_RIGHT},
    { 16, 1, 8, -16,   0, RIGHT_RIGHT_RIGHT},
    {  4, 1, 4,  -8,  -4, RIGHT_RIGHT_RIGHT},
    {  4, 1, 4,   8,  -8, LEFT_LEFT_LEFT},
    {  8, 1, 4,   0,   0, LEFT_LEFT_LEFT},
    {  6, 1, 5, -10,   4, RIGHT_RIGHT_RIGHT},
    {  8, 1, 6,  -8,   0, RIGHT_RIGHT_RIGHT},
  };

#pragma code_seg(".iter_word")
INLINER void iterWord(long itime, unsigned char word, float size,
		      int & start, int & end, float & x, float & y,
		      int dt, char & dp)
{
  if (itime >= start && itime <= end)
  {
    showWord(word, x, y, size, falloff(start, end, itime));
  }

  start += dt;
  end += dt;

#if WORD_WIDTHS
  // Right 00
  // Left  01
  // Up    10
  // Down  11
  int mvy = (dp>>1) & 1;
  int mv = 1 - ((dp & 1)<<1);
  x += !mvy * mv * size * wordWidths[word];
  y += mvy * mv * size * p0d70;
#else
  x += !((dp>>1) & 1) * (1 - ((dp & 1)<<1)) * size * 1.6f;
  y += ((dp>>1) & 1) * (1 - ((dp & 1)<<1)) * size * 0.8f;
#endif

  dp >>= 2;
}

#pragma code_seg(".show_words")
INLINER void showWords(long itime, float ftime)
{
  int beat = 0;
  int i = 0;
  do
  {
    beat += wordDeltaEncodedTimestamps[i];
    int start = beatToTime(beat);
    int end = start + 200 * wordPositions[i].duration;
    const int dt = 200 * wordPositions[i].dt;
    if (itime >= start && itime <= end + 3 * dt)
    {
      float x = wordPositions[i].startx / 32.f;
      float y = wordPositions[i].starty / 32.f;
      const float size = wordPositions[i].size / 32.f;
      char dp = wordPositions[i].dp321;

      int j = 0;
      do
      {
	iterWord(itime, displayedWords[4 * i + j++],
		 size, start, end, x, y, dt, dp);
      }
      while (j < 4);
    }
  }
  while (++i < numberOfDisplays);

  // Giffle
  if (itime > 112500 - 2000 && itime < 112500)
  {
    float step = mix(112500 - 2000, 112500, itime);
    step = 1 - step * step;
    showWord(FACE_, 0.125f, -0.125f, 0.5f, 1.f);
    showWord(HAND_, 0.125f + step, -0.125f + 0.5f * step * step, 0.5f, 1.f);
  }
}

// ============================================================================
// Shaders

// A bidouiller : résultat intéressant
// const static char * postpross = "void main(){ vec2 uv = gl_TexCoord[0].xy; float vignetting_radius = 0.3; float vignetting_intensity = 0.35; vec2 center = vec2(0.5, 0.5); float vi = distance(uv, center) / length(center); float vignetting = mix(1. - vignetting_intensity, 1., smoothstep(1., vignetting_radius, vi)); gl_FragColor = vec4(1., vignetting, 0., 1.);}";

#pragma data_seg(".shader_source")
const char * postpross =
 "void main()"
 "{"
   "vec2 r=gl_TexCoord[0].rg;"
   "float m=mix(.1,0.,smoothstep(1.,.2,distance(r,vec2(.5))/.707));"
   "gl_FragColor=vec4(.1-m,0.,m,m*4.+.1*fract(sin(gl_Color.r*.233+dot(r,vec2(12.9898,78.233)))*43758.5));"
 "}";

#pragma code_seg(".init_shader")
INLINER int initShader()
{
  // Ordre déterminé par des essais successifs avec Crinkler
  const GLuint s = ((PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader"))(GL_FRAGMENT_SHADER);
  ((PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource"))(s, 1, &postpross, NULL);

  glUseProgram = ((PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram"));
  ((PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader"))(s);

  pid = ((PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram"))();
  ((PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader"))(pid, s);
  ((PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram"))(pid);

// #ifdef DEBUG
//   int result;
//   char info[1536];
//   oglGetObjectParameteriv( fsId,   GL_OBJECT_COMPILE_STATUS_ARB, &result ); oglGetInfoLog( fsId,   1024, NULL, (char *)info ); if( !result ) DebugBreak();
//   oglGetObjectParameteriv( pid, GL_OBJECT_LINK_STATUS_ARB,    &result ); oglGetInfoLog( pid, 1024, NULL, (char*)info ); if( !result ) DebugBreak();
// #endif

  return pid;
}

#pragma code_seg(".post_processing")
INLINER void poorManPostProcessing(long itime)
{
  // Post processing de pauvre
  glUseProgram(pid);
  resetMatrix();
  glColor3f(sin((float)(itime/40)), 0.f, 0.f); // 25 Hz
  glDrawArrays(GL_QUADS, 0, 4);
}

// ============================================================================
// Textures

#pragma code_seg(".create_texture")
INLINER GLuint createTexture(unsigned int width)
{
  GLuint id;
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, width, width, 0, GL_ALPHA, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
//   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

  return id;
}

#pragma code_seg(".create_words")
INLINER void createWords()
{
  glClearColor(0, 0, 0, 0);
  glViewport(0, 0, TRES, TRES);
  glEnable(GL_TEXTURE_2D);

  const unsigned int white = 0xffffffff;
  glColor3ubv((const GLubyte*)&white);

  LOOP_(i, numberOfWords)
  {
    glClear(GL_COLOR_BUFFER_BIT);
    glBindTexture(GL_TEXTURE_2D, 0);
    glRasterPos2f(-1.f, 0);
    glCallLists(4, GL_BYTE, words + 4 * i);

#if WORD_WIDTHS
    glCallLists(1, GL_BYTE, " ");
    float xyzw[4];
    glGetFloatv(GL_CURRENT_RASTER_POSITION, xyzw);
    wordWidths[i] = 2.f * xyzw[0] / TRES;
#endif

    wordTextures[i] = createTexture(TRES);
    glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, TRES, TRES);
  }
}

// ============================================================================
// Font

#pragma code_seg(".create_font")
INLINER int createFont(const HDC & hdc)
{
  const unsigned int list = glGenLists(GLYPHS);
  if (!list)
    return 0;

  // Récupération des glyphes
  HFONT font = CreateFont(-TRES / 3, // FIXME : ajuster une fois la police choisie
			  0, 0, 0, FW_MEDIUM,
			  FALSE, FALSE, FALSE, ANSI_CHARSET,
			  OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
			  ANTIALIASED_QUALITY,//PROOF_QUALITY,
			  FF_DONTCARE | DEFAULT_PITCH,
			  "Impact");
  if (!font) return 0;
  SelectObject(hdc, font);
//   DeleteObject(font);
  if (!wglUseFontBitmaps(hdc, FIRST_GLYPH, GLYPHS, list)) return 0;

  createWords();

//   glDeleteLists(list, GLYPHS);

  return 1;
}

// ============================================================================

#pragma code_seg(".intro_init")
int intro_init(const HDC & hdc)
{
  if (!(//EXT_Init() &&
	initShader() &&
	createFont(hdc)))
    return 0;

#if (VPH != YRES)
  // Bandes noires
  glClearColor(0, 0, 0, 0);
  glViewport(0, 0, XRES, YRES);
  glClear(GL_COLOR_BUFFER_BIT);
#endif

  glViewport(0, VPY, XRES, VPH);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // FIXME: Vérifier que ce n'est pas la valeur par défaut

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glVertexPointer(2, GL_FLOAT, 0, faceVerts);
  glTexCoordPointer(2, GL_FLOAT, 0, faceCoords);

  return 1;
}

// ============================================================================
// Boucle de la démo

#pragma code_seg(".intro_do")
void intro_do( long itime )
{
  const float ftime = 0.001f * (float)itime;

  resetMatrix();
  clearScreen(itime);

  glUseProgram(0);

  showWords(itime, ftime);
  poorManPostProcessing(itime);
}

// ============================================================================
