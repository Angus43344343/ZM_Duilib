#pragma once

#include <future>
#include "Duilib.h"

class CCtrlControlEx : public CVirtualWnd
{
public:
	virtual void Notify(TNotifyUI& msg) override;
	virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled) override;

	void JsRunFunction(CDuiPtrArray& arrayJsArg);

protected:
	void OnInitWindow(TNotifyUI& msg);
	void OnPaletteChange(TNotifyUI& msg);
	void OnClicked(TNotifyUI& msg);

private:
	void OnInitCircleProcess(TNotifyUI& msg);
	void OnInitEcharts(TNotifyUI& msg);
	void OnInitWkeFunction(TNotifyUI& msg);
	void OnInitWkeBrowser(TNotifyUI& msg);

public:
	CCtrlControlEx(CPaintManagerUI* pobjManager);
	virtual ~CCtrlControlEx();

private:
	bool m_bThreadExit;
	std::future<void> m_Thread;

	CEditUI* m_pobjEditWke;
	CWkeBrowserUI* m_pobjWke;
	CWkeBrowserUI* m_pobjFunction;//C++ 与 JS 相互调用
};

