﻿#include "stdafx.h"
#include "deelx.h"
#include <set>

namespace DuiLib {
const TCHAR *const ALPHABET_LOWER = _T("abcdefghijklmnopqrstuvwxyz");
const TCHAR *const ALPHABET_UPPER = _T("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
const TCHAR *const BLANK = _T(" ");
const TCHAR *const NUMBERS_BIN = _T("01");
const TCHAR *const NUMBERS_DEC = _T("0123456789");
const TCHAR *const NUMBERS_HEX = _T("0123456789abcdefABCDEF");
const TCHAR *const LINUX_NOTSUPPORT =
    _T("@#$&()*/，。！……——￥（）、？:：；“”＆～％×＠＃＋－＝＜＞，．~·《》\\|\"'[]{}<>!?; \t");
const TCHAR *const SPECIAL_SYMBOL = _T("`~!@#$%^&*()-_=+[]{};:,.<>/?'\"\\|");

// 仅数字、字符过滤、正则过滤 说明：
// 1. 大多数按键，先触发 WM_CHAR，再触发 EN_CHANGE
// 2. Delete/Shift+Delete，只触发 EN_CHANGE
// 3. 保存 TEXTCHANGED 通知消息一定在 越界检查、字符过滤、正则过滤之后
//
// 数值越界检查，不能在用户输入后立即进行，否则会导致用户无法正常输入。必须延时进行越界检查。检查后立即发送 TEXTCHANGED 通知
//     比如：输入范围限制为 [10,30]，用户输入第一个数字时，由于立即进行越界检查，会导致编辑框内容变成 10
// 字符过滤、正则过滤：在用户输入时进行，检查后，根据属性 delaytxtchange 决定怎么发送 TEXTCHANGED 通知

class CEditWnd : public CWindowWnd
{
public:
    CEditWnd();

    void Init(CEditUI *pOwner);
    RECT CalPos();

    LPCTSTR GetWindowClassName() const;
    LPCTSTR GetSuperClassName() const;
    void OnFinalMessage(HWND hWnd);

    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    LRESULT OnEditChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

private:
    bool CheckValidNumber(void);        // CEditUI 内容改变时返回true

    bool IsValidChar(LPCTSTR pstrTxt);
    bool IsRegExpMatch(LPCTSTR pstrTxt);

    // 返回当前Edit窗口的内容
    CDuiString GetWndText();

    // 发送编辑框内容改变通知；如果是 layered 窗口，触发重画
    void SendTxtChangeNty();
    // 执行过滤（字符、正则）失败，用已保存的内容，恢复Edit的内容
    void RestoreContent();

protected:
    CEditUI    *m_pOwner;
    HBRUSH      m_hBkBrush;
    bool        m_bInit;
    bool        m_bDrawCaret;
    bool        m_bEditChangeNoAction;  // true 表示在 OnEditChanged 中不做任何处理
    int         m_nCaretPos;    // 触发 WM_CHAR 消息时光标位置
#ifndef UNICODE
    BYTE m_byDChar; // 非0表示当前输入字符为双字节字符
#endif // UNICODE
};


CEditWnd::CEditWnd()
    : m_pOwner(NULL)
    , m_hBkBrush(NULL)
    , m_bInit(false)
    , m_bDrawCaret(false)
    , m_bEditChangeNoAction(false)
    , m_nCaretPos(0)
{
#ifndef UNICODE
    m_byDChar = 0;
#endif // UNICODE
}

void CEditWnd::Init(CEditUI *pOwner)
{
    m_pOwner = pOwner;
    RECT rcPos = CalPos();
    UINT uStyle = 0;

    if (pOwner->GetManager()->IsLayered())
    {
        uStyle = WS_POPUP | ES_AUTOHSCROLL | WS_VISIBLE;
        RECT rcWnd = { 0 };
        ::GetWindowRect(m_pOwner->GetManager()->GetPaintWindow(), &rcWnd);
        rcPos.left += rcWnd.left;
        rcPos.right += rcWnd.left;
        rcPos.top += rcWnd.top;
        rcPos.bottom += rcWnd.top;
    }
    else
    {
        uStyle = WS_CHILD | pOwner->GetWindowStyls();
    }

    UINT uTextStyle = m_pOwner->GetTextStyle();

    if (uTextStyle & DT_LEFT) { uStyle |= ES_LEFT; }
    else if (uTextStyle & DT_CENTER) { uStyle |= ES_CENTER; }
    else if (uTextStyle & DT_RIGHT) { uStyle |= ES_RIGHT; }

    // 2018-08-29 zhuyadong 解决多行编辑框，输入的行数不能超过高度的问题
    if (!(uTextStyle & DT_SINGLELINE)) { uStyle |= ES_MULTILINE | ES_AUTOVSCROLL; }
    else { uStyle |= ES_AUTOHSCROLL; }

    if (uTextStyle & DT_WORDBREAK) { uStyle |= ES_WANTRETURN; }

    if (m_pOwner->IsPasswordMode()) { uStyle |= ES_PASSWORD; }

    Create(m_pOwner->GetManager()->GetPaintWindow(), NULL, uStyle, 0, rcPos);

    HFONT hFont = NULL;
    int iFontIndex = m_pOwner->GetFont();

    if (iFontIndex != -1) { hFont = m_pOwner->GetManager()->GetFont(iFontIndex); }

    if (hFont == NULL) { hFont = m_pOwner->GetManager()->GetDefaultFontInfo()->hFont; }

    SetWindowFont(m_hWnd, hFont, TRUE);
    Edit_LimitText(m_hWnd, m_pOwner->GetMaxChar());

    if (m_pOwner->IsPasswordMode()) { Edit_SetPasswordChar(m_hWnd, m_pOwner->GetPasswordChar()); }

    Edit_SetText(m_hWnd, m_pOwner->GetText());
    Edit_SetModify(m_hWnd, FALSE);
    SendMessage(EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(0, 0));
    Edit_Enable(m_hWnd, m_pOwner->IsEnabled() == true);
    Edit_SetReadOnly(m_hWnd, m_pOwner->IsReadOnly() == true);

    //Styls
    ::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
    ::SetFocus(m_hWnd);

    if (m_pOwner->IsAutoSelAll())
    {
        int nSize = GetWindowTextLength(m_hWnd);

        if (nSize == 0) { nSize = 1; }

        Edit_SetSel(m_hWnd, 0, nSize);
    }
    else
    {
        int nSize = GetWindowTextLength(m_hWnd);
        Edit_SetSel(m_hWnd, nSize, nSize);
    }

    m_bInit = true;
}

RECT CEditWnd::CalPos()
{
    CDuiRect rcPos = m_pOwner->GetPos();
    RECT rcInset = m_pOwner->GetPadding();
    rcPos.left += rcInset.left;
    rcPos.top += rcInset.top;
    rcPos.right -= rcInset.right;
    rcPos.bottom -= rcInset.bottom;
    LONG lEditHeight = m_pOwner->GetManager()->GetFontInfo(m_pOwner->GetFont())->tm.tmHeight;
    UINT uText = m_pOwner->GetTextStyle();

    if ((uText & DT_SINGLELINE) && lEditHeight < rcPos.GetHeight())
    {
        rcPos.top += (rcPos.GetHeight() - lEditHeight) / 2;
        rcPos.bottom = rcPos.top + lEditHeight;
    }

    CControlUI *pParent = m_pOwner;
    RECT rcParent;

    while (pParent = pParent->GetParent())
    {
        if (!pParent->IsVisible())
        {
            rcPos.left = rcPos.top = rcPos.right = rcPos.bottom = 0;
            break;
        }

        rcParent = pParent->GetClientPos();

        if (!::IntersectRect(&rcPos, &rcPos, &rcParent))
        {
            rcPos.left = rcPos.top = rcPos.right = rcPos.bottom = 0;
            break;
        }
    }

    return rcPos;
}

LPCTSTR CEditWnd::GetWindowClassName() const
{
    return _T("EditWnd");
}

LPCTSTR CEditWnd::GetSuperClassName() const
{
    return WC_EDIT;
}

void CEditWnd::OnFinalMessage(HWND hWnd)
{
    m_pOwner->Invalidate();

    // Clear reference and die
    if (m_hBkBrush != NULL) { ::DeleteObject(m_hBkBrush); }

    m_pOwner->GetManager()->RemoveNativeWindow(hWnd);
    m_pOwner->m_pWindow = NULL;
    delete this;
}

LRESULT CEditWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT lRes = 0;
    BOOL bHandled = TRUE;
    bool bTrack = false;

    if (uMsg == WM_CREATE)
    {
        m_pOwner->GetManager()->AddNativeWindow(m_pOwner, m_hWnd);

        if (m_pOwner->GetManager()->IsLayered())
        {
            ::SetTimer(m_hWnd, TIMERID_CARET, ::GetCaretBlinkTime(), NULL);
        }

        bHandled = FALSE;
    }
    else if (uMsg == WM_KILLFOCUS) { lRes = OnKillFocus(uMsg, wParam, lParam, bHandled); }
    else if (uMsg == OCM_COMMAND)
    {
        if (GET_WM_COMMAND_CMD(wParam, lParam) == EN_CHANGE) { lRes = OnEditChanged(uMsg, wParam, lParam, bHandled); }
        else if (GET_WM_COMMAND_CMD(wParam, lParam) == EN_UPDATE)
        {
            RECT rcClient;
            ::GetClientRect(m_hWnd, &rcClient);
            ::InvalidateRect(m_hWnd, &rcClient, FALSE);
        }
    }
    //else if (uMsg == WM_KEYDOWN && TCHAR(wParam) == VK_RETURN)
	else if (uMsg == WM_KEYDOWN)//zm
    {
		//2021-10-18 zm 添加事件消息
		if (TCHAR(wParam) == VK_RETURN)
		{
			// 2019-04-24 zhuyadong 仅数字模式，按回车键时立即执行越界检查，并发送文本变化通知
			if (m_pOwner->IsNumberOnly())
			{
				CheckValidNumber() ? SendTxtChangeNty() : NULL; //lint !e62
			}

			m_pOwner->GetManager()->SendNotify(m_pOwner, DUI_MSGTYPE_RETURN);
		}
		else
		{
			m_pOwner->GetManager()->SendNotify(m_pOwner, DUI_MSGTYPE_EDITKEYDOWN, wParam, lParam);//zm
		}
    }
    else if (uMsg == OCM__BASE + WM_CTLCOLOREDIT  || uMsg == OCM__BASE + WM_CTLCOLORSTATIC)
    {
        if (m_pOwner->GetManager()->IsLayered() && !m_pOwner->GetManager()->IsPainting())
        {
            m_pOwner->GetManager()->AddNativeWindow(m_pOwner, m_hWnd);
        }

        DWORD clrColor = m_pOwner->GetNativeEditBkColor();

        // 2018-08-29 zhuyadong 解决刷新问题。问题描述：
        // 多行模式，每一行的长度大于宽度而折行；鼠标单击编辑框，此时有部分区域仍然显示CEditUI的内容。
        // if (clrColor == 0xFFFFFFFF) { return 0; }

        ::SetBkMode((HDC)wParam, TRANSPARENT);
        DWORD dwTextColor = m_pOwner->GetTextColor();
        ::SetTextColor((HDC)wParam, RGB(GetBValue(dwTextColor), GetGValue(dwTextColor), GetRValue(dwTextColor)));

        if (clrColor < 0xFF000000)
        {
            if (m_hBkBrush != NULL) { ::DeleteObject(m_hBkBrush); }

            RECT rcWnd = m_pOwner->GetManager()->GetNativeWindowRect(m_hWnd);
            HBITMAP hBmpEditBk = CRenderEngine::GenerateBitmap(m_pOwner->GetManager(), rcWnd, m_pOwner, clrColor);
            m_hBkBrush = ::CreatePatternBrush(hBmpEditBk);
            ::DeleteObject(hBmpEditBk);
        }
        else
        {
            if (m_hBkBrush == NULL)
            {
                m_hBkBrush = ::CreateSolidBrush(RGB(GetBValue(clrColor), GetGValue(clrColor), GetRValue(clrColor)));
            }
        }

        return (LRESULT)m_hBkBrush;
    }
    else if (uMsg == WM_PAINT)
    {
        if (m_pOwner->GetManager()->IsLayered())
        {
            m_pOwner->GetManager()->AddNativeWindow(m_pOwner, m_hWnd);
        }

        bHandled = FALSE;
    }
    else if (uMsg == WM_PRINT)
    {
        if (m_pOwner->GetManager()->IsLayered())
        {
            lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);

            if (m_pOwner->IsEnabled() && m_bDrawCaret)    // todo:判断是否enabled
            {
                RECT rcClient;
                ::GetClientRect(m_hWnd, &rcClient);
                POINT ptCaret;
                ::GetCaretPos(&ptCaret);
                RECT rcCaret = { ptCaret.x, ptCaret.y, ptCaret.x, ptCaret.y + rcClient.bottom - rcClient.top };
                CRenderEngine::DrawLine((HDC)wParam, rcCaret, 1, 0xFF000000);
            }

            return lRes;
        }

        bHandled = FALSE;
    }
    else if (uMsg == WM_TIMER)
    {
        if (wParam == TIMERID_CARET)
        {
            m_bDrawCaret = !m_bDrawCaret;
            RECT rcClient;
            ::GetClientRect(m_hWnd, &rcClient);
            ::InvalidateRect(m_hWnd, &rcClient, FALSE);
            return 0;
        }

        // 通知用户，编辑框内容变化
        if (TIMERID_DELAY_NTY == wParam)
        {
            KillTimer(m_hWnd, wParam);
            m_pOwner->GetManager()->SendNotify(m_pOwner, DUI_MSGTYPE_TEXTCHANGED);
            return 0;
        }

        // 检查数值范围
        if (TIMERID_NUM_CHECK == wParam)
        {
            KillTimer(m_hWnd, wParam);
            CheckValidNumber() ? SendTxtChangeNty() : NULL; //lint !e62
            return 0;
        }

        bHandled = FALSE;
    }
    else if (uMsg == WM_CHAR)
    {
        bHandled = FALSE;
        m_nCaretPos = ::SendMessage(m_hWnd, EM_GETSEL, 0, 0);
    }
    else if (uMsg == WM_PASTE)
    {
        bHandled = FALSE;
        m_nCaretPos = ::SendMessage(m_hWnd, EM_GETSEL, 0, 0);
    }
    // 2018-06-02 修复编辑框获取焦点后不显示Tooltip的问题
    else if (uMsg == WM_MOUSEMOVE)
    {
        // 父窗口的UIManager 会处理两个 WM_MOUSEMOVE 消息，后面一条消息导致找不到鼠标下的编辑框控件，也就不会显示tooltip
        // POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        // ::ClientToScreen(m_hWnd, &pt);
        // ::ScreenToClient(m_pOwner->GetManager()->GetPaintWindow(), &pt);
        // LPARAM lp = MAKELPARAM(pt.x, pt.y);
        // ::PostMessage(m_pOwner->GetManager()->GetPaintWindow(), uMsg, wParam, lp);

        if (!bTrack && !(wParam & MK_LBUTTON))
        {
            int iHoverTime = m_pOwner->GetManager()->GetHoverTime();
            TRACKMOUSEEVENT tme = { 0 };
            tme.cbSize = sizeof(TRACKMOUSEEVENT);
            tme.dwFlags = TME_HOVER | TME_LEAVE;
            tme.hwndTrack = m_hWnd;
            tme.dwHoverTime = iHoverTime;
            _TrackMouseEvent(&tme);
            bTrack = true;
        }
        else { bHandled = FALSE; }
    }
    else if (uMsg == WM_MOUSELEAVE)
    {
        bTrack = false;
    }
    else if (uMsg == WM_MOUSEHOVER)
    {
        bTrack = false;
        lRes = 1;   // 表示程序没有处理该消息

        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        ::ClientToScreen(m_hWnd, &pt);
        ::ScreenToClient(m_pOwner->GetManager()->GetPaintWindow(), &pt);
        LPARAM lp = MAKELPARAM(pt.x, pt.y);
        LRESULT pCtrl = (LRESULT)m_pOwner;   // 借用来传递控件指针，其它情况下，第4参数总是为0
        m_pOwner->GetManager()->MessageHandler(WM_MOUSEHOVER, wParam, lp, pCtrl);
    }

#ifndef UNICODE
    else if (uMsg == WM_IME_CHAR)
    {
        lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
        // 2018-03-14
        // 1. 操作系统会把一个 WM_IME_CHAR 消息转换为两个 WM_CHAR 消息
        // 2. 编辑框只有处理了两个 WM_CHAR 消息后，该字符才会出现在编辑框中
        // 3. 为了支持在编辑框中插入、选择字符后替换输入等功能
        // 该消息不作字符过滤、正则匹配工作，而是设置标识，在 WM_CHAR 消息中处理
        m_byDChar = 0 != HIBYTE(wParam) ? 1 : 0;
    }

#endif // UNICODE
    else { bHandled = FALSE; }

    if (!bHandled) { return CWindowWnd::HandleMessage(uMsg, wParam, lParam); }

    return lRes;
}

LRESULT CEditWnd::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    LRESULT lRes = ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);

    if ((HWND)wParam != m_pOwner->GetManager()->GetPaintWindow())
    {
        ::SendMessage(m_pOwner->GetManager()->GetPaintWindow(), WM_KILLFOCUS, wParam, lParam);
    }

    // 2018-08-29 zhuyadong 解决多行编辑框，用户添加新行后，显示不全的问题
    if (!(m_pOwner->GetTextStyle() & DT_SINGLELINE))
    {
        m_pOwner->m_bNeedEstimateSize = true;
        m_pOwner->NeedParentUpdate();
    }

    // 2019-04-24 zhuyadong 仅数字模式，失去焦点时立即执行越界检查，并发送文本变化通知
    if (m_pOwner->IsNumberOnly())
    {
        CheckValidNumber() ? SendTxtChangeNty() : NULL; //lint !e62
    }

    SendMessage(WM_CLOSE);
    return lRes;
}

LRESULT CEditWnd::OnEditChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/)
{
    if (!m_bInit) { return 0; }

    if (m_pOwner == NULL) { return 0; }

    m_bEditChangeNoAction = m_bEditChangeNoAction ? false : m_bEditChangeNoAction;
    CDuiString sTxt = GetWndText();

    // 2019-01-11 zhuyadong 修复数值范围限制问题。
    // 说明：假如数值范围是 40~200，则输出入 5 时立即自动变为 40，没办法正常输入。
    //if (m_pOwner->IsNumberOnly() && !GetValidNumber(pstr)) { MessageBeep(MB_ICONWARNING); }
    if (m_pOwner->IsNumberOnly())
    {
        // 2019-04-24 zhuyadong 仅数字模式支持属性设置越界检查超时时间
        SetTimer(GetHWND(), TIMERID_NUM_CHECK, m_pOwner->GetDelayValidateTime(), NULL);
        return 0;
    }

    // 字符过滤
    if (m_pOwner->IsCharFilter())
    {
        if (IsValidChar(sTxt))
        {
            m_pOwner->m_sText = sTxt;
            SendTxtChangeNty();     // 发送文本变化通知
        }
        else { RestoreContent(); }  // 恢复之前的文本

        return 0;
    }

    // 正则过滤
    // 2019-11-06 zhuyadong 解决编辑框正则匹配导致程序异常的问题
    // 原因描述：输入一个字符，正则匹配失败，恢复编辑框为空字符，而空字符正则匹配仍然失败，如此无穷循环最终程序栈溢出而崩溃。
    if (m_pOwner->IsRegExpFilter() && m_pOwner->m_sText != sTxt)
    {
        if (m_pOwner->IsRegExpMatch(sTxt))
        {
            m_pOwner->m_sText = sTxt;
            SendTxtChangeNty();     // 发送文本变化通知
        }
        else { RestoreContent(); }  // 恢复之前的文本

        return 0;
    }

    m_pOwner->m_sText = sTxt;
    SendTxtChangeNty();     // 发送文本变化通知
    return 0;
}

bool CEditWnd::CheckValidNumber()
{
    bool bRet = false;
    CDuiString sTxt = GetWndText();
    int nLen = sTxt.GetLength();
    // 2018-03-07 zhuyadong 删除数字左侧的0
    WORD wIdx = LOWORD(::SendMessage(m_hWnd, EM_GETSEL, 0, 0));

    if (nLen > 1 && 0 != wIdx)
    {
        LPTSTR pstrTxt = sTxt.GetData(0);
        int i = 0;

        for (; i < nLen; ++i)
        {
            if (_T('0') != pstrTxt[i]) { break; }
        }

        if (0 != i)
        {
            i = (i == nLen) ? --i : i;
            pstrTxt += i;
            sTxt = pstrTxt;
            bRet = true;
        }
    }

    int nValue = _ttoi(sTxt.GetData());

    if (nValue < m_pOwner->GetMinNumber())
    {
        sTxt.Format(_T("%d"), m_pOwner->GetMinNumber());
        bRet = true;
    }

    if (nValue > m_pOwner->GetMaxNumber())
    {
        sTxt.Format(_T("%d"), m_pOwner->GetMaxNumber());
        bRet = true;
    }

    if (bRet || m_pOwner->m_sText != sTxt)
    {
        // 2019-04-24 zhuyadong 这里应设置标记，在 EN_CHANGE 中使用，避免重复触发
        m_bEditChangeNoAction = true;
        bRet = true;
        m_pOwner->m_sText = sTxt;
        SetWindowText(m_hWnd, sTxt);
        ::SendMessage(m_hWnd, WM_KEYDOWN, VK_END, 0);
        ::MessageBeep(MB_ICONWARNING);
    }

    return bRet;
}

bool CEditWnd::IsValidChar(LPCTSTR pstrTxt)
{
    for (const TCHAR *pch = pstrTxt; NULL != pch && 0 != *pch; pch += 1)
    {
        if (!m_pOwner->IsValidChar(*pch)) { return false; }
    }

    return true;
}

DuiLib::CDuiString CEditWnd::GetWndText()
{
    int cchLen = ::GetWindowTextLength(m_hWnd) + 1;
    LPTSTR pstrTxt = static_cast<LPTSTR>(_alloca(cchLen * sizeof(TCHAR)));
    ASSERT(pstrTxt);

    if (pstrTxt == NULL) { return CDuiString(); }

    ::GetWindowText(m_hWnd, pstrTxt, cchLen);
    return CDuiString(pstrTxt);
}

void CEditWnd::SendTxtChangeNty()
{
    if (!m_pOwner->IsDelayTxtChange() || m_pOwner->IsNumberOnly())
    {
        m_pOwner->GetManager()->SendNotify(m_pOwner, DUI_MSGTYPE_TEXTCHANGED);
    }
    else { SetTimer(m_hWnd, TIMERID_DELAY_NTY, m_pOwner->GetDelayTxtChange(), NULL); }

	if (m_pOwner->GetManager()->IsLayered()) { m_pOwner->Invalidate(); }
}

void CEditWnd::RestoreContent()
{
    // 2019-04-24 zhuyadong 这里应设置标记，在 EN_CHANGE 中使用，避免重复触发
    m_bEditChangeNoAction = true;
    ::MessageBeep(MB_ICONWARNING);
    ::SetWindowText(m_hWnd, m_pOwner->m_sText.GetData());
    // SendMessage 在此会引发异常
    ::PostMessage(m_hWnd, EM_SETSEL, LOWORD(m_nCaretPos), HIWORD(m_nCaretPos));
}

/////////////////////////////////////////////////////////////////////////////////////
//
//

CEditUI::CEditUI()
    : m_pWindow(NULL)
    , m_uMaxChar(255)
    , m_bReadOnly(false)
    , m_bPasswordMode(false)
    , m_cPasswordChar(_T('*'))
    , m_bAutoSelAll(false)
    , m_uButtonState(0)
    , m_iWindowStyls(0)
    , m_dwTipColor(0xFFBAC0C5)
    , m_nMinNumber(0)
    , m_nMaxNumber(0)
    , m_bCharFilter(false)
    , m_bWiteList(true)
    , m_bRegExp(false)
    , m_pRegExp(NULL)
    , m_dwDelayTxtChangeTime(0)
    , m_dwDelayValidateTime(2000)
{
    SetPadding(CDuiRect(4, 3, 4, 3));
    SetBkColor(0xFFFFFFFF);
}

CEditUI::~CEditUI()
{
    if (NULL != m_pRegExp)
    {
        delete m_pRegExp;
        m_pRegExp = NULL;
    }
}

LPCTSTR CEditUI::GetClass() const
{
    return DUI_CTR_EDIT;
}

LPVOID CEditUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcscmp(pstrName, DUI_CTR_EDIT) == 0) { return static_cast<CEditUI *>(this); }

    return CLabelUI::GetInterface(pstrName);
}

UINT CEditUI::GetControlFlags() const
{
    if (!IsEnabled()) { return CControlUI::GetControlFlags(); }

    return UIFLAG_SETCURSOR | UIFLAG_TABSTOP;
}

HWND CEditUI::GetNativeWindow() const
{
    return (NULL != m_pWindow) ? m_pWindow->GetHWND() : NULL;
}

void CEditUI::DoEvent(TEventUI &event)
{
    if (!IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND)
    {
        if (m_pParent != NULL) { m_pParent->DoEvent(event); }
        else { CLabelUI::DoEvent(event); }

        return;
    }

    if (event.Type == UIEVENT_SETCURSOR && IsEnabled())
    {
        ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_IBEAM)));
        return;
    }

    if (event.Type == UIEVENT_WINDOWSIZE)
    {
        //if (m_pWindow != NULL) { m_pManager->SetFocusNeeded(this); }
        if (NULL != m_pManager) { m_pManager->SetFocusNeeded(this); }
    }

    if (event.Type == UIEVENT_SCROLLWHEEL)
    {
        if (m_pWindow != NULL) { return; }
    }

    if (event.Type == UIEVENT_SETFOCUS && IsEnabled())
    {
        if (m_pWindow) { return; }

        m_pWindow = new CEditWnd();
        ASSERT(m_pWindow);
        m_pWindow->Init(this);
        Invalidate();
    }

    if (event.Type == UIEVENT_KILLFOCUS && IsEnabled())
    {
        Invalidate();
    }

    if (event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_LBUTTONDBLDOWN ||
        event.Type == UIEVENT_RBUTTONDOWN)
    {
        if (IsEnabled())
        {
            // 2018-05-28 原来代码在manager中捕获，在此释放。修改Capture机制后，由控件自己决定是否捕获
            // ReleaseCapture();

            if (IsFocused() && m_pWindow == NULL)
            {
                m_pWindow = new CEditWnd();
                ASSERT(m_pWindow);
                m_pWindow->Init(this);
            }
            else if (m_pWindow != NULL)
            {
                if (!m_bAutoSelAll)
                {
                    POINT pt = event.ptMouse;
                    pt.x -= m_rcItem.left + m_rcPadding.left;
                    pt.y -= m_rcItem.top + m_rcPadding.top;
                    Edit_SetSel(*m_pWindow, 0, 0);
                    ::SendMessage(*m_pWindow, WM_LBUTTONDOWN, event.wParam, MAKELPARAM(pt.x, pt.y));
                }
            }
        }

        return;
    }

    if (event.Type == UIEVENT_MOUSEMOVE)
    {
        if ((event.wParam & MK_LBUTTON) && m_bDragEnable) { OnDoDragDrop(event); }

        return;
    }

    if (event.Type == UIEVENT_BUTTONUP)
    {
        return;
    }

    if (event.Type == UIEVENT_CONTEXTMENU)
    {
        return;
    }

    if (event.Type == UIEVENT_MOUSEENTER)
    {
        if (::PtInRect(&m_rcItem, event.ptMouse))
        {
            if (IsEnabled())
            {
                if ((m_uButtonState & UISTATE_HOT) == 0)
                {
                    m_uButtonState |= UISTATE_HOT;
                    Invalidate();
                }
            }
        }
    }

    if (event.Type == UIEVENT_MOUSELEAVE)
    {
        if (!::PtInRect(&m_rcItem, event.ptMouse))
        {
            if (IsEnabled())
            {
                if ((m_uButtonState & UISTATE_HOT) != 0)
                {
                    m_uButtonState &= ~UISTATE_HOT;
                    Invalidate();
                }
            }

            if (m_pManager) { m_pManager->RemoveMouseLeaveNeeded(this); }
        }
        else
        {
            if (m_pManager) { m_pManager->AddMouseLeaveNeeded(this); }

            return;
        }
    }

    CLabelUI::DoEvent(event);
}

void CEditUI::SetEnabled(bool bEnable)
{
    CControlUI::SetEnabled(bEnable);

    if (!IsEnabled())
    {
        m_uButtonState = 0;
    }
}

void CEditUI::SetText(LPCTSTR pstrText)
{
    // 2018-08-29 zhuyadong 把 \r 或 \n 统一替换为 \r\n ，以解决该控件与本地编辑框多行模式显示不一致的问题
    CDuiString sTmp;

    while (pstrText && *pstrText != _T('\0'))
    {
        if (*pstrText == _T('\r') || *pstrText == _T('\n'))
        {
            sTmp += _T("\r\n");
            pstrText += 1;

            if (*pstrText == _T('\r') || *pstrText == _T('\n')) { pstrText += 1; }
        }
        else
        {
            sTmp += *pstrText;
            pstrText += 1;
        }
    }

    CLabelUI::SetText(sTmp);

    if (m_pWindow != NULL)
    {
        Edit_SetText(*m_pWindow, m_sText);
        int nSize = GetWindowTextLength(*m_pWindow);
        Edit_SetSel(*m_pWindow, nSize, nSize);
    }

    // 2018-08-29 zhuyadong 多行编辑框，更新文本后，自动重新计算宽高
    if (!(m_uTextStyle & DT_SINGLELINE)) { NeedParentUpdate(); }
}

void CEditUI::SetMaxChar(UINT uMax)
{
    m_uMaxChar = uMax;

    if (m_pWindow != NULL) { Edit_LimitText(*m_pWindow, m_uMaxChar); }
}

UINT CEditUI::GetMaxChar()
{
    return m_uMaxChar;
}

void CEditUI::SetReadOnly(bool bReadOnly)
{
    if (m_bReadOnly == bReadOnly) { return; }

    m_bReadOnly = bReadOnly;

    if (m_pWindow != NULL) { Edit_SetReadOnly(*m_pWindow, m_bReadOnly); }

    Invalidate();
}

bool CEditUI::IsReadOnly() const
{
    return m_bReadOnly;
}

void CEditUI::SetNumberOnly(bool bNumberOnly)
{
    if (bNumberOnly)
    {
        m_iWindowStyls |= ES_NUMBER;
    }
    else
    {
        m_iWindowStyls &= ~ES_NUMBER;
    }
}

bool CEditUI::IsNumberOnly() const
{
    return m_iWindowStyls & ES_NUMBER ? true : false;
}

int CEditUI::GetWindowStyls() const
{
    return m_iWindowStyls;
}

void CEditUI::SetPasswordMode(bool bPasswordMode)
{
    if (m_bPasswordMode == bPasswordMode) { return; }

    m_bPasswordMode = bPasswordMode;
    Invalidate();
}

bool CEditUI::IsPasswordMode() const
{
    return m_bPasswordMode;
}

void CEditUI::SetPasswordChar(TCHAR cPasswordChar)
{
    if (m_cPasswordChar == cPasswordChar) { return; }

    m_cPasswordChar = cPasswordChar;

    if (m_pWindow != NULL) { Edit_SetPasswordChar(*m_pWindow, m_cPasswordChar); }

    Invalidate();
}

TCHAR CEditUI::GetPasswordChar() const
{
    return m_cPasswordChar;
}

bool CEditUI::IsAutoSelAll()
{
    return m_bAutoSelAll;
}

void CEditUI::SetAutoSelAll(bool bAutoSelAll)
{
    m_bAutoSelAll = bAutoSelAll;
}

LPCTSTR CEditUI::GetNormalImage()
{
    return m_diNormal.sDrawString;
}

void CEditUI::SetNormalImage(LPCTSTR pStrImage)
{
    if (m_diNormal.sDrawString == pStrImage && m_diNormal.pImageInfo != NULL) { return; }

    m_diNormal.Clear();
    m_diNormal.sDrawString = pStrImage;
    Invalidate();
}

LPCTSTR CEditUI::GetHotImage()
{
    return m_diHot.sDrawString;
}

void CEditUI::SetHotImage(LPCTSTR pStrImage)
{
    if (m_diHot.sDrawString == pStrImage && m_diHot.pImageInfo != NULL) { return; }

    m_diHot.Clear();
    m_diHot.sDrawString = pStrImage;
    Invalidate();
}

LPCTSTR CEditUI::GetFocusedImage()
{
    return m_diFocused.sDrawString;
}

void CEditUI::SetFocusedImage(LPCTSTR pStrImage)
{
    if (m_diFocused.sDrawString == pStrImage && m_diFocused.pImageInfo != NULL) { return; }

    m_diFocused.Clear();
    m_diFocused.sDrawString = pStrImage;
    Invalidate();
}

LPCTSTR CEditUI::GetDisabledImage()
{
    return m_diDisabled.sDrawString;
}

void CEditUI::SetDisabledImage(LPCTSTR pStrImage)
{
    if (m_diDisabled.sDrawString == pStrImage && m_diDisabled.pImageInfo != NULL) { return; }

    m_diDisabled.Clear();
    m_diDisabled.sDrawString = pStrImage;
    Invalidate();
}

DWORD CEditUI::GetNativeEditBkColor() const
{
    return GetBkColor();
}

DWORD CEditUI::GetSel()
{
	return (m_pWindow != NULL) ? Edit_GetSel(*m_pWindow) : 0;
}

void CEditUI::SetSel(long nStartChar, long nEndChar)
{
    if (m_pWindow != NULL) { Edit_SetSel(*m_pWindow, nStartChar, nEndChar); }
}

void CEditUI::SetSelAll()
{
    SetSel(0, -1);
}

void CEditUI::SetReplaceSel(LPCTSTR lpszReplace)
{
    if (m_pWindow != NULL) { Edit_ReplaceSel(*m_pWindow, lpszReplace); }
}

void CEditUI::SetPos(RECT rc, bool bNeedInvalidate)
{
    CControlUI::SetPos(rc, bNeedInvalidate);

    if (m_pWindow != NULL)
    {
        RECT rcPos = m_pWindow->CalPos();

        if (::IsRectEmpty(&rcPos)) { ::ShowWindow(m_pWindow->GetHWND(), SW_HIDE); }
        else
        {
            ::SetWindowPos(m_pWindow->GetHWND(), NULL, rcPos.left, rcPos.top, rcPos.right - rcPos.left,
                           rcPos.bottom - rcPos.top, SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
        }
    }
}

void CEditUI::Move(SIZE szOffset, bool bNeedInvalidate)
{
    CControlUI::Move(szOffset, bNeedInvalidate);

    if (m_pWindow != NULL)
    {
        RECT rcPos = m_pWindow->CalPos();

        if (::IsRectEmpty(&rcPos)) { ::ShowWindow(m_pWindow->GetHWND(), SW_HIDE); }
        else
        {
            ::SetWindowPos(m_pWindow->GetHWND(), NULL, rcPos.left, rcPos.top, rcPos.right - rcPos.left,
                           rcPos.bottom - rcPos.top, SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
        }
    }
}

bool CEditUI::SetVisible(bool bVisible /*= true*/)
{
    // 2018-08-18 zhuyadong 添加特效
    if (!CControlUI::SetVisible(bVisible)) { return false; }

    if (!IsVisible() && m_pWindow != NULL) { m_pManager->SetFocus(NULL); }

    return true;
}

void CEditUI::SetInternVisible(bool bVisible)
{
    if (!IsVisible() && m_pWindow != NULL) { m_pManager->SetFocus(NULL); }
}

SIZE CEditUI::EstimateSize(SIZE szAvailable)
{
    if (!IsMultiLine())
    {
        if (m_cxyFixed.cy == 0) { return CDuiSize(m_cxyFixed.cx, m_pManager->GetFontInfo(GetFont())->tm.tmHeight + 8); }
    }

    return CLabelUI::EstimateSize(szAvailable);
}

void CEditUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if (_tcscmp(pstrName, _T("readonly")) == 0) { SetReadOnly(ParseBool(pstrValue)); }
    else if (_tcscmp(pstrName, _T("numberonly")) == 0) { SetNumberOnly(ParseBool(pstrValue)); }
    else if (_tcscmp(pstrName, _T("password")) == 0) { SetPasswordMode(ParseBool(pstrValue)); }
	//2021-09-08 zm 添加passwordchar 系统默认为'*',允许用户根据需求定制
	else if (_tcscmp(pstrName, _T("passwordchar")) == 0) { SetPasswordChar(ParseTChar(pstrValue)); }
    else if (_tcscmp(pstrName, _T("autoselall")) == 0) { SetAutoSelAll(ParseBool(pstrValue)); }
    else if (_tcscmp(pstrName, _T("maxchar")) == 0) { SetMaxChar(ParseInt(pstrValue)); }
    else if (_tcscmp(pstrName, _T("normalimage")) == 0) { SetNormalImage(ParseString(pstrValue)); }
    else if (_tcscmp(pstrName, _T("hotimage")) == 0) { SetHotImage(ParseString(pstrValue)); }
    else if (_tcscmp(pstrName, _T("focusedimage")) == 0) { SetFocusedImage(ParseString(pstrValue)); }
    else if (_tcscmp(pstrName, _T("disabledimage")) == 0) { SetDisabledImage(ParseString(pstrValue)); }
    else if (_tcscmp(pstrName, _T("tiptext")) == 0) { SetTipText(pstrValue); }  // 2019-01-10 zhuyadong 不做任何处理
    else if (_tcscmp(pstrName, _T("tipcolor")) == 0)
    {
        DWORD clr = ParseColor(pstrValue);
        SetTipColor(clr);
    }
    // 2017-07-21 zhuyadong 添加 minmaxnumber 属性
    else if (_tcscmp(pstrName, _T("minmaxnumber")) == 0)
    {
        SIZE sz = ParseSize(pstrValue);
        SetMinMaxNumber(sz.cx, sz.cy);
    }
    else if (_tcscmp(pstrName, _T("charfilter")) == 0)
    {
        // 是否支持字符过滤
        CDuiString str(pstrValue);
        int nPos = str.Find(_T(','));

        if (-1 != nPos)
        {
            SetCharFilter(ParseBool(str.Left(nPos).GetData()));
            str = str.Right(str.GetLength() - nPos - 1);
        }
        else
        {
            SetCharFilter(ParseBool(str.GetData()));
            str.Empty();
        }

        // 黑白名单模式
        if (!str.IsEmpty())
        {
            nPos = str.Find(_T(','));

            if (-1 != nPos)
            {
                m_bWiteList = ParseBool(str.Left(nPos).GetData());
                str = str.Right(str.GetLength() - nPos - 1);
            }
            else
            {
                m_bWiteList = ParseBool(str.GetData());
                str.Empty();
            }
        }

        // 过滤字符集合
        if (!str.IsEmpty()) { SetFilterCharSet(str); }
    }
    // else if (_tcscmp(pstrName, _T("whitelist")) == 0)
    // {
    //     m_bWiteList = ParseBool(pstrValue);
    // }
    else if (_tcscmp(pstrName, _T("regexp")) == 0)
    {
        // 是否支持正则过滤
        CDuiString str(pstrValue);
        int nPos = str.Find(_T(','));

        if (-1 != nPos)
        {
            SetRegExpFilter(ParseBool(str.Left(nPos).GetData()));
            str = str.Right(str.GetLength() - nPos - 1);
        }
        else
        {
            SetRegExpFilter(ParseBool(str.GetData()));
            str.Empty();
        }

        // 设置正则表达式
        SetRegExpPattern(str);
    }
    else if (_tcscmp(pstrName, _T("delaytxtchange")) == 0) { m_dwDelayTxtChangeTime = ParseDWord(pstrValue); }
    else if (_tcscmp(pstrName, _T("delayvalidate")) == 0)
    {
        m_dwDelayValidateTime = ParseDWord(pstrValue);
        // m_dwDelayValidateTime = m_dwDelayValidateTime >= 300 ? m_dwDelayValidateTime : 300;
    }
    else if (_tcscmp(pstrName, _T("dragenable")) == 0) { DUITRACE(_T("不支持属性:dragenable")); }
    else if (_tcscmp(pstrName, _T("dragimage")) == 0) { DUITRACE(_T("不支持属性:drageimage")); }
    else if (_tcscmp(pstrName, _T("enabledeffect")) == 0) { DUITRACE(_T("不支持属性:enabledeffect")); }
    else if (_tcscmp(pstrName, _T("enabledluminous")) == 0) { DUITRACE(_T("不支持属性:enabledluminous")); }
    else if (_tcscmp(pstrName, _T("luminousfuzzy")) == 0) { DUITRACE(_T("不支持属性:luminousfuzzy")); }
    else if (_tcscmp(pstrName, _T("gradientangle")) == 0) { DUITRACE(_T("不支持属性:gradientangle")); }
    else if (_tcscmp(pstrName, _T("enabledstroke")) == 0) { DUITRACE(_T("不支持属性:enabledstroke")); }
    else if (_tcscmp(pstrName, _T("strokecolor")) == 0) { DUITRACE(_T("不支持属性:strokecolor")); }
    else if (_tcscmp(pstrName, _T("enabledshadow")) == 0) { DUITRACE(_T("不支持属性:enabledshadow")); }
    else if (_tcscmp(pstrName, _T("gradientlength")) == 0) { DUITRACE(_T("不支持属性:gradientlength")); }
    else if (_tcscmp(pstrName, _T("shadowoffset")) == 0) { DUITRACE(_T("不支持属性:shadowoffset")); }
    else if (_tcscmp(pstrName, _T("textcolor1")) == 0) { DUITRACE(_T("不支持属性:textcolor1")); }
    else if (_tcscmp(pstrName, _T("textshadowcolora")) == 0) { DUITRACE(_T("不支持属性:textshadowcolora")); }
    else if (_tcscmp(pstrName, _T("textshadowcolorb")) == 0) { DUITRACE(_T("不支持属性:textshadowcolorb")); }
    else if (_tcscmp(pstrName, _T("whitelist")) == 0) { DUITRACE(_T("不支持属性:whitelist，详见 charfilter")); }
	//2021-09-08 zm Edit不支持多行操作
	else if (_tcscmp(pstrName, _T("multiline")) == 0) { DUITRACE(_T("不支持属性:multiline")); }
    else { CLabelUI::SetAttribute(pstrName, pstrValue); }
}

void CEditUI::PaintStatusImage(HDC hDC)
{
    if (IsFocused()) { m_uButtonState |= UISTATE_FOCUSED; }
    else { m_uButtonState &= ~ UISTATE_FOCUSED; }

    if (!IsEnabled()) { m_uButtonState |= UISTATE_DISABLED; }
    else { m_uButtonState &= ~ UISTATE_DISABLED; }

    if ((m_uButtonState & UISTATE_DISABLED) != 0)
    {
        if (DrawImage(hDC, m_diDisabled)) { return; }
    }
    else if ((m_uButtonState & UISTATE_FOCUSED) != 0)
    {
        if (DrawImage(hDC, m_diFocused)) { return; }
    }
    else if ((m_uButtonState & UISTATE_HOT) != 0)
    {
        if (DrawImage(hDC, m_diHot)) { return; }
    }

    if (DrawImage(hDC, m_diNormal)) { return; }
}

void CEditUI::PaintText(HDC hDC)
{
    if (m_dwTextColor == 0) { m_dwTextColor = m_pManager->GetDefaultFontColor(); }

    if (m_dwDisabledTextColor == 0) { m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor(); }

    DWORD mCurTextColor = m_dwTextColor;
    CDuiString sText;

    if (m_sText.IsEmpty() && !m_sTipText.IsEmpty())
    {
        mCurTextColor = m_dwTipColor;
        sText = m_sTipText;
    }
    else
    {
        if (m_bPasswordMode)
        {
            LPCTSTR p = m_sText.GetData();

            while (*p != _T('\0'))
            {
                sText += m_cPasswordChar;
                p = ::CharNext(p);
            }
        }
        else
        {
            sText = m_sText;
        }
    }

    if (sText.IsEmpty()) { return; }

    RECT rc = m_rcItem;
    rc.left += m_rcPadding.left;
    rc.right -= m_rcPadding.right;
    rc.top += m_rcPadding.top;
    rc.bottom -= m_rcPadding.bottom;

    if (IsEnabled())
    {
        CRenderEngine::DrawText(hDC, m_pManager, rc, sText, mCurTextColor, m_iFont, m_uTextStyle);
    }
    else
    {
        CRenderEngine::DrawText(hDC, m_pManager, rc, sText, m_dwDisabledTextColor, m_iFont, m_uTextStyle);
    }
}

//2017-02-25 zhuyadong 添加提示文字及其颜色
void CEditUI::SetTipText(LPCTSTR pstrTip)
{
    m_sTipText = pstrTip;
	//2021-09-10 zm 提示文字不显示的问题
    //m_sTipTextOrig = m_sText;
	m_sTipTextOrig = m_sText.IsEmpty() ? m_sTipText : m_sText;
    CPaintManagerUI::ProcessMultiLanguageTokens(m_sTipText);
    Invalidate();
}

CDuiString CEditUI::GetTipText() const
{
    return m_sTipText.GetData();
}

void CEditUI::ReloadText(void)
{
	CLabelUI::ReloadText();
    SetTipText(m_sTipTextOrig);
}

void CEditUI::SetTipColor(DWORD dwColor)
{
    m_dwTipColor = dwColor;
}

DWORD CEditUI::GetTipColor()
{
    return m_dwTipColor;
}
// 2017-07-21 zhuyadong 添加 minmaxnumber 属性
void CEditUI::SetMinMaxNumber(int nMin, int nMax)
{
    m_nMinNumber = nMin;
    m_nMaxNumber = nMax;
}
int CEditUI::GetMinNumber()
{
    return m_nMinNumber;
}
int CEditUI::GetMaxNumber()
{
    return m_nMaxNumber;
}

void CEditUI::SetCharFilter(bool bFilter)
{
    ASSERT(!m_bRegExp);

    if (!m_bRegExp)
    {
        m_bCharFilter = bFilter;
        m_iWindowStyls = bFilter ? (m_iWindowStyls &= ~ES_NUMBER) : m_iWindowStyls;
    }
}

bool CEditUI::IsCharFilter(void)
{
    return m_bCharFilter;
}

void CEditUI::SetFilterMode(bool bWiteList)
{
    m_bWiteList = bWiteList;
}

bool CEditUI::GetFilterMode(void)
{
    return m_bWiteList;
}

bool CEditUI::IsValidChar(TCHAR ch)
{
    int nFind = m_sFilterCharSet.Find(ch);

    if (m_bWiteList)
    {
        return (nFind > -1) ? true : false;
    }
    else
    {
        return (nFind < 0) ? true : false;
    }
}

#ifndef UNICODE
bool CEditUI::IsValidChar(LPCTSTR pstr)
{
    int nFind = m_sFilterCharSet.Find(pstr);

    if (m_bWiteList)
    {
        return (nFind > -1) ? true : false;
    }
    else
    {
        return (nFind < 0) ? true : false;
    }
}
#endif // UNICODE

DuiLib::CDuiString CEditUI::GetFilterCharSet(void)
{
    return m_sFilterCharSet;
}

void CEditUI::SetFilterCharSet(CDuiString sCharSet)
{
    std::set<TCHAR> setChar;
    m_sFilterCharSet.Empty();

    for (int i = 0; i < sCharSet.GetLength(); ++i) { setChar.insert(sCharSet[i]); }

    for (auto it(setChar.begin()); it != setChar.end(); ++it)
    {
        m_sFilterCharSet += *it;
    }
}

void CEditUI::SetFilterCharSet(int nFilterType)
{
    m_sFilterCharSet = _T("");

    if (nFilterType & EFILTER_CHAR_UPPER)
    {
        m_sFilterCharSet += ALPHABET_UPPER;
    }

    if (nFilterType & EFILTER_CHAR_LOWER)
    {
        m_sFilterCharSet += ALPHABET_LOWER;
    }

    if (nFilterType & EFILTER_CHAR_BLANK)
    {
        m_sFilterCharSet += BLANK;
    }

    if (nFilterType & EFILTER_SPECIAL_SYMBOL)
    {
        m_sFilterCharSet += SPECIAL_SYMBOL;
    }

    if (nFilterType & EFILTER_CHAR_LINUX)
    {
        m_sFilterCharSet += LINUX_NOTSUPPORT;
    }

    if (nFilterType & EFILTER_NUMBER_BIN)
    {
        m_sFilterCharSet += NUMBERS_BIN;
    }

    if (nFilterType & EFILTER_NUMBER_DEC)
    {
        m_sFilterCharSet += NUMBERS_DEC;
    }

    if (nFilterType & EFILTER_NUMBER_HEX)
    {
        m_sFilterCharSet += NUMBERS_HEX;
    }
}

void CEditUI::AppendFilterCharSet(CDuiString sCharSet)
{
    TCHAR ch;

    for (int i = 0; i < sCharSet.GetLength(); ++i)
    {
        ch = sCharSet.GetAt(i);

        if (m_sFilterCharSet.Find(ch) < 0)
        {
            m_sFilterCharSet += ch;
        }
    }
}

void CEditUI::SetRegExpFilter(bool bRegExp)
{
    ASSERT(!m_bCharFilter);

    if (!m_bCharFilter)
    {
        m_bRegExp = bRegExp;
        m_iWindowStyls = bRegExp ? (m_iWindowStyls &= ~ES_NUMBER) : m_iWindowStyls;
    }
}

bool CEditUI::IsRegExpFilter(void)
{
    return m_bRegExp;
}

void CEditUI::SetRegExpPattern(CDuiString sRegExp)
{
    if (NULL == m_pRegExp) { m_pRegExp = new CRegexpT<TCHAR>(NULL, 0); }

    if (NULL != m_pRegExp)
    {
        //CDuiString sRE;
        //sRE.Append(_T("\\Q")).Append(sRegExp).Append(_T("\\E"));
        //m_pRegExp->Compile(sRE.GetData());
        if (sRegExp.GetAt(0) != _T('^')) { sRegExp.Format(_T("%c%s"), _T('^'), sRegExp); }

        if (sRegExp.GetAt(sRegExp.GetLength() - 1) != _T('$')) { sRegExp.Append(_T("$")); }

        m_pRegExp->Compile(sRegExp.GetData());
    }
}

bool CEditUI::IsRegExpMatch(TCHAR ch)
{
    if (NULL == m_pRegExp) { return false; }

    CDuiString sTxt = m_sText;
    sTxt += ch;

    int result = m_pRegExp->MatchExact(sTxt.GetData()).IsMatched();
    return (0 != result) ? true : false;
}

bool CEditUI::IsRegExpMatch(LPCTSTR pstr)
{
    if (NULL == m_pRegExp || NULL == pstr) { return false; }

    int result = m_pRegExp->MatchExact(pstr).IsMatched();
    return (0 != result) ? true : false;
}

bool CEditUI::IsDelayTxtChange()
{
    return (0 != m_dwDelayTxtChangeTime) ? true : false;
}

DWORD CEditUI::GetDelayTxtChange()
{
    return m_dwDelayTxtChangeTime;
}

void CEditUI::SetDelayTxtChange(DWORD dwMiliSec)
{
    m_dwDelayTxtChangeTime = dwMiliSec;
}

DWORD CEditUI::GetDelayValidateTime()
{
    return m_dwDelayValidateTime;
}

void CEditUI::SetDelayValidateTime(DWORD dwMiliSec)
{
    m_dwDelayValidateTime = dwMiliSec;
}

}
