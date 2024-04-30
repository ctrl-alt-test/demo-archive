/*============================================================================


                    NVIDIA FXAA 3.11 by TIMOTHY LOTTES


------------------------------------------------------------------------------
COPYRIGHT (C) 2010, 2011 NVIDIA CORPORATION. ALL RIGHTS RESERVED.
------------------------------------------------------------------------------

============================================================================*/

/*============================================================================
                     FXAA QUALITY - LOW DITHER PRESETS
============================================================================*/
const float qps[12] = float[12](1.0, 1.5, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 4.0, 8.0);

/*============================================================================
                     FXAA QUALITY - EXTREME QUALITY
============================================================================*/
//const float qps[12] = float[12](1.0, 1.0, 1.0, 1.0, 1.0, 1.5, 2.0, 2.0, 2.0, 2.0, 4.0, 8.0);

float Luma2(vec4 rgba) { return rgba.w; }

vec4 FXAA_WM(
    //
    // Use noperspective interpolation here (turn off perspective interpolation).
    // {xy} = center of pixel
    vec2 pos,
    //
    // Input color texture.
    // {rgb_} = color in linear or perceptual color space
    // {___a} = luma in perceptual color space (not linear)
    sampler2D tex,
    //
    // Only used on FXAA Quality.
    // This must be from a constant/uniform.
    // {x_} = 1.0/screenWidthInPixels
    // {_y} = 1.0/screenHeightInPixels
    vec2 RcpFrame,
    //
    // Only used on FXAA Quality.
    // This used to be the Q_SUBPIX define.
    // It is here now to allow easier tuning.
    // Choose the amount of sub-pixel aliasing removal.
    // This can effect sharpness.
    //   1.00 - upper limit (softer)
    //   0.75 - default amount of filtering
    //   0.50 - lower limit (sharper, less sub-pixel aliasing removal)
    //   0.25 - almost off
    //   0.00 - completely off
    float Subpix,
    //
    // Only used on FXAA Quality.
    // This used to be the Q_EDGE_THRESHOLD define.
    // It is here now to allow easier tuning.
    // The minimum amount of local contrast required to apply algorithm.
    //   0.333 - too little (faster)
    //   0.250 - low quality
    //   0.166 - default
    //   0.125 - high quality
    //   0.063 - overkill (slower)
    float EdgeThreshold,
    //
    // Only used on FXAA Quality.
    // This used to be the Q_EDGE_THRESHOLD_MIN define.
    // It is here now to allow easier tuning.
    // Trims the algorithm from processing darks.
    //   0.0833 - upper limit (default, the start of visible unfiltered edges)
    //   0.0625 - high quality (faster)
    //   0.0312 - visible limit (slower)
    float EdgeThresholdMin
) {
/*--------------------------------------------------------------------------*/
    vec2 posM = pos;
    vec4 rgbyM = textureLod(tex, posM, 0.);
    float lumaM = rgbyM.w;
    float lumaS = Luma2(textureLodOffset(tex, posM, 0., ivec2( 0, 1)));
    float lumaE = Luma2(textureLodOffset(tex, posM, 0., ivec2( 1, 0)));
    float lumaN = Luma2(textureLodOffset(tex, posM, 0., ivec2( 0,-1)));
    float lumaW = Luma2(textureLodOffset(tex, posM, 0., ivec2(-1, 0)));
/*--------------------------------------------------------------------------*/
    float maxSM = max(lumaS, lumaM);
    float minSM = min(lumaS, lumaM);
    float maxESM = max(lumaE, maxSM);
    float minESM = min(lumaE, minSM);
    float maxWN = max(lumaN, lumaW);
    float minWN = min(lumaN, lumaW);
    float rangeMax = max(maxWN, maxESM);
    float rangeMin = min(minWN, minESM);
    float rangeMaxScaled = rangeMax * EdgeThreshold;
    float range = rangeMax - rangeMin;
    float rangeMaxClamped = max(EdgeThresholdMin, rangeMaxScaled);
    bool earlyExit = range < rangeMaxClamped;
/*--------------------------------------------------------------------------*/
    if(earlyExit)
        return rgbyM;
/*--------------------------------------------------------------------------*/
    float lumaNW = Luma2(textureLodOffset(tex, posM, 0., ivec2(-1,-1)));
    float lumaSE = Luma2(textureLodOffset(tex, posM, 0., ivec2( 1, 1)));
    float lumaNE = Luma2(textureLodOffset(tex, posM, 0., ivec2( 1,-1)));
    float lumaSW = Luma2(textureLodOffset(tex, posM, 0., ivec2(-1, 1)));
/*--------------------------------------------------------------------------*/
    float lumaNS = lumaN + lumaS;
    float lumaWE = lumaW + lumaE;
    float subpixRcpRange = 1.0/range;
    float subpixNSWE = lumaNS + lumaWE;
    float edgeHorz1 = (-2.0 * lumaM) + lumaNS;
    float edgeVert1 = (-2.0 * lumaM) + lumaWE;
/*--------------------------------------------------------------------------*/
    float lumaNESE = lumaNE + lumaSE;
    float lumaNWNE = lumaNW + lumaNE;
    float edgeHorz2 = (-2.0 * lumaE) + lumaNESE;
    float edgeVert2 = (-2.0 * lumaN) + lumaNWNE;
/*--------------------------------------------------------------------------*/
    float lumaNWSW = lumaNW + lumaSW;
    float lumaSWSE = lumaSW + lumaSE;
    float edgeHorz4 = (abs(edgeHorz1) * 2.0) + abs(edgeHorz2);
    float edgeVert4 = (abs(edgeVert1) * 2.0) + abs(edgeVert2);
    float edgeHorz3 = (-2.0 * lumaW) + lumaNWSW;
    float edgeVert3 = (-2.0 * lumaS) + lumaSWSE;
    float edgeHorz = abs(edgeHorz3) + edgeHorz4;
    float edgeVert = abs(edgeVert3) + edgeVert4;
/*--------------------------------------------------------------------------*/
    float subpixNWSWNESE = lumaNWSW + lumaNESE;
    float lengthSign = RcpFrame.x;
    bool horzSpan = edgeHorz >= edgeVert;
    float subpixA = subpixNSWE * 2.0 + subpixNWSWNESE;
/*--------------------------------------------------------------------------*/
    if(!horzSpan) lumaN = lumaW;
    if(!horzSpan) lumaS = lumaE;
    if(horzSpan) lengthSign = RcpFrame.y;
    float subpixB = (subpixA * (1.0/12.0)) - lumaM;
/*--------------------------------------------------------------------------*/
    float gradientN = lumaN - lumaM;
    float gradientS = lumaS - lumaM;
    float lumaNN = 0.5 * (lumaN + lumaM);
    float lumaSS = 0.5 * (lumaS + lumaM);
    bool pairN = abs(gradientN) >= abs(gradientS);
    float gradient = max(abs(gradientN), abs(gradientS));
    if(pairN) lengthSign = -lengthSign;
    float subpixC = clamp(abs(subpixB) * subpixRcpRange, 0., 1.);
/*--------------------------------------------------------------------------*/
    vec2 offNP;
    offNP.x = (!horzSpan) ? 0.0 : RcpFrame.x;
    offNP.y = ( horzSpan) ? 0.0 : RcpFrame.y;
    if(!horzSpan) pos.x += lengthSign * 0.5;
    if( horzSpan) pos.y += lengthSign * 0.5;
/*--------------------------------------------------------------------------*/
    vec2 posN = pos;
    vec2 posP = pos;
    posN -= offNP * qps[0];
    posP += offNP * qps[0];
    float subpixD = ((-2.0)*subpixC) + 3.0;
    float subpixE = subpixC * subpixC;
/*--------------------------------------------------------------------------*/
    if(!pairN) lumaNN = lumaSS;
    float gradientScaled = gradient * 1.0/4.0;
    float lumaMM = lumaM - lumaNN;
    float subpixF = subpixD * subpixE;
    bool lumaMLTZero = lumaMM < 0.0;
/*--------------------------------------------------------------------------*/
    float lumaEndN;
    float lumaEndP;
    bool doneN = false;
    bool doneP = false;
    for (int i = 1; i < 12; ++i)
    {
      if (doneN && doneP) break;
      if(!doneN) lumaEndN = Luma2(textureLod(tex, posN, 0.)) - lumaNN;
      if(!doneP) lumaEndP = Luma2(textureLod(tex, posP, 0.)) - lumaNN;
      doneN = abs(lumaEndN) >= gradientScaled;
      doneP = abs(lumaEndP) >= gradientScaled;
      if(!doneN) posN -= offNP * qps[i];
      if(!doneP) posP += offNP * qps[i];
    }
    float dstN = posM.x - posN.x;
    float dstP = posP.x - posM.x;
    if(!horzSpan) dstN = posM.y - posN.y;
    if(!horzSpan) dstP = posP.y - posM.y;
/*--------------------------------------------------------------------------*/
    bool goodSpanN = (lumaEndN < 0.0) != lumaMLTZero;
    float spanLength = (dstP + dstN);
    bool goodSpanP = (lumaEndP < 0.0) != lumaMLTZero;
    float spanLengthRcp = 1.0/spanLength;
/*--------------------------------------------------------------------------*/
    bool directionN = dstN < dstP;
    float dst = min(dstN, dstP);
    bool goodSpan = directionN ? goodSpanN : goodSpanP;
    float subpixG = subpixF * subpixF;
    float pixelOffset = (dst * (-spanLengthRcp)) + 0.5;
    float subpixH = subpixG * Subpix;
/*--------------------------------------------------------------------------*/
    float pixelOffsetGood = goodSpan ? pixelOffset : 0.0;
    float pixelOffsetSubpix = max(pixelOffsetGood, subpixH);
    if(!horzSpan) posM.x += pixelOffsetSubpix * lengthSign;
    if( horzSpan) posM.y += pixelOffsetSubpix * lengthSign;
    return vec4(textureLod(tex, posM, 0.).xyz, lumaM);
}
