﻿#include "stdafx.h"
#include <algorithm>

namespace DuiLib {
CVerticalLayoutUI::CVerticalLayoutUI() 
: m_iSepHeight(0)
, m_uButtonState(0)
, m_bImmMode(false)
, m_dwSepImmColor(0xAA000000)
{
    m_ptLastMouse.x = m_ptLastMouse.y = 0;
    ::ZeroMemory(&m_rcNewPos, sizeof(m_rcNewPos));
}

LPCTSTR CVerticalLayoutUI::GetClass() const
{
    return DUI_CTR_VERTICALLAYOUT;
}

LPVOID CVerticalLayoutUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcscmp(pstrName, DUI_CTR_VERTICALLAYOUT) == 0) { return static_cast<CVerticalLayoutUI *>(this); }

    return CContainerUI::GetInterface(pstrName);
}

UINT CVerticalLayoutUI::GetControlFlags() const
{
    if (IsEnabled() && m_iSepHeight != 0) { return UIFLAG_SETCURSOR; }
    else { return 0; }
}

void CVerticalLayoutUI::SetPos(RECT rc, bool bNeedInvalidate)
{
    CControlUI::SetPos(rc, bNeedInvalidate);
    rc = m_rcItem;

    // Adjust for inset
    // 2019-05-30 zhuyadong 排除边框占用的空间
    rc.left += (m_rcBorderSize.left + m_rcPadding.left);
    rc.top += (m_rcBorderSize.top + m_rcPadding.top);
    rc.right -= (m_rcBorderSize.right + m_rcPadding.right);
	rc.bottom -= (m_rcBorderSize.bottom + m_rcPadding.bottom) + m_iSepHeight;//2021-09-06 zm 解决分隔符无效的bug

    if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible()) { rc.right -= m_pVerticalScrollBar->GetFixedWidth(); }

    if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible()) { rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight(); }

    if (m_items.GetSize() == 0)
    {
        ProcessScrollBar(rc, 0, 0);
        return;
    }

    // Determine the minimum size
    SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };

    if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible())
    { szAvailable.cx += m_pHorizontalScrollBar->GetScrollRange(); }

    if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible())
    { szAvailable.cy += m_pVerticalScrollBar->GetScrollRange(); }

    int cxNeeded = 0;
    int nAdjustables = 0;
    int cyFixed = 0;
    int nEstimateNum = 0;
    SIZE szControlAvailable;
    int iControlMaxWidth = 0;
    int iControlMaxHeight = 0;

    for (int it1 = 0; it1 < m_items.GetSize(); it1++)
    {
        CControlUI *pControl = static_cast<CControlUI *>(m_items[it1]);

        if (!pControl->IsVisible()) { continue; }

        if (pControl->IsFloat()) { continue; }

        szControlAvailable = szAvailable;
        RECT rcMargin = pControl->GetMargin();
        szControlAvailable.cx -= rcMargin.left + rcMargin.right;
        iControlMaxWidth = pControl->GetFixedWidth();
        iControlMaxHeight = pControl->GetFixedHeight();

        if (iControlMaxWidth <= 0) { iControlMaxWidth = pControl->GetMaxWidth(); }

        if (iControlMaxHeight <= 0) { iControlMaxHeight = pControl->GetMaxHeight(); }

        if (szControlAvailable.cx > iControlMaxWidth) { szControlAvailable.cx = iControlMaxWidth; }

        if (szControlAvailable.cy > iControlMaxHeight) { szControlAvailable.cy = iControlMaxHeight; }

        SIZE sz = pControl->EstimateSize(szControlAvailable);

        if (sz.cy == 0)
        {
            nAdjustables++;
        }
        else
        {
            if (sz.cy < pControl->GetMinHeight()) { sz.cy = pControl->GetMinHeight(); }

            if (sz.cy > pControl->GetMaxHeight()) { sz.cy = pControl->GetMaxHeight(); }
        }

        cyFixed += sz.cy + pControl->GetMargin().top + pControl->GetMargin().bottom;

        sz.cx = std::max<int>(sz.cx, 0);

        if (sz.cx < pControl->GetMinWidth()) { sz.cx = pControl->GetMinWidth(); }

        if (sz.cx > pControl->GetMaxWidth()) { sz.cx = pControl->GetMaxWidth(); }

        cxNeeded = std::max<int>(cxNeeded, sz.cx + rcMargin.left + rcMargin.right);
        nEstimateNum++;
    }

    cyFixed += (nEstimateNum - 1) * m_iChildMargin;

    // Place elements
    int cyNeeded = 0;
    int cyExpand = 0;

    if (nAdjustables > 0) { cyExpand = std::max<int>(0, (szAvailable.cy - cyFixed) / nAdjustables); }

    // Position the elements
    SIZE szRemaining = szAvailable;
    int iPosY = rc.top;

    if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible())
    {
        iPosY -= m_pVerticalScrollBar->GetScrollPos();
    }

    int iEstimate = 0;
    int iAdjustable = 0;
    int cyFixedRemaining = cyFixed;

    for (int it2 = 0; it2 < m_items.GetSize(); it2++)
    {
        CControlUI *pControl = static_cast<CControlUI *>(m_items[it2]);

        if (!pControl->IsVisible()) { continue; }

        if (pControl->IsFloat())
        {
            SetFloatPos(it2);
            continue;
        }

        iEstimate += 1;
        RECT rcMargin = pControl->GetMargin();
        szRemaining.cy -= rcMargin.top;

        szControlAvailable = szRemaining;
        szControlAvailable.cx -= rcMargin.left + rcMargin.right;
        iControlMaxWidth = pControl->GetFixedWidth();
        iControlMaxHeight = pControl->GetFixedHeight();

        if (iControlMaxWidth <= 0) { iControlMaxWidth = pControl->GetMaxWidth(); }

        if (iControlMaxHeight <= 0) { iControlMaxHeight = pControl->GetMaxHeight(); }

        if (szControlAvailable.cx > iControlMaxWidth) { szControlAvailable.cx = iControlMaxWidth; }

        if (szControlAvailable.cy > iControlMaxHeight) { szControlAvailable.cy = iControlMaxHeight; }

        cyFixedRemaining = cyFixedRemaining - (rcMargin.top + rcMargin.bottom);

        if (iEstimate > 1) { cyFixedRemaining = cyFixedRemaining - m_iChildMargin; }

        SIZE sz = pControl->EstimateSize(szControlAvailable);

        if (sz.cy == 0)
        {
            iAdjustable++;
            sz.cy = cyExpand;

            // Distribute remaining to last element (usually round-off left-overs)
            if (iAdjustable == nAdjustables)
            {
                sz.cy = std::max<int>(0, szRemaining.cy - rcMargin.bottom - cyFixedRemaining);
            }

            if (sz.cy < pControl->GetMinHeight()) { sz.cy = pControl->GetMinHeight(); }

            if (sz.cy > pControl->GetMaxHeight()) { sz.cy = pControl->GetMaxHeight(); }
        }
        else
        {
            if (sz.cy < pControl->GetMinHeight()) { sz.cy = pControl->GetMinHeight(); }

            if (sz.cy > pControl->GetMaxHeight()) { sz.cy = pControl->GetMaxHeight(); }

            cyFixedRemaining -= sz.cy;
        }

        sz.cx = pControl->GetMaxWidth();

        if (sz.cx == 0) { sz.cx = szAvailable.cx - rcMargin.left - rcMargin.right; }

        if (sz.cx < 0) { sz.cx = 0; }

        if (sz.cx > szControlAvailable.cx) { sz.cx = szControlAvailable.cx; }

        if (sz.cx < pControl->GetMinWidth()) { sz.cx = pControl->GetMinWidth(); }

        UINT iChildAlign = GetChildAlign();

        if (iChildAlign == DT_CENTER)
        {
            int iPosX = (rc.right + rc.left) / 2;

            if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible())
            {
                iPosX += m_pHorizontalScrollBar->GetScrollRange() / 2;
                iPosX -= m_pHorizontalScrollBar->GetScrollPos();
            }

            RECT rcCtrl = { iPosX - sz.cx / 2, iPosY + rcMargin.top, iPosX + sz.cx - sz.cx / 2, iPosY + sz.cy + rcMargin.top };
            pControl->SetPos(rcCtrl, false);
        }
        else if (iChildAlign == DT_RIGHT)
        {
            int iPosX = rc.right;

            if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible())
            {
                iPosX += m_pHorizontalScrollBar->GetScrollRange();
                iPosX -= m_pHorizontalScrollBar->GetScrollPos();
            }

            RECT rcCtrl = { iPosX - rcMargin.right - sz.cx, iPosY + rcMargin.top, iPosX - rcMargin.right, iPosY + sz.cy + rcMargin.top };
            pControl->SetPos(rcCtrl, false);
        }
        else
        {
            int iPosX = rc.left;

            if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible())
            {
                iPosX -= m_pHorizontalScrollBar->GetScrollPos();
            }

            RECT rcCtrl = { iPosX + rcMargin.left, iPosY + rcMargin.top, iPosX + rcMargin.left + sz.cx, iPosY + sz.cy + rcMargin.top };
            pControl->SetPos(rcCtrl, false);
        }

        iPosY += sz.cy + m_iChildMargin + rcMargin.top + rcMargin.bottom;
        cyNeeded += sz.cy + rcMargin.top + rcMargin.bottom;
        szRemaining.cy -= sz.cy + m_iChildMargin + rcMargin.bottom;
    }

    cyNeeded += (nEstimateNum - 1) * m_iChildMargin;

    // Process the scrollbar
    ProcessScrollBar(rc, cxNeeded, cyNeeded);
}

void CVerticalLayoutUI::DoPostPaint(HDC hDC, const RECT &rcPaint)
{
    if ((m_uButtonState & UISTATE_CAPTURED) != 0 && !m_bImmMode)
    {
        RECT rcSeparator = GetThumbRect(true);
        CRenderEngine::DrawColor(hDC, rcSeparator, 0xAA000000);
    }
}

void CVerticalLayoutUI::SetSepHeight(int iHeight)
{
    m_iSepHeight = iHeight;
}

int CVerticalLayoutUI::GetSepHeight() const
{
    return m_iSepHeight;
}

void CVerticalLayoutUI::SetSepImmMode(bool bImmediately)
{
    if (m_bImmMode == bImmediately) { return; }

    if ((m_uButtonState & UISTATE_CAPTURED) != 0 && !m_bImmMode && m_pManager != NULL)
    {
        m_pManager->RemovePostPaint(this);
    }

    m_bImmMode = bImmediately;
}

bool CVerticalLayoutUI::IsSepImmMode() const
{
    return m_bImmMode;
}

void CVerticalLayoutUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if (_tcscmp(pstrName, _T("sepheight")) == 0) { SetSepHeight(ParseInt(pstrValue)); }
	//2021-09-08 zm
	else if (_tcscmp(pstrName, _T("sepcolor")) == 0) { SetSepImmColor(ParseColor(pstrValue)); }
    else if (_tcscmp(pstrName, _T("sepimm")) == 0) { SetSepImmMode(ParseBool(pstrValue)); }
    else if (_tcscmp(pstrName, _T("autowidth")) == 0) { DUITRACE(_T("不支持属性:autowidth")); }
    else { CContainerUI::SetAttribute(pstrName, pstrValue); }
}

void CVerticalLayoutUI::DoEvent(TEventUI &event)
{
    if (m_iSepHeight != 0)
    {
        if (event.Type == UIEVENT_BUTTONDOWN && IsEnabled())
        {
            SetCapture();
            RECT rcSeparator = GetThumbRect(false);

            if (::PtInRect(&rcSeparator, event.ptMouse))
            {
                m_uButtonState |= UISTATE_CAPTURED;
                m_ptLastMouse = event.ptMouse;
                m_rcNewPos = m_rcItem;

                if (!m_bImmMode && m_pManager) { m_pManager->AddPostPaint(this); }

                return;
            }
        }

        if (event.Type == UIEVENT_BUTTONUP)
        {
            ReleaseCapture();

            if ((m_uButtonState & UISTATE_CAPTURED) != 0)
            {
                m_uButtonState &= ~UISTATE_CAPTURED;
                m_rcItem = m_rcNewPos;

                if (!m_bImmMode && m_pManager) { m_pManager->RemovePostPaint(this); }

                NeedParentUpdate();
                return;
            }
        }

        if (event.Type == UIEVENT_MOUSEMOVE)
        {
            if ((m_uButtonState & UISTATE_CAPTURED) != 0)
            {
                LONG cy = event.ptMouse.y - m_ptLastMouse.y;
                m_ptLastMouse = event.ptMouse;
                RECT rc = m_rcNewPos;

                if (m_iSepHeight >= 0)
                {
                    if (cy > 0 && event.ptMouse.y < m_rcNewPos.bottom + m_iSepHeight) { return; }

                    if (cy < 0 && event.ptMouse.y > m_rcNewPos.bottom) { return; }

                    rc.bottom += cy;

                    if (rc.bottom - rc.top <= GetMinHeight())
                    {
                        if (m_rcNewPos.bottom - m_rcNewPos.top <= GetMinHeight()) { return; }

                        rc.bottom = rc.top + GetMinHeight();
                    }

                    if (rc.bottom - rc.top >= GetMaxHeight())
                    {
                        if (m_rcNewPos.bottom - m_rcNewPos.top >= GetMaxHeight()) { return; }

                        rc.bottom = rc.top + GetMaxHeight();
                    }
                }
                else
                {
                    if (cy > 0 && event.ptMouse.y < m_rcNewPos.top) { return; }

                    if (cy < 0 && event.ptMouse.y > m_rcNewPos.top + m_iSepHeight) { return; }

                    rc.top += cy;

                    if (rc.bottom - rc.top <= GetMinHeight())
                    {
                        if (m_rcNewPos.bottom - m_rcNewPos.top <= GetMinHeight()) { return; }

                        rc.top = rc.bottom - GetMinHeight();
                    }

                    if (rc.bottom - rc.top >= GetMaxHeight())
                    {
                        if (m_rcNewPos.bottom - m_rcNewPos.top >= GetMaxHeight()) { return; }

                        rc.top = rc.bottom - GetMaxHeight();
                    }
                }

                CDuiRect rcInvalidate = GetThumbRect(true);
                m_rcNewPos = rc;
                m_cxyFixed.cy = m_rcNewPos.bottom - m_rcNewPos.top;

                if (m_bImmMode)
                {
                    m_rcItem = m_rcNewPos;
                    NeedParentUpdate();
                }
                else
                {
                    rcInvalidate.Join(GetThumbRect(true));
                    rcInvalidate.Join(GetThumbRect(false));

                    if (m_pManager) { m_pManager->Invalidate(rcInvalidate); }
                }

                return;
            }

            if ((event.wParam & MK_LBUTTON) && m_bDragEnable)
            {
                OnDoDragDrop(event);
                return;
            }
        }

        if (event.Type == UIEVENT_SETCURSOR)
        {
            RECT rcSeparator = GetThumbRect(false);

            if (IsEnabled() && ::PtInRect(&rcSeparator, event.ptMouse))
            {
                ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENS)));
                return;
            }
        }
    }

    CContainerUI::DoEvent(event);
}

SIZE CVerticalLayoutUI::EstimateSize(SIZE szAvailable)
{
    SIZE sz = CControlUI::EstimateSize(szAvailable);

    if (m_bAutoHeight)
    {
        sz.cy = m_iChildMargin * (GetCount() - 1);

        for (int i = 0; i < GetCount(); ++i)
        {
            CControlUI *pCtrl = GetItemAt(i);

            if (!pCtrl->IsVisible()) { continue; }

            SIZE sz2 = pCtrl->EstimateSize(szAvailable);
            sz.cy += sz2.cy;
        }

        sz.cy += m_rcPadding.top + m_rcPadding.bottom + m_rcBorderSize.top + m_rcBorderSize.bottom;
        m_cxyFixed.cy = sz.cy;
    }

    return sz;
}

RECT CVerticalLayoutUI::GetThumbRect(bool bUseNew) const
{
    if ((m_uButtonState & UISTATE_CAPTURED) != 0 && bUseNew)
    {
        if (m_iSepHeight >= 0)
            return CDuiRect(m_rcNewPos.left, std::max<int>(m_rcNewPos.bottom - m_iSepHeight, m_rcNewPos.top),
                            m_rcNewPos.right, m_rcNewPos.bottom);
        else
            return CDuiRect(m_rcNewPos.left, m_rcNewPos.top, m_rcNewPos.right,
                            std::min<long>(m_rcNewPos.top - m_iSepHeight, m_rcNewPos.bottom));
    }
    else
    {
        if (m_iSepHeight >= 0)
            return CDuiRect(m_rcItem.left, std::max<int>(m_rcItem.bottom - m_iSepHeight, m_rcItem.top),
                            m_rcItem.right, m_rcItem.bottom);
        else
            return CDuiRect(m_rcItem.left, m_rcItem.top, m_rcItem.right,
                            std::min<long>(m_rcItem.top - m_iSepHeight, m_rcItem.bottom));

    }
}

void CVerticalLayoutUI::SetSepImmColor(DWORD dwSepImmColor)
{
	if (m_dwSepImmColor == dwSepImmColor){ return; }

	m_dwSepImmColor = dwSepImmColor;
	Invalidate();
}

DWORD CVerticalLayoutUI::GetSepImmColor()
{
	return m_dwSepImmColor;
}

void CVerticalLayoutUI::PaintBkColor(HDC hDC)
{
	CContainerUI::PaintBkColor(hDC);

	if (0 != m_iSepHeight)
	{
		RECT rcSeparator = GetThumbRect(true);
		CRenderEngine::DrawColor(hDC, rcSeparator, m_dwSepImmColor);
	}
}

}
