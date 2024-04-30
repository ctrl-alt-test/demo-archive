
#if TEXTURE_EXPOSE == TEXTURE_NAMES

albedoTest,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_NAME

buildAlbedoTest,

#elif TEXTURE_EXPOSE == TEXTURE_FILE

__FILE__,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_BODY

//
// Test de couleur
//
void buildAlbedoTest()
{
  Channel r;
  Channel g;
  Channel b;
  Channel a;
  r.Gradient(1.1f, 0.3f, 0.2f, 0.6f);
  g.Fade(0);
  b.Fade(90.f);

  Channel grid;
  grid.Fade(0); grid *= 32.f; grid.Mod();
  Channel tmp = grid;
  tmp.Rotate90();
  grid.MinimizeTo(tmp);
  grid.ClampAndScale(0.9f, 0.95f);
  r.MinimizeTo(grid);
  g.MinimizeTo(grid);
  b.MinimizeTo(grid);

  a.Fade(0); a *= 6.f; a.Mod();
  tmp = a;
  tmp.Rotate90();
  a.MaximizeTo(tmp);
  a.ClampAndScale(1.f, 0.7f);

  queueTextureRGBA(albedoTest, r, g, b, a,
		   GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		   true, GL_REPEAT, true
		   DBGARG("albedoTest"));
}

#endif // TEXTURE_EXPOSE
