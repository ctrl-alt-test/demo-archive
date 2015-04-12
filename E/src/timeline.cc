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
  float fadeIn(date begin, date end, date d)
  {
    assert(begin < end);
    if (d < begin || d > end) return 1.f;
    return smoothStepI(begin, end, d);
  }

  float fadeOut(date begin, date end, date d)
  {
    assert(begin < end);
    if (d < begin || d > end) return 1.f;
    return 1.f - smoothStepI(begin, end, d);
  }

  // Gestion des fondus
  void doFade(date renderdate)
  {
    float fade_coef = 1.f;

    // Lever de rideau au départ
    fade_coef *= fadeIn(    0, 2000, renderdate);
    fade_coef *= fadeOut(2000, 3500, renderdate);
    fade_coef *= fadeIn( 3500, 5500, renderdate);
    fade_coef *= fadeOut(5500, 7000, renderdate);

    if (renderdate < 7000)
      fade_coef *= 0.3f;

    // Titre
    fade_coef *= fadeIn(  7000, 12000, renderdate);
    fade_coef *= fadeOut(12500, 14000, renderdate);

    // Premier mouvement
    fade_coef *= fadeIn( 14000, 14200, renderdate);

    // Fondu final
    fade_coef *= fadeOut(worldEndDate - 1000, worldEndDate, renderdate);

    float fade_luminance = 0;

    Shader::state.fade = fade_coef;
    Shader::state.fadeLuminance = fade_luminance;
  }

  typedef struct
  {
    date start;
    date duration;
    float x; // > 0 : depuis la gauche ; < 0 : depuis la droite ; 0 : centré
    float y; // > 0 : depuis le haut ;   < 0 : depuis le bas ;    0 : centré
    float size;
    float alpha;
    char * str;
  } textMessage;

  //
  //  /!\ Tableau de greetings /!\
  //
  //  Pour pouvoir utiliser _TV(), il est dans une fonction.
  //  Du coup, il faut penser à mettre à jour le nombre suivant
  //
  static const unsigned int numberOfgreetings = 13; //sizeof(greetings) / sizeof(textMessage);
  static const textMessage greetings(unsigned int i, int textHeight)
  {
    const date presents = _TV(8000); // Synchro avec le 3e lever de rideau
    const date music = _TV(14000); // Synchro avec le début du 1er mouvement
    const date party = _TV(21000);
    const date title = _TV(29000);
    const date credits = _TV(137500);
    const date final = _TV(193000);

    textMessage greetings[numberOfgreetings] = {
      { presents,             _TV(5000), _TV( 0.00f), _TV( 0.41f), _TV(1.00f), _TV(0.65f), "Ctrl-Alt-Test" },
      { presents,             _TV(5000), _TV( 0.00f), _TV( 0.53f), _TV(0.60f), _TV(0.55f), "presents" },

      { music,                _TV(6000), _TV(-0.06f), _TV( 0.09f), _TV(0.60f), _TV(0.50f), "on a music by" },
      { music,                _TV(6000), _TV(-0.06f), _TV( 0.13f), _TV(1.00f), _TV(0.95f), "Cyborg Jeff" },

      { party,                _TV(6000), _TV( 0.06f), _TV(-0.17f), _TV(0.60f), _TV(0.50f), "for" },
      { party,                _TV(6000), _TV( 0.06f), _TV(-0.10f), _TV(1.00f), _TV(0.95f), "the Ultimate Meeting 2010" },

      { title,                _TV(5000), _TV(-0.06f), _TV(-0.10f), _TV(1.20f), _TV(0.70f), "DEPARTURE" },

      { credits,              _TV(5000), _TV(-0.06f), _TV(-0.39f), _TV(0.60f), _TV(0.50f), "Music:" },
      { credits,              _TV(5000), _TV(-0.06f), _TV(-0.32f), _TV(1.00f), _TV(0.80f), "Cyborg Jeff" },
      { credits + _TV(1000),  _TV(4000), _TV(-0.06f), _TV(-0.17f), _TV(0.60f), _TV(0.50f), "Code:" },
      { credits + _TV(1000),  _TV(4000), _TV(-0.06f), _TV(-0.10f), _TV(1.00f), _TV(0.80f), "Zavie, LLB" },

      { final,                _TV(6000), _TV( 0.00f), _TV( 0.41f), _TV(0.60f), _TV(0.20f), "Ctrl-Alt-Test" },
      { final + _TV(2000),    _TV(4000), _TV( 0.00f), _TV( 0.53f), _TV(0.60f), _TV(0.20f), "2010" },
    };
    return greetings[i];
  }

  static void printGreeting(const textMessage & greeting, const Font * font,
			    float textHeight, float maxAlpha,
			    int xres, int yres, date displayDate)
  {
    float x = xres * greeting.x;
    float y = yres * greeting.y;
    float height = textHeight * greeting.size;

    if (0 == x)
    {
      x += 0.5f * (xres - font->width(greeting.str, height));
    }
    if (0 == y)
    {
      y += 0.5f * (yres - textHeight);
    }
    if (x < 0)
    {
      x += xres - font->width(greeting.str, height);
    }
    if (y < 0)
    {
      y += yres - height;
    }

    const date end = greeting.start + greeting.duration;
    const float textStep = interpolate((float)displayDate, (float)greeting.start, (float)end);
    Shader::setUniform1f(Shader::text, Shader::textStep, textStep);

    float alpha = (maxAlpha *
		   fadeIn(greeting.start, greeting.start + 1000, displayDate) *
		   fadeOut((greeting.start + 3 * end) / 4, end, displayDate));
    glColor4f(1.f, 1.f, 1.f, alpha);
    font->print(greeting.str, height, x, y);
  }

  static void printGreeting(const textMessage & greeting, const IntroObject & intro)
  {
    printGreeting(greeting, intro.font,
		  (float)intro.textHeight, greeting.alpha,
		  intro.xres, intro.yres, intro.now.youtube);

    // Echo
    if (intro.now.youtube >= 14000 && intro.now.youtube < 190000)
    {
      printGreeting(greeting, intro.font,
		    1.7f * (float)intro.textHeight, _TV(0.06f),
		    intro.xres, intro.yres, intro.now.youtube);
    }
  }

  void printText(const IntroObject & intro)
  {
    for (unsigned int i = 0; i < numberOfgreetings; ++i)
    {
      const textMessage & greeting = greetings(i, intro.textHeight);
      if (intro.now.youtube >= greeting.start &&
	  intro.now.youtube < (greeting.start + greeting.duration))
      {
	printGreeting(greeting, intro);
      }
    }
  }

  //
  // FIXME : qu'est ce que cette fonction fait ici ?
  //
  Transformation getCamera(const IntroObject & intro)
  {
    const date animDate = intro.now.camera;
    Anim::Anim * cameraAnim = intro.cameraAnim;

    return cameraAnim->getTransformation(animDate, 0);
  }

  Transformation getOldCamera(const IntroObject & intro)
  {
    const date animDate = intro.now.camera;
    const date oldAnimDate = intro.now.camera - 10;
    Anim::Anim * cameraAnim = intro.cameraAnim;

    Transformation p1 = cameraAnim->getTransformation(animDate, 0);
    Transformation p0 = cameraAnim->getTransformation(oldAnimDate, 0);
    vector3f dp = p1.v - p0.v;
    const float d = norm(dp);
    return (d < 5.f ? p0 : p1);
  }

  // Contrôle de la vitesse de lecture

  struct timeControl
  {
    float speed;
    date storyTime;
    date cameraTime;
    date youtubeTime;
  };

  Clock computeClock(date youtubeTime)
  {
    /*
    timeControl tc[] = {
      {  1.0f,                             0, 0, 0},
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

    Clock targetTime;
    targetTime.story = msys_ifloorf(tc[k - 1].storyTime + diff * tc[k - 1].speed);
    targetTime.camera = msys_ifloorf(tc[k - 1].cameraTime + diff * tc[k - 1].speed);
    */
    Clock targetTime;
    targetTime.story = youtubeTime;
    targetTime.camera = youtubeTime;

    return targetTime;
  }
}
