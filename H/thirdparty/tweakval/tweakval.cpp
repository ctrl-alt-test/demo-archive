#ifdef ENABLE_TWEAKVAL

#include <map>
#include <vector>
#include <string>
#include <cstring>

#include <sys/stat.h> 

#include "tweakval.h"

namespace tweakval
{
bool isEnabled = true; // Disable tweakval at runtime.
	
struct Tweakable
{
	enum TweakableType
	{
		Type_INT,
		Type_FLOAT,
		Type_STRING,
	};

	TweakableType type;
	union {
		float f_val;
		int i_val;
		char *s_val;
	} val;
};

struct TweakableSourceFile
{
	std::string filename;
	time_t modTime;
};

// List of tweakable files to check for updates
typedef std::vector<TweakableSourceFile> TweakFileList;
static TweakFileList g_tweakableFile;

// Stored tweak values
typedef std::pair< std::string, size_t> TweakValKey;
typedef std::map< TweakValKey, Tweakable*> TweakValMap;
static TweakValMap g_tweakVals;

Tweakable *LookupTweakValue( const char *file, size_t counter )
{
	TweakValMap::iterator fi = g_tweakVals.find( TweakValKey( file, counter ) );
	if (fi != g_tweakVals.end())
	{
		return (*fi).second;
	}
	else return NULL;
}

time_t GetFileModTime( const std::string &filename )
{	
	struct stat statInfo;
	stat( filename.c_str(), &statInfo );	
	return statInfo.st_mtime;
}

Tweakable *AddTweakableValue( const std::string &filename, size_t counter, Tweakable *twk )
{
	// First, see if this file is in the files list
	bool found = false;
	for( TweakFileList::iterator ti = g_tweakableFile.begin();
		  ti != g_tweakableFile.end(); ++ti )
	{
		if (ti->filename == filename)
		{
			found = true;
			break;
		}
	}
	
	// if it's not found, add to the list of tweakable files,
	// assume it's unmodified since the program has been built 
	if (!found)
	{
		TweakableSourceFile srcFile;
		srcFile.filename = filename;

		// use the initial modification time		
		srcFile.modTime = GetFileModTime( filename );
		
		g_tweakableFile.push_back( srcFile );
	}

	// add to the tweakables
	g_tweakVals[ TweakValKey( filename, counter) ] = twk;
	return twk;
}

void ReloadTweakableFile( TweakableSourceFile &srcFile )
{	
	size_t counter = 0;
	FILE *fp = fopen( srcFile.filename.c_str(), "rt" );
	
	if (fp == NULL)
	{
		return;
	}

	char line[2048], strval[512];
	while (!feof( fp ))
	{
		fgets( line, 2048, fp );
		char *ch = line;

		// chop off c++ comments. C style comments, and 
		// preprocessor directives like #if 0 are not currently
		// handled so beware if you use those too much
		char *comment = strstr( line, "//" );
		if (comment)
		{
			*comment = 0;
		}

		// Abuse compiler string concatination.. since the parser isn't smart enough
		// to skip _TV( in quoted strings (but it should skip this comment)
		while ((ch=strstr( ch, "_T" "V(")) )
		{
			ch += 4; // skip the _TV( value

			char *chend;
			if (*ch == '"')
			{
				// It's a string
				ch++;
				chend = strstr( ch, "\"" );
			}
			else
			{
				chend = strstr( ch, ")" );
			}
			if (chend)
			{
				strncpy( strval, ch, chend-ch );
				strval[ chend-ch ] = '\0';

				//printf("TWK: %s, %d : val '%s'\n", srcFile.filename.c_str(), counter, strval );
				ch = chend;

				// Apply the tweaked value (if found)
				Tweakable *tv = LookupTweakValue( srcFile.filename.c_str(), counter );
				if (tv)
				{
					if (tv->type == Tweakable::Type_INT)
					{
						tv->val.i_val = atoi( strval );
					}
					else if (tv->type == Tweakable::Type_FLOAT)
					{
						tv->val.f_val = (float)atof( strval );
					}
					else if (tv->type == Tweakable::Type_STRING)
					{
						tv->val.s_val = strdup(strval);
					}
				}
			}
			else
			{
				break; // Unmatched parenthesis
			}
			counter++;
		}
	}
	fclose( fp );
}

} // namespace tweakval

using namespace tweakval;
float _TweakValue( const char *file, size_t counter, float origVal )
{
	if (!isEnabled)
	{
		return origVal;
	}

	Tweakable *tv = LookupTweakValue( file, counter );
	if (tv)
	{
		return tv->val.f_val;
	}
	else 
	{
		Tweakable *twk = new Tweakable();
		twk->type = Tweakable::Type_FLOAT;
		twk->val.f_val = origVal;
		return AddTweakableValue( file, counter, twk )->val.f_val;
	}
}

int _TweakValue( const char *file, size_t counter, int origVal )
{
	if (!isEnabled)
	{
		return origVal;
	}

	Tweakable *tv = LookupTweakValue( file, counter );
	if (tv)
	{
		return tv->val.i_val;
	}
	else 
	{
		Tweakable *twk = new Tweakable();
		twk->type = Tweakable::Type_INT;
		twk->val.i_val = origVal;
		return AddTweakableValue( file, counter, twk )->val.i_val;
	}
}

char *_TweakValue( const char *file, size_t counter, char *origVal )
{
	if (!isEnabled)
	{
		return origVal;
	}

	Tweakable *tv = LookupTweakValue( file, counter );
	if (tv)
	{
		return tv->val.s_val;
	}
	else
	{
		Tweakable *twk = new Tweakable();
		twk->type = Tweakable::Type_STRING;
		twk->val.s_val = origVal;
		return AddTweakableValue( file, counter, twk )->val.s_val;
	}
}

void ReloadChangedTweakableValues()
{
	// Go through the list of Tweakable Files and see if any
	// have changed since their last modification time
	for( TweakFileList::iterator ti = g_tweakableFile.begin();
		  ti != g_tweakableFile.end(); ++ti )
	{
		TweakableSourceFile &src = (*ti);
		time_t currModTime = GetFileModTime( src.filename );
		if (currModTime > src.modTime)
		{			
			ReloadTweakableFile( src );
			src.modTime = currModTime;
		}
	}
}
#endif

//===================================================================
// Example/test
//===================================================================
#ifdef _TV_TEST_TWEAKVAL
#include <windows.h>
#include <stdio.h>
#include <time.h>
int main( int argc, char *argv[] )
{
    while (1)
    {
        int val = _TV( 50 ), val2 = _TV( 19 );
        float fval = _TV( 3.14152f );

        
        printf("value is %d, %d, valuef is %f\n", val, val2, fval );
        Sleep( 1000 );
        
        // call this once per frame (or whatever interval you want to 
		// check for updates)
#ifndef NDEBUG
        ReloadChangedTweakableValues();        
#endif
    }
    
}
#endif // !ENABLE_TWEAKVAL
