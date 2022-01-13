#include "stdafx.h"
#include "MainFrame.h"
#include "PopFrame.h"

#include "BkSkin.h"

CMainFrame::CMainFrame()
{
	RegisterVirtualWnd(DUI_VIRTUAL_WND_BASICS, new CCtrlBasics(&m_pm));
	RegisterVirtualWnd(DUI_VIRTUAL_WND_SENIOR, new CCtrlSenior(&m_pm));
	RegisterVirtualWnd(DUI_VIRTUAL_WND_LIST, new CCtrlList(&m_pm));
	RegisterVirtualWnd(DUI_VIRTUAL_WND_ACTIVEX, new CCtrlActiveX(&m_pm));
	RegisterVirtualWnd(DUI_VIRTUAL_WND_CONTROLEX, new CCtrlControlEx(&m_pm));
}

CMainFrame::~CMainFrame()
{
	PostQuitMessage(0);//程序退出 
}

//简化操作：可以直接在XML里实现option + tablayout操作
//void CMainFrame::OnTabOption(TNotifyUI &msg)
//{
//	CTabLayoutUI* pobjTabLayout = static_cast<CTabLayoutUI*>(m_pm.FindControl(_T("tabClient")));
//	if (nullptr == pobjTabLayout) return;
//
//	std::vector<CDuiString> vecPages = 
//		{_T("optCtrlBasics"), _T("optCtrlSenior"), _T("optCtrlList"), _T("optCtrlActiveX"), _T("optCtrlRedraw")};
//
//	auto atIter = std::find(vecPages.begin(), vecPages.end(), msg.pSender->GetName());
//	if (vecPages.end() != atIter) pobjTabLayout->SelectItem(std::distance(vecPages.begin(), atIter));
//}

void CMainFrame::SetLanguage(int nLangType, const LPCTSTR xml, LPCTSTR szResType)
{
	CPaintManagerUI::LoadLanguage(nLangType, xml, szResType);
	CPaintManagerUI::ChangeLanguage();
}

void CMainFrame::OnInitSystemTray()
{
	m_objSysTray.Create(&m_pm, _T("DuiCtrlDemo程序的托盘图标"), ::LoadIcon(CPaintManagerUI::GetInstance(), MAKEINTRESOURCE(IDI_ICON_TRAY)), _T("menu.xml"), _T("xml"));

	m_objSysTray.SetIconList(IDI_ICON_TRAY, IDI_ICON_TRAY_HIDE);
	m_objSysTray.Animate(300);//模拟QQ的闪烁图标效果

	m_objSysTray.SetCallbackTray(this);//如果用户需要对托盘窗口消息进行处理,可以添加回调处理
}

void CMainFrame::OnMenuClick(TNotifyUI& msg)
{
	if (CMenuWnd::s_strName.Compare(_T("menuConfig")) == 0)
	{
		CWndImplBase* pobjPopWnd = new CWndNonModal();
		if (nullptr == pobjPopWnd) return;

		pobjPopWnd->Create(m_pm.GetPaintWindow(), nullptr, WS_POPUP | WS_VISIBLE, WS_EX_TOOLWINDOW | WS_EX_TOPMOST);
		pobjPopWnd->CenterWindow();
	}
	else if (CMenuWnd::s_strName.Compare(_T("menuChinese")) == 0)
	{
		SetLanguage(EN_LANG_TYPE_CHINESE, _T("language/chinese.xml"), _T("xml"));
	}
	else if (CMenuWnd::s_strName.Compare(_T("menuEnglish")) == 0)
	{
		SetLanguage(EN_LANG_TYPE_ENGLISH, _T("language/english.xml"), _T("xml"));
	}
	else if (CMenuWnd::s_strName.Compare(_T("menuJapanese")) == 0)
	{
		SetLanguage(EN_LANG_TYPE_JAPANESE, _T("language/japanese.xml"), _T("xml"));
	}
}

void CMainFrame::OnCreateMenu(TNotifyUI& msg)
{
	CDuiPoint stPoint = msg.ptMouse;
	ClientToScreen(m_pm.GetPaintWindow(), &stPoint);
	CMenuWnd::CreateMenu(nullptr, _T("menu.xml"), _T("xml"), stPoint, &m_pm);
}

void CMainFrame::OnBkSkin(TNotifyUI& msg)
{
	CWndImplBase* pobjSkinWnd = new CBkSkin(msg.pSender->GetPos(), m_pm.GetPaintWindow());
	if (nullptr == pobjSkinWnd) return;

	pobjSkinWnd->Create(m_pm.GetPaintWindow(), nullptr, WS_POPUP | WS_VISIBLE, WS_EX_TOOLWINDOW | WS_EX_TOPMOST);
	//pobjSkinWnd->CenterWindow();
}

//=========================================================================================
//检测程序是否已存在，如果存在，则弹出该程序窗口
TCHAR g_acProName[] = _T("{E91C1C60-DC70-460D-9299-FC67422ECA23}");
HANDLE g_hValue = (HANDLE)9527;

BOOL CALLBACK EnumExistPro(HWND hWnd, LPARAM lParam)
{
	HANDLE hValue = ::GetProp(hWnd, g_acProName);
	if (g_hValue == hValue)//获取值 
	{
		*(HWND*)lParam = hWnd;//将窗口句柄传出去
		return FALSE;
	}

	return TRUE;
}

void CMainFrame::CheckProExist()
{
	HWND hWnd = nullptr;
	::EnumWindows(EnumExistPro, (LPARAM)&hWnd);
	if (nullptr != hWnd)//说明窗口存在了
	{
		::MessageBox(nullptr, _T("已经有一个程序正在运行..."), nullptr, MB_OK);
		::ShowWindow(hWnd, SW_SHOWNORMAL);
		::SetForegroundWindow(hWnd);

		ExitProcess(0);
		return;
	}

	::SetProp(m_hWnd, g_acProName, g_hValue);//设置属性
}


LPCTSTR CMainFrame::GetWindowClassName(void) const 
{
	return _T("UIDuiCtrlDemo");
}

CDuiString CMainFrame::GetSkinFolder()
{
	return _T("");
}

CDuiString CMainFrame::GetSkinFile()
{
	return _T("duilib.xml");
}

void CMainFrame::OnPrepare(void)
{

}

void CMainFrame::OnInitWindow(void)
{
	CheckProExist();//检测程序是否已经在运行

	__super::OnInitWindow();

	OnInitSystemTray();//添加系统托盘图标

	CSubjectSkin::GetInstance().AddObserver(this);//添加观察者

	//添加虚拟窗口到消息列表
	for (auto atVirtualWnd : m_vecVirtualWnd) { atVirtualWnd->AddDuiMessage(); }	
}

void CMainFrame::OnFinalMessage(HWND hWnd)
{
	__super::OnFinalMessage(hWnd);

	//移除虚拟窗口到消息列表
	for (auto atVirtualWnd : m_vecVirtualWnd) { atVirtualWnd->RemoveDuiMessage(); }

	UnRegisterVirtualWnd();

	::RemoveProp(m_hWnd, g_acProName);//删除属性
	CSubjectSkin::GetInstance().RemoveObserver(this);

	delete this;
}

void CMainFrame::Notify(TNotifyUI &msg)
{
	//if (DUI_MSGTYPE_SELECTCHANGED == msg.sType)
	//{
	//	OnTabOption(msg);
	//}
	if (DUI_MSGTYPE_CLICK == msg.sType)
	{
		if (msg.pSender->GetName().Compare(_T("btnMenu")) == 0)// button + menu测试
		{
			OnCreateMenu(msg);
		}
		else if (msg.pSender->GetName().Compare(_T("btnSkin")) == 0)//button + skin界面换肤
		{
			OnBkSkin(msg);
		}	
	}
	else if (DUI_MSGTYPE_MENUITEM_CLICK == msg.sType) // menuclick测试
	{
		OnMenuClick(msg);
	}

	__super::Notify(msg);
}

LRESULT CMainFrame::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled)
{

	return __super::MessageHandler(uMsg, wParam, lParam, bHandled);
}

HRESULT CMainFrame::MessageTray(CSystemTray* pobjSystemTray, WPARAM wParam, LPARAM lParam)
{
	if (nullptr == pobjSystemTray) return S_FALSE;

	if (WM_LBUTTONDBLCLK == LOWORD(lParam))//双击系统图标,解除QQ闪烁效果,同时弹出窗口
	{
		pobjSystemTray->StopAnimation();
		ShowWindow(true);
		::SetForegroundWindow(m_hWnd);
	}

	return S_OK;
}

bool CMainFrame::OnSubjectUpdate(WPARAM p1, WPARAM p2, LPARAM p3, CSubjectBase *pSub)
{
	CControlUI* pobjRoot = m_pm.FindControl(_T("vRoot"));
	if (nullptr == pobjRoot) return false;

	switch (p1)
	{
	case CBkSkin::EN_SKIN_BKCOLOR: { pobjRoot->SetBkColor((DWORD)p2); pobjRoot->SetBkImage(_T("")); } break;
	case CBkSkin::EN_SKIN_BKIMAGE: { pobjRoot->SetBkImage((LPCTSTR)(p2)); } break;
	default: break;
	}

	return true;
}

void CMainFrame::RegisterVirtualWnd(LPCTSTR strName, CVirtualWnd* pobjVirtualWnd)
{
	if ((nullptr == strName) || (nullptr == pobjVirtualWnd)) return;

	pobjVirtualWnd->RegisterVirtualWnd(strName);//添加虚拟窗口
	m_vecVirtualWnd.push_back(pobjVirtualWnd);
}

void CMainFrame::UnRegisterVirtualWnd()
{
	for (auto atVirtualWnd : m_vecVirtualWnd)
	{
		atVirtualWnd->UnRegisterVirtualWnd();
		if (nullptr != atVirtualWnd) delete atVirtualWnd;
	}
	m_vecVirtualWnd.clear();
}