#if TEXTURE_EXPOSE == TEXTURE_NAMES
#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_NAME

builArmchair,

#elif TEXTURE_EXPOSE == TEXTURE_FILE

__FILE__,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_BODY

void builArmchair()
{
  Channel t;
  t.Cells(100);

  Channel r = t; r.Scale(53.f/255.f, 83.f/255.f);
  Channel g = t; g.Scale(32.f/255.f, 54.f/255.f);
  Channel b = t; b.Scale(27.f/245.f, 50.f/255.f);

  queueTextureRGB(armchair,
                  r, g, b,
        	  GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
        	  true, GL_REPEAT, false);

  t.Random();
  t.GaussianBlur();
  t.Scale(0., 0.25f);
  buildAndQueueBumpMapFromHeightMap(armchairBump, t, true);
}

#endif // TEXTURE_EXPOSE
