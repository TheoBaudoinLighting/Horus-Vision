#pragma once

// C++ Standard Library
#include <string>

// GLM
#include "glm/glm.hpp"

struct PreProcessMeshData
{
	// File Informations
	std::string Path;
	std::string FileName;
	std::string FileVersion;
	std::string FileCreator;
	std::string FileFormat;

	// Import Transformations Options
	glm::vec3 ImportTranslate = glm::vec3(0.0f);
	glm::vec3 ImportRotate = glm::vec3(0.0f);
	glm::vec3 ImportScale = glm::vec3(1.0f);
	glm::vec3 ImportUniformScale = glm::vec3(1.0f);

	// Import Options
	bool bCalcTangentSpace = true;
	bool bJoinIdenticalVertices = true;
	bool bMakeLeftHanded = false;
	bool bTriangulate = true;
	bool bRemoveComponent = false;
	bool bGenNormals = true;
	bool bGenSmoothNormals = true;
	bool bSplitLargeMeshes = false;
	bool bPreTransformVertices = false;
	bool bLimitBoneWeights = false;
	bool bValidateDataStructure = true;
	bool bImproveCacheLocality = false;
	bool bRemoveRedundantMaterials = false;
	bool bFixInfacingNormals = false;
	bool bPopulateArmatureData = false;
	bool bSortByPType = true;
	bool bFindDegenerates = false;
	bool bFindInvalidData = false;
	bool bGenUVCoords = false;
	bool bTransformUVCoords = false;
	bool bFindInstances = false;
	bool bOptimizeMeshes = false;
	bool bOptimizeGraph = false;
	bool bFlipUVs = false;
	bool bFlipWindingOrder = false;
	bool bSplitByBoneCount = false;
	bool bDebone = false;
	bool bGlobalScale = false;
	bool bEmbedTextures = false;
	bool bForceGenNormals = false;
	bool bDropNormals = false;
	bool bGenBoundingBoxes = false;
	bool bConvertAxis = false;

	// File Informations
	int NumMeshes = 0;
	int NumMaterials = 0;
	int NumTextures = 0;
	int NumLights = 0;
	int NumCameras = 0;
	int NumAnimations = 0;

	// Mesh Informations
	unsigned int NumTotalVertices = 0;
	unsigned int NumTotalFaces = 0;
	unsigned int NumTotalIndices = 0;
};
