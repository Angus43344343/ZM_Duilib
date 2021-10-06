#pragma once

#include "Duilib.h"

//添加换肤观察对象
class CSubjectSkin : public CSubjectBase
{
public:
	static CSubjectSkin& GetInstance() { static CSubjectSkin s_objSubjectSkin; return s_objSubjectSkin; }

public:
	CSubjectSkin(void){};
	CSubjectSkin(CSubjectSkin&) = delete;
	CSubjectSkin& operator=(CSubjectSkin&) = delete;
};


//=====================================================
class CBkSkin : public CWndImplBase
{
public:
	//定义换肤：背景图或背景色
	enum{ EN_SKIN_BKCOLOR = 1, EN_SKIN_BKIMAGE };

	//资源文件管理
	virtual LPCTSTR GetWindowClassName(void) const override;
	virtual CDuiString GetSkinFolder() override;
	virtual CDuiString GetSkinFile() override;

	//初始化界面
	virtual void OnInitWindow(void) override;

	//界面关闭信息处理
	virtual void OnFinalMessage(HWND hWnd) override;

	// INotifyUI 接口，duilib 控件通知消息
	virtual void Notify(TNotifyUI &msg) override;

	//IMessageFilterUI 接口，进行消息发送到窗口过程前的过滤处理
	virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled) override;

	//丢失焦点，就关闭窗口
	virtual LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled) override;

public:
	CBkSkin(const RECT& rectControl, HWND hWnd) : m_rectControl(rectControl), m_hParent(hWnd) {}
	virtual ~CBkSkin() = default;

private:
	HWND m_hParent;
	RECT m_rectControl;
};
