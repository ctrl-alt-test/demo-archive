
#if TEXTURE_EXPOSE == TEXTURE_NAMES

buildingLightPatterns,
buildingBumpPatterns,
buildingSpecularPatterns,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_NAME

buildBuildingLightPatterns,

#elif TEXTURE_EXPOSE == TEXTURE_FILE

__FILE__,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_BODY

//
// Test de speculaire
//
void buildBuildingLightPatterns()
{
  float height = 0.05f;

  const unsigned int size = 128;

  Channel h(size, size); h.Fade(90); h.Scale(0, 2.f); h.Mod();
  Channel v = h; v.Rotate90();

  // ==========================================
  Channel t(h.Width(), h.Height());
  Channel t2(t.Width(), t.Height());

  Channel r(t.Width(), t.Height());
  Channel g(t.Width(), t.Height());
  Channel b(t.Width(), t.Height());

  for (unsigned int i = 0; i < 8; ++i)
  {
    const float x = msys_sfrand();
    const float y = msys_sfrand();
    float width = 0.05f + 0.05f * msys_frand();
    float length = width * (1.f + 2.f * msys_frand());
    if (msys_rand() % 2 == 0)
    {
      float swap = length;
      length = width;
      width = swap;
    }
    t = h; t.ClampAndScale(1.f - width, 1.f - width + height);
    t2 = v; t2.ClampAndScale(1.f - length, 1.f - length + height);

    t.MaximizeTo(t2);
    t.Translate(x, y);

    const int choose = msys_rand() % 3;
    Channel & target = (0 == choose ? r : (1 == choose ? g : b));
    target.MinimizeTo(t);
  }

  // ==========================================
  // Heightmap
  t.Flat(0);

  t2.Random();
  t2.GaussianBlur();
  t2.HorizontalMotionBlur(0.05f);
  t2.VerticalMotionBlur(0.05f);
  t2.Scale(0, 1.f);
  t += t2;

  t2.Random();
  t2.HorizontalMotionBlur(0.001f);
  t2.VerticalMotionBlur(0.001f);
  t2.Scale(0, 0.05f);
  t += t2;

  t2.Random();
  t2.GaussianBlur();
  t2.Scale(0.f, 1.f);
  t2.ClampAndScale(0.4f, 1.f);
  t2.Pow(1.2f);
  t *= t2;

  t.Scale(0.8f, 1.f);

  t2.Flat(0);
  t2.MinimizeTo(r);
  t2.MinimizeTo(g);
  t2.MinimizeTo(b);
  t2.ClampAndScale(0.5f, 0);
  t2.Pow(2.f);

  t.MaximizeTo(t2);

  buildAndQueueBumpMapFromHeightMap(buildingBumpPatterns, t, true);

  // ==========================================
  // Light map
  Channel l(t.Width(), t.Height());
  buildAmbientOcclusionMapFromHeightMap(t, l);

  r.ClampAndScale(0.4f, 0.5f);
  g.ClampAndScale(0.4f, 0.5f);
  b.ClampAndScale(0.4f, 0.5f);

  queueTextureRGBA(buildingLightPatterns, r, g, b, l,
		   GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		   true, GL_REPEAT);

  // ==========================================
  // Specular map
  t.Flat(0);
  t.MinimizeTo(r);
  t.MinimizeTo(g);
  t.MinimizeTo(b);

  queueTextureA(buildingSpecularPatterns, t,
		GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		true, GL_REPEAT);
}

#endif // TEXTURE_EXPOSE
