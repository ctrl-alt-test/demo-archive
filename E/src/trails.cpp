//--------------------------------------------------------------------------//
// iq . 2003/2008 . code for 64 kb intros by RGBA                           //
//--------------------------------------------------------------------------//


#include "intro.h"

#include "sys/msys.h"
#include "sys/msys_debug.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include "fbos.hh"
#include "shaderprogram.hh"
#include "textures.hh"
#include "vbos.hh"

//--------------------------------------------------------

namespace Trails
{
  typedef struct
  {
    quad vertices;
    float intensity;
    int group;
    date birth;
  } trailDesc;
  Array<trailDesc> * trailElements = NULL;

  void init()
  {
    trailElements = new Array<trailDesc>(32000);

    /*
    for (unsigned int i = 0; i < 6000; ++i)
    {
      const int note = msys_rand() % 127;
      const int volume = msys_rand() % 127;
      const int channel = msys_rand() % 16;
      const int t = (int)(msys_frand() * 200000.f);
      intro_addNote(t, note, volume, channel);
    }
    */
  }

  static void addElement(int t,
			 float x, float y, float width, float height,
			 float intensity, int group)
  {
    trailDesc element;
    element.vertices.x1 = x;
    element.vertices.y1 = y;
    element.vertices.x2 = x;
    element.vertices.y2 = y + height;
    element.vertices.x3 = x + width;
    element.vertices.y3 = y + height;
    element.vertices.x4 = x + width;
    element.vertices.y4 = y;

    assert(intensity != 0);
    element.intensity = intensity;

    element.group = group;

    element.birth = t;

    trailElements->add(element);
  }

  void addNoteImage(int t, int note, int volume, int channel)
  {
    const float x = note / 128.f;
    const float y = channel / 17.f;;
    const float width = 0.1f;//1.f / 128.f;
    const float height = 0.1f;//1.f / 17.f;
    const float intensity = 0.5f + 0.5f * volume / 127.f;
    const int group = 1;
    addElement(t, x, y, width, height, intensity, group);
  }

  void addFluteImage(int t, int note, int volume)
  {
    const float level = (note - 33) / (float)(89 - 33);
    const float x = 0.16f * ((9.f * level + msys_sfrand()) / 10.f) + 0.16f * (msys_rand() % 6);
    const float y = 0.16f * ((9.f * level + msys_sfrand()) / 10.f) + 0.16f * (msys_rand() % 6);
    const float width = 3.f / 50.f;
    const float height = 3.f / 60.f;
    const float intensity = 0.5f + 0.5f * volume / 127.f;
    const int group = 2;
    addElement(t, x, y, width, height, intensity, group);
  }

  void addRolandImage(int t, int note, int volume)
  {
    const float level = (float)(note - 76) / (105 - 76);
    const float x = (float)(msys_rand() % 60) / 60.f;
    const float y = 0.25f * (0.5f * ((0.2f + 0.6f * level) + (float)(msys_rand() % 50) / 50.f)) + 0.5f * (float)(msys_rand() % 2);
    const float width = 12.f / 50.f;
    const float height = 12.f / 60.f;
    const float intensity = 0.7f + 0.3f * volume / 84.f;
    const int group = 1;
    addElement(t + 150, x, y, width, height, intensity, group);
  }

  void addPianoImage(int t, int note, int volume)
  {
    const float level = (float)(note - 76) / (105 - 76);
    const float x = (float)(msys_rand() % 60) / 60.f;
    const float y = 0.25f * (0.5f * ((0.2f + 0.6f * level) + (float)(msys_rand() % 50) / 50.f)) + 0.5f * (float)(msys_rand() % 2);
    const float width = 8.f / 50.f;
    const float height = 8.f / 60.f;
    const float intensity = 0.7f + 0.3f * volume / 84.f;
    const int group = 2;
    addElement(t + 150, x, y, width, height, intensity, group);
  }

  void addStringsImage(int t, int note, int volume)
  {
    const float level = (float)(note - 72) / (108 - 72);
    const float x = (float)(msys_rand() % 60) / 60.f;
    const float y = 0.25f * (0.2f * (4.f * (0.2f + 0.6f * level) + (float)(msys_rand() % 50) / 50.f)) + 0.25f * (float)(msys_rand() % 4);
    const float width = 1.f / 50.f;
    const float height = 1.f / 60.f;
    const float intensity = 0.7f + 0.3f * volume / 84.f;
    const int group = 2;
    addElement(t, x, y, width, height, intensity, group);
  }

  void addTubularImage(int t, int note, int volume)
  {
    const float x = (float)(msys_rand() % 50) / 50.f - 0.3f;
    const float y = (float)(msys_rand() % 60) / 60.f;
    const float width = 0.2f + 0.4f * msys_frand();
    const float height = 5.f / 60.f;
    const int group = (msys_rand() % 2);
    const float intensity = 1.f;//0.5f + 0.5f * volume / 127.f;
    addElement(t, x, y, width, height, intensity, group);
  }

  void addBoomBoomImage(int t, int note, int volume)
  {
    const float x = 0;
    const float y = (float)(msys_rand() % 60) / 60.f;
    const float width = 1.f;
    const float height = 1.5f / 60.f;
    const int group = (msys_rand() % 2);
    const float intensity = 0.6f + 0.4f * volume / 127.f;
    addElement(t, x, y, width, height, intensity, group);
  }

  void addNote(int t, int note, int volume, int channel)
  {
    // Premiers sons
    if (t > 28000)
    {
      // Flûte (347 notes -> 12492; 33 à 117)
      if (channel == 7)
      {
	DBG("Flute : t = %6d, %3d, %3d", t, note, volume);
	for (unsigned int i = 0; i < 36; ++i)
	{
	  addFluteImage(t, note, volume);
	}
	return;
      }

      // Roland (207 notes -> 414; 76 à 105)
      if (t > 54000 && (t < 68469 || t >= 96178 && t < 109500) && channel == 8)
      {
	DBG("Roland : t = %6d, %3d, %3d", t, note, volume);
	for (unsigned int i = 0; i < 36; ++i)
	{
	  addRolandImage(t, note, volume);
	}
	return;
      }

      // Piano (363 notes ; 53 à 96)
      if (t > 68000 && (t < 109500 || t >= 150600) && channel == 11)
      {
	DBG("Piano : t = %6d, %3d, %3d", t, note, volume);
	addPianoImage(t, note, volume);
	return;
      }

      // Strings (944 notes -> 9440; 72 à 108)
      // channel 4 à partir de ~136000, puis 14 à partir de 178019
      if ((t >= 136000 && t < 178019 && channel == 6) ||
	  (t >= 178019 && t < 190000 && channel == 14))
      {
	DBG("Strings : t = %6d, %3d, %3d", t, note, volume);
	for (unsigned int i = 0; i < 10; ++i)
	{
	  addStringsImage(t, note, volume);
	}
	return;
      }

      // Tubulars (85 notes -> 340 éléments; 65 à 93)
      if (t >= 112760 && t < 135000 && channel == 2)
      {
	DBG("Tubular : t = %6d, %3d, %3d", t, note, volume);
	for (unsigned int i = 0; i < 4; ++i)
	{
	  addTubularImage(t, note, volume);
	}
	return;
      }

      // Boum boum (112 notes -> 1120 elements)
      if (t >= 150800 && (/* channel == 0 || */ channel == 3 || channel == 4))
      {
	DBG("Boul boum : t = %6d, %3d, %3d", t, note, volume);
	for (unsigned int i = 0; i < 10; ++i)
	{
	  addBoomBoomImage(t, note, volume);
	}
	return;
      }
    }

    // Le reste
    /*
      const float x = note / 128.f;
      const float y = channel / 17.f;;
      const float width = 1.f / 128.f;
      const float height = 1.f / 17.f;

      trailDesc element;
      element.vertices.x1 = x;
      element.vertices.y1 = y;
      element.vertices.x2 = x;
      element.vertices.y2 = y + height;
      element.vertices.x3 = x + width;
      element.vertices.y3 = y + height;
      element.vertices.x4 = x + width;
      element.vertices.y4 = y;

      element.intensity = 0.5f + 0.5f * volume / 127.f;
      element.group = 2 * (channel % 2);

      element.birth = t;

      trailElements->add(element);
    */
  }

  void renderElement(const trailDesc & element, float level)
  {
    glColor4f(1.f * (element.group == 0),
		      1.f * (element.group == 1),
	      1.f * (element.group == 2),
	      element.intensity * msys_powf(1.f - level, 1.5f));
    glVertexPointer(2, GL_FLOAT, 0, &(element.vertices.x1));
    glDrawArrays(GL_QUADS, 0, 4);
  }

  void render(date renderDate)
  {
    assert(trailElements != NULL);

    Shader::Program::unUse();
    Texture::Unit::unUse();
    VBO::Element::unUse();
    FBO::list[FBO::trails].use();

    GLenum buffers[] = {
      GL_COLOR_ATTACHMENT0_EXT,
      GL_COLOR_ATTACHMENT1_EXT
    };

    oglDrawBuffers(1, buffers);

#if DEBUG_TRAILS
    glClearColor(0.1f, 0.1f, 0.1f, 1.f);
#else
    glClearColor(0, 0, 0, 1.f);
#endif // DEBUG_TRAILS

    glClear(GL_COLOR_BUFFER_BIT);

    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glDisable( GL_LIGHTING );
    glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0., 1., 0., 1.);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    /*
    {
      int i = 0;
      while (i < trailElements->size)
	if ((*trailElements)[i].birth + 500 < renderDate)
	{
	  trailElements->remove(i);
	}
	else
	{
	  ++i;
	}
    }
    */

#if DEBUG_TRAILS

    // Fond pour debugger
    glColor4f(0.4f, 0.4f, 0.4f, 0.4f);

    glBegin(GL_QUADS);
    glVertex2f(0.05f, 0.05f);
    glVertex2f(0.05f, 0.95f);
    glVertex2f(0.95f, 0.95f);
    glVertex2f(0.95f, 0.05f);
    glEnd();

#endif // DEBUG_TRAILS

    glEnableClientState(GL_VERTEX_ARRAY);

    Array<trailDesc> & list = (*trailElements);
    for (int i = 0; i < list.size; ++i)
    {
      const trailDesc & element = list[i];
      if (element.birth + 500 >= renderDate &&
	  element.birth <= renderDate)
      {
	renderElement(element, 0.002f * (renderDate - element.birth));
      }
    }

    glDisableClientState(GL_VERTEX_ARRAY);
  }
}
