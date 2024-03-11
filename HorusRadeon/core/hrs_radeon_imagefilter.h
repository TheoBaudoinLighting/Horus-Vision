#pragma once

#include <iostream>
#include <string>
#include <memory>

#define USE_OCIO 1

#include <OpenColorIO/OpenColorIO.h>
#include <OpenColorIO/OpenColorTransforms.h>
#include <OpenColorIO/OpenColorTypes.h>
namespace OCIO = OCIO_NAMESPACE;

#include "RadeonImageFilters.h"
#include <Math/float3.h>
#include <Math/mathutils.h>
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

inline unsigned char FloatToByte(float value)
{
	return (unsigned char)RadeonProRender::clamp((int)(value * 255.0f), (int)0, (int)255);
}

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

	class HorusOCIO
	{
	public:

		HorusOCIO() = default;
		~HorusOCIO() {}

		static void CreateOCIOConfig(const std::string& OcioConfigPath, const std::string& ColorSpaceToUse, const std::string& Display, const std::string& View, float Exposure, float Gamma);

		static void ApplyOCIOToFramebuffer(std::shared_ptr<float[]>& InputFramebufferData, int RenderRegionWidth, int RenderRegionHeight);

	private:

		inline static OCIO::ConstProcessorRcPtr m_Processor_ = nullptr;
		inline static OCIO::ConstCPUProcessorRcPtr m_CpuProcessor_ = nullptr;
		inline static OCIO::ConstConfigRcPtr m_ConfigOcio_ = nullptr;

		static inline std::string m_OcioConfigPath_;
		static inline std::string m_ColorSpaceToUse_;
		static inline std::string m_Display_;
		static inline std::string m_View_;
		static inline float m_Exposure_;
		static inline float m_Gamma_;

	};

	inline void HorusOCIO::CreateOCIOConfig(const std::string& OcioConfigPath, const std::string& ColorSpaceToUse, const std::string& Display, const std::string& View, float Exposure, float Gamma)
	{
		// Check if the OCIO config file exists
		if (OcioConfigPath.empty())
		{
			spdlog::error("OCIO config path is empty");
			return;
		}

		// Check if the color space to use is empty
		if (ColorSpaceToUse.empty())
		{
			spdlog::error("Color space to use is empty");
			return;
		}

		// Check if the display is empty
		if (Display.empty())
		{
			spdlog::error("Display is empty");
			return;
		}

		// Check if the view is empty
		if (View.empty())
		{
			spdlog::error("View is empty");
			return;
		}

		// Check if the exposure is 0
		if (Exposure == 0.0f)
		{
			spdlog::warn("Exposure for OCIO is 0");
		}

		m_Exposure_ = Exposure;
		m_Gamma_ = Gamma;
		m_OcioConfigPath_ = OcioConfigPath;
		m_ColorSpaceToUse_ = ColorSpaceToUse;
		m_Display_ = Display;
		m_View_ = View;

		try
		{
			m_ConfigOcio_ = OCIO::Config::CreateFromFile(OcioConfigPath.c_str());

			spdlog::info("OCIO config created from file: {}", OcioConfigPath);

			OCIO::DisplayViewTransformRcPtr DisplayViewTransform = OCIO::DisplayViewTransform::Create();
			//DisplayViewTransform->setSrc(ColorSpaceToUse.c_str());
			DisplayViewTransform->setSrc(OCIO::ROLE_SCENE_LINEAR);
			DisplayViewTransform->setDisplay(Display.c_str());
			DisplayViewTransform->setView(View.c_str());

			spdlog::info("OCIO color space: {}", ColorSpaceToUse);

			OCIO::LegacyViewingPipelineRcPtr ViewingPipeline = OCIO::LegacyViewingPipeline::Create();
			ViewingPipeline->setDisplayViewTransform(DisplayViewTransform);
			ViewingPipeline->setLooksOverrideEnabled(false);
			ViewingPipeline->setLooksOverride("");

			spdlog::info("OCIO display: {}", Display);
			spdlog::info("OCIO view: {}", View);
			spdlog::info("OCIO exposure: {}", Exposure);
			spdlog::info("OCIO gamma: {}", Gamma);


			// Exposure
			{
				const double Slope4f[4] = { m_Exposure_, m_Exposure_, m_Exposure_, m_Exposure_ };
				double m44[16];
				double Offset4[4];
				OCIO::MatrixTransform::Scale(m44, Offset4, Slope4f);
				OCIO::MatrixTransformRcPtr ExposureTransform = OCIO::MatrixTransform::Create();
				ExposureTransform->setMatrix(m44);
				ExposureTransform->setOffset(Offset4);
				ViewingPipeline->setLinearCC(ExposureTransform);
			}

			// Post-Display Transform Gamma
			if (Gamma != 0.0f)
			{
				double Exponent = 1.0 / std::max(1e-6, (double)m_Gamma_);
				const double Exponent4f[4] = { Exponent, Exponent, Exponent, Exponent };
				OCIO::ExponentTransformRcPtr GammaTransform = OCIO::ExponentTransform::Create();
				GammaTransform->setValue(Exponent4f);
				ViewingPipeline->setDisplayCC(GammaTransform);
			}

			m_Processor_ = ViewingPipeline->getProcessor(m_ConfigOcio_);
			m_CpuProcessor_ = m_Processor_->getDefaultCPUProcessor();

			if (!m_Processor_)
			{
				spdlog::critical("OCIO processor is null");
				return;
			}

			if (!m_CpuProcessor_)
			{
				spdlog::critical("OCIO CPU processor is null");
				return;
			}

			spdlog::info("OCIO processor created");
			spdlog::info("OCIO CPU processor created");

		}
		catch (std::exception& e)
		{
			const char* What = e.what();
			m_Processor_ = nullptr;
			m_CpuProcessor_ = nullptr;
			m_ConfigOcio_ = nullptr;
			spdlog::error("Error: {}", What);
			return;
		}

	}

	inline void HorusOCIO::ApplyOCIOToFramebuffer(std::shared_ptr<float[]>& InputFramebufferData, int RenderRegionWidth, int RenderRegionHeight)
	{
		// Check if the input framebuffer is null
		if (!InputFramebufferData) {
			spdlog::error("Input framebuffer data is null at apply OCIO");
			return;
		}

		const int PixelCount = RenderRegionWidth * RenderRegionHeight;

		const int ChannelCountLDR = 3;
		auto FramebufferDataLDR = std::make_unique<unsigned char[]>(PixelCount * ChannelCountLDR);

		// Run the OCIO transform
		try
		{
			for (int64_t i = 0; i < PixelCount; i++)
			{
				RadeonProRender::float4 RGBA = RadeonProRender::float4(
					InputFramebufferData[i * 4 + 0],
					InputFramebufferData[i * 4 + 1],
					InputFramebufferData[i * 4 + 2],
					1.0f
				);

#if (USE_OCIO == 1)
				if (USE_OCIO == 1)
				{
					m_CpuProcessor_->applyRGBA(&(RGBA.x));
				}
				else
#endif
				{
					RGBA.x = powf(RGBA.x * m_Exposure_, 1.0f / m_Gamma_);
					RGBA.y = powf(RGBA.y * m_Exposure_, 1.0f / m_Gamma_);
					RGBA.z = powf(RGBA.z * m_Exposure_, 1.0f / m_Gamma_);
					RGBA.w = 1.0f;
				}

				/*FramebufferDataLDR[i * 3 + 0] = FloatToByte(RGB.x);
				FramebufferDataLDR[i * 3 + 1] = FloatToByte(RGB.y);
				FramebufferDataLDR[i * 3 + 2] = FloatToByte(RGB.z);*/
			}

		}
		catch (std::exception& e)
		{
			spdlog::error("Error: {}", e.what());
			return;
		}
	}



}
