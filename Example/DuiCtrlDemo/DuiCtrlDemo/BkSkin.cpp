#include "stdafx.h"
#include "BkSkin.h"

LPCTSTR CBkSkin::GetWindowClassName(void) const
{
	return _T("UIBkSkin");
}

CDuiString CBkSkin::GetSkinFolder()
{
	return _T("");
}

CDuiString CBkSkin::GetSkinFile()
{
	return _T("bkskin.xml");
}

void CBkSkin::OnInitWindow(void)
{
	__super::OnInitWindow();

	POINT stPoint = { 0 };
	SIZE szWndSize = m_pm.GetInitSize();

	stPoint.x = m_rectControl.right - szWndSize.cx;
	stPoint.y = m_rectControl.bottom;

	::ClientToScreen(m_hParent, &stPoint);
	::SetWindowPos(m_hWnd, nullptr, stPoint.x, stPoint.y, szWndSize.cx, szWndSize.cy, SWP_NOSIZE | SWP_NOACTIVATE);
}

void CBkSkin::OnFinalMessage(HWND hWnd)
{
	__super::OnFinalMessage(hWnd);
	delete this;
}

void CBkSkin::Notify(TNotifyUI &msg)
{
	if (DUI_MSGTYPE_CLICK == msg.sType)
	{
		if (msg.pSender->GetName().Compare(_T("btnImage")) == 0)
		{
			CSubjectSkin::GetInstance().NotifyObserver(WPARAM(EN_SKIN_BKIMAGE), (WPARAM)(msg.pSender->GetUserData().GetData()));
		}
		else if (msg.pSender->GetName().Compare(_T("btnColor")) == 0)
		{
			CSubjectSkin::GetInstance().NotifyObserver(WPARAM(EN_SKIN_BKCOLOR), (WPARAM)(msg.pSender->GetBkColor()));
		}
	}

	__super::Notify(msg);
}

LRESULT CBkSkin::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled)
{
	LRESULT lRes = 0;
	
	return lRes;
}

LRESULT CBkSkin::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	Close();

	return 0;
}