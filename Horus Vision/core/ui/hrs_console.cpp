#include "hrs_console.h"

static int TextEditCallbackStub(ImGuiInputTextCallbackData* data)
{
	HorusConsole& console = HorusConsole::get_instance();
	return console.TextEditCallback(data);
}

void HorusConsole::InitConsole()
{
	ClearLog();
	memset(m_InputBuf_, 0, sizeof(m_InputBuf_));
	m_HistoryPos_ = -1;

	m_Commands_.push_back("help");
	m_Commands_.push_back("history");
	m_Commands_.push_back("clear");
	m_Commands_.push_back("classify");

	m_AutoScroll_ = true;
	m_ScrollToBottom_ = true;

	AddLog("Horus Vision Console");
}

void HorusConsole::Console(bool* p_open)
{
	ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin("Console", p_open))
	{
		ImGui::End();
		return;
	}

	if (ImGui::BeginPopupContextItem())
	{
		if (ImGui::MenuItem("Close Console"))
			*p_open = false;
		ImGui::EndPopup();
	}

	ImGui::TextWrapped("Test Console Text Horus Vision - Theo Baudoin - 2023");

	// %d = int
	// %s = string
	// %f = float
	// %c = char
	// %u = unsigned int
	// %ld = long
	// %lu = unsigned long
	// %lld = long long
	// %llu = unsigned long long
	// %x = hexadecimal
	// %o = octal
	// %b = binary
	// %p = pointer
	// %e = float exponent


	if (ImGui::SmallButton("Add Dummy Text"))
	{
		AddLog(" %d some text", m_Items_.Size);

		AddLog(" [warning] Warning text ");

		AddLog(" [info] info message here ! ");

		AddLog(" [success] success message here ! ");

		AddLog(" [debug] debug message here ! ");

	}

	ImGui::SameLine();

	if (ImGui::SmallButton("Add Dummy Error")) AddLog("[error] something went wrong");

	ImGui::SameLine();

	if (ImGui::SmallButton("Clear")) { ClearLog(); }

	ImGui::SameLine();

	bool copy_to_clipboard = ImGui::SmallButton("Copy");

	ImGui::Separator();

	// Options menu
	if (ImGui::BeginPopup("Options"))
	{
		ImGui::Checkbox("Auto-scroll", &m_AutoScroll_);
		ImGui::EndPopup();
	}

	// Options, Filter
	if (ImGui::Button("Options"))
		ImGui::OpenPopup("Options");
	ImGui::SameLine();
	m_Filter_.Draw("Filter (\"incl,-excl\") (\"error\")", 180);
	ImGui::Separator();

	const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
	if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar))
	{
		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::Selectable("Clear")) ClearLog();
			ImGui::EndPopup();
		}

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));

		if (copy_to_clipboard)
			ImGui::LogToClipboard();

		for (const char* item : m_Items_)
		{
			//ImVec4 GreenColor = ImVec4(0.6f, 1.0f, 0.6f, 1.0f);
			//ImVec4 PinkColor = ImVec4(1.0f, 0.6f, 0.7f, 1.0f);

			if (!m_Filter_.PassFilter(item))
				continue;

			ImVec4 color;
			bool has_color = false;
			if (strstr(item, "[error]"))
			{
				color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
				has_color = true;
			}

			else if (strncmp(item, "# ", 2) == 0)
			{
				color = ImVec4(1.0f, 0.8f, 0.6f, 1.0f);
				has_color = true;
			}

			else if (strstr(item, "[warning]"))
			{
				color = ImVec4(0.9f, 0.9f, 0.4f, 1.0f); // Couleur jaune
				has_color = true;
			}

			else if (strstr(item, "[info]"))
			{
				color = ImVec4(0.4f, 0.8f, 1.0f, 1.0f); // Couleur bleue
				has_color = true;
			}

			else if (strstr(item, "[success]"))
			{
				color = ImVec4(0.6f, 1.0f, 0.6f, 1.0f); // Couleur verte
				has_color = true;
			}

			else if (strstr(item, "[debug]"))
			{
				color = ImVec4(1.0f, 0.6f, 0.7f, 1.0f); // Couleur rose
				has_color = true;
			}


			if (has_color)
				ImGui::PushStyleColor(ImGuiCol_Text, color);
			ImGui::TextUnformatted(item);
			if (has_color)
				ImGui::PopStyleColor();
		}

		if (copy_to_clipboard)
			ImGui::LogFinish();

		if (m_ScrollToBottom_ || (m_AutoScroll_ && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
			ImGui::SetScrollHereY(1.0f);
		m_ScrollToBottom_ = false;

		ImGui::PopStyleVar();
	}
	ImGui::EndChild();
	ImGui::Separator();

	// Command-line
	bool reclaim_focus = false;
	ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;
	if (ImGui::InputText("Input", m_InputBuf_, IM_ARRAYSIZE(m_InputBuf_), input_text_flags, &TextEditCallbackStub, (void*)this))
	{
		char* s = m_InputBuf_;
		Strtrim(s);
		if (s[0])
			ExecCommand(s);
		strcpy(s, "");
		reclaim_focus = true;
	}

	// Auto-focus on window apparition
	ImGui::SetItemDefaultFocus();
	if (reclaim_focus)
		ImGui::SetKeyboardFocusHere(-1);

	ImGui::End();
}

void HorusConsole::AddLog(const char* fmt, ...) // IM_FMTARGS(2)
{
	char buf[1024];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
	buf[IM_ARRAYSIZE(buf) - 1] = 0;
	va_end(args);
	m_Items_.push_back(Strdup(buf));
}

void HorusConsole::ExecCommand(const char* command_line)
{
	AddLog("# %s\n", command_line);

	m_HistoryPos_ = -1;
	for (int i = m_History_.Size - 1; i >= 0; i--)
		if (Stricmp(m_History_[i], command_line) == 0)
		{
			free(m_History_[i]);
			m_History_.erase(m_History_.begin() + i);
			break;
		}
	m_History_.push_back(Strdup(command_line));

	// Process command
	if (Stricmp(command_line, "CLEAR") == 0)
	{
		ClearLog();
	}
	else if (Stricmp(command_line, "HELP") == 0)
	{
		AddLog("Commands:");
		for (int i = 0; i < m_Commands_.Size; i++)
			AddLog("- %s", m_Commands_[i]);
	}
	else if (Stricmp(command_line, "HISTORY") == 0)
	{
		int first = m_History_.Size - 10;
		for (int i = first > 0 ? first : 0; i < m_History_.Size; i++)
			AddLog("%3d: %s\n", i, m_History_[i]);
	}
	else
	{
		AddLog("Unknown command: '%s'\n", command_line);
	}

	// On command input, we scroll to bottom even if AutoScroll==false
	m_ScrollToBottom_ = true;
}

int HorusConsole::TextEditCallback(ImGuiInputTextCallbackData* data)
{
	//AddLog("cursor: %d, selection: %d-%d", data->CursorPos, data->SelectionStart, data->SelectionEnd);
	switch (data->EventFlag)
	{
	case ImGuiInputTextFlags_CallbackCompletion:
	{
		const char* word_end = data->Buf + data->CursorPos;
		const char* word_start = word_end;
		while (word_start > data->Buf)
		{
			const char c = word_start[-1];
			if (c == ' ' || c == '\t' || c == ',' || c == ';')
				break;
			word_start--;
		}

		ImVector<const char*> candidates;
		for (int i = 0; i < m_Commands_.Size; i++)
			if (Strnicmp(m_Commands_[i], word_start, (int)(word_end - word_start)) == 0)
				candidates.push_back(m_Commands_[i]);

		if (candidates.Size == 0)
		{
			AddLog("No match for \"%.*s\"!\n", (int)(word_end - word_start), word_start);
		}
		else if (candidates.Size == 1)
		{
			data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
			data->InsertChars(data->CursorPos, candidates[0]);
			data->InsertChars(data->CursorPos, " ");
		}
		else
		{
			int match_len = (int)(word_end - word_start);
			for (;;)
			{
				int c = 0;
				bool all_candidates_matches = true;
				for (int i = 0; i < candidates.Size && all_candidates_matches; i++)
					if (i == 0)
						c = toupper(candidates[i][match_len]);
					else if (c == 0 || c != toupper(candidates[i][match_len]))
						all_candidates_matches = false;
				if (!all_candidates_matches)
					break;
				match_len++;
			}

			if (match_len > 0)
			{
				data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
				data->InsertChars(data->CursorPos, candidates[0], candidates[0] + match_len);
			}

			AddLog("Possible matches:\n");
			for (int i = 0; i < candidates.Size; i++)
				AddLog("- %s\n", candidates[i]);
		}

		break;
	}
	case ImGuiInputTextFlags_CallbackHistory:
	{
		const int prev_history_pos = m_HistoryPos_;
		if (data->EventKey == ImGuiKey_UpArrow)
		{
			if (m_HistoryPos_ == -1)
				m_HistoryPos_ = m_History_.Size - 1;
			else if (m_HistoryPos_ > 0)
				m_HistoryPos_--;
		}
		else if (data->EventKey == ImGuiKey_DownArrow)
		{
			if (m_HistoryPos_ != -1)
				if (++m_HistoryPos_ >= m_History_.Size)
					m_HistoryPos_ = -1;
		}

		if (prev_history_pos != m_HistoryPos_)
		{
			const char* history_str = (m_HistoryPos_ >= 0) ? m_History_[m_HistoryPos_] : "";
			data->DeleteChars(0, data->BufTextLen);
			data->InsertChars(0, history_str);
		}
	}
	}
	return 0;
}

void HorusConsole::ClearLog()
{
	for (int i = 0; i < m_Items_.Size; i++)
		free(m_Items_[i]);
	m_Items_.clear();
}
