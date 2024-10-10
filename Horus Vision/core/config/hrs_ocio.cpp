#include "hrs_ocio.h"

#include "Math/mathutils.h"

#include "common.h"
#include "spdlog/spdlog.h"

//unsigned char FloatToByte(float f)
//{
//	return static_cast<unsigned char>(RadeonProRender::clamp(static_cast<int>(f * 255.0f), (int)0, (int)255));
//}

rpr_status HorusOCIO::Init(rpr_context Context, rpr_framebuffer Framebuffer, std::shared_ptr<float[]> FramebufferData, const std::string& OcioFile,
	const std::string& Source, const std::string& Display, const std::string& View,
	float Exposure, float Gamma)
{
	m_CurrentOcioConfigPath_ = OcioFile.c_str();
	m_CurrentOcioRenderSpace_ = Display.c_str();

	spdlog::info("OCIO config : {}", m_CurrentOcioConfigPath_);

	// Setup Radeon with OCIO
	// For texture file color space uniquely
	{
		SetOcioConfigPath(Context, m_CurrentOcioConfigPath_);

		SetOcioRenderColorSpace(Context, m_CurrentOcioRenderSpace_);
	}

	m_Status_ = rprFrameBufferGetInfo(Framebuffer, RPR_FRAMEBUFFER_DESC, sizeof(m_FbInfoDesc_), &m_FbInfoDesc_, 0);
	if (m_Status_ != RPR_SUCCESS)
	{
		spdlog::error("rprFrameBufferGetInfo failed");
		return m_Status_;
	}

	m_PixelCount_ = m_FbInfoDesc_.fb_width * m_FbInfoDesc_.fb_height;
	m_ChannelCount_ = m_PixelCount_ * 4;

	m_FramebufferData_ = std::move(FramebufferData);

	ChannelCountLDR = 3;
	m_FramebufferDataLdr_ = std::make_unique<float[]>(m_PixelCount_ * ChannelCountLDR);

	try
	{
		m_Config_ = OCIO::Config::CreateFromFile(OcioFile.c_str());

		m_Transform_ = OCIO::DisplayViewTransform::Create();
		m_Transform_->setSrc(Source.c_str());
		m_Transform_->setDisplay(Display.c_str());
		m_Transform_->setView(View.c_str());

		m_ViewingPipeline_ = OCIO::LegacyViewingPipeline::Create();
		m_ViewingPipeline_->setDisplayViewTransform(m_Transform_);
		m_ViewingPipeline_->setLooksOverrideEnabled(true);
		m_ViewingPipeline_->setLooksOverride("");

		spdlog::info("OCIO source : {}", Source.c_str());

		// Exposure
		{
			m_Gain_ = Exposure;

			OCIO::MatrixTransform::Scale(m_Matrix4X4_, m_Offset4F_, m_Slope4F_);
			m_ExposureTransform_ = OCIO::MatrixTransform::Create();
			m_ExposureTransform_->setMatrix(m_Matrix4X4_);
			m_ExposureTransform_->setOffset(m_Offset4F_);
			m_ViewingPipeline_->setLinearCC(m_ExposureTransform_);

			spdlog::info("OCIO exposure : {}", Exposure);
		}

		// Post display transform
		if (Gamma != 0.0f)
		{
			double Exponent = 1.0 / std::max(1e-6, double(Gamma));
			const double Exponent4f[4] = { Exponent, Exponent, Exponent, Exponent };

			m_ExponentTransform_ = OCIO::ExponentTransform::Create();
			m_ExponentTransform_->setValue(Exponent4f);
			m_ViewingPipeline_->setDisplayCC(m_ExponentTransform_);

			spdlog::info("OCIO gamma : {}", Gamma);
		}

		m_Processor_ = m_ViewingPipeline_->getProcessor(m_Config_);
		m_CpuProcessor_ = m_Processor_->getDefaultCPUProcessor(); // getDefaultGPUProcessor
		m_GpuProcessor_ = m_Processor_->getDefaultGPUProcessor();

		spdlog::info("OCIO config loaded");
	}
	catch (std::exception& e)
	{
		const char* What = e.what();
		m_Processor_ = nullptr;
		m_CpuProcessor_ = nullptr;
		m_GpuProcessor_ = nullptr;
		m_Config_ = nullptr;
		std::cout << "OCIO exception: " << What << "\n";
		return RPR_ERROR_INTERNAL_ERROR;
	}

	return RPR_SUCCESS;
}


void HorusOCIO::ApplyOCIO(rpr_framebuffer Framebuffer)
{


}


void HorusOCIO::SetOcioConfigPath(rpr_context Context, std::string OcioConfigPath)
{
	const char ocio_config[] = "resources/aces_1.0.3/config.ocio";

	CHECK(rprContextSetParameterByKeyString(Context, RPR_CONTEXT_OCIO_CONFIG_PATH, ocio_config));
}

void HorusOCIO::SetOcioRenderColorSpace(rpr_context Context, std::string RenderingColorSpace)
{
	CHECK(rprContextSetParameterByKeyString(Context, RPR_CONTEXT_OCIO_RENDERING_COLOR_SPACE, "ACES - ACEScg"));
}

