uniform mat4 oldModelView;

varying vec3 vSpeed;

//
// Représentation de la vitesse en un sommet
//
// - direction en xy,
// - intensité en z (avec une correction type gamma pour avoir de la
//   précision)
//
vec3 getSpeed()
{
  vec4 oldScreenCoord = gl_ProjectionMatrix * oldModelView * gl_Vertex;
  vec4 newScreenCoord = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
  vec2 speed = newScreenCoord.xy / newScreenCoord.w - oldScreenCoord.xy / oldScreenCoord.w;
  float norm = length(speed);
  return vec3(normalize(speed), norm);
}
