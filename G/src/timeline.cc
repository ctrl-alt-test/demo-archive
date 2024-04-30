#include "timeline.hh"

#include "demo.hh"
#include "font.hh"
#include "interpolation.hh"
#include "intro.hh"
#include "shaderid.hh"
#include "textures.hh"
#include "tweakval.hh"

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
      clock = Demo::computeClock(i);
      if (storyTime <= clock.story)
	return i;
    }
  }

#endif

  float fadeIn(date begin, date end, date d)
  {
    assert(begin < end);
    if (d < begin || d > end) return 1.f;
    return smoothStep(begin, end, d);
  }

  float fadeOut(date begin, date end, date d)
  {
    assert(begin < end);
    if (d < begin || d > end) return 1.f;
    return 1.f - smoothStep(begin, end, d);
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
  static const unsigned int numberOfGreetings = 3; //sizeof(greetings) / sizeof(textMessage);
  static const textMessage greetings(unsigned int i, int textHeight)
  {
    textMessage greetings[numberOfGreetings] = {
      { _TV(298200), _TV(2500), _TV( 0.00f), _TV( 0.60f), _TV(0.70f), _TV(0.65f), "Ctrl-Alt-Test" },
      { _TV(298700), _TV(2500), _TV( 0.00f), _TV( 0.485f), _TV(0.75f), _TV(0.65f), "hAND" },
      { _TV(299200), _TV(2500), _TV( 0.00f), _TV( 0.40f), _TV(0.50f), _TV(0.65f), "2014" },
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
    Shader::uniforms[Shader::textStep].set(textStep);
    Shader::list[Shader::text].use();

    float alpha = (maxAlpha *
		   fadeIn(greeting.start, greeting.start + 1000, displayDate) *
		   fadeOut((greeting.start + 3 * end) / 4, end, displayDate));

    glColor4f(_TV(0.f), _TV(0.f), _TV(0.f), alpha * _TV(0.5f));
    font->print(greeting.str, height * _TV(1.f), x + _TV(3.f), y + _TV(-3.f));
    glColor4f(1.f, 1.f, 1.f, alpha);
    font->print(greeting.str, height, x, y);
  }

  static void printGreeting(const textMessage & greeting)
  {
    if (intro.showCredits)
    {
      printGreeting(greeting, intro.font,
		    (float)intro.textHeight, greeting.alpha,
		    intro.now.youtube);
    }

    // Echo
    /*
    if (intro.now.youtube >= 14000 && intro.now.youtube < 190000)
    {
      printGreeting(greeting, intro.font,
		    1.7f * (float)intro.textHeight, 0.06f,
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
}
