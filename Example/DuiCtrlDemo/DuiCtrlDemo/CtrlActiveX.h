#pragma once

#include "Duilib.h"

class CCtrlActiveX : public CVirtualWnd, public CWebBrowserEventHandler
{
public:
	virtual void Notify(TNotifyUI& msg) override;
	virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled) override;

	/*WebBrowser½Ó¿Ú*/
	virtual void NewWindow3(CWebBrowserUI* pWeb, IDispatch **pDisp, VARIANT_BOOL *&Cancel, DWORD dwFlags, BSTR bstrUrlContext, BSTR bstrUrl);

	virtual HRESULT STDMETHODCALLTYPE GetHostInfo(CWebBrowserUI* pWeb, /* [out][in] */ DOCHOSTUIINFO __RPC_FAR *pInfo);
	virtual HRESULT STDMETHODCALLTYPE ShowContextMenu(CWebBrowserUI* pWeb, DWORD dwID, POINT __RPC_FAR *ppt, IUnknown __RPC_FAR *pcmdtReserved, IDispatch __RPC_FAR *pdispReserved);
	
private:
	void OnInitWindow(TNotifyUI& msg);
	void OnActiveXShow(TNotifyUI& msg);
	void OnFlashShow(TNotifyUI& msg);
	void OnWebBrowser(TNotifyUI& msg);

public:
	CCtrlActiveX(CPaintManagerUI* pobjManager);
	virtual ~CCtrlActiveX();

private:
	CWebBrowserUI* m_pobjWebBrowser;
};

