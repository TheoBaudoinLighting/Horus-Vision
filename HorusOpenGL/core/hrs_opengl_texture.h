#pragma once

#include "glad/glad.h"
#include "hrs_opengl_resource.h"
#include "spdlog/spdlog.h"

#include <filesystem>
#include <stb_image.h>
namespace fs = std::filesystem;

class HorusOpenGlTexture : public HorusOpenGlResource
{
public:

	GLuint ID = 0;
	const char* Type;
	GLuint Unit;

	HorusOpenGlTexture(const char* Image, const char* Textype, GLuint Slot) : Type(Textype), Unit(Slot)
	{
		Type = Textype;

		if (!Image)
		{
			spdlog::warn("Image path is empty");
			return;
		}

		if (!fs::exists(Image))
		{
			spdlog::warn("Image path doesn't exist : {}", Image);
			return;
		}

		int Width = 0, Height = 0, Channels = 0;

		unsigned char* Data = stbi_load(Image, &Width, &Height, &Channels, 0);
		if (!Data)
		{
			spdlog::warn("Failed to load texture : {}", Image);
			return;
		}

		glGenTextures(1, &ID);
		glActiveTexture(GL_TEXTURE0 + Slot);
		Unit = Slot;
		glBindTexture(GL_TEXTURE_2D, ID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		if (Type == "normal") 
			glTexImage2D
			(
				GL_TEXTURE_2D,
				0,
				GL_RGB,
				Width,
				Height,
				0,
				GL_RGBA,
				GL_UNSIGNED_BYTE,
				Data
			);
		else if (Type == "displacement")
			glTexImage2D
			(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				Width,
				Height,
				0,
				GL_RGBA,
				GL_UNSIGNED_BYTE,
				Data
			);
		else if (Channels == 4)
			glTexImage2D
			(
				GL_TEXTURE_2D,
				0,
				GL_SRGB_ALPHA,
				Width,
				Height,
				0,
				GL_RGBA,
				GL_UNSIGNED_BYTE,
				Data
			);
		else if (Channels == 3)
			glTexImage2D
			(
				GL_TEXTURE_2D,
				0,
				GL_SRGB,
				Width,
				Height,
				0,
				GL_RGB,
				GL_UNSIGNED_BYTE,
				Data
			);
		else if (Channels == 1)
			glTexImage2D
			(
				GL_TEXTURE_2D,
				0,
				GL_SRGB,
				Width,
				Height,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				Data
			);
		else
			throw std::invalid_argument("Automatic Texture type recognition failed");

		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(Data);

		glBindTexture(GL_TEXTURE_2D, 0);

		spdlog::info("Texture created : {}", Image);
	}

	~HorusOpenGlTexture() override
	{
		if (ID != 0) {
			glDeleteTextures(1, &ID);
			ID = 0;
		}
	}

	void TexUnit(GLuint ProgramID, const char* Name, GLuint Unit)
	{
		GLuint TexID = glGetUniformLocation(ProgramID, Name);
		glUseProgram(ProgramID);
		glUniform1i(TexID, Unit);
	}

	void Bind() const {
		glActiveTexture(GL_TEXTURE0 + Unit);
		glBindTexture(GL_TEXTURE_2D, ID);
	}

	void Unbind() const {
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	GLuint GetTextureID() const {
		return ID;
	}
};
