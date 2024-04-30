#version 120

uniform sampler2D tex0; // color (bokeh shape)
varying vec2 aberration;

void main()
{
  vec2 uv = gl_PointCoord;
  gl_FragData[0] = vec4(gl_Color.rgb, 1) *
    (vec4(1., 0., 0., 1.) * texture2D(tex0, uv + aberration).a +
     vec4(0., 1., 0., 1.) * texture2D(tex0, uv).a +
     vec4(0., 0., 1., 1.) * texture2D(tex0, uv - aberration).a);
}
