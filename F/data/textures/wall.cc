#if TEXTURE_EXPOSE == TEXTURE_NAMES

wall,
wallBump,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_NAME

buildWall,

#elif TEXTURE_EXPOSE == TEXTURE_FILE

__FILE__,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_BODY

void addLeaf(Channel & tex, float angle, float sx, float sy, float x, float y)
{
  static float leafData[] = {
    0,  0.00f, 0.00f,
    0,  0.12f,-0.03f,
    0,  0.17f, 0.13f,
    0,  0.06f, 0.40f,
    0,  0.02f, 0.53f,
    0,  0.02f, 0.53f,
    0, -0.09f, 0.37f,
    0, -0.17f, 0.15f,
    0, -0.13f,-0.04f,
    0,  0.00f, 0.00f,
  };

  const float m[] = {
    sx * msys_cosf(angle),
    sy * msys_sinf(angle),
    x,
    -sx * msys_sinf(angle),
    sy * msys_cosf(angle),
    y,
  };

  drawSpline(tex, leafData, sizeof(leafData)/sizeof(leafData[0]), m);
}

void buildWall()
{
  Channel t;
  Channel t2;

  t.GaussianBlur();
  t.Fade(90.);
  t.Mult(50.);
  t.Sin();
  t2.Perlin(_TV(11), 1, 1, 1);
  t.Waves(0, _TV(0.75f), t2);
  t.Waves(90, _TV(0.75f), t2);
  t.AverageScale(6, 1.);

  Channel bande;
  bande.Fade(90);
  bande.Mult(4.f);
  bande.Mod();
  bande.ClampAndScale(0.5f, 0.55f);

  Channel pattern;

  {
    float plantData[] = {
      0,  0.00f, 0.48f,
      0,  0.18f, 0.34f,
      0,  0.61f, 0.23f,
      0,  0.65f, 0.14f,
      0,  0.36f, 0.05f,
      0, -0.12f, 0.00f,
    };

    // Fleur 1a
    {
      const float m[] = { 0.1f, 0.f, 0.f, 0.f, 1.f, 0.f };
      drawSpline(pattern, plantData, sizeof(plantData)/sizeof(plantData[0]), m);

      for (int i = 3; i < 7; ++i)
      {
	addLeaf(pattern, 2.f * PI * float(i) / 7.f, 0.1f, 0.2f, 0, 0);
      }
    }

    // Fleur 1b
    {
      const float m[] = { -0.1f, 0.f, 0.5f, 0.f, 1.f, 0.25f };
      drawSpline(pattern, plantData, sizeof(plantData)/sizeof(plantData[0]), m);

      for (int i = 1; i < 4; ++i)
      {
	addLeaf(pattern, 2.f * PI * float(i) / 7.f, 0.1f, 0.2f, 0.5f, 0.25f);
      }
    }
  }

  {
    float plantData[] = {
      0,  0.00f, 0.48f,
      0, -0.30f, 0.35f,
      0, -0.23f, 0.25f,
      0,  0.19f, 0.21f,
      0,  0.59f, 0.23f,
    };
    float curveData[] = {
      0,  0.05f, 0.22f,
      0,  0.42f, 0.16f,
      0,  0.55f, 0.08f,
      0,  0.26f, 0.02f,
      0, -0.22f, 0.00f,
    };

    // Fleur 2a
    {
      const float m[] = { 0.1f, 0.f, 0.25f, 0.f, 1.f, 0.5f };
      drawSpline(pattern, plantData, sizeof(plantData)/sizeof(plantData[0]), m);
      drawSpline(pattern, curveData, sizeof(curveData)/sizeof(curveData[0]), m);

      addLeaf(pattern, 1.f, 0.1f, 0.1f, 0.3f, 0.72f);

      for (int i = 0; i < 3; ++i)
      {
	addLeaf(pattern, 4.f + 0.5f * float(i), 0.2f, 0.1f, 0.25f, 0.5f);
      }
    }

    // Fleur 2b
    {
      const float m[] = { -0.1f, 0.f, 0.75f, 0.f, 1.f, 0.75f };
      drawSpline(pattern, plantData, sizeof(plantData)/sizeof(plantData[0]), m);
      drawSpline(pattern, curveData, sizeof(curveData)/sizeof(curveData[0]), m);

      addLeaf(pattern, -1.f, 0.1f, 0.1f, 0.7f, 0.97f);

      for (int i = 0; i < 5; ++i)
      {
	addLeaf(pattern, -0.5f + 0.5f * float(i), 0.2f, 0.1f, 0.75f, 0.75f);
      }
    }
  }

  pattern.Dilate(0.015f);

  Channel pattern2;

  // Allez on se prend pour des artistes :
  for (int i = 0; i < 60; ++i)
  {
    addLeaf(pattern2, 3.f * msys_frand() - 1.5f, 0.06f, 0.06f, msys_frand(), msys_frand());
  }
  pattern2.Dilate(0.005f);

  pattern += pattern2;
  pattern.ClampAndScale(0.5f, 0.6f);

  bande += pattern;
  bande.Mod();

  Channel bump = t;
  t.Mix(0.8f, bande);

  Channel r = t; r.Scale( 61.f/255.f, 136.f/255.f);
  Channel g = t; g.Scale(131.f/255.f, 211.f/255.f);
  Channel b = t; b.Scale( 98.f/255.f, 164.f/255.f);

  queueTextureRGB(wall,
		  r, g, b,
        	  GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
        	  true, GL_REPEAT, true);

  bump.Scale(_TV(0.f), _TV(0.4f));
  buildAndQueueBumpMapFromHeightMap(wallBump, bump, true);
}

#endif // TEXTURE_EXPOSE
