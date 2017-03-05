// Barre de progression pour le chargement

#define ITER_FULL 64
#define ITER_REFLECT 32
#define ITER_SHADOW 20
uniform float percent;
uniform float ratio;
uniform vec2 invResolution;


// ---------------------------------------------------------------------------
// Structures de données

struct C_Ray
{
  vec3 vOrigin;
  vec3 vDir;
};

struct C_HitInfo
{
  float fDistance;
  float fObjectId;
  vec3 vPos;
};

struct C_Material
{
  vec3 cAlbedo;
  float fR0;
  float fSmoothness;
};

// ---------------------------------------------------------------------------
// Opérateurs logiques

vec2 Union( vec2 v1, vec2 v2 )
{
  return mix(v1, v2, step(v2.x, v1.x));
}

// ---------------------------------------------------------------------------
// Transformations

float PI=3.141592654;

vec3 rotateX(vec3 v, float x)
{
  return vec3(v.x,
	      v.y * cos(x) - v.z * sin(x),
	      v.y * sin(x) + v.z * cos(x));
}

vec3 rotateY(vec3 v, float x)
{
  return vec3(v.x * cos(x) - v.z * sin(x),
	      v.y,
	      v.x * sin(x) + v.z * cos(x));
}

vec3 rotateZ(vec3 v, float x)
{
  return vec3(v.x * cos(x) - v.y * sin(x),
	      v.x * sin(x) + v.y * cos(x),
	      v.z);
}

vec3 DomainRepeatXZGetTile( vec3 vPos, vec2 vRepeat, out vec2 vTile )
{
  vec3 vResult = vPos;
  vec2 vTilePos = (vPos.xz / vRepeat) + 0.5;
  vTile = floor(vTilePos + 1000.0);
  vResult.xz = (fract(vTilePos) - 0.5) * vRepeat;
  return vResult;
}

vec3 DomainRepeatY( vec3 vPos, float fSize )
{
  vec3 vResult = vPos;
  vResult.y = (fract(vPos.y / fSize + 0.5) - 0.5) * fSize;
  return vResult;
}


// ---------------------------------------------------------------------------
// Formes

float GetDistanceSphere(vec3 p, float r)
{
  return length(p) - r;
}

float GetDistanceCylinderY(vec3 p, float r)
{
  return length(p.xz) - r;
}

float GetDistanceCylinderY(vec3 p, float r, float h)
{
  return max(GetDistanceCylinderY(p, r), abs(p.y) - h);
}

float GetDistancePasDeVis(vec3 p, float r)
{
  return length(p.xz) - r * (1. + 0.2 * abs(fract(r * 300.* p.y - atan(p.x, p.z)/(2. * PI)) - 0.5));
}

float GetDistancePoutreZ(vec3 p, float l, float h)
{
  return max(abs(p.x) - l, abs(p.y) - h);
}

float GetDistanceBox(vec3 p, vec3 size)
{
  return max(GetDistancePoutreZ(p, size.x, size.y), abs(p.z) - size.z);
}

float GetDistanceHexaPrism(vec3 p, float l, float h)
{
  float d = GetDistancePoutreZ(p, l, h);
  d = max(d, GetDistancePoutreZ(rotateY(p, PI/3.), l, h));
  return max(d, GetDistancePoutreZ(rotateY(p, 2. * PI/3.), l, h));
}

float GetDistanceEcrouHead(vec3 p, float size)
{
  return GetDistanceHexaPrism(p, size, 0.4 * size);
}

float GetDistanceEcrou(vec3 p, float size)
{
  return max(GetDistanceEcrouHead(p, size),
	     -GetDistancePasDeVis(p, 0.45 * size));
}

float GetDistanceVis(vec3 p, float size, float l)
{
  return min(GetDistanceEcrouHead(p, size),
	     max(GetDistancePasDeVis(p, 0.45 * size),
		 max(-p.y, p.y - l)));
}

float GetDistanceRondelle(vec3 p, float r)
{
  return max(GetDistanceCylinderY(p, r, 0.05 * r),
	     -GetDistanceCylinderY(p, 0.6 * r, r));
}

float GetDistanceObject(vec3 vPos, float object)
{
  float choose = 3. * fract(object / 3.);

  if (choose > 2.)
    return GetDistanceEcrou(vPos, 0.2);
  if (choose > 1.)
    return GetDistanceVis(vPos, 0.2, 0.6);
  return GetDistanceRondelle(vPos, 0.25);
}

float GetDistanceLine(vec3 vPos)
{
  vec3 p = vPos;
  p.xz = fract(p.xz - 1000.) - vec2(0.5);
  float d = GetDistanceObject(p, vPos.x);//max(vPos.x,vPos.z));
  /*
  for (float i = 0.; i < 5.; i += 1.f)
  {
    d = min(d, GetDistanceObject(vPos + vec3(0.5 * i, 0., 0), 3.*fract(i/3.)));
  }
  */
  return d;
}

// ---------------------------------------------------------------------------
// Ray marching

vec2 GetDistanceScene( vec3 vPos )
{
  vec2 vDistFloor = vec2(vPos.y, 1.);
  vec2 vDistChain = vec2(GetDistanceLine(vPos), 2.);
  return Union( vDistFloor, vDistChain );
}

// This is an excellent resource on ray marching -> http://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm
void Raymarch(C_Ray ray,
	      out C_HitInfo result,
	      float maxDist,
	      int maxIter)
{
  float fEpsilon = 0.0001;
  float fStartDistance = 0.001;

  result.fDistance = fStartDistance;
  result.fObjectId = 0.0;

  for (int i = 0; i < maxIter; i++)
  {
    result.vPos = ray.vOrigin + ray.vDir * result.fDistance;
    vec2 vSceneDist = GetDistanceScene( result.vPos );
    result.fObjectId = vSceneDist.y;

    if((vSceneDist.x <= fEpsilon) ||
       (result.fDistance >= maxDist))
    {
      break;
    }

    result.fDistance = result.fDistance + vSceneDist.x;
  }


  if (result.fDistance >= maxDist)
  {
    result.fObjectId = 0.0;
    result.fDistance = 1000.0;
  }
}

// ---------------------------------------------------------------------------
// Éclairage

vec3 GetLightPos()
{
  float pct = min(percent, 1.);
  return vec3(sin(pct), 2., cos(pct));
}

vec3 GetLightCol()
{
  return 10. * vec3(1.0, 0.88, 0.75);
}

vec3 GetSkyGradient( vec3 vDir )
{
  float fBlend = clamp(vDir.y, 0.0, 1.0);
  return mix(vec3(1., 0.94, 0.8), vec3(1., 0.88, 0.75), fBlend);
}

vec3 GetAmbientLight(vec3 vNormal)
{
  return GetSkyGradient(vNormal);
}

float GetDiffuseIntensity(vec3 vLightDir, vec3 vNormal)
{
  return max(0.0, dot(vLightDir, vNormal));
}

float GetBlinnPhongIntensity(C_Ray ray, C_Material mat, vec3 vLightDir, vec3 vNormal)
{
  vec3 vHalf = normalize(vLightDir - ray.vDir);
  float fNdotH = max(0.0, dot(vHalf, vNormal));

  float fSpecPower = exp2(4.0 + 6.0 * mat.fSmoothness);
  float fSpecIntensity = (fSpecPower + 2.0) * 0.125;

  return pow(fNdotH, fSpecPower) * fSpecIntensity;
}

// http://en.wikipedia.org/wiki/Schlick's_approximation
float Schlick( vec3 vNormal, vec3 vView, float fR0, float fSmoothFactor)
{
  float fDot = dot(vNormal, -vView);
  fDot = min(max((1.0 - fDot), 0.0), 1.0);
  float fDot2 = fDot * fDot;
  float fDot5 = fDot2 * fDot2 * fDot;
  return fR0 + (1.0 - fR0) * fDot5 * fSmoothFactor;
}

/*
float GetShadow( vec3 vPos, vec3 vLightDir, float fLightDistance )
{
  C_Ray shadowRay;
  shadowRay.vDir = vLightDir;
  shadowRay.vOrigin = vPos;

  C_HitInfo shadowIntersect;
  Raymarch(shadowRay, shadowIntersect, fLightDistance, ITER_SHADOW);

  return step(0.0, shadowIntersect.fDistance) * step(fLightDistance, shadowIntersect.fDistance );
}
*/

// use distance field to evaluate ambient occlusion
float GetAmbientOcclusion(C_Ray ray, C_HitInfo intersection, vec3 vNormal)
{
  vec3 vPos = intersection.vPos;

  float fAmbientOcclusion = 1.0;

  float fDist = 0.0;
  for(int i=0; i<=2; i++)//2
  {
    fDist += 0.1;

    vec2 vSceneDist = GetDistanceScene(vPos + vNormal * fDist);

    fAmbientOcclusion *= 1.0 - max(0.0, (fDist - vSceneDist.x) * 0.2 / fDist );
  }

  return fAmbientOcclusion;
}

vec3 GetObjectLighting(C_Ray ray, C_HitInfo intersection, C_Material material, vec3 vNormal, vec3 cReflection)
{
  vec3 cScene ;

  vec3 vLightPos = GetLightPos();
  vec3 vToLight = vLightPos - intersection.vPos;
  vec3 vLightDir = normalize(vToLight);
  float fLightDistance = length(vToLight);

  float fAttenuation = 1.0 / (fLightDistance * fLightDistance);

  float fShadowBias = 0.1;
  float fShadowFactor = 1.;//GetShadow( intersection.vPos + vLightDir * fShadowBias, vLightDir, fLightDistance - fShadowBias );
  vec3 vIncidentLight = GetLightCol() * fShadowFactor * fAttenuation;

  vec3 vDiffuseLight = GetDiffuseIntensity( vLightDir, vNormal ) * vIncidentLight;

  float fAmbientOcclusion = GetAmbientOcclusion(ray, intersection, vNormal);
  vec3 vAmbientLight = GetAmbientLight(vNormal) * fAmbientOcclusion;

  vec3 vDiffuseReflection = material.cAlbedo * (vDiffuseLight + vAmbientLight);

  vec3 vSpecularReflection = cReflection * fAmbientOcclusion;

  vSpecularReflection += GetBlinnPhongIntensity( ray, material, vLightDir, vNormal ) * vIncidentLight;

  float fFresnel = Schlick(vNormal, ray.vDir, material.fR0, material.fSmoothness * 0.9 + 0.1);
  cScene = mix(vDiffuseReflection , vSpecularReflection, fFresnel);

  return cScene;
}

// ---------------------------------------------------------------------------

void ApplyAtmosphere(inout vec3 col, C_Ray ray, C_HitInfo intersection)
{
  // fog
  float fFogDensity = 0.025;
  float fFogAmount = exp(intersection.fDistance * -fFogDensity);
  vec3 cFog = GetSkyGradient(ray.vDir);
  col = mix(cFog, col, fFogAmount);

  // glare from light (a bit hacky - use length of closest approach from ray to light)

  vec3 vToLight = GetLightPos() - ray.vOrigin;
  float fDot = dot(vToLight, ray.vDir);
  fDot = clamp(fDot, 0.0, intersection.fDistance);

  vec3 vClosestPoint = ray.vOrigin + ray.vDir * fDot;
  float fDist = length(vClosestPoint - GetLightPos());
  //  col += GetLightCol() * 0.01/ (fDist * fDist);
}

vec3 floorColor(vec3 pos, out vec2 spe )
{
  float x = mod(0.947 * (sin(0.05 * pos.x) + sin(0.8 * pos.z)) +
		0.05 * (sin(1.3 * pos.x + 0.3) + sin(pos.z + 0.7)) +
		0.003 * (sin(10. * pos.x + 0.5) + sin(11. * pos.z + 0.1)), 0.1) * 10.;
  vec3 c1 = vec3(164./255., 133./255., 107./255.);
  vec3 c2 = vec3(211./255., 178./255., 139./255.);

  return mix(c1, c2, x);
}

C_Material GetObjectMaterial( in float fObjId, in vec3 vPos )
{
  C_Material mat;

  if(fObjId < 1.5)
  {
    // floor
    mat.fR0 = 0.02;
    mat.fSmoothness = 0.2;
    vec2 foo;
    mat.cAlbedo = floorColor(vPos, foo);//vec3(0.75, 0.54, 0.34);
  }
  else
  {
    // visserie
    mat.fR0 = 0.5;
    mat.fSmoothness = 0.3;
    mat.cAlbedo = vec3(0.3, 0.3, 0.3);
  }

  return mat;
}

vec3 GetSceneNormal( vec3 vPos )
{
  // tetrahedron normal

  float fDelta = 0.005;

  vec3 vOffset1 = vec3( fDelta, -fDelta, -fDelta);
  vec3 vOffset2 = vec3(-fDelta, -fDelta,  fDelta);
  vec3 vOffset3 = vec3(-fDelta,  fDelta, -fDelta);
  vec3 vOffset4 = vec3( fDelta,  fDelta,  fDelta);

  float f1 = GetDistanceScene( vPos + vOffset1 ).x;
  float f2 = GetDistanceScene( vPos + vOffset2 ).x;
  float f3 = GetDistanceScene( vPos + vOffset3 ).x;
  float f4 = GetDistanceScene( vPos + vOffset4 ).x;

  vec3 vNormal = vOffset1 * f1 + vOffset2 * f2 + vOffset3 * f3 + vOffset4 * f4;

  return normalize( vNormal );
}

vec3 GetSceneColourSimple( C_Ray ray )
{
  C_HitInfo intersection;
  Raymarch(ray, intersection, 5., ITER_REFLECT);

  vec3 cScene;

  if(intersection.fObjectId < 0.5)
  {
    cScene = GetSkyGradient(ray.vDir);
  }
  else
  {
    C_Material material = GetObjectMaterial(intersection.fObjectId, intersection.vPos);
    vec3 vNormal = GetSceneNormal(intersection.vPos);

    // use sky gradient instead of reflection
    vec3 cReflection = GetSkyGradient(reflect(ray.vDir, vNormal));

    // apply lighting
    cScene = GetObjectLighting(ray, intersection, material, vNormal, cReflection );
  }

  ApplyAtmosphere(cScene, ray, intersection);

  return cScene;
}

vec3 GetSceneColour( C_Ray ray )
{
  C_HitInfo intersection;
  Raymarch(ray, intersection, 15.0, ITER_FULL);

  vec3 cScene;

  if(intersection.fObjectId < 0.5)
  {
    cScene = GetSkyGradient(ray.vDir);
  }
  else
  {
    C_Material material = GetObjectMaterial(intersection.fObjectId, intersection.vPos);
    vec3 vNormal = GetSceneNormal(intersection.vPos);

    vec3 cReflection;
    {
      // get colour from reflected ray
      float fSepration = 0.05;
      C_Ray reflectRay;
      reflectRay.vDir = reflect(ray.vDir, vNormal);
      reflectRay.vOrigin = intersection.vPos + reflectRay.vDir * fSepration;

      cReflection = GetSceneColourSimple(reflectRay);
      //      cReflection = vec3(0.);
    }

    // apply lighting
    cScene = GetObjectLighting(ray, intersection, material, vNormal, cReflection );
  }

  ApplyAtmosphere(cScene, ray, intersection);

  return cScene;
}

// ---------------------------------------------------------------------------
// Rendu

void GetCameraRay( vec3 vPos, vec3 vForwards, vec3 vWorldUp, out C_Ray ray)
{
  vec2 vUV = gl_FragCoord.xy * invResolution;
  vec2 vViewCoord = 0.5*(vUV * 2.0 - 1.0);

  vViewCoord.y /= ratio;

  ray.vOrigin = vPos;

  vec3 vRight = normalize(cross(vForwards, vWorldUp));
  vec3 vUp = cross(vRight, vForwards);

  ray.vDir = normalize( vRight * vViewCoord.x + vUp * vViewCoord.y + vForwards);
}

void GetCameraRayLookat( vec3 vPos, vec3 vInterest, out C_Ray ray)
{
  vec3 vForwards = normalize(vInterest - vPos);
  vec3 vUp = vec3(0.0, 1.0, 0.0);

  GetCameraRay(vPos, vForwards, vUp, ray);
}

vec3 OrbitPoint( float fHeading, float fElevation )
{
  return vec3(sin(fHeading) * cos(fElevation), sin(fElevation), cos(fHeading) * cos(fElevation));
}

vec3 Tonemap( vec3 cCol )
{
  // simple Reinhard tonemapping operator
  return cCol / (1.0 + cCol);
}

vec4 progress(vec2 uv, float percent)
{
  float marginLeft = 0.3;
  float distCent = 0.5 - marginLeft;

  vec2 p = uv - vec2(0.5, 0.5);
  float barre = float(abs(p.x) < distCent && abs(p.y) < 0.03);

  float left = mix(marginLeft, 1. - marginLeft, min(percent, 1.));

  return vec4(vec3(step(uv.x, left)), 0.25 * barre);
}

void main()
{
  C_Ray ray;

  GetCameraRayLookat(vec3(-1.3, 4.0, -5.0), vec3(0.), ray);

  vec3 cScene = GetSceneColour( ray );

  float fExposure = 1.2;
  vec3 color = Tonemap(cScene * fExposure);

  vec2 vUV = gl_FragCoord.xy * invResolution;
  vec4 circ = progress(vUV, percent);
  color = mix(color, circ.xyz, circ.a);

  color *= smoothstep(1., 0., max(0., percent - 1.)); // fondu de fin
  gl_FragColor = vec4(color, 1.);
}
