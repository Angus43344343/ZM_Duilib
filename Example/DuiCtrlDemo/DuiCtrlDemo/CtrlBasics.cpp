#include "stdafx.h"
#include "CtrlBasics.h"
#include "PopFrame.h"

CCtrlBasics::CCtrlBasics(CPaintManagerUI* pobjManager)
{
	m_pobjManager = pobjManager;
	m_bThreadExit = false;
	m_pobjBtnModal = nullptr;
	m_pobjBtnNonModal = nullptr;
	m_pobjBtnChild = nullptr;
	m_pobjBtnDeChild = nullptr;
}

CCtrlBasics::~CCtrlBasics()
{
	m_bThreadExit = true;
	if(m_Thread.valid()) m_Thread.wait();//等待线程退出 
}

void CCtrlBasics::CreatePopWnd(bool bModal)
{
	CWndImplBase* pobjPopWnd = nullptr;

	if (bModal) { pobjPopWnd = new CWndModal(); }
	else { pobjPopWnd = new CWndNonModal(); }

	if (nullptr == pobjPopWnd) return;
	pobjPopWnd->Create(m_pobjManager->GetPaintWindow(), nullptr, WS_POPUP | WS_VISIBLE, WS_EX_TOOLWINDOW | WS_EX_TOPMOST);
	pobjPopWnd->CenterWindow();

	if (bModal) pobjPopWnd->ShowModal();
}

void CCtrlBasics::OnInitIpAddress()
{
	CIPAddressUI* pobjIpAddress = static_cast<CIPAddressUI*>(m_pobjManager->FindControl(_T("ipTest")));
	pobjIpAddress->SetIpStr(_T("192.168.1.1"));//设置初始的IP地址
}

void CCtrlBasics::OnInitProgress()
{
	CProgressUI* pobjProgress = static_cast<CProgressUI*>(m_pobjManager->FindControl(_T("proTest1")));
	//CCircleProgressUI* pobjCircleProgress = static_cast<CCircleProgressUI*>(m_pobjManager->FindControl((_T("cirproTest"))));

	m_Thread = std::async(std::launch::async, [=, this]() //线程查看progress状态
	{
		int iValue = pobjProgress->GetValue();
		while (pobjProgress->GetMaxValue() > iValue)
		{
			std::unique_lock<std::mutex> lock(CMutex::GetInstance().GetMutex());
			pobjProgress->SetValue(++iValue);
			//pobjCircleProgress->SetValue(iValue);

			if (pobjProgress->GetMaxValue() == iValue) iValue = 0;
			if (m_bThreadExit) break;

			std::this_thread::sleep_for(std::chrono::milliseconds(300));
		}
	});
}

void CCtrlBasics::OnInitHotKey()
{
	CHotKeyUI* pobjHotKeyTest = static_cast<CHotKeyUI*>(m_pobjManager->FindControl(_T("keyTest")));
	CHotKeyUI* pobjHotKeyTest1 = static_cast<CHotKeyUI*>(m_pobjManager->FindControl(_T("keyTest1")));
	if ((nullptr == pobjHotKeyTest) || (nullptr == pobjHotKeyTest1)) return;

	//初始化热键
	pobjHotKeyTest->SetHotKey('F', HOTKEYF_CONTROL | HOTKEYF_SHIFT);
	pobjHotKeyTest->RegisterHotKey();
	pobjHotKeyTest->SetText(_T("Ctrl + Shift + F"));

	//初始化热键1
	pobjHotKeyTest1->SetHotKey('K', HOTKEYF_CONTROL | HOTKEYF_ALT);
	pobjHotKeyTest1->RegisterHotKey();
	pobjHotKeyTest1->SetText(_T("Ctrl + Alt + K"));
}

void CCtrlBasics::OnInitButton()
{
	m_pobjBtnModal = static_cast<CButtonUI*>(m_pobjManager->FindControl(_T("btnModal")));
	m_pobjBtnNonModal = static_cast<CButtonUI*>(m_pobjManager->FindControl(_T("btnNonModal")));
	m_pobjBtnChild = static_cast<CButtonUI*>(m_pobjManager->FindControl(_T("btnChild")));
	m_pobjBtnDeChild = static_cast<CButtonUI*>(m_pobjManager->FindControl(_T("btnDeChild")));
}

void CCtrlBasics::OnInitWindow(TNotifyUI& msg)
{
	OnInitIpAddress();
	OnInitProgress();
	OnInitHotKey();
	OnInitButton();
}

void CCtrlBasics::OnTextLinks(TNotifyUI& msg)
{
	CDuiString astrLinks[] = { _T("www.baidu.com"), _T("www.csdn.net"), _T("https://fanyi.baidu.com/") };
	::ShellExecute(nullptr, _T("open"), astrLinks[msg.wParam], nullptr, nullptr, SW_SHOW);
}

void CCtrlBasics::OnSliderChanging(TNotifyUI& msg)
{
	CSliderUI* pobjSlider = static_cast<CSliderUI*>(m_pobjManager->FindControl(_T("sliTest")));
	CProgressUI* pobjProgress = static_cast<CProgressUI*>(m_pobjManager->FindControl(_T("proTest")));
	if ((nullptr == pobjSlider) && (nullptr == pobjProgress))return;

	pobjProgress->SetValue(pobjSlider->GetValue());
}

void CCtrlBasics::OnSliderChanged(TNotifyUI& msg)
{
	if (msg.pSender->GetName().Compare(_T("sliTest1")) != 0) return;

	CSliderUI* pobjSlider = static_cast<CSliderUI*>(m_pobjManager->FindControl(_T("sliTest1")));
	CProgressUI* pobjProgress = static_cast<CProgressUI*>(m_pobjManager->FindControl(_T("proTest")));
	if ((nullptr == pobjSlider) && (nullptr == pobjProgress))return;

	pobjProgress->SetValue(pobjSlider->GetValue());
}

void CCtrlBasics::OnHotKey(TNotifyUI& msg)
{
	if (msg.pSender->GetName().Compare(_T("keyTest")) == 0)
	{
		CreatePopWnd(true);
	}
	else if (msg.pSender->GetName().Compare(_T("keyTest1")) == 0)
	{
		CreatePopWnd(false);
	}
}

void CCtrlBasics::OnButtonClick(TNotifyUI& msg)
{
	if (msg.pSender->GetName().Compare(_T("btnModal")) == 0)
	{
		CreatePopWnd(true);
	}
	else if (msg.pSender->GetName().Compare(_T("btnNonModal")) == 0)
	{
		CreatePopWnd(false);
	}
}

void CCtrlBasics::OnEditText(TNotifyUI& msg)
{
	if (msg.pSender->GetName().Compare(_T("editPwd")) == 0)//密码检测测试：与Edit配合
	{
		CEditUI* pobjEdit = static_cast<CEditUI*>(m_pobjManager->FindControl(_T("editPwd")));
		CPwdCheckUI* pobjPwdCheck = static_cast<CPwdCheckUI*>(m_pobjManager->FindControl(_T("pwdTest")));
		if ((nullptr == pobjEdit) || (nullptr == pobjPwdCheck)) return;

		pobjPwdCheck->SetPwd(pobjEdit->GetText());
	}
}

void CCtrlBasics::Notify(TNotifyUI& msg)
{
	//虚拟窗口会捕获整个界面的所有消息，如果别的虚拟窗口出现控件名相同时，可以用下面方法进行区分
	//if (msg.sVirtualWnd.Compare(DUI_VIRTUAL_WND_BASICS) != 0) return;

	if (DUI_MSGTYPE_WINDOWINIT == msg.sType)//窗口初始化
	{
		OnInitWindow(msg);
	}
	else if (DUI_MSGTYPE_LINK == msg.sType)//text文本链接测试
	{
		OnTextLinks(msg);
	}
	else if (DUI_MSGTYPE_VALUECHANGING == msg.sType)//slider值实时变化 
	{	
		OnSliderChanging(msg);
	}
	else if (DUI_MSGTYPE_VALUECHANGED == msg.sType)//slider值鼠标弹起后变化
	{
		OnSliderChanged(msg);
	}
	else if (DUI_MSGTYPE_HOTKEY == msg.sType) //hotkey控件响应
	{
		OnHotKey(msg);
	}
	else if (DUI_MSGTYPE_CLICK == msg.sType) //button控件响应
	{
		OnButtonClick(msg);
	}
	else if (DUI_MSGTYPE_TEXTCHANGED == msg.sType) //edit控件响应
	{
		OnEditText(msg);
	}
}

LRESULT CCtrlBasics::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
	LRESULT lRes = 0;

	return lRes;
}
