
#include "specific/scenes.hh"
#include "specific/seagul.hh"

#include "array.hxx"
#include "algebra/vector4.hxx"
#include "camera.hh"
#include "demo.hh"
#include "interpolation.hxx"
#include "intro.hh"
#include "materials.hh"
#include "mesh/meshpool.hh"
#include "meshes.hh"
#include "shaders.hh"
#include "snd/midisync.hh"
#include "snd/sound.hh"
#include "texgen/texture.hh"
#include "tweakval.hh"
#include "variable.hh"
#include "vbos.hh"

#include "sys/msys_debug.h"
#include "sys/msys_libc.h"

namespace Demo
{
  // Scenes
  Node * create()
  {
    return Scenes::create();
  }

  const int fftsize = 2048;
  const int sampleFreq = 441;

  float getTranslatedPos(date d) {
    // Pas de translation à partir du désert
    float t = msys_min(116000.f, (float) d);
    // Vitesse au début : 1 m/s -- hmm nope, ce n'est pas 1m/s; à recalculer
    // Puis accélération entre t1 et t2, puis vitesse constante
    float t1 = 44000.f;
    float t2 = 53000.f;
    float topSpeed = 2.f;
    float pos = t + smoothStep(t1, t2, t) * (t - t1) * topSpeed;
    return -4.f * pos * sampleFreq / float(10 * fftsize);
  }

  int iPosFromDate(date t) { return (t * sampleFreq) / (10 * fftsize); }
  static int dateFromRow(int row) { return (row * 10 * fftsize) / sampleFreq; }

  static void updateFFT(date t)
  {
    Array<float*> * fft = Sound::getFFT();

    int shift = _TV(65);
    int pos = iPosFromDate(t);
    int row = (pos + shift) % 256;
    if (pos >= 0 && pos < fft->size)
      Texture::list[Texture::fftMap].updateDynamicTexture(0, row, 256, 1, GL_FLOAT, (*fft)[pos]);

    Texture::list[Texture::fftMap].use(Texture::fft);
  }

  void preloadFFT(date t)
  {
    for (int i = 0; i < 256; ++i)
      updateFFT(t + dateFromRow(i));
  }

  void update(date t)
  {
    updateFFT(t + dateFromRow(256 - _TV(65)));
    if ((int) t < 110000 || (int) t > 280000)
      Seagul::textureAnimation(intro.now.story);
  }

  void generateMeshes()
  {
    Mesh::initPool();
    Scenes::generateMeshes();
    Meshes::buildAll();
    Mesh::destroyPool();
  }

  vector4f countryShading(const Clock & now, const vector4f& color)
  {
    const int firstBeatTime = _TV(102400);
    const int lastBeatTime  = _TV(114400);
    const int beatDuration  = _TV(600); // en ms
    const int numberOfBeats = _TV(14);

    const int time = (int) now.youtube;
    if (time < firstBeatTime || time > lastBeatTime + beatDuration) return color;

    const int beatSpace = (lastBeatTime - firstBeatTime) / (numberOfBeats - 1);
    const int startTime = firstBeatTime + ((time - firstBeatTime) / beatSpace) * beatSpace;
    const int endTime = startTime + beatDuration;

    const float beatIntensity = interpolate(time, 101870, 101870 + _TV(7000));//smoothStep(101870, 101870 + _TV(7000), time);
    const float beatDecay = (1.f - smoothStep(startTime, endTime, time));
    const float beat = beatIntensity * beatDecay * beatDecay;
    const vector4f beatColor(_TV(1.0f), _TV(0.69f), _TV(0.13f), _TV(0.5f));

    return vector4f(mix(color.x, beatColor.x, beat),
		    mix(color.y, beatColor.y, beat),
		    mix(color.z, beatColor.z, beat),
		    mix(color.w, beatColor.w, beat));
  }

  //
  // Shading différent pendant la scène de l'autoroute
  //
  void highwayShading(const Clock & now)
  {
    Shader::uniforms[Shader::retroParams].set(vector4f(1.f, 1.f, 1.f, 0.f));
    Shader::uniforms[Shader::retroEdgeColor].set(vector4f(0.f, 0.f, 0.f, 1.f));
    Shader::uniforms[Shader::retroFaceColor].set(vector4f(1.f, 1.f, 1.f, 1.f));
  }

  void setFrameParams(const Clock & now)
  {
    const float t = (float)now.camera;//youtube;

    // Light
    Light::updateLightPositions(now.camera);

    Shader::uniforms[Shader::ambient].set(VAR(lightAmbientColor)->get4(t));
    Shader::uniforms[Shader::l0Color].set(VAR(light0Color)->get4(t));
    Shader::uniforms[Shader::l1Color].set(VAR(light1Color)->get4(t));

    // Si pos[3] == 0, c'est une lumière directionnelle et on ne veut
    // pas d'atténuation.
    // Sinon, on élève au carré juste pour que les valeurs soient plus
    // intuitives à éditer (l'atténuation est fonction du carré de la
    // distance).
    float pointLightAttenuation = VAR(pointLightAttenuation)->get1(t);
    pointLightAttenuation *= pointLightAttenuation;
    Shader::uniforms[Shader::l0Attenuation].set(Shader::uniforms[Shader::l0Position].getf()[3] == 0 ? 0 : pointLightAttenuation);
    Shader::uniforms[Shader::l1Attenuation].set(Shader::uniforms[Shader::l1Position].getf()[3] == 0 ? 0 : pointLightAttenuation);

    // Wireframe
    Shader::uniforms[Shader::retroEdgeThick].set(VAR(retroEdgeThick)->get2(t));
    Shader::uniforms[Shader::retroFaceColor].set(VAR(retroFaceColor)->get4(t));
    Shader::uniforms[Shader::retroEdgeColor].set(countryShading(now, VAR(retroEdgeColor)->get4(t)));
    Shader::uniforms[Shader::retroParams].set(VAR(retroParams)->get4(t));

    // Lens effects
    Shader::uniforms[Shader::lensFishEye].set(VAR(lensFishEye)->get1(t));
    Shader::uniforms[Shader::lensGlowPower].set(VAR(lensGlowPower)->get1(t));
    Shader::uniforms[Shader::lensStreakPower].set(VAR(lensStreakPower)->get2(t));
    Shader::uniforms[Shader::lensOrbsSize].set((float)intro.xres * VAR(lensOrbsSize)->get4(t));
    Shader::uniforms[Shader::lensOrbsColor].set(VAR(lensOrbsColor)->get4(t));
    Shader::uniforms[Shader::lensFlareColor].set(VAR(lensFlareColor)->get4(t));
    Shader::uniforms[Shader::lensGlowColor].set(VAR(lensGlowColor)->get4(t));
    Shader::uniforms[Shader::lensStreakColor].set(VAR(lensStreakColor)->get4(t));

    // Other post processing params
    Shader::uniforms[Shader::vignetting].set(VAR(vignetting)->get2(t));
    Shader::uniforms[Shader::grain].set(VAR(grainParams)->get3(t));
    Shader::uniforms[Shader::fade].set(VAR(fade)->get1(t));
    Shader::uniforms[Shader::fadeLuminance].set(VAR(fadeLuminance)->get1(t));

    // G specific: Y position of the camera
    Shader::uniforms[Shader::yPos].set(getTranslatedPos(now.youtube));
    Shader::uniforms[Shader::oldYPos].set(getTranslatedPos(now.youtube - 16));

    // Sky / fog
    Shader::uniforms[Shader::skyColor].set(VAR(skyColor)->get4(t));
    Shader::uniforms[Shader::fogColor].set(VAR(fogColor)->get4(t));
    Shader::uniforms[Shader::fogDensity].set(VAR(fogDensity)->get1(t));

    Camera::Camera cam = Camera::getCamera(now.camera);
    if (cam.focus == 0.f)
      highwayShading(now);
  }

  // Camera
  const char * cameraFile = "../data/camera/factory.txt";

  const Camera::KeyframeSource cameraFrames[] = {
  #include "../data/camera/factory.txt"
  };

  const int cameraFramesLength = ARRAY_LEN(cameraFrames);
}
