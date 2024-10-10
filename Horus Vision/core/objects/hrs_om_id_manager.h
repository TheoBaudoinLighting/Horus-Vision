#pragma once

#include <deque>
#include <mutex>

namespace Utils
{
	static uint32_t PcgHash(uint32_t Value)
	{
		uint32_t State = Value * 747796405u + 2891336453u;
		uint32_t Word = ((State >> ((State >> 28u) + 4u)) ^ State) * 277803737u;
		return (Word >> 22u) ^ Word;
	}

	static float RandomFloat(uint32_t& Seed)
	{
		Seed = PcgHash(Seed);
		return (float)Seed / (float)std::numeric_limits<uint32_t>::max();
	}

	static int RandomInt(uint32_t& Seed)
	{
		Seed = PcgHash(Seed);
		return (int)Seed / (int)std::numeric_limits<uint32_t>::max();
	}
}

class HorusIdManager
{
public:
	static HorusIdManager& GetInstance()
	{
		static HorusIdManager Instance;
		return Instance;
	}

	HorusIdManager(const HorusIdManager&) = delete;
	HorusIdManager& operator=(const HorusIdManager&) = delete;

	int GetNewId()
	{
		std::lock_guard<std::mutex> Lock(m_Mutex_);
		if (!m_AvailableIDs_.empty())
		{
			int Id = m_AvailableIDs_.front();
			m_AvailableIDs_.pop_front();
			return Id;
		}
		return m_NextId_++;
	}
	void ReleaseId(int id)
	{
		std::lock_guard<std::mutex> Lock(m_Mutex_);
		m_AvailableIDs_.push_back(id);
	}

	// Getters
	int GetIdFromObjectName(const std::string& Name)
	{
		std::lock_guard<std::mutex> Lock(m_Mutex_);
		return m_ObjectNameToIdMap_[Name];
	}
	bool IsIdAvailable(int Id)
	{
		std::lock_guard<std::mutex> Lock(m_Mutex_);
		return std::find(m_AvailableIDs_.begin(), m_AvailableIDs_.end(), Id) != m_AvailableIDs_.end();
	}
	bool IsIdSelected(int Id)
	{
		std::lock_guard<std::mutex> Lock(m_Mutex_);
		return m_ActualSelectedId_ == Id;
	}
	bool IsNameSelected(const std::string& Name)
	{
		std::lock_guard<std::mutex> Lock(m_Mutex_);
		return m_Name_ == Name;
	}
	bool IsNameAvailable(const std::string& Name)
	{
		std::lock_guard<std::mutex> Lock(m_Mutex_);
		return m_ObjectNameToIdMap_.find(Name) != m_ObjectNameToIdMap_.end();
	}
	bool IsNameAvailableAndSelected(const std::string& Name)
	{
		std::lock_guard<std::mutex> Lock(m_Mutex_);
		return m_ObjectNameToIdMap_.find(Name) != m_ObjectNameToIdMap_.end() && m_Name_ == Name;
	}

	int GetSelectedId()
	{
		std::lock_guard<std::mutex> Lock(m_Mutex_);
		return m_ActualSelectedId_;
	}
	std::string GetName()
	{
		std::lock_guard<std::mutex> Lock(m_Mutex_);
		return m_Name_;
	}
	bool ContainsName(const std::string& Name)
	{
		std::lock_guard<std::mutex> Lock(m_Mutex_);
		if (m_ObjectNameToIdMap_.contains(Name))
		{
			return true;
		}

		return false;
		//return m_ObjectNameToIdMap_.find(Name) != m_ObjectNameToIdMap_.end();
	}

	std::map<std::string, int>& GetObjectNameToIdMap()
	{
		return m_ObjectNameToIdMap_;
	}
	int GetIDFromObjectName(const std::string& Name)
	{
		return m_ObjectNameToIdMap_[Name];
	}


	// Setters
	int SetObjectNameToID(const std::string& Name, int ID)
	{
		std::lock_guard<std::mutex> Lock(m_Mutex_);
		m_ObjectNameToIdMap_[Name] = ID;

		return ID;
	}
	void SetActualSelectedId(int Id)
	{
		std::lock_guard<std::mutex> Lock(m_Mutex_);
		m_ActualSelectedId_ = Id;
	}
	void SetSelectedId(int Id)
	{
		std::lock_guard<std::mutex> Lock(m_Mutex_);
		m_ActualSelectedId_ = Id;
	}
	void SetName(const std::string& Name)
	{
		std::lock_guard<std::mutex> Lock(m_Mutex_);
		m_Name_ = Name;
	}
	void DeleteClearAndRelease()
	{
		std::lock_guard<std::mutex> Lock(m_Mutex_);
		m_ObjectNameToIdMap_.clear();
		m_ActualSelectedId_ = -1;
		m_Name_ = "";
		m_NextId_ = 0;
		m_AvailableIDs_.clear();
	}

private:
	HorusIdManager() : m_NextId_(0)
	{
	}

	std::map<std::string, int> m_ObjectNameToIdMap_;
	int m_ActualSelectedId_ = -1;
	std::string m_Name_;

	int m_NextId_;
	std::deque<int> m_AvailableIDs_;
	std::mutex m_Mutex_;
};
