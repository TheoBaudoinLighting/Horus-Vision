#pragma once
#include <string>
#include <vector>


class HorusStatistics
{
public:

	struct PerformanceData
	{
		static const int DataLength = 60;
		int MsPerFrameIdx = 0;
		float MsPerFrame[DataLength] = { 0 };
		float MsPerFrameAccum = 0.0f;
		float FpsValues[DataLength] = { 0 };

		void Update(float deltaTime, float framerate)
		{
			MsPerFrameAccum -= MsPerFrame[MsPerFrameIdx];
			MsPerFrame[MsPerFrameIdx] = deltaTime * 1000.0f;
			MsPerFrameAccum += MsPerFrame[MsPerFrameIdx];
			FpsValues[MsPerFrameIdx] = framerate;
			MsPerFrameIdx = (MsPerFrameIdx + 1) % DataLength;
		}

		float GetAverageMsPerFrame() const
		{
			return MsPerFrameAccum / DataLength;
		}
	};

	static HorusStatistics& GetInstance()
	{
		static HorusStatistics Instance;
		return Instance;
	}

	HorusStatistics(HorusStatistics const&) = delete;
	void operator=(HorusStatistics const&) = delete;

	void InitStatistics(int NumberOfGpus, int NumberOfCpu);
	void Statistics(bool* p_open);
	void PopulateStatistics(int NumberOfGpus, int NumberOfCPU);

private:

	HorusStatistics() = default;

	PerformanceData m_PerformanceData_;

	int m_NumberOfGpus_ = 0;

	std::vector<std::string> m_GpuNames_;

};

