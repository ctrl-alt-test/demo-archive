#include "timeline.hh"

#include "interpolation.hh"
#include "intro.h"
#include "factory.hh"
#include "shaderid.hh"
#include "textures.hh"
#include "tweakval.h"

#define TEXT_FADE_IN_DURATION 800
#define TEXT_FADE_OUT_DURATION 1000

#define DEBUG_FADE 0

namespace Timeline
{
  // Gestion des fo%ndus
  void doFade(date renderdate)
  {
    float fade_coef = 1.f;
    fade_coef *= Shader::calcFade(sphereStartDate + _TV(29000), sphereStartDate + _TV(30000), renderdate, false);
    fade_coef *= Shader::calcFade(sphereStartDate + _TV(34000), sphereStartDate + _TV(35000), renderdate, true);
    if (renderdate > sphereStartDate + _TV(30000) && renderdate < sphereStartDate + _TV(34000))
      fade_coef = 0.f;

    if (renderdate > cityStartDate && renderdate < cityEndDate)
    {
      fade_coef *= Shader::calcFade(cityStartDate + _TV(36000), cityStartDate + _TV(36700), renderdate, false);
      fade_coef *= Shader::calcFade(cityStartDate + _TV(36700), cityStartDate + _TV(37000), renderdate, true);
      fade_coef *= Shader::calcFade(cityStartDate + _TV(40500), cityStartDate + _TV(41200), renderdate, false);
      fade_coef *= Shader::calcFade(cityStartDate + _TV(41200), cityStartDate + _TV(41500), renderdate, true);
    }
    else if (renderdate > rampStartDate && renderdate < forestStartDate)
    {
      fade_coef *= Shader::calcFade(starfieldStartDate - 500, starfieldStartDate, renderdate, false);
      fade_coef *= Shader::calcFade(starfieldStartDate      , starfieldStartDate + 1000, renderdate, true);

      fade_coef *= Shader::calcFade(forestStartDate - 200, forestStartDate, renderdate, false);
      fade_coef *= Shader::calcFade(forestStartDate      , forestStartDate + 500, renderdate, true);
    }
    else if (renderdate > elevationStartDate)
    {
      fade_coef *= Shader::calcFade(elevationEndDate - 2000, elevationEndDate, renderdate, false);


      // Fondu noir final
      // ================
      fade_coef *= Shader::calcFade(demoEndDate - 12000, demoEndDate - 8000, renderdate, false);
      if (renderdate >= demoEndDate - 8000)
      {
	fade_coef = 0;
      }
    }


    float fade_luminance = 0;
    if (renderdate >= forestStartDate - 1000 && renderdate < forestStartDate + 1000)
      fade_luminance = 1.f;

#if DEBUG_FADE
    fade_coef = 0.2f + 0.8f * fade_coef;
#endif

    Shader::unicast1f(Shader::postProcess, Shader::fade, fade_coef);
    Shader::unicast1f(Shader::postProcess, Shader::fadeLuminance, fade_luminance);
    Shader::unicast1f(Shader::glow, Shader::fade,  fade_coef);
    Shader::unicast1f(Shader::glow, Shader::fadeLuminance, fade_luminance);
  }

  bool setSkybox(date renderDate,
		 Texture::id & wallsTextureId, Texture::id & roofTextureId,
		 Texture::id & wallsTextureId2, Texture::id & roofTextureId2)
  {
    if (renderDate < factoryEndDate)
      return false;
    if (renderDate >= screenStartDate)
      return false;

    float coef = 0;

    if (renderDate >= cityStartDate &&
	renderDate <= cityEndDate)
    {
      if (renderDate <= cityEndDate - _TV(8000))
      {
	wallsTextureId = Texture::citySky;
	roofTextureId = Texture::citySkyRoof;
      }
      else
      {
        wallsTextureId = Texture::black;
	roofTextureId = Texture::black;
      }

      wallsTextureId2 = Texture::rampBox;
      roofTextureId2 = Texture::rampBox;

      if (renderDate >= cityEndDate - _TV(5000))
        coef = smoothStepI(cityEndDate - _TV(5000), cityEndDate, renderDate);
    }
    else if (renderDate >= rampStartDate - 2000 &&
	     renderDate < rampEndDate)
    {
      wallsTextureId = Texture::rampBox;
      roofTextureId = Texture::rampBox;
      wallsTextureId2 = Texture::rampBox2;
      roofTextureId2 = Texture::rampBox2;

      coef = smoothStepI(rampStartDate, rampEndDate, renderDate);
    }
    else if (renderDate >= starfieldStartDate &&
	     renderDate < starfieldEndDate)
    {
      wallsTextureId = Texture::spaceBox;
      roofTextureId = Texture::spaceBox;
    }

    /*
    if (renderDate >= starfieldEndDate && renderDate < rampStartDate + 2000)
    {
      textureId = Texture::spaceBox;
      textureId2 = Texture::rampBox;
      coef = smoothStepI(starfieldEndDate, rampStartDate+2000, renderDate);
    }
    */

    if (renderDate >= forestStartDate &&
	renderDate < forestEndDate + 4000)
    {
      wallsTextureId = Texture::forestSky;
      roofTextureId = Texture::forestSkyRoof;
      wallsTextureId2 = Texture::black;
      roofTextureId2 = Texture::black;

      // Fondu vers noir
      if (renderDate >= forestEndDate)
      {
        coef = smoothStepI(forestEndDate, forestEndDate + 4000, renderDate);
      }
    }
    else if (renderDate >= forestEndDate + 4000)
    {
      wallsTextureId = Texture::black;
      roofTextureId = Texture::black;
    }

    Shader::unicast1f(Shader::texture, Shader::textureFade, coef);
    return true;
  }

  static float getGlowFlash(date t)
  {
    float glow = 1.f;
    glow *= Shader::calcFade(cityStartDate+4000, cityStartDate+4500, t, false);
    glow *= Shader::calcFade(cityStartDate+8000, cityStartDate+9500, t, false);
    for (date i = rampStartDate; i < rampEndDate; i += 5000)
      glow *= Shader::calcFade(i, i + 800, t, false);
    return glow >= 1.f ? 0.f : glow;
  }

  Shader::id getPostProcess(date date)
  {
    Shader::unicast1f(Shader::glow, Shader::glowFlash, getGlowFlash(date));
    if (date > cityStartDate + 14800 && date < 136700 /* pas de cityStartDate, désolé ! */)
      return Shader::godRays;
    return Shader::glow;
  }


  typedef struct
  {
    date start;
    date duration;
    float x; // > 0 : depuis la gauche ; < 0 : depuis la droite ; 0 : centré
    float y; // > 0 : depuis le haut ;   < 0 : depuis le bas ;    0 : centré
    char * str;
  } textMessage;


  //
  //  /!\ Tableau de greetings /!\
  //
  //  Pour pouvoir utiliser _TV(), il est dans une fonction.
  //  Du coup, il faut penser à mettre à jour le nombre suivant
  //
  static const unsigned int numberOfgreetings = 12; //sizeof(greetings) / sizeof(textMessage);
  static const textMessage greetings(unsigned int i, int textHeight)
  {
    textMessage greetings[numberOfgreetings] = {
      { _TV(19000), _TV(3000), _TV(-0.20f), _TV(-0.25f), "Ctrl-Alt-Test presents" },
      { _TV(35300), _TV(3000), _TV( 0.15f), _TV( 0.38f), "a 64kB production" },
      { _TV(38400), _TV(2500), _TV( 0.05f), _TV( 0.36f), "for Evoke 2010" },

      { _TV(83000), _TV(5000), _TV(0.2f), _TV(0.35f), "Incubation" },
      { _TV(84500), _TV(2500), _TV(-0.1f), _TV(-0.25f), "Made exclusively with cubes" },

      { cityStartDate + _TV(36500), _TV(4000),      _TV(-0.10f), _TV( 0.02f), "Code: LLB, Zavie" },
      { cityStartDate + _TV(38500), _TV(4000),      _TV(-0.10f), _TV( 0.09f), "Music: Cyborg Jeff, LLB" },
      { cityStartDate + _TV(40500), _TV(4000),      _TV(-0.10f), _TV( 0.16f), "Additional code: Nicuveo, Tarmil" },

      { elevationStartDate + _TV(10000), _TV(5000), _TV(0.10f), _TV( 0.17f), "You have been watching" },
      { elevationStartDate + _TV(10500), _TV(7500), _TV(-0.60f), _TV( 0.24f), "Incubation" },

      //{ screenStartDate + _TV( 6000), _TV(4000),    _TV(-0.60f), _TV( 0.15f), "Hail to the elders" },
      //{ screenStartDate + _TV( 7000), _TV(4000),    _TV(-0.60f), _TV( 0.20f), "Hail to the newcomers" },
      //{ screenStartDate + _TV(11000), _TV(4000),    _TV(-0.15f), _TV(-0.25f), "We love your work!" },
      //{	screenStartDate + _TV(12000), _TV(7000),    _TV(-0.15f), _TV(-0.17f), "Continue making us dream..." },

      {	_TV(341000), _TV(6000), _TV(0.f), _TV( -0.50f), "Ctrl-Alt-Test" },
      {	_TV(343000), _TV(4000), _TV(0.f), _TV( 0.55f), "2010" },

      /*
      { factoryStartDate + _TV(12000), factoryStartDate + _TV(4000), -100, 100, "\"For some esoteric reason demosceners love cubes\"" },
      { factoryStartDate + _TV(13000), factoryStartDate + _TV(4000), -100, 200, "(IQ/RGBA)" },
      { factoryStartDate + _TV(16000), factoryStartDate + _TV(8000), 100, -200, "\"Intros without cubes normally show spheres!\"" },
      { factoryStartDate + _TV(17000), factoryStartDate + _TV(8000), 100, -200 + textHeight, "(same author)" },
      { factoryStartDate + _TV(20000), factoryStartDate + _TV(12000), -100, -200, "\"But spheres are not as cool as cubes!\"" },
      { factoryStartDate + _TV(21000), factoryStartDate + _TV(12000), -100, -200 + textHeight, "(still same author)" },
      */
    };
    return greetings[i];
  }

  static void printGreeting(const textMessage & greeting, const IntroObject & intro)
  {
    float alpha = 1.f;
    /*
    if (intro.youtubeNow < greeting.start + TEXT_FADE_IN_DURATION)
    {
      alpha = (intro.youtubeNow - greeting.start) / float(TEXT_FADE_IN_DURATION);
    }
    if (intro.youtubeNow > greeting.end - TEXT_FADE_OUT_DURATION)
    {
      alpha = (intro.youtubeNow - greeting.end) / float(TEXT_FADE_OUT_DURATION);
    }
    */
    glColor4f(1.f, 1.f, 1.f, 0.7f * alpha);

    const float progress = smoothStepI(greeting.start, greeting.start + greeting.duration, intro.youtubeNow);
    Shader::unicast1f(Shader::text, Shader::trans, progress);

    float x = intro.xres * greeting.x;
    float y = intro.yres * greeting.y;

    if (0 == x)
    {
      x += 0.5f * (intro.xres - intro.font->width(greeting.str, intro.textHeight));
    }
    if (0 == y)
    {
      y += 0.5f * (intro.yres - intro.textHeight);
    }
    if (x < 0)
    {
      x += intro.xres - intro.font->width(greeting.str, intro.textHeight);
    }
    if (y < 0)
    {
      y += intro.yres - intro.textHeight;
    }
    intro.font->print(greeting.str, intro.textHeight, x, y);
  }

  void printText(const IntroObject & intro)
  {
    for (unsigned int i = 0; i < numberOfgreetings; ++i)
    {
      const textMessage & greeting = greetings(i, intro.textHeight);
      if (intro.youtubeNow >= greeting.start &&
	  intro.youtubeNow < (greeting.start + greeting.duration))
      {
	printGreeting(greeting, intro);
      }
    }
  }

  static void applyShakeCamera(Transformation &tr, float frequency, float intensity, float x)
  {
    if (x <= 0.f || x >= 1.f) return;
    x *= PI;
    tr.v.x += intensity * msys_sfrand()/10.f;
    tr.v.y += intensity * msys_sfrand()/10.f;
    tr.v.z += intensity * msys_sfrand()/10.f;
  }

  void shakeCamera(Transformation &tr, date t)
  {
    for (int i = 0; i < 5; i++)
    {
      date d = cityGrowingBuildingDate + 1000 * i;
      applyShakeCamera(tr, 20.f, 0.05f, smoothStepI(d, d + 500, t));
    }

    applyShakeCamera(tr, 20.f, 0.06f,
      smoothStepI(tetrisDate + 5000, tetrisDate + 5500, t));
    applyShakeCamera(tr, 20.f, 0.06f,
      smoothStepI(tetrisDate + 8000, tetrisDate + 8500, t));
  }

  Transformation getCamera(const IntroObject & intro)
  {
    const date animDate = intro.cameraNow;
    const int ropeTransitionTime = _TV(2000); // duree du cross-camera au debut de la rope
    Anim::Anim * cameraAnim = intro.cameraAnim;

    return cameraAnim->getTransformation(animDate, 0);


    // FIXME : cette partie est devenue bancale avec l'introduction
    // des ralentis, qui désynchronisent la caméra de l'histoire
/*
    if (intro.storyNow >= elevationStartDate &&
	intro.storyNow < elevationEndDate)
    {
      cameraAnim = intro.ropeCameraAnim;
      date ropeDate = intro.storyNow - elevationStartDate;
      ropeDate += _TV(2750); // decalage, hack
      if (intro.storyNow <= elevationStartDate + ropeTransitionTime)
      {
        float coef = smoothStepI(elevationStartDate, elevationStartDate + ropeTransitionTime, intro.storyNow);
        return Anim::getMixedTransformation(intro.cameraNow, ropeDate, *intro.cameraAnim, *intro.ropeCameraAnim, coef);
      }
      else
        return cameraAnim->getTransformation(ropeDate, 0);
    }
    else
      return cameraAnim->getTransformation(animDate, 0);
*/
  }

  // Contrôle de la vitesse de lecture

  struct timeControl
  {
    float speed;
    date storyTime;
    date cameraTime;
    date youtubeTime;
  };

#define CALC_SPEED(date1, date2, diffLength)  \
  {  (float)((date2) - (date1)) / ((date2) - (date1) + (diffLength)), (date1), 0, 0},    \
  {  1.f, (date2),  0, 0},

  doubleDate time_control(date youtubeTime)
  {
    timeControl tc[] = {
      {  1.0f,                             0, 0, 0},
      {  0.2f,      factoryStartDate + 28500, 0, 0},
      {  1.0f,      factoryStartDate + 29000, 0, 0},
      {     0,      factoryStartDate + 30000, 0, 0},
      {  1.0f,      factoryStartDate + 20000, 0, 0},

      CALC_SPEED(factoryStartDate + 20000, 41000, 1000)
      CALC_SPEED( 41000,  59000, -2300)
      CALC_SPEED( 59000,  70500, 1200)
      CALC_SPEED( 70500,  74500, -1000)
      CALC_SPEED( 74500, 104500, 1810)
      CALC_SPEED(104500, 170500, 1500)
      CALC_SPEED(170500, 181500, -600)
      CALC_SPEED(181500, 184000, 0)  // 2.5

      {  1.5f,      rampStartDate, 0, 0},
      {  1.0f,      rampStartDate +_TV(1000), 0, 0},
      {  2.0f,      rampStartDate +_TV(2000), 0, 0},
      {  1.0f,      rampStartDate +_TV(4000), 0, 0},
      {  1.3f,      rampStartDate+_TV(10000), 0, 0},
      { -3.f ,      rampStartDate+_TV(13800), 0, 0},
      {  3.f,       rampStartDate+_TV(13500), 0, 0},
      { -3.f ,      rampStartDate+_TV(13800), 0, 0},
      {  1.4f,      rampStartDate+_TV(13500), 0, 0},

      { -3.f ,      rampStartDate+_TV(23000), 0, 0},
      {  3.f,       rampStartDate+_TV(22700), 0, 0},
      { -3.f,       rampStartDate+_TV(23000), 0, 0},
      {  1.3f,      rampStartDate+_TV(22700), 0, 0},
      {  1.0f,      rampEndDate, 0, 0},

      {  0.83f,             starfieldStartDate, 0, 0},
      {      0,     starfieldStartDate + 13500, 0, 0},
      {  0.83f,     starfieldStartDate + 16000, 0, 0},
      {  0.955f,               forestStartDate, 0, 0},

      CALC_SPEED(screenStartDate, screenStartDate + 8000, -3200)
      CALC_SPEED(screenStartDate + 8000, demoEndDate, -4500)
//      {   1.f,      screenStartDate + 8, 0, 0},

      //CALC_SPEED(2210000, 2410000, 0)
      //CALC_SPEED(2410000, 2685000, 0)
      //CALC_SPEED(2920000, 2685000, 0)

      {0.f, demoEndDate, 0, 0},
    };

    // Calcul des correspondances
    // FIXME: ne pas refaire ce calcul à chaque appel de fonction en release
    int numberOfItems = sizeof(tc) / sizeof(timeControl);
    for(int i = 1; i < numberOfItems; i++)
    {
      int diff = tc[i].storyTime - tc[i - 1].storyTime;
      const float shift = (tc[i - 1].speed != 0 ? diff / tc[i - 1].speed : 0);
      tc[i].youtubeTime = msys_ifloorf(tc[i - 1].youtubeTime + shift);
      if (tc[i - 1].speed == 0.f) diff = abs(diff);
      tc[i].cameraTime = tc[i - 1].cameraTime + (tc[i - 1].speed != 0 ? diff : 0);
    }

    // FIXME: en release, on peut mettre k en statique pour accélérer
    int k = 0;
    while (youtubeTime >= tc[k].youtubeTime) ++k;
    const int diff = youtubeTime - tc[k - 1].youtubeTime;
    doubleDate times;
    times.story = msys_ifloorf(tc[k - 1].storyTime + diff * tc[k - 1].speed);
    times.camera = msys_ifloorf(tc[k - 1].cameraTime + diff * tc[k - 1].speed);
    return times;
  }
}
