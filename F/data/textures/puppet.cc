#if TEXTURE_EXPOSE == TEXTURE_NAMES
#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_NAME

buildPuppet,

#elif TEXTURE_EXPOSE == TEXTURE_FILE

__FILE__,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_BODY

void buildPuppet()
{
  Channel t;
  Channel t2;
  Channel w;

  w.Random();
  w.HorizontalMotionBlur(0.01f);

  // visage
  t.Conic();
  t.ClampAndScale(0.8f, 1.f);

  t2.Fade(90);
  t2.ClampAndScale(0.52f, 0.4f);
  t.MaximizeTo(t2);

  t.Pow(0.25f);
  t.Translate(0.15f, 0.f);

  t2.Conic();
  t2.ClampAndScale(0.93f, 0.95f);
  t2.Translate(-0.1f, 0.05f);
  t.MinimizeTo(t2);
  t2.Translate(0.f, -0.1f);
  t.MinimizeTo(t2);
  t.Translate(0, 0.5f);

  t.Mix(0.5, w);
  Channel r = t; r.Scale(162.f/255.f, 233.f/255.f);
  Channel g = t; g.Scale(110.f/255.f, 186.f/255.f);
  Channel b = t; b.Scale(63.f/255.f, 154.f/255.f);
  queueTextureRGB(puppetHead,
                  //t, t, t,
                  r, g, b,
        	  GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
        	  true, GL_REPEAT, true);

  // corps
  t.Fade(90.f);
  t.ClampAndScale(0.57f, 0.58f);

  t2.Fade(0.f);
  t2.ClampAndScale(0.11f, 0.109f);
  t2.VerticalMotionBlur(0.006f);
  float shift = 0.06f;
  t2.Translate(0.f, -shift);
  t.MinimizeTo(t2);
  t2.Translate(0.f, 3.f*shift);
  t.MinimizeTo(t2);

  t.Mix(0.5f, w);
  Channel r2 = t; r2.Scale(92.f/255.f, 233.f/255.f);
  Channel g2 = t; g2.Scale(60.f/255.f, 186.f/255.f);
  Channel b2 = t; b2.Scale(43.f/255.f, 154.f/255.f);
  queueTextureRGB(puppetBody,
                  r2, g2, b2,
        	  GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
        	  true, GL_REPEAT, true);

  w.Scale(0., 0.5);
  buildAndQueueBumpMapFromHeightMap(puppetBump, w, true);
}

#endif // TEXTURE_EXPOSE
