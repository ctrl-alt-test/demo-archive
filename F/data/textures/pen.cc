#if TEXTURE_EXPOSE == TEXTURE_NAMES

pen,
penBump,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_NAME

buildPen,

#elif TEXTURE_EXPOSE == TEXTURE_FILE

__FILE__,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_BODY

void buildPen()
{
  // Attention, le mesh du crayon utilise un hack qui a pour effet
  // secondaire d'étirer les coordonnées de texture. Ce code en tient
  // compte.
  int size = 256;
  Channel t(size, size);
  t.Fade(90.f);
  t.ClampAndScale(0.665f, 0.67f); // Corps/bois

  Channel grain(size, size);
  grain.Random();
  grain.HorizontalMotionBlur(0.02f);
  grain.Scale(0.f, 1.f);
  grain *= t;

  t.Scale(1.f, 0.f);

  Channel grain2(size, size);
  grain2.Random();
  grain2.GaussianBlur();
  grain2.HorizontalMotionBlur(0.01f);
  grain2.VerticalMotionBlur(0.05f);
  grain2.Scale(0.f, 1.f);

  buildAndQueueBumpMapFromHeightMap(orgalameBump, grain2, true);

  grain2.Scale(0.5f, 1.f);
  grain2 *= t;
  grain += grain2;

  buildAndQueueBumpMapFromHeightMap(penBump, grain, true);

  // Bleu pour le corps, bois pour le bois
  Channel r = t; r.Scale(255.f/255.f,   0.f/255.f);
  Channel g = t; g.Scale(204.f/255.f,  82.f/255.f);
  Channel b = t; b.Scale(161.f/255.f, 180.f/255.f);

  // Mine
  t.Fade(85.f);
  t.ClampAndScale(0.88f, 0.875f);

  r *= t;
  g *= t;
  b *= t;

  queueTextureRGB(pen,
                  r, g, b,
                  GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
                  true, GL_REPEAT, true);

}

#endif // TEXTURE_EXPOSE
