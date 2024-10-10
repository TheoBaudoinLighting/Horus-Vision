#pragma once

// C++ Standard Library
#include <string>

// Radeon ProRender
#include "RadeonProRender_v2.h"

// Horus includes
#include "common.h"
#include "hrs_vertex_data.h"
#include "hrs_opengl_shape.h"

struct MeshData // Just for 1 mesh
{
	int MeshID = 0;
	std::string MeshName = "";

	HorusOpenGlShape OpenGlShape;
	rpr_shape RadeonShape = nullptr;

	std::vector<VertexData> Vertices = {};
	std::vector<unsigned int> MaterialIDs = {};
	std::vector<unsigned int> Indices = {};
	std::vector<unsigned int> FaceIndices = {};

	unsigned int NumVertices = 0;
	unsigned int NumIndices = 0;
	unsigned int NumFaces = 0;

	int NumTextureCoords = 0;

	bool bHasNormals = false;
	bool bHasTangents = false;
	bool bHasBitangents = false;
	bool bHasTextureCoords = false;
	bool bHasMaterialIDs = false;

	glm::vec3 BoundingBoxMin = glm::vec3(0.0f);
	glm::vec3 BoundingBoxMax = glm::vec3(0.0f);

	// Functions
	int SetMeshDataID(int ID);
	void SetMeshDataName(const std::string& Name);

	void CopyIndices(const unsigned int SourceIndices, size_t Position, size_t Size);
};

inline int MeshData::SetMeshDataID(int ID)
{
	MeshID = ID;
	return MeshID;
}
inline void MeshData::SetMeshDataName(const std::string& Name)
{
	MeshName = Name;
}
inline void MeshData::CopyIndices(const unsigned int SourceIndices, size_t Position, size_t Size)
{
	size_t RealPosition = Position / sizeof(unsigned int);

	if (RealPosition < Indices.size())
	{
		Indices[RealPosition] = SourceIndices;
	}
	else
	{
		Indices.push_back(SourceIndices);
	}


}
