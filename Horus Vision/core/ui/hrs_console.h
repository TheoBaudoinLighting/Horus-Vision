#pragma once
#pragma warning (disable: 4127)  
#pragma warning (disable: 4996)  
#pragma warning (disable: 26451) 
#pragma warning (disable: 26495)
#pragma warning (disable: 26812)

#include <imgui.h>

#include <ctype.h>         
#include <limits.h>        
#include <math.h>          
#include <stdio.h>         
#include <stdlib.h>        
#include <stdint.h>   

class HorusConsole
{
public:

	static HorusConsole& get_instance()
	{
		static HorusConsole instance;
		return instance;
	}

	HorusConsole(HorusConsole const&) = delete;
	void operator=(HorusConsole const&) = delete;

	void InitConsole();
	void Console(bool* p_open);
	void AddLog(const char* fmt, ...);
	void ExecCommand(const char* command_line);
	int TextEditCallback(ImGuiInputTextCallbackData* data);
	void ClearLog();

	// Portable helpers
	static int   Stricmp(const char* s1, const char* s2) { int d; while ((d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; } return d; }
	static int   Strnicmp(const char* s1, const char* s2, int n) { int d = 0; while (n > 0 && (d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; n--; } return d; }
	static char* Strdup(const char* s) { IM_ASSERT(s); size_t len = strlen(s) + 1; void* buf = malloc(len); IM_ASSERT(buf); return (char*)memcpy(buf, (const void*)s, len); }
	static void  Strtrim(char* s) { char* str_end = s + strlen(s); while (str_end > s && str_end[-1] == ' ') str_end--; *str_end = 0; }

private:

	HorusConsole() {}

	char m_InputBuf_[256];

	ImVector<char*> m_Items_;
	ImVector<const char*> m_Commands_;
	ImVector<char*> m_History_;

	int m_HistoryPos_ = -1;

	ImGuiTextFilter m_Filter_;

	bool m_AutoScroll_;
	bool m_ScrollToBottom_;

};