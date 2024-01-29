#pragma once

#include <string>

#include "hrs_opengl.h"

#include <GLFW/glfw3.h>

#include <Windows.h>
#include <commdlg.h>
#include <ShlObj.h>
#include <shobjidl.h> 

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>


namespace Utils
{

	class HorusFileDialog
	{
	public:

		static std::string OpenFile(const char* filter)
		{
			HorusOpenGL& OpenGL = HorusOpenGL::GetInstance();

			OPENFILENAMEA ofn;
			CHAR szFile[260] = { 0 };
			ZeroMemory(&ofn, sizeof(OPENFILENAME));
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = glfwGetWin32Window(OpenGL.GetWindow());
			ofn.lpstrFile = szFile;
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrFilter = filter;
			ofn.nFilterIndex = 1;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

			if (GetOpenFileNameA(&ofn) == TRUE)
			{
				return ofn.lpstrFile;
			}
			return std::string();
		}

		static std::string SaveFile(const char* filter)
		{
			HorusOpenGL& OpenGL = HorusOpenGL::GetInstance();

			OPENFILENAMEA ofn;
			CHAR szFile[260] = { 0 };
			ZeroMemory(&ofn, sizeof(OPENFILENAME));
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = glfwGetWin32Window(OpenGL.GetWindow());
			ofn.lpstrFile = szFile;
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrFilter = filter;
			ofn.nFilterIndex = 1;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

			if (GetSaveFileNameA(&ofn) == TRUE)
			{
				return ofn.lpstrFile;
			}
			return std::string();
		}

		static std::string OpenFolder()
		{
			HorusOpenGL& OpenGL = HorusOpenGL::GetInstance();
			BROWSEINFO Bi = { 0 };
			Bi.hwndOwner = glfwGetWin32Window(OpenGL.GetWindow());
			Bi.lpszTitle = "Select a folder";
			Bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

			if (LPITEMIDLIST Pidl = SHBrowseForFolder(&Bi); Pidl != nullptr)
			{
				if (char Path[MAX_PATH]; SHGetPathFromIDList(Pidl, Path))
				{
					IMalloc* Imalloc = 0;
					if (SUCCEEDED(SHGetMalloc(&Imalloc)))
					{
						Imalloc->Free(Pidl);
						Imalloc->Release();
					}
					return Path;
				}
			}

			return std::string();
		}

	};





}
