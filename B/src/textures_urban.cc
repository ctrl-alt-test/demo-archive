#include "textures.hh"
#include "textures_urban.hh"

#include "sys/msys.h"

#include "array.hh"
#include "textureid.hh"
#include "texgen/texture.hh"

namespace Texture
{
  Texture::Channel *buffers;

  static void paste(Texture::Channel &out, const Texture::Channel &chunk,
                    const Texture::Channel &mask, float x, float y)
  {
    Texture::Channel tmp = chunk;
    tmp.Translate(x, y);
    Texture::Channel tmp2 = mask;
    tmp2.Translate(x, y);
    out.Mask(tmp2, tmp);
  }

  /*
  static void buildGutter()
  {
    Texture::Channel & t = buffers[gutter];
    Texture::Channel & alpha = buffers[gutterAlpha];
    Texture::Channel t2;

    // Gouttière
    t.Fade(90.f);
    t.Scale(-1.f, 20.f);
    t.Abs();
    alpha = t;
    alpha.ClampAndScale(0.21f, 0.2f);
    t.ClampAndScale(0.45f, 0.f);
    t.Sin();
    t.Scale(0.f, 0.6f);

    t2.Fade(0.f);
    t2.Scale(0.f, 100.f);
    t2.Mod();
    t2.Pow(0.1f);
    t *= t2;
  }
  */

  static void buildWalls()
  {
    int size = 512;
    float nlines = size / 16.f;
    float ncols = 12.f;

    Texture::Channel t;
    t.Fade(0.f);
    t.Scale(-0.1f, nlines*2.f - 0.1f);
    t.Mod();
    t.Pow(0.25f);

    Texture::Channel t2;
    t2.Fade(90.f);
    t2 *= ncols * 2.f;
    t2.Mod();
    t2.Pow(0.2f);

    t.MaximizeTo(t2);

    Texture::Channel rnd;
    rnd.Random();
    rnd.GaussianBlur();
    rnd.Scale(-0.1f, 0.1f);
    t += rnd;
    t.ClampAndScale(0.5f, 0.8f);

    //t.ClampAndScale(0.6, 0.7);
    rnd.Cells(5);
    rnd.Scale(0.6f, 1.f);
    t *= rnd;
    rnd.Cells(10);
    rnd.Scale(0.5f, 1.f);
    t *= rnd;

    rnd.Fade(90.f);
    rnd *= 17.f;
    rnd.Sin();
    rnd.Scale(0.7f, 1.f);
    t *= rnd;

    Texture::Channel line;
    line.Fade(0.f);
    line.ClampAndScale(1.f/nlines, 1.f/nlines+0.01f);

    for (int i = 0; i < nlines; i++) {
      t2 = t;
      float shift = msys_rand() % 12 / 12.f;
      t2.Translate(shift + (i%2) / (2.f*ncols), 0.f);
      t.Mask(line, t2);
      t.Translate(0.f, 1.f / nlines);
    }

    t2 = t;
    rnd.Random();
    rnd.GaussianBlur();
    rnd.Scale(-0.1f, 0.1f);
    t += rnd;
    t.Pow(0.7f);

    t2.ClampAndScale(0.25f, 0.1f);

    Texture::Channel grey;
    grey.Random();
    grey.Blur();

    // Wall 1

    Texture::Channel & r1 = buffers[wall1r];
    Texture::Channel & g1 = buffers[wall1g];
    Texture::Channel & b1 = buffers[wall1b];
    r1 = t; r1.Scale(0.f, 0.87f);
    g1 = t; g1.Scale(0.f, 0.4f);
    b1 = t; b1.Scale(0.f, 0.2f);

    grey.Scale(0.3f, 0.7f);
    r1.Mask(t2, grey);
    g1.Mask(t2, grey);
    b1.Mask(t2, grey);

    // Wall 2

    Texture::Channel & r2 = buffers[wall2r];
    Texture::Channel & g2 = buffers[wall2g];
    Texture::Channel & b2 = buffers[wall2b];
    r2 = t; r2.Scale(0.6f, 1.f);
    g2 = t; g2.Scale(0.1f, 0.7f);
    b2 = t; b2.Scale(-0.1f, 0.4f);

    grey.Scale(0.1f, 0.4f);
    r2.Mask(t2, grey);
    g2.Mask(t2, grey);
    b2.Mask(t2, grey);

    Texture::Channel & height = buffers[wallHeight];
    height = t2;
    height.Scale(1.f, 0.8f);

    Texture::Channel & spec = buffers[wallSpecular];
    spec = height;
    spec.Scale(0.2f, 0.5f);
  }

  static void buildRoofs()
  {
    Channel roofTop;
    roofTop.Square();
    roofTop.ClampAndScale(0.75f, 0.755f);
    roofTop.Translate(0.2f, 0.15f);

    Channel r = buffers[wall2r];
    Channel g = buffers[wall2g];
    Channel b = buffers[wall2b];

    Channel glass; glass.Flat(0.75f);

    r.Mask(roofTop, glass);
    g.Mask(roofTop, glass);
    b.Mask(roofTop, glass);

    buffers[roof2r] = r;
    buffers[roof2g] = g;
    buffers[roof2b] = b;


    Channel wallOrRoof;
    wallOrRoof.Square();
    wallOrRoof.ClampAndScale(0.105f, 0.1f);

    Channel gravel;
    gravel.Random();
    r = gravel; r.Scale(0.76f, 0.82f);
    g = gravel; g.Scale(0.72f, 0.78f);
    b = gravel; b.Scale(0.61f, 0.66f);

    r.Mask(wallOrRoof, buffers[wall3r]);
    g.Mask(wallOrRoof, buffers[wall3g]);
    b.Mask(wallOrRoof, buffers[wall3b]);

    buffers[roof3r] = r;
    buffers[roof3g] = g;
    buffers[roof3b] = b;
  }

  static void buildWindows()
  {
    Texture::Channel t;
    Texture::Channel & mask = buffers[windowAlpha];
    mask.Square();
    mask.ClampAndScale(0.8f, 0.85f);
    t = mask;

    Texture::Channel t2;
    t2.Square();
    t2.ClampAndScale(0.85f, 0.851f);

    t -= t2;
    t *= 2.f;
    t.Sin();

    Texture::Channel t3;
    t3.Conic();

    Texture::Channel rnd;
    rnd.Random();
    rnd.GaussianBlur();
    rnd.Scale(-0.1f, 0.1f);
    t3 += rnd;
    t3.Translate(0.f, -0.1f);
    t3.HorizontalMotionBlur(0.05f);
    t3.ClampAndScale(0.8f, 0.9f);
    t3.Scale(0.f, 0.3f);
    t3.Line(0.5f, 0.4f, 0.5f, 0.6f);
    t3.Line(0.4f, 0.5f, 0.6f, 0.5f);
    t3.GaussianBlur();

    t3 *= t2;

    Texture::Channel t4;
    t4.Fade(0);
    t4.ClampAndScale(0.6f, 0.4f);


    t4 += rnd;
    t4 *= t2;
    t4 -= t3;
    t4.Clamp(0.f, 1.f);

    rnd.Scale(0.6f, 1.f);
    t *= rnd;

    Texture::Channel & r = buffers[window1r];
    Texture::Channel & g = buffers[window1g];
    Texture::Channel & b = buffers[window1b];

    r = t; t4.Scale(0.f, 0.3f); r += t4;
    g = t; t4.Scale(0.f, 0.7f); g += t4;
    b = t; b.Scale(0.f, 0.7f);
    t4.Scale(0.f, 0.8f); b += t4;

    Texture::Channel & height = buffers[windowHeight];
    height = t2;
    height.Scale(0.5f, 0.f);
    height += t;

    Texture::Channel & spec = buffers[windowSpecular];
    spec = height;
    spec.Scale(0.9f, 0.5f);

    height.GaussianBlur();
  }

  static void buildWalls2()
  {
    Texture::Channel rnd;
    rnd.Random();
    rnd.GaussianBlur();
    rnd.Emboss();
    rnd.Sharpen();

    rnd.Scale(0.6f, 1.f);
    buffers[wall3r] = rnd;
    buffers[wall3g] = rnd;
    buffers[wall3b] = rnd;
    rnd.Scale(0.f, 0.1f);
    buffers[wall3Height] = rnd;
    buffers[wall3Specular] = rnd;
  }

  // Groupe les fenêtres par 9
  void buildWindowsConfigurations()
  {
    // test - comment optimiser ?
    for (int i = -1; i <= 1; i++)
      for (int j = -1; j <= 1; j++)
      {
        float x = (float)i / 3.f; if (i == 0) x = 0.05f;
        float y = (float)j / 3.f;
        paste(buffers[windowsConf1r], buffers[window1r], buffers[windowAlpha], x, y);
        paste(buffers[windowsConf1g], buffers[window1g], buffers[windowAlpha], x, y);
        paste(buffers[windowsConf1b], buffers[window1b], buffers[windowAlpha], x, y);
        paste(buffers[windowsConfHeight], buffers[windowHeight], buffers[windowAlpha], x, y);
        paste(buffers[windowsConfSpecular], buffers[windowSpecular], buffers[windowAlpha], x, y);
        paste(buffers[windowsConfAlpha], buffers[windowAlpha], buffers[windowAlpha], x, y);
      }
  }

  static void pacman(Texture::Channel & t)
  {
    Texture::Channel t2;
    Texture::Channel t3;
    Texture::Channel u;
    Texture::Channel v;
    Texture::Channel eye;
    float n = 0.01f;
    t2.Diamond();
    t2.ClampAndScale(0.8f-n, 0.8f);
    t2.HorizontalMotionBlur(0.002f);
    t2.Translate(-0.22f, 0);
    t2.Scale(1, 0);
    t.Conic();
    t.ClampAndScale(0.65f-n, 0.65f);
    t *= t2;
    eye.Square();
    eye.ClampAndScale(0.96f-n, 0.96f);
    t3 = eye;
    eye.Translate(0, 0.09f);
    eye.Scale(1, 0);
    t3.Translate(-0.16f, 0);
    t *= eye;
    t += t3;
    t3.Translate(-0.1f, 0);
    t += t3;
  }

  static void paintLeak(Texture::Channel & t)
  {
    Texture::Channel fade;
    fade = t;
    fade.VerticalMotionBlur(0.03f);
    fade.HorizontalMotionBlur(0.03f);
    fade.VerticalMotionBlur(0.06f);
    fade.Pow(0.7f);
    fade.Translate(0.f, -0.02f);

    Texture::Channel t2;
    t2 = t;

    Texture::Channel rnd;
    rnd.Random();
    rnd.GaussianBlur();
    rnd.AddCuts(1.4f, 0.06f);
    rnd.GaussianBlur();
    rnd.Scale(0.f, 0.8f);
    fade += rnd;

    fade.ClampAndScale(1.f, 1.2f);
    t += fade;
  }

  static void buildPacmanWall()
  {
    Texture::Channel p;
    Texture::Channel rnd;
    pacman(p);

    rnd.Random();
    rnd.GaussianBlur();
    rnd.ClampAndScale(0.25f, 0.4f);

    paintLeak(p);
    p.Clamp(0.f, 0.9f);
    p *= rnd;

    Texture::Channel & r = buffers[pacWallr];
    Texture::Channel & g = buffers[pacWallg];
    Texture::Channel & b = buffers[pacWallb];
    Texture::Channel & height = buffers[pacWallHeight];
    Texture::Channel & spec = buffers[pacWallSpecular];
    r = buffers[wall1r];
    g = buffers[wall1g];
    b = buffers[wall1b];
    height = buffers[wallHeight];
    spec = buffers[wallSpecular];

    r += p;
    g += p;
    b -= p;
    spec += p;
  }

  void buildUrban()
  {
//     buildGutter();
    buildWalls();
    buildWalls2();
    buildRoofs();
    buildWindows();
    buildWindowsConfigurations();
    buildPacmanWall();

    buffers[wall1r].Mask(buffers[windowsConfAlpha], buffers[windowsConf1r]);
    buffers[wall1g].Mask(buffers[windowsConfAlpha], buffers[windowsConf1g]);
    buffers[wall1b].Mask(buffers[windowsConfAlpha], buffers[windowsConf1b]);
    buffers[wallHeight].Mask(buffers[windowsConfAlpha], buffers[windowsConfHeight]);
    buffers[wallSpecular].Mask(buffers[windowsConfAlpha], buffers[windowsConfSpecular]);

    buffers[wall2r].Mask(buffers[windowsConfAlpha], buffers[windowsConf1r]);
    buffers[wall2g].Mask(buffers[windowsConfAlpha], buffers[windowsConf1g]);
    buffers[wall2b].Mask(buffers[windowsConfAlpha], buffers[windowsConf1b]);

    buffers[wall3r].Mask(buffers[windowsConfAlpha], buffers[windowsConf1r]);
    buffers[wall3g].Mask(buffers[windowsConfAlpha], buffers[windowsConf1g]);
    buffers[wall3b].Mask(buffers[windowsConfAlpha], buffers[windowsConf1b]);
    buffers[wall3Height].Mask(buffers[windowsConfAlpha], buffers[windowsConfHeight]);
    buffers[wall3Specular].Mask(buffers[windowsConfAlpha], buffers[windowsConfSpecular]);
  }
}
