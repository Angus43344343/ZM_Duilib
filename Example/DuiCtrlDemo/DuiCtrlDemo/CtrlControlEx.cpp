#include "stdafx.h"
#include "CtrlControlEx.h"


CCtrlControlEx::CCtrlControlEx(CPaintManagerUI* pobjManager)
{
	m_pobjManager = pobjManager;
	m_bThreadExit = false;
}


CCtrlControlEx::~CCtrlControlEx()
{
	m_bThreadExit = true;
	if (m_Thread.valid()) m_Thread.wait();//等待线程退出 
}

void CCtrlControlEx::OnInitWindow(TNotifyUI& msg)
{
	CCircleProgressUI* pobjCircleProgress = static_cast<CCircleProgressUI*>(m_pobjManager->FindControl((_T("cirproTest"))));

	m_Thread = std::async(std::launch::async, [=, this]() //线程查看progress状态
	{
		int iValue = pobjCircleProgress->GetValue();
		while (pobjCircleProgress->GetMaxValue() > iValue)
		{
			std::unique_lock<std::mutex> lock(CMutex::GetInstance().GetMutext());
			pobjCircleProgress->SetValue(++iValue);

			if (pobjCircleProgress->GetMaxValue() == iValue) iValue = 0;
			if (m_bThreadExit) break;

			std::this_thread::sleep_for(std::chrono::milliseconds(300));
		}
	});
}

void CCtrlControlEx::Notify(TNotifyUI& msg)
{
	//虚拟窗口会捕获整个界面的所有消息，如果别的虚拟窗口出现控件名相同时，可以用下面方法进行区分
	//if (msg.sVirtualWnd.Compare(DUI_VIRTUAL_WND_BASICS) != 0) return;

	if (DUI_MSGTYPE_WINDOWINIT == msg.sType)//窗口初始化
	{
		OnInitWindow(msg);
	}
	else if (DUI_MSGTYPE_COLORCHANGED == msg.sType)//Palette
	{
		CControlUI* pRoot = m_pobjManager->GetRoot();
		CColorPaletteUI* pobjColorPalete = static_cast<CColorPaletteUI*>(m_pobjManager->FindControl(_T("paletteTest")));
		if ((nullptr != pRoot) && (nullptr != pobjColorPalete))
		{
			pRoot->SetBkColor(pobjColorPalete->GetSelectColor());
		}
	}
}

LRESULT CCtrlControlEx::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
	LRESULT lRes = 0;

	return lRes;
}

