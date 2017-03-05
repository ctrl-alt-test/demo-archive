
uniform sampler2D mMap; // glow
uniform sampler2D sMap; // grain

uniform int time;
uniform float glowness;
uniform vec2 invResolution;
uniform vec2 center;

//
// Note concernant le gamma :
//
// - l'éclairage doit être fait en espace linéaire (avant correction)
// - en R8G8B8 la précision est insuffisante et ça crée des artefacts
//

// ---------------------------------------------------------------------------

// Fonction rand référence
float rand(vec2 v)
{
  return fract(sin(dot(v, vec2(12.9898,78.233))) * 43758.5453);
}

/*
float rand(vec2 texcoord)
{
  return fract(sin(dot(texcoord, vec2(12.9898,78.233)) + time * 0.0001) * 43758.5453) * 2. - 1.;
}

// FIXME : si possible factoriser le rand
vec3 tvRand(vec3 v)
{
  float seed = 0.0005 * v.x + v.y + v.z;
  return fract(vec3(sin(seed * 12.9898),
		    sin(seed * 78.233),
		    sin(seed * 91.2228)) * 43758.5453);
}
*/

vec3 rand(vec3 v)
{
  // FIXME : vérifier que ça ne fait pas de motif
  float seed = dot(v.xyz, v.yzx);
  return fract(43758.5453 * sin(seed * vec3(12.9898, 78.233, 91.2228)));
}

// ---------------------------------------------------------------------------
// Effets sur les couleurs

/*
//
// Ajoute un bruit homogène, type bruit de capteur
//
// Exemple d'utilisation :
// color = addRegularNoise(color, gl_TexCoord[0].xy, 0.03);
//
vec4 addRegularNoise(vec4 color, vec2 uv, float intensity)
{
  vec3 noise = rand(vec3(uv, float(time) * 0.0002));
  return vec4(mix(color.rgb, noise, intensity), color.a);
}

//
// Ajoute un bruit qui suit des scanlines, type antenne télé mal réglée
//
// Exemple d'utilisation :
// color = addTVNoise(color, uv, 0.4, 1. + sin(0.001 * time));
//
vec4 addTVNoise(vec4 color, vec2 uv, float intensity, float speed)
{
  vec3 noise = tvRand(vec3(uv, float(time) * 0.0001 * speed));
  return vec4(mix(color.rgb, noise, intensity), color.a);
}
*/

// Grain à partir d'une texture
// Exemple :
//   grain(uv, color, 60, 0.75, 0.02);
float getGrain(vec2 uv, int t, int period)
{
  vec2 coord = uv;
  coord.y *= invResolution.x / invResolution.y;
  coord += rand(vec3(float(t / period), 1., 1.)).xy;
  return texture2D(sMap, coord).a;
}

vec4 grain(vec2 uv, vec4 color, int period, float freq, float intensity)
{
  float grain1 = getGrain(uv * freq, time, period);
  float grain2 = getGrain(uv * freq, time + period, period);
  float perc = mod(float(time), float(period)) / float(period);
  float grain = 2. * mix(grain1, grain2, perc) - 1.;
  return vec4(color.rgb + grain * intensity, color.a);
}


//
// Assombrit le pourtour de l'image
//
// En réalité ça dépend de la fov, ici on considère qu'on a à peu près 60°
//
vec4 addVignetting(vec4 color, vec2 uv)
{
  // Le vignetting est une fonction de cos4(theta)
  vec2 coord = uv - center;
  coord.y *= invResolution.x / invResolution.y;
  float d = 1.77 * length(coord); // Diagonale approximative

  // Identité remarquable : cos x = 1/sqrt(1 + tan2 x)
  //
  // Du coup  cos4 x = 1/(1 + tan2 x)^2
  //                 = 1/(1 + (d/l)^2)^2
  // Pour 60°, l de l'ordre de 2.5
  // Au carré ça fait ~6.25, j'ai mis 8
  float cos4 = 1./pow(1. + d * d / 8., 2.);

  return vec4(cos4 * color.rgb, color.a);
}

//
// Ajoute une fine bordure noire autour de l'image, pour rendre le
// pourtout moins violent et imiter la pellicule
//
// Exemple d'utilisation :
// color = addBorder(color, gl_TexCoord[0].xy, 0.02);
//
vec4 addBorder(vec4 color, vec2 uv, float thickness)
{
  float hThickness = thickness;
  float vThickness = hThickness * invResolution.y / invResolution.x;
  float coeff = (smoothstep(0., hThickness, uv.x) *
		 smoothstep(1., 1. - hThickness, uv.x) *
		 smoothstep(0., vThickness, uv.y) *
		 smoothstep(1., 1. - vThickness, uv.y));
  coeff = pow(coeff, 0.4);
  return vec4(coeff * color.rgb, color.a);
}

//
// Renvoie la couleur en tenant compte du glow
//
// Exemple d'utilisation :
// color = combineGlow(color, gl_TexCoord[0].xy);
//
vec4 combineGlow(vec4 color, vec2 uv)
{
  vec4 glow = texture2D(mMap, uv);
  return color + glow * glowness * pow(1. - glow.a, 1.75);
}

//
// Filtre la couleur suivant les arguments
//
// Exemple d'utilisation :
// color = filter(color, vec3(1.), vec3(0.), vec3(1.));
//
vec4 filter(vec4 color, vec3 low, vec3 hig, vec3 gamma)
{
  return vec4(pow(clamp(mix(low, hig, color.rgb), 0., 1.), gamma), color.a);
}

/*
//
// Réduction des couleurs à une palette 256 naïve
//
// Exemple d'utilisation :
// color = palettize(texture2D(cMap, gl_TexCoord[0].xy).rgb);
//
vec3 palettize(vec3 color)
{
  vec3 nuances = vec3(8., 8., 4.);
  return color - fract(nuances * color) / nuances;
}

//
// Saturation entre 0 et la couleur originale
//
vec3 desaturate(vec3 color, float saturation)
{
  // FIXME : voir pour une équation plus rigoureuse de l'intensité
  float intensity = length(color);
  return mix(vec3(intensity), color.rgb, saturation);
}

vec3 saturate(vec3 col, float n)
{
  float grey = (col.r + col.g + col.b) / 3.;
  col.r += (col.r - grey) * n;
  col.g += (col.g - grey) * n;
  col.b += (col.b - grey) * n;
  return col;
}

// Ca pourrait être mieux d'avoir une fonction saturate entre -1 et 1 :
// -1 : désaturé
// 0 : identique
// 1 : saturé
// Voir comment c'est dans les logiciels de retouche d'image


// ---------------------------------------------------------------------------
// Effets sur la géométrie

//
// Dispersion chromatique
//
// Exemple d'utilisation :
// color = dispersion(cMap, gl_TexCoord[0].xy, 0.02);
//
vec4 dispersion(sampler2D tex, vec2 uv, float intensity)
{
  vec2 offset = vec2(intensity, 0.);
  vec3 c = vec3(0.);

  float i;
  float inc = 0.1; // FIXME: reste à rendre cette valeur dynamique
  for(i = -1.; i <= 1.; i += inc)
  {
    float rb = .5 + i * .5;
    vec3 w = vec3(rb, 1. - abs(i), 1. - rb);
    c += inc * w * texture2D(tex, uv + i * offset).rgb;
  }
  return vec4(c, texture2D(tex, uv).a);
}

//
// Décalle les composantes R et B de part et d'autre
//
// Exemple d'utilisation :
// color = dealign(cMap, gl_TexCoord[0].xy, 0.005);
//
vec4 dealign(sampler2D tex, vec2 uv, float intensity)
{
  vec2 offset = vec2(intensity, 0);

  float r = texture2D(tex, uv - offset).r;
  float g = texture2D(tex, uv).g;
  float b = texture2D(tex, uv + offset).b;
  float a = texture2D(tex, uv).a;
  return vec4(r, g, b, a);
}

//
// Décalle les composantes R et B de part et d'autre en suivant un
// motif d'onde
//
// Exemple d'utilisation :
// color = TVdealign(cMap, gl_TexCoord[0].xy, 1.);
//
vec4 TVdealign(sampler2D tex, vec2 uv, float intensity)
{
  float y = gl_TexCoord[0].y;
  float var1 = pow(abs(sin(10. * y + 0.001 * float(time))), 2.);
  float var2 = 0.75 + 0.25 * sin(500. * y + 0.01 * float(time));
  float var = 0.002 + 0.0015 * var1 * var2;
  return dealign(tex, uv, var * intensity);
}

//
// Applique un décalage horizontal aléatoire à chaque bloc de lignes
//
// Exemple d'utilisation :
// color = hcryptic(cMap, gl_TexCoord[0].xy, 0.05, 0.001);
//
vec4 hcryptic(sampler2D tex, vec2 uv, float intensity, float resolution)
{
  vec2 offset = vec2(intensity * rand(vec2(float(time), uv.y)), 0);
  return texture2D(tex, uv + offset);
}

//
// Pixelisation
//
// Exemple d'utilisation :
// color = pixelize(cMap, gl_TexCoord[0].xy, 1024. * (1. - abs(sin(0.001 * time))));
//
vec4 pixelize(sampler2D tex, vec2 uv, float resolution)
{
  vec2 coord = floor(resolution * uv) / resolution;
  return texture2D(tex, coord);
}

// Film grain & scanlines shader
// http://www.truevision3d.com/forums/showcase/staticnoise_colorblackwhite_scanline_shaders-t18698.0.html

// Exemple d'utilisation :
// color.rgb = film(color.rgb, time, uv);

vec3 film(vec3 color, int time, vec2 pos) {
  float nIntensity = 0.5;
  float sIntensity = 0.1;
  float sCount = 4096;

  vec2 vUv = pos;
  // sample the source
  vec3 cTextureScreen = color;
  // make some noise
  float x = vUv.x * vUv.y * time *  2000.0;
  x = mod( x, 13.0 ) * mod( x, 123.0 );
  float dx = mod( x, 0.01 );
  // float dx = rand(vec3(vUv.x, vUv.y, float(time))).x; //.x;
  // add noise
  vec3 cResult = cTextureScreen + cTextureScreen * clamp( 0.1 + dx * 100.0, 0.0, 1.0 );
  // get us a sine and cosine
  vec2 sc = vec2( sin( vUv.y * sCount ), cos( vUv.y * sCount ) );
  // add scanlines
  cResult += cTextureScreen * vec3( sc.x, sc.y, sc.x ) * sIntensity;
  // interpolate between source and result by intensity
  cResult = cTextureScreen + clamp( nIntensity, 0.0,1.0 ) * ( cResult - cTextureScreen );
  // convert to grayscale if desired
  cResult = vec3( cResult.r * 0.3 + cResult.g * 0.59 + cResult.b * 0.11 );
  return cResult;
}

 //	Depth-of-field shader with bokeh
 //	ported from GLSL shader by Martins Upitis
 //	http://artmartinsh.blogspot.com/2010/02/glsl-lens-blur-filter-with-bokeh.html

uniform float zNear;
uniform float zFar;

vec4 bokeh(sampler2D tColor, sampler2D tDepth, vec2 vUv) {
  float focus = 0.6;
  float aspect = 1.;
  float aperture = 0.05;
  float maxblur = 1.;

  vec2 aspectcorrect = vec2(1., aspect);
  float z = texture2D(tDepth, vUv).x; // FIXME: utiliser getDepth
  float depth1 = (2.0 * zNear) / (zNear + mix(zFar, zNear, z));

  float factor = depth1 - focus;
  vec2 dofblur = vec2(clamp(factor * aperture, -maxblur, maxblur));
  vec2 dofblur9 = dofblur * 0.9;
  vec2 dofblur7 = dofblur * 0.7;
  vec2 dofblur4 = dofblur * 0.4;

  vec4 col = vec4(0.);

  col += texture2D(tColor, vUv.xy);
  col += texture2D(tColor, vUv.xy + (vec2( 0.0,   0.4 ) * aspectcorrect) * dofblur);
  col += texture2D(tColor, vUv.xy + (vec2( 0.15,  0.37) * aspectcorrect) * dofblur);
  col += texture2D(tColor, vUv.xy + (vec2( 0.29,  0.29) * aspectcorrect) * dofblur);
  col += texture2D(tColor, vUv.xy + (vec2(-0.37,  0.15) * aspectcorrect) * dofblur);
  col += texture2D(tColor, vUv.xy + (vec2( 0.40,  0.0 ) * aspectcorrect) * dofblur);
  col += texture2D(tColor, vUv.xy + (vec2( 0.37, -0.15) * aspectcorrect) * dofblur);
  col += texture2D(tColor, vUv.xy + (vec2( 0.29, -0.29) * aspectcorrect) * dofblur);
  col += texture2D(tColor, vUv.xy + (vec2(-0.15, -0.37) * aspectcorrect) * dofblur);
  col += texture2D(tColor, vUv.xy + (vec2( 0.0,  -0.4 ) * aspectcorrect) * dofblur);
  col += texture2D(tColor, vUv.xy + (vec2(-0.15,  0.37) * aspectcorrect) * dofblur);
  col += texture2D(tColor, vUv.xy + (vec2(-0.29,  0.29) * aspectcorrect) * dofblur);
  col += texture2D(tColor, vUv.xy + (vec2( 0.37,  0.15) * aspectcorrect) * dofblur);
  col += texture2D(tColor, vUv.xy + (vec2(-0.4,   0.0 ) * aspectcorrect) * dofblur);
  col += texture2D(tColor, vUv.xy + (vec2(-0.37, -0.15) * aspectcorrect) * dofblur);
  col += texture2D(tColor, vUv.xy + (vec2(-0.29, -0.29) * aspectcorrect) * dofblur);
  col += texture2D(tColor, vUv.xy + (vec2( 0.15, -0.37) * aspectcorrect) * dofblur);

  col += texture2D(tColor, vUv.xy + (vec2( 0.15,  0.37) * aspectcorrect) * dofblur9);
  col += texture2D(tColor, vUv.xy + (vec2(-0.37,  0.15) * aspectcorrect) * dofblur9);
  col += texture2D(tColor, vUv.xy + (vec2( 0.37, -0.15) * aspectcorrect) * dofblur9);
  col += texture2D(tColor, vUv.xy + (vec2(-0.15, -0.37) * aspectcorrect) * dofblur9);
  col += texture2D(tColor, vUv.xy + (vec2(-0.15,  0.37) * aspectcorrect) * dofblur9);
  col += texture2D(tColor, vUv.xy + (vec2( 0.37,  0.15) * aspectcorrect) * dofblur9);
  col += texture2D(tColor, vUv.xy + (vec2(-0.37, -0.15) * aspectcorrect) * dofblur9);
  col += texture2D(tColor, vUv.xy + (vec2( 0.15, -0.37) * aspectcorrect) * dofblur9);

  col += texture2D(tColor, vUv.xy + (vec2( 0.29,  0.29) * aspectcorrect) * dofblur7);
  col += texture2D(tColor, vUv.xy + (vec2( 0.40,  0.0 ) * aspectcorrect) * dofblur7);
  col += texture2D(tColor, vUv.xy + (vec2( 0.29, -0.29) * aspectcorrect) * dofblur7);
  col += texture2D(tColor, vUv.xy + (vec2( 0.0,  -0.4 ) * aspectcorrect) * dofblur7);
  col += texture2D(tColor, vUv.xy + (vec2(-0.29,  0.29) * aspectcorrect) * dofblur7);
  col += texture2D(tColor, vUv.xy + (vec2(-0.4,   0.0 ) * aspectcorrect) * dofblur7);
  col += texture2D(tColor, vUv.xy + (vec2(-0.29, -0.29) * aspectcorrect) * dofblur7);
  col += texture2D(tColor, vUv.xy + (vec2( 0.0,   0.4 ) * aspectcorrect) * dofblur7);

  col += texture2D(tColor, vUv.xy + (vec2( 0.29,  0.29) * aspectcorrect) * dofblur4);
  col += texture2D(tColor, vUv.xy + (vec2( 0.4,   0.0 ) * aspectcorrect) * dofblur4);
  col += texture2D(tColor, vUv.xy + (vec2( 0.29, -0.29) * aspectcorrect) * dofblur4);
  col += texture2D(tColor, vUv.xy + (vec2( 0.0,  -0.4 ) * aspectcorrect) * dofblur4);
  col += texture2D(tColor, vUv.xy + (vec2(-0.29,  0.29) * aspectcorrect) * dofblur4);
  col += texture2D(tColor, vUv.xy + (vec2(-0.4,   0.0 ) * aspectcorrect) * dofblur4);
  col += texture2D(tColor, vUv.xy + (vec2(-0.29, -0.29) * aspectcorrect) * dofblur4);
  col += texture2D(tColor, vUv.xy + (vec2( 0.0,   0.4 ) * aspectcorrect) * dofblur4);

  //return mix(col / 41.0, texture2D(mMap, vUv), abs(dofblur.x) * 0.5);
  //return texture2D(mMap, vUv);
  //return mix(texture2D(tColor, vUv), texture2D(mMap, vUv), clamp(0., 1., abs(dofblur.x) * 20.));
  //return mix(col / 41., texture2D(mMap, vUv), clamp(0., 1., pow(abs(dofblur.x), 0.5)));
  return col / 41.0;
}
*/
