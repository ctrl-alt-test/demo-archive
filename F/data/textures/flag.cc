#if TEXTURE_EXPOSE == TEXTURE_NAMES

flag,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_NAME

buildFlag,

#elif TEXTURE_EXPOSE == TEXTURE_FILE

__FILE__,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_BODY

void buildFlag()
{
  const unsigned int width = 32;
  const unsigned int height = 1;

  Channel rouge(width, height); rouge.Fade(90.f);
  rouge.ClampAndScale(0.667f, 0.668f);

  Channel bleu(width, height); bleu.Fade(90.f);
  bleu.ClampAndScale(0.334f, 0.333f);

  Channel blanc = rouge; blanc += bleu;
  blanc.Scale(1.f, 0.f);

  Channel r = rouge; r.Scale(0, 0.957f);
  r.MinimizeTo(blanc);

  Channel g = blanc;

  Channel b = bleu; b.Scale(0, 0.56f);
  b.MinimizeTo(blanc);

  queueTextureRGB(flag,
                  r, g, b,
        	  GL_LINEAR_MIPMAP_LINEAR, GL_NEAREST,
        	  true, GL_CLAMP_TO_EDGE, true);

}
#endif
