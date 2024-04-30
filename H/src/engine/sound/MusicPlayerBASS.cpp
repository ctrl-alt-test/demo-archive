#include "MusicPlayerBASS.hh"

#include "bass/c/bass.h"
#include "engine/core/Debug.hh"
#include "engine/container/Utils.hh"
#include <cassert>

using namespace Sound;

struct ErrorDescription
{
	int			error;
	const char*	description;
};

static const ErrorDescription errorDescriptionLUT[] = {
	{ BASS_OK,				"all is OK",										},
	{ BASS_ERROR_MEM,		"memory error",										},
	{ BASS_ERROR_FILEOPEN,	"can't open the file",								},
	{ BASS_ERROR_DRIVER,	"can't find a free/valid driver",					},
	{ BASS_ERROR_BUFLOST,	"the sample buffer was lost",						},
	{ BASS_ERROR_HANDLE,	"invalid handle",									},
	{ BASS_ERROR_FORMAT,	"unsupported sample format",						},
	{ BASS_ERROR_POSITION,	"invalid position",									},
	{ BASS_ERROR_INIT,		"BASS_Init has not been successfully called",		},
	{ BASS_ERROR_START,		"BASS_Start has not been successfully called",		},
	{ BASS_ERROR_SSL,		"SSL/HTTPS support isn't available",				},
	{ BASS_ERROR_ALREADY,	"already initialized/paused/whatever",				},
	{ BASS_ERROR_NOCHAN,	"can't get a free channel",							},
	{ BASS_ERROR_ILLTYPE,	"an illegal type was specified",					},
	{ BASS_ERROR_ILLPARAM,	"an illegal parameter was specified",				},
	{ BASS_ERROR_NO3D,		"no 3D support",									},
	{ BASS_ERROR_NOEAX,		"no EAX support",									},
	{ BASS_ERROR_DEVICE,	"illegal device number",							},
	{ BASS_ERROR_NOPLAY,	"not playing",										},
	{ BASS_ERROR_FREQ,		"illegal sample rate",								},
	{ BASS_ERROR_NOTFILE,	"the stream is not a file stream",					},
	{ BASS_ERROR_NOHW,		"no hardware voices available",						},
	{ BASS_ERROR_EMPTY,		"the MOD music has no sequence data",				},
	{ BASS_ERROR_NONET,		"no internet connection could be opened",			},
	{ BASS_ERROR_CREATE,	"couldn't create the file",							},
	{ BASS_ERROR_NOFX,		"effects are not available",						},
	{ BASS_ERROR_NOTAVAIL,	"requested data is not available",					},
	{ BASS_ERROR_DECODE,	"the channel is/isn't a \"decoding channel\"",		},
	{ BASS_ERROR_DX,		"a sufficient DirectX version is not installed",	},
	{ BASS_ERROR_TIMEOUT,	"connection timedout",								},
	{ BASS_ERROR_FILEFORM,	"unsupported file format",							},
	{ BASS_ERROR_SPEAKER,	"unavailable speaker",								},
	{ BASS_ERROR_VERSION,	"invalid BASS version (used by add-ons)",			},
	{ BASS_ERROR_CODEC,		"codec is not available/supported",					},
	{ BASS_ERROR_ENDED,		"the channel/file has ended",						},
	{ BASS_ERROR_BUSY,		"the device is busy",								},
	{ BASS_ERROR_UNKNOWN,	"some other mystery problem",						},
};

bool MusicPlayerBASS::Init(const char* mp3Filename)
{
	if (!BASS_Init(-1, 44100, 0, 0, nullptr))
	{
		int error = BASS_ErrorGetCode();
		assert(error < ARRAY_LEN(errorDescriptionLUT));
		LOG_ERROR("Cannot init BASS player : %s", mp3Filename, errorDescriptionLUT[error].description);
		return false;
    }

	if (!(m_channelHandle = BASS_StreamCreateFile(FALSE, mp3Filename, 0, 0, BASS_SAMPLE_LOOP)) &&
		(!(m_channelHandle = BASS_MusicLoad(FALSE, mp3Filename, 0, 0, BASS_MUSIC_RAMP | BASS_SAMPLE_LOOP, 1))))
	{
		int error = BASS_ErrorGetCode();
		assert(error < ARRAY_LEN(errorDescriptionLUT));
		LOG_ERROR("Cannot open file %s : %s", mp3Filename, errorDescriptionLUT[error].description);
		return false;
	}
	return true;
}

void MusicPlayerBASS::Play(unsigned a_time)
{
	QWORD position = BASS_ChannelSeconds2Bytes(m_channelHandle, a_time / 1000.);
	BASS_ChannelSetPosition(m_channelHandle, position, BASS_POS_BYTE);
	BASS_ChannelPlay(m_channelHandle, 0);
}

void MusicPlayerBASS::Stop()
{
	BASS_ChannelStop(m_channelHandle);
}
