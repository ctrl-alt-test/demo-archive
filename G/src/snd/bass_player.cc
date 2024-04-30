#include "bass.h"
#include "array.hxx"
#include "loading.hh"
#include "sys/msys.h"

namespace Sound
{
  static DWORD chan;
  static const char * mp3Filename = "data/sound/hand - Level One.mp3";
  static Array<float*> FFT;

#define FFT_SAMPLES 1024
#define FFT_GET_DATA_FLAGS BASS_DATA_FFT2048

  Array<float> buffer;

  bool init()
  {
    if (!(chan = BASS_StreamCreateFile(FALSE, mp3Filename, 0, 0, BASS_SAMPLE_LOOP)) &&
       (!(chan = BASS_MusicLoad(FALSE, mp3Filename, 0, 0, BASS_MUSIC_RAMP | BASS_SAMPLE_LOOP, 1)))) {
      //Error("Can't play file");
      return false; // Can't load the file
    }
    return true;
  }

  bool precalcFFT()
  {
    START_TIME_EVAL;

    if (!BASS_Init(-1, 44100, 0, 0, NULL))
      return false;
    int stream = BASS_StreamCreateFile(FALSE, mp3Filename, 0, 0,
        BASS_STREAM_DECODE  | BASS_SAMPLE_FLOAT | BASS_STREAM_PRESCAN);
    if (stream == 0)
      return false;

    buffer.init(FFT_SAMPLES);
    buffer.size = FFT_SAMPLES;
    FFT.init(8000);

    while (true)
    {
      int res = BASS_ChannelGetData(stream, buffer.elt, FFT_GET_DATA_FLAGS);

      // On alloue 8000x256x4 -> ~8Mo pour la fft
      float * segment = new float[FFT_SAMPLES / 4];
      for (int i = 0; i < FFT_SAMPLES; i += 4)
      {
        segment[i/4] = msys_sqrtf((buffer[i] + buffer[i+1] + buffer[i+2] + buffer[i+3])) / 2.f;
      }
      if (res == -1) break;
      FFT.add(segment);
    }
    BASS_ChannelStop(stream);

    END_TIME_EVAL("FFT precalc");
    return true;
  }

  Array<float*> * getFFT()
  {
    return & FFT;
  }

  void play(unsigned a_time)
  {
    QWORD position = BASS_ChannelSeconds2Bytes(chan, a_time / 1000.);
    BASS_ChannelSetPosition(chan, position, BASS_POS_BYTE);
    BASS_ChannelPlay(chan, 0);
  }

  void stop()
  {
    BASS_ChannelStop(chan);
  }

  void getWaveform(float *data, int size)
  {
    BASS_CHANNELINFO ci;
    BASS_ChannelGetInfo(chan, &ci); // get number of channels
    BASS_ChannelGetData(chan, data, (ci.chans * size * sizeof(float)) | BASS_DATA_FLOAT); // get the sample data (floating-point to avoid 8 & 16 bit processing)

    // Ex:
    // for (c = 0; c < ci.chans; c++) {
    //   for (x = 0; x < size; x++) {
    //     int y = (1 - buf[x * ci.chans + c]) * SPECHEIGHT / 2; // invert and scale to fit display
    //     plot(x, y);
  }

  void getFFT256Samples(float *fft)
  {
    BASS_ChannelGetData(chan, fft, BASS_DATA_FFT512);
  }

  float getIntensity()
  {
    buffer.size = FFT_SAMPLES;

    float sum = 0.f;
    BASS_ChannelGetData(chan, buffer.elt, FFT_GET_DATA_FLAGS);
    for (int i = 0; i < FFT_SAMPLES; ++i)
    {
      // on pourrait utiliser un coefficient pour faire ressortir certaines fréquences
      sum += buffer[i];
    }
    return sum;
  }
}
