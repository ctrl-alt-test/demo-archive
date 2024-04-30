
#if TEXTURE_EXPOSE == TEXTURE_NAMES

dummyRelief,
dummyBumpRelief,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_NAME

buildDummyRelief,

#elif TEXTURE_EXPOSE == TEXTURE_FILE

__FILE__,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_BODY

//
// Test de speculaire
//
void buildDummyRelief()
{
  // ==========================================
  // Heightmap
  Channel t;
  t.Flat(0.f);

  float x1, y1, x2, y2;
  int size = 18;
  for (unsigned int i = 0; i < 200; ++i)
  {
    x1 = msys_rand() % size * (1.f / size);
    y1 = msys_rand() % size * (1.f / size);
    if (msys_rand() % 2 == 0)
    {
      x2 = x1 + 1.f/size;
      y2 = y1;
    }
    else
    {
      y2 = y1 + 1.f/size;
      x2 = x1;
    }
    t.Line(x1, y1, x2, y2);
  }

  Channel t2 = t;
  t.VerticalMotionBlur(0.005f);
  t.HorizontalMotionBlur(0.01f);
  t += t2;
  t.VerticalMotionBlur(0.01f);
  t.HorizontalMotionBlur(0.01f);

  t.Scale(0.f, 2.8f);
  t.Mod();
  t.ClampAndScale(0.1f, 0.4f);

  t.GaussianBlur();

  t.AverageScale(4, 1.9f);
  t.Scale(0, 1.f);
  t.Pow(1.2f);
  t.Scale(0.5f, 1.f);

  t2.Conic(); t2 *= 2.f; t2.Mod();
  t2.ClampAndScale(0.9f, 0.8f);
  t.MaximizeTo(t2);

  buildAndQueueBumpMapFromHeightMap(dummyBumpRelief, t);

  // ==========================================
  // Light map
  Channel l;
  buildAmbientOcclusionMapFromHeightMap(t, l);

  t.Flat(0);

  queueTextureRGBA(dummyRelief, t, t, t, l,
		   GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		   true, GL_REPEAT);
}

#endif // TEXTURE_EXPOSE
