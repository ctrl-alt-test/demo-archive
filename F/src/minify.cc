// Minification des shaders

#include "minify.hh"

#include "array.hh"
#include "sys/msys.h"

// Choisir si on utilise la minification rapide ou si on appelle Shader Minifier
#define FAST_MINIFICATION 1

#if FAST_MINIFICATION
static bool search(char c, const char* str)
{
  for (const char *ptr = str; *ptr; ptr++)
    if (*ptr == c) return true;
  return false;
}

Minifier::Minifier() : files(100)
{
  char *file = "src/shaders_gen.cc";
  fp = fopen(file, "w");
  if (fp == NULL)
    DBG("Cannot open '%s', shaders won't be minified", file);
}

void Minifier::addFile(char * data, const char * filename)
{
  if (fp == NULL) return;
  if (data == NULL)
  {
    fprintf(fp, "NULL,\n");
    return;
  }
  fprintf(fp, "// %s\n", filename);
  DBG("%s", filename);

  bool comment = false;  // simple //
  bool commentm = false; // multiligne /* */
  bool squeeze = true;  // squeeze les espaces
  bool macro = false;
  char * result = _strdup(data);
  char * ptr2 = result;
  for (char * ptr = data; *ptr; ptr++) {
    if (commentm)
    {
      if (*ptr == '/' && ptr[-1] == '*')
	commentm = false;
      continue;
    }
    if (macro)
    {
      if (*ptr == '\n')
      {
        *ptr2++ = '\\';
        *ptr2++ = 'n';
        macro = false;
        squeeze = true;
      }
      else
        *ptr2++ = *ptr;
      continue;
    }
    if (comment)
    {
      if (*ptr == '\n') comment = false;
      continue;
    }

    if (*ptr == '/' && ptr[1] == '/') {
      comment = true;
      continue;
    }
    if (*ptr == '/' && ptr[1] == '*') {
      commentm = true;
      continue;
    }
    if (*ptr == '#') {
      if (ptr2 > result + 1 && ptr2[-2] != '\\' && ptr2[-1] != 'n') { // # doit être en début de ligne
        *ptr2++ = '\\';
        *ptr2++ = 'n';
      }
      macro = true;
    }
    if (squeeze && search(*ptr, " \t\r\n")) continue;
    squeeze = search(*ptr, "+-*/=;(){}[],|&<> \t\r\n");

    if (squeeze && ptr2[-1] == ' ' && search(*ptr, "+-*/=;(){},|&<>"))
      ptr2--;

    if (*ptr == '\r' || *ptr == '\n') // pas de retour ligne dans une string
      *ptr2++ = ' ';
    else
      *ptr2++ = *ptr;
  }
  *ptr2 = '\0';
  fprintf(fp, "\"%s\",\n", result);
  free(result);
}

Minifier::~Minifier()
{
  if (fp != NULL)
    fclose(fp);
}

#else // utilisation de Shader Minifier

Minifier::Minifier() : files(100)
{
}

void Minifier::addFile(char * data, const char * filename)
{
  files.add(filename);
}

Minifier::~Minifier()
{
  char cmd[5000] = {0};

  // on génère la ligne de commande
  strcat(cmd, "..\\tools\\shader_minifier.exe ");
  strcat(cmd, "-o src/shaders_gen.cc --format c-array -v --preserve-all-globals ");
  for (int i = 0; i < files.size; i++)
  {
    strcat(cmd, " shaders/");
    strcat(cmd, files[i]);
  }
  strcat(cmd, " || pause");
  DBG("cmd = %s", cmd);

  // on écrit le .bat
  FILE* out = fopen("minify.bat", "w");
  fprintf(out, "%s", cmd);
  fclose(out);

  // on exécute la commande
  system(cmd);
}

#endif
