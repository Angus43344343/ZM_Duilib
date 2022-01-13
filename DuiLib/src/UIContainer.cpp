﻿#include "StdAfx.h"

namespace DuiLib {

/////////////////////////////////////////////////////////////////////////////////////
//
//

CContainerUI::CContainerUI()
    : m_iChildMargin(0)
    , m_iChildAlign(DT_LEFT)
    , m_iChildVAlign(DT_TOP)
    , m_bAutoDestroy(true)
    , m_bDelayedDestroy(true)
    , m_bMouseChildEnabled(true)
    , m_pVerticalScrollBar(NULL)
    , m_pHorizontalScrollBar(NULL)
    , m_bScrollProcess(false)
{
}

CContainerUI::~CContainerUI()
{
    m_bDelayedDestroy = false;
    RemoveAll();

    if (m_pVerticalScrollBar) { m_pVerticalScrollBar->Delete(); }

    if (m_pHorizontalScrollBar) { m_pHorizontalScrollBar->Delete(); }
}

LPCTSTR CContainerUI::GetClass() const
{
    return DUI_CTR_CONTAINER;
}

LPVOID CContainerUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcscmp(pstrName, DUI_CTR_ICONTAINER) == 0) { return static_cast<IContainerUI *>(this); }
    else if (_tcscmp(pstrName, DUI_CTR_CONTAINER) == 0) { return static_cast<CContainerUI *>(this); }

    return CControlUI::GetInterface(pstrName);
}

CControlUI *CContainerUI::GetItemAt(int iIndex) const
{
    if (iIndex < 0 || iIndex >= m_items.GetSize()) { return NULL; }

    return static_cast<CControlUI *>(m_items[iIndex]);
}

int CContainerUI::GetItemIndex(CControlUI *pControl) const
{
    for (int it = 0; it < m_items.GetSize(); it++)
    {
        if (static_cast<CControlUI *>(m_items[it]) == pControl)
        {
            return it;
        }
    }

    return -1;
}

bool CContainerUI::SetItemIndex(CControlUI *pControl, int iNewIndex)
{
    for (int it = 0; it < m_items.GetSize(); it++)
    {
        if (static_cast<CControlUI *>(m_items[it]) == pControl)
        {
            NeedUpdate();
            m_items.Remove(it);
            return m_items.InsertAt(iNewIndex, pControl);
        }
    }

    return false;
}

bool CContainerUI::SetMultiItemIndex(CControlUI *pStartControl, int iCount, int iNewStartIndex)
{
    if (pStartControl == NULL || iCount < 0 || iNewStartIndex < 0) { return false; }

    int iStartIndex = GetItemIndex(pStartControl);

    if (iStartIndex == iNewStartIndex) { return true; }

    if (iStartIndex + iCount > GetCount()) { return false; }

    if (iNewStartIndex + iCount > GetCount()) { return false; }

    CDuiPtrArray pControls(iCount);
    pControls.Resize(iCount);
    ::CopyMemory(pControls.GetData(), m_items.GetData() + iStartIndex, iCount * sizeof(LPVOID));
    m_items.Remove(iStartIndex, iCount);

    for (int it3 = 0; it3 < pControls.GetSize(); it3++)
    {
        if (!pControls.InsertAt(iNewStartIndex + it3, pControls[it3])) { return false; }
    }

    NeedUpdate();
    return true;
}

int CContainerUI::GetCount() const
{
    return m_items.GetSize();
}

bool CContainerUI::Add(CControlUI *pControl)
{
    if (pControl == NULL) { return false; }

    if (m_pManager != NULL) { m_pManager->InitControls(pControl, this); }

    if (IsVisible()) { NeedUpdate(); }
    else { pControl->SetInternVisible(false); }

    return m_items.Add(pControl);
}

bool CContainerUI::AddAt(CControlUI *pControl, int iIndex)
{
    if (pControl == NULL) { return false; }

    if (m_pManager != NULL) { m_pManager->InitControls(pControl, this); }

    if (IsVisible()) { NeedUpdate(); }
    else { pControl->SetInternVisible(false); }

    return m_items.InsertAt(iIndex, pControl);
}

bool CContainerUI::Remove(CControlUI *pControl, bool bDoNotDestroy)
{
    if (pControl == NULL) { return false; }

    for (int it = 0; it < m_items.GetSize(); it++)
    {
        if (static_cast<CControlUI *>(m_items[it]) == pControl)
        {
            NeedUpdate();

            if (!bDoNotDestroy && m_bAutoDestroy)
            {
                if (m_bDelayedDestroy && m_pManager) { m_pManager->AddDelayedCleanup(pControl); }
                else { pControl->Delete(); }
            }

            return m_items.Remove(it);
        }
    }

    return false;
}

bool CContainerUI::RemoveAt(int iIndex, bool bDoNotDestroy)
{
    CControlUI *pControl = GetItemAt(iIndex);

    if (pControl != NULL)
    {
        return CContainerUI::Remove(pControl, bDoNotDestroy);
    }

    return false;
}

void CContainerUI::RemoveAll()
{
    for (int it = 0; m_bAutoDestroy && it < m_items.GetSize(); it++)
    {
        if (m_bDelayedDestroy && m_pManager) { m_pManager->AddDelayedCleanup(static_cast<CControlUI *>(m_items[it])); }
        else { static_cast<CControlUI *>(m_items[it])->Delete(); }
    }

    m_items.Empty();
    NeedUpdate();
}

bool CContainerUI::RemoveCount(int iIndex, int iCount, bool bDoNotDestroy)
{
    if (iIndex >= m_items.GetSize() || iIndex + iCount > m_items.GetSize()) { return false; }

    for (int i = iIndex + iCount - 1; i >= iIndex; --i)
    {
        CControlUI *pControl = static_cast<CControlUI *>(m_items[i]);
        NeedUpdate();

        if (!bDoNotDestroy && m_bAutoDestroy)
        {
            if (m_bDelayedDestroy && m_pManager) { m_pManager->AddDelayedCleanup(pControl); }
            else { pControl->Delete(); }
        }

        m_items.Remove(i);
    }

    return true;
}

bool CContainerUI::IsAutoDestroy() const
{
    return m_bAutoDestroy;
}

void CContainerUI::SetAutoDestroy(bool bAuto)
{
    m_bAutoDestroy = bAuto;
}

bool CContainerUI::IsDelayedDestroy() const
{
    return m_bDelayedDestroy;
}

void CContainerUI::SetDelayedDestroy(bool bDelayed)
{
    m_bDelayedDestroy = bDelayed;
}

void CContainerUI::SetPadding(RECT rcPadding)
{
    m_rcPadding = rcPadding;
    NeedUpdate();
}

int CContainerUI::GetChildMargin() const
{
    return m_iChildMargin;
}

void CContainerUI::SetChildMargin(int iMargin)
{
    m_iChildMargin = iMargin;

    if (m_iChildMargin < 0) { m_iChildMargin = 0; }

    NeedUpdate();
}

UINT CContainerUI::GetChildAlign() const
{
    return m_iChildAlign;
}

void CContainerUI::SetChildAlign(UINT iAlign)
{
    m_iChildAlign = iAlign;
    NeedUpdate();
}

UINT CContainerUI::GetChildVAlign() const
{
    return m_iChildVAlign;
}

void CContainerUI::SetChildVAlign(UINT iVAlign)
{
    m_iChildVAlign = iVAlign;
    NeedUpdate();
}

bool CContainerUI::IsMouseChildEnabled() const
{
    return m_bMouseChildEnabled;
}

void CContainerUI::SetMouseChildEnabled(bool bEnable)
{
    m_bMouseChildEnabled = bEnable;
}

bool CContainerUI::SetVisible(bool bVisible /*= true*/)
{
    if (TRIGGER_NONE != m_byEffectTrigger && !IsLastFrame())
    {
        if (TRIGGER_HIDE == m_byEffectTrigger)
        {
            // 正在播放特效 隐藏：
            // 1. 再次触发隐藏特效，重新开始隐藏特效
            // 2. 再次触发显示特效，立即开始显示特效
            // 3. 没有显示特效，触发显示，终止隐藏特效并立即显示
            if (StartEffect(bVisible ? TRIGGER_SHOW : TRIGGER_HIDE)) { return false; }
        }
        else if (TRIGGER_SHOW == m_byEffectTrigger)
        {
            // 正在播放特效 显示：
            // 1. 再次触发显示特效，重新开始显示特效
            // 2. 再次触发隐藏特效，立即开始隐藏特效
            // 3. 没有隐藏特效，触发隐藏，终止显示特效并立即隐藏
            if (bVisible) { if (StartEffect(TRIGGER_SHOW)) { return false; } }
            else { StopEffect(); }
        }
    }

    if (m_bVisible == bVisible) { return true; }

    // 2018-08-18 zhuyadong 添加特效。隐藏特效
    if (!bVisible && HasEffect(TRIGGER_HIDE) && TRIGGER_NONE == m_byEffectTrigger)
    {
        if (StartEffect(TRIGGER_HIDE)) { return false; }
    }

    bool v = IsVisible();
    m_bVisible = bVisible;

    if (m_bFocused) { m_bFocused = false; }

    if (!bVisible && m_pManager && m_pManager->GetFocus() == this)
    {
        m_pManager->SetFocus(NULL);
    }

    if (IsVisible() != v)
    {
        NeedParentUpdate();
    }

    if (m_pCover != NULL) { m_pCover->SetInternVisible(IsVisible()); }

    for (int it = 0; it < m_items.GetSize(); it++)
    {
        static_cast<CControlUI *>(m_items[it])->SetInternVisible(IsVisible());
    }

    // 2018-08-18 zhuyadong 添加特效。显示特效
    if (bVisible && HasEffect(TRIGGER_SHOW) && TRIGGER_NONE == m_byEffectTrigger)
    {
        // 2019-05-25 zhuyadong 修复条件判断不正确的问题
        //if (m_rcItem.left == m_rcItem.right || m_rcItem.top == m_rcItem.bottom && m_pParent)
        if ((m_rcItem.left == m_rcItem.right || m_rcItem.top == m_rcItem.bottom) && m_pParent)
        {
            // 当父控件不空、并且当前控件位置矩形不正确时，触发计算所有子控件位置，以解决特效不能正常显示
            RECT rc = m_pParent->GetPos();
            m_pParent->SetPos(rc, true);
        }

        StartEffect(TRIGGER_SHOW);
    }

    return true;
}

// 逻辑上，对于Container控件不公开此方法
// 调用此方法的结果是，内部子控件隐藏，控件本身依然显示，背景等效果存在
void CContainerUI::SetInternVisible(bool bVisible)
{
    CControlUI::SetInternVisible(bVisible);

    if (m_items.IsEmpty()) { return; }

    for (int it = 0; it < m_items.GetSize(); it++)
    {
        // 控制子控件显示状态
        // InternVisible状态应由子控件自己控制
        static_cast<CControlUI *>(m_items[it])->SetInternVisible(IsVisible());
    }
}

void CContainerUI::SetMouseEnabled(bool bEnabled)
{
    if (m_pVerticalScrollBar != NULL) { m_pVerticalScrollBar->SetMouseEnabled(bEnabled); }

    if (m_pHorizontalScrollBar != NULL) { m_pHorizontalScrollBar->SetMouseEnabled(bEnabled); }

    CControlUI::SetMouseEnabled(bEnabled);
}

void CContainerUI::DoEvent(TEventUI &event)
{
    if (!IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND)
    {
        if (m_pParent != NULL) { m_pParent->DoEvent(event); }
        else { CControlUI::DoEvent(event); }

        return;
    }

    if (event.Type == UIEVENT_SETFOCUS)
    {
        m_bFocused = true;
        return;
    }

    if (event.Type == UIEVENT_KILLFOCUS)
    {
        m_bFocused = false;
        return;
    }

    if (event.Type == UIEVENT_KEYDOWN)
    {
        if (IsKeyboardEnabled() && IsEnabled())
        {
            if (m_pVerticalScrollBar != NULL && m_pVerticalScrollBar->IsVisible() && m_pVerticalScrollBar->IsEnabled())
            {
                switch (event.chKey)
                {
                case VK_DOWN:
                    LineDown();
                    return;

                case VK_UP:
                    LineUp();
                    return;

                case VK_NEXT:
                    PageDown();
                    return;

                case VK_PRIOR:
                    PageUp();
                    return;

                case VK_HOME:
                    HomeUp();
                    return;

                case VK_END:
                    EndDown();
                    return;
                }
            }
            else if (m_pHorizontalScrollBar != NULL && m_pHorizontalScrollBar->IsVisible() &&
                     m_pHorizontalScrollBar->IsEnabled())
            {
                switch (event.chKey)
                {
                case VK_DOWN:
                    LineRight();
                    return;

                case VK_UP:
                    LineLeft();
                    return;

                case VK_NEXT:
                    PageRight();
                    return;

                case VK_PRIOR:
                    PageLeft();
                    return;

                case VK_HOME:
                    HomeLeft();
                    return;

                case VK_END:
                    EndRight();
                    return;
                }
            }
        }
    }
    else if (event.Type == UIEVENT_SCROLLWHEEL)
    {
        if (m_pHorizontalScrollBar != NULL && m_pHorizontalScrollBar->IsVisible() &&
            m_pHorizontalScrollBar->IsEnabled())
        {
            RECT rcHorizontalScrollBar = m_pHorizontalScrollBar->GetPos();

            if (::PtInRect(&rcHorizontalScrollBar, event.ptMouse))
            {
                switch (LOWORD(event.wParam))
                {
                case SB_LINEUP:
                    LineLeft();
                    return;

                case SB_LINEDOWN:
                    LineRight();
                    return;
                }
            }
        }

        if (m_pVerticalScrollBar != NULL && m_pVerticalScrollBar->IsVisible() && m_pVerticalScrollBar->IsEnabled())
        {
            switch (LOWORD(event.wParam))
            {
            case SB_LINEUP:
                LineUp();
                return;

            case SB_LINEDOWN:
                LineDown();
                return;
            }
        }

        if (m_pHorizontalScrollBar != NULL && m_pHorizontalScrollBar->IsVisible() &&
            m_pHorizontalScrollBar->IsEnabled())
        {
            switch (LOWORD(event.wParam))
            {
            case SB_LINEUP:
                LineLeft();
                return;

            case SB_LINEDOWN:
                LineRight();
                return;
            }
        }
    }

    CControlUI::DoEvent(event);
}

SIZE CContainerUI::GetScrollPos() const
{
    SIZE sz = {0, 0};

    if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible()) { sz.cy = m_pVerticalScrollBar->GetScrollPos(); }

    if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible()) { sz.cx = m_pHorizontalScrollBar->GetScrollPos(); }

    return sz;
}

SIZE CContainerUI::GetScrollRange() const
{
    SIZE sz = {0, 0};

    if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible()) { sz.cy = m_pVerticalScrollBar->GetScrollRange(); }

    if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible()) { sz.cx = m_pHorizontalScrollBar->GetScrollRange(); }

    return sz;
}

void CContainerUI::SetScrollPos(SIZE szPos)
{
    int cx = 0;
    int cy = 0;

    if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible())
    {
        int iLastScrollPos = m_pVerticalScrollBar->GetScrollPos();
        m_pVerticalScrollBar->SetScrollPos(szPos.cy);
        cy = m_pVerticalScrollBar->GetScrollPos() - iLastScrollPos;
    }

    if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible())
    {
        int iLastScrollPos = m_pHorizontalScrollBar->GetScrollPos();
        m_pHorizontalScrollBar->SetScrollPos(szPos.cx);
        cx = m_pHorizontalScrollBar->GetScrollPos() - iLastScrollPos;
    }

    if (cx == 0 && cy == 0) { return; }

    for (int it2 = 0; it2 < m_items.GetSize(); it2++)
    {
        CControlUI *pControl = static_cast<CControlUI *>(m_items[it2]);

        if (!pControl->IsVisible()) { continue; }

        if (pControl->IsFloat()) { continue; }

        pControl->Move(CDuiSize(-cx, -cy), false);
    }

    Invalidate();
}

void CContainerUI::LineUp()
{
    int cyLine = SCROLLBAR_LINESIZE;

    if (m_pManager)
    {
        cyLine = m_pManager->GetDefaultFontInfo()->tm.tmHeight + 8;

        if (m_pVerticalScrollBar && m_pVerticalScrollBar->GetScrollUnit() > 1)
        { cyLine = m_pVerticalScrollBar->GetScrollUnit(); }
    }

    SIZE sz = GetScrollPos();
    sz.cy -= cyLine;
    SetScrollPos(sz);
}

void CContainerUI::LineDown()
{
    int cyLine = SCROLLBAR_LINESIZE;

    if (m_pManager)
    {
        cyLine = m_pManager->GetDefaultFontInfo()->tm.tmHeight + 8;

        if (m_pVerticalScrollBar && m_pVerticalScrollBar->GetScrollUnit() > 1)
        { cyLine = m_pVerticalScrollBar->GetScrollUnit(); }
    }

    SIZE sz = GetScrollPos();
    sz.cy += cyLine;
    SetScrollPos(sz);
}

void CContainerUI::PageUp()
{
    SIZE sz = GetScrollPos();
    int iOffset = m_rcItem.bottom - m_rcItem.top - m_rcPadding.top - m_rcPadding.bottom;

    if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible()) { iOffset -= m_pHorizontalScrollBar->GetFixedHeight(); }

    sz.cy -= iOffset;
    SetScrollPos(sz);
}

void CContainerUI::PageDown()
{
    SIZE sz = GetScrollPos();
    int iOffset = m_rcItem.bottom - m_rcItem.top - m_rcPadding.top - m_rcPadding.bottom;

    if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible()) { iOffset -= m_pHorizontalScrollBar->GetFixedHeight(); }

    sz.cy += iOffset;
    SetScrollPos(sz);
}

void CContainerUI::HomeUp()
{
    SIZE sz = GetScrollPos();
    sz.cy = 0;
    SetScrollPos(sz);
}

void CContainerUI::EndDown()
{
    SIZE sz = GetScrollPos();
    sz.cy = GetScrollRange().cy;
    SetScrollPos(sz);
}

void CContainerUI::LineLeft()
{
    int cxLine = SCROLLBAR_LINESIZE;

    if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->GetScrollUnit() > 1)
    { cxLine = m_pHorizontalScrollBar->GetScrollUnit(); }

    SIZE sz = GetScrollPos();
    sz.cx -= cxLine;
    SetScrollPos(sz);
}

void CContainerUI::LineRight()
{
    int cxLine = SCROLLBAR_LINESIZE;

    if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->GetScrollUnit() > 1)
    { cxLine = m_pHorizontalScrollBar->GetScrollUnit(); }

    SIZE sz = GetScrollPos();
    sz.cx += cxLine;
    SetScrollPos(sz);
}

void CContainerUI::PageLeft()
{
    SIZE sz = GetScrollPos();
    int iOffset = m_rcItem.right - m_rcItem.left - m_rcPadding.left - m_rcPadding.right;

    if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible()) { iOffset -= m_pVerticalScrollBar->GetFixedWidth(); }

    sz.cx -= iOffset;
    SetScrollPos(sz);
}

void CContainerUI::PageRight()
{
    SIZE sz = GetScrollPos();
    int iOffset = m_rcItem.right - m_rcItem.left - m_rcPadding.left - m_rcPadding.right;

    if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible()) { iOffset -= m_pVerticalScrollBar->GetFixedWidth(); }

    sz.cx += iOffset;
    SetScrollPos(sz);
}

void CContainerUI::HomeLeft()
{
    SIZE sz = GetScrollPos();
    sz.cx = 0;
    SetScrollPos(sz);
}

void CContainerUI::EndRight()
{
    SIZE sz = GetScrollPos();
    sz.cx = GetScrollRange().cx;
    SetScrollPos(sz);
}

void CContainerUI::EnableScrollBar(bool bEnableVertical, bool bEnableHorizontal)
{
    if (bEnableVertical && !m_pVerticalScrollBar)
    {
        m_pVerticalScrollBar = new CScrollBarUI;
        m_pVerticalScrollBar->SetScrollRange(0);
        m_pVerticalScrollBar->SetOwner(this);
        m_pVerticalScrollBar->SetManager(m_pManager, NULL, false);

        if (m_pManager)
        {
            LPCTSTR pDefAttr = m_pManager->GetDefaultAttributeList(_T("VScrollBar"), true);
            m_pVerticalScrollBar->SetAttributeList(pDefAttr);
            pDefAttr = m_pManager->GetDefaultAttributeList(_T("VScrollBar"), false);
            m_pVerticalScrollBar->SetAttributeList(pDefAttr);
            // LPCTSTR pDefaultAttributes = m_pManager->GetDefaultAttributeList(_T("VScrollBar"));
            // if (pDefaultAttributes)
            // {
            //     m_pVerticalScrollBar->SetAttributeList(pDefaultAttributes);
            // }
        }
    }
    else if (!bEnableVertical && m_pVerticalScrollBar)
    {
        m_pVerticalScrollBar->Delete();
        m_pVerticalScrollBar = NULL;
    }

    if (bEnableHorizontal && !m_pHorizontalScrollBar)
    {
        m_pHorizontalScrollBar = new CScrollBarUI;
        m_pHorizontalScrollBar->SetScrollRange(0);
        m_pHorizontalScrollBar->SetHorizontal(true);
        m_pHorizontalScrollBar->SetOwner(this);
        m_pHorizontalScrollBar->SetManager(m_pManager, NULL, false);

        if (m_pManager)
        {
            LPCTSTR pDefAttr = m_pManager->GetDefaultAttributeList(_T("HScrollBar"), true);
            m_pHorizontalScrollBar->SetAttributeList(pDefAttr);
            pDefAttr = m_pManager->GetDefaultAttributeList(_T("HScrollBar"), false);
            m_pHorizontalScrollBar->SetAttributeList(pDefAttr);
            // LPCTSTR pDefaultAttributes = m_pManager->GetDefaultAttributeList(_T("HScrollBar"));
            // if (pDefaultAttributes)
            // {
            //     m_pHorizontalScrollBar->SetAttributeList(pDefaultAttributes);
            // }
        }
    }
    else if (!bEnableHorizontal && m_pHorizontalScrollBar)
    {
        m_pHorizontalScrollBar->Delete();
        m_pHorizontalScrollBar = NULL;
    }

    NeedUpdate();
}

CScrollBarUI *CContainerUI::GetVerticalScrollBar() const
{
    return m_pVerticalScrollBar;
}

CScrollBarUI *CContainerUI::GetHorizontalScrollBar() const
{
    return m_pHorizontalScrollBar;
}

int CContainerUI::FindSelectable(int iIndex, bool bForward /*= true*/) const
{
    // NOTE: This is actually a helper-function for the list/combo/ect controls
    //       that allow them to find the next enabled/available selectable item
    if (GetCount() == 0) { return -1; }

    iIndex = clamp<int>(iIndex, 0, GetCount() - 1);

    if (bForward)
    {
        for (int i = iIndex; i < GetCount(); i++)
        {
            if (GetItemAt(i)->GetInterface(DUI_CTR_ILISTITEM) != NULL
                && GetItemAt(i)->IsVisible()
                && GetItemAt(i)->IsEnabled()) { return i; }
        }

        return -1;
    }
    else
    {
        for (int i = iIndex; i >= 0; --i)
        {
            if (GetItemAt(i)->GetInterface(DUI_CTR_ILISTITEM) != NULL
                && GetItemAt(i)->IsVisible()
                && GetItemAt(i)->IsEnabled()) { return i; }
        }

        return FindSelectable(0, true);
    }
}

RECT CContainerUI::GetClientPos() const
{
    RECT rc = m_rcItem;
    rc.left += (m_rcBorderSize.left + m_rcPadding.left);
    rc.top += (m_rcBorderSize.top + m_rcPadding.top);
    rc.right -= (m_rcBorderSize.right + m_rcPadding.right);
    rc.bottom -= (m_rcBorderSize.bottom + m_rcPadding.bottom);

    if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible())
    {
        rc.right -= m_pVerticalScrollBar->GetFixedWidth();
    }

    if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible())
    {
        rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();
    }

    return rc;
}

void CContainerUI::SetPos(RECT rc, bool bNeedInvalidate)
{
    CControlUI::SetPos(rc, bNeedInvalidate);

    if (m_items.IsEmpty()) { return; }

    rc = m_rcItem;
    // 2019-05-30 zhuyadong 排除边框占用的空间
    rc.left += (m_rcBorderSize.left + m_rcPadding.left);
    rc.top += (m_rcBorderSize.top + m_rcPadding.top);
    rc.right -= (m_rcBorderSize.right + m_rcPadding.right);
    rc.bottom -= (m_rcBorderSize.bottom + m_rcPadding.bottom);

    if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible())
    {
        rc.top -= m_pVerticalScrollBar->GetScrollPos();
        rc.bottom -= m_pVerticalScrollBar->GetScrollPos();
        rc.bottom += m_pVerticalScrollBar->GetScrollRange();
        rc.right -= m_pVerticalScrollBar->GetFixedWidth();
    }

    if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible())
    {
        rc.left -= m_pHorizontalScrollBar->GetScrollPos();
        rc.right -= m_pHorizontalScrollBar->GetScrollPos();
        rc.right += m_pHorizontalScrollBar->GetScrollRange();
        rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();
    }

    for (int it = 0; it < m_items.GetSize(); it++)
    {
        CControlUI *pControl = static_cast<CControlUI *>(m_items[it]);

        if (!pControl->IsVisible()) { continue; }

        if (pControl->IsFloat())
        {
            SetFloatPos(it);
        }
        else
        {
            SIZE sz = { rc.right - rc.left, rc.bottom - rc.top };

            if (sz.cx < pControl->GetMinWidth()) { sz.cx = pControl->GetMinWidth(); }

            if (sz.cx > pControl->GetMaxWidth()) { sz.cx = pControl->GetMaxWidth(); }

            if (sz.cy < pControl->GetMinHeight()) { sz.cy = pControl->GetMinHeight(); }

            if (sz.cy > pControl->GetMaxHeight()) { sz.cy = pControl->GetMaxHeight(); }

            RECT rcCtrl = { rc.left, rc.top, rc.left + sz.cx, rc.top + sz.cy };
            pControl->SetPos(rcCtrl, false);
        }
    }
}

void CContainerUI::Move(SIZE szOffset, bool bNeedInvalidate)
{
    CControlUI::Move(szOffset, bNeedInvalidate);

    if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible()) { m_pVerticalScrollBar->Move(szOffset, false); }

    if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible()) { m_pHorizontalScrollBar->Move(szOffset, false); }

    for (int it = 0; it < m_items.GetSize(); it++)
    {
        CControlUI *pControl = static_cast<CControlUI *>(m_items[it]);

        if (pControl != NULL && pControl->IsVisible()) { pControl->Move(szOffset, false); }
    }
}

void CContainerUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if (_tcscmp(pstrName, _T("padding")) == 0 || _tcscmp(pstrName, _T("inset")) == 0)
    {
        RECT rt = ParseRect(pstrValue);
        SetPadding(rt);
    }
    else if (_tcscmp(pstrName, _T("mousechild")) == 0) { SetMouseChildEnabled(ParseBool(pstrValue)); }
    else if (_tcscmp(pstrName, _T("vscrollbar")) == 0)
    {
        EnableScrollBar(ParseBool(pstrValue), GetHorizontalScrollBar() != NULL);
    }
    else if (_tcscmp(pstrName, _T("vscrollbarstyle")) == 0)
    {
        EnableScrollBar(true, GetHorizontalScrollBar() != NULL);

        if (GetVerticalScrollBar()) 
		{ 
			//2021-09-07 zm 修改加载方式，通过样式表的形式加载
			LPCTSTR pstrStyle = m_pManager->GetStyle(pstrValue);
			if (pstrStyle) GetVerticalScrollBar()->ApplyAttributeList(pstrStyle);
		}
    }
    else if (_tcscmp(pstrName, _T("hscrollbar")) == 0)
    {
        EnableScrollBar(GetVerticalScrollBar() != NULL, ParseBool(pstrValue));
    }
    else if (_tcscmp(pstrName, _T("hscrollbarstyle")) == 0)
    {
        EnableScrollBar(GetVerticalScrollBar() != NULL, true);

        if (GetHorizontalScrollBar()) 
		{ 
			//2021-09-07 zm 修改加载方式，通过样式表的形式加载
			LPCTSTR pstrStyle = m_pManager->GetStyle(pstrValue);
			if (pstrStyle) GetHorizontalScrollBar()->SetAttributeList(pstrStyle);
		}
    }
    else if (_tcscmp(pstrName, _T("childmargin")) == 0) { SetChildMargin(ParseInt(pstrValue)); }
    else if (_tcscmp(pstrName, _T("childalign")) == 0)
    {
        CDuiString strVal = ParseString(pstrValue);

        if (strVal == _T("left")) { m_iChildAlign = DT_LEFT; }
        else if (strVal == _T("center")) { m_iChildAlign = DT_CENTER; }
        else if (strVal == _T("right")) { m_iChildAlign = DT_RIGHT; }
    }
    else if (_tcscmp(pstrName, _T("childvalign")) == 0)
    {
        CDuiString strVal = ParseString(pstrValue);

        if (strVal == _T("top")) { m_iChildVAlign = DT_TOP; }
        else if (strVal == _T("center")) { m_iChildVAlign = DT_VCENTER; }
        else if (strVal == _T("bottom")) { m_iChildVAlign = DT_BOTTOM; }
    }
    // 2018-08-28 zhuyadong 解决List不支持拖拽源、目的的问题，去掉属性拦截
    else { CControlUI::SetAttribute(pstrName, pstrValue); }
}

void CContainerUI::SetManager(CPaintManagerUI *pManager, CControlUI *pParent, bool bInit)
{
    for (int it = 0; it < m_items.GetSize(); it++)
    {
        static_cast<CControlUI *>(m_items[it])->SetManager(pManager, this, bInit);
    }

    if (m_pVerticalScrollBar != NULL) { m_pVerticalScrollBar->SetManager(pManager, this, bInit); }

    if (m_pHorizontalScrollBar != NULL) { m_pHorizontalScrollBar->SetManager(pManager, this, bInit); }

    CControlUI::SetManager(pManager, pParent, bInit);
}

CControlUI *CContainerUI::FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags)
{
    // Check if this guy is valid
    if ((uFlags & UIFIND_VISIBLE) != 0 && !IsVisible()) { return NULL; }

    if ((uFlags & UIFIND_ENABLED) != 0 && !IsEnabled()) { return NULL; }

    if ((uFlags & UIFIND_HITTEST) != 0 && !::PtInRect(&m_rcItem, *(static_cast<LPPOINT>(pData)))) { return NULL; }

    if ((uFlags & UIFIND_UPDATETEST) != 0 && Proc(this, pData) != NULL) { return NULL; }

    CControlUI *pResult = NULL;

    if ((uFlags & UIFIND_ME_FIRST) != 0)
    {
        if ((uFlags & UIFIND_HITTEST) == 0 || IsMouseEnabled()) { pResult = Proc(this, pData); }
    }

    if (pResult == NULL && m_pCover != NULL)
    {
        if ((uFlags & UIFIND_HITTEST) == 0 || IsMouseChildEnabled()) { pResult = m_pCover->FindControl(Proc, pData, uFlags); }
    }

    if (pResult == NULL && m_pVerticalScrollBar != NULL)
    {
        if ((uFlags & UIFIND_HITTEST) == 0 || IsMouseEnabled()) { pResult = m_pVerticalScrollBar->FindControl(Proc, pData, uFlags); }
    }

    if (pResult == NULL && m_pHorizontalScrollBar != NULL)
    {
        if ((uFlags & UIFIND_HITTEST) == 0 || IsMouseEnabled()) { pResult = m_pHorizontalScrollBar->FindControl(Proc, pData, uFlags); }
    }

    if (pResult != NULL) { return pResult; }

    if ((uFlags & UIFIND_HITTEST) == 0 || IsMouseChildEnabled())
    {
        RECT rc = m_rcItem;
        rc.left += (m_rcBorderSize.left + m_rcPadding.left);
        rc.top += (m_rcBorderSize.top + m_rcPadding.top);
        rc.right -= (m_rcBorderSize.right + m_rcPadding.right);
        rc.bottom -= (m_rcBorderSize.bottom + m_rcPadding.bottom);

        if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible()) { rc.right -= m_pVerticalScrollBar->GetFixedWidth(); }

        if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible()) { rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight(); }

        if ((uFlags & UIFIND_TOP_FIRST) != 0)
        {
            for (int it = m_items.GetSize() - 1; it >= 0; it--)
            {
                pResult = static_cast<CControlUI *>(m_items[it])->FindControl(Proc, pData, uFlags);

                if (pResult != NULL)
                {
                    if ((uFlags & UIFIND_HITTEST) != 0 && !pResult->IsFloat() && !::PtInRect(&rc, *(static_cast<LPPOINT>(pData))))
                    { continue; }
                    else
                    { return pResult; }
                }
            }
        }
        else
        {
            for (int it = 0; it < m_items.GetSize(); it++)
            {
                pResult = static_cast<CControlUI *>(m_items[it])->FindControl(Proc, pData, uFlags);

                if (pResult != NULL)
                {
                    if ((uFlags & UIFIND_HITTEST) != 0 && !pResult->IsFloat() && !::PtInRect(&rc, *(static_cast<LPPOINT>(pData))))
                    { continue; }
                    else
                    { return pResult; }
                }
            }
        }
    }

    pResult = NULL;

    if (pResult == NULL && (uFlags & UIFIND_ME_FIRST) == 0)
    {
        if ((uFlags & UIFIND_HITTEST) == 0 || IsMouseEnabled()) { pResult = Proc(this, pData); }
    }

    return pResult;
}

bool CContainerUI::DoPaint(HDC hDC, const RECT &rcPaint, CControlUI *pStopControl)
{
    // 2018-08-18 zhuyadong 添加特效
    if (IsEffectRunning())
    {
        // 窗体显示特效：第一次走到这里，并非是特效，而是系统触发的绘制。应该过滤掉
        if (TRIGGER_SHOW == m_byEffectTrigger && 0 == m_pEffect->GetCurFrame(m_byEffectTrigger)) { return true; }

        BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
        static PFunAlphaBlend spfAlphaBlend = GetAlphaBlend();
        spfAlphaBlend(hDC, m_rcItem.left, m_rcItem.top, m_rcItem.right - m_rcItem.left,
                      m_rcItem.bottom - m_rcItem.top, m_pEffect->GetMemHDC(m_byEffectTrigger),
                      0, 0, m_rcItem.right - m_rcItem.left, m_rcItem.bottom - m_rcItem.top, bf);
        return true;
    }

    RECT rcTemp = { 0 };
    RECT rcBox = m_rcItem;
    rcBox.left -= m_rcMargin.left;       rcBox.top -= m_rcMargin.top;
    rcBox.right += m_rcMargin.right;     rcBox.bottom += m_rcMargin.bottom;

    if (!::IntersectRect(&rcTemp, &rcPaint, &rcBox)) { return true; }

    CRenderClip clip;
    CRenderClip::GenerateClip(hDC, rcTemp, clip);
    CControlUI::DoPaint(hDC, rcPaint, pStopControl);

    if (m_items.GetSize() > 0)
    {
        RECT rc = m_rcItem;
        rc.left += (m_rcBorderSize.left + m_rcPadding.left);
        rc.top += (m_rcBorderSize.top + m_rcPadding.top);
        rc.right -= (m_rcBorderSize.right + m_rcPadding.right);
        rc.bottom -= (m_rcBorderSize.bottom + m_rcPadding.bottom);

        if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible()) { rc.right -= m_pVerticalScrollBar->GetFixedWidth(); }

        if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible()) { rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight(); }

        if (!::IntersectRect(&rcTemp, &rcPaint, &rc))
        {
            for (int it = 0; it < m_items.GetSize(); it++)
            {
                CControlUI *pControl = static_cast<CControlUI *>(m_items[it]);

                if (pControl == pStopControl) { return false; }

                if (!pControl->IsVisible()) { continue; }

                if (!::IntersectRect(&rcTemp, &rcPaint, &pControl->GetPos())) { continue; }

                if (pControl->IsFloat())
                {
                    if (!::IntersectRect(&rcTemp, &m_rcItem, &pControl->GetPos())) { continue; }

                    if (!pControl->Paint(hDC, rcPaint, pStopControl)) { return false; }
                }
            }
        }
        else
        {
            CRenderClip childClip;
            CRenderClip::GenerateClip(hDC, rcTemp, childClip);

            for (int it = 0; it < m_items.GetSize(); it++)
            {
                CControlUI *pControl = static_cast<CControlUI *>(m_items[it]);

                if (pControl == pStopControl) { return false; }

                if (!pControl->IsVisible()) { continue; }

                if (!::IntersectRect(&rcTemp, &rcPaint, &pControl->GetPos())) { continue; }

                if (pControl->IsFloat())
                {
                    if (!::IntersectRect(&rcTemp, &m_rcItem, &pControl->GetPos())) { continue; }

                    CRenderClip::UseOldClipBegin(hDC, childClip);

                    if (!pControl->Paint(hDC, rcPaint, pStopControl)) { return false; }

                    CRenderClip::UseOldClipEnd(hDC, childClip);
                }
                else
                {
                    if (!::IntersectRect(&rcTemp, &rc, &pControl->GetPos())) { continue; }

                    if (!pControl->Paint(hDC, rcPaint, pStopControl)) { return false; }
                }
            }
        }
    }

    if (m_pVerticalScrollBar != NULL)
    {
        if (m_pVerticalScrollBar == pStopControl) { return false; }

        if (m_pVerticalScrollBar->IsVisible())
        {
            if (::IntersectRect(&rcTemp, &rcPaint, &m_pVerticalScrollBar->GetPos()))
            {
                if (!m_pVerticalScrollBar->Paint(hDC, rcPaint, pStopControl)) { return false; }
            }
        }
    }

    if (m_pHorizontalScrollBar != NULL)
    {
        if (m_pHorizontalScrollBar == pStopControl) { return false; }

        if (m_pHorizontalScrollBar->IsVisible())
        {
            if (::IntersectRect(&rcTemp, &rcPaint, &m_pHorizontalScrollBar->GetPos()))
            {
                if (!m_pHorizontalScrollBar->Paint(hDC, rcPaint, pStopControl)) { return false; }
            }
        }
    }

    return true;
}

SIZE CContainerUI::EstimateSize(SIZE szAvailable)
{
    SIZE sz = CControlUI::EstimateSize(szAvailable);

    if (m_bAutoWidth || m_bAutoHeight)
    {
        for (int i = 0; i < GetCount(); ++i)
        {
            SIZE sz2 = GetItemAt(i)->EstimateSize(szAvailable);

            if (sz2.cx > m_cxyFixed.cx) { m_cxyFixed.cx = sz2.cx; }

            if (sz2.cy > m_cxyFixed.cy) { m_cxyFixed.cy = sz2.cy; }
        }

        if (m_bAutoWidth)
        {
            m_cxyFixed.cx += m_rcPadding.left + m_rcPadding.right + m_rcBorderSize.left + m_rcBorderSize.right;
            sz.cx = m_cxyFixed.cx;
        }

        if (m_bAutoHeight)
        {
            m_cxyFixed.cy += m_rcPadding.top + m_rcPadding.bottom + m_rcBorderSize.top + m_rcBorderSize.bottom;
            sz.cy = m_cxyFixed.cy;
        }
    }

    return sz;
}

void CContainerUI::SetFloatPos(int iIndex)
{
    // 因为CControlUI::SetPos对float的操作影响，这里不能对float组件添加滚动条的影响
    if (iIndex < 0 || iIndex >= m_items.GetSize()) { return; }

    CControlUI *pControl = static_cast<CControlUI *>(m_items[iIndex]);

    if (!pControl->IsVisible()) { return; }

    if (!pControl->IsFloat()) { return; }

    // 2018-08-03 解决绝对布局不支持 autowidth/autoheight 属性问题
    if (pControl->GetAutoWidth() || pControl->GetAutoHeight())
    {
        SIZE szAvailable = { m_rcItem.right - m_rcItem.left + m_rcMargin.left + m_rcMargin.right,
                             m_rcItem.bottom - m_rcItem.top + m_rcMargin.top + m_rcMargin.bottom
                           };
        SIZE sz = pControl->EstimateSize(szAvailable);

        if (sz.cx != pControl->GetFixedWidth()) { pControl->SetFixedWidth(sz.cx); }

        if (sz.cy != pControl->GetFixedHeight()) { pControl->SetFixedHeight(sz.cy); }
    }

    SIZE szXY = pControl->GetFixedXY();
    SIZE sz = {pControl->GetFixedWidth(), pControl->GetFixedHeight()};

	TPercentInfo rcPercent = pControl->GetFloatPercent();
	LONG width = m_rcItem.right - m_rcItem.left;
	LONG height = m_rcItem.bottom - m_rcItem.top;
	RECT rcCtrl = { 0 };
	rcCtrl.left = (LONG)(width * rcPercent.left) + szXY.cx;
	rcCtrl.top = (LONG)(height * rcPercent.top) + szXY.cy;
	rcCtrl.right = (LONG)(width * rcPercent.right) + szXY.cx + sz.cx;
	rcCtrl.bottom = (LONG)(height * rcPercent.bottom) + szXY.cy + sz.cy;
	pControl->SetPos(rcCtrl, false);
}

void CContainerUI::ProcessScrollBar(RECT rc, int cxRequired, int cyRequired)
{
    if (m_pHorizontalScrollBar != NULL)
    {
        if (m_pVerticalScrollBar == NULL)
        {
            if (cxRequired > rc.right - rc.left && !m_pHorizontalScrollBar->IsVisible())
            {
                m_pHorizontalScrollBar->SetVisible(true);
                m_pHorizontalScrollBar->SetScrollRange(cxRequired - (rc.right - rc.left));
                m_pHorizontalScrollBar->SetScrollPos(0);
                m_bScrollProcess = true;

                if (!IsFloat()) { SetPos(GetPos()); }
                else { SetPos(GetRelativePos()); }

                m_bScrollProcess = false;
                return;
            }

            // No scrollbar required
            if (!m_pHorizontalScrollBar->IsVisible()) { return; }

            // Scroll not needed anymore?
            int cxScroll = cxRequired - (rc.right - rc.left);

            if (cxScroll <= 0 && !m_bScrollProcess)
            {
                m_pHorizontalScrollBar->SetVisible(false);
                m_pHorizontalScrollBar->SetScrollPos(0);
                m_pHorizontalScrollBar->SetScrollRange(0);

                if (!IsFloat()) { SetPos(GetPos()); }
                else { SetPos(GetRelativePos()); }
            }
            else
            {
                RECT rcScrollBarPos = { rc.left, rc.bottom, rc.right, rc.bottom + m_pHorizontalScrollBar->GetFixedHeight()};
                m_pHorizontalScrollBar->SetPos(rcScrollBarPos, false);

                if (m_pHorizontalScrollBar->GetScrollRange() != cxScroll)
                {
                    int iScrollPos = m_pHorizontalScrollBar->GetScrollPos();
                    m_pHorizontalScrollBar->SetScrollRange(::abs(cxScroll));

                    if (m_pHorizontalScrollBar->GetScrollRange() == 0)
                    {
                        m_pHorizontalScrollBar->SetVisible(false);
                        m_pHorizontalScrollBar->SetScrollPos(0);
                    }

                    if (iScrollPos > m_pHorizontalScrollBar->GetScrollPos())
                    {
                        if (!IsFloat()) { SetPos(GetPos(), false); }
                        else { SetPos(GetRelativePos(), false); }
                    }
                }
            }

            return;
        }
        else
        {
            bool bNeedSetPos = false;

            if (cxRequired > rc.right - rc.left)
            {
                if (!m_pHorizontalScrollBar->IsVisible())
                {
                    m_pHorizontalScrollBar->SetVisible(true);
                    m_pHorizontalScrollBar->SetScrollPos(0);
                    rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();
                }

                RECT rcScrollBarPos = { rc.left, rc.bottom, rc.right, rc.bottom + m_pHorizontalScrollBar->GetFixedHeight()};
                m_pHorizontalScrollBar->SetPos(rcScrollBarPos, false);

                if (m_pHorizontalScrollBar->GetScrollRange() != cxRequired - (rc.right - rc.left))
                {
                    m_pHorizontalScrollBar->SetScrollRange(cxRequired - (rc.right - rc.left));
                    bNeedSetPos = true;
                }
            }
            else
            {
                if (m_pHorizontalScrollBar->IsVisible())
                {
                    m_pHorizontalScrollBar->SetVisible(false);
                    rc.bottom += m_pHorizontalScrollBar->GetFixedHeight();
                }
            }

            if (cyRequired > rc.bottom - rc.top && !m_pVerticalScrollBar->IsVisible())
            {
                m_pVerticalScrollBar->SetVisible(true);
                m_pVerticalScrollBar->SetScrollRange(cyRequired - (rc.bottom - rc.top));
                m_pVerticalScrollBar->SetScrollPos(0);
                rc.right -= m_pVerticalScrollBar->GetFixedWidth();

                if (m_pHorizontalScrollBar->IsVisible())
                {
                    RECT rcScrollBarPos = { rc.left, rc.bottom, rc.right, rc.bottom + m_pHorizontalScrollBar->GetFixedHeight()};
                    m_pHorizontalScrollBar->SetPos(rcScrollBarPos, false);
                    m_pHorizontalScrollBar->SetScrollRange(cxRequired - (rc.right - rc.left));
                }

                m_bScrollProcess = true;

                if (!IsFloat()) { SetPos(GetPos()); }
                else { SetPos(GetRelativePos()); }

                m_bScrollProcess = false;
                return;
            }

            // No scrollbar required
            if (!m_pVerticalScrollBar->IsVisible())
            {
                if (bNeedSetPos)
                {
                    if (!IsFloat()) { SetPos(GetPos()); }
                    else { SetPos(GetRelativePos()); }
                }

                return;
            }

            // Scroll not needed anymore?
            int cyScroll = cyRequired - (rc.bottom - rc.top);

            if (cyScroll <= 0 && !m_bScrollProcess)
            {
                m_pVerticalScrollBar->SetVisible(false);
                m_pVerticalScrollBar->SetScrollPos(0);
                m_pVerticalScrollBar->SetScrollRange(0);
                rc.right += m_pVerticalScrollBar->GetFixedWidth();

                if (m_pHorizontalScrollBar->IsVisible())
                {
                    RECT rcScrollBarPos = { rc.left, rc.bottom, rc.right, rc.bottom + m_pHorizontalScrollBar->GetFixedHeight()};
                    m_pHorizontalScrollBar->SetPos(rcScrollBarPos, false);
                    m_pHorizontalScrollBar->SetScrollRange(cxRequired - (rc.right - rc.left));
                }

                if (!IsFloat()) { SetPos(GetPos()); }
                else { SetPos(GetRelativePos()); }
            }
            else
            {
                RECT rcScrollBarPos = { rc.right, rc.top, rc.right + m_pVerticalScrollBar->GetFixedWidth(), rc.bottom };
                m_pVerticalScrollBar->SetPos(rcScrollBarPos, false);

                if (m_pVerticalScrollBar->GetScrollRange() != cyScroll)
                {
                    int iScrollPos = m_pVerticalScrollBar->GetScrollPos();
                    m_pVerticalScrollBar->SetScrollRange(::abs(cyScroll));

                    if (m_pVerticalScrollBar->GetScrollRange() == 0)
                    {
                        m_pVerticalScrollBar->SetVisible(false);
                        m_pVerticalScrollBar->SetScrollPos(0);
                    }

                    if (iScrollPos > m_pVerticalScrollBar->GetScrollPos() || bNeedSetPos)
                    {
                        if (!IsFloat()) { SetPos(GetPos(), false); }
                        else { SetPos(GetRelativePos(), false); }
                    }
                }
            }
        }
    }
    else
    {
        if (m_pVerticalScrollBar == NULL) { return; }

        if (cyRequired > rc.bottom - rc.top && !m_pVerticalScrollBar->IsVisible())
        {
            m_pVerticalScrollBar->SetVisible(true);
            m_pVerticalScrollBar->SetScrollRange(cyRequired - (rc.bottom - rc.top));
            m_pVerticalScrollBar->SetScrollPos(0);
            m_bScrollProcess = true;

            if (!IsFloat()) { SetPos(GetPos()); }
            else { SetPos(GetRelativePos()); }

            m_bScrollProcess = false;
            return;
        }

        // No scrollbar required
        if (!m_pVerticalScrollBar->IsVisible()) { return; }

        // Scroll not needed anymore?
        int cyScroll = cyRequired - (rc.bottom - rc.top);

        if (cyScroll <= 0 && !m_bScrollProcess)
        {
            m_pVerticalScrollBar->SetVisible(false);
            m_pVerticalScrollBar->SetScrollPos(0);
            m_pVerticalScrollBar->SetScrollRange(0);

            if (!IsFloat()) { SetPos(GetPos()); }
            else { SetPos(GetRelativePos()); }
        }
        else
        {
            RECT rcScrollBarPos = { rc.right, rc.top, rc.right + m_pVerticalScrollBar->GetFixedWidth(), rc.bottom };
            m_pVerticalScrollBar->SetPos(rcScrollBarPos, false);

            if (m_pVerticalScrollBar->GetScrollRange() != cyScroll)
            {
                int iScrollPos = m_pVerticalScrollBar->GetScrollPos();
                m_pVerticalScrollBar->SetScrollRange(::abs(cyScroll));

                if (m_pVerticalScrollBar->GetScrollRange() == 0)
                {
                    m_pVerticalScrollBar->SetVisible(false);
                    m_pVerticalScrollBar->SetScrollPos(0);
                }

                if (iScrollPos > m_pVerticalScrollBar->GetScrollPos())
                {
                    if (!IsFloat()) { SetPos(GetPos(), false); }
                    else { SetPos(GetRelativePos(), false); }
                }
            }
        }
    }
}

bool CContainerUI::SetSubControlText(LPCTSTR pstrSubControlName, LPCTSTR pstrText)
{
    CControlUI *pSubControl = NULL;
    pSubControl = this->FindSubControl(pstrSubControlName);

    if (pSubControl != NULL)
    {
        pSubControl->SetText(pstrText);
        return TRUE;
    }
    else
    { return FALSE; }
}

bool CContainerUI::SetSubControlFixedHeight(LPCTSTR pstrSubControlName, int cy)
{
    CControlUI *pSubControl = NULL;
    pSubControl = this->FindSubControl(pstrSubControlName);

    if (pSubControl != NULL)
    {
        pSubControl->SetFixedHeight(cy);
        return TRUE;
    }
    else
    { return FALSE; }
}

bool CContainerUI::SetSubControlFixedWdith(LPCTSTR pstrSubControlName, int cx)
{
    CControlUI *pSubControl = NULL;
    pSubControl = this->FindSubControl(pstrSubControlName);

    if (pSubControl != NULL)
    {
        pSubControl->SetFixedWidth(cx);
        return TRUE;
    }
    else
    { return FALSE; }
}

bool CContainerUI::SetSubControlUserData(LPCTSTR pstrSubControlName, LPCTSTR pstrText)
{
    CControlUI *pSubControl = NULL;
    pSubControl = this->FindSubControl(pstrSubControlName);

    if (pSubControl != NULL)
    {
        pSubControl->SetUserData(pstrText);
        return TRUE;
    }
    else
    { return FALSE; }
}

CDuiString CContainerUI::GetSubControlText(LPCTSTR pstrSubControlName)
{
    CControlUI *pSubControl = NULL;
    pSubControl = this->FindSubControl(pstrSubControlName);

    if (pSubControl == NULL)
    { return _T(""); }
    else
    { return pSubControl->GetText(); }
}

int CContainerUI::GetSubControlFixedHeight(LPCTSTR pstrSubControlName)
{
    CControlUI *pSubControl = NULL;
    pSubControl = this->FindSubControl(pstrSubControlName);

    if (pSubControl == NULL)
    { return -1; }
    else
    { return pSubControl->GetFixedHeight(); }
}

int CContainerUI::GetSubControlFixedWdith(LPCTSTR pstrSubControlName)
{
    CControlUI *pSubControl = NULL;
    pSubControl = this->FindSubControl(pstrSubControlName);

    if (pSubControl == NULL)
    { return -1; }
    else
    { return pSubControl->GetFixedWidth(); }
}

const CDuiString CContainerUI::GetSubControlUserData(LPCTSTR pstrSubControlName)
{
    CControlUI *pSubControl = NULL;
    pSubControl = this->FindSubControl(pstrSubControlName);

    if (pSubControl == NULL)
    { return _T(""); }
    else
    { return pSubControl->GetUserData(); }
}

CControlUI *CContainerUI::FindSubControl(LPCTSTR pstrSubControlName)
{
    CControlUI *pSubControl = NULL;
    pSubControl = static_cast<CControlUI *>(GetManager()->FindSubControlByName(this, pstrSubControlName));
    return pSubControl;
}

void CContainerUI::ReloadText(void)
{
    CControlUI::ReloadText();

    for (int it = 0; it < m_items.GetSize(); it++)
    {
        static_cast<CControlUI *>(m_items[it])->ReloadText();
    }
}

int CContainerUI::FindItemByTag(UINT_PTR pTag)
{
    int i = -1;

    for (i = m_items.GetSize() - 1; i >= 0; --i)
    {
        if (static_cast<CControlUI *>(m_items[i])->GetTag() == pTag) { break; }
    }

    return i;
}

int CContainerUI::FindItemByUserData(LPCTSTR pstrText)
{
    int i = -1;

    for (i = m_items.GetSize() - 1; i >= 0; --i)
    {
        if (static_cast<CControlUI *>(m_items[i])->GetUserData() == pstrText) { break; }
    }

    return i;
}

DuiLib::CControlUI *CContainerUI::GetItemByTag(UINT_PTR pTag)
{
    for (int i = m_items.GetSize() - 1; i >= 0; --i)
    {
        CControlUI *pCtrl = static_cast<CControlUI *>(m_items[i]);

        if (pCtrl->GetTag() == pTag) { return pCtrl; }
    }

    return NULL;
}

DuiLib::CControlUI *CContainerUI::GetItemByUserData(LPCTSTR pstrText)
{
    for (int i = m_items.GetSize() - 1; i >= 0; --i)
    {
        CControlUI *pCtrl = static_cast<CControlUI *>(m_items[i]);

        if (pCtrl->GetUserData() == pstrText) { return pCtrl; }
    }

    return NULL;
}

bool CContainerUI::IsEnabled() const
{
    return m_bEnabled;
}

void CContainerUI::SetEnabled(bool bEnable /*= true*/)
{
    if (m_bEnabled == bEnable) { return; }

    m_bEnabled = bEnable;

    for (int i = 0; i < GetCount(); ++i)
    {
        // static_cast<CControlUI *>(m_items[i])->SetEnabled(bEnable);
        static_cast<CControlUI *>(GetItemAt(i))->SetEnabled(bEnable);
    }

    Invalidate();
}

} // namespace DuiLib
