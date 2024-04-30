
#if TEXTURE_EXPOSE == TEXTURE_NAMES

grainMap,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_NAME

buildGrain,

#elif TEXTURE_EXPOSE == TEXTURE_FILE

__FILE__,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_BODY

//
// Grain pour le post-processing
//
void buildGrain()
{
  Channel grain;
  grain.Random();
  grain.Blur();
  grain.Scale(0., 1.);

  queueTextureA(grainMap,
		grain,
		GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		true, GL_REPEAT, false
		DBGARG("grainMap"));
}

#endif // TEXTURE_EXPOSE
