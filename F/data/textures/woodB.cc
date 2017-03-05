#if TEXTURE_EXPOSE == TEXTURE_NAMES
#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_NAME

buildWoodB,

#elif TEXTURE_EXPOSE == TEXTURE_FILE

__FILE__,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_BODY

void buildWoodB()
{
  Channel a;
  Channel t;
  Channel t2;
  Channel t3;

  t.Fade(0);
  t.Scale(0.f, 16.f);
  t.Mod();

  t2.Random();
  t2.HorizontalMotionBlur(0.005f);
  t.Mix(0.8f, t2);

  t2.Perlin(4, 1, 1, 1);
  t.Mix(0.3f, t2);

  t2.Fade(90);
  t2.Scale(0.f, 2.f);
  t2.Sin();
  t2.Pow(0.5f);

  t3.Perlin(3, 1, 1, 1);
  t2.Mix(0.1f, t3);

  t.Waves(90, 6.f, t2);
  t.Pow(1.5f);

//   queueWoodAlbedo(wood6, &t,
// 		  0.7f, 0.3f, 0.1f,
// 		  1.0f, 0.8f, 0.5f);

  // couleurs d'un noyer
  queueWoodAlbedo(wood_NoyerB, &t,
		  0.17f, 0.10f, 0.07f,
		  0.40f, 0.27f, 0.11f);

  // Merisier clair
//   queueWoodAlbedo(wood8, &t,
// 		  133.f/255.f,  72.f/255.f,  25.f/255.f,
// 		  195.f/255.f, 123.f/255.f,  65.f/255.f);
}

#endif // TEXTURE_EXPOSE
