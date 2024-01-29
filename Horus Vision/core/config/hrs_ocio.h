#pragma once

#include "RadeonProRender.hpp"
#include <string>

#include <OpenColorIO/OpenColorIO.h>
#include <OpenColorIO/OpenColorTypes.h>

#include "Math/mathutils.h"


namespace OCIO = OCIO_NAMESPACE;

class HorusOCIO
{
public:

	static HorusOCIO& GetInstance()
	{
		static HorusOCIO instance;
		return instance;
	}

	HorusOCIO(HorusOCIO const&) = delete;
	void operator=(HorusOCIO const&) = delete;

	rpr_status Init(rpr_context Context, rpr_framebuffer Framebuffer, std::shared_ptr<float[]> FramebufferData,
	                const std::string& OcioFile, const std::string& Source, const std::string& Display, const std::string& View, float
	                Exposure, float Gamma);

	void CreateOCIOConfig(rpr_context Context, std::string OcioConfigPath);
	void ApplyOCIO(rpr_framebuffer Framebuffer);

	void SetOcioConfigPath(rpr_context Context, std::string OcioConfigPath);
	void SetOcioRenderColorSpace(rpr_context Context, std::string OcioConfigPath);


private:

	HorusOCIO() {}

	std::string m_CurrentOcioConfigPath_ = "";
	std::string m_CurrentOcioRenderSpace_ = "";

	// OCIO
	OCIO::ConstProcessorRcPtr m_Processor_ = nullptr;
	OCIO::ConstCPUProcessorRcPtr m_CpuProcessor_ = nullptr;
	OCIO::ConstGPUProcessorRcPtr m_GpuProcessor_ = nullptr;
	OCIO::ConstConfigRcPtr m_Config_ = nullptr;

	OCIO::DisplayViewTransformRcPtr m_Transform_;
	OCIO::LegacyViewingPipelineRcPtr m_ViewingPipeline_;
	OCIO::MatrixTransformRcPtr m_ExposureTransform_;
	OCIO::ExponentTransformRcPtr m_ExponentTransform_;

	double m_Gain_ = 1.0;
	double m_Slope4F_[4] = { m_Gain_, m_Gain_, m_Gain_, m_Gain_ };
	double m_Matrix4X4_[16];
	double m_Offset4F_[4];

	// Radeon
	rpr_status m_Status_ = RPR_SUCCESS;

	RadeonProRender::float3 m_Rgb_;

	rpr_framebuffer_desc m_FbInfoDesc_;
	std::shared_ptr<float[]> m_FramebufferData_;
	std::shared_ptr<float[]> m_FramebufferDataLdr_;

	int64_t m_PixelCount_;
	int m_ChannelCount_;

	int ChannelCountLDR;

};