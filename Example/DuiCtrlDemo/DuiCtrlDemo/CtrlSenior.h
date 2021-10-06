#pragma once

#include "Duilib.h"

class CCtrlSenior : public CVirtualWnd
{
public:
	virtual void Notify(TNotifyUI& msg) override;
	virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled) override;

private:
	void OnInitWindow(TNotifyUI& msg);
	void OnCombo(TNotifyUI& msg);
	void OnDragDrop(TNotifyUI& msg);
	void OnCreateMenu(TNotifyUI& msg);
	void OnTextChanged(TNotifyUI& msg);
	//void OnMenuClick(TNotifyUI& msg);

private:
	void OnInitEdit();

private:
	void OnDropFile(LPCTSTR lpFilePath, TNotifyUI& msg);

public:
	CCtrlSenior(CPaintManagerUI* pobjManager);
	virtual ~CCtrlSenior();
};
