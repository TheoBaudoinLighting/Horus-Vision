#pragma once

// OpenGL includes
#include <glad/glad.h>

// Spdlog includes
#include "spdlog/spdlog.h"

// Horus includes
#include <iostream>
#include "hrs_vao.h"
#include "hrs_vbo.h"
#include "hrs_ebo.h"
#include "hrs_opengl_camera.h"
#include "hrs_vertex_data.h"

class HorusOpenGlShape
{
public:

	HorusOpenGlShape() = default;

	void DestroyOpenGlShape() {
		mVertices_.clear();
		mIndices_.clear();
		mVao_.Delete();
	}

	void SetShapeName(const std::string& Name) { mShapeName_ = Name; }
	std::string GetShapeName() const { return mShapeName_; }

	void SetTranslation(glm::vec3 Pos) { mTranslation_ = Pos; }
	void SetRotation(glm::vec3 Rot) { mRotation_ = Rot; }
	void SetScale(glm::vec3 Scale_) { mScale_ = Scale_; }

	void SetModelMatrix(glm::mat4 ModelMatrix) { mModelMatrix_ = ModelMatrix; }

	glm::mat4 GetModelMatrix() const { return mModelMatrix_; }
	glm::vec3 GetTranslation() const { return mTranslation_; }
	glm::vec3 GetRotation() const { return mRotation_; }
	glm::vec3 GetScale() const { return mScale_; }

	bool InitOpenGlMesh(std::vector<VertexData>& Vertices, std::vector<GLuint>& Indices)
	{
		if (mVertices_.size() > 0 || mIndices_.size() > 0)
		{
			spdlog::error("OpenGl mesh already initialized");
			return false;
		}

		if (Vertices.size() == 0 || Indices.size() == 0)
		{
			spdlog::error("OpenGl mesh vertices or indices are empty");
			return false;
		}

		mVertices_ = Vertices;
		mIndices_ = Indices;
		mVao_.Bind();

		HorusVBO VBO(mVertices_);
		HorusEBO EBO(mIndices_);

		mVao_.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(VertexData), (void*)0);
		mVao_.LinkAttrib(VBO, 1, 3, GL_FLOAT, sizeof(VertexData), (void*)(3 * sizeof(float)));
		mVao_.LinkAttrib(VBO, 2, 3, GL_FLOAT, sizeof(VertexData), (void*)(6 * sizeof(float)));
		mVao_.LinkAttrib(VBO, 3, 2, GL_FLOAT, sizeof(VertexData), (void*)(9 * sizeof(float)));

		if (!mVertices_.empty() && !mIndices_.empty())
		{

		}

		mVao_.Unbind();
		VBO.Unbind();
		EBO.Unbind();

		return true;
	}

	int GetVerticesCount() const { return mVertices_.size() / 3; }
	int GetIndicesCount() const { return mIndices_.size() / 3; }

	void GetMeshInstanceIDSystem()
	{
		std::cout << "Mesh instance id: " << this << '\n';
	}

	void Draw(GLuint& ProgramID, HorusOpenGLCamera& Camera, bool IsSelected = false)
	{
		glUseProgram(ProgramID);
		mVao_.Bind();

		const glm::vec3 CameraPosition = Camera.GetPosition();

		glUniform3f(glGetUniformLocation(ProgramID, "camPos"), CameraPosition.x, CameraPosition.y, CameraPosition.z);
		Camera.Matrix(ProgramID, "CameraMatrix");

		glUniformMatrix4fv(glGetUniformLocation(ProgramID, "translation"), 1, GL_FALSE, glm::value_ptr(mTranslation_));
		glUniformMatrix4fv(glGetUniformLocation(ProgramID, "rotation"), 1, GL_FALSE, glm::value_ptr(mRotation_));
		glUniformMatrix4fv(glGetUniformLocation(ProgramID, "scale"), 1, GL_FALSE, glm::value_ptr(mScale_));

		glUniformMatrix4fv(glGetUniformLocation(ProgramID, "Model"), 1, GL_FALSE, &mModelMatrix_[0][0]);

		glUniform1i(glGetUniformLocation(ProgramID, "IsMeshSelected"), GL_FALSE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawElements(GL_TRIANGLES, mIndices_.size(), GL_UNSIGNED_INT, 0);

		if (IsSelected)
		{
			glUniform1i(glGetUniformLocation(ProgramID, "IsMeshSelected"), GL_TRUE);

			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDrawElements(GL_TRIANGLES, mIndices_.size(), GL_UNSIGNED_INT, 0);

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}

private:

	// OpenGl stuff
	HorusVAO mVao_;
	std::string mShapeName_;
	std::vector<VertexData> mVertices_;
	std::vector<GLuint> mIndices_;
	//std::vector<HorusOpenGlTexture> Textures;

	glm::mat4 mModelMatrix_ = glm::mat4(1.0f);
	glm::vec3 mTranslation_ = glm::vec3(0.0f);
	glm::vec3 mRotation_ = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 mScale_ = glm::vec3(100.0f); // = 1.0f because the unit is in cm

};
