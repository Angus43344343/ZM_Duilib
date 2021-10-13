#include "stdafx.h"
#include "CtrlActiveX.h"
#include "flash11.tlh"


CCtrlActiveX::CCtrlActiveX(CPaintManagerUI* pobjManager) 
: m_pobjWebBrowser(nullptr)
{
	m_pobjManager = pobjManager;
}


CCtrlActiveX::~CCtrlActiveX()
{
}

void CCtrlActiveX::OnInitWindow(TNotifyUI& msg)
{
	//WebBrowser��ʼ��
	m_pobjWebBrowser = static_cast<CWebBrowserUI*>(m_pobjManager->FindControl(_T("webbrowserTest")));
	if (nullptr == m_pobjWebBrowser) return;

	m_pobjWebBrowser->SetWebBrowserEventHandler(this);
	//m_pobjWebBrowser->NavigateUrl(_T("www.baidu.com"));
}

void CCtrlActiveX::OnActiveXShow(TNotifyUI& msg)
{
	ShockwaveFlashObjects::IShockwaveFlash *pFlash = nullptr;
	CActiveXUI *pCtrl = static_cast<CActiveXUI *>(msg.pSender);
	pCtrl->GetControl(__uuidof(ShockwaveFlashObjects::IShockwaveFlash), (LPVOID *)&pFlash);
	if (nullptr == pFlash) return;

	if (msg.pSender->GetName().Compare(_T("ActiveXTest")) == 0)
	{
		pFlash->put_WMode(_bstr_t(_T("Transparent")));
		pFlash->put_Movie(_bstr_t(CPaintManagerUI::GetResourcePath() + _T("flash/1.swf")));

		pFlash->Release();
	}
}

void CCtrlActiveX::OnFlashShow(TNotifyUI& msg)
{
	CFlashUI *pCtrl = static_cast<CFlashUI *>(msg.pSender);
	ShockwaveFlashObjects::IShockwaveFlash *pFlash = pCtrl->GetShockwaveFlash();//����IShockwaveFlash

	if (msg.pSender->GetName().Compare(_T("flashTest")) == 0)
	{
		//��̬����
		pCtrl->SetWMode(_T("Transparent"));
		pCtrl->SetMovie(_T("flash/1.swf"));
	}
	else if (msg.pSender->GetName().Compare(_T("flashTest1")) == 0)
	{
		//��̬����
		pCtrl->SetWMode(_T("Transparent"));
		pCtrl->SetMovie(_T("flash/2.swf"));
	}
	else if (msg.pSender->GetName().Compare(_T("flashTest2")) == 0)
	{
		//��̬����
		pCtrl->SetWMode(_T("Transparent"));
		pCtrl->SetMovie(_T("flash/3.swf"));

		//����������flash�ڲ���װ�˼����ӿڣ�put_WMode,put_Movie�ȣ����������ú�ᱻ���ǵ��ã����Դ˷��ڴ���Ч
		//pFlash->put_WMode(_bstr_t(_T("Transparent")));
		//_bstr_t strFilePath = CPaintManagerUI::GetResourcePath() + _T("flash/3.swf");
		//pFlash->put_Movie(strFilePath);

		//��������Ը������������չ�з�,��Duilib��falsh11.tlh����������Ŀ�����У�����ӿڿɲμ�flash11.tlh
		pFlash->CallFunction(nullptr, nullptr);
	}
}

void CCtrlActiveX::OnWebBrowser(TNotifyUI& msg)
{
	CWebBrowserUI* pobjWebBrowser = static_cast<CWebBrowserUI*>(msg.pSender);
	IWebBrowser2* pobjWebBroswer2 = pobjWebBrowser->GetWebBrowser2();//����WebBrowser2

	pobjWebBrowser->NavigateUrl(_T("www.baidu.com"));
}

void CCtrlActiveX::Notify(TNotifyUI& msg)
{
	//���ⴰ�ڻᲶ�����������������Ϣ�����������ⴰ�ڳ��ֿؼ�����ͬʱ�����������淽����������
	//if (msg.sVirtualWnd.Compare(DUI_VIRTUAL_WND_BASICS) != 0) return;

	if (DUI_MSGTYPE_WINDOWINIT == msg.sType)
	{
		OnInitWindow(msg);
	}
	else if (DUI_MSGTYPE_SHOWACTIVEX == msg.sType)
	{
		if (_tcscmp(msg.pSender->GetClass(), DUI_CTR_ACTIVEX) == 0)//ActiveX
		{
			OnActiveXShow(msg);
		}
		else if (_tcscmp(msg.pSender->GetClass(), DUI_CTR_FLASH) == 0)//Flash
		{
			OnFlashShow(msg);
		}
		else if (_tcscmp(msg.pSender->GetClass(), DUI_CTR_WEBBROWSER) == 0)//WebBorwser
		{
			OnWebBrowser(msg);
		}
	}
}

LRESULT CCtrlActiveX::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
	LRESULT lRes = 0;

	return lRes;
}

void CCtrlActiveX::NewWindow3(CWebBrowserUI* pWeb, IDispatch **pDisp, VARIANT_BOOL *&Cancel, DWORD dwFlags, BSTR bstrUrlContext, BSTR bstrUrl)
{
	*Cancel = 1;//Ĭ�ϵ���IE�����;����Ϊ��,�򲻵���

	if (nullptr != m_pobjWebBrowser) { m_pobjWebBrowser->NavigateUrl(bstrUrl); }
}

HRESULT STDMETHODCALLTYPE CCtrlActiveX::ShowContextMenu(CWebBrowserUI* pWeb,//�Ҽ��˵�
	/* [in] */ DWORD dwID,
	/* [in] */ POINT __RPC_FAR *ppt,
	/* [in] */ IUnknown __RPC_FAR *pcmdtReserved,
	/* [in] */ IDispatch __RPC_FAR *pdispReserved)
{
	return E_NOTIMPL;/*�����Ҽ��˵�*/
}

HRESULT STDMETHODCALLTYPE CCtrlActiveX::GetHostInfo(CWebBrowserUI* pWeb,//����IE����
	/* [out][in] */ DOCHOSTUIINFO __RPC_FAR *pInfo)
{
	//ȥ�߿�͹�����
	//if (pInfo != NULL) 
	//{
	//	pInfo->dwFlags |= DOCHOSTUIFLAG_SCROLL_NO | DOCHOSTUIFLAG_NO3DBORDER;
	//}
	//return S_OK;

	return E_NOTIMPL;
}
