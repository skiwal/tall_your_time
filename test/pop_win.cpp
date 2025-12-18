#include <wx/wx.h>
#include <wx/taskbar.h>  // 用于顶部菜单栏图标
#include <wx/artprov.h>  // 用于获取系统默认图标（无需自定义图片）
#include <wx/wxprec.h>
 
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
// 1. 自定义顶部菜单栏图标类（继承wxTaskBarIcon）
class MyMenuBarIcon : public wxTaskBarIcon {
public:
    MyMenuBarIcon() {}

protected:
    // 处理“图标左键点击”事件
    void OnLeftClick(wxTaskBarIconEvent& event) {
        // 弹出消息弹窗
        wxMessageBox(
            "你点击了顶部图标！这是弹出的消息~",  // 消息内容
            "提示",                                // 弹窗标题
            wxOK | wxICON_INFORMATION              // 按钮+图标样式
        );
    }

    // 绑定事件（wxWidgets的事件表机制）
    DECLARE_EVENT_TABLE()
};

// 定义事件表：将“左键点击”绑定到OnLeftClick函数
BEGIN_EVENT_TABLE(MyMenuBarIcon, wxTaskBarIcon)
    EVT_TASKBAR_LEFT_DOWN(MyMenuBarIcon::OnLeftClick)
END_EVENT_TABLE()

class MyFrame : public wxFrame {
public:
    MyFrame(const wxString& title) 
        : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(400, 300)) {}
private:
    void OnQuit(wxCommandEvent& event) { Close(true); }
};

// 2. 自定义应用程序类（继承wxApp）
class MyApp : public wxApp {
public:
    virtual bool OnInit() override {
        // --- 核心：创建顶部菜单栏图标 ---
        MyMenuBarIcon* topIcon = new MyMenuBarIcon();
        // 设置图标（用系统默认的“信息图标”，无需自己准备图片）
        wxIcon icon = wxArtProvider::GetIcon(
            wxART_INFORMATION,  // 图标类型（信息、警告等）
            wxART_MENU,         // 图标用途
            wxSize(16, 16)      // 图标尺寸（Mac菜单栏图标建议16x16）
        );
        // 将图标显示到顶部菜单栏（第二个参数是“鼠标悬停时的提示文字”）
        if (!topIcon->SetIcon(icon, "我的顶部图标")) {
            wxMessageBox("顶部图标创建失败！");
            return false;
        }

        // --- 可选：隐藏主窗口（只保留顶部图标）---
        MyFrame *frame = new MyFrame("wxWidgets顶部图标演示");
        frame->Hide();  // 隐藏主窗口，只显示顶部菜单栏图标

        return true;
    }
};


// 3. 自定义主窗口类（之前的代码，可保留）



// 声明应用程序对象
wxIMPLEMENT_APP(MyApp);