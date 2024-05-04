#pragma once

// This file is ugly. We could find a cleaner solution (e.g. let this job to
// Shader Minifier). But the deadline is in 36 hours, and we have other
// concerns for now. :)

#include "engine/core/FileIO.hh"

#if DEBUG
#define UNIFORM_NAME(shortName,longName) longName
#else // !DEBUG
#define UNIFORM_NAME(shortName,longName) shortName
#endif // !DEBUG

// List maintained by hand.
#define U_ABOVEWATER						UNIFORM_NAME("_a", "aboveWater")
#define U_AMBIENTLIGHT						UNIFORM_NAME("_b", "ambientLight")
#define U_AMPLITUDE							UNIFORM_NAME("_c", "amplitude")
#define U_BLOOMCOLOR						UNIFORM_NAME("_d", "bloomColor")
#define U_CAMERAPOSITION					UNIFORM_NAME("_e", "cameraPosition")
#define U_CENTER							UNIFORM_NAME("_f", "center")
#define U_CLIPPLANE							UNIFORM_NAME("_g", "clipPlane")
#define U_COMPUTEBUFFERSIZE					UNIFORM_NAME("_h", "computeBufferSize")
#define U_DEPTHMAP							UNIFORM_NAME("_i", "depthMap")
#define U_DIFFUSECOLOR						UNIFORM_NAME("_j", "diffuseColor")
#define U_DIFFUSEENVMAP						UNIFORM_NAME("_k", "diffuseEnvMap")
#define U_DIFFUSETEXTURE					UNIFORM_NAME("_l", "diffuseTexture")
#define U_DIRECTION							UNIFORM_NAME("_m", "direction")
#define U_EMISSIVECOLOR						UNIFORM_NAME("_n", "emissiveColor")
#define U_EMISSIVEMULTIPLIER				UNIFORM_NAME("_o", "emissiveMultiplier")
#define U_EMISSIVETEXTURE					UNIFORM_NAME("_p", "emissiveTexture")
#define U_EXPOSURE							UNIFORM_NAME("_q", "exposure")
#define U_FOGCOLOR							UNIFORM_NAME("_r", "fogColor")
#define U_FORWARDSCATTERINGSTRENGTH			UNIFORM_NAME("_s", "forwardScatteringStrength")
#define U_GAIN								UNIFORM_NAME("_t", "gain")
#define U_GAMMA								UNIFORM_NAME("_u", "gamma")
#define U_HEIGHT							UNIFORM_NAME("_v", "height")
#define U_INIT								UNIFORM_NAME("_w", "init")
#define U_INVERSEMODELVIEWPROJECTIONMATRIX	UNIFORM_NAME("_x", "inverseModelViewProjectionMatrix")
#define U_INVERSEVIEWPROJECTIONMATRIX		UNIFORM_NAME("_y", "inverseViewProjectionMatrix")
#define U_INVRESOLUTION						UNIFORM_NAME("_z", "invResolution")
#define U_LIFT								UNIFORM_NAME("_A", "lift")
#define U_LIGHTCOLORS						UNIFORM_NAME("_B", "lightColors")
#define U_LIGHTCONECOS						UNIFORM_NAME("_C", "lightConeCos")
#define U_LIGHTDIRECTIONS					UNIFORM_NAME("_D", "lightDirections")
#define U_LIGHTPOSITIONS					UNIFORM_NAME("_E", "lightPositions")
#define U_LIGHTTYPES						UNIFORM_NAME("_F", "lightTypes")
#define U_MARCHINGSTEPS						UNIFORM_NAME("_G", "marchingSteps")
#define U_MEDIUMCOLOR						UNIFORM_NAME("_H", "mediumColor")
#define U_MEDIUMDENSITY						UNIFORM_NAME("_I", "mediumDensity")
#define U_MODELLIGHTPROJECTIONMATRIX		UNIFORM_NAME("_J", "modelLightProjectionMatrix")
#define U_MODELMATRIX						UNIFORM_NAME("_K", "modelMatrix")
#define U_MODELVIEWMATRIX					UNIFORM_NAME("_L", "modelViewMatrix")
#define U_MODELVIEWPROJECTIONMATRIX			UNIFORM_NAME("_M", "modelViewProjectionMatrix")
#define U_NORMALANDHEIGHTTEXTURE			UNIFORM_NAME("_N", "normalAndHeightTexture")
#define U_NORMALTEXTURE						UNIFORM_NAME("_O", "normalTexture")
#define U_OPACITY							UNIFORM_NAME("_P", "opacity")
#define U_PARALLAXSCALE						UNIFORM_NAME("_Q", "parallaxScale")
#define U_PARTICLESIZE						UNIFORM_NAME("_R", "particleSize")
#define U_PASS								UNIFORM_NAME("_S", "pass")
#define U_PROGRESS							UNIFORM_NAME("_T", "progress")
#define U_PROJECTIONMATRIX					UNIFORM_NAME("_U", "projectionMatrix")
#define U_RANDOMTEXTURE						UNIFORM_NAME("_V", "randomTexture")
#define U_REFLECTION						UNIFORM_NAME("_W", "reflection")
#define U_REFLECTIONDEPTH					UNIFORM_NAME("_X", "reflectionDepth")
#define U_REFRACTION						UNIFORM_NAME("_Y", "refraction")
#define U_REFRACTIONDEPTH					UNIFORM_NAME("_Z", "refractionDepth")
#define U_RESOLUTION						UNIFORM_NAME("a_", "resolution")
#define U_ROUGHNESS							UNIFORM_NAME("b_", "roughness")
#define U_SATURATION						UNIFORM_NAME("c_", "saturation")
#define U_SCATTERINGINTENSITY				UNIFORM_NAME("d_", "scatteringIntensity")
#define U_SHADOWMAPS						UNIFORM_NAME("e_", "shadowMaps")
#define U_SHARPNESS							UNIFORM_NAME("f_", "sharpness")
#define U_SKYCOLOR							UNIFORM_NAME("g_", "skyColor")
#define U_SPECULARANDROUGHNESSTEXTURE		UNIFORM_NAME("h_", "specularAndRoughnessTexture")
// Skip i_ deliberately, because it has a special meaning for ShaderMinifier.
#define U_SPECULARCOLOR						UNIFORM_NAME("j_", "specularColor")
#define U_SPECULARENVMAP					UNIFORM_NAME("k_", "specularEnvMap")
#define U_SPECULARTEXTURE					UNIFORM_NAME("l_", "specularTexture")
#define U_SPEED								UNIFORM_NAME("m_", "speed")
#define U_SPLASHINTENSITY					UNIFORM_NAME("n_", "splashIntensity")
#define U_SPLASHRADIUS						UNIFORM_NAME("o_", "splashRadius")
#define U_STREAKCOLOR						UNIFORM_NAME("p_", "streakColor")
#define U_STREAKPOWER						UNIFORM_NAME("q_", "streakPower")
#define U_SUBMERSIBLEDEPTH					UNIFORM_NAME("r_", "submersibleDepth")
#define U_TEXTURE0							UNIFORM_NAME("s_", "texture0")
#define U_TEXTURE1							UNIFORM_NAME("t_", "texture1")
#define U_TEXTURE2							UNIFORM_NAME("u_", "texture2")
#define U_TEXTURE3							UNIFORM_NAME("v_", "texture3")
#define U_TEXTURE4							UNIFORM_NAME("w_", "texture4")
#define U_TEXTURE5							UNIFORM_NAME("x_", "texture5")
#define U_TEXTURE6							UNIFORM_NAME("y_", "texture6")
#define U_TEXTURE7							UNIFORM_NAME("z_", "texture7")
#define U_TIME								UNIFORM_NAME("A_", "time")
#define U_VERTEXCOLOR						UNIFORM_NAME("B_", "vertexColor")
#define U_VERTEXNORMAL						UNIFORM_NAME("C_", "vertexNormal")
#define U_VERTEXPOSITION					UNIFORM_NAME("D_", "vertexPosition")
#define U_VERTEXTANGENT						UNIFORM_NAME("E_", "vertexTangent")
#define U_VERTEXTEXCOORD					UNIFORM_NAME("F_", "vertexTexCoord")
#define U_VIEWMATRIX						UNIFORM_NAME("G_", "viewMatrix")
#define U_VIGNETTING						UNIFORM_NAME("H_", "vignetting")
#define U_VOLUMESIZE						UNIFORM_NAME("I_", "volumeSize")
#define U_WAVEPARAMETERS					UNIFORM_NAME("J_", "waveParameters")
#define U_ZFAR								UNIFORM_NAME("K_", "zFar")
#define U_ZNEAR								UNIFORM_NAME("L_", "zNear")

#if DEBUG
static Core::ShaderReplacements getShaderReplacements()
{
	Core::ShaderReplacements r;
	r["_a"] = "aboveWater";
	r["_b"] = "ambientLight";
	r["_c"] = "amplitude";
	r["_d"] = "bloomColor";
	r["_e"] = "cameraPosition";
	r["_f"] = "center";
	r["_g"] = "clipPlane";
	r["_h"] = "computeBufferSize";
	r["_i"] = "depthMap";
	r["_j"] = "diffuseColor";
	r["_k"] = "diffuseEnvMap";
	r["_l"] = "diffuseTexture";
	r["_m"] = "direction";
	r["_n"] = "emissiveColor";
	r["_o"] = "emissiveMultiplier";
	r["_p"] = "emissiveTexture";
	r["_q"] = "exposure";
	r["_r"] = "fogColor";
	r["_s"] = "forwardScatteringStrength";
	r["_t"] = "gain";
	r["_u"] = "gamma";
	r["_v"] = "height";
	r["_w"] = "init";
	r["_x"] = "inverseModelViewProjectionMatrix";
	r["_y"] = "inverseViewProjectionMatrix";
	r["_z"] = "invResolution";
	r["_A"] = "lift";
	r["_B"] = "lightColors";
	r["_C"] = "lightConeCos";
	r["_D"] = "lightDirections";
	r["_E"] = "lightPositions";
	r["_F"] = "lightTypes";
	r["_G"] = "marchingSteps";
	r["_H"] = "mediumColor";
	r["_I"] = "mediumDensity";
	r["_J"] = "modelLightProjectionMatrix";
	r["_K"] = "modelMatrix";
	r["_L"] = "modelViewMatrix";
	r["_M"] = "modelViewProjectionMatrix";
	r["_N"] = "normalAndHeightTexture";
	r["_O"] = "normalTexture";
	r["_P"] = "opacity";
	r["_Q"] = "parallaxScale";
	r["_R"] = "particleSize";
	r["_S"] = "pass";
	r["_T"] = "progress";
	r["_U"] = "projectionMatrix";
	r["_V"] = "randomTexture";
	r["_W"] = "reflection";
	r["_X"] = "reflectionDepth";
	r["_Y"] = "refraction";
	r["_Z"] = "refractionDepth";
	r["a_"] = "resolution";
	r["b_"] = "roughness";
	r["c_"] = "saturation";
	r["d_"] = "scatteringIntensity";
	r["e_"] = "shadowMaps";
	r["f_"] = "sharpness";
	r["g_"] = "skyColor";
	r["h_"] = "specularAndRoughnessTexture";
	// See remark above regarding i_.
	r["j_"] = "specularColor";
	r["k_"] = "specularEnvMap";
	r["l_"] = "specularTexture";
	r["m_"] = "speed";
	r["n_"] = "splashIntensity";
	r["o_"] = "splashRadius";
	r["p_"] = "streakColor";
	r["q_"] = "streakPower";
	r["r_"] = "submersibleDepth";
	r["s_"] = "texture0";
	r["t_"] = "texture1";
	r["u_"] = "texture2";
	r["v_"] = "texture3";
	r["w_"] = "texture4";
	r["x_"] = "texture5";
	r["y_"] = "texture6";
	r["z_"] = "texture7";
	r["A_"] = "time";
	r["B_"] = "vertexColor";
	r["C_"] = "vertexNormal";
	r["D_"] = "vertexPosition";
	r["E_"] = "vertexTangent";
	r["F_"] = "vertexTexCoord";
	r["G_"] = "viewMatrix";
	r["H_"] = "vignetting";
	r["I_"] = "volumeSize";
	r["J_"] = "waveParameters";
	r["K_"] = "zFar";
	r["L_"] = "zNear";

	// Let's rename in/out variables. This code is certified handmade.
	r["ZA"] = "clipSpacePosition";
	r["ZB"] = "fragmentColor";
	r["ZC"] = "lightVecs";
	r["ZD"] = "normal";
	r["ZE"] = "objectSpacePosition";
	r["ZF"] = "shadowCoord";
	r["ZG"] = "tangent";
	r["ZH"] = "texCoord";
	r["ZI"] = "viewSpaceNormal";
	r["ZJ"] = "viewSpacePosition";
	r["ZK"] = "viewSpaceTangent";
	r["ZL"] = "viewVec";
	r["ZM"] = "worldSpacePosition";
	return r;
}
#endif
