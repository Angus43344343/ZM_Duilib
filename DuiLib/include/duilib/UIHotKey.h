//2017-02-25 zhuyadong 添加快捷键控件
#ifndef __UIHOTKEY_H__
#define __UIHOTKEY_H__
#pragma once

namespace DuiLib {
class CHotKeyWnd;

class DUILIB_API CHotKeyUI : public CLabelUI, public IMessageFilterUI
{
    friend class CHotKeyWnd;
public:
    CHotKeyUI();
	virtual ~CHotKeyUI();//zm

    virtual LPCTSTR GetClass() const override;
    virtual LPVOID GetInterface(LPCTSTR pstrName) override;
    virtual UINT GetControlFlags() const override;
    virtual void SetEnabled(bool bEnable = true) override;
    virtual void SetText(LPCTSTR pstrText) override;
    LPCTSTR GetNormalImage();
    void SetNormalImage(LPCTSTR pStrImage);
    LPCTSTR GetHotImage();
    void SetHotImage(LPCTSTR pStrImage);
    LPCTSTR GetFocusedImage();
    void SetFocusedImage(LPCTSTR pStrImage);
    LPCTSTR GetDisabledImage();
    void SetDisabledImage(LPCTSTR pStrImage);

	//2021-09-08 zm 解决当焦点在输入窗口时，滚动条滚动输入窗口不随控件移动的bug
	virtual void SetPos(RECT rc, bool bNeedInvalidate = true) override;
	virtual void Move(SIZE szOffset, bool bNeedInvalidate = true) override;

    virtual bool SetVisible(bool bVisible = true) override;
    virtual void SetInternVisible(bool bVisible = true) override;
    virtual SIZE EstimateSize(SIZE szAvailable) override;
    virtual void DoEvent(TEventUI &event) override;
    virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;

    virtual void PaintStatusImage(HDC hDC) override;
    virtual void PaintText(HDC hDC) override;
public:
    void GetHotKey(WORD &wVirtualKeyCode, WORD &wModifiers) const;
    DWORD GetHotKey(void) const;
    void SetHotKey(WORD wVirtualKeyCode, WORD wModifiers);

protected:
	DWORD GetNativeBkColor() const;

	//2021-09-07 zm 控件失去焦点时注册系统热键，捕获系统窗口消息WM_HOTKEY，再发送给用户处理
	//添加注册系统热键功能到控件，用户设置系统热键后，由用户来响应DUI_MSGTYPE_HOTKEY消息
public:
	//注册系统热键
	void RegisterHotKey();

	//注销系统热键
	void UnRegisterHotKey();

	//添加窗口消息响应
	virtual void DoInit();

	//窗口消息过程函数
	virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

private:
	//系统热键与传统键的shift与alt值不同, 需要转换
	UINT HotkeyToMod(UINT uiModifiers);

protected:
    CHotKeyWnd *m_pWindow;
    UINT m_uButtonState;
    TDrawInfo m_diNormal;
    TDrawInfo m_diHot;
    TDrawInfo m_diFocused;
    TDrawInfo m_diDisabled;

    WORD m_wVirtualKeyCode;
    WORD m_wModifiers;

	int m_iHotKeyId;//zm
};
}


#endif