// Attention, ce fichier est utilisé à la fois par la démo et par le texgen en ligne.

#include "texture.hh"
#include "../textures.hh"
extern "C" {
#include "../../tools/picoc/picoc.h"
}
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

using namespace Texture;

// Interop avec les textures
extern unsigned int texture_size;
extern Channel displayed_texture[3];

void register_texture(const char* s, Channel *t);

#ifdef LINUX
void PlatformLibraryInit()
{
    struct ParseState Parser;
    char *Identifier;
    struct ValueType *ParsedType;
    void *Tokens;
    const char *IntrinsicName = TableStrRegister("platform library");
    // Fake definition - l'utilisateur doit passer par les fonctions
    const char *StructDefinition = "struct Texture{float foo;};";

    /* define an example structure */
    Tokens = LexAnalyse(IntrinsicName, StructDefinition, strlen(StructDefinition), NULL);
    LexInitParser(&Parser, StructDefinition, Tokens, IntrinsicName, TRUE);
    TypeParse(&Parser, &ParsedType, &Identifier);
    //HeapFree(Tokens);
}
#endif


namespace PicocTex
{

void Ctest (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
    printf("test(%d)\n", Param[0]->Val->Integer);
    Param[0]->Val->Integer = 1234;
}

void Clineno (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
    ReturnValue->Val->Integer = Parser->Line;
}

void Cerrormsg (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
    PlatformErrorPrefix(Parser);
    LibPrintf(Parser, ReturnValue, Param, NumArgs);
}


void newTexture (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  Channel* tex = new Channel(texture_size, texture_size);
  register_texture((char*) Param[0]->Val->NativePointer, tex);

  ReturnValue->Val->NativePointer = tex;
  //struct myComplex *ComplexVal = Param[0]->Val->NativePointer;  /* casts the pointer */
  //int i = ComplexVal->i;
  //int j = ComplexVal->j;
  //i++;
}

void show(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  displayed_texture[0] = *((Channel*) Param[0]->Val->NativePointer);
  displayed_texture[1] = *((Channel*) Param[1]->Val->NativePointer);
  displayed_texture[2] = *((Channel*) Param[2]->Val->NativePointer);
}

void flat (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  Channel* tex = (Channel*) Param[0]->Val->NativePointer;
  tex->Flat(Param[1]->Val->FP);
  //struct myComplex* res = malloc(sizeof(struct myComplex));
  //res->i = Param[0]->Val->Integer;
  //res->j = Param[1]->Val->Integer;
  //ReturnValue->Val->NativePointer = "hello"; //(void*) 42;
}

void gradient (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  Channel* tex = (Channel*) Param[0]->Val->NativePointer;
  tex->Gradient(Param[1]->Val->FP, Param[2]->Val->FP, Param[3]->Val->FP, Param[4]->Val->FP);
}

void addCuts (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  Channel* tex = (Channel*) Param[0]->Val->NativePointer;
  tex->AddCuts(Param[1]->Val->FP, Param[2]->Val->FP);
}

void perlin (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  Channel* tex = (Channel*) Param[0]->Val->NativePointer;
  tex->Perlin(Param[1]->Val->Integer, Param[2]->Val->Integer, Param[3]->Val->Integer, Param[4]->Val->FP);
}

void max_ (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  Channel* tex = (Channel*) Param[0]->Val->NativePointer;
  Channel* tex2 = (Channel*) Param[1]->Val->NativePointer;
  tex->MaximizeTo(*tex2);
}

void min_ (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  Channel* tex = (Channel*) Param[0]->Val->NativePointer;
  Channel* tex2 = (Channel*) Param[1]->Val->NativePointer;
  tex->MinimizeTo(*tex2);
}

void mix (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  Channel* tex = (Channel*) Param[0]->Val->NativePointer;
  Channel* tex2 = (Channel*) Param[2]->Val->NativePointer;
  tex->Mix(Param[1]->Val->FP, *tex2);
}

void mask (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  Channel* tex = (Channel*) Param[0]->Val->NativePointer;
  Channel* mask = (Channel*) Param[1]->Val->NativePointer;
  Channel* tex2 = (Channel*) Param[2]->Val->NativePointer;
  tex->Mask(*mask, *tex2);
}

void cells (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  Channel* tex = (Channel*) Param[0]->Val->NativePointer;
  tex->Cells(Param[1]->Val->Integer);
}

void cells2 (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  Channel* tex = (Channel*) Param[0]->Val->NativePointer;
  tex->Cells2(Param[1]->Val->Integer);
}

void conic (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  Channel* tex = (Channel*) Param[0]->Val->NativePointer;
  tex->Conic();
}

void radial (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  Channel* tex = (Channel*) Param[0]->Val->NativePointer;
  tex->Radial();
}

void square (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  Channel* tex = (Channel*) Param[0]->Val->NativePointer;
  tex->Square();
}

void diamond (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  Channel* tex = (Channel*) Param[0]->Val->NativePointer;
  tex->Diamond();
}

void bump (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  Channel* tex = (Channel*) Param[0]->Val->NativePointer;
  tex->Bump();
}

void sharpen (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  Channel* tex = (Channel*) Param[0]->Val->NativePointer;
  tex->Sharpen();
}

void emboss (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  Channel* tex = (Channel*) Param[0]->Val->NativePointer;
  tex->Emboss();
}

void edgeDetect (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  Channel* tex = (Channel*) Param[0]->Val->NativePointer;
  tex->EdgeDetect();
}

void fade (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  Channel* tex = (Channel*) Param[0]->Val->NativePointer;
  tex->Fade(Param[1]->Val->FP);
}

void sinus (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  Channel* tex = (Channel*) Param[0]->Val->NativePointer;
  tex->Sinus(Param[1]->Val->Integer, Param[2]->Val->Integer);
}

void cosinus (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  Channel* tex = (Channel*) Param[0]->Val->NativePointer;
  tex->Cosinus(Param[1]->Val->Integer, Param[2]->Val->Integer);
}

void random (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  Channel* tex = (Channel*) Param[0]->Val->NativePointer;
  tex->Random();
}

void cut (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  Channel* tex = (Channel*) Param[0]->Val->NativePointer;
  tex->Cut(Param[1]->Val->Integer);
}

void abs_ (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  Channel* tex = (Channel*) Param[0]->Val->NativePointer;
  tex->Abs();
}

void pow_ (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  Channel* tex = (Channel*) Param[0]->Val->NativePointer;
  tex->Pow(Param[1]->Val->FP);
}

void mod (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  Channel* tex = (Channel*) Param[0]->Val->NativePointer;
  tex->Mod();
}

void sin_ (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  Channel* tex = (Channel*) Param[0]->Val->NativePointer;
  tex->Sin();
}

void scale (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  Channel* tex = (Channel*) Param[0]->Val->NativePointer;
  tex->Scale(Param[1]->Val->FP, Param[2]->Val->FP);
}

void clamp (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  Channel* tex = (Channel*) Param[0]->Val->NativePointer;
  tex->Clamp(Param[1]->Val->FP, Param[2]->Val->FP);
}

void clampAndScale (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  Channel* tex = (Channel*) Param[0]->Val->NativePointer;
  tex->ClampAndScale(Param[1]->Val->FP, Param[2]->Val->FP);
}

void blur (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  Channel* tex = (Channel*) Param[0]->Val->NativePointer;
  tex->Blur();
}

void gaussianBlur (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  Channel* tex = (Channel*) Param[0]->Val->NativePointer;
  tex->GaussianBlur();
}

void horizontalMotionBlur (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  Channel* tex = (Channel*) Param[0]->Val->NativePointer;
  tex->HorizontalMotionBlur(Param[1]->Val->FP);
}

void verticalMotionBlur (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  Channel* tex = (Channel*) Param[0]->Val->NativePointer;
  tex->VerticalMotionBlur(Param[1]->Val->FP);
}

void translate (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  Channel* tex = (Channel*) Param[0]->Val->NativePointer;
  tex->Translate(Param[1]->Val->FP, Param[2]->Val->FP);
}

void verticalFlip (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  Channel* tex = (Channel*) Param[0]->Val->NativePointer;
  tex->VerticalFlip();
}

void horizontalFlip (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  Channel* tex = (Channel*) Param[0]->Val->NativePointer;
  tex->HorizontalFlip();
}

void transpose (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  Channel* tex = (Channel*) Param[0]->Val->NativePointer;
  tex->Transpose();
}

void rotate90 (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  Channel* tex = (Channel*) Param[0]->Val->NativePointer;
  tex->Rotate90();
}

void verticalWave (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  Channel* tex = (Channel*) Param[0]->Val->NativePointer;
  tex->VerticalWave(Param[1]->Val->FP, Param[2]->Val->FP);
}

void horizontalWave (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  Channel* tex = (Channel*) Param[0]->Val->NativePointer;
  tex->HorizontalWave(Param[1]->Val->FP, Param[2]->Val->FP);
}

void waves (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  Channel* tex = (Channel*) Param[0]->Val->NativePointer;
  Channel* tex2 = (Channel*) Param[3]->Val->NativePointer;
  tex->Waves(Param[1]->Val->FP, Param[2]->Val->FP, *tex2);
}

void averageScale (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  Channel* tex = (Channel*) Param[0]->Val->NativePointer;
  tex->AverageScale(Param[1]->Val->Integer, Param[2]->Val->FP);
}

void zoomout (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  Channel* tex = (Channel*) Param[0]->Val->NativePointer;
  tex->ZoomOut(Param[1]->Val->FP, Param[2]->Val->FP);
}

void add (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  Channel* tex = (Channel*) Param[0]->Val->NativePointer;
  Channel* tex2 = (Channel*) Param[1]->Val->NativePointer;
  *tex += *tex2;
}

void sub (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  Channel* tex = (Channel*) Param[0]->Val->NativePointer;
  Channel* tex2 = (Channel*) Param[1]->Val->NativePointer;
  *tex -= *tex2;
}

// *= entre textures
void multT (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  Channel* tex = (Channel*) Param[0]->Val->NativePointer;
  Channel* tex2 = (Channel*) Param[1]->Val->NativePointer;
  *tex *= *tex2;
}

// *= avec un nombre
void mult (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  Channel* tex = (Channel*) Param[0]->Val->NativePointer;
  *tex *= Param[1]->Val->FP;
}

void copy (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  Channel* tex = (Channel*) Param[0]->Val->NativePointer;
  Channel* tex2 = (Channel*) Param[1]->Val->NativePointer;
  *tex = *tex2;
}

void set (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  Channel* tex = (Channel*) Param[0]->Val->NativePointer;
  float *res = &tex->Pixel(Param[1]->Val->Integer, Param[2]->Val->Integer);
  *res = (float)Param[3]->Val->FP;
}

void get (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  Channel* tex = (Channel*) Param[0]->Val->NativePointer;
  float *res = &tex->Pixel(Param[1]->Val->Integer, Param[2]->Val->Integer);
  ReturnValue->Val->FP = *res;
}

void floodFill (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  Channel* tex = (Channel*) Param[0]->Val->NativePointer;
  tex->FloodFill(Param[1]->Val->Integer, Param[2]->Val->Integer, Param[3]->Val->FP);
}

void line (struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  Channel* tex = (Channel*) Param[0]->Val->NativePointer;
  tex->Line(Param[1]->Val->FP, Param[2]->Val->FP, Param[3]->Val->FP, Param[4]->Val->FP);
}

void Crand(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  ReturnValue->Val->Integer = (int)rand() % (unsigned int)(Param[0]->Val->Integer);
}

void Csrand(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
  srand(Param[0]->Val->Integer);
}

void setSize(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) {
   texture_size = Param[0]->Val->Integer;
}

// Fonctions d'export

// version simple
void export_(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
  Channel * tex = new Channel(texture_size, texture_size);
  *tex = * (Channel*) Param[0]->Val->NativePointer;
  register_texture((char*) Param[1]->Val->NativePointer, tex);
}

void queueTextureRGB_(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
#ifdef WINDOWS
  int id = Param[0]->Val->Integer;
  Channel* r = (Channel*) Param[1]->Val->NativePointer;
  Channel* g = (Channel*) Param[2]->Val->NativePointer;
  Channel* b = (Channel*) Param[3]->Val->NativePointer;
  int min_ = Param[4]->Val->Integer;
  int max_ = Param[5]->Val->Integer;
  bool anisotropy = (Param[6]->Val->Integer != 0);
  int tiling = Param[7]->Val->Integer;
  bool compressed = (Param[8]->Val->Integer != 0);

  Texture::queueTextureRGB((Texture::id) id, *r, *g, *b, min_, max_,
         	anisotropy, tiling, compressed DBGARG("noname"));
#endif
}

void queueTextureRGBA_(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
#ifdef WINDOWS
  int id = Param[0]->Val->Integer;
  Channel* r = (Channel*) Param[1]->Val->NativePointer;
  Channel* g = (Channel*) Param[2]->Val->NativePointer;
  Channel* b = (Channel*) Param[3]->Val->NativePointer;
  Channel* a = (Channel*) Param[4]->Val->NativePointer;
  int min_ = Param[5]->Val->Integer;
  int max_ = Param[6]->Val->Integer;
  bool anisotropy = (Param[7]->Val->Integer != 0);
  int tiling = Param[8]->Val->Integer;
  bool compressed = (Param[9]->Val->Integer != 0);

  Texture::queueTextureRGBA((Texture::id) id, *r, *g, *b, *a, min_, max_,
         	anisotropy, tiling, compressed DBGARG("noname"));
#endif
}

void queueTextureA_(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
#ifdef WINDOWS
  int id = Param[0]->Val->Integer;
  Channel* a = (Channel*) Param[1]->Val->NativePointer;
  int min_ = Param[2]->Val->Integer;
  int max_ = Param[3]->Val->Integer;
  bool anisotropy = (Param[4]->Val->Integer != 0);
  int tiling = Param[5]->Val->Integer;
  bool compressed = (Param[6]->Val->Integer != 0);

  Texture::queueTextureA((Texture::id) id, *a, min_, max_,
         	anisotropy, tiling, compressed DBGARG("noname"));
#endif
}

void buildAndQueueBumpMapFromHeightMap_(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs)
{
#ifdef WINDOWS
  int id = Param[0]->Val->Integer;
  Channel* t = (Channel*) Param[1]->Val->NativePointer;
  bool repeat = (Param[2]->Val->Integer != 0);
  buildAndQueueBumpMapFromHeightMap((Texture::id)id, *t, repeat);
#endif
}

/* list of all library functions and their prototypes */
struct LibraryFunction funcList[] =
{
    { newTexture,   "struct Texture* New(char *);" },
    { show,         "void Show(struct Texture*, struct Texture*, struct Texture*);" },
    { export_,      "void Export(struct Texture*, char *);" },

    // Générateurs (ignorent l'ancienne valeur de la texture)
    { flat,         "void Flat(struct Texture*, float);" },
    { random,       "void Random(struct Texture*);" },
    { perlin,       "void Perlin(struct Texture*, int, int, int, float);" }, // Lent
    { cells,        "void Cells(struct Texture*, int);" },
    { cells2,       "void Cells2(struct Texture*, int);" },
    { conic,        "void Conic(struct Texture*);" },
    { radial,       "void Radial(struct Texture*);" }, // Ne tile pas
    { square,       "void Square(struct Texture*);" },
    { diamond,      "void Diamond(struct Texture*);" },
    { sinus,        "void Sinus(struct Texture*, int, int);" },
    { cosinus,      "void Cosinus(struct Texture*, int, int);" },
    { gradient,     "void Gradient(struct Texture*, float, float, float, float);" }, // Ne tile pas
    { fade,         "void Fade(struct Texture*, float);" }, // Ne tile pas

    // Opérateurs
    { max_,         "void Max(struct Texture*, struct Texture*);" },
    { min_,         "void Min(struct Texture*, struct Texture*);" },
    { mix,          "void Mix(struct Texture*, float, struct Texture*);" },
    { mask,         "void Mask(struct Texture*, struct Texture*, struct Texture*);" },
    { add,          "void Add(struct Texture*, struct Texture*);" },
    { sub,          "void Sub(struct Texture*, struct Texture*);" },
    { multT,        "void MultT(struct Texture*, struct Texture*);" },
    { mult,         "void Mult(struct Texture*, float);" },

    // Filtres
    { bump,         "void Bump(struct Texture*);" },
    { sharpen,      "void Sharpen(struct Texture*);" },
    { emboss,       "void Emboss(struct Texture*);" },
    { edgeDetect,   "void EdgeDetect(struct Texture*);" },
    { blur,         "void Blur(struct Texture*);" },
    { gaussianBlur, "void GaussianBlur(struct Texture*);" },
    { horizontalMotionBlur, "void HorizontalMotionBlur(struct Texture*, float);" },
    { verticalMotionBlur, "void VerticalMotionBlur(struct Texture*, float);" },

    // Opérations pixel par pixel
    { cut,          "void Cut(struct Texture*, int);" },
    { abs_,         "void Abs(struct Texture*);" },
    { pow_,         "void Pow(struct Texture*, float);" },
    { mod,          "void Mod(struct Texture*);" },
    { sin_,         "void Sin(struct Texture*);" },
    { scale,        "void Scale(struct Texture*, float, float);" },
    { clamp,        "void Clamp(struct Texture*, float, float);" },
    { clampAndScale,"void ClampAndScale(struct Texture*, float, float);" },

    // Autres effets
    { translate,    "void Translate(struct Texture*, float, float);" },
    { horizontalFlip,"void HorizontalFlip(struct Texture*);" },
    { verticalFlip, "void VerticalFlip(struct Texture*);" },
    { transpose,    "void Transpose(struct Texture*);" },
    { rotate90,     "void Rotate90(struct Texture*);" },
    { horizontalWave,"void HorizontalWave(struct Texture*, float, float);" },
    { verticalWave, "void VerticalWave(struct Texture*, float, float);" },
    { averageScale, "void AverageScale(struct Texture*, int, float);" },
    { waves,        "void Waves(struct Texture*, float ang, float amp, struct Texture*);" },
    { zoomout,      "void ZoomOut(struct Texture*, float zx, float zy);" },

    { addCuts,      "void AddCuts(struct Texture*, float, float);" },
    { floodFill,    "void FloodFill(struct Texture*, int, int, float);" },
    { line,         "void Line(struct Texture*, float, float, float, float);" },

    { copy,         "void Copy(struct Texture*, struct Texture*);" },
    { set,          "void Set(struct Texture*, int, int, float);" },
    { get,          "float Get(struct Texture*, int, int);" },

    { Crand,        "int rand(int);" },
    { Csrand,       "void srand(int);" },
    { setSize,      "void setSize(int);" }, // modifie la taille des prochaines textures

    // Fonctions d'interface
    { queueTextureRGB_, "void queueTextureRGB(int texId, struct Texture* r, struct Texture* g, struct Texture* b,"
                                      "int min, int max, int anisotropy, int tiling, int compressed);" },
    { queueTextureRGBA_, "void queueTextureRGBA(int texId, struct Texture* r, struct Texture* g, struct Texture* b,"
                                      "struct Texture* a, int min, int max, int anisotropy, int tiling, int compressed);" },
    { queueTextureA_, "void queueTextureA(int texId, struct Texture* a,"
                                      "int min, int max, int anisotropy, int tiling, int compressed);" },
    { buildAndQueueBumpMapFromHeightMap_, "void buildAndQueueBumpMapFromHeightMap(int texId,"
                                      "struct Texture* r, int repeat);" },


    { NULL,         NULL }
};

}
