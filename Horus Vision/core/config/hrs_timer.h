#pragma once

#include <string>
#include <future>
#include <chrono>
#include <functional>
#include <unordered_map>
#include <memory>


class HorusTimer
{
public:

	HorusTimer() = default;
	~HorusTimer() = default;

	void StartTimer()
	{
		if (!m_IsRunning_)
		{
			m_StartTime_ = std::chrono::steady_clock::now();
			m_IsRunning_ = true;
			m_IsFinished_ = false;
		}
	}

	long long StopTimer()
	{
		if (m_IsRunning_) 
		{
			m_IsRunning_ = false;
			auto Now = std::chrono::steady_clock::now();
			auto Elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(Now - m_StartTime_);
			m_EndTime_ = Now;
			ResetTimer();
			return Elapsed.count();
		}

		return 0;
	}

	void ResetTimer()
	{
		if (m_IsRunning_)
		{
			StopTimer();
		}

		m_IsFinished_ = false;
		m_IsRunning_ = false;
		m_StartTime_ = std::chrono::steady_clock::now();
		m_EndTime_ = m_StartTime_;

	}

	[[nodiscard]] bool IsTimerRunning() const { return m_IsRunning_; }
	[[nodiscard]] bool IsTimerFinished() const { return m_IsFinished_; }

private:

	bool m_IsRunning_ = false;
	bool m_IsFinished_ = false;
	std::function<void()> m_Callback_;
	std::future<void> m_Future_;
	std::chrono::time_point<std::chrono::steady_clock> m_StartTime_;
	std::chrono::time_point<std::chrono::steady_clock> m_EndTime_;

};

class HorusTimerManager
{
public:

	static HorusTimerManager& GetInstance()
	{
		static HorusTimerManager Instance;
		return Instance;
	}

	HorusTimerManager(const HorusTimerManager&) = delete;
	void operator=(const HorusTimerManager&) = delete;

	void StartTimer(const std::string& Name)
	{
		if (m_Timers_.contains(Name) && m_Timers_[Name]->IsTimerRunning()) {
			DeleteTimer(Name);
		}
		else
		{
			m_Timers_[Name] = std::make_unique<HorusTimer>();
			m_Timers_[Name]->StartTimer();
		}
	}

	long long StopTimer(const std::string& Name)
	{
		if (m_Timers_.contains(Name))
		{
			const long long ElapsedTime = m_Timers_[Name]->StopTimer();
			return ElapsedTime;
		}

		spdlog::error("Timer {} not found", Name);
		return 0;
	}

	void DeleteTimer(const std::string& Name)
	{
		m_Timers_.erase(Name);
	}

	void ResetTimer(const std::string& Name)
	{
		if (m_Timers_.contains(Name))
		{
			m_Timers_[Name]->ResetTimer();
		}
	}


private:

	HorusTimerManager() {}

	std::unordered_map<std::string, std::unique_ptr<HorusTimer>> m_Timers_;
};