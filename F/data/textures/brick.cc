#if TEXTURE_EXPOSE == TEXTURE_NAMES

brick,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_NAME

buildBrick,

#elif TEXTURE_EXPOSE == TEXTURE_FILE

__FILE__,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_BODY

void buildBrick()
{
  const int size = 512; //256;
  float nlines = size / 32.f;
  int ncols = 8;

  Texture::Channel t;
  t.Fade(0.f);
  t.Scale(-0.1f, nlines*2.f-0.1f);
  t.Mod();
  t.Pow(0.25f);

  Texture::Channel t2;
  t2.Fade(90.f);
  t2.Scale(0.f, ncols * 2.f);
  t2.Mod();
  t2.Pow(0.2f);

  t.MaximizeTo(t2);

  Texture::Channel rnd;
  rnd.Random();
  rnd.GaussianBlur();
  rnd.Scale(-0.1f, 0.1f);
  t += rnd;
  t.ClampAndScale(0.5f, 0.8f);

  rnd.Cells(5);
  rnd.Scale(0.6f, 1.f);
  t *= rnd;
  rnd.Cells(10);
  rnd.Scale(0.5f, 1.f);
  t *= rnd;

  rnd.Fade(90.f);
  rnd.Scale(0.f, 17);
  rnd.Sin();
  rnd.Scale(0.7f, 1.f);
  t *= rnd;

  Texture::Channel line;
  line.Fade(0.f);
  line.ClampAndScale(1.f/nlines, 1.f/nlines+0.01f);

  for (int i = 0; i < nlines; i++) {
    t2 = t;
    t2.Translate((msys_rand() % ncols)/(float)ncols + (i%2)/(2.f*ncols), 0.f);
    //  t2.Translate(0.35f, 0.f);
    t.Mask(line, t2);
    t.Translate(0.f, 1.f/nlines);
  }

  t2 = t;
  rnd.Random();
  rnd.GaussianBlur();
  rnd.Scale(-0.1f, 0.1f);
  t += rnd;
  t.Pow(0.7f);

  // EOI

  Texture::Channel r; r=t; r.Scale(0.f, 0.87f);
  Texture::Channel g; g=t; g.Scale(0.f, 0.4f);
  Texture::Channel b; b=t; b.Scale(0.f, 0.2f);

  Texture::Channel grey;
  grey.Random();
  grey.Blur();
  grey.Scale(0.5f, 0.9f);

  t2.ClampAndScale(0.25f, 0.1f);
  r.Mask(t2, grey);
  g.Mask(t2, grey);
  b.Mask(t2, grey);

  queueTextureRGB(brick,
                  r, g, b,
        	  GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
        	  true, GL_REPEAT, true);
}
#endif
