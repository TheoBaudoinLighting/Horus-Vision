#pragma once

#define USE_OCIO 1

#include "RadeonProRender.hpp"
#include <string>

class HorusOCIO
{
public:

	static rpr_status Display(
		rpr_framebuffer Framebuffer, 
		const std::string& OcioFile, 
		const std::string& Src,

		const std::string& Display,
		const std::string& View,    

		float Exposure,
		float Gamma,
		const std::string& FilePath,

		bool UseOcio
	);
};