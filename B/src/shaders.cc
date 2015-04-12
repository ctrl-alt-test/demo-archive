//
// Tous les shaders utilisés
//

#include "shaders.hh"

#include "sys/msys.h"
#include <GL/gl.h>
#include "sys/glext.h"
#include "interpolation.hh"

namespace Shader
{
  Program * list = NULL;

  static const char* uniformNames[numberOfUniforms] =
    {
      "id",    // id de l'objet
      "cMap",  // color map
      "c2Map", // color map
      "nMap",  // normal map
      "sMap",  // specular map
      "dof",   // profondeur de champ
      "fade",  // coefficient de fondu
      "fadeLuminance",  // couleur du fondu
      "trans", // coefficient pour la transition par texture
      "texFade", // coefficient de fondu entre deux textures
      "glowFlash", // glow min
      "screenCX", // screen center
      "screenCY",
      "time",  // intro.now
    };

  static GLint* listUniforms = NULL;

  // Liste des shaders généré en DEBUG, pour la RELEASE
#ifndef DEBUG
  static const char *shader_src[] = {
# include "shaders_gen.cc"
    NULL
  };
#endif

  static void init_uniforms()
  {
    if (NULL == listUniforms)
    {
      listUniforms = new GLint[numberOfUniforms * numberOfShaders];
    }
    int count = 0;
    for (int j = 0; j < numberOfUniforms; j++)
      for (int i = 0; i < numberOfShaders; i++)
      {
 	if (list[i].id() != 0)
	{
	  DBG("%d (shader id %d), \"%s\"", i, list[i].id(), uniformNames[j]);
	  listUniforms[count] = oglGetUniformLocation(list[i].id(), uniformNames[j]);
	  OGL_ERROR_CHECK("oglGetUniformLocation");
	}
	++count;
      }

    // Envoi des valeurs d'initialisation
    unicast1f(postProcess, dof, 0.6f);
    unicast1f(postProcess, fade, 1.f);
    unicast1f(postProcess, fadeLuminance, 1.f);
    unicast1f(glow, fade, 1.f);
    unicast1f(glow, fadeLuminance, 1.f);
    unicast1f(text, trans, 0.f);

    broadcast1i(Shader::colorMap, 0);
    broadcast1i(Shader::sndColorMap, 1);
    broadcast1i(Shader::normalMap, 2);
    broadcast1i(Shader::specularMap, 3);
  }

  void broadcast3f(uniforms u, float x, float y, float z)
  {
    const GLint* li = &listUniforms[u * numberOfShaders];
    for (int i = 0; i < numberOfShaders; i++)
      if (li[i] >= 0)
      {
        list[i].use();
        oglUniform3f(li[i], x, y, z);
      }
  }

  // FIXME : factoriser ?
  void broadcast1i(uniforms u, int x)
  {
    assert(listUniforms != NULL);
    const GLint* li = &listUniforms[u * numberOfShaders];
    for (int i = 0; i < numberOfShaders; i++)
      if (li[i] >= 0)
      {
        list[i].use();
        oglUniform1i(li[i], x);
      }
  }

  void unicast1i(id shaderId, uniforms u, int x)
  {
    assert(listUniforms != NULL);
    const GLint* li = &listUniforms[u * numberOfShaders];
    list[shaderId].use();
    oglUniform1i(li[shaderId], x);
  }

  void broadcast1f(uniforms u, float x)
  {
    assert(listUniforms != NULL);
    const GLint* li = &listUniforms[u * numberOfShaders];
    for (int i = 0; i < numberOfShaders; i++)
      if (li[i] >= 0)
      {
        list[i].use();
        oglUniform1f(li[i], x);
      }
  }

  void unicast1f(id shaderId, uniforms u, float x)
  {
    assert(listUniforms != NULL);
    const GLint* li = &listUniforms[u * numberOfShaders];
    list[shaderId].use();
    oglUniform1f(li[shaderId], x);
  }

  float calcFade(date begin, date end, date d, bool fadein)
  {
    assert(begin < end);
    if (d < begin || d > end) return 1.f;
    if (fadein)
      return smoothStepI(begin, end, d);
    else
      return 1.f - smoothStepI(begin, end, d);
  }

#ifdef DEBUG

  typedef struct
  {
    id id;
    const char * vertFile;
    const char * pixFile;
  } shaderInfo;

#endif // DEBUG

  void loadFirstShader()
  {
    list = new Program[numberOfShaders];

#ifdef DEBUG

    // Chargement et écriture du premier shader dans shaders_gen.cc
    FILE * out = fopen("src/shaders_gen.cc", "w");
    LoadProgram(list[0], "color.vert", "color.frag", out);
    fclose(out);

#else

    list[Shader::color].LoadShaders(shader_src[0], shader_src[1]);

#endif // DEBUG
  }

  void loadShaders(Loading::ProgressDelegate * pd, int low, int high)
  {
    assert(list != NULL);

    const int ldStep = (high - low) / numberOfShaders;
    int currentLd = low;

    DBG("loadShaders...");

    // Attention : on ajoute les shaders en Debug dans le tableau ci-dessous.
    // Après exécution, le fichier utilisé en Release est mis à jour automatiquement.
#ifdef DEBUG
    FILE * out = fopen("src/shaders_gen.cc", "a"); // Append à cause de loadFirstShader

    shaderInfo shader_files[] =
      {
	{default,	"shader.vert",		"shader.frag"},
	{sphere,	"sphere.vert",		"sphere.frag"},
	{bump,		"bump.vert",		"bump.frag"},
	{grass,		"grass.vert",		"grass.frag"},
	{pixel,		"pixel.vert",		"pixel.frag"},
	{texture,	"texture.vert",		"texture.frag"},
	{glow,		"glow.vert",		"glow.frag"},
        {godRays,	"godrays.vert",		"godrays.frag"},
	{postProcess,	"dof.vert",		"dof.frag"},
	{ramp,		"ramp.vert",		"ramp.frag"},
	{conway,	"conway.vert",		"ramp.frag"},
	{tree,		"tree.vert",		"tree.frag"},
	{rope,		"rope.vert",		"rope.frag"},
        {pharmacy,      "pharmacy.vert",        "pharmacy.frag"},
        {sodium,        "sodium.vert",          "sodium.frag"},
        {building,      "building.vert",        "building.frag"},
	{text,  	"text.vert",		"text.frag"},
      };

    // Tri de la liste pour être dans l'ordre de l'enum
    const char* sorted_shader_files[2 * numberOfShaders];
    for (int i = 0; i < 2 * numberOfShaders; i++)
      sorted_shader_files[i] = NULL;
    const unsigned int shadersToLoad = sizeof(shader_files) / sizeof(shaderInfo);
    for (int i = 0; i < shadersToLoad; i++)
      {
        sorted_shader_files[2*shader_files[i].id] = shader_files[i].vertFile;
        sorted_shader_files[2*shader_files[i].id+1] = shader_files[i].pixFile;
      }

    // Chargement et écriture dans shaders_gen.cc
    for (int i = 1; i < numberOfShaders; ++i)
    {
      LoadProgram(list[i], sorted_shader_files[2*i], sorted_shader_files[2*i+1], out);
      if (pd)
      {
	pd->func( pd->obj, currentLd ); currentLd += ldStep; // TEST
      }
    }
    fclose(out);
#else
    // Charge les shaders compresses dans un tableau.
    for (int i = 1; i < numberOfShaders; i++)
    {
      list[i].LoadShaders(shader_src[i*2], shader_src[i*2+1]);
      pd->func( pd->obj, currentLd ); currentLd += ldStep; // TEST
    }
#endif // DEBUG

    init_uniforms();

#ifdef DEBUG

    if (pd)
    {
      pd->func( pd->obj, high ); // TEST
    }

#else

    pd->func( pd->obj, high ); // TEST

#endif // DEBUG
  }

#ifdef DEBUG

  char* LoadSource(const char *filename)
  {
    char name[100];
    strcpy(name, "shaders/");
    strcat(name, filename);
    FILE * fp = fopen(name, "r");
    if (fp == NULL)
      return NULL;

    fseek(fp, 0, SEEK_END);
    int size = ftell(fp);
    rewind(fp);

    char * src = (char*)msys_mallocAlloc(size + 1);

    for(int i = 0; i < size; i++)
    {
      const int c = fgetc(fp);
      if (c < 0) {
        src[i] = '\0';
	break;
      }
      src[i] = (char) c;
    }
    fclose(fp);
    return src;
  }

  bool search(char c, char* str)
  {
    for (char *ptr = str; *ptr; ptr++)
      if (*ptr == c) return true;
    return false;
  }

  // Compresse et ecrit le shader dans un fichier .cc
  void generate_cc(char *data, const char * name, FILE * out)
  {
    if (out == 0) return;
    if (data == 0) {
      fprintf(out, "NULL,\n");
      return;
    }
    fprintf(out, "// %s\n", name);

    bool comment = false;  // simple //
    bool commentm = false; // multiligne /* */
    bool squeeze = true;  // squeeze les espaces
    bool macro = false;
    char *ptr2 = data;
    for (char *ptr = data; *ptr; ptr++) {
      if (*ptr == '/' && ptr[1] == '/')
	comment = true;
      if (*ptr == '/' && ptr[1] == '*')
	commentm = true;
      if (*ptr == '#')
	macro = true;

      if (search(*ptr, "\r\n")) comment = false;
      if (!macro && squeeze && search(*ptr, " \t\r\n")) continue;
      if (search(*ptr, "\r\n")) macro = false;

      squeeze = commentm || comment || search(*ptr, "+-*/=;(){}[],|&<> \t\r\n");

      if (squeeze && ptr2[-1] == ' ' && search(*ptr, "+-*/=;(){},|&<>"))
	ptr2--;

      if (!comment && !commentm) {
	if (*ptr == '\r' || *ptr == '\n') { // echappe le saut de ligne
	  *ptr2++ = '\\';
	  *ptr2++ = 'n';
	}
	else *ptr2++ = *ptr;
      }

      if (commentm && *ptr == '/' && ptr[-1] == '*')
	commentm = false;
    }
    *ptr2 = '\0';
    fprintf(out, "\"%s\",\n", data);
  }

  void LoadProgram(Program & program,
		   const char * vsname, const char * fsname, FILE * out)
  {
    DBG("LoadProgram : %s / %s", (vsname ? vsname : "NULL"), (fsname ? fsname : "NULL"));

    char * vertexSource = (vsname != NULL) ? LoadSource(vsname) : NULL;
    char * fragmentSource = (fsname != NULL) ? LoadSource(fsname) : NULL;

    generate_cc(vertexSource, vsname, out);
    generate_cc(fragmentSource, fsname, out);

    if(NULL == vsname && NULL == fsname) return;
    program.LoadShaders(vertexSource, fragmentSource);

    if (vertexSource != NULL)
      msys_mallocFree(vertexSource);
    if (fragmentSource != NULL)
      msys_mallocFree(fragmentSource);
  }

#endif // DEBUG

}
