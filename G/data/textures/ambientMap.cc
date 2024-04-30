
#if TEXTURE_EXPOSE == TEXTURE_NAMES

ambientMap,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_NAME

buildAmbientMap,

#elif TEXTURE_EXPOSE == TEXTURE_FILE

__FILE__,

#elif TEXTURE_EXPOSE == TEXTURE_BUILDER_BODY

//
// Ambient map style spherical harmonics
// Du bleu d'un côté, du beige de l'autre, du blanc en haut et en bas
//
// Blanc de base : FFFBF4 - 255, 251, 244 - 1.00, 0.98, 0.96
// Bleu :          A7B6D5 - 167, 182, 213 - 0.65, 0.71, 0.83
// Orange :        FFEAC3 - 255, 234, 195 - 1.00, 0.92, 0.76
//
void buildAmbientMap()
{
  const float intensity = 0.5f;

  const float white_r = 255.f/255.f * intensity;
  const float white_g = 251.f/255.f * intensity;
  const float white_b = 244.f/255.f * intensity;

  const float blue_r = 0.5f * (1.f + 167.f/255.f) * intensity;
  const float blue_g = 0.5f * (1.f + 182.f/255.f) * intensity;
  const float blue_b = 0.5f * (1.f + 213.f/255.f) * intensity;

  const float beige_r = 0.5f * (1.f + 255.f/255.f) * intensity;
  const float beige_g = 0.5f * (1.f + 234.f/255.f) * intensity;
  const float beige_b = 0.5f * (1.f + 195.f/255.f) * intensity;

  Channel h(256, 256); h.Cosinus(1, 0);
  Channel r(h); r.Scale(blue_r, beige_r);
  Channel g(h); g.Scale(blue_g, beige_g);
  Channel b(h); b.Scale(blue_b, beige_b);

  Channel v(256, 256); v.Cosinus(0, 1);
  Channel wr(256, 256); wr.Flat(white_r); r.Mask(v, wr);
  Channel wg(256, 256); wg.Flat(white_g); g.Mask(v, wg);
  Channel wb(256, 256); wb.Flat(white_b); b.Mask(v, wb);

  // Pas de mipmapping car les interpolations créent des artefacts
  // très laids
  queueTextureRGB(ambientMap, r, g, b,
		  GL_LINEAR, GL_LINEAR,
		  false, GL_REPEAT);
}

#endif // TEXTURE_EXPOSE
