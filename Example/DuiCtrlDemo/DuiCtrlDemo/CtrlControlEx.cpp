#include "stdafx.h"
#include "CtrlControlEx.h"
#include "PopFrame.h"

CCtrlControlEx::CCtrlControlEx(CPaintManagerUI* pobjManager)
{
	m_pobjManager = pobjManager;
	m_bThreadExit = false;
	m_pobjWke = nullptr;
	m_pobjEditWke = nullptr;
	m_pobjFunction = nullptr;
}


CCtrlControlEx::~CCtrlControlEx()
{
	m_bThreadExit = true;
	if (m_Thread.valid()) m_Thread.wait();//等待线程退出 
}

void CCtrlControlEx::OnInitCircleProcess(TNotifyUI& msg)
{
	CCircleProgressUI* pobjCircleProgress = static_cast<CCircleProgressUI*>(m_pobjManager->FindControl((_T("cirproTest"))));
	if (nullptr == pobjCircleProgress) { return; }

	m_Thread = std::async(std::launch::async, [=, this]() //线程查看progress状态
	{
		int iValue = pobjCircleProgress->GetValue();
		while (pobjCircleProgress->GetMaxValue() > iValue)
		{
			std::unique_lock<std::mutex> lock(CMutex::GetInstance().GetMutex());
			pobjCircleProgress->SetValue(++iValue);

			if (pobjCircleProgress->GetMaxValue() == iValue) iValue = 0;
			if (m_bThreadExit) break;

			std::this_thread::sleep_for(std::chrono::milliseconds(300));
		}
	});
}

void CCtrlControlEx::OnInitEcharts(TNotifyUI& msg)
{
	CWkeBrowserUI* pobjLine = static_cast<CWkeBrowserUI*>(m_pobjManager->FindControl(_T("WkeLine")));
	CWkeBrowserUI* pobjBar = static_cast<CWkeBrowserUI*>(m_pobjManager->FindControl(_T("WkeBar")));
	CWkeBrowserUI* pobjPie = static_cast<CWkeBrowserUI*>(m_pobjManager->FindControl(_T("WkePie")));

	if (nullptr == pobjLine || nullptr == pobjBar || nullptr == pobjPie) return;

	pobjLine->LoadFile(CPaintManagerUI::GetResourcePath() + _T("echart//line.html"));
	pobjBar->LoadFile(CPaintManagerUI::GetResourcePath() + _T("echart//bar.html"));
	pobjPie->LoadFile(CPaintManagerUI::GetResourcePath() + _T("echart//pie.html"));
}

jsValue JS_CALL js_msgBox(jsExecState es)
{
	int n = jsArgCount(es);
	jsType t0 = jsArgType(es, 0);
	jsType t1 = jsArgType(es, 1);
	CDuiString strArg1 = jsToTempString(es, jsArgValue(es, 0));//获取JS参数1
	CDuiString strArg2 = jsToTempString(es, jsArgValue(es, 1));//获取JS参数2
	
	CWndImplBase* pobjPopWnd = new CWndNonModal();
	if (nullptr == pobjPopWnd) return jsUndefined();
	pobjPopWnd->Create(nullptr, _T("JS调用C++窗口"), WS_POPUP | WS_VISIBLE, WS_EX_TOOLWINDOW | WS_EX_TOPMOST);
	pobjPopWnd->CenterWindow();

	return jsUndefined();
}

jsValue WKE_CALL js_msgBoxEx(jsExecState es, void* param)
{
	CDuiPtrArray arrayJsArg;
	CCtrlControlEx* pobjCtrlControlEx = (CCtrlControlEx*)param;

	//获取JS传递的参数
	for (int iNum = 0; iNum < jsArgCount(es); iNum++)
	{
		//jsType tNum = jsArgType(es, iNum);
		CDuiString strArg = jsToTempString(es, jsArgValue(es, iNum));//获取参数
		arrayJsArg.Add((LPVOID)(new CDuiString(strArg)));
	}

	//将全局函数转换到类对象中,这样JS就可以处理C++的事务
	pobjCtrlControlEx->JsRunFunction(arrayJsArg);

	//清除资源数据
	for (int iCount = 0; iCount < arrayJsArg.GetSize(); iCount++)
	{
		CDuiString* pJsArg = static_cast<CDuiString*>(arrayJsArg.GetAt(iCount));
		if (nullptr != pJsArg) delete pJsArg;
	}
	arrayJsArg.Empty();

	return jsUndefined();
}

void CCtrlControlEx::OnInitWkeFunction(TNotifyUI& msg)
{
	m_pobjFunction = static_cast<CWkeBrowserUI*>(m_pobjManager->FindControl(_T("WkeFunction")));

	if (nullptr == m_pobjFunction) return;

	m_pobjFunction->JsBindFunction(_T("msgBox"), js_msgBox, 2);//这里绑定js函数，让js主动调用c++函数
	m_pobjFunction->JsBindFunctionEx(_T("msgBoxEx"), js_msgBoxEx, (void*)this, 2);//这里绑定js函数，让js主动调用c++函数
	m_pobjFunction->LoadFile(CPaintManagerUI::GetResourcePath() + _T("miniblink//function.html"));
}

void CCtrlControlEx::OnInitWkeBrowser(TNotifyUI& msg)
{
	m_pobjEditWke = static_cast<CEditUI*>(m_pobjManager->FindControl(_T("editUrl")));
	m_pobjWke = static_cast<CWkeBrowserUI*>(m_pobjManager->FindControl(_T("WkeTest")));

	if (nullptr == m_pobjWke || nullptr == m_pobjEditWke) return;

	m_pobjWke->LoadUrl(_T("www.baidu.com"));
}

void CCtrlControlEx::JsRunFunction(CDuiPtrArray& arrayJsArg)
{
	CDuiString strLabelText;
	for (int iNum = 0; iNum < arrayJsArg.GetSize(); iNum++)
	{
		CDuiString strArg;
		CDuiString* pstrArg = static_cast<CDuiString*>(arrayJsArg.GetAt(iNum));

		strArg.Format(_T("参数%d:"), iNum);
		strArg += pstrArg->GetData();

		strLabelText += strArg + _T("  ");
	}

	CLabelUI* pobjLbl = static_cast<CLabelUI*>(m_pobjManager->FindControl(_T("lblArg")));
	if (nullptr == pobjLbl) return;

	pobjLbl->SetText(strLabelText);
}

void CCtrlControlEx::OnInitWindow(TNotifyUI& msg)
{
	OnInitCircleProcess(msg);
	OnInitEcharts(msg);
	OnInitWkeFunction(msg);
	OnInitWkeBrowser(msg);
}

void CCtrlControlEx::OnPaletteChange(TNotifyUI& msg)
{
	CControlUI* pRoot = m_pobjManager->GetRoot();
	CColorPaletteUI* pobjColorPalete = static_cast<CColorPaletteUI*>(m_pobjManager->FindControl(_T("paletteTest")));
	if ((nullptr != pRoot) && (nullptr != pobjColorPalete))
	{
		pRoot->SetBkColor(pobjColorPalete->GetSelectColor());
	}
}

void CCtrlControlEx::OnClicked(TNotifyUI& msg)
{
	if (msg.pSender->GetName().Compare(_T("btnWkeBackward")) == 0) 
	{ 
		m_pobjWke->GoBack(); 
	}
	else if (msg.pSender->GetName().Compare(_T("btnWkeGo")) == 0) 
	{ 
		m_pobjWke->GoForward(); 
	}
	else if (msg.pSender->GetName().Compare(_T("btnWkeRefresh")) == 0) 
	{ 
		m_pobjWke->RunJS(m_pobjEditWke->GetText().GetData()); // C++调用JS函数
	}
	else if (msg.pSender->GetName().Compare(_T("btnWkeReturn")) == 0)
	{
		m_pobjWke->LoadUrl(m_pobjEditWke->GetText().GetData());
	}
	else if (msg.pSender->GetName().Compare(_T("btnJs")) == 0)
	{
		m_pobjFunction->RunJS(_T("myFunction('C++调用了JS函数修改属性')"));// C++调用JS函数
	}
}

void CCtrlControlEx::Notify(TNotifyUI& msg)
{
	//虚拟窗口会捕获整个界面的所有消息，如果别的虚拟窗口出现控件名相同时，可以用下面方法进行区分
	//if (msg.sVirtualWnd.Compare(DUI_VIRTUAL_WND_CONTROLEX) != 0) return;

	if (DUI_MSGTYPE_WINDOWINIT == msg.sType)//窗口初始化
	{
		OnInitWindow(msg);
	}
	else if (DUI_MSGTYPE_COLORCHANGED == msg.sType)//Palette
	{
		OnPaletteChange(msg);
	}
	else if (DUI_MSGTYPE_CLICK == msg.sType) //click
	{
		OnClicked(msg);
	}
	else if (DUI_MSGTYPE_WEBNOTIFY == msg.sType)//wke消息封装
	{
		if (CWkeBrowserUI::ENTY_URL == msg.wParam)
		{
			m_pobjEditWke->SetText(m_pobjWke->GetURL());
		}
	}
}

LRESULT CCtrlControlEx::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
	LRESULT lRes = 0;

	return lRes;
}

