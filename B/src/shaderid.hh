//
// Identifiants de tous les shaders utilisés
//

#ifndef		SHADER_ID_HH
# define	SHADER_ID_HH

namespace Shader
{
  typedef enum
    {
      color = 0, // HAS TO BE FIRST

      none,
      default,
      sphere,
      grass,
      bump,
      pixel,
      texture,
      ramp,
      conway,
      tree,
      rope,
      pharmacy,
      sodium,
      building,

      glow,
      postProcess,
      godRays,
      text,

      numberOfShaders
    } id;

  typedef enum
    {
      objectId,
      colorMap,   // Couleur
      sndColorMap,// Couleur secondaire
      normalMap,  // bump
      specularMap,// specular
      dof,        // profondeur de champ, entre 0. et 1.
      fade,       // permet de faire un fondu
      fadeLuminance, // fondu noir ou fondu blanc ?
      trans,      // transition par texture
      textureFade,// Poids entre couleur et couleur secondaire
      glowFlash,  // valeur de glow min
      screenCX,   // screen center
      screenCY,
      time,
      numberOfUniforms
    } uniforms;
}

#endif		// SHADER_ID_HH
