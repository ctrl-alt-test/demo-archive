
#include "project_loader.hh"

#include "array.hxx"
#include "intro.hh"
#include "variable.hh"

#include "sys/msys_debug.h"
#include "sys/msys_malloc.h"
#include <stdio.h>

#if DEBUG
# define NETWORK 1
#else
# define NETWORK 0 // pas de loader en release !
#endif

namespace ProjectLoader {
#if NETWORK

  char* skipSpaces(char* input)
  {
    char* old;
    do {
      old = input;

      int len = 0;
      sscanf(input, "%*[ \r\n\t]%n", &len);
      input += len;

      len = 0;
      sscanf(input, "//%*[^\r\n]%n", &len);
      input += len;

    } while (input > old);
    return input;
  }

  char* readArray(char* input, Array<float>& arr)
  {
    int len;
    for (int i = 0; i < arr.max_size; i++)
      {
        input = skipSpaces(input);
	float f;
	int ret = sscanf(input, "%f%n", &f, &len);
	assert(ret == 1);
	arr.add(f);
	input += len;
        // optional comma
        len = 0;
        sscanf(input, ",%n", &len);
	input += len;
      }
    input = skipSpaces(input);
    len = 0;
    sscanf(input, "; %n", &len);
    assert(len > 0);
    input += len;
    return input;
  }

  void readInput(char* input)
  {
    Variables & variables = intro.variables;
    while (*input)
    {
      char name[81];
      int dim, size, len;
      input = skipSpaces(input);
      int ret = sscanf(input, "%80[^ \n] %n", name, &len);
      if (ret < 1) break;
      input += len;
      if (strcmp(name, "command") == 0)
      {
	int command;
        char args[200] = {0};
	ret = sscanf(input, "%d %s%n", &command, args, &len);
	if (ret < 1) break;
	executeCommand(command, args);
      }
      else
      {
        int interp;
        ret = sscanf(input, "%d %d %d%n", &size, &dim, &interp, &len);
        if (ret < 2) break;
        input += len;

        Array<float> data(size * dim);
        input = readArray(input, data);

        Variable *v = variables.tryFind(name);
	const bool isSpline = interp != 5;// 5=temporal vector
        if (v == NULL)
        {
          DBG("New variable '%s', dim = %d, size = %d", name, dim, size);
          v = new Variable(data.elt, dim, size, isSpline);
          variables.define(name, v);
        }
        else
        {
          DBG("Updating '%s', dim = %d, size = %d", name, dim, size);
          v->update(data.elt, dim, size, isSpline);
        }
      }
    }
    if (*input != 0) {
      DBG("unparsed: <<%s>>", input);
    }
  }

  void exportVariablesList(const char * filename, const char * macroName)
  {
    const Array<Variables::Assoc> & assocs = intro.variables.All();

    FILE * fp = fopen(filename, "w");
    assert(fp != NULL);
    fprintf(fp, "// Generated file\n\n#ifndef %s\n#define %s\n\n", macroName, macroName);

    fprintf(fp, "enum variableId {\n");
    for (int i = 0; i < assocs.size; ++i)
    {
      fprintf(fp, "    %s%s,\n", assocs[i].name, i == 0 ? " = 0" : "");
    }
    fprintf(fp, "\n    numberOfVariables,\n");
    fprintf(fp, "};\n\n#endif\n");
    fclose(fp);
  }

  void exportVariablesData(const char * filename, const char * macroName)
  {
    const Array<Variables::Assoc> & assocs = intro.variables.All();

    FILE * fp = fopen(filename, "w");
    assert(fp != NULL);
    fprintf(fp, "// Generated file\n\n#ifndef %s\n#define %s\n\n", macroName, macroName);

    fprintf(fp, "const int variableDef[] = {\n");
    for (int i = 0; i < assocs.size; ++i)
    {
      const Variable & var = *assocs[i].var;
      fprintf(fp, "    %d, %d, %d,\n", var.dim(), var.size(), var.isSpline() ? 1 : 0);
    }
    fprintf(fp, "};\n\n\n");

    fprintf(fp, "const float variableData[] = {\n");
    for (int i = 0; i < assocs.size; ++i)
    {
      const Variable & var = *assocs[i].var;
      for (int j = 0; j < var.size() * var.dim(); ++j)
      {
	fprintf(fp, " %ff,", var.data()[j]);
      }
      fprintf(fp, "\n");
    }
    fprintf(fp, "};\n\n#endif\n");
    fclose(fp);
  }

  void load(const char * filename)
  {
    FILE * fp = fopen(filename, "r");
    assert(fp != NULL);

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
    readInput(src);
    msys_mallocFree(src);

    exportVariablesList("data/exported-variableId.hh", "EXPORTED_VARIABLE_ID_HH");
    exportVariablesData("data/exported-variablesData.hh", "EXPORTED_VARIABLES_DATA_HH");
  }
#else

# include "../data/exported-variablesData.hh"

  void load(const char * filename)
  {
    int dataOffset = 0;
    size_t defIndex = 0;
    for (size_t i = 0; i < VariablesEnum::numberOfVariables; ++i)
    {
      const float * data = variableData + dataOffset;
      const int dim = variableDef[defIndex++];
      const int size = variableDef[defIndex++];
      const bool isSpline = variableDef[defIndex++] != 0;

      allVariables[i] = Variable(data, dim, size, isSpline);
      dataOffset += dim * size;
    }
  }
#endif // NETWORK
}
