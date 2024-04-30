#version 120

// Barre de progression pour le chargement

uniform float percent;
uniform float ratio;
uniform vec2 invResolution;

bool rect(vec2 bottomLeft, vec2 topRight, vec2 point)
{
  return point.x > bottomLeft.x && point.x < topRight.x &&
         point.y > bottomLeft.y && point.y < topRight.y;
}

void main()
{
  vec2 vUV = gl_FragCoord.xy * invResolution;
  float width = 0.8;
  float height = 0.008;
  float p = min(1., percent) * width;
  float r = float(rect(vec2(0.5-width*0.5 + p*0.5, 0.5 - height*0.5),
                       vec2(0.5+width*0.5 - p*0.5, 0.5 + height*0.5), vUV));

  vec3 eigengrau = vec3(0.09, 0.09, 0.11);
  vec3 lvlColor = vec3(1.00, 0.80, 0.18);
  vec3 color = mix(eigengrau, lvlColor, r);
  color *= smoothstep(0., 1., min(1., 2. - percent)); // fondu de fin
  gl_FragColor = vec4(color, 1.);
}
