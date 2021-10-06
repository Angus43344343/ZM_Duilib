#include "stdafx.h"
#include "PopFrame.h"

LPCTSTR CWndModal::GetWindowClassName(void) const
{
	return _T("UIWndModal");
}

CDuiString CWndModal::GetSkinFolder()
{
	return _T("");
}

CDuiString CWndModal::GetSkinFile()
{
	return _T("popxml//modal.xml");
}

void CWndModal::OnInitWindow(void)
{
	__super::OnInitWindow();
}

void CWndModal::OnFinalMessage(HWND hWnd)
{
	__super::OnFinalMessage(hWnd);

	delete this;
}

void CWndModal::Notify(TNotifyUI &msg)
{

	__super::Notify(msg);
}

LRESULT CWndModal::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled)
{
	if (WM_LANGUAGE_UPDATE == uMsg)//子界面的多国语言切换
	{
		ReloadTitle();
		m_pm.UpdateLanguage();
	}

	return 0;
}

//============================================================================
LPCTSTR CWndNonModal::GetWindowClassName(void) const
{
	return _T("UIWndNonModal");
}

CDuiString CWndNonModal::GetSkinFolder()
{
	return _T("");
}

CDuiString CWndNonModal::GetSkinFile()
{
	return _T("popxml//nonmodal.xml");
}

void CWndNonModal::OnInitWindow(void)
{
	__super::OnInitWindow();

}

void CWndNonModal::OnFinalMessage(HWND hWnd)
{
	__super::OnFinalMessage(hWnd);
	delete this;
}

void CWndNonModal::Notify(TNotifyUI &msg)
{

	__super::Notify(msg);
}

LRESULT CWndNonModal::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled)
{


	return 0;
}
