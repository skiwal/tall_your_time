#include <wx/wx.h>
#include <wx/scrolwin.h>
#include <map>

// Objective-C++ 菜单栏函数声明
// i am coming
extern "C" {
	void CreateStatusBarIcon(void *framePtr);
	void UpdateStatusBarTitle(const char *title);
	void DestroyStatusBar();
}

// the id of the various controls and events
enum
{
	ID_Add = wxID_HIGHEST + 1,
	ID_Done_Base = wxID_HIGHEST + 100,
	ID_TaskBar = wxID_HIGHEST + 200
};

class NoteFrame : public wxFrame
{
public:
	NoteFrame()
		: wxFrame(nullptr, wxID_ANY, "Time Notes", wxDefaultPosition, wxSize(480, 400))
	{
		auto *panel = new wxPanel(this);

		auto *noteLabel = new wxStaticText(panel, wxID_ANY, "Event:");
		noteInput = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxSize(200, -1));

		auto *addButton = new wxButton(panel, ID_Add, "Add");

		auto *listLabel = new wxStaticText(panel, wxID_ANY, "Events:");
		scrolledWindow = new wxScrolledWindow(panel, wxID_ANY, wxDefaultPosition, wxSize(-1, 200));
		scrolledWindow->SetScrollRate(0, 10);
		
		eventSizer = new wxBoxSizer(wxVERTICAL);
		scrolledWindow->SetSizer(eventSizer);

		auto *inputSizer = new wxBoxSizer(wxHORIZONTAL);
		inputSizer->Add(noteLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 6);
		inputSizer->Add(noteInput, 1, wxRIGHT, 12);
		inputSizer->Add(addButton, 0);

		auto *mainSizer = new wxBoxSizer(wxVERTICAL);
		mainSizer->Add(inputSizer, 0, wxALL | wxEXPAND, 10);
		mainSizer->Add(listLabel, 0, wxLEFT | wxRIGHT | wxBOTTOM, 10);
		mainSizer->Add(scrolledWindow, 1, wxLEFT | wxRIGHT | wxBOTTOM | wxEXPAND, 10);

		panel->SetSizer(mainSizer);

		Bind(wxEVT_BUTTON, &NoteFrame::OnAdd, this, ID_Add);
		Bind(wxEVT_CLOSE_WINDOW, &NoteFrame::OnClose, this);

		// 创建菜单栏图标
		CreateStatusBarIcon(this);
	}

	void ForceClose()
	{
		forceExit = true;
		Close(true);
	}

private:
	void OnAdd(wxCommandEvent &)
	{
		const wxString noteText = noteInput->GetValue().Trim(true).Trim(false);

		if (noteText.empty())
		{
			wxMessageBox("Please enter a note.", "Missing note", wxOK | wxICON_WARNING, this);
			return;
		}

		AddEventRow(noteText);
		noteInput->Clear();
		noteInput->SetFocus();
	}

	void AddEventRow(const wxString &noteText)
	{
		auto *rowPanel = new wxPanel(scrolledWindow);
		auto *rowSizer = new wxBoxSizer(wxHORIZONTAL);

		auto *textLabel = new wxStaticText(rowPanel, wxID_ANY, noteText);
		auto *doneButton = new wxButton(rowPanel, ID_Done_Base + nextDoneId, "Done");

		rowSizer->Add(textLabel, 1, wxALIGN_CENTER_VERTICAL | wxRIGHT, 8);
		rowSizer->Add(doneButton, 0);

		rowPanel->SetSizer(rowSizer);
		eventSizer->Add(rowPanel, 0, wxEXPAND | wxBOTTOM, 5);

		eventRows[ID_Done_Base + nextDoneId] = rowPanel;
		nextDoneId++;

		Bind(wxEVT_BUTTON, &NoteFrame::OnDone, this, ID_Done_Base + nextDoneId - 1);

		scrolledWindow->FitInside();
		scrolledWindow->Layout();
	}

	void OnDone(wxCommandEvent &event)
	{
		int buttonId = event.GetId();
		auto it = eventRows.find(buttonId);
		if (it != eventRows.end())
		{
			wxPanel *rowPanel = it->second;
			eventSizer->Detach(rowPanel);
			rowPanel->Destroy();
			eventRows.erase(it);
			scrolledWindow->FitInside();
			scrolledWindow->Layout();
		}
	}

	void CreateTaskBarIcon()
	{
		// 菜单栏图标由 Objective-C++ 代码管理
	}

	void ToggleWindowVisibility()
	{
		if (IsShown())
		{
			Hide();
		}
		else
		{
			Show();
			Raise();
			SetFocus();
		}
	}

	void OnClose(wxCloseEvent &event)
	{
		// 如果是强制关闭（来自菜单 Exit），则真正退出
		if (event.CanVeto() && !forceExit)
		{
			Hide();
			event.Veto();
		}
		else
		{
			DestroyStatusBar();
			Destroy();
			wxTheApp->ExitMainLoop();
		}
	}

	wxTextCtrl *noteInput{nullptr};
	wxScrolledWindow *scrolledWindow{nullptr};
	wxBoxSizer *eventSizer{nullptr};
	std::map<int, wxPanel *> eventRows;
	int nextDoneId = 0;
	bool forceExit = false;
};

class NoteApp : public wxApp
{
public:
	bool OnInit() override
	{
		frame = new NoteFrame();
		frame->Show(true);
		return true;
	}

	// 处理 Dock 退出或 Cmd+Q（wxApp 中不是虚函数，不能 override）
	void MacExitMenuItemCallback()
	{
		if (frame)
		{
			frame->ForceClose();
			ExitMainLoop();
		}
		else
		{
			ExitMainLoop();
		}
	}

	// 处理 Dock 点击图标重新打开（签名与 wxApp 保持一致）
	void MacReopenApp() override
	{
		if (frame)
		{
			frame->Show();
			frame->Raise();
			frame->SetFocus();
		}
	}

	int OnExit() override
	{
		DestroyStatusBar();
		return wxApp::OnExit();
	}

	static NoteFrame *frame;
};

NoteFrame *NoteApp::frame = nullptr;

// 菜单栏回调函数
void NotifyToggleWindow()
{
	if (NoteApp::frame)
	{
		if (NoteApp::frame->IsShown())
		{
			NoteApp::frame->Hide();
		}
		else
		{
			NoteApp::frame->Show();
			NoteApp::frame->Raise();
			NoteApp::frame->SetFocus();
		}
	}
}

void NotifyExitApp()
{
	if (NoteApp::frame)
	{
		NoteApp::frame->ForceClose();
		wxTheApp->ExitMainLoop();
	}
}

wxIMPLEMENT_APP(NoteApp);
