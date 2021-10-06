#include "stdafx.h"
#include "CtrlSenior.h"
#include "PopFrame.h"


CCtrlSenior::CCtrlSenior(CPaintManagerUI* pobjManager)
{
	m_pobjManager = pobjManager;
}


CCtrlSenior::~CCtrlSenior()
{	
}

/*拖放一个ANSI格式的txt文本进行测试，否则可能会中文乱码*/
void CCtrlSenior::OnDropFile(LPCTSTR lpFilePath, TNotifyUI& msg)
{
	HANDLE hFile = CFileUtil::GetInstance().CreateFile(lpFilePath, GENERIC_READ | GENERIC_WRITE, OPEN_ALWAYS);
	if (INVALID_HANDLE_VALUE != hFile)
	{
		DWORD dwFileLen = CFileUtil::GetInstance().GetFileSize(hFile);
		if (0 < dwFileLen)
		{
			DWORD dwReadLen = 0;
			char* pcBuffer = new char[dwFileLen + 1];
			if (CFileUtil::GetInstance().ReadFile(hFile, pcBuffer, dwFileLen + 1, &dwReadLen))
			{	
#if _UNICODE
				ATL::CA2W objA2W(pcBuffer);
				msg.pSender->SetText((LPCTSTR)objA2W);
#else 
				msg.pSender->SetText(pcBuffer);
#endif
			}
			if (pcBuffer) delete pcBuffer;
		}
		CFileUtil::GetInstance().CloseHandle(hFile);
	}
}

void CCtrlSenior::OnInitEdit()
{
	CEditUI* pobjEdit = static_cast<CEditUI*>(m_pobjManager->FindControl(_T("editTest7")));
	if (nullptr == pobjEdit) return;

	pobjEdit->SetCharFilter(true);//开启字符匹配
	pobjEdit->SetFilterMode(false);//黑名单模式
	pobjEdit->SetFilterCharSet(EFILTER_CHAR_LOWER);//过滤所有小写字母'a'-'z'
}

void CCtrlSenior::OnInitWindow(TNotifyUI& msg)
{
	OnInitEdit();

	CComboUI* pobjCombo = static_cast<CComboUI*>(m_pobjManager->FindControl(_T("comboTest")));
	if (nullptr == pobjCombo) return;
}

void CCtrlSenior::OnCombo(TNotifyUI& msg)
{
	if (msg.pSender->GetName().Compare(_T("comboTest")) == 0)
	{
		CComboUI* pobjCombo = static_cast<CComboUI*>(m_pobjManager->FindControl(_T("comboTest")));
		CDuiString strText = pobjCombo->GetText();
	}
}

void CCtrlSenior::OnDragDrop(TNotifyUI& msg)
{
	if (msg.pSender->GetName().Compare(_T("richTest")) == 0)
	{
		DWORD dwLen = 0;
		COleDataHelper *pHelper = (COleDataHelper *)msg.wParam;

		//1.从界面外部拖放文件到控件上时，进行如下处理
		CDuiPtrArray ptrAry = pHelper->GetFileList();
		for (auto iNum = 0; iNum < ptrAry.GetSize(); iNum++)
		{
			LPCTSTR szFile = (LPCTSTR)ptrAry.GetAt(iNum);
			OnDropFile(szFile, msg);
			delete ptrAry.GetAt(iNum);//释放资源
		}
		ptrAry.Empty();

		//2.将一个控件拖放到另一个控件上时，处理如下事件
		if (pHelper->HasText()) { msg.pSender->SetText(pHelper->GetText()); }//处理文本
		if (pHelper->HasBitmap()){ HBITMAP hBMP = pHelper->GetBitmap(); } //处理位图
		if (pHelper->HasCustomData()){ LPCTSTR pBuf = (LPCTSTR)pHelper->GetCustomData(ECF_STRUCTDATA, dwLen); }//处理自定义数据
	}
}

void CCtrlSenior::OnCreateMenu(TNotifyUI& msg)
{
	if (msg.pSender->GetName().Compare(_T("btnMenuTest")) == 0)
	{
		CDuiPoint stPoint = msg.ptMouse;
		ClientToScreen(m_pobjManager->GetPaintWindow(), &stPoint);

		CMenuWnd* pMenuWnd = CMenuWnd::CreateMenu(nullptr, _T("menu.xml"), _T("xml"), stPoint, m_pobjManager);
		if (nullptr == pMenuWnd) return;
		
		CMenuUI *pRootMenu = pMenuWnd->GetMenuUI();

		// 添加一级菜单项
		CMenuElementUI *pNew = pRootMenu->NewMenuItem();
		pNew->SetName(_T("Menu_Dynamic_1"));
		pNew->SetText(_T("动态一级菜单"));
		pRootMenu->Add(pNew);

		// 添加二级菜单项
		CMenuElementUI *pItem1 = pRootMenu->FindMenuItem(_T("menuMulti"));
		pNew = pItem1->NewMenuItem();
		pNew->SetName(_T("Menu_Dynamic_2"));
		pNew->SetText(_T("动态二级菜单"));
		pItem1->Add(pNew);
		
		//重绘菜单栏
		pMenuWnd->ResizeMenu();
	}
}

void CCtrlSenior::OnTextChanged(TNotifyUI& msg)
{
	if (msg.pSender->GetName().Compare(_T("editTest")) == 0)
	{
		int i = 0;
	}
}

//void CCtrlSenior::OnMenuClick(TNotifyUI& msg)
//{
//	if (CMenuWnd::s_strName.Compare(_T("menuConfig")) == 0)
//	{
//		CWndImplBase* pobjPopWnd = new CWndNonModal();
//		if (nullptr == pobjPopWnd) return;
//
//		pobjPopWnd->Create(m_pobjManager->GetPaintWindow(), nullptr, WS_POPUP | WS_VISIBLE, WS_EX_TOOLWINDOW | WS_EX_TOPMOST);
//		pobjPopWnd->CenterWindow();
//	}
//	else if (CMenuWnd::s_strName.Compare(_T("menuListen")) == 0)
//	{
//
//	}
//}

void CCtrlSenior::Notify(TNotifyUI& msg)
{
	if (DUI_MSGTYPE_WINDOWINIT == msg.sType)//窗口初始化
	{
		OnInitWindow(msg);
	}
	else if (DUI_MSGTYPE_ITEMSELECT == msg.sType) //Combo测试
	{
		OnCombo(msg);
	}
	else if (DUI_MSGTYPE_DRAGDROP == msg.sType) //drop测试
	{
		OnDragDrop(msg);
	}
	else if (DUI_MSGTYPE_CLICK == msg.sType)// button + menu测试
	{
		OnCreateMenu(msg);
	}
	else if (DUI_MSGTYPE_TEXTCHANGED == msg.sType) //edit测试
	{
		OnTextChanged(msg);
	}
	//menu用于测试，主窗口也要用到，所以将menu的响应转换到主窗口处理
	//else if (DUI_MSGTYPE_MENUITEM_CLICK == msg.sType) // menuclick测试
	//{
	//	OnMenuClick(msg);
	//}

}

LRESULT CCtrlSenior::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
	LRESULT lRes = 0;

	return lRes;
}

