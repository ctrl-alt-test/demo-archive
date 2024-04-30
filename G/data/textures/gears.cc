#if TEXTURE_EXPOSE == TEXTURE_BUILDER_NAME
buildGear,

#elif TEXTURE_EXPOSE == TEXTURE_FILE
__FILE__,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_BODY

void buildGear()
{
  Channel t;
  Channel t2;

  t.Cells(20);
  t.AverageScale(6, 1.5f);

  t2.Cells(5);
  t2.AverageScale(6, 2.f);
  t.Mix(0.5f, t2);

  t2.Random();
  t.Mix(0.125f, t2);

  t.Scale(0.f, 1.f);
  t.ClampAndScale(0.2f, 0.8f);
  //t.Scale(0., 0.2);

  Channel a;
  a.Flat(1.f);
  queueTextureRGBA(gear, t, t, t, a,
		   GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		   true, GL_REPEAT, false
		   DBGARG("gear"));

  buildAndQueueBumpMapFromHeightMap(gearBump, t, true);
}

#endif // TEXTURE_EXPOSE
