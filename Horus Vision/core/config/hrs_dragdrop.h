#pragma once


class HorusDragAndDrop
{
public:

	static HorusDragAndDrop& GetInstance()
	{
		static HorusDragAndDrop instance;
		return instance;
	}

	HorusDragAndDrop(HorusDragAndDrop const&) = delete;
	void operator=(HorusDragAndDrop const&) = delete;

	bool IsDragging() { return m_bIsDragging_; }
	void SetIsDragging(bool bIsDragging) { m_bIsDragging_ = bIsDragging; }

private:

	HorusDragAndDrop() {}

	bool m_bIsDragging_ = false;


};