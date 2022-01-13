#ifndef __WKE_BROWSER_H
#define __WKE_BROWSER_H
#pragma once

namespace DuiLib {

class DUILIB_API CWkeBrowserUI : public CControlUI, public IMessageFilterUI
{
public:
    enum
    {
        ENTY_TITLE      = 1,    // 网页标题改变
        ENTY_URL,               // 网页URL 改变
        ENTY_ALERTBOX,          // 网页弹框 提醒
        ENTY_CONFIRMBOX,        // 网页弹框 确认
        ENTY_PROMPTBOX,         // 网页弹框 提示
    };
public:
    CWkeBrowserUI(void);
    virtual ~CWkeBrowserUI(void);

    virtual LPCTSTR GetClass() const override;
    virtual LPVOID GetInterface(LPCTSTR pstrName) override;
    virtual void DoEvent(TEventUI &event) override;
    virtual void SetPos(RECT rc, bool bNeedInvalidate = true) override;
    virtual bool DoPaint(HDC hDC, const RECT &rcPaint, CControlUI *pStopControl) override;
    virtual void DoInit() override;


    void LoadUrl(LPCTSTR szUrl);
    void LoadFile(LPCTSTR szFile);
    void Reload(void);

	//C++调用JS的函数
    CDuiString RunJS(LPCTSTR strValue);
	//JS绑定C++函数
	void JsBindFunction(const CDuiString &name, jsNativeFunction fn, unsigned int argCount);
	//JS绑定C++函数:对比JsBindFunction多了一个自定义数据param
	void JsBindFunctionEx(const CDuiString &name, jsNativeFunctionEx fn, void *param, unsigned int argCount);

	// 使用示例：
	// c++里：
	//  --------
	//  jsValue JS_CALL onNativeFunction(jsExecState es) {
	//      jsValue v = jsArg(es, 0);
	//      const wchar_t* str = jsToTemdivStringW(es, v);
	//      OutdivutdebugStringW(str);
	//  }
	//  jsBindFunction("testCall", onNativeFunction， 1);
	//
	// js里：
	//  --------
	//  window.testCall('testStrt');

    bool CanGoBack();
    void GoBack();
    bool CanGoForward();
    void GoForward();

    const CDuiString &GetTitle(void) { return m_sTitle; }
    const CDuiString &GetURL(void) { return m_sURL; }

    //ENTY_ALERTBOX ENTY_CONFIRMBOX ENTY_PROMPTBOX
    const CDuiString &GetMsg(void) { return m_sMsg; }
    // ENTY_PROMPTBOX
    const CDuiString &GetDefRet(void) { return m_sDefRet; }
    const CDuiString &GetRet(void) { return m_sRet; }

    bool SendNotify(void *pWebView, int nFlag, LPCTSTR sMsg, LPCTSTR sDefRet = NULL, LPCTSTR sRet = NULL);

	//2021-10-14 zm更新鼠标在不同的网页控件上的样式
	virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

protected:
    void InitBrowser(void);
    void PaintWebContent(HDC hDC, const RECT &rcPaint);

private:
	UINT GetFlags(DWORD dwParam);
	void UpdateCursor();

protected:
    wkeWebView  m_pWeb;

    // 回调通知数据
    void       *m_pView;    //
    CDuiString  m_sTitle;   // 当前网页标题
    CDuiString  m_sURL;     // 当前网页标题
    CDuiString  m_sMsg;
    CDuiString  m_sDefRet;
    CDuiString  m_sRet;

	int m_iCurCursor;
};

}

#endif // __WKE_BROWSER_H

