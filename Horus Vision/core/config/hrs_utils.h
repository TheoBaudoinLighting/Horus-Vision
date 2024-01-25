#pragma once
// File of utility functions for Horus Vision 4305

#pragma warning(disable: 4305)
#pragma warning(disable: 4244)

// External includes
#include "hrs_garbage_collector.h"
#include "RadeonProRender.hpp"

#include "spdlog/spdlog.h"

#include <cassert>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

struct Vertex
{
	rpr_float Pos[3];
	rpr_float Norm[3];
	rpr_float Tex[2];
};

inline rpr_shape CreateQuad(HorusGarbageCollector& Gc, rpr_context Context, rpr_scene Scene, Vertex* MeshVertices, unsigned int MeshVerticesNbOfElement)
{
	rpr_int Indices[] = { 3,2,1,0, };
	rpr_int NumFaceVertices[] = { 4, };

	const unsigned int NumFaceVerticesNbOfElement = sizeof(NumFaceVertices) / sizeof(NumFaceVertices[0]);

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

inline rpr_shape CreateQuadXy(HorusGarbageCollector& Gc, rpr_context Context, rpr_scene Scene, float ax, float ay, float bx, float by, float Z, float Normal)
{
	Vertex MeshVertices[] =
	{
		{{ax, by, Z}, {0.0f, 0.0f, Normal}, {0.0f, 0.0f}},
		{{bx, by, Z}, {0.0f, 0.0f, Normal}, {1.0f, 0.0f}},
		{{bx, ay, Z}, {0.0f, 0.0f, Normal}, {1.0f, 1.0f}},
		{{ax, ay, Z}, {0.0f, 0.0f, Normal}, {0.0f, 1.0f}},
	};

	const unsigned int MeshVerticesNbOfElement = sizeof(MeshVertices) / sizeof(MeshVertices[0]);

	rpr_shape Mesh = CreateQuad(Gc, Context, Scene, MeshVertices, MeshVerticesNbOfElement);
	return Mesh;
}

inline rpr_shape CreateSquare(HorusGarbageCollector& Gc, rpr_context Context, rpr_scene Scene, float PosX, float PosY, const float Size, const float Margin)
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

// sRGB to ACEScg
RadeonProRender::float4 sRGB_to_ACEScg(const RadeonProRender::float4& srgb)
{
	RadeonProRender::matrix sRGB_To_XYZ_D65(
		0.4124, 0.3576, 0.1805, 0.0,
		0.2126, 0.7152, 0.0722, 0.0,
		0.0193, 0.1192, 0.9505, 0.0,
		0.0, 0.0, 0.0, 1.0);

	RadeonProRender::matrix D65_to_D60(
		1.01281, 0.00597, -0.01497, 0.0,
		0.00749, 0.99834, -0.00502, 0.0,
		-0.00285, 0.00471, 0.92424, 0.0,
		0.0, 0.0, 0.0, 1.0);

	RadeonProRender::matrix XYZd60_to_ACEScg(
		1.64184, -0.32496, -0.23654, 0.0,
		-0.66351, 1.61496, 0.01675, 0.0,
		0.01172, -0.00828, 0.98866, 0.0,
		0.0, 0.0, 0.0, 1.0);

	RadeonProRender::matrix sRGB_to_ACEScg = XYZd60_to_ACEScg * D65_to_D60 * sRGB_To_XYZ_D65;
	RadeonProRender::float4 ret = sRGB_to_ACEScg * srgb;
	return ret;
}

// ACEScg to sRGB
RadeonProRender::float4 ACEScg_to_sRGB(const RadeonProRender::float4& acescg)
{
	RadeonProRender::matrix ACEScg_to_XYZd60(
		0.66245418, 0.13400421, 0.15618769, 0.0,
		0.27222872, 0.67408177, 0.05368952, 0.0,
		-0.00557465, 0.00406073, 1.010339100, 0.0,
		0.0, 0.0, 0.0, 1.0);

	RadeonProRender::matrix D60_to_D65(
		0.987224, -0.007598, 0.015372, 0.0,
		0.012298, 1.001338, -0.011438, 0.0,
		-0.003882, 0.007246, 0.987744, 0.0,
		0.0, 0.0, 0.0, 1.0);

	RadeonProRender::matrix XYZ_D65_to_sRGB(
		3.2404542, -1.5371385, -0.4985314, 0.0,
		-0.9692660, 1.8760108, 0.0415560, 0.0,
		0.0556434, -0.2040259, 1.0572252, 0.0,
		0.0, 0.0, 0.0, 1.0);

	RadeonProRender::matrix ACEScg_to_sRGB = XYZ_D65_to_sRGB * D60_to_D65 * ACEScg_to_XYZd60;
	RadeonProRender::float4 ret = ACEScg_to_sRGB * acescg;
	return ret;
}

// SRGB to Linear
RadeonProRender::float4 sRGB_to_Linear(const RadeonProRender::float4& srgb)
{
	RadeonProRender::float4 linear;

	for (int i = 0; i < 3; ++i) 
	{
		linear[i] = (srgb[i] <= 0.04045) ? (srgb[i] / 12.92) : std::pow((srgb[i] + 0.055) / 1.055, 2.4);
	}

	linear[3] = srgb[3]; 
	return linear;
}

// Linear to SRGB
RadeonProRender::float4 Linear_to_sRGB(const RadeonProRender::float4& linear)
{
	RadeonProRender::float4 srgb;

	for (int i = 0; i < 3; ++i) 
	{
		srgb[i] = (linear[i] <= 0.0031308) ? (12.92 * linear[i]) : (1.055 * std::pow(linear[i], 1.0 / 2.4) - 0.055);
	}

	srgb[3] = linear[3]; 
	return srgb;
}

// sRGB to XYZ
RadeonProRender::float4 sRGB_to_XYZ(const RadeonProRender::float4& srgb)
{
	RadeonProRender::matrix sRGB_to_XYZ_D65(
		0.4124564, 0.3575761, 0.1804375, 0.0,
		0.2126729, 0.7151522, 0.0721750, 0.0,
		0.0193339, 0.1191920, 0.9503041, 0.0,
		0.0, 0.0, 0.0, 1.0);

	RadeonProRender::float4 xyz = sRGB_to_XYZ_D65 * sRGB_to_Linear(srgb);
	return xyz;
}

// XYZ to sRGB
RadeonProRender::float4 XYZ_to_sRGB(const RadeonProRender::float4& xyz)
{
	RadeonProRender::matrix XYZ_to_sRGB_D65(
		3.2404542, -1.5371385, -0.4985314, 0.0,
		-0.9692660, 1.8760108, 0.0415560, 0.0,
		0.0556434, -0.2040259, 1.0572252, 0.0,
		0.0, 0.0, 0.0, 1.0);

	RadeonProRender::float4 srgb = Linear_to_sRGB(XYZ_to_sRGB_D65 * xyz);
	return srgb;
}
