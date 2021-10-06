#pragma once

#include <future>
#include "Duilib.h"

class CCtrlBasics : public CVirtualWnd
{
public:
	//Duilib消息响应
	virtual void Notify(TNotifyUI& msg) override;

	//windows窗口消息响应
	virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled) override;

private:
	//窗口消息处理
	void OnInitWindow(TNotifyUI& msg);
	void OnTextLinks(TNotifyUI& msg);
	void OnSliderChanging(TNotifyUI& msg);
	void OnSliderChanged(TNotifyUI& msg);
	void OnHotKey(TNotifyUI& msg);
	void OnButtonClick(TNotifyUI& msg);
	void OnEditText(TNotifyUI& msg);

private:
	//控件信息初始化
	void OnInitIpAddress();
	void OnInitProgress();
	void OnInitHotKey();
	void OnInitButton();

private:
	void CreatePopWnd(bool bModal);

public:
	CCtrlBasics(CPaintManagerUI* pobjManager);
	virtual ~CCtrlBasics();

private:
	bool m_bThreadExit;
	std::future<void> m_Thread;

	CButtonUI* m_pobjBtnModal;
	CButtonUI* m_pobjBtnNonModal;
	CButtonUI* m_pobjBtnChild;
	CButtonUI* m_pobjBtnDeChild;
};
