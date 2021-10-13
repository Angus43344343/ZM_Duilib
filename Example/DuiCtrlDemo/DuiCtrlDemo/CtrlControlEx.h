#pragma once

#include <future>
#include "Duilib.h"

class CCtrlControlEx : public CVirtualWnd
{
public:
	virtual void Notify(TNotifyUI& msg) override;
	virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled) override;

protected:
	void OnInitWindow(TNotifyUI& msg);

public:
	CCtrlControlEx(CPaintManagerUI* pobjManager);
	virtual ~CCtrlControlEx();

private:
	bool m_bThreadExit;
	std::future<void> m_Thread;
};

