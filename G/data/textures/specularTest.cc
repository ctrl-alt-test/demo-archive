
#if TEXTURE_EXPOSE == TEXTURE_NAMES

testSpecular,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_NAME

buildSpecularTest,

#elif TEXTURE_EXPOSE == TEXTURE_FILE

__FILE__,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_BODY

//
// Test de speculaire
//
void buildSpecularTest()
{
  Channel t;
  t.Fade(0);

  Channel grid;
  grid.Fade(0); grid *= 8.f; grid.Mod();
  Channel tmp = grid;
  tmp.Rotate90();
  grid.MaximizeTo(tmp);
  grid.ClampAndScale(0.4f, 0.45f);

  t *= grid;

  queueTextureA(testSpecular, t,
		GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		true, GL_REPEAT, false
		DBGARG("specularTest"));
}

#endif // TEXTURE_EXPOSE
