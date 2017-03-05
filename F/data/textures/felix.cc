#if TEXTURE_EXPOSE == TEXTURE_NAMES

felix,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_NAME

buildFelix,

#elif TEXTURE_EXPOSE == TEXTURE_FILE

__FILE__,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_BODY

void buildFelix()
{
  Channel t;
  Channel t2;

  float leafData[] = {
    0, 158, 487,
    0, 122, 470,
    0, 110, 414,
    0, 106, 202,
    0, 97, 195,
    0, 86, 236,
    0, 63, 240,
    0, 72, 216,
    0, 87, 111,
    0, 119, 74,
    0, 128, 53,
    0, 121, 31,
    0, 140, 11,
    0, 165, 34,
    0, 165, 70,
    0, 194, 92,
    0, 212, 155,
    0, 220, 215,
    0, 212, 275,
    0, 203, 259,
    0, 207, 221,
    0, 204, 201,
    0, 188, 185,
    0, 194, 281,
    0, 182, 389,
    0, 180, 478,
    0, 158, 487,
  };

  for (int i = 0; i < sizeof(leafData)/sizeof(leafData[0]); i += 3)
  {
    leafData[i+1] /= 256.f;
    leafData[i+2] /= 512.f;
  }
  const float m[] = { 1.f, 0.f, 0.f, 0.f, 1.f, 0.f };
  drawSpline(t, leafData, sizeof(leafData)/sizeof(leafData[0]), m);
  t.FloodFill(1, 1, 1.f);

  t.HorizontalMotionBlur(0.02f);
  t.VerticalMotionBlur(0.02f);

  queueTextureA(felix,
		t,
		GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		true, GL_REPEAT, true);
}

#endif // TEXTURE_EXPOSE
