#include "RevolutionFunctions.hh"
#include "Revolution.hh"
#include "Mesh.hh"
#include "engine/algebra/Functions.hh"
#include "engine/algebra/Interpolation.hxx"
#include "engine/algebra/Spline.hh"
#include "engine/algebra/Utils.hh"
#include "engine/algebra/Vector3.hxx"
#include "engine/container/Array.hxx"

#if DEBUG
#include <stdio.h>
#endif

using namespace Algebra;

namespace mesh
{
	// --------------------------------------------------------------------------
	// Calcul de position des points

	// Cylindre débouché (in : version intérieure)
	vector3f pipeHFunc(float t, float)
	{
		vector3f result = { 0, t, 0 };
		return result;
	}
	vector3f inPipeHFunc(float t, float)
	{
		vector3f result = { 0, 1.f - t, 0 };
		return result;
	}
	float pipeRFunc(float, float)
	{
		return 1.f;
	}

	// Roue
	vector3f wheelHFunc(float t, float)
	{
		if (t <= 0.25f)
		{
			vector3f result = { 0, -0.5f, 0 };
			return result;
		}
		else if (t >= 0.75f)
		{
			vector3f result = { 0, 0.5f, 0 };
			return result;
		}
		else
		{
			const float alpha = PI * (2.f * t - 0.5f);
			vector3f result = { 0, -0.5f * msys_cosf(alpha), 0 };
			return result;
		}
	}
	float wheelRFunc(float t, float)
	{
		if (t <= 0.25f)
		{
			return 0.0001f + 3.2f * t;
		}
		else if (t >= 0.75f)
		{
			return 0.0001f + 3.2f * (1.f - t);
		}
		else
		{
			const float alpha = PI * (2.f * t - 0.5f);
			return 0.8f + 0.2f * Algebra::pow(msys_sinf(alpha), 0.7f);
		}
	}

	// Sphère
	vector3f sphereHFunc(float t, float)
	{
		const float lat = PI * (t - 0.5f);
		vector3f result = { 0.f, msys_sinf(lat), 0.f };
		return result;
	}
	float sphereRFunc(float t, float)
	{
		const float lat = PI * (t - 0.5f);
		return msys_cosf(lat) + 0.000001f;
	}

	// Hemisphère
	vector3f hemisphereHFunc(float t, float)
	{
		const float lat = 0.5f * PI * t;
		vector3f result = { 0.f, msys_sinf(lat), 0.f };
		return result;
	}
	float hemisphereRFunc(float t, float)
	{
		const float lat = 0.5f * PI * t;
		return msys_cosf(lat) + 0.000001f;
	}

	// Tore
	vector3f torusHFunc(float t, float)
	{
		const float lat = 2.f * PI * (t - 0.5f);
		vector3f result = { 0.f, 0.1f * msys_sinf(lat), 0.f };
		return result;
	}
	float torusRFunc(float t, float)
	{
		const float lat = 2.f * PI * (t - 0.5f);
		return 0.8f + 0.1f * msys_cosf(lat);
	}

#if 0

	// Truc bizarre
	vector3f trucHFunc(float t, float) { return vector3f(0.f, t, 0.f); }
	float trucRFunc(float t, float)
	{
		return 0.9f + 0.1f * msys_sinf(40.f * t);
	}

	// Pneu
	vector3f tireHFunc(float t, float)
	{
		const float lat = 2.f * PI * (t - 0.5f);
		return vector3f(0.f, 0.5f * msys_sinf(lat), 0.f);
	}
	float tireRFunc(float t, float)
	{
		const float lat = 2.f * PI * (t - 0.5f);
		const float cos = msys_cosf(lat);
		const float abs = 0.0001f + Algebra::abs(cos);
		const float sign = 1.f - 2.f * (cos < 0);
		return 1.f + 0.35f * sign * msys_powf(abs, 0.5f);
	}

	// Écrou
	vector3f ecrouHFunc(float t, float)
	{
		float y;
		if (6.f * t <= 3.f)
			y = 3.f * t;
		else if (6.f * t <= 4.f)
			y = 1.f;
		else if (6.f * t <= 5.f)
			y = 5.f - 6.f * t;
		else y = 0;
		return vector3f(0.f, y, 0.f);
	}
	float ecrouRFunc(float, float) { return 1.f; }

	// Colonne
	vector3f columnHFunc(float t, float) { return vector3f(0.f, t, 0.f); }
	float columnRFunc(float t, float theta)
	{
		if (t < 0.05 || t > 0.95)
			return 1.f;
		if (t < 0.1 || t > 0.9)
			return 0.9f;

		const float sin = msys_sinf(10.f * theta);
		const float curve = 1.f - msys_powf((t - 0.1f)/0.4f - 1.f, 10.f);
		return 0.8f - 0.1f * curve * (sin > 0 ? msys_powf(sin, 0.2f) : 0);
	}

	// Square revolution
	float square(float theta)
	{
		float a = msys_fmodf((theta + PI / 4), PI / 2) - PI / 4;
		return 1.f / cos(a);
	}
#endif

#if DEBUG
	// Code pour charger un mesh de révolution à partir d'une spline dans un fichier
	static Container::Array<float> splineData(1000);

	static vector3f splineHFunc(float t, float)
	{
		float ret[2];
		spline(splineData.elt, splineData.size / 3, 2, t, ret);
		vector3f result = { 0.f, ret[0] / 100.f + 0.001f, 0.f };
		return result;
	}
	static float splineRFunc(float t, float)
	{
		float ret[2];
		spline(splineData.elt, splineData.size / 3, 2, t, ret);
		return ret[1] / 100.f + 0.001f;
	}

	Revolution loadSplineFromFile(const char *file)
	{
		FILE * fd = fopen(file, "r");
		assert(fd != NULL);

		char buffer[2000];
		splineData.empty();
		while (fgets(buffer, sizeof (buffer), fd) != NULL)
		{
			int f[3];
			int result = sscanf(buffer, " %d , %d , %d ,", &f[0], &f[1], &f[2]);
			if (result == 3)
			{
				splineData.add((float) f[0]);
				splineData.add((float) f[1]);
				splineData.add((float) f[2]);
			}
		}

		// Calcul de t
		for (int i = 0; i < splineData.size; i += 3)
		{
			splineData[i] = (float)i / (splineData.size - 3);
		}

		return Revolution(splineHFunc, splineRFunc);
	}

#endif // DEBUG
}
