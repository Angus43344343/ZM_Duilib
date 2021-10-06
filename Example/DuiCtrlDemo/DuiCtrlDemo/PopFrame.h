#pragma once

#include "Duilib.h"

class CWndModal : public CWndImplBase
{
public:
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

public:
	CWndModal() = default;
	virtual ~CWndModal() = default;
};

//====================================================================================
class CWndNonModal : public CWndImplBase
{
public:
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

public:
	CWndNonModal() = default;
	virtual ~CWndNonModal() = default;
};



