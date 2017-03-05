#if TEXTURE_EXPOSE == TEXTURE_NAMES

paper,
paperBump,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_NAME

buildPaper,

#elif TEXTURE_EXPOSE == TEXTURE_FILE

__FILE__,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_BODY

void buildPaper()
{
  Channel t;
  t.Fade(0.);
  t.Mult(64);
  t.Sin();
  t.Pow(0.08f);

  Channel t2 = t;
  t2.Rotate90();
  t *= t2;

  Channel r = t; r.Scale(177.f/255.f, 1.f);
  Channel g = t; g.Scale(200.f/255.f, 1.f);
  Channel b = t; b.Scale(210.f/245.f, 1.f);

  queueTextureRGB(paper,
                  r, g, b,
        	  GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
        	  true, GL_REPEAT, true);
}

#endif // TEXTURE_EXPOSE
