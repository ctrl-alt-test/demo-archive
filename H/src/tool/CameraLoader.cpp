#include "CameraLoader.hh"

#include "engine/algebra/Functions.hh"
#include "engine/algebra/Utils.hh"
#include "engine/algebra/Vector3.hh"
#include "engine/container/Dict.hxx"
#include "engine/core/Debug.hh"
#include "engine/core/msys_temp.hh"
#include "engine/core/FileIO.hh"
#include "engine/core/StringUtils.hh"
#include "engine/timeline/Anim.hxx"
#include "engine/timeline/Timeline.hh"
#include "rapidjson/document.h"
#include "rapidjson/error/en.h"
#include "tool/IncludeUtils.hh"

#include <cassert>

#if CAMERA_EDIT

using namespace Algebra;

static const char* findScene(SceneDict& scenes, Timeline::IScene* scene)
{
	for (auto it = scenes.begin(); it != scenes.end(); it++)
	{
		if (it->second == scene)
			return it->first.c_str();
	}
	LOG_ERROR("Scene not found in SceneDict.");
	return nullptr;
}

float smallf(float f)
{
	// bigger number need more precision
	if (f > 10.f)
		return roundb(f, 18);
	else if (f > 100.f) // bigger number need more precision
		return roundb(f, 20);
	else
		return roundb(f, 16);
}

static void exportTimelineToCpp(Timeline::Timeline& timeline, SceneDict& scenes)
{
	const char* filename = "src/" CONCATENATE_PATH(PROJECT_DIRECTORY,exported_camera.hh);
	FILE* fp = fopen(filename, "w");
	if (fp == nullptr) return;

	fprintf(fp, "// File generated\n// by %s\n// in %s:%d\n", __FUNCTION__, __FILE__, __LINE__);
	fprintf(fp, "#pragma once\n\n");

	Container::Array<int> dates(10000);
	Container::Array<float> fov(1000);
	Container::Array<Algebra::quaternion> quat(1000);
	Container::Array<Algebra::vector3f> pos(1000);
	char* animsStr = new char[64 * 1024];
	char* str = animsStr;
	int lastDate = 0; // used for delta encoding dates

	for (int i = 0; i < timeline.GetShots().size; i++)
	{
		Timeline::Shot& shot = timeline.GetShots()[i];

		str += sprintf(str, "static Timeline::Anim<float> animFov%d(%d, dates + %d, fov + %d);\n",
			i, shot.fov->GetDates().size, dates.size, fov.size);
		lastDate = 0;
		for (int j = 0; j < shot.fov->GetDates().size; j++)
		{
			dates.add(shot.fov->GetDates()[j] - lastDate);
			fov.add(shot.fov->GetFrames()[j]);
			lastDate = shot.fov->GetDates()[j];
		}

		str += sprintf(str, "static Timeline::Anim<vector3f> animPos%d(%d, dates + %d, pos + %d);\n",
			i, shot.positions->GetDates().size, dates.size, pos.size);
		lastDate = 0;
		for (int j = 0; j < shot.positions->GetDates().size; j++)
		{
			dates.add(shot.positions->GetDates()[j] - lastDate);
			pos.add(shot.positions->GetFrames()[j]);
			lastDate = shot.positions->GetDates()[j];
		}

		str += sprintf(str, "static Timeline::Anim<quaternion> animQuat%d(%d, dates + %d, quat + %d);\n",
			i, shot.orientation->GetDates().size, dates.size, quat.size);
		lastDate = 0;
		for (int j = 0; j < shot.orientation->GetDates().size; j++)
		{
			dates.add(shot.orientation->GetDates()[j] - lastDate);
			quat.add(shot.orientation->GetFrames()[j]);
			lastDate = shot.orientation->GetDates()[j];
		}

	}

	fprintf(fp, "static int dates[] = {\n");
	for (int i = 0; i < dates.size; i++)
	{
		fprintf(fp, "  %d,\n", dates[i]);
	}
	fprintf(fp, "};\n\n");

	fprintf(fp, "static vector3f pos[] = {\n");
	for (int i = 0; i < pos.size; i++)
	{
		fprintf(fp, "  {%.10ff, %.10ff, %.10ff},\n", smallf(pos[i].x), smallf(pos[i].y), smallf(pos[i].z));
	}
	fprintf(fp, "};\n\n");

	fprintf(fp, "static compressedQuaternion cquat[] = {\n");
	for (int i = 0; i < quat.size; i++)
	{
		compressedQuaternion q = compressQuaternion(quat[i]);
		fprintf(fp, "  %d, %d, %d, %d,\n", q.x, q.y, q.z, q.w);
	}
	fprintf(fp, "};\n\n");
	fprintf(fp, "quaternion* quat = unpackQuaternions(cquat, ARRAY_LEN(cquat));\n\n");

	fprintf(fp, "static float fov[] = {\n");
	for (int i = 0; i < fov.size; i++)
	{
		fprintf(fp, "  %.10ff,\n", smallf(fov[i]));
	}
	fprintf(fp, "};\n\n");

	fprintf(fp, "%s\n", animsStr);
	delete[] animsStr;

	fprintf(fp, "void H::CreateShots() {\n");
	fprintf(fp, "  Timeline::Shot shots[] = {\n");
	for (int i = 0; i < timeline.GetShots().size; i++)
	{
		const Timeline::Shot& shot = timeline.GetShots()[i];
		fprintf(fp, "    {%d, %d, %s, &animPos%d, &animQuat%d, &animFov%d},\n",
			shot.youtubeTime, shot.storyTime, findScene(scenes, shot.scene), i, i, i);
	}
	fprintf(fp, "  };\n");
	fprintf(fp, "  for (unsigned int i = 0; i < ARRAY_LEN(shots); i++)\n");
	fprintf(fp, "    m_timeline.GetShots().add(shots[i]);\n");
	fprintf(fp, "}\n");
	fclose(fp);
}

static void ParseShot(rapidjson::Value & json, Timeline::Shot& shot, SceneDict& scenes)
{
	using namespace rapidjson;
	strcpy(shot.shotName, json["name"].GetString());
	strcpy(shot.sceneName, json["scene"].GetString());
	shot.scene = scenes[shot.sceneName];
	shot.storyTime = json["time"].GetInt();

	Value & fov = json["fov"];
	shot.fov = new Timeline::Anim<float>(fov.Size() / 2, 1);
	int date = 0;
	for (unsigned i = 0; i < fov.Size(); i += 2)
	{
		date += fov[i].GetInt();
		shot.fov->Add(date, fov[i + 1].GetInt() * DEG_TO_RAD);
	}

	date = 0;
	Value & positions = json["pos"];
	shot.positions = new Timeline::Anim<vector3f>(positions.Size() / 4, 1);
	shot.positions->SetInterpolation(positions.Size() / 4 <= 2 ? Timeline::LinearInterpolation : Timeline::SplineInterpolation);
	for (unsigned i = 0; i < positions.Size(); i += 4)
	{
		date += positions[i].GetInt();
		vector3f p = {positions[i + 1].GetFloat(), positions[i + 2].GetFloat(), positions[i + 3].GetFloat()};
		shot.positions->Add(date, p);
	}

	date = 0;
	Value & quat = json["quat"];
	shot.orientation = new Timeline::Anim<quaternion>(quat.Size() / 5, 1);
	shot.orientation->SetInterpolation(quat.Size() / 5 ? Timeline::LinearInterpolation : Timeline::SplineInterpolation);
	for (unsigned i = 0; i < quat.Size(); i += 5)
	{
		date += quat[i].GetInt();
		compressedQuaternion k = {
			(char) quat[i + 1].GetInt(),
			(char) quat[i + 2].GetInt(),
			(char) quat[i + 3].GetInt(),
			(char) quat[i + 4].GetInt()};
		shot.orientation->Add(date, k.q());
	}
}

void ParseCameraFile(const char* path, Timeline::Timeline& timeline, SceneDict& scenes)
{
	using namespace rapidjson;
	std::string fileContent = Core::ReadFileContent(path);
	Document d;
	d.Parse<kParseTrailingCommasFlag | kParseCommentsFlag>(fileContent.c_str());
	if (d.HasParseError())
	{
		LOG_ERROR("%s:%d: %s\n", path, d.GetErrorOffset(), GetParseError_En(d.GetParseError()));
		return;
	}

	// Most likely a memory leak on reload
	timeline.GetShots().empty();

	int date = 0;
	for (unsigned i = 0; i < d.Size(); i++)
	{
		Timeline::Shot shot;
		shot.youtubeTime = date;
		ParseShot(d[i], shot, scenes);
		int shotLength = shot.Duration();
		date += shotLength;
		timeline.GetShots().add(shot);
	}
	exportTimelineToCpp(timeline, scenes);
}

void PrintCamera(const Core::CameraDescription& camera)
{
	LOG_INFO("Camera:");
	Algebra::compressedQuaternion q = Algebra::compressQuaternion(camera.transform.q);
	LOG_RAW("\"fov\": [\n\t0, %d,\n],\n", (int) (camera.fov * RAD_TO_DEG));
	// TODO: round the floats?
	LOG_RAW("\"pos\": [\n\t0, %f, %f, %f,\n],\n", camera.transform.v.x, camera.transform.v.y, camera.transform.v.z);
	LOG_RAW("\"quat\": [\n\t0, %d, %d, %d, %d,\n],\n", q.x, q.y, q.z, q.w);
}

void PrintShot(Timeline::Shot& shot, const SceneDict& allScenes)
{
	// Find the scene in the dictionary
	std::string sceneName;
	for (auto it = allScenes.begin(); it != allScenes.end(); it++)
	{
		if (it->second == shot.scene)
			sceneName = it->first;
	}
	LOG_RAW("{\n");
	LOG_RAW("\t\"name\": \"noname\",\n");
	LOG_RAW("\t\"scene\": \"%s\",\n", sceneName.c_str());
	LOG_RAW("\t\"time\": %d,\n", shot.storyTime);

	int lastDate = 0;
	LOG_RAW("\t\"fov\": [\n");
	for (int i = 0; i < shot.fov->NumberOfFrames(); i++)
	{
		int date = shot.orientation->GetDates()[i];
		auto& fov = shot.fov->GetFrames();
		LOG_RAW("\t\t%d, %d,\n", date - lastDate, (int)(fov[i] * RAD_TO_DEG + 0.5f));
		lastDate = date;
	}
	LOG_RAW("\t],\n");

	lastDate = 0;
	LOG_RAW("\t\"pos\": [\n");
	for (int i = 0; i < shot.positions->NumberOfFrames(); i++)
	{
		int date = shot.orientation->GetDates()[i];
		vector3f v = shot.positions->GetFrames()[i];
		LOG_RAW("\t\t%d, %f, %f, %f,\n", date - lastDate, v.x, v.y, v.z);
		lastDate = date;
	}
	LOG_RAW("\t],\n");

	lastDate = 0;
	LOG_RAW("\t\"quat\": [\n");
	for (int i = 0; i < shot.orientation->NumberOfFrames(); i++)
	{
		int date = shot.orientation->GetDates()[i];
		auto& quaternions = shot.orientation->GetFrames();
		Algebra::compressedQuaternion q = Algebra::compressQuaternion(quaternions[i]);
		LOG_RAW("\t\t%d, %d, %d, %d, %d,\n", date - lastDate, q.x, q.y, q.z, q.w);
		lastDate = date;
	}
	LOG_RAW("\t],\n");
	LOG_RAW("},\n");
}
#endif // CAMERA_EDIT
