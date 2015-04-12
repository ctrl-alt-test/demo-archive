uniform sampler2D c2Map;
uniform float glowness;

//
// Renvoie la couleur en tenant compte du glow
//
// Exemple d'utilisation :
// color = combineGlow(color, gl_TexCoord[0].xy);
//
vec4 combineGlow(vec4 color, vec2 uv)
{
    vec4 glow = texture2D(c2Map, uv);

    return color + glow * glowness * pow(1. - glow.a, 1.25);
}
