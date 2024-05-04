#ifndef __SYNTH_H__
#define __SYNTH_H__

#ifdef _WIN32
# include "windows.h"
#else
// Linux
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 64klang core interface functions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _64klang_Init			(BYTE* songStream, void* patchData, DWORD const1Offset, DWORD const2Offset, DWORD maxoffset);
int  _64klang_ACMConvert	(void* srcFormat, void* dstFormat, LPBYTE srcBuffer, DWORD srcBufferSize, LPBYTE& dstBuffer, DWORD& dstBufferSize);
#ifdef COMPILE_VSTI
void _64klang_NoteOn		(DWORD channel, DWORD note, DWORD velocity);
void _64klang_NoteOff		(DWORD channel, DWORD note, DWORD velocity);
void _64klang_NoteAftertouch(DWORD channel, DWORD note, DWORD value);
void _64klang_MidiSignal	(DWORD channel, int value, DWORD cc);
void _64klang_SetBPM		(float bpm);
void _64klang_Tick			(float* left, float* right, DWORD samples);
#else
void __cdecl _64klang_Render(float* dstbuffer);
bool _64klang_RenderDone();
#endif

#endif
