#include "hrs_statistics.h"

#include "hrs_object_manager.h"
#include "hrs_radeon.h"
#include "imgui.h"

void HorusStatistics::InitStatistics(int NumberOfGpus, int NumberOfCpu)
{
	PopulateStatistics(NumberOfGpus, NumberOfCpu);
}

void HorusStatistics::Statistics(bool* p_open)
{
	HorusRadeon& Radeon = HorusRadeon::GetInstance();

	if (!ImGui::Begin("Statistic", p_open))
	{
		ImGui::End();
		return;
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	// Render Informations
	{
		//rprContextGetInfo(Radeon.GetContext(), RPR_CONTEXT_GPU0_NAME, sizeof(m_Gpu00N_), m_Gpu00N_, nullptr); // TODO : Fix exception here
		//rprContextGetInfo(Radeon.GetContext(), RPR_CONTEXT_GPU1_NAME, sizeof(m_Gpu01N_), m_Gpu01N_, nullptr); // TODO : Fix exception here
		//rprContextGetInfo(Radeon.GetContext(), RPR_CONTEXT_RENDER_STATISTICS, sizeof(m_RenderStatistics_), &m_RenderStatistics_, 0); // TODO : Fix exception here

		//ImGui::Begin("Property Panel");

		// gpu name
		/*rpr_longlong MemoryUsage = m_RenderStatistics_.gpumem_usage / 1024 / 1024;
		rpr_longlong SystemMemoryUsage = m_RenderStatistics_.sysmem_usage / 1024 / 1024;
		rpr_longlong GpuMaxAllocation = m_RenderStatistics_.gpumem_max_allocation / 1024 / 1024;
		rpr_longlong GpuTotal = m_RenderStatistics_.gpumem_total / 1024 / 1024;*/

		/*long long MemoryUsage = 0;
		long long SystemMemoryUsage = 0;
		long long GpuMaxAllocation = 0;
		long long GpuTotal = 0;*/



		if (ImGui::CollapsingHeader("Render Info", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Text("Render Info");
			ImGui::Separator();
			//ImGui::Text("Render Status: %s", Radeon.GetRenderStatus() ? "Rendering" : "Not Rendering");

			// Gpus
			ImGui::Text("Number of GPUs: %d", m_NumberOfGpus_);
			ImGui::Separator();
			ImGui::Text("GPU Names:");
			for (size_t i = 0; i < m_GpuNames_.size(); ++i)
			{
				ImGui::Text("GPU N%02d : %s", static_cast<int>(i + 1), m_GpuNames_[i].c_str());
			}
			ImGui::Separator();
			ImGui::Text("Samples: %d", Radeon.GetSampleCount()); // %d is a placeholder for an integer

			/*ImGui::Text("System memory usage : %d MB", SystemMemoryUsage);
			ImGui::Text("GPU Memory usage : %d MB", MemoryUsage);
			ImGui::Text("GPU max allocation : %d MB", GpuMaxAllocation);
			ImGui::Text("GPU total : %d MB", GpuTotal);*/

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			float DeltaTime = ImGui::GetIO().DeltaTime;
			float Framerate = ImGui::GetIO().Framerate;

			m_PerformanceData_.Update(DeltaTime, Framerate);

			//float avgMsPerFrame = m_PerformanceData_.GetAverageMsPerFrame();
			//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", avgMsPerFrame, 1000.0f / avgMsPerFrame);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / Framerate, Framerate);
			ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(0.6f, 1.0f, 0.6f, 1.0f));
			ImGui::PlotLines("FPS", m_PerformanceData_.FpsValues, PerformanceData::DataLength, 0, "FPS", 0.0f, 100.0f, ImVec2(0, 80));
			ImGui::PopStyleColor();
		}
	}

	ImGui::End();
}

void HorusStatistics::PopulateStatistics(int NumberOfGpus, int NumberOfCPU)
{
	HorusObjectManager& ObjectManager = HorusObjectManager::GetInstance();
	HorusRadeon& Radeon = HorusRadeon::GetInstance();

	// GPU Part

	// set the number of gpus to 7 if it's greater than 7
	m_NumberOfGpus_ = NumberOfGpus > 15 ? 15 : NumberOfGpus;

	m_GpuNames_.clear();
	m_GpuNames_.resize(m_NumberOfGpus_);

	// for each number of gpu populate the statistics
	for (int i = 0; i < m_NumberOfGpus_; i++)
	{
		char GpuName[1024];
		rpr_context_info GpuInfoId;

		switch (i) {
		case 0: GpuInfoId = RPR_CONTEXT_GPU0_NAME; break;
		case 1: GpuInfoId = RPR_CONTEXT_GPU1_NAME; break;
		case 2: GpuInfoId = RPR_CONTEXT_GPU2_NAME; break;
		case 3: GpuInfoId = RPR_CONTEXT_GPU3_NAME; break;
		case 4: GpuInfoId = RPR_CONTEXT_GPU4_NAME; break;
		case 5: GpuInfoId = RPR_CONTEXT_GPU5_NAME; break;
		case 6: GpuInfoId = RPR_CONTEXT_GPU6_NAME; break;
		case 7: GpuInfoId = RPR_CONTEXT_GPU7_NAME; break;
		case 8: GpuInfoId = RPR_CONTEXT_GPU8_NAME; break;
		case 9: GpuInfoId = RPR_CONTEXT_GPU9_NAME; break;
		case 10: GpuInfoId = RPR_CONTEXT_GPU10_NAME; break;
		case 11: GpuInfoId = RPR_CONTEXT_GPU11_NAME; break;
		case 12: GpuInfoId = RPR_CONTEXT_GPU12_NAME; break;
		case 13: GpuInfoId = RPR_CONTEXT_GPU13_NAME; break;
		case 14: GpuInfoId = RPR_CONTEXT_GPU14_NAME; break;
		case 15: GpuInfoId = RPR_CONTEXT_GPU15_NAME; break;
			// If NumberOfGpus > 15, this case should not happen, but it's to avoid errors.
		default: continue;
		}

		CHECK(rprContextGetInfo(Radeon.GetContext(), GpuInfoId, sizeof(GpuName), GpuName, nullptr));

		m_GpuNames_[i] = GpuName;
	}


}