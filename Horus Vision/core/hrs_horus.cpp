// Main file for Horus Vision

// Project includes
#include "hrs_window.h"

class Horus_Core
{
public:

	Horus_Core(int window_width, int window_height, const std::string& window_title)
	{
		m_window_ = std::make_unique<HorusWindow>();
		m_window_->init_window(window_width, window_height, window_title);
	}

	void run()
	{
		while (m_window_->is_running())
		{
			m_window_->render();
		}
	}

private:

	std::unique_ptr<HorusWindow> m_window_;
};

int main()
{
	std::unique_ptr<Horus_Core> horus_core = std::make_unique<Horus_Core>(800, 600, "Horus Vision");
	horus_core->run();
}