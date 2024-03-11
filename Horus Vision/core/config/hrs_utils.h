#pragma once
// File of utility functions for Horus Vision 4305

#pragma warning(disable: 4305)
#pragma warning(disable: 4244)
#pragma warning(disable: 4996)
#pragma warning(disable: 4101)

#define NOMINMAX
#include <Windows.h>
#include <shellapi.h>

// External includes
#include "hrs_garbage_collector.h"
#include <Math/mathutils.h>

#include "glm/glm.hpp"

#include "spdlog/spdlog.h"

#include <cassert>
#include <filesystem>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>

#include "hrs_horus_parameters.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "json.hpp"

inline void OsOpenInShell(const char* path)
{
	ShellExecuteA(nullptr, "open", path, nullptr, nullptr, SW_SHOWDEFAULT);
}

using namespace std;

struct Vertex
{
	RadeonProRender::float3 pos[3];
	RadeonProRender::float3 normal[3];
	RadeonProRender::float2 uv[3];
};

inline rpr_creation_flags GpuFlags[] = {
	RPR_CREATION_FLAGS_ENABLE_GPU0,
	RPR_CREATION_FLAGS_ENABLE_GPU1,
	RPR_CREATION_FLAGS_ENABLE_GPU2,
	RPR_CREATION_FLAGS_ENABLE_GPU3,
	RPR_CREATION_FLAGS_ENABLE_GPU4,
	RPR_CREATION_FLAGS_ENABLE_GPU5,
	RPR_CREATION_FLAGS_ENABLE_GPU6,
	RPR_CREATION_FLAGS_ENABLE_GPU7,
	RPR_CREATION_FLAGS_ENABLE_GPU8,
	RPR_CREATION_FLAGS_ENABLE_GPU9,
	RPR_CREATION_FLAGS_ENABLE_GPU10,
	RPR_CREATION_FLAGS_ENABLE_GPU11,
	RPR_CREATION_FLAGS_ENABLE_GPU12,
	RPR_CREATION_FLAGS_ENABLE_GPU13,
	RPR_CREATION_FLAGS_ENABLE_GPU14,
	RPR_CREATION_FLAGS_ENABLE_GPU15,
};

inline rpr_creation_flags CpuFlags[] = {
	RPR_CREATION_FLAGS_ENABLE_CPU,
};

const rpr_context_properties ContextProperties[] =
{
	(rpr_context_properties)RPR_CONTEXT_PRECOMPILED_BINARY_PATH,
	(rpr_context_properties)HORUS_APP_HIPBIN_PATH.c_str(),

	(rpr_context_properties)RPR_CONTEXT_CACHE_PATH,
	(rpr_context_properties)HORUS_APP_CACHE_PATH.c_str(),

	(rpr_context_properties)RPR_CONTEXT_OOC_TEXTURE_CACHE,
	(rpr_context_properties)HORUS_APP_TEXTURE_CACHE_PATH.c_str(),

	(rpr_context_properties)RPR_CONTEXT_OOC_CACHE_PATH,
	(rpr_context_properties)HORUS_APP_OOC_CACHE_PATH.c_str(),

	// terminate the list of properties with a NULL <property name>
	(rpr_context_properties)0,
};

inline rpr_shape CreateQuad(HorusGarbageCollector& Gc, rpr_context Context, rpr_scene Scene, Vertex* MeshVertices,
	unsigned int MeshVerticesNbOfElement)
{
	rpr_int Indices[] = { 3, 2, 1, 0, };
	rpr_int NumFaceVertices[] = { 4, };

	constexpr unsigned int NumFaceVerticesNbOfElement = sizeof(NumFaceVertices) / sizeof(NumFaceVertices[0]);

	rpr_shape Mesh = nullptr;

	CHECK(rprContextCreateMesh(Context,
		(rpr_float const*)&MeshVertices[0], MeshVerticesNbOfElement, sizeof(Vertex),
		(rpr_float const*)((char*)&MeshVertices[0] + sizeof(rpr_float) * 3), MeshVerticesNbOfElement, sizeof(Vertex),
		(rpr_float const*)((char*)&MeshVertices[0] + sizeof(rpr_float) * 6), MeshVerticesNbOfElement, sizeof(Vertex),
		(rpr_int const*)Indices, sizeof(rpr_int),
		(rpr_int const*)Indices, sizeof(rpr_int),
		(rpr_int const*)Indices, sizeof(rpr_int),
		NumFaceVertices, NumFaceVerticesNbOfElement, &Mesh));
	Gc.Add(Mesh);

	if (Scene) { CHECK(rprSceneAttachShape(Scene, Mesh)); }

	return Mesh;
}

inline rpr_shape CreateQuadXy(HorusGarbageCollector& Gc, rpr_context Context, rpr_scene Scene, float ax, float ay,
	float bx, float by, float Z, float Normal)
{
	Vertex MeshVertices[] =
	{
		{{ax, by, Z}, {0.0f, 0.0f, Normal}, {0.0f, 0.0f}},
		{{bx, by, Z}, {0.0f, 0.0f, Normal}, {1.0f, 0.0f}},
		{{bx, ay, Z}, {0.0f, 0.0f, Normal}, {1.0f, 1.0f}},
		{{ax, ay, Z}, {0.0f, 0.0f, Normal}, {0.0f, 1.0f}},
	};

	constexpr unsigned int MeshVerticesNbOfElement = sizeof(MeshVertices) / sizeof(MeshVertices[0]);

	rpr_shape Mesh = CreateQuad(Gc, Context, Scene, MeshVertices, MeshVerticesNbOfElement);
	return Mesh;
}

inline rpr_shape CreateSquare(HorusGarbageCollector& Gc, rpr_context Context, rpr_scene Scene, float PosX, float PosY,
	const float Size, const float Margin)
{
	// Basic Check
	if (Size <= 0.0f)
	{
		spdlog::error("Size must be greater than 0.0f");
		return nullptr;
	}

	if (Context == nullptr)
	{
		spdlog::error("Context is null");
		return nullptr;
	}

	if (Scene == nullptr)
	{
		spdlog::error("Scene is null");
		return nullptr;
	}

	float HalfSize = Size / 2.0f;

	rpr_shape Quad = CreateQuadXy(Gc, Context, Scene,
		-HalfSize + PosX * (Margin + Size), -HalfSize + PosY * (Margin + Size),
		HalfSize + PosX * (Margin + Size), HalfSize + PosY * (Margin + Size),
		0.0, 1.0f);

	return Quad;
}

// Utility 
// Loop functions
inline void DoOnce(bool& flag, std::function<void()> func)
{
	if (!flag)
	{
		func();
		flag = true;
	}
}

inline void DoOnce(bool& flag, std::function<void()> func, std::function<void()> elseFunc)
{
	if (!flag)
	{
		func();
		flag = true;
	}
	else
	{
		elseFunc();
	}
}

inline void ForEachLoop(std::vector<int>& vec, std::function<void(int&)> func)
{
	for (int& i : vec)
	{
		func(i);
	}
}

inline void ForEachLoopWithBreak(bool& flag, std::vector<int>& vec, std::function<void(int&)> func)
{
	for (int& i : vec)
	{
		func(i);
		if (flag)
		{
			break;
		}
	}
}

inline void ReverseForEachLoop(std::vector<int>& vec, std::function<void(int&)> func)
{
	for (int i = vec.size() - 1; i >= 0; i--)
	{
		func(vec[i]);
	}
}

inline void ReverseForEachLoopWithBreak(bool& flag, std::vector<int>& vec, std::function<void(int&)> func)
{
	for (int i = vec.size() - 1; i >= 0; i--)
	{
		func(vec[i]);
		if (flag)
		{
			break;
		}
	}
}

inline void ForLoop(int start, int end, std::function<void(int)> func)
{
	for (int i = start; i < end; i++)
	{
		func(i);
	}
}

inline void ReverseForLoop(int start, int end, std::function<void(int)> func)
{
	for (int i = end - 1; i >= start; i--)
	{
		func(i);
	}
}

inline void DoNTimes(int n, std::function<void()> func)
{
	for (int i = 0; i < n; i++)
	{
		func();
	}
}

inline void RepeatUntil(bool& flag, std::function<void()> func)
{
	while (!flag)
	{
		func();
	}
}

inline void FlipFlopper(bool& flag, std::function<void()> func1, std::function<void()> func2)
{
	if (flag)
	{
		func1();
	}
	else
	{
		func2();
	}
	flag = !flag;
}

inline void FlipFlopper(bool& flag, std::function<void()> func1, std::function<void()> func2,
	std::function<void()> func3)
{
	if (flag)
	{
		func1();
	}
	else
	{
		func2();
	}
	flag = !flag;
	func3();
}

inline void WhileLoop(bool& flag, std::function<void()> func)
{
	while (flag)
	{
		func();
	}
}

inline void FlipFlopperText(bool& flag, std::string& text1, std::string& text2)
{
	if (flag)
	{
		ImGui::Text(text1.c_str());
	}
	else
	{
		ImGui::Text(text2.c_str());
	}
	flag = !flag;
}


// Comparaison functions
inline void IsValueEqual(float value, float target, std::function<void()> func)
{
	if (value == target)
	{
		func();
	}
}

inline void IsValueInRange(float value, float min, float max, std::function<void()> func)
{
	if (value >= min && value <= max)
	{
		func();
	}
}

inline void IsValueInRange(float value, float min, float max, std::function<void()> func,
	std::function<void()> elseFunc)
{
	if (value >= min && value <= max)
	{
		func();
	}
	else
	{
		elseFunc();
	}
}

inline void IsValueInRange(float value, float min, float max, std::function<void()> func,
	std::function<void()> elseFunc, std::function<void()> elseFunc2)
{
	if (value >= min && value <= max)
	{
		func();
	}
	else
	{
		elseFunc();
		elseFunc2();
	}
}

// Sequence functions
inline void Sequence(std::vector<std::function<void()>> funcs)
{
	for (auto& func : funcs)
	{
		func();
	}
}

// Delay functions
inline void Delay(float seconds, std::function<void()> func)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(seconds * 1000)));
	func();
}

inline void RepeatEvery(float seconds, std::function<void()> func)
{
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(seconds * 1000)));
		func();
	}
}

// Random functions
inline int RandomInt(int min, int max)
{
	return min + (rand() % (max - min + 1));
}

inline float RandomFloat(float min, float max)
{
	return min + static_cast<float>(rand()) / (RAND_MAX / (max - min));
}

// Math functions
// Clamp a value between a minimum and a maximum

inline float Clamp(float value, float min, float max)
{
	return value < min ? min : (value > max ? max : value);
}

// Linear interpolation
inline float Lerp(float a, float b, float t)
{
	return a + t * (b - a);
}

// Smoothstep interpolation
inline float Smoothstep(float a, float b, float t)
{
	t = Clamp((t - a) / (b - a), 0.0f, 1.0f);
	return t * t * (3.0f - 2.0f * t);
}

// Smootherstep interpolation
inline float Smootherstep(float a, float b, float t)
{
	t = Clamp((t - a) / (b - a), 0.0f, 1.0f);
	return t * t * t * (t * (t * 6 - 15) + 10);
}

// Remap a value from one range to another
inline float Remap(float value, float fromMin, float fromMax, float toMin, float toMax)
{
	return Lerp(toMin, toMax, (value - fromMin) / (fromMax - fromMin));
}

// Convert degrees to radians
inline float Radians(float degrees)
{
	return degrees * 0.0174532925f;
}

// Convert radians to degrees
inline float Degrees(float radians)
{
	return radians * 57.2957795f;
}

// Check if a number is a power of two
inline bool IsPowerOfTwo(int value)
{
	return (value & (value - 1)) == 0;
}

// Round a number up to the nearest power of two
inline int RoundUpToPowerOfTwo(int value)
{
	value--;
	value |= value >> 1;
	value |= value >> 2;
	value |= value >> 4;
	value |= value >> 8;
	value |= value >> 16;
	value++;
	return value;
}

// Retriggerable functions
// Retriggerable delay

inline void RetriggerableDelay(float seconds, bool& flag, std::function<void()> func)
{
	while (flag)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(seconds * 1000)));
		func();

		if (!flag)
		{
			break;
		}
	}
}

// Retriggerable repeat every
inline void RetriggerableRepeatEvery(float seconds, bool& flag, std::function<void()> func)
{
	while (flag)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(seconds * 1000)));
		func();

		if (!flag)
		{
			break;
		}
	}
}

// Retriggerable sequence

inline void RetriggerableSequence(std::vector<std::function<void()>> funcs, bool& flag)
{
	for (auto& func : funcs)
	{
		func();

		if (!flag)
		{
			break;
		}
	}
}

// Retriggerable loop

inline void RetriggerableLoop(std::function<void()> func, bool& flag)
{
	while (flag)
	{
		func();
	}
}

// Retriggerable for loop

inline void RetriggerableForLoop(int start, int end, std::function<void(int)> func, bool& flag)
{
	for (int i = start; i < end; i++)
	{
		func(i);

		if (!flag)
		{
			break;
		}
	}
}


// Retriggerable reverse for loop

inline void RetriggerableReverseForLoop(int start, int end, std::function<void(int)> func, bool& flag)
{
	for (int i = end - 1; i >= start; i--)
	{
		func(i);

		if (!flag)
		{
			break;
		}
	}
}

// Retriggerable do n times

inline void RetriggerableDoNTimes(int n, std::function<void()> func, bool& flag)
{
	for (int i = 0; i < n; i++)
	{
		func();

		if (!flag)
		{
			break;
		}
	}
}

// Retriggerable repeat until

inline void RetriggerableRepeatUntil(bool& flag, std::function<void()> func)
{
	while (!flag)
	{
		func();
	}
}

// Retriggerable flip flopper

inline void RetriggerableFlipFlopper(bool& flag, std::function<void()> func1, std::function<void()> func2)
{
	if (flag)
	{
		func1();
	}
	else
	{
		func2();
	}
	flag = !flag;
}

inline void RetriggerableFlipFlopper(bool& flag, std::function<void()> func1, std::function<void()> func2,
	std::function<void()> func3)
{
	if (flag)
	{
		func1();
	}
	else
	{
		func2();
	}
	flag = !flag;
	func3();
}

// Flow control functions

inline void WaitForEvent(bool& flag, std::function<void()> func)
{
	while (!flag)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	func();
}

inline void Debounce(float seconds, bool& flag, std::function<void()> func)
{
	if (flag)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(seconds * 1000)));
		func();
		flag = false;
	}
}

inline void Throttle(float seconds, bool& flag, std::function<void()> func)
{
	if (!flag)
	{
		func();
		flag = true;
		std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(seconds * 1000)));
		flag = false;
	}
}

inline void PollCondition(bool& flag, std::function<void()> func)
{
	while (!flag)
	{
		func();
	}
}

inline void GuardCondition(bool& flag, std::function<void()> func)
{
	if (flag)
	{
		func();
	}
}

inline void ParallelExecutors(std::vector<std::function<void()>> funcs)
{
	std::vector<std::thread> threads;
	for (auto& func : funcs)
	{
		threads.push_back(std::thread(func));
	}
	for (auto& thread : threads)
	{
		thread.join();
	}
}

inline void ParallelExecutor(std::function<void()> func, int n)
{
	std::vector<std::thread> threads;
	for (int i = 0; i < n; i++)
	{
		threads.push_back(std::thread(func));
	}
	for (auto& thread : threads)
	{
		thread.join();
	}
}

// Error handling functions

inline void TryCatch(std::function<void()> func, std::function<void()> catchFunc)
{
	try
	{
		func();
	}
	catch (const std::exception& e)
	{
		catchFunc();
	}
}

inline void TryCatch(std::function<void()> func, std::function<void()> catchFunc, std::function<void()> finallyFunc)
{
	try
	{
		func();
	}
	catch (const std::exception& e)
	{
		catchFunc();
	}
	finallyFunc();
}

inline void RecoverableError(std::function<void()> func, std::function<void()> recoverFunc)
{
	try
	{
		func();
	}
	catch (const std::exception& e)
	{
		recoverFunc();
	}
}

inline void RetryOnFail(int maxRetries, std::function<void()> func, std::function<void()> recoverFunc)
{
	int retries = 0;
	while (retries < maxRetries)
	{
		try
		{
			func();
			break;
		}
		catch (const std::exception& e)
		{
			recoverFunc();
			retries++;
		}
	}
}

inline void RetryOnFailWithDelay(int maxRetries, float seconds, std::function<void()> func,
	std::function<void()> recoverFunc)
{
	int Retries = 0;
	while (Retries < maxRetries)
	{
		try
		{
			func();
			break;
		}
		catch (const std::exception& E)
		{
			recoverFunc();
			std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(seconds * 1000)));
			Retries++;
		}
	}
}

inline void FallbackOnFail(std::function<void()> Func, std::function<void()> fallbackFunc)
{
	try
	{
		Func();
	}
	catch (const std::exception& E)
	{
		fallbackFunc();
	}
}

inline void FallbackOnFailWithDelay(float seconds, std::function<void()> func, std::function<void()> fallbackFunc)
{
	try
	{
		func();
	}
	catch (const std::exception& e)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(seconds * 1000)));
		fallbackFunc();
	}
}

inline void ValideOrThrow(bool condition, const char* message)
{
	if (!condition)
	{
		throw std::runtime_error(message);
	}
}

// Optimization functions
inline void Memoize(std::function<void()> func)
{
	static bool memoized = false;
	if (!memoized)
	{
		func();

		// TODO : Add a way to reset the memoization
		// TODO : Add a way to memoize a function with arguments

		memoized = true;
	}
}

inline void BatchProcess(std::vector<std::function<void()>> funcs)
{
	for (auto& func : funcs)
	{
		func();
	}
}

inline void BatchProcessWithDelay(float seconds, std::vector<std::function<void()>> funcs)
{
	for (auto& func : funcs)
	{
		func();
		std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(seconds * 1000)));
	}
}

inline void LazyLoad(std::function<void()> func)
{
	static bool loaded = false;
	if (!loaded)
	{
		func();
		loaded = true;
	}
}

inline void LazyLoadWithDelay(float seconds, std::function<void()> func)
{
	static bool loaded = false;
	if (!loaded)
	{
		func();
		std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(seconds * 1000)));
		loaded = true;
	}
}

inline void LazyLoadInDetachedThread(std::function<void()> func)
{
	static bool loaded = false;
	if (!loaded)
	{
		std::thread t(func);
		t.detach();
		loaded = true;
	}
}

inline void CachingVector(std::vector<int>& vec, std::function<void()> func)
{
	static bool cached = false;
	if (!cached)
	{
		func();
		cached = true;
	}
}

inline void CachingBool(bool& flag, std::function<void()> func)
{
	static bool cached = false;
	if (!cached)
	{
		func();
		cached = true;
	}
}

inline void CachingInt(int& value, std::function<void()> func)
{
	static bool cached = false;
	if (!cached)
	{
		func();
		cached = true;
	}
}

inline void CachingFloat(float& value, std::function<void()> func)
{
	static bool cached = false;
	if (!cached)
	{
		func();
		cached = true;
	}
}

inline void CachingString(std::string& value, std::function<void()> func)
{
	static bool cached = false;
	if (!cached)
	{
		func();
		cached = true;
	}
}

inline void CachingValue(void* value, std::function<void()> func)
{
	static bool cached = false;
	if (!cached)
	{
		func();
		cached = true;
	}
}

inline void CachingValueWithDelay(float seconds, void* value, std::function<void()> func)
{
	static bool cached = false;
	if (!cached)
	{
		func();
		std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(seconds * 1000)));
		cached = true;
	}
}

inline void CachingValueInDetachedThread(void* value, std::function<void()> func)
{
	static bool cached = false;
	if (!cached)
	{
		std::thread t(func);
		t.detach();
		cached = true;
	}
}

// File functions
inline void ReadFileSafe(const std::string& path, std::function<void(std::string)> func)
{
	std::ifstream file(path);
	if (file.is_open())
	{
		std::stringstream buffer;
		buffer << file.rdbuf();
		func(buffer.str());
	}
	else
	{
		spdlog::error("Can't open file {}", path);
	}
}

inline void WriteFileSafe(const std::string& path, const std::string& content)
{
	std::ofstream file(path);
	if (file.is_open())
	{
		file << content;
	}
	else
	{
		spdlog::error("Can't open file {}", path);
	}
}

inline void WatchFileChange(const std::string& path, std::function<void()> func)
{
	std::ifstream file(path);
	if (file.is_open())
	{
		std::string content;
		std::string line;
		while (std::getline(file, line))
		{
			content += line;
		}
		file.close();

		std::time_t lastWriteTime = std::filesystem::last_write_time(path).time_since_epoch().count();
		while (true)
		{
			std::time_t newWriteTime = std::filesystem::last_write_time(path).time_since_epoch().count();
			if (newWriteTime != lastWriteTime)
			{
				lastWriteTime = newWriteTime;
				func();
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}
	spdlog::error("Can't open file {}", path);
}

// Encryption functions
inline std::string Encrypt(const std::string& text)
{
	std::string encrypted;
	for (char c : text)
	{
		encrypted += c + 1;
	}
	return encrypted;
}

inline std::string Decrypt(const std::string& text)
{
	std::string decrypted;
	for (char c : text)
	{
		decrypted += c - 1;
	}
	return decrypted;
}

// Json functions
inline nlohmann::json LoadJson(const std::string& content)
{
	return nlohmann::json::parse(content);
}

inline nlohmann::json LoadJsonFromFile(const std::string& path)
{
	const std::ifstream File(path);
	if (File.is_open())
	{
		std::stringstream buffer;
		buffer << File.rdbuf();
		return nlohmann::json::parse(buffer.str());
	}
	spdlog::error("Can't open file {}", path);
	return {};
}

inline std::string WriteJson(const nlohmann::json& j)
{
	return j.dump();
}

inline void SaveJsonToFile(const std::string& path, const nlohmann::json& json)
{
	std::ofstream File(path);
	if (File.is_open())
	{
		File << json.dump(4);
	}
	else
	{
		spdlog::error("Can't open file {}", path);
	}
}

inline void TraverseJsonWithKey(const nlohmann::json& json, std::function<void(const std::string&, const nlohmann::json&)> func)
{
	if (json.is_object())
	{
		for (auto& [key, value] : json.items())
		{
			func(key, value);
			TraverseJsonWithKey(value, func);
		}
	}
	else if (json.is_array())
	{
		for (auto& value : json)
		{
			TraverseJsonWithKey(value, func);
		}
	}
}

inline void TraverseJsonWithIndex(const nlohmann::json& json, std::function<void(int, const nlohmann::json&)> func)
{
	if (json.is_array())
	{
		for (int i = 0; i < json.size(); i++)
		{
			func(i, json[i]);
			TraverseJsonWithIndex(json[i], func);
		}
	}
}

inline void TraverseJsonWithKeyAndIndex(const nlohmann::json& json, std::function<void(const std::string&, int, const nlohmann::json&)> func)
{
	if (json.is_object())
	{
		for (auto& [key, value] : json.items())
		{
			func(key, -1, value);
			TraverseJsonWithKeyAndIndex(value, func);
		}
	}
	else if (json.is_array())
	{
		for (int i = 0; i < json.size(); i++)
		{
			func("", i, json[i]);
			TraverseJsonWithKeyAndIndex(json[i], func);
		}
	}
}

inline void AddElementToJson(nlohmann::json& json, const std::string& key, const nlohmann::json& value)
{
	json[key] = value;
}

inline void ModifyElementInJson(nlohmann::json& json, const std::string& key, const nlohmann::json& value)
{
	if (json.contains(key))
	{
		json[key] = value;
	}
}

inline void RemoveElementFromJson(nlohmann::json& json, const std::string& key)
{
	if (json.contains(key))
	{
		json.erase(key);
	}
}

inline nlohmann::json FilterJson(const nlohmann::json& j, const std::function<bool(const nlohmann::json&)>& filterFunc)
{
	nlohmann::json result;
	for (const auto& element : j.items())
	{
		if (filterFunc(element.value()))
		{
			result[element.key()] = element.value();
		}
	}
	return result;
}

inline void SortJson(nlohmann::json& json, std::function<bool(const nlohmann::json&, const nlohmann::json&)> func)
{
	if (json.is_object())
	{
		std::map<std::string, nlohmann::json> sorted;
		for (auto& [key, value] : json.items())
		{
			sorted[key] = value;
		}
		json = sorted;
	}
	else if (json.is_array())
	{
		std::sort(json.begin(), json.end(), func);
	}
}

inline bool IsValidateJson(const std::string& content)
{
	try
	{
		auto j = nlohmann::json::parse(content);
		return true;
	}
	catch (...)
	{
		return false;
	}
}

inline std::string MinifyJson(const nlohmann::json& j)
{
	return j.dump();
}

inline std::string BeautifyJson(const nlohmann::json& j)
{
	return j.dump(4); // 4 spaces for indentation
}

// ImGui utility functions

inline bool ImRightAlign(const char* str_id)
{
	if (ImGui::BeginTable(str_id, 2, ImGuiTableFlags_SizingFixedFit, ImVec2(-1, 0)))
	{
		ImGui::TableSetupColumn("a", ImGuiTableColumnFlags_WidthStretch);

		ImGui::TableNextColumn();
		ImGui::TableNextColumn();
		return true;
	}
	return false;
}
#define IM_END_RIGHT_ALIGN ImGui::EndTable

inline void ShowHandCursorOnHover()
{
	if (ImGui::IsItemHovered())
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
	}
}

inline void ShowResizeCursorOnHover()
{
	if (ImGui::IsItemHovered())
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
	}
}

inline void ShowResizeHorizontalCursorOnHover()
{
	if (ImGui::IsItemHovered())
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
	}
}

inline void ShowResizeVerticalCursorOnHover()
{
	if (ImGui::IsItemHovered())
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
	}
}

inline void ShowResizeTopLeftBottomRightCursorOnHover()
{
	if (ImGui::IsItemHovered())
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNESW);
	}
}

inline void ShowResizeTopRightBottomLeftCursorOnHover()
{
	if (ImGui::IsItemHovered())
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNWSE);
	}
}

inline void ShowTextInputCursorOnHover()
{
	if (ImGui::IsItemHovered())
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_TextInput);
	}
}

inline void ShowNotAllowedCursorOnHover()
{
	if (ImGui::IsItemHovered())
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_NotAllowed);
	}
}

inline void ShowVerticalResizeCursorOnHover()
{
	if (ImGui::IsItemHovered())
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
	}
}

inline void ShowHorizontalResizeCursorOnHover()
{
	if (ImGui::IsItemHovered())
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
	}
}

inline void ShowHandCursorOnHover(const char* tooltip)
{
	if (ImGui::IsItemHovered())
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
		ImGui::SetTooltip(tooltip);
	}
}

inline void ShowResizeCursorOnHover(const char* tooltip)
{
	if (ImGui::IsItemHovered())
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
		ImGui::SetTooltip(tooltip);
	}
}

inline void ShowResizeHorizontalCursorOnHover(const char* tooltip)
{
	if (ImGui::IsItemHovered())
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
		ImGui::SetTooltip(tooltip);
	}
}

// ImGui Link 
inline void OpenLink(const char* text, const char* url)
{
	ImGui::TextUnformatted(text);
	if (ImGui::IsItemHovered())
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
		if (ImGui::IsMouseClicked(0))
		{
			OsOpenInShell(url);
		}
	}
}

inline void OpenLink(const char* text, const char* url, const ImVec4& color)
{
	ImGui::TextColored(color, text);
	if (ImGui::IsItemHovered())
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
		if (ImGui::IsMouseClicked(0))
		{
			OsOpenInShell(url);
		}
	}
}

inline void OpenLink(const char* text, const char* url, const ImVec4& color, float fontSize)
{
	ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
	ImGui::TextColored(color, text);
	ImGui::PopFont();
	if (ImGui::IsItemHovered())
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
		if (ImGui::IsMouseClicked(0))
		{
			OsOpenInShell(url);
		}
	}
}

inline void OpenLink(const char* text, const char* url, const ImVec4& color, float fontSize, float width)
{
	ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
	ImGui::SetCursorPosX((width - ImGui::CalcTextSize(text).x) / 2);
	ImGui::TextColored(color, text);
	ImGui::PopFont();
	if (ImGui::IsItemHovered())
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
		if (ImGui::IsMouseClicked(0))
		{
			OsOpenInShell(url);
		}
	}
}

// ImGui Draw a rectangle
inline void DrawRectangle(const ImVec2& position, const ImVec2& size, const ImVec4& color)
{
	ImGui::GetWindowDrawList()->AddRectFilled(position, ImVec2(position.x + size.x, position.y + size.y),
		ImGui::GetColorU32(color));
}

// ImGui Draw a rectangle with a border
inline void DrawRectangle(const ImVec2& position, const ImVec2& size, const ImVec4& color, const ImVec4& borderColor)
{
	ImGui::GetWindowDrawList()->AddRectFilled(position, ImVec2(position.x + size.x, position.y + size.y),
		ImGui::GetColorU32(color));
	ImGui::GetWindowDrawList()->AddRect(position, ImVec2(position.x + size.x, position.y + size.y),
		ImGui::GetColorU32(borderColor));
}

// ImGui Draw a rectangle around item
inline void DrawRectangleAroundItem(const ImVec4& color)
{
	ImVec2 min = ImGui::GetItemRectMin();
	ImVec2 max = ImGui::GetItemRectMax();
	ImGui::GetWindowDrawList()->AddRect(min, max, ImGui::GetColorU32(color));
}

// Set the tooltip of the current item
inline void SetTooltip(const char* text)
{
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip(text);
	}
}

inline void SetTooltip(const char* text, const ImVec4& color)
{
	if (ImGui::IsItemHovered())
	{
		ImGui::PushStyleColor(ImGuiCol_Text, color);
		ImGui::SetTooltip(text);
		ImGui::PopStyleColor();
	}
}

inline void SetTooltip(const char* text, const ImVec4& color, float fontSize)
{
	if (ImGui::IsItemHovered())
	{
		ImGui::PushStyleColor(ImGuiCol_Text, color);
		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
		ImGui::SetTooltip(text);
		ImGui::PopFont();
		ImGui::PopStyleColor();
	}
}

inline void SetTooltip(const char* text, const ImVec4& color, float fontSize, float width)
{
	if (ImGui::IsItemHovered())
	{
		ImGui::PushStyleColor(ImGuiCol_Text, color);
		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
		ImGui::SetTooltip(text);
		ImGui::PopFont();
		ImGui::PopStyleColor();
	}
}

inline void SetTooltip(const char* text, const ImVec4& color, float fontSize, float width, float height)
{
	if (ImGui::IsItemHovered())
	{
		ImGui::PushStyleColor(ImGuiCol_Text, color);
		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
		ImGui::SetTooltip(text);
		ImGui::PopFont();
		ImGui::PopStyleColor();
	}
}

inline void SetTooltip(const char* text, const ImVec4& color, float fontSize, float width, float height, bool centered)
{
	if (ImGui::IsItemHovered())
	{
		ImGui::PushStyleColor(ImGuiCol_Text, color);
		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
		ImGui::SetTooltip(text);
		ImGui::PopFont();
		ImGui::PopStyleColor();
	}
}


// Text utility functions
inline void TextCentered(const char* Text)
{
	ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize(Text).x) / 2);
	ImGui::Text(Text);
}

inline void TextCentered(const char* Text, const ImVec4& Color)
{
	ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize(Text).x) / 2);
	ImGui::PushStyleColor(ImGuiCol_Text, Color);
	ImGui::Text(Text);
	ImGui::PopStyleColor();
}

inline void TextCentered(const char* Text, const ImVec4& Color, float FontSize)
{
	ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize(Text).x) / 2);
	ImGui::PushStyleColor(ImGuiCol_Text, Color);
	ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
	ImGui::Text(Text);
	ImGui::PopFont();
	ImGui::PopStyleColor();
}

inline void TextCentered(const char* Text, const ImVec4& Color, float FontSize, float Width)
{
	ImGui::SetCursorPosX((Width - ImGui::CalcTextSize(Text).x) / 2);
	ImGui::PushStyleColor(ImGuiCol_Text, Color);
	ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
	ImGui::Text(Text);
	ImGui::PopFont();
	ImGui::PopStyleColor();
}

// Convert a string to a wide string
inline std::wstring StringToWideString(const std::string& str)
{
	std::wstring wstr(str.length(), L' ');
	std::copy(str.begin(), str.end(), wstr.begin());
	return wstr;
}

// Convert a wide string to a string
inline std::string WideStringToString(const std::wstring& wstr)
{
	std::string str(wstr.length(), ' ');
	std::copy(wstr.begin(), wstr.end(), str.begin());
	return str;
}

// Convert const char* to std::string
inline std::string CTS(const char* Char)
{
	std::string Str(Char);
	return Str;
}

// Convert std::string to const char*
inline const char* STC(const std::string& Str)
{
	return Str.c_str();
}

// Extract the file name from a path
inline std::string ExtractFileName(const std::string& Path)
{
	size_t LastSlash = Path.find_last_of("\\/");
	return Path.substr(LastSlash + 1);
}

// Extract the file extension from a path
inline std::string ExtractFileExtension(const std::string& Path)
{
	size_t LastDot = Path.find_last_of(".");
	return Path.substr(LastDot + 1);
}

// Extract the file name without the extension from a path
inline std::string ExtractFileNameWithoutExtension(const std::string& Path)
{
	std::string FileName = ExtractFileName(Path);
	size_t LastDot = FileName.find_last_of(".");
	return FileName.substr(0, LastDot);
}

// Check if a file exists
inline bool FileExists(const std::string& Path)
{
	std::ifstream File(Path);
	return File.good();
}

// Read text file
inline std::string ReadTextFile(const std::string& filePath)
{
	std::ifstream fileStream(filePath);
	std::stringstream buffer;
	buffer << fileStream.rdbuf();
	return buffer.str();
}

// Write text file
inline void WriteTextFile(const std::string& filePath, const std::string& content)
{
	std::ofstream fileStream(filePath);
	fileStream << content;
}

// read binary file
inline std::vector<char> ReadBinaryFile(const std::string& filePath)
{
	std::ifstream fileStream(filePath, std::ios::binary | std::ios::ate);
	std::streamsize size = fileStream.tellg();
	fileStream.seekg(0, std::ios::beg);
	std::vector<char> buffer(size);
	if (fileStream.read(buffer.data(), size))
	{
		return buffer;
	}
	return {};
}

// write binary file
inline void WriteBinaryFile(const std::string& filePath, const std::vector<char>& data)
{
	std::ofstream fileStream(filePath, std::ios::binary);
	fileStream.write(data.data(), data.size());
}

// Check extension
inline bool CheckExtension(const std::string& fileName, const std::string& extension)
{
	size_t pos = fileName.rfind('.');
	if (pos != std::string::npos)
	{
		return fileName.substr(pos + 1) == extension;
	}
	return false;
}

inline std::string ConcatenatePath(const std::string& directory, const std::string& fileName)
{
	return directory + ((directory.back() == '/' || directory.back() == '\\') ? "" : "/") + fileName;
}

inline std::string FormatPath(std::string path)
{
	std::replace(path.begin(), path.end(), '\\', '/');
	return path;
}

inline std::vector<std::string> SplitString(const std::string& str, char delimiter)
{
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream tokenStream(str);
	while (std::getline(tokenStream, token, delimiter))
	{
		tokens.push_back(token);
	}
	return tokens;
}

//-------------------------------------------------------- DEBUGGING AND MESSAGE ----------------------------------//

// Show an info message
inline void LogInfo(const std::string& message)
{
	std::cout << "Info: " << message << '\n';
}

// Show an error message
inline void LogError(const std::string& message)
{
	std::cerr << "Error: " << message << '\n';
}

// Show a warning message
inline void LogWarning(const std::string& message)
{
	std::cout << "Warning: " << message << '\n';
}

// AssertCondition: Check a condition and display a message if it is not met.
inline void AssertCondition(bool condition, const std::string& message)
{
	if (!condition)
	{
		std::cerr << "Assertion failed: " << message << '\n';
		abort();
	}
}

// DisplayFPS: Calculate and display frames per second.
inline void DisplayFPS(double deltaTime)
{
	static int frameCount = 0;
	static double totalTime = 0.0;
	totalTime += deltaTime;
	++frameCount;
	if (totalTime > 1.0)
	{
		std::cout << "FPS: " << frameCount / totalTime << '\n';
		frameCount = 0;
		totalTime = 0.0;
	}
}

// Show the components of a vector
inline void DebugVector(const glm::vec3& vec)
{
	std::cout << "Vector: (" << vec.x << ", " << vec.y << ", " << vec.z << ")" << '\n';
}

// Show the components of a matrix
inline void DebugMatrix(const glm::mat4& mat)
{
	std::cout << "Matrix: " << '\n';
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			std::cout << mat[i][j] << " ";
		}
		std::cout << '\n';
	}
}

// This function is a template function that takes a function and its arguments 
// and measures the time it takes to execute the function.
// Exemple of use: ExecutionTime("FunctionName", FunctionName, arg1, arg2, arg3);
#include <chrono>

template <typename Func, typename... Args>
void ExecutionTime(const std::string& functionName, Func func, Args&&... args)
{
	auto start = std::chrono::high_resolution_clock::now();
	func(std::forward<Args>(args)...);
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> duration = end - start;
	std::cout << functionName << " executed in " << duration.count() << " ms" << std::endl;
}

// ListResources: Display a list of resources.
inline void ListResources(const std::vector<std::string>& resources)
{
	std::cout << "Loaded Resources:" << '\n';
	for (const auto& resource : resources)
	{
		std::cout << "- " << resource << '\n';
	}
}
