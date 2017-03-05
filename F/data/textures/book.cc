#if TEXTURE_EXPOSE == TEXTURE_NAMES
#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_NAME

buildBook,

#elif TEXTURE_EXPOSE == TEXTURE_FILE

__FILE__,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_BODY

void buildBook()
{
  Channel t;
  Channel t2;

  Channel mask; // tranche ou pas
  mask.Fade(90.);
  mask.ClampAndScale(0.75f, 0.77f);

  t.Random();
  t.Mult(0.15f);
  t.VerticalMotionBlur(0.05f);

  // pages
  Channel pa;
  pa.Random();
  pa.VerticalMotionBlur(0.1f);

  t.Mask(mask, pa);

  Channel r = t; r.Scale(0.4f, 1.f);
  Channel g = t; g.Scale(0.1f, 1.f);
  Channel b = t; b.Scale(0.05f, 0.9f);

  queueTextureRGB(book,
                  r, g, b,
        	  GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
        	  true, GL_REPEAT, true);

  t.Random();
  t.VerticalMotionBlur(0.01f);
  mask.Scale(1., 0.);
  t *= mask;
  t.Scale(0., 0.2f);
  buildAndQueueBumpMapFromHeightMap(bookBump, t, true);

  /*
  r.Fade(0); r.Scale(0.f, 16.f); r.Mod(); r.ClampAndScale(0.499f, 0.501f);
  g.Fade(90); g.ClampAndScale(0.499f, 0.501f);
  b.Fade(0); b.ClampAndScale(0.499f, 0.501f);
  queueTextureRGB(greetingsBooks,
                  r, g, b,
        	  GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
        	  true, GL_REPEAT);
  */
}

#endif // TEXTURE_EXPOSE
