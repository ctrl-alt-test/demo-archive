#include "timeline.hh"

#include "font.hh"
#include "interpolation.hh"
#include "intro.hh"
#include "shaderid.hh"
#include "textures.hh"
#include "tweakval.h"

#define TEXT_FADE_IN_DURATION 800
#define TEXT_FADE_OUT_DURATION 1000

#define DEBUG_FADE 0

namespace Timeline
{

#if DEBUG

  // recherche (approximativement) le youtubeTime à partir du storyTime
  // sert pour le intro.startTime de debug
  int findYoutubeTime(date storyTime)
  {
    Clock clock;
    for (int i = 0; ; i += 500)
    {
      clock = computeClock(i);
      if (storyTime < clock.story)
	return i;
    }
  }

#endif

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
    if ((int)renderdate < _TV(1000)) fade_coef = 0.f;
    fade_coef *= fadeIn(_TV(1000), _TV(2500), renderdate);

    // Avant le plan de la boite à musique
    fade_coef *= fadeOut(_TV(31000), _TV(32000), renderdate);
    fade_coef *= fadeIn(_TV(32000), _TV(32500), renderdate);

    // Ouverture de la porte
    fade_coef *= fadeOut(_TV(217000), _TV(218000), renderdate);
    if ((int) renderdate >= _TV(218000) && (int) renderdate <= _TV(220000))
      fade_coef = 0;
    fade_coef *= fadeIn(_TV(220000), _TV(221500), renderdate);

    // Rideau final
    fade_coef *= fadeOut(_TV(226000), _TV(227000), renderdate);
    if ((int) renderdate >= _TV(227000))
      fade_coef = 0;

    float fade_luminance = 0;

    Shader::state.fade = fade_coef;
    Shader::state.fadeLuminance = fade_luminance;
  }

  struct textMessage
  {
    date start;
    date duration;
    float x; // > 0 : depuis la gauche ; < 0 : depuis la droite ; 0 : centré
    float y; // > 0 : depuis le bas ;    < 0 : depuis le haut ;   0 : centré
    float size;
    float alpha;
    char * str;
  };

  //
  //  /!\ Tableau de greetings /!\
  //
  //  Pour pouvoir utiliser _TV(), il est dans une fonction.
  //  Du coup, il faut penser à mettre à jour le nombre suivant
  //
  static const unsigned int numberOfGreetings = 9; //sizeof(greetings) / sizeof(textMessage);
  static const textMessage greetings(unsigned int i, int textHeight)
  {
    textMessage greetings[numberOfGreetings] = {
      // Tests de placement :
//       {  1000, 5000,  0.0001f, -0.0001f, 1.00f, 0.80f, "b haut gauche Rod" },
//       {  1200, 5000, -0.0001f, -0.0001f, 1.00f, 0.80f, "p haut droite Rod" },
//       {  1400, 5000, -0.0001f,  0.0001f, 1.00f, 0.80f, "b bas droite Roq" },
//       {  1600, 5000,  0.0001f,  0.0001f, 1.00f, 0.80f, "p bas gauche Roq" },
//       {  1800, 5000,  0.f,      0.f,     1.00f, 0.80f, "b centre Roq" },

      // Tests de taille de texte :
//       {  2000, 5000,  0.0001f,  0.10f, 0.50f, 0.80f, "0.5" },
//       {  2000, 5000,  0.0001f,  0.15f, 1.00f, 0.80f, "1.0" },
//       {  2000, 5000,  0.0001f,  0.25f, 2.00f, 0.80f, "2.0" },

      { _TV(2250), _TV(3500), _TV( 0.00f), _TV( 0.47f), _TV(1.00f), _TV(0.65f), "Ctrl-Alt-Test" },

      { _TV(22500), _TV(3500), _TV( 0.05f), _TV(-0.10f), _TV(0.40f), _TV(0.50f), "ORIGINAL SCORE" },
      { _TV(23500), _TV(2500), _TV( 0.05f), _TV(-0.15f), _TV(0.75f), _TV(0.65f), "Pierre Martin" },

      { _TV(14500), _TV(7000), _TV( 0.05f), _TV( 0.22f), _TV(0.40f), _TV(0.50f), "PROGRAMMING AND DIRECTION" },
      { _TV(15500), _TV(2500), _TV( 0.05f), _TV( 0.14f), _TV(0.75f), _TV(0.65f), "Julien Guertault" },
      { _TV(18500), _TV(2500), _TV( 0.05f), _TV( 0.14f), _TV(0.75f), _TV(0.65f), "Laurent Le Brun" },

      { _TV(57500), _TV(3500), _TV( 0.00f), _TV( 0.55f), _TV(1.00f), _TV(0.80f), "Felix's workshop" },

      { _TV(227000), _TV(4000), _TV( 0.00f), _TV( 0.50f), _TV(0.70f), _TV(0.65f), "Ctrl-Alt-Test" },
      { _TV(228000), _TV(3000), _TV( 0.00f), _TV( 0.44f), _TV(0.50f), _TV(0.65f), "2012" },
    };
    return greetings[i];
  }

  static void printGreeting(const textMessage & greeting, const Font::Font * font,
			    float textHeight, float maxAlpha, date displayDate)
  {
    float x = intro.xres * greeting.x;
    float y = intro.yresVP * greeting.y;
    float height = textHeight * greeting.size;

    if (0 == x)
    {
      x += 0.5f * (intro.xres - font->width(greeting.str, height));
    }
    if (0 == y)
    {
      y += 0.5f * (intro.yresVP - textHeight);
    }
    if (x < 0)
    {
      x += intro.xres - font->width(greeting.str, height);
    }
    if (y < 0)
    {
      y += intro.yresVP - height;
    }

    const date end = greeting.start + greeting.duration;
    const float textStep = interpolate((float)displayDate, (float)greeting.start, (float)end);
    Shader::setUniform1f(Shader::text, Shader::textStep, textStep);

    float alpha = (maxAlpha *
		   fadeIn(greeting.start, greeting.start + 1000, displayDate) *
		   fadeOut((greeting.start + 3 * end) / 4, end, displayDate));

    glColor4f(_TV(0.f), _TV(0.f), _TV(0.f), alpha * _TV(0.75f));
    font->print(greeting.str, height * _TV(1.f), x + _TV(2.f), y + _TV(-2.f));
    glColor4f(1.f, 1.f, 1.f, alpha);
    font->print(greeting.str, height, x, y);
  }

  static void printGreeting(const textMessage & greeting)
  {
    printGreeting(greeting, intro.font,
		  (float)intro.textHeight, greeting.alpha,
		  intro.now.youtube);

    // Echo
    /*
    if (intro.now.youtube >= 14000 && intro.now.youtube < 190000)
    {
      printGreeting(greeting, intro.font,
		    1.7f * (float)intro.textHeight, _TV(0.06f),
		    intro.now.youtube);
    }
    */
  }

  void printGreetings()
  {
    for (unsigned int i = 0; i < numberOfGreetings; ++i)
    {
      const textMessage & greeting = greetings(i, intro.textHeight);
      if (intro.now.youtube >= greeting.start &&
	  intro.now.youtube < (greeting.start + greeting.duration))
      {
	printGreeting(greeting);
      }
    }
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
