
#if TEXTURE_EXPOSE == TEXTURE_NAMES

lightPatterns,
bumpPatterns,
specularPatterns,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_NAME

buildLightPatterns,

#elif TEXTURE_EXPOSE == TEXTURE_FILE

__FILE__,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_BODY

//
// Test de speculaire
//
void buildLightPatterns()
{
  const unsigned int size = 256;

  float widthext = 0.03f;

  Channel h(size, size); h.Fade(90); h.Scale(0, 2.f); h.Mod();
  Channel v(size, size); v = h; v.Rotate90();

  // ==========================================
  // Motif de base

  Channel t1(h); t1.ClampAndScale(1.f - widthext, 1.f);
  Channel t2(v); t2.ClampAndScale(0.5f - widthext, 0.5f);

  Channel t(t1);
  t.Translate(-0.2f, 0);

  t1.MaximizeTo(t2);

  t1.Translate(0.4f, -0.1f); t += t1;
  t1.Translate(0.25f, 0); t += t1;
  t1.Translate(0.25f, 0); t += t1;
  t1.Translate(0.25f, 0); t += t1;

  t2 = v; t2.ClampAndScale(0.75f - widthext, 0.75f);
  t1.MaximizeTo(t2);

  t1.Translate(-0.1f, 0.25f);
  t2 = t1;
  t2.Translate(0, 0.5f);
  t1 += t2;

  t += t1;
  t1.Translate(-0.5f, 0.25f);
  t += t1;

  // ==========================================
  // Heightmap
  Channel tile(size / 4, size / 4);
  {
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
      tile.Line(x1, y1, x2, y2);
    }

    Channel h2(tile);
    tile.VerticalMotionBlur(0.005f);
    tile.HorizontalMotionBlur(0.01f);
    tile += h2;
    tile.VerticalMotionBlur(0.01f);
    tile.HorizontalMotionBlur(0.01f);

    tile.Scale(0.f, 2.8f); tile.Mod(); tile.ClampAndScale(0.1f, 0.4f);
    tile.GaussianBlur();

    tile.AverageScale(4, 1.9f);
    tile.Scale(0, 1.f);
    tile.Pow(1.2f);
    tile.Scale(0.5f, 1.f);
  }

  Channel height(size, size);
  for (unsigned int j = 0; j < 4; ++j)
    for (unsigned int i = 0; i < 4; ++i)
    {
      height.subCopy(tile,
		     0, 0,
		     i * tile.Width(), j * tile.Height(),
		     tile.Width(), tile.Height());
    }

  // Rainures
  t1 = t; t1.Clamp(0.f, 0.2f); t1.Scale(1.f, 0.1f);
  t2 = t; t2.Clamp(0.5f, 0.51f); t2.Scale(0.1f, 0);

  t1 += t2;

  height.MaximizeTo(t1);

  //
  // FIXME : ajouter le bruit
  //

  buildAndQueueBumpMapFromHeightMap(bumpPatterns, height, true);

  // ==========================================
  // Rayons
  t1.Fade(90); t1 *= 8.f; t1.Mod();
  t1.ClampAndScale(0.66f, 0.661f);

  Channel r(t);
  r.ClampAndScale(0.75f, 0.755f); // Un quart de la rainure

  Channel g(r);
  r *= t1;

  t1.Scale(1.f, 0.f);
  g *= t1;

  t1.Flat(0); // Dummy

  Channel l(size, size);
  buildAmbientOcclusionMapFromHeightMap(height, l);

  queueTextureRGBA(lightPatterns, r, g, t1, l,
		   GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		   true, GL_REPEAT);

  // ==========================================
  // Specular map
  r.MinimizeTo(g);
  //     r.MinimizeTo(b);

  queueTextureA(specularPatterns, r,
		GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		true, GL_REPEAT);
}

#endif // TEXTURE_EXPOSE
