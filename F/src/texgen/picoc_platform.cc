// Attention, ce fichier est utilis� � la fois par la d�mo et par le texgen en ligne.

extern "C" {
#include "../../tools/picoc/picoc.h"
}
#include "../sys/msys.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

/* a source file we need to clean up */
static char *CleanupText = NULL;

/* deallocate any storage */
void PlatformCleanup()
{
    if (CleanupText != NULL)
        free(CleanupText);
}

/* get a line of interactive input */
char *PlatformGetLine(char *Buf, int MaxLen)
{
    fflush(stdout);
    return fgets(Buf, MaxLen, stdin);
}

/* get a character of interactive input */
int PlatformGetCharacter()
{
    fflush(stdout);
    return getchar();
}

/* write a character to the console */
void PlatformPutc(unsigned char OutCh, union OutputStreamInfo *Stream)
{
    msys_debugPrintf("%c", OutCh);
}

/* read a file into memory */
char *PlatformReadFile(const char *FileName)
{
    struct stat FileInfo;
    char *ReadText;
    FILE *InFile;
    int BytesRead;
    
    if (stat(FileName, &FileInfo))
        ProgramFail(NULL, "can't read file %s\n", FileName);
    
    ReadText = (char*) malloc(FileInfo.st_size + 1);
    if (ReadText == NULL)
        ProgramFail(NULL, "out of memory\n");
        
    InFile = fopen(FileName, "r");
    if (InFile == NULL)
        ProgramFail(NULL, "can't read file %s\n", FileName);
    
    BytesRead = fread(ReadText, 1, FileInfo.st_size, InFile);
    if (BytesRead == 0)
        ProgramFail(NULL, "can't read file %s\n", FileName);

    ReadText[BytesRead] = '\0';
    fclose(InFile);
    
    return ReadText;    
}

/* read and scan a file for definitions */
void PlatformScanFile(const char *FileName)
{
    char *SourceStr = PlatformReadFile(FileName);
    char *OrigCleanupText = CleanupText;
    if (CleanupText == NULL)
        CleanupText = SourceStr;

    Parse(FileName, SourceStr, strlen(SourceStr), TRUE);
    free(SourceStr);

    if (OrigCleanupText == NULL)
        CleanupText = NULL;
}

/* mark where to end the program for platforms which require this */
//jmp_buf ExitBuf;

/* exit the program */
void PlatformExit()
{
  DBG("Exiting picoc.");
  throw "Picoc: exit";
  // longjmp(ExitBuf, 1);
}

