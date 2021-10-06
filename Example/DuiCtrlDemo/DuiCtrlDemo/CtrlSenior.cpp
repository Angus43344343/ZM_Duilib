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

/*�Ϸ�һ��ANSI��ʽ��txt�ı����в��ԣ�������ܻ���������*/
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

	pobjEdit->SetCharFilter(true);//�����ַ�ƥ��
	pobjEdit->SetFilterMode(false);//������ģʽ
	pobjEdit->SetFilterCharSet(EFILTER_CHAR_LOWER);//��������Сд��ĸ'a'-'z'
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

		//1.�ӽ����ⲿ�Ϸ��ļ����ؼ���ʱ���������´���
		CDuiPtrArray ptrAry = pHelper->GetFileList();
		for (auto iNum = 0; iNum < ptrAry.GetSize(); iNum++)
		{
			LPCTSTR szFile = (LPCTSTR)ptrAry.GetAt(iNum);
			OnDropFile(szFile, msg);
			delete ptrAry.GetAt(iNum);//�ͷ���Դ
		}
		ptrAry.Empty();

		//2.��һ���ؼ��Ϸŵ���һ���ؼ���ʱ�����������¼�
		if (pHelper->HasText()) { msg.pSender->SetText(pHelper->GetText()); }//�����ı�
		if (pHelper->HasBitmap()){ HBITMAP hBMP = pHelper->GetBitmap(); } //����λͼ
		if (pHelper->HasCustomData()){ LPCTSTR pBuf = (LPCTSTR)pHelper->GetCustomData(ECF_STRUCTDATA, dwLen); }//�����Զ�������
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

		// ���һ���˵���
		CMenuElementUI *pNew = pRootMenu->NewMenuItem();
		pNew->SetName(_T("Menu_Dynamic_1"));
		pNew->SetText(_T("��̬һ���˵�"));
		pRootMenu->Add(pNew);

		// ��Ӷ����˵���
		CMenuElementUI *pItem1 = pRootMenu->FindMenuItem(_T("menuMulti"));
		pNew = pItem1->NewMenuItem();
		pNew->SetName(_T("Menu_Dynamic_2"));
		pNew->SetText(_T("��̬�����˵�"));
		pItem1->Add(pNew);
		
		//�ػ�˵���
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
	if (DUI_MSGTYPE_WINDOWINIT == msg.sType)//���ڳ�ʼ��
	{
		OnInitWindow(msg);
	}
	else if (DUI_MSGTYPE_ITEMSELECT == msg.sType) //Combo����
	{
		OnCombo(msg);
	}
	else if (DUI_MSGTYPE_DRAGDROP == msg.sType) //drop����
	{
		OnDragDrop(msg);
	}
	else if (DUI_MSGTYPE_CLICK == msg.sType)// button + menu����
	{
		OnCreateMenu(msg);
	}
	else if (DUI_MSGTYPE_TEXTCHANGED == msg.sType) //edit����
	{
		OnTextChanged(msg);
	}
	//menu���ڲ��ԣ�������ҲҪ�õ������Խ�menu����Ӧת���������ڴ���
	//else if (DUI_MSGTYPE_MENUITEM_CLICK == msg.sType) // menuclick����
	//{
	//	OnMenuClick(msg);
	//}

}

LRESULT CCtrlSenior::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
	LRESULT lRes = 0;

	return lRes;
}

