#pragma once

#include <vector>

#include "CtrlBasics.h"
#include "CtrlSenior.h"
#include "CtrlList.h"
#include "CtrlActiveX.h"
#include "CtrlControlEx.h"

class CMainFrame : public CWndImplBase, public IObserver, public ICallBackTray
{
public:
	//资源文件管理
	virtual LPCTSTR GetWindowClassName(void) const override;
	virtual CDuiString GetSkinFolder() override;
	virtual CDuiString GetSkinFile() override;

	//窗体布局第一次正确初始化后调用的接口，此时窗口将提交刷新/动画。在窗口显示前调用
	virtual void OnPrepare(void) override;

	//初始化界面
	virtual void OnInitWindow(void) override;

	//界面关闭信息处理
	virtual void OnFinalMessage(HWND hWnd) override;

	//uilib 控件通知消息
	virtual void Notify(TNotifyUI &msg) override;

	//进行消息发送到窗口过程前的过滤处理
	virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled) override;

	//系统托盘的窗口消息
	virtual HRESULT MessageTray(CSystemTray* pobjSystemTray, WPARAM wParam, LPARAM lParam) override;


	//处理观察对象变化事件
	virtual bool OnSubjectUpdate(WPARAM p1, WPARAM p2 = NULL, LPARAM p3 = NULL, CSubjectBase *pSub = NULL) override;

	//虚拟窗口管理
	void RegisterVirtualWnd(LPCTSTR strName, CVirtualWnd* pobjVirtualWnd);
	void UnRegisterVirtualWnd();

private:
	void OnInitSystemTray();
	void OnMenuClick(TNotifyUI& msg);
	void OnCreateMenu(TNotifyUI& msg);
	void OnBkSkin(TNotifyUI& msg);

	void CheckProExist();
private:
	void SetLanguage(int nLangType, const LPCTSTR xml, LPCTSTR szResType = NULL);

//private:
//	void OnTabOption(TNotifyUI &msg);

public:
	CMainFrame();
	virtual ~CMainFrame();

private:
	std::vector<CVirtualWnd*> m_vecVirtualWnd;

	CSystemTray	m_objSysTray;// 系统托盘图标
};

