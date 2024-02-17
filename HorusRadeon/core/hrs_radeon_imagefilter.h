#pragma once

#include "RadeonImageFilters.h"
#define USE_OPENGL 1

#define DEVICE 0

#if defined(USE_OPENGL)
#include "RadeonImageFilters_gl.h"
#endif

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "ImageTools.h"

namespace TBM
{
	inline int CreateRIFTest()
	{
		// First create context and queue
		rif_int status = RIF_SUCCESS;
		rif_context context = nullptr;
		rif_command_queue queue = nullptr;
		rif_image_filter filter = nullptr;
		rif_image inputImage = nullptr;
		rif_image outputImage = nullptr;

		int deviceCount = 0;
		status = rifGetDeviceCount(RIF_BACKEND_API_OPENCL, &deviceCount);
		if (status != RIF_SUCCESS)
		{
			return -1;
		}
		if (deviceCount > 0 || status)
		{
			status = rifCreateContext(RIF_API_VERSION, RIF_BACKEND_API_OPENCL, DEVICE, "cache", &context);
			if (status != RIF_SUCCESS || !context)
			{
				return -1;
			}
		}
		status = rifContextCreateCommandQueue(context, &queue);
		if (status != RIF_SUCCESS || !queue)
		{
			return -1;
		}

		//Create image filter
		status = rifContextCreateImageFilter(context, RIF_IMAGE_FILTER_COLOR_SPACE, &filter);
		if (status != RIF_SUCCESS)
		{
			return -1;
		}

		
		

		// Load input image
		inputImage = ImageTools::LoadImageData("resources/Textures/art.jpg", context);
		if (!inputImage)
		{
			return -1;
		}

		rif_image_desc desc;
		size_t retSize;
		rifImageGetInfo(inputImage, RIF_IMAGE_DESC, sizeof(desc), &desc, &retSize);
		status = rifContextCreateImage(context, &desc, nullptr, &outputImage);
		if (status != RIF_SUCCESS)
		{
			return -1;
		}

		status = rifImageFilterSetParameter1u(filter, "srcColorSpace", RIF_COLOR_SPACE_SRGB);
		if (status != RIF_SUCCESS)
		{
			return -1;
		}
		status = rifImageFilterSetParameter1u(filter, "dstColorSpace", RIF_COLOR_SPACE_DCIP3);
		if (status != RIF_SUCCESS)
		{
			return -1;
		}
		

		status = rifCommandQueueAttachImageFilter(queue, filter, inputImage, outputImage);
		if (status != RIF_SUCCESS)
		{
			return -1;
		}

		status = rifContextExecuteCommandQueue(context, queue, nullptr, nullptr, nullptr);
		if (status != RIF_SUCCESS)
		{
			return -1;
		}

		//Save output image
		//ImageTools::SaveImage(outputImage, "FlowerOut.png");

		rif_image_desc descOut;
		size_t retSizeOut;
		rif_int statusOut = rifImageGetInfo(outputImage, RIF_IMAGE_DESC, sizeof(desc), &desc, &retSize);
		if (status != RIF_SUCCESS)
		{
			return false;
		}

		// get image data
		rif_uchar* data;
		status = rifImageMap(outputImage, RIF_IMAGE_MAP_READ, (void**)&data);
		if (status != RIF_SUCCESS || !data)
		{
			return false;
		}

		// write image to disk
		stbi_write_png("FlowerOut.png", desc.image_width, desc.image_height, desc.num_components, data, 0);
		status = rifImageUnmap(outputImage, data);
		if (status != RIF_SUCCESS)
		{
			return false;
		}

		//Free resources
		rifCommandQueueDetachImageFilter(queue, filter);
		rifObjectDelete(inputImage);
		rifObjectDelete(outputImage);
		rifObjectDelete(filter);
		rifObjectDelete(queue);
		rifObjectDelete(context);

		std::cout << "RIF test finished" << '\n';

		return 0;



	}








}
