﻿#include "StdAfx.h"
#include <algorithm>

namespace DuiLib {


/////////////////////////////////////////////////////////////////////////////////////
//

class CListBodyUI : public CVerticalLayoutUI
{
public:
    CListBodyUI(CListUI *pOwner);

    void SetScrollPos(SIZE szPos);
    void SetPos(RECT rc, bool bNeedInvalidate = true);
    void DoEvent(TEventUI &event);
    bool DoPaint(HDC hDC, const RECT &rcPaint, CControlUI *pStopControl);
    bool SortItems(PULVCompareFunc pfnCompare, UINT_PTR dwData, int &iCurSel);

	//2021-10-17 zm 添加橡皮筋框选
	POINT GetSatrtPoint();
	void SetStartPoint(POINT stPoint);
	virtual void DoPostPaint(HDC hDC, const RECT &rcPaint) override;

protected:
    static int __cdecl ItemComareFunc(void *pvlocale, const void *item1, const void *item2);
    int __cdecl ItemComareFunc(const void *item1, const void *item2);

protected:
    CListUI *m_pOwner;
    PULVCompareFunc m_pCompareFunc;
    UINT_PTR m_compareData;

	POINT m_stMousePoint;
};

/////////////////////////////////////////////////////////////////////////////////////
//
//

CListUI::CListUI() : m_pCallback(NULL), m_bScrollSelect(false), m_iCurSel(-1), m_iExpandedItem(-1),
m_nRow(-1), m_nColumn(-1), m_pCmbCallback(NULL), m_pEdit(NULL), m_pCombo(NULL), m_bMultiSel(false)
, m_iDragBeginIndex(-1), m_szCheckBox({ 0 })//zm
{
    m_pList = new CListBodyUI(this);
    m_pHeader = new CListHeaderUI;

    Add(m_pHeader);
    CVerticalLayoutUI::Add(m_pList);

    m_ListInfo.nColumns = 0;
    ::ZeroMemory(&m_ListInfo.rcColumn, sizeof(m_ListInfo.rcColumn));
    m_ListInfo.uFixedHeight = 0;
    m_ListInfo.nFont = -1;
    m_ListInfo.uTextStyle = DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS;
    m_ListInfo.rcPadding.left = m_ListInfo.rcPadding.right = 0;
    m_ListInfo.rcPadding.top = m_ListInfo.rcPadding.bottom = 0;
    m_ListInfo.dwTextColor = 0;
    m_ListInfo.dwBkColor = 0;
    m_ListInfo.bAlternateBk = false;
    m_ListInfo.dwSelectedTextColor = 0xFF000000;
    m_ListInfo.dwSelectedBkColor = 0xFFC1E3FF;
    m_ListInfo.dwHotTextColor = 0xFF000000;
    m_ListInfo.dwHotBkColor = 0xFFE9F5FF;
    m_ListInfo.dwDisabledTextColor = 0;
    m_ListInfo.dwDisabledBkColor = 0xFFFFFFFF;
    m_ListInfo.iHLineSize = 0;
    m_ListInfo.dwHLineColor = 0xFF3C3C3C;
    m_ListInfo.iVLineSize = 0;
    m_ListInfo.dwVLineColor = 0xFF3C3C3C;
    m_ListInfo.bShowHtml = false;
    m_ListInfo.bMultiExpandable = false;
    m_ListInfo.bCheckBox = false;
}

LPCTSTR CListUI::GetClass() const
{
    return DUI_CTR_LIST;
}

UINT CListUI::GetControlFlags() const
{
    return UIFLAG_TABSTOP;
}

LPVOID CListUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcscmp(pstrName, DUI_CTR_LIST) == 0) { return static_cast<CListUI *>(this); }

    if (_tcscmp(pstrName, DUI_CTR_ILIST) == 0) { return static_cast<IListUI *>(this); }

    if (_tcscmp(pstrName, DUI_CTR_ILISTOWNER) == 0) { return static_cast<IListOwnerUI *>(this); }

    return CVerticalLayoutUI::GetInterface(pstrName);
}

void CListUI::SetEnabled(bool bEnable /*= true*/)
{
    CVerticalLayoutUI::SetEnabled(bEnable);

    if (NULL != m_pHeader) { m_pHeader->SetEnabled(IsEnabled()); }

    if (NULL != m_pList) { m_pList->SetEnabled(IsEnabled()); }
}

CControlUI *CListUI::GetItemAt(int iIndex) const
{
    return m_pList->GetItemAt(iIndex);
}

int CListUI::GetItemIndex(CControlUI *pControl) const
{
    if (pControl->GetInterface(DUI_CTR_LISTHEADER) != NULL) { return CVerticalLayoutUI::GetItemIndex(pControl); }

    // We also need to recognize header sub-items
    if (_tcsstr(pControl->GetClass(), DUI_CTR_LISTHEADERITEM) != NULL) { return m_pHeader->GetItemIndex(pControl); }

    return m_pList->GetItemIndex(pControl);
}

bool CListUI::SetItemIndex(CControlUI *pControl, int iIndex)
{
    if (pControl->GetInterface(DUI_CTR_LISTHEADER) != NULL) { return CVerticalLayoutUI::SetItemIndex(pControl, iIndex); }

    // We also need to recognize header sub-items
    if (_tcsstr(pControl->GetClass(), DUI_CTR_LISTHEADERITEM) != NULL) { return m_pHeader->SetItemIndex(pControl, iIndex); }

    int iOrginIndex = m_pList->GetItemIndex(pControl);

    if (iOrginIndex == -1) { return false; }

    if (iOrginIndex == iIndex) { return true; }

    IListItemUI *pSelectedListItem = NULL;

    if (m_iCurSel >= 0) pSelectedListItem =
            static_cast<IListItemUI *>(GetItemAt(m_iCurSel)->GetInterface(DUI_CTR_ILISTITEM));

    if (!m_pList->SetItemIndex(pControl, iIndex)) { return false; }

    int iMinIndex = min(iOrginIndex, iIndex);
    int iMaxIndex = max(iOrginIndex, iIndex);

    for (int i = iMinIndex; i < iMaxIndex + 1; ++i)
    {
        CControlUI *p = m_pList->GetItemAt(i);
        IListItemUI *pListItem = static_cast<IListItemUI *>(p->GetInterface(DUI_CTR_ILISTITEM));

        if (pListItem != NULL) { pListItem->SetIndex(i); }
    }

    if (m_iCurSel >= 0 && pSelectedListItem != NULL) { m_iCurSel = pSelectedListItem->GetIndex(); }

    return true;
}

bool CListUI::SetMultiItemIndex(CControlUI *pStartControl, int iCount, int iNewStartIndex)
{
    if (pStartControl == NULL || iCount < 0 || iNewStartIndex < 0) { return false; }

    if (pStartControl->GetInterface(DUI_CTR_LISTHEADER) != NULL) { return CVerticalLayoutUI::SetMultiItemIndex(pStartControl, iCount, iNewStartIndex); }

    // We also need to recognize header sub-items
    if (_tcsstr(pStartControl->GetClass(), DUI_CTR_LISTHEADERITEM) != NULL) { return m_pHeader->SetMultiItemIndex(pStartControl, iCount, iNewStartIndex); }

    int iStartIndex = GetItemIndex(pStartControl);

    if (iStartIndex == iNewStartIndex) { return true; }

    if (iStartIndex + iCount > GetCount()) { return false; }

    if (iNewStartIndex + iCount > GetCount()) { return false; }

    IListItemUI *pSelectedListItem = NULL;

    if (m_iCurSel >= 0) pSelectedListItem =
            static_cast<IListItemUI *>(GetItemAt(m_iCurSel)->GetInterface(DUI_CTR_ILISTITEM));

    if (!m_pList->SetMultiItemIndex(pStartControl, iCount, iNewStartIndex)) { return false; }

    int iMinIndex = min(iStartIndex, iNewStartIndex);
    int iMaxIndex = max(iStartIndex + iCount, iNewStartIndex + iCount);

    for (int i = iMinIndex; i < iMaxIndex + 1; ++i)
    {
        CControlUI *p = m_pList->GetItemAt(i);
        IListItemUI *pListItem = static_cast<IListItemUI *>(p->GetInterface(DUI_CTR_ILISTITEM));

        if (pListItem != NULL) { pListItem->SetIndex(i); }
    }

    if (m_iCurSel >= 0 && pSelectedListItem != NULL) { m_iCurSel = pSelectedListItem->GetIndex(); }

    return true;
}

int CListUI::GetCount() const
{
    return m_pList->GetCount();
}

bool CListUI::Add(CControlUI *pControl)
{
    if (NULL == pControl) { return false; }

    pControl->SetEnabled(IsEnabled());

    // Override the Add() method so we can add items specifically to
    // the intended widgets. Headers are assumed to be
    // answer the correct interface so we can add multiple list headers.
    if (pControl->GetInterface(DUI_CTR_LISTHEADER) != NULL)
    {
        if (m_pHeader != pControl && m_pHeader->GetCount() == 0)
        {
            CVerticalLayoutUI::Remove(m_pHeader);
            m_pHeader = static_cast<CListHeaderUI *>(pControl);
        }

        m_ListInfo.nColumns = std::min<int>(m_pHeader->GetCount(), UILIST_MAX_COLUMNS);
        return CVerticalLayoutUI::AddAt(pControl, 0);
    }

    // We also need to recognize header sub-items
    if (_tcsstr(pControl->GetClass(), DUI_CTR_LISTHEADERITEM) != NULL)
    {
        bool ret = m_pHeader->Add(pControl);
        m_ListInfo.nColumns = std::min<int>(m_pHeader->GetCount(), UILIST_MAX_COLUMNS);
        return ret;
    }

    // The list items should know about us
    IListItemUI *pListItem = static_cast<IListItemUI *>(pControl->GetInterface(DUI_CTR_ILISTITEM));

    if (pListItem != NULL)
    {
        pListItem->SetOwner(this);
        pListItem->SetIndex(GetCount());
    }

    return m_pList->Add(pControl);
}

bool CListUI::AddAt(CControlUI *pControl, int iIndex)
{
    if (NULL == pControl) { return false; }

    pControl->SetEnabled(IsEnabled());

    // Override the AddAt() method so we can add items specifically to
    // the intended widgets. Headers and are assumed to be
    // answer the correct interface so we can add multiple list headers.
    if (pControl->GetInterface(DUI_CTR_LISTHEADER) != NULL)
    {
        if (m_pHeader != pControl && m_pHeader->GetCount() == 0)
        {
            CVerticalLayoutUI::Remove(m_pHeader);
            m_pHeader = static_cast<CListHeaderUI *>(pControl);
        }

        m_ListInfo.nColumns = std::min<int>(m_pHeader->GetCount(), UILIST_MAX_COLUMNS);
        return CVerticalLayoutUI::AddAt(pControl, 0);
    }

    // We also need to recognize header sub-items
    if (_tcsstr(pControl->GetClass(), DUI_CTR_LISTHEADERITEM) != NULL)
    {
        bool ret = m_pHeader->AddAt(pControl, iIndex);
        m_ListInfo.nColumns = std::min<int>(m_pHeader->GetCount(), UILIST_MAX_COLUMNS);
        return ret;
    }

    if (!m_pList->AddAt(pControl, iIndex)) { return false; }

    // The list items should know about us
    IListItemUI *pListItem = static_cast<IListItemUI *>(pControl->GetInterface(DUI_CTR_ILISTITEM));

    if (pListItem != NULL)
    {
        pListItem->SetOwner(this);
        pListItem->SetIndex(iIndex);
    }

    for (int i = iIndex + 1; i < m_pList->GetCount(); ++i)
    {
        CControlUI *p = m_pList->GetItemAt(i);
        pListItem = static_cast<IListItemUI *>(p->GetInterface(DUI_CTR_ILISTITEM));

        if (pListItem != NULL) { pListItem->SetIndex(i); }
    }

    if (m_iCurSel >= iIndex) { m_iCurSel += 1; }

    return true;
}

bool CListUI::Remove(CControlUI *pControl, bool bDoNotDestroy)
{
    if (pControl->GetInterface(DUI_CTR_LISTHEADER) != NULL) { return CVerticalLayoutUI::Remove(pControl, bDoNotDestroy); }

    // We also need to recognize header sub-items
    if (_tcsstr(pControl->GetClass(), DUI_CTR_LISTHEADERITEM) != NULL) { return m_pHeader->Remove(pControl, bDoNotDestroy); }

    int iIndex = m_pList->GetItemIndex(pControl);

    if (iIndex == -1) { return false; }

    if (!m_pList->RemoveAt(iIndex, bDoNotDestroy)) { return false; }

    for (int i = iIndex; i < m_pList->GetCount(); ++i)
    {
        CControlUI *p = m_pList->GetItemAt(i);
        IListItemUI *pListItem = static_cast<IListItemUI *>(p->GetInterface(DUI_CTR_ILISTITEM));

        if (pListItem != NULL) { pListItem->SetIndex(i); }
    }

    if (iIndex == m_iCurSel && m_iCurSel >= 0)
    {
        int iSel = m_iCurSel;
        m_iCurSel = -1;
        SelectItem(FindSelectable(iSel, false));
    }
    else if (iIndex < m_iCurSel) { m_iCurSel -= 1; }

    return true;
}

bool CListUI::RemoveAt(int iIndex, bool bDoNotDestroy)
{
    if (!m_pList->RemoveAt(iIndex, bDoNotDestroy)) { return false; }

    for (int i = iIndex; i < m_pList->GetCount(); ++i)
    {
        CControlUI *p = m_pList->GetItemAt(i);
        IListItemUI *pListItem = static_cast<IListItemUI *>(p->GetInterface(DUI_CTR_ILISTITEM));

        if (pListItem != NULL) { pListItem->SetIndex(i); }
    }

    if (iIndex == m_iCurSel && m_iCurSel >= 0)
    {
        int iSel = m_iCurSel;
        m_iCurSel = -1;
        SelectItem(FindSelectable(iSel, false));
    }
    else if (iIndex < m_iCurSel) { m_iCurSel -= 1; }

    return true;
}

void CListUI::RemoveAll()
{
    m_iCurSel = -1;
    m_iExpandedItem = -1;
    m_pList->RemoveAll();
	m_aSelItems.Empty();//zm
}

bool CListUI::RemoveCount(int iIndex, int iCount, bool bDoNotDestroy)
{
    if (!m_pList->RemoveCount(iIndex, iCount, bDoNotDestroy)) { return false; }

    for (int i = iIndex; i < m_pList->GetCount(); ++i)
    {
        CControlUI *p = m_pList->GetItemAt(i);
        IListItemUI *pListItem = static_cast<IListItemUI *>(p->GetInterface(DUI_CTR_ILISTITEM));

        if (pListItem != NULL) { pListItem->SetIndex(i); }
    }

    if (iIndex == m_iCurSel && m_iCurSel >= 0)
    {
        int iSel = m_iCurSel;
        m_iCurSel = -1;
        SelectItem(FindSelectable(iSel, false));
    }
    else if (iIndex < m_iCurSel) { m_iCurSel -= 1; }

    return true;
}

int CListUI::FindItemByTag(UINT_PTR pTag)
{
    return m_pList->FindItemByTag(pTag);
}

int CListUI::FindItemByUserData(LPCTSTR pstrText)
{
    return m_pList->FindItemByUserData(pstrText);
}

DuiLib::CControlUI *CListUI::GetItemByTag(UINT_PTR pTag)
{
    return m_pList->GetItemByTag(pTag);
}

DuiLib::CControlUI *CListUI::GetItemByUserData(LPCTSTR pstrText)
{
    return m_pList->GetItemByUserData(pstrText);
}

void CListUI::SetPos(RECT rc, bool bNeedInvalidate)
{
    if (m_pHeader != NULL)    // 设置header各子元素x坐标,因为有些listitem的setpos需要用到(临时修复)
    {
        int iLeft = rc.left + m_rcPadding.left + m_rcBorderSize.left;
        int iRight = rc.right - m_rcPadding.right - m_rcBorderSize.right;

        m_ListInfo.nColumns = std::min<int>(m_pHeader->GetCount(), UILIST_MAX_COLUMNS);

        if (!m_pHeader->IsVisible())
        {
            for (int it = m_pHeader->GetCount() - 1; it >= 0; it--)
            {
                static_cast<CControlUI *>(m_pHeader->GetItemAt(it))->SetInternVisible(true);
            }
        }

        m_pHeader->SetPos(CDuiRect(iLeft, 0, iRight, 0), false);
        int iOffset = m_pList->GetScrollPos().cx;

        for (int i = 0; i < m_ListInfo.nColumns; i++)
        {
            CControlUI *pControl = static_cast<CControlUI *>(m_pHeader->GetItemAt(i));

            if (!pControl->IsVisible()) { continue; }

            if (pControl->IsFloat()) { continue; }

            RECT rcPos = pControl->GetPos();

            if (iOffset > 0)
            {
                rcPos.left -= iOffset;
                rcPos.right -= iOffset;
                pControl->SetPos(rcPos, false);
            }

            m_ListInfo.rcColumn[i] = pControl->GetPos();
        }

        if (!m_pHeader->IsVisible())
        {
            for (int it = m_pHeader->GetCount() - 1; it >= 0; it--)
            {
                static_cast<CControlUI *>(m_pHeader->GetItemAt(it))->SetInternVisible(false);
            }

            m_pHeader->SetInternVisible(false);
        }
    }

    CVerticalLayoutUI::SetPos(rc, bNeedInvalidate);

    // 当 ListUI改变大小时，重新计算 编辑框/下拉框的大小
    CListElementUI *pItem = dynamic_cast<CListElementUI *>(GetItemAt(m_nRow));
    RECT rt = pItem ? pItem->GetSubItemPos(m_nColumn, false) : RECT{ 0, 0, 0, 0 };
    rt.left -= m_rcItem.left;   rt.right -= m_rcItem.left;
    rt.top -= m_rcItem.top;     rt.bottom -= m_rcItem.top;
    (m_pEdit && m_pEdit->IsVisible()) ? m_pEdit->SetPos(rt) : NULL; //lint !e62
    (m_pCombo && m_pCombo->IsVisible()) ? m_pCombo->SetPos(rt) : NULL; //lint !e62

    if (m_pHeader == NULL) { return; }

    rc = m_rcItem;
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

    m_ListInfo.nColumns = std::min<int>(m_pHeader->GetCount(), UILIST_MAX_COLUMNS);

    if (!m_pHeader->IsVisible())
    {
        for (int it = m_pHeader->GetCount() - 1; it >= 0; it--)
        {
            static_cast<CControlUI *>(m_pHeader->GetItemAt(it))->SetInternVisible(true);
        }

        m_pHeader->SetPos(CDuiRect(rc.left, 0, rc.right, 0), false);
    }

    int iOffset = m_pList->GetScrollPos().cx;

    for (int i = 0; i < m_ListInfo.nColumns; i++)
    {
        CControlUI *pControl = static_cast<CControlUI *>(m_pHeader->GetItemAt(i));

        if (!pControl->IsVisible()) { continue; }

        if (pControl->IsFloat()) { continue; }

        RECT rcPos = pControl->GetPos();

        if (iOffset > 0)
        {
            rcPos.left -= iOffset;
            rcPos.right -= iOffset;
            pControl->SetPos(rcPos, false);
        }

        m_ListInfo.rcColumn[i] = pControl->GetPos();
    }

    if (!m_pHeader->IsVisible())
    {
        for (int it = m_pHeader->GetCount() - 1; it >= 0; it--)
        {
            static_cast<CControlUI *>(m_pHeader->GetItemAt(it))->SetInternVisible(false);
        }

        m_pHeader->SetInternVisible(false);
    }
}

void CListUI::Move(SIZE szOffset, bool bNeedInvalidate)
{
    CVerticalLayoutUI::Move(szOffset, bNeedInvalidate);

    if (!m_pHeader->IsVisible()) { m_pHeader->Move(szOffset, false); }
}

void CListUI::DoEvent(TEventUI &event)
{
    if (!IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND)
    {
        if (m_pParent != NULL) { m_pParent->DoEvent(event); }
        else { CVerticalLayoutUI::DoEvent(event); }

        return;
    }

    if (event.Type == UIEVENT_SCROLLWHEEL || event.Type == UIEVENT_BUTTONDOWN)
    {
        // 隐藏编辑框、下拉框
        HideEdit();
        HideCombo();
    }

	//2021-10-16 zm 添加鼠标点击/弹起事件
	if (event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_RBUTTONDOWN)
	{
		if (m_pManager && m_pList)
		{
			m_pList->SetStartPoint(event.ptMouse);//设置鼠标落点的初始位置
			m_pManager->AddPostPaint(m_pList);
		}
	}

	if (event.Type == UIEVENT_BUTTONUP || event.Type == UIEVENT_RBUTTONUP)
	{
		if (m_pManager && m_pList)
		{
			m_pManager->RemovePostPaint(m_pList);
			Invalidate();
		}
	}

	if (event.Type == UIEVENT_MOUSEMOVE)
	{
		if (IsEnabled()) { Invalidate(); }
	}


    if (event.Type == UIEVENT_SETFOCUS)
    {
        if (IsEnabled()) { m_bFocused = true; }

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
            switch (event.chKey)
            {
            //2017-02-25 zhuyadong 修复 case 语句没有 break 的 bug
			//2021-10-16 zm 添加键盘多选
			case VK_UP:
			{
				if (m_aSelItems.GetSize() > 0)
				{
					int index = GetMinSelItemIndex() - 1;

					UnSelectAllItems();
					index > 0 ? SelectItem(index, true) : SelectItem(0, true);
				}
			}
			break;
				
			case VK_DOWN:
			{
				if (m_aSelItems.GetSize() > 0)
				{
					int index = GetMaxSelItemIndex() + 1;

					UnSelectAllItems();
					index + 1 > m_pList->GetCount() ? SelectItem(GetCount() - 1, true) : SelectItem(index, true);
				}
			}
			break;

            case VK_PRIOR:  PageUp();                                               break;

            case VK_NEXT:   PageDown();                                             break;

            case VK_HOME:   SelectItem(FindSelectable(0, false), true);             break;

            case VK_END:    SelectItem(FindSelectable(GetCount() - 1, true), true); break;

            case VK_RETURN: if (m_iCurSel != -1) GetItemAt(m_iCurSel)->Activate();  break;

			case 'A'://zm
			{
				if (IsMultiSelect() && (GetKeyState(VK_CONTROL) & 0x8000)) { SelectAllItems(); }
			}
			break;

			default: break;
            }

            return;
        }
    }

    if (event.Type == UIEVENT_SCROLLWHEEL)
    {
        switch (LOWORD(event.wParam))
        {
        case SB_LINEUP:
            if (m_bScrollSelect) { SelectItem(FindSelectable(m_iCurSel - 1, false), true); }
            else { LineUp(); }

            return;

        case SB_LINEDOWN:
            if (m_bScrollSelect) { SelectItem(FindSelectable(m_iCurSel + 1, true), true); }
            else { LineDown(); }

            return;
        }
    }

    CVerticalLayoutUI::DoEvent(event);
}

CListHeaderUI *CListUI::GetHeader() const
{
    return m_pHeader;
}

CContainerUI *CListUI::GetList() const
{
    return m_pList;
}

bool CListUI::GetScrollSelect()
{
    return m_bScrollSelect;
}

void CListUI::SetScrollSelect(bool bScrollSelect)
{
    m_bScrollSelect = bScrollSelect;
}

int CListUI::GetCurSel() const
{
    return m_iCurSel;
}

bool CListUI::SelectItem(int iIndex, bool bTakeFocus, bool bTriggerEvent)
{
	UnSelectAllItems();// 取消所有选择项

    if (iIndex == m_iCurSel) { return true; }

    int iOldSel = m_iCurSel;

    // We should first unselect the currently selected item
    if (m_iCurSel >= 0)
    {
        CControlUI *pControl = GetItemAt(m_iCurSel);

        if (pControl != NULL)
        {
            IListItemUI *pListItem = static_cast<IListItemUI *>(pControl->GetInterface(DUI_CTR_ILISTITEM));

            if (pListItem != NULL) { pListItem->Select(false, bTriggerEvent); }
        }

        m_iCurSel = -1;
    }

    if (iIndex < 0) { return false; }

    CControlUI *pControl = GetItemAt(iIndex);

    if (pControl == NULL) { return false; }

    if (!pControl->IsVisible()) { return false; }

    if (!pControl->IsEnabled()) { return false; }

    IListItemUI *pListItem = static_cast<IListItemUI *>(pControl->GetInterface(DUI_CTR_ILISTITEM));

    if (pListItem == NULL) { return false; }

    m_iCurSel = iIndex;

    if (!pListItem->Select(true, bTriggerEvent))
    {
        m_iCurSel = -1;
        return false;
    }

	//2021-10-17 zm
	if (0 > m_aSelItems.Find((LPVOID)iIndex))
	{
		m_aSelItems.Add((LPVOID)iIndex);
	}

    EnsureVisible(m_iCurSel);

    if (bTakeFocus) { pControl->SetFocus(); }

    if (m_pManager != NULL && bTriggerEvent)
    {
        m_pManager->SendNotify(this, DUI_MSGTYPE_ITEMSELECT, m_iCurSel, iOldSel);
    }

    return true;
}

void CListUI::SetMultiSelect(bool bMultiSel)
{
	m_bMultiSel = bMultiSel;
	if (!bMultiSel) UnSelectAllItems();
}

bool CListUI::IsMultiSelect() const
{
	return m_bMultiSel;
}

void CListUI::SelectAllItems()
{
	m_aSelItems.Empty();

	for (int i = 0; i < GetCount(); ++i)
	{
		CControlUI* pControl = GetItemAt(i);
		if (pControl == NULL) continue;
		if (!pControl->IsVisible()) continue;
		if (!pControl->IsEnabled()) continue;
		IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(DUI_CTR_ILISTITEM));
		if (pListItem == NULL) continue;
		if (!pListItem->SelectMulti(true)) continue;

		m_aSelItems.Add((LPVOID)i);
		m_iCurSel = i;
	}
}

void CListUI::UnSelectAllItems()
{
	for (int i = 0; i < m_aSelItems.GetSize(); ++i)
	{
		int iSelIndex = (int)m_aSelItems.GetAt(i);
		CControlUI* pControl = GetItemAt(iSelIndex);
		if (pControl == NULL) continue;
		if (!pControl->IsEnabled()) continue;
		IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(DUI_CTR_ILISTITEM));
		if (pListItem == NULL) continue;
		if (!pListItem->SelectMulti(false)) continue;
	}

	m_aSelItems.Empty();
	m_iCurSel = -1;
}

bool CListUI::IsPtInSelItem(POINT& stPoint)
{
	for (int iIndex = 0; iIndex < m_aSelItems.GetSize(); iIndex++)
	{
		int iSelIndex = (int)m_aSelItems.GetAt(iIndex);
		CControlUI* pContorl = GetItemAt(iSelIndex);

		if (pContorl && ::PtInRect(&pContorl->GetPos(), stPoint)) { return true; }
	}

	return false; 
}

int CListUI::GetMinSelItemIndex()
{
	if (m_aSelItems.GetSize() <= 0){ return -1; }

	int min = (int)m_aSelItems.GetAt(0);

	for (int i = 0; i < m_aSelItems.GetSize(); ++i)
	{
		int index = (int)m_aSelItems.GetAt(i);

		if (min > index) { min = index; }
	}

	return min;
}

int CListUI::GetMaxSelItemIndex()
{
	if (m_aSelItems.GetSize() <= 0) { return -1; }

	int max = (int)m_aSelItems.GetAt(0);
	
	for (int i = 0; i < m_aSelItems.GetSize(); ++i)
	{
		int index = (int)m_aSelItems.GetAt(i);

		if (max < index) { max = index; }
	}

	return max;
}

bool CListUI::SelectMultiItem(int iIndex, bool bTakeFocus /*= false*/)
{
	if (!IsMultiSelect()) return SelectItem(iIndex, bTakeFocus);

	if (iIndex < 0) { return false; }

	CControlUI* pControl = GetItemAt(iIndex);
	if (pControl == NULL) { return false; }

	CListElementUI* pListElement = static_cast<CListElementUI*>(pControl->GetInterface(DUI_CTR_LISTELEMENT));
	if (pListElement == NULL) { return false; }

	if ((pListElement->GetShiftEnble()) && (m_aSelItems.GetSize() > 0))
	{
		int iMaxIndex = max(GetMinSelItemIndex(), iIndex);
		int iMinIndex = (GetMinSelItemIndex() == -1) ? 0 : min(GetMinSelItemIndex(), iIndex);

		UnSelectAllItems();

		for (int iCount = iMinIndex; iCount < iMaxIndex + 1; iCount++)
		{
			CControlUI* pControl = GetItemAt(iCount);
			if (pControl == NULL) continue;
			if (!pControl->IsVisible()) continue;
			if (!pControl->IsEnabled()) continue;
			IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(DUI_CTR_ILISTITEM));
			if (pListItem == NULL) continue;
			if (!pListItem->SelectMulti(true)) continue;

			m_aSelItems.Add((LPVOID)iCount);
			m_iCurSel = -1;
		}
	}
	else
	{
		IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(DUI_CTR_ILISTITEM));
		if (pListItem == NULL) { return false; }

		if (m_aSelItems.Find((LPVOID)iIndex) >= 0) return false;
		if (!pListItem->SelectMulti(true)) return false;

		m_iCurSel = iIndex;
		m_aSelItems.Add((LPVOID)iIndex);

		if (bTakeFocus) pControl->SetFocus();
	}

	if (m_pManager != NULL)
	{
		m_pManager->SendNotify(this, DUI_MSGTYPE_ITEMSELECT, iIndex);
	}
	return true;
}

bool CListUI::UnSelectItem(int iIndex, bool bOthers /*= false*/)
{
	if (!IsMultiSelect()) { return false; }

	if (bOthers)
	{
		for (int i = m_aSelItems.GetSize() - 1; i >= 0; --i)
		{
			int iSelIndex = (int)m_aSelItems.GetAt(i);
			if (iSelIndex == iIndex) continue;
			CControlUI* pControl = GetItemAt(iSelIndex);
			if (pControl == NULL) continue;
			if (!pControl->IsEnabled()) continue;
			IListItemUI* pSelListItem = static_cast<IListItemUI*>(pControl->GetInterface(DUI_CTR_ILISTITEM));
			if (pSelListItem == NULL) continue;
			if (!pSelListItem->SelectMulti(false)) continue;
			m_aSelItems.Remove(i);
		}
	}
	else
	{
		if (iIndex < 0) return false;
		CControlUI* pControl = GetItemAt(iIndex);
		if (pControl == NULL) return false;
		if (!pControl->IsEnabled()) return false;
		IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(DUI_CTR_ILISTITEM));
		if (pListItem == NULL) return false;
		int aIndex = m_aSelItems.Find((LPVOID)iIndex);
		if (aIndex < 0) return false;
		if (!pListItem->SelectMulti(false)) return false;
		if (m_iCurSel == iIndex) m_iCurSel = -1;
		m_aSelItems.Remove(aIndex);
	}

	return true;
}

int CListUI::GetComboIndex(CDuiString& strItemText)
{
	if (nullptr == m_pCombo) { return -1; }
	if (0 >= m_pCombo->GetCount()) { return -1; }

	for (int i = 0; i < m_pCombo->GetCount(); i++)
	{
		CListElementUI* pobjElement = (CListElementUI*)(m_pCombo->GetItemAt(i));

		if (pobjElement && (0 == pobjElement->GetText().Compare(strItemText))) { return i; }
	}

	return -1;
}

void CListUI::DragBegin(TEventUI& event)
{
	if ((m_uButtonState & UISTATE_CAPTURED) || (m_pManager == NULL)) return;

	m_uButtonState |= UISTATE_CAPTURED;

	RECT rcListBody = GetList()->CControlUI::GetClientPos();
	if (::PtInRect(&rcListBody, event.ptMouse))
	{
		CControlUI* pControl = m_pManager->FindControl(event.ptMouse);
		if (NULL != pControl)
		{
			IListItemUI* pobjBeginItem = static_cast<IListItemUI*>(pControl->GetInterface(DUI_CTR_ILISTITEM));
			if (NULL != pobjBeginItem)
			{
				m_iDragBeginIndex = pobjBeginItem->GetIndex();
			}
		}
	}
}

void CListUI::Draging(TEventUI& event)
{
	if ((m_uButtonState & UISTATE_CAPTURED) && (m_pManager != NULL) && IsMultiSelect())
	{
		int iMinIndex = -1;
		int iMaxIndex = -1;
		RECT rcListBody = GetList()->CControlUI::GetClientPos();

		if (::PtInRect(&rcListBody, event.ptMouse))
		{
			CControlUI* pControlEnd = m_pManager->FindControl(event.ptMouse);
			if (NULL != pControlEnd)
			{
				IListItemUI* pobjEndItem = static_cast<IListItemUI*>(pControlEnd->GetInterface(DUI_CTR_ILISTITEM));
				if (NULL != pobjEndItem)
				{
					iMinIndex = max(min(m_iDragBeginIndex, pobjEndItem->GetIndex()), 0);
					iMaxIndex = min(max(m_iDragBeginIndex, pobjEndItem->GetIndex()), GetCount());
				}
			}
		}

		if ((iMinIndex == -1) || (iMaxIndex == -1)) return;

		UnSelectAllItems();
		for (int iCount = iMinIndex; iCount < iMaxIndex + 1; iCount++)
		{
			CControlUI* pControl = GetItemAt(iCount);
			if (pControl == NULL) continue;
			if (!pControl->IsVisible()) continue;
			if (!pControl->IsEnabled()) continue;
			IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(DUI_CTR_ILISTITEM));
			if (pListItem == NULL) continue;
			if (!pListItem->SelectMulti(true)) continue;

			m_aSelItems.Add((LPVOID)iCount);
			m_iCurSel = iCount;
		}
	}
}

void CListUI::DragEnd(TEventUI& event)
{
	if (m_uButtonState & UISTATE_CAPTURED)
	{
		m_iDragBeginIndex = -1;
		m_uButtonState &= ~UISTATE_CAPTURED;
	}
}

TListInfoUI *CListUI::GetListInfo()
{
    return &m_ListInfo;
}

int CListUI::GetChildMargin() const
{
    return m_pList->GetChildMargin();
}

void CListUI::SetChildMargin(int iMargin)
{
    m_pList->SetChildMargin(iMargin);
}

UINT CListUI::GetItemFixedHeight()
{
    return m_ListInfo.uFixedHeight;
}

void CListUI::SetItemFixedHeight(UINT nHeight)
{
    m_ListInfo.uFixedHeight = nHeight;
    NeedUpdate();
}

int CListUI::GetItemFont(int index)
{
    return m_ListInfo.nFont;
}

void CListUI::SetItemFont(int index)
{
    m_ListInfo.nFont = index;
    NeedUpdate();
}

UINT CListUI::GetItemTextStyle()
{
    return m_ListInfo.uTextStyle;
}

void CListUI::SetItemTextStyle(UINT uStyle)
{
    m_ListInfo.uTextStyle = uStyle;
    NeedUpdate();
}

void CListUI::SetItemPadding(RECT rc)
{
    m_ListInfo.rcPadding = rc;
    NeedUpdate();
}

RECT CListUI::GetItemPadding() const
{
    return m_ListInfo.rcPadding;
}

void CListUI::SetItemTextColor(DWORD dwTextColor)
{
    m_ListInfo.dwTextColor = dwTextColor;
    Invalidate();
}

void CListUI::SetItemBkColor(DWORD dwBkColor)
{
    m_ListInfo.dwBkColor = dwBkColor;
    Invalidate();
}

void CListUI::SetItemBkImage(LPCTSTR pStrImage)
{
    if (m_ListInfo.diBk.sDrawString == pStrImage && m_ListInfo.diBk.pImageInfo != NULL) { return; }

    m_ListInfo.diBk.Clear();
    m_ListInfo.diBk.sDrawString = pStrImage;
    Invalidate();
}

bool CListUI::IsAlternateBk() const
{
    return m_ListInfo.bAlternateBk;
}

void CListUI::SetAlternateBk(bool bAlternateBk)
{
    m_ListInfo.bAlternateBk = bAlternateBk;
    Invalidate();
}

DWORD CListUI::GetItemTextColor() const
{
    return m_ListInfo.dwTextColor;
}

DWORD CListUI::GetItemBkColor() const
{
    return m_ListInfo.dwBkColor;
}

LPCTSTR CListUI::GetItemBkImage() const
{
    return m_ListInfo.diBk.sDrawString;
}

void CListUI::SetSelectedItemTextColor(DWORD dwTextColor)
{
    m_ListInfo.dwSelectedTextColor = dwTextColor;
    Invalidate();
}

void CListUI::SetSelectedItemBkColor(DWORD dwBkColor)
{
    m_ListInfo.dwSelectedBkColor = dwBkColor;
    Invalidate();
}

void CListUI::SetSelectedItemImage(LPCTSTR pStrImage)
{
    if (m_ListInfo.diSelected.sDrawString == pStrImage && m_ListInfo.diSelected.pImageInfo != NULL) { return; }

    m_ListInfo.diSelected.Clear();
    m_ListInfo.diSelected.sDrawString = pStrImage;
    Invalidate();
}

DWORD CListUI::GetSelectedItemTextColor() const
{
    return m_ListInfo.dwSelectedTextColor;
}

DWORD CListUI::GetSelectedItemBkColor() const
{
    return m_ListInfo.dwSelectedBkColor;
}

LPCTSTR CListUI::GetSelectedItemImage() const
{
    return m_ListInfo.diSelected.sDrawString;
}

void CListUI::SetHotItemTextColor(DWORD dwTextColor)
{
    m_ListInfo.dwHotTextColor = dwTextColor;
    Invalidate();
}

void CListUI::SetHotItemBkColor(DWORD dwBkColor)
{
    m_ListInfo.dwHotBkColor = dwBkColor;
    Invalidate();
}

void CListUI::SetHotItemImage(LPCTSTR pStrImage)
{
    if (m_ListInfo.diHot.sDrawString == pStrImage && m_ListInfo.diHot.pImageInfo != NULL) { return; }

    m_ListInfo.diHot.Clear();
    m_ListInfo.diHot.sDrawString = pStrImage;
    Invalidate();
}

DWORD CListUI::GetHotItemTextColor() const
{
    return m_ListInfo.dwHotTextColor;
}
DWORD CListUI::GetHotItemBkColor() const
{
    return m_ListInfo.dwHotBkColor;
}

LPCTSTR CListUI::GetHotItemImage() const
{
    return m_ListInfo.diHot.sDrawString;
}

void CListUI::SetDisabledItemTextColor(DWORD dwTextColor)
{
    m_ListInfo.dwDisabledTextColor = dwTextColor;
    Invalidate();
}

void CListUI::SetDisabledItemBkColor(DWORD dwBkColor)
{
    m_ListInfo.dwDisabledBkColor = dwBkColor;
    Invalidate();
}

void CListUI::SetDisabledItemImage(LPCTSTR pStrImage)
{
    if (m_ListInfo.diDisabled.sDrawString == pStrImage && m_ListInfo.diDisabled.pImageInfo != NULL) { return; }

    m_ListInfo.diDisabled.Clear();
    m_ListInfo.diDisabled.sDrawString = pStrImage;
    Invalidate();
}

DWORD CListUI::GetDisabledItemTextColor() const
{
    return m_ListInfo.dwDisabledTextColor;
}

DWORD CListUI::GetDisabledItemBkColor() const
{
    return m_ListInfo.dwDisabledBkColor;
}

LPCTSTR CListUI::GetDisabledItemImage() const
{
    return m_ListInfo.diDisabled.sDrawString;
}

int CListUI::GetItemHLineSize() const
{
    return m_ListInfo.iHLineSize;
}

void CListUI::SetItemHLineSize(int iSize)
{
    m_ListInfo.iHLineSize = iSize;
    Invalidate();
}

DWORD CListUI::GetItemHLineColor() const
{
    return m_ListInfo.dwHLineColor;
}

void CListUI::SetItemHLineColor(DWORD dwLineColor)
{
    m_ListInfo.dwHLineColor = dwLineColor;
    Invalidate();
}

int CListUI::GetItemVLineSize() const
{
    return m_ListInfo.iVLineSize;
}

void CListUI::SetItemVLineSize(int iSize)
{
    m_ListInfo.iVLineSize = iSize;
    Invalidate();
}

DWORD CListUI::GetItemVLineColor() const
{
    return m_ListInfo.dwVLineColor;
}

void CListUI::SetItemVLineColor(DWORD dwLineColor)
{
    m_ListInfo.dwVLineColor = dwLineColor;
    Invalidate();
}

bool CListUI::IsItemShowHtml()
{
    return m_ListInfo.bShowHtml;
}

void CListUI::SetItemShowHtml(bool bShowHtml)
{
    if (m_ListInfo.bShowHtml == bShowHtml) { return; }

    m_ListInfo.bShowHtml = bShowHtml;
    NeedUpdate();
}

void CListUI::SetMultiExpanding(bool bMultiExpandable)
{
    m_ListInfo.bMultiExpandable = bMultiExpandable;
}

bool CListUI::ExpandItem(int iIndex, bool bExpand /*= true*/)
{
    if (m_iExpandedItem >= 0 && !m_ListInfo.bMultiExpandable)
    {
        CControlUI *pControl = GetItemAt(m_iExpandedItem);

        if (pControl != NULL)
        {
            IListItemUI *pItem = static_cast<IListItemUI *>(pControl->GetInterface(DUI_CTR_ILISTITEM));

            if (pItem != NULL) { pItem->Expand(false); }
        }

        m_iExpandedItem = -1;
    }

    if (bExpand)
    {
        CControlUI *pControl = GetItemAt(iIndex);

        if (pControl == NULL) { return false; }

        if (!pControl->IsVisible()) { return false; }

        IListItemUI *pItem = static_cast<IListItemUI *>(pControl->GetInterface(DUI_CTR_ILISTITEM));

        if (pItem == NULL) { return false; }

        m_iExpandedItem = iIndex;

        if (!pItem->Expand(true))
        {
            m_iExpandedItem = -1;
            return false;
        }
    }

    NeedUpdate();
    return true;
}

int CListUI::GetExpandedItem() const
{
    return m_iExpandedItem;
}

void CListUI::EnsureVisible(int iIndex)
{
    if (m_iCurSel < 0) { return; }

    RECT rcItem = m_pList->GetItemAt(iIndex)->GetPos();
    RECT rcList = m_pList->GetPos();
    RECT rcListInset = m_pList->GetPadding();

    rcList.left += rcListInset.left;
    rcList.top += rcListInset.top;
    rcList.right -= rcListInset.right;
    rcList.bottom -= rcListInset.bottom;

    CScrollBarUI *pHorizontalScrollBar = m_pList->GetHorizontalScrollBar();

    if (pHorizontalScrollBar && pHorizontalScrollBar->IsVisible()) { rcList.bottom -= pHorizontalScrollBar->GetFixedHeight(); }

    int iPos = m_pList->GetScrollPos().cy;

    if (rcItem.top >= rcList.top && rcItem.bottom < rcList.bottom) { return; }

    int dx = 0;

    if (rcItem.top < rcList.top) { dx = rcItem.top - rcList.top; }

    if (rcItem.bottom > rcList.bottom) { dx = rcItem.bottom - rcList.bottom; }

    Scroll(0, dx);
}

void CListUI::Scroll(int dx, int dy)
{
    if (dx == 0 && dy == 0) { return; }

    SIZE sz = m_pList->GetScrollPos();
    m_pList->SetScrollPos(CDuiSize(sz.cx + dx, sz.cy + dy));
}

void CListUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if (_tcscmp(pstrName, _T("header")) == 0) { GetHeader()->SetVisible(ParseBool(pstrValue)); }
    else if (_tcscmp(pstrName, _T("headerbkimage")) == 0) { GetHeader()->SetBkImage(ParseString(pstrValue)); }
    else if (_tcscmp(pstrName, _T("scrollselect")) == 0) { SetScrollSelect(ParseBool(pstrValue)); }
    else if (_tcscmp(pstrName, _T("multiexpanding")) == 0) { SetMultiExpanding(ParseBool(pstrValue)); }
    else if (_tcscmp(pstrName, _T("itemheight")) == 0) { m_ListInfo.uFixedHeight = ParseDWord(pstrValue); }
    else if (_tcscmp(pstrName, _T("itemfont")) == 0) { m_ListInfo.nFont = ParseInt(pstrValue); }
    else if (_tcscmp(pstrName, _T("itemalign")) == 0)
    {
        CDuiString str = ParseString(pstrValue);

        if (str == _T("left"))
        {
            m_ListInfo.uTextStyle &= ~(DT_CENTER | DT_RIGHT);
            m_ListInfo.uTextStyle |= DT_LEFT;
        }
        else if (str == _T("center"))
        {
            m_ListInfo.uTextStyle &= ~(DT_LEFT | DT_RIGHT);
            m_ListInfo.uTextStyle |= DT_CENTER;
        }
        else if (str == _T("right"))
        {
            m_ListInfo.uTextStyle &= ~(DT_LEFT | DT_CENTER);
            m_ListInfo.uTextStyle |= DT_RIGHT;
        }
    }
    else if (_tcscmp(pstrName, _T("itemvalign")) == 0)
    {
        CDuiString str = ParseString(pstrValue);

        if (str == _T("top"))
        {
            m_ListInfo.uTextStyle &= ~(DT_BOTTOM | DT_VCENTER);
            m_ListInfo.uTextStyle |= DT_TOP;
        }
        else if (str == _T("center"))
        {
            m_ListInfo.uTextStyle &= ~(DT_TOP | DT_BOTTOM);
            m_ListInfo.uTextStyle |= DT_VCENTER;
        }
        else if (str == _T("bottom"))
        {
            m_ListInfo.uTextStyle &= ~(DT_TOP | DT_VCENTER);
            m_ListInfo.uTextStyle |= DT_BOTTOM;
        }
    }
    else if (_tcscmp(pstrName, _T("itemendellipsis")) == 0)
    {
        if (ParseBool(pstrValue)) { m_ListInfo.uTextStyle |= DT_END_ELLIPSIS; }
        else { m_ListInfo.uTextStyle &= ~DT_END_ELLIPSIS; }
    }
    else if (_tcscmp(pstrName, _T("itemmultiline")) == 0)
    {
        if (ParseBool(pstrValue))
        {
            m_ListInfo.uTextStyle &= ~DT_SINGLELINE;
            m_ListInfo.uTextStyle |= DT_WORDBREAK;
        }
        else { m_ListInfo.uTextStyle |= DT_SINGLELINE; }
    }
    else if (_tcscmp(pstrName, _T("itemtextpadding")) == 0 || _tcscmp(pstrName, _T("itempadding")) == 0)
    {
        RECT rt = ParseRect(pstrValue);
        SetItemPadding(rt);
    }
    else if (_tcscmp(pstrName, _T("itemtextcolor")) == 0)
    {
        DWORD clr = ParseColor(pstrValue);
        SetItemTextColor(clr);
    }
    else if (_tcscmp(pstrName, _T("itembkcolor")) == 0)
    {
        DWORD clr = ParseColor(pstrValue);
        SetItemBkColor(clr);
    }
    else if (_tcscmp(pstrName, _T("itembkimage")) == 0) { SetItemBkImage(ParseString(pstrValue)); }
    else if (_tcscmp(pstrName, _T("itemaltbk")) == 0) { SetAlternateBk(ParseBool(pstrValue)); }
    else if (_tcscmp(pstrName, _T("itemselectedtextcolor")) == 0)
    {
        DWORD clr = ParseColor(pstrValue);
        SetSelectedItemTextColor(clr);
    }
    else if (_tcscmp(pstrName, _T("itemselectedbkcolor")) == 0)
    {
        DWORD clr = ParseColor(pstrValue);
        SetSelectedItemBkColor(clr);
    }
    else if (_tcscmp(pstrName, _T("itemselectedimage")) == 0) { SetSelectedItemImage(ParseString(pstrValue)); }
    else if (_tcscmp(pstrName, _T("itemhottextcolor")) == 0)
    {
        DWORD clr = ParseColor(pstrValue);
        SetHotItemTextColor(clr);
    }
    else if (_tcscmp(pstrName, _T("itemhotbkcolor")) == 0)
    {
        DWORD clr = ParseColor(pstrValue);
        SetHotItemBkColor(clr);
    }
    else if (_tcscmp(pstrName, _T("itemhotimage")) == 0) { SetHotItemImage(ParseString(pstrValue)); }
    else if (_tcscmp(pstrName, _T("itemdisabledtextcolor")) == 0)
    {
        DWORD clr = ParseColor(pstrValue);
        SetDisabledItemTextColor(clr);
    }
    else if (_tcscmp(pstrName, _T("itemdisabledbkcolor")) == 0)
    {
        DWORD clr = ParseColor(pstrValue);
        SetDisabledItemBkColor(clr);
    }
    else if (_tcscmp(pstrName, _T("itemdisabledimage")) == 0) { SetDisabledItemImage(ParseString(pstrValue)); }
    else if (_tcscmp(pstrName, _T("itemvlinesize")) == 0)
    {
        SetItemVLineSize(ParseInt(pstrValue));
    }
    else if (_tcscmp(pstrName, _T("itemvlinecolor")) == 0)
    {
        DWORD clr = ParseColor(pstrValue);
        SetItemVLineColor(clr);
    }
    else if (_tcscmp(pstrName, _T("itemhlinesize")) == 0)
    {
        SetItemHLineSize(ParseInt(pstrValue));
    }
    else if (_tcscmp(pstrName, _T("itemhlinecolor")) == 0)
    {
        DWORD clr = ParseColor(pstrValue);
        SetItemHLineColor(clr);
    }
    else if (_tcscmp(pstrName, _T("itemshowhtml")) == 0) { SetItemShowHtml(ParseBool(pstrValue)); }
	//2021-10-17 zm 由表头来决定是否使用checkble
    // ischeckbox 属性废弃，改用 checkable
    //else if (_tcscmp(pstrName, _T("ischeckbox")) == 0 || _tcscmp(pstrName, _T("checkable")) == 0)
    //{
    //    m_ListInfo.bCheckBox = ParseBool(pstrValue);
    //}
	//2021-10-17 zm 
    else if (_tcscmp(pstrName, _T("checkunselimage")) == 0) { m_diUnSel.sDrawString = ParseString(pstrValue); }
    else if (_tcscmp(pstrName, _T("checkselimage")) == 0) { m_diSel.sDrawString = ParseString(pstrValue); }
	else if (_tcscmp(pstrName, _T("checkwidth")) == 0) { m_szCheckBox.cx = ParseInt(pstrValue); }
	else if (_tcscmp(pstrName, _T("checkheight")) == 0) { m_szCheckBox.cy = ParseInt(pstrValue); }
	else if (_tcscmp(pstrName, _T("multiselect")) == 0) { SetMultiSelect(ParseBool(pstrValue)); }
    else { CVerticalLayoutUI::SetAttribute(pstrName, pstrValue); }
}

IListCallbackUI *CListUI::GetTextCallback() const
{
    return m_pCallback;
}

void CListUI::SetTextCallback(IListCallbackUI *pCallback)
{
    m_pCallback = pCallback;
}

SIZE CListUI::GetScrollPos() const
{
    return m_pList->GetScrollPos();
}

SIZE CListUI::GetScrollRange() const
{
    return m_pList->GetScrollRange();
}

void CListUI::SetScrollPos(SIZE szPos)
{
    m_pList->SetScrollPos(szPos);
}

void CListUI::LineUp()
{
    m_pList->LineUp();
}

void CListUI::LineDown()
{
    m_pList->LineDown();
}

void CListUI::PageUp()
{
    m_pList->PageUp();
}

void CListUI::PageDown()
{
    m_pList->PageDown();
}

void CListUI::HomeUp()
{
    m_pList->HomeUp();
}

void CListUI::EndDown()
{
    m_pList->EndDown();
}

void CListUI::LineLeft()
{
    m_pList->LineLeft();
}

void CListUI::LineRight()
{
    m_pList->LineRight();
}

void CListUI::PageLeft()
{
    m_pList->PageLeft();
}

void CListUI::PageRight()
{
    m_pList->PageRight();
}

void CListUI::HomeLeft()
{
    m_pList->HomeLeft();
}

void CListUI::EndRight()
{
    m_pList->EndRight();
}

void CListUI::EnableScrollBar(bool bEnableVertical, bool bEnableHorizontal)
{
    m_pList->EnableScrollBar(bEnableVertical, bEnableHorizontal);
}

CScrollBarUI *CListUI::GetVerticalScrollBar() const
{
    return m_pList->GetVerticalScrollBar();
}

CScrollBarUI *CListUI::GetHorizontalScrollBar() const
{
    return m_pList->GetHorizontalScrollBar();
}

bool CListUI::SortItems(PULVCompareFunc pfnCompare, UINT_PTR dwData)
{
    if (!m_pList) { return false; }

    int iCurSel = m_iCurSel;
    bool bResult = m_pList->SortItems(pfnCompare, dwData, iCurSel);

    if (bResult)
    {
        //m_iCurSel = iCurSel;
		m_iCurSel = -1;//2021-10-02 zm 
        EnsureVisible(m_iCurSel);
        NeedUpdate();
    }

    return bResult;
}

TDrawInfo &CListUI::GetUnSelImage(void)
{
    return m_diUnSel;
}

TDrawInfo &CListUI::GetSelImage(void)
{
    return m_diSel;
}

int CListUI::GetCheckBoxWidth() const
{
	return m_szCheckBox.cx;
}

int CListUI::GetCheckBoxHeight()  const
{
	return m_szCheckBox.cy;
}

RECT CListUI::GetCheckBoxRect(RECT rc)
{
	RECT rcItem = rc;
	rcItem.left += 6;
	rcItem.top += (rc.bottom - rc.top - GetCheckBoxHeight()) / 2;
	rcItem.right = rcItem.left + GetCheckBoxWidth();
	rcItem.bottom = rcItem.top + GetCheckBoxHeight();

	return rcItem;
}

void CListUI::GetAllSelectedItem(CDuiValArray &arySelIdx, int nColumn)
{
    arySelIdx.Empty();

    for (int i = 0; i < GetCount(); i++)
    {
        CListTextElementUI *pItem = dynamic_cast<CListTextElementUI *>(GetItemAt(i));
        CListContainerElementUI *pItem2 = dynamic_cast<CListContainerElementUI *>(GetItemAt(i));

        if (NULL != pItem && pItem->GetCheckBoxState(nColumn)) { arySelIdx.Add(&i); }

        if (NULL != pItem2 && pItem2->GetCheckBoxState(nColumn)) { arySelIdx.Add(&i); }
    }
}

void CListUI::SetAllItemSelected(bool bSelect, int nColumn)
{
    if (nColumn >= m_pHeader->GetCount())
    {
        DUITRACE(_T("nColumn=%d, 超出了列范围！"), nColumn);
        return;
    }

    // 2021-10-17 zm 设置表头中的复选框状态
    {
        //CListHeaderItemUI *pHItem = dynamic_cast<CListHeaderItemUI *>(m_pHeader->GetItemAt(nColumn));
        //ASSERT(NULL != pHItem);
        //CCheckBoxUI *pCtrl = dynamic_cast<CCheckBoxUI *>(m_pManager->FindSubControlByClass(pHItem, DUI_CTR_CHECKBOX));
        //ASSERT(NULL != pCtrl);
        //pCtrl ? pCtrl->SetCheck(bSelect) : NULL; //lint !e62
    }

    for (int i = 0; i < GetCount(); ++i)
    {
        if (CListElementUI *pCtrl = dynamic_cast<CListElementUI *>(GetItemAt(i)))
        {
            pCtrl->SetCheckBoxState(bSelect, nColumn);
        }
        else if (CListContainerElementUI *pCtrl = dynamic_cast<CListContainerElementUI *>(GetItemAt(i)))
        {
            pCtrl->SetCheckBoxState(bSelect, nColumn);
        }
    }
}

void CListUI::DoInit()
{
    CVerticalLayoutUI::DoInit();

    // 2018-09-17 zhuyadong 默认字体颜色
    if (m_ListInfo.dwTextColor == 0)
    {
        m_ListInfo.dwTextColor = m_pManager->GetDefaultFontColor();
    }

    if (m_ListInfo.dwDisabledTextColor == 0)
    {
        m_ListInfo.dwDisabledTextColor = m_pManager->GetDefaultDisabledColor();
    }

    if (NULL == m_pHeader) { return; }

    // 绑定表头每一列的CheckBox通知消息
    for (int j = 0; j < m_pHeader->GetCount(); ++j)
    {
        CListHeaderItemUI *pHItem = (CListHeaderItemUI *)m_pHeader->GetItemAt(j);

        if (NULL == pHItem || !pHItem->IsCheckable()) { continue; }

		//2021-10-17 zm 简化checkbox的xml设置
		pHItem->SetOwner(this);
		m_ListInfo.bCheckBox = true;
		pHItem->OnNotify += MakeDelegate(this, &CListUI::OnHeaderCheckBoxNotify);

        //CControlUI *pCtrl = m_pManager->FindSubControlByClass(pHItem, DUI_CTR_CHECKBOX);
        //pCtrl ? (pCtrl->OnNotify += MakeDelegate(this, &CListUI::OnHeaderCheckBoxNotify)) : NULL; //lint !e62
    }
}

void CListUI::GetLastModifiedItem(int &nRow, int &nColumn)
{
    nRow = m_nRow;
    nColumn = m_nColumn;
}

void CListUI::ShowEdit(int nRow, int nColumn, RECT &rt, CDuiString &sItemTxt)
{
    if (!GetEditUI()) { return; }

    m_nRow = nRow;
    m_nColumn = nColumn;

    m_pEdit->SetVisible(true);
    //移动位置
    m_pEdit->SetPos(rt);

    //设置文字
    if (m_pCallback)
    {
        CDuiString str = m_pCallback->GetItemText(this, nRow, nColumn);
        m_pEdit->SetText(str);
    }
    else
    {
        m_pEdit->SetText(sItemTxt);
    }

    m_pEdit->SetFocus();
}

void CListUI::HideEdit()
{
    if (NULL != m_pEdit)
    {
        RECT rc = { 0, 0, 0, 0 };
        m_pEdit->SetPos(rc);
        m_pEdit->SetVisible(false);
    }
}

CEditUI *CListUI::GetEditUI()
{
    if (NULL == m_pEdit)
    {
        m_pEdit = new CEditUI;
        ASSERT(m_pEdit);

        if (NULL == m_pEdit)
        {
            DUITRACE(_T("GetEditUI failed."));
            return NULL;
        }

        CVerticalLayoutUI::Add(m_pEdit);    // duilib 会自动销毁编辑框
        m_pEdit->OnNotify += MakeDelegate(this, &CListUI::OnEditNotify);
        // 列表框内容滚动时，隐藏编辑框
        CScrollBarUI *pScrollBar = m_pList->GetVerticalScrollBar();
        pScrollBar ? pScrollBar->OnNotify += MakeDelegate(this, &CListUI::OnScrollNotify) : NULL; //lint !e62
        pScrollBar = m_pList->GetHorizontalScrollBar();
        pScrollBar ? pScrollBar->OnNotify += MakeDelegate(this, &CListUI::OnScrollNotify) : NULL; //lint !e62

        m_pEdit->SetName(_T("_edt_list"));
        m_pEdit->SetBkColor(0xFFFFFFFF);
        m_pEdit->SetPadding(CDuiRect(2, 2, 2, 2));

        LPCTSTR pDefAttr = GetManager()->GetDefaultAttributeList(_T("Edit"), true);
        pDefAttr ? m_pEdit->SetAttributeList(pDefAttr) : pDefAttr; //lint !e62
        pDefAttr = GetManager()->GetDefaultAttributeList(_T("Edit"), false);
        pDefAttr ? m_pEdit->SetAttributeList(pDefAttr) : pDefAttr; //lint !e62
        m_pEdit->SetFloat(true);
        m_pEdit->SetAttribute(_T("autohscroll"), _T("true"));
    }

    return m_pEdit;
}

IListCmbCallbackUI *CListUI::GetCmbItemCallback() const
{
    return m_pCmbCallback;
}

void CListUI::SetCmbItemCallback(IListCmbCallbackUI *pCallback)
{
    m_pCmbCallback = pCallback;
}

void CListUI::ShowCombo(int nRow, int nColumn, RECT &rt, CDuiString &sItemTxt)
{
    if (!GetComboUI()) { return; }

    m_nRow = nRow;
    m_nColumn = nColumn;

	//移除所有选项
    m_pCombo->RemoveAll();

	//移动位置
	m_pCombo->SetPos(rt);
	m_pCombo->SetVisible(true);
	m_pCombo->SetFocus();

    // 设置下拉框可选项
    if (m_pCmbCallback)
    {
        m_pCmbCallback->GetComboItems(m_pCombo, nRow, nColumn);
    }

	m_pCombo->SelectItem(GetComboIndex(sItemTxt));//2021-10-16 zm更符合显示逻辑
}

void CListUI::HideCombo()
{
    if (NULL != m_pCombo)
    {
        RECT rc = { 0, 0, 0, 0 };
        m_pCombo->SetPos(rc);
        m_pCombo->SetVisible(false);
    }
}

CComboUI *CListUI::GetComboUI()
{
    if (NULL == m_pCombo)
    {
        m_pCombo = new CComboUI;
        ASSERT(m_pCombo);

        if (NULL == m_pCombo)
        {
            DUITRACE(_T("GetEditUI failed."));
            return NULL;
        }

        CVerticalLayoutUI::Add(m_pCombo);    // duilib 会自动销毁下拉框
        m_pCombo->OnNotify += MakeDelegate(this, &CListUI::OnComboNotify);
        // 列表框内容滚动时，隐藏下拉框
        CScrollBarUI *pScrollBar = m_pList->GetVerticalScrollBar();
        pScrollBar ? pScrollBar->OnNotify += MakeDelegate(this, &CListUI::OnScrollNotify) : NULL; //lint !e62
        pScrollBar = m_pList->GetHorizontalScrollBar();
        pScrollBar ? pScrollBar->OnNotify += MakeDelegate(this, &CListUI::OnScrollNotify) : NULL; //lint !e62

        m_pCombo->SetName(_T("_cmb_list"));
        m_pCombo->SetBkColor(0xFFFFFFFF);
        m_pCombo->SetPadding(CDuiRect(2, 2, 2, 2));

        LPCTSTR pDefAttr = GetManager()->GetDefaultAttributeList(_T("Combo"), true);
        pDefAttr ? m_pCombo->SetAttributeList(pDefAttr) : pDefAttr; //lint !e62
        pDefAttr = GetManager()->GetDefaultAttributeList(_T("Combo"), false);
        pDefAttr ? m_pCombo->SetAttributeList(pDefAttr) : pDefAttr; //lint !e62
        m_pCombo->SetFloat(true);
    }

    return m_pCombo;
}

int CListUI::GetMouseColumn(POINT pt)
{
    if (NULL == m_pHeader) { return -1; }

    for (int i = 0; i < m_pHeader->GetCount(); ++i)
    {
        CControlUI *pHI = m_pHeader->GetItemAt(i);
        const RECT &rt = pHI->GetPos();

        if (::PtInRect(&rt, pt)) { return i; }
    }

    return -1;
}

bool CListUI::OnHeaderCheckBoxNotify(void *pParam)
{
    if (NULL == pParam) { return false; }

    TNotifyUI *pMsg = (TNotifyUI *)pParam;

	if (pMsg->sType != DUI_MSGTYPE_HEADERCLICK) { return false; }

    //if (pMsg->sType != DUI_MSGTYPE_SELECTCHANGED) { return false; }

    //// 列表支持复选框时，用户单击表头的复选框，则自动 选中/取消 列表中的所有项
    int nColumn = GetMouseColumn(pMsg->ptMouse);
	SetAllItemSelected(((CListHeaderItemUI*)pMsg->pSender)->GetCheckState(), nColumn);
    //SetAllItemSelected(((CCheckBoxUI *)pMsg->pSender)->IsSelected(), nColumn);
    return true;
}

bool CListUI::OnEditNotify(void *pParam)
{
    if (NULL == pParam) { return false; }

    TNotifyUI *pMsg = (TNotifyUI *)pParam;

	if (DUI_MSGTYPE_RETURN == pMsg->sType || DUI_MSGTYPE_TEXTCHANGED == pMsg->sType)
    {
		//2021-10-02 zm是否存在m_pCallback不应该影响编辑框的输入,DUI_MSGTYPE_TEXTCHANGED编辑框的隐藏交给list消息处理
        //if (!m_pCallback)
        //{
			CListTextElementUI *pItem = dynamic_cast<CListTextElementUI *>(GetItemAt(m_nRow));
			if (NULL != pItem) { pItem->SetText(m_nColumn, pMsg->pSender->GetText()); }
        //}

			if (DUI_MSGTYPE_RETURN == pMsg->sType) HideEdit();
    }

    return true;
}

bool CListUI::OnComboNotify(void *pParam)
{
    if (NULL == pParam) { return false; }

    TNotifyUI *pMsg = (TNotifyUI *)pParam;

    // Combo 下拉框收起时更新列表框
    if (DUI_MSGTYPE_DROPUP == pMsg->sType)
    {
		//2021-10-02 zm是否存在m_pCallback不应该影响下拉框的选择
        //if (!m_pCallback && m_pCombo)
        //{
			if (NULL == m_pCombo) return false;
            CListTextElementUI *pItem = dynamic_cast<CListTextElementUI *>(GetItemAt(m_nRow));
            CControlUI *pSel = m_pCombo->GetItemAt(m_pCombo->GetCurSel());

            if (NULL != pItem && NULL != pSel) { pItem->SetText(m_nColumn, pSel->GetText()); }

           // HideCombo();
        //}
    }

    return true;
}

bool CListUI::OnScrollNotify(void *pParam)
{
    if (NULL == pParam) { return false; }

    TNotifyUI *pMsg = (TNotifyUI *)pParam;

    if (DUI_MSGTYPE_SCROLL == pMsg->sType) { HideEdit(); HideCombo(); }

    return true;
}

/////////////////////////////////////////////////////////////////////////////////////
//
//

CListBodyUI::CListBodyUI(CListUI *pOwner) : m_pOwner(pOwner), m_pCompareFunc(NULL), m_compareData(0), m_stMousePoint({ 0 })
{
    ASSERT(m_pOwner);
}

bool CListBodyUI::SortItems(PULVCompareFunc pfnCompare, UINT_PTR dwData, int &iCurSel)
{
    if (!pfnCompare) { return false; }

    m_pCompareFunc = pfnCompare;
    m_compareData = dwData;
    CControlUI *pCurSelControl = GetItemAt(iCurSel);
    CControlUI **pData = (CControlUI **)m_items.GetData();
    qsort_s(m_items.GetData(), m_items.GetSize(), sizeof(CControlUI *), CListBodyUI::ItemComareFunc, this);

    if (pCurSelControl) { iCurSel = GetItemIndex(pCurSelControl); }

    IListItemUI *pItem = NULL;

    for (int i = 0; i < m_items.GetSize(); ++i)
    {
		//2021-10-04 zm 信息不同步bug
        //pItem = (IListItemUI *)(static_cast<CControlUI *>(m_items[i])->GetInterface(TEXT("ListItem")));
		pItem = (IListItemUI *)(static_cast<CControlUI *>(m_items[i])->GetInterface(DUI_CTR_ILISTITEM));

        if (pItem)
        {
            pItem->SetIndex(i);
			pItem->Select(false);//2021-10-02 zm 取消所有选择
        }
    }

    return true;
}

POINT CListBodyUI::GetSatrtPoint()
{
	return m_stMousePoint;
}

void CListBodyUI::SetStartPoint(POINT stPoint)
{
	m_stMousePoint = stPoint;
}

void CListBodyUI::DoPostPaint(HDC hDC, const RECT &rcPaint)
{
	POINT ptSatrtPoint = GetSatrtPoint();
	POINT ptMovePoint = m_pManager->GetMousePos();

	if (!::PtInRect(&m_rcItem, ptSatrtPoint)) { return; }

	CDuiRect duircItem = { ptSatrtPoint.x, ptSatrtPoint.y, ptSatrtPoint.x + ptMovePoint.x - ptSatrtPoint.x, ptSatrtPoint.y + ptMovePoint.y - ptSatrtPoint.y };
	duircItem.Normalize();//重新调整坐标位置

	//绘制区域的大小
	RECT rcItem = duircItem;
	if (rcItem.left < m_rcItem.left)  { rcItem.left = m_rcItem.left; }

	if (GetVerticalScrollBar() && (rcItem.right > m_rcItem.right - GetVerticalScrollBar()->GetFixedWidth()))
	{
		rcItem.right = m_rcItem.right - GetVerticalScrollBar()->GetFixedWidth();
	}
	else if (!GetVerticalScrollBar() && (rcItem.right > m_rcItem.right))
	{
		rcItem.right = m_rcItem.right;
	}

	if (rcItem.top < m_rcItem.top) { rcItem.top = m_rcItem.top; }

	if (GetHorizontalScrollBar() && (rcItem.bottom > m_rcItem.bottom - GetHorizontalScrollBar()->GetFixedHeight()))
	{
		rcItem.bottom = m_rcItem.bottom - GetHorizontalScrollBar()->GetFixedHeight();
	}
	else if (!GetHorizontalScrollBar() && (rcItem.bottom > m_rcItem.bottom))
	{
		rcItem.bottom = m_rcItem.bottom;
	}

	RECT rcScale9 = { 0 };
	RECT rcBmpPart = { 0, 0, rcPaint.right - rcPaint.left, rcPaint.bottom - rcPaint.top };

	//生成的源图片大小
	HBITMAP hBitmap = CRenderEngine::GenerateBitmap(m_pManager, this, rcPaint, 0xffFFFAFA);//生成颜色位图

	CRenderEngine::DrawImage(hDC, hBitmap, rcItem, rcItem, rcBmpPart, rcScale9, true, 0x64);//透明绘制
}

int __cdecl CListBodyUI::ItemComareFunc(void *pvlocale, const void *item1, const void *item2)
{
    CListBodyUI *pThis = (CListBodyUI *)pvlocale;

    if (!pThis || !item1 || !item2) { return 0; }

    return pThis->ItemComareFunc(item1, item2);
}

int __cdecl CListBodyUI::ItemComareFunc(const void *item1, const void *item2)
{
    CControlUI *pControl1 = *(CControlUI **)item1;
    CControlUI *pControl2 = *(CControlUI **)item2;
    return m_pCompareFunc((UINT_PTR)pControl1, (UINT_PTR)pControl2, m_compareData);
}

void CListBodyUI::SetScrollPos(SIZE szPos)
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

    if (cx != 0 && m_pOwner)
    {
        CListHeaderUI *pHeader = m_pOwner->GetHeader();

        if (pHeader == NULL) { return; }

        TListInfoUI *pInfo = m_pOwner->GetListInfo();
        pInfo->nColumns = std::min<int>(pHeader->GetCount(), UILIST_MAX_COLUMNS);

        for (int i = 0; i < pInfo->nColumns; i++)
        {
            CControlUI *pControl = static_cast<CControlUI *>(pHeader->GetItemAt(i));

            if (!pControl->IsVisible()) { continue; }

            if (pControl->IsFloat()) { continue; }

            pControl->Move(CDuiSize(-cx, -cy), false);
            pInfo->rcColumn[i] = pControl->GetPos();
        }

        pHeader->Invalidate();
    }
}

void CListBodyUI::SetPos(RECT rc, bool bNeedInvalidate)
{
    CControlUI::SetPos(rc, bNeedInvalidate);
    rc = m_rcItem;

    // Adjust for inset
    rc.left += (m_rcBorderSize.left + m_rcPadding.left);
    rc.top += (m_rcBorderSize.top + m_rcPadding.top);
    rc.right -= (m_rcBorderSize.right + m_rcPadding.right);
    rc.bottom -= (m_rcBorderSize.bottom + m_rcPadding.bottom);

    if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible()) { rc.right -= m_pVerticalScrollBar->GetFixedWidth(); }

    if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible()) { rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight(); }

    // Determine the minimum size
    SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };

    if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible())
    { szAvailable.cx += m_pHorizontalScrollBar->GetScrollRange(); }

    int iChildMargin = m_iChildMargin;
    TListInfoUI *pInfo = NULL;

    if (m_pOwner)
    {
        pInfo = m_pOwner->GetListInfo();

        if (pInfo != NULL)
        {
            iChildMargin += pInfo->iHLineSize;

            if (pInfo->nColumns > 0)
            {
                szAvailable.cx = pInfo->rcColumn[pInfo->nColumns - 1].right - pInfo->rcColumn[0].left;
            }
        }
    }

    int cxNeeded = 0;
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

        SIZE sz = pControl->EstimateSize(szAvailable);

        if (sz.cy < pControl->GetMinHeight()) { sz.cy = pControl->GetMinHeight(); }

        if (sz.cy > pControl->GetMaxHeight()) { sz.cy = pControl->GetMaxHeight(); }

        cyFixed += sz.cy + pControl->GetMargin().top + pControl->GetMargin().bottom;

        sz.cx = std::max<int>(sz.cx, 0);

        if (sz.cx < pControl->GetMinWidth()) { sz.cx = pControl->GetMinWidth(); }

        if (sz.cx > pControl->GetMaxWidth()) { sz.cx = pControl->GetMaxWidth(); }

        cxNeeded = std::max<int>(cxNeeded, sz.cx);
        nEstimateNum++;
    }

    cyFixed += (nEstimateNum - 1) * iChildMargin;

    if (m_pOwner)
    {
        CListHeaderUI *pHeader = m_pOwner->GetHeader();

        if (pHeader != NULL && pHeader->GetCount() > 0)
        {
            CDuiSize size(rc.right - rc.left, rc.bottom - rc.top);
            cxNeeded = std::max<int>(0, pHeader->EstimateSize(size).cx);
        }
    }

    // Place elements
    int cyNeeded = 0;
    // Position the elements
    SIZE szRemaining = szAvailable;
    int iPosY = rc.top;

    if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible())
    {
        iPosY -= m_pVerticalScrollBar->GetScrollPos();
    }

    int iPosX = rc.left;

    if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible())
    {
        iPosX -= m_pHorizontalScrollBar->GetScrollPos();
    }

    int iAdjustable = 0;

    for (int it2 = 0; it2 < m_items.GetSize(); it2++)
    {
        CControlUI *pControl = static_cast<CControlUI *>(m_items[it2]);

        if (!pControl->IsVisible()) { continue; }

        if (pControl->IsFloat())
        {
            SetFloatPos(it2);
            continue;
        }

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

        SIZE sz = pControl->EstimateSize(szControlAvailable);

        if (sz.cy < pControl->GetMinHeight()) { sz.cy = pControl->GetMinHeight(); }

        if (sz.cy > pControl->GetMaxHeight()) { sz.cy = pControl->GetMaxHeight(); }

        sz.cx = pControl->GetMaxWidth();

        if (sz.cx == 0) { sz.cx = szAvailable.cx - rcMargin.left - rcMargin.right; }

        if (sz.cx < 0) { sz.cx = 0; }

        if (sz.cx > szControlAvailable.cx) { sz.cx = szControlAvailable.cx; }

        if (sz.cx < pControl->GetMinWidth()) { sz.cx = pControl->GetMinWidth(); }

        RECT rcCtrl = { iPosX + rcMargin.left, iPosY + rcMargin.top, iPosX + rcMargin.left + sz.cx, iPosY + sz.cy + rcMargin.top + rcMargin.bottom };
        pControl->SetPos(rcCtrl, false);

        iPosY += sz.cy + iChildMargin + rcMargin.top + rcMargin.bottom;
        cyNeeded += sz.cy + rcMargin.top + rcMargin.bottom;
        szRemaining.cy -= sz.cy + iChildMargin + rcMargin.bottom;
    }

    cyNeeded += (nEstimateNum - 1) * iChildMargin;

    // Process the scrollbar
    ProcessScrollBar(rc, cxNeeded, cyNeeded);
}

void CListBodyUI::DoEvent(TEventUI &event)
{
    if (!IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND)
    {
        if (m_pOwner != NULL) { m_pOwner->DoEvent(event); }
        else { CControlUI::DoEvent(event); }

        return;
    }

    if (m_pOwner != NULL)
    {
        if (event.Type == UIEVENT_SCROLLWHEEL)
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
                        m_pOwner->LineLeft();
                        return;

                    case SB_LINEDOWN:
                        m_pOwner->LineRight();
                        return;
                    }
                }
            }
        }

        m_pOwner->DoEvent(event);
    }
    else
    {
        CControlUI::DoEvent(event);
    }
}

bool CListBodyUI::DoPaint(HDC hDC, const RECT &rcPaint, CControlUI *pStopControl)
{
    RECT rcTemp = { 0 };

    if (!::IntersectRect(&rcTemp, &rcPaint, &m_rcItem)) { return true; }

    TListInfoUI *pListInfo = NULL;

    if (m_pOwner) { pListInfo = m_pOwner->GetListInfo(); }

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
            int iDrawIndex = 0;
            CRenderClip childClip;
            CRenderClip::GenerateClip(hDC, rcTemp, childClip);

            for (int it = 0; it < m_items.GetSize(); it++)
            {
                CControlUI *pControl = static_cast<CControlUI *>(m_items[it]);

                if (pControl == pStopControl) { return false; }

                if (!pControl->IsVisible()) { continue; }

                if (!pControl->IsFloat())
                {
                    IListItemUI *pListItem = static_cast<IListItemUI *>(pControl->GetInterface(DUI_CTR_ILISTITEM));

                    if (pListItem != NULL)
                    {
                        pListItem->SetDrawIndex(iDrawIndex);
                        iDrawIndex += 1;
                    }

                    if (pListInfo && pListInfo->iHLineSize > 0)
                    {
                        // 因为没有为最后一个预留分割条长度，如果list铺满，最后一条不会显示
                        RECT rcMargin = pControl->GetMargin();
                        const RECT &rcPos = pControl->GetPos();
                        RECT rcBottomLine = { rcPos.left, rcPos.bottom + rcMargin.bottom, rcPos.right, rcPos.bottom + rcMargin.bottom + pListInfo->iHLineSize };

                        if (::IntersectRect(&rcTemp, &rcPaint, &rcBottomLine))
                        {
                            rcBottomLine.top += pListInfo->iHLineSize / 2;
                            rcBottomLine.bottom = rcBottomLine.top;
                            CRenderEngine::DrawLine(hDC, rcBottomLine, pListInfo->iHLineSize, GetAdjustColor(pListInfo->dwHLineColor));
                        }
                    }
                }

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

/////////////////////////////////////////////////////////////////////////////////////
//
//

CListHeaderUI::CListHeaderUI()
{
}

LPCTSTR CListHeaderUI::GetClass() const
{
    return DUI_CTR_LISTHEADER;
}

LPVOID CListHeaderUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcscmp(pstrName, DUI_CTR_LISTHEADER) == 0) { return this; }

    return CHorizontalLayoutUI::GetInterface(pstrName);
}

SIZE CListHeaderUI::EstimateSize(SIZE szAvailable)
{
    SIZE cXY = {0, m_cxyFixed.cy};

    if (cXY.cy == 0 && m_pManager != NULL)
    {
        for (int it = 0; it < m_items.GetSize(); it++)
        {
            cXY.cy = std::max<int>(cXY.cy, static_cast<CControlUI *>(m_items[it])->EstimateSize(szAvailable).cy);
        }

        int nMin = m_pManager->GetDefaultFontInfo()->tm.tmHeight + 8;
        cXY.cy = std::max<int>(cXY.cy, nMin);
    }

    for (int it = 0; it < m_items.GetSize(); it++)
    {
        cXY.cx +=  static_cast<CControlUI *>(m_items[it])->EstimateSize(szAvailable).cx;
    }

    return cXY;
}

/////////////////////////////////////////////////////////////////////////////////////
//
//

CListHeaderItemUI::CListHeaderItemUI() : m_bDragable(true), m_uButtonState(0), m_iSepWidth(4),
    m_uTextStyle(DT_LEFT | DT_VCENTER | DT_SINGLELINE), m_dwSepColor(0),
	m_iFont(-1), m_bShowHtml(false), m_bEditable(false), m_bComboable(false), m_bCheckable(false), 
	m_bChecked(false), m_pOwner(NULL)//zm
{
    //设置内边距，防止遮挡拖放的间隔条。在资源文件中设置
    // m_rcPadding.left = m_rcPadding.right = 2;

    m_ptLastMouse.x = m_ptLastMouse.y = 0;
    SetMinWidth(16);
}

LPCTSTR CListHeaderItemUI::GetClass() const
{
    return DUI_CTR_LISTHEADERITEM;
}

LPVOID CListHeaderItemUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcscmp(pstrName, DUI_CTR_LISTHEADERITEM) == 0) { return this; }

    return CContainerUI::GetInterface(pstrName);
}

UINT CListHeaderItemUI::GetControlFlags() const
{
    if (IsEnabled() && m_iSepWidth != 0) { return UIFLAG_SETCURSOR; }
    else { return 0; }
}

void CListHeaderItemUI::SetEnabled(bool bEnable)
{
    CContainerUI::SetEnabled(bEnable);

    if (!IsEnabled()) { m_uButtonState = 0; }
}

bool CListHeaderItemUI::IsDragable() const
{
    return m_bDragable;
}

void CListHeaderItemUI::SetDragable(bool bDragable)
{
    m_bDragable = bDragable;

    if (!m_bDragable) { m_uButtonState &= ~UISTATE_CAPTURED; }
}

DWORD CListHeaderItemUI::GetSepWidth() const
{
    return m_iSepWidth;
}

void CListHeaderItemUI::SetSepWidth(int iWidth)
{
    m_iSepWidth = iWidth;
}

DWORD CListHeaderItemUI::GetTextStyle() const
{
    return m_uTextStyle;
}

void CListHeaderItemUI::SetTextStyle(UINT uStyle)
{
    m_uTextStyle = uStyle;
    Invalidate();
}

DWORD CListHeaderItemUI::GetSepColor() const
{
    return m_dwSepColor;
}

void CListHeaderItemUI::SetSepColor(DWORD dwSepColor)
{
    m_dwSepColor = dwSepColor;
    Invalidate();
}

void CListHeaderItemUI::SetPadding(RECT rc)
{
    m_rcPadding = rc;
    Invalidate();
}

void CListHeaderItemUI::SetFont(int index)
{
    m_iFont = index;
}

bool CListHeaderItemUI::IsShowHtml()
{
    return m_bShowHtml;
}

void CListHeaderItemUI::SetShowHtml(bool bShowHtml)
{
    if (m_bShowHtml == bShowHtml) { return; }

    m_bShowHtml = bShowHtml;
    Invalidate();
}

LPCTSTR CListHeaderItemUI::GetNormalImage() const
{
    return m_diNormal.sDrawString;
}

void CListHeaderItemUI::SetNormalImage(LPCTSTR pStrImage)
{
    if (m_diNormal.sDrawString == pStrImage && m_diNormal.pImageInfo != NULL) { return; }

    m_diNormal.Clear();
    m_diNormal.sDrawString = pStrImage;
    Invalidate();
}

LPCTSTR CListHeaderItemUI::GetHotImage() const
{
    return m_diHot.sDrawString;
}

void CListHeaderItemUI::SetHotImage(LPCTSTR pStrImage)
{
    if (m_diHot.sDrawString == pStrImage && m_diHot.pImageInfo != NULL) { return; }

    m_diHot.Clear();
    m_diHot.sDrawString = pStrImage;
    Invalidate();
}

LPCTSTR CListHeaderItemUI::GetPushedImage() const
{
    return m_diPushed.sDrawString;
}

void CListHeaderItemUI::SetPushedImage(LPCTSTR pStrImage)
{
    if (m_diPushed.sDrawString == pStrImage && m_diPushed.pImageInfo != NULL) { return; }

    m_diPushed.Clear();
    m_diPushed.sDrawString = pStrImage;
    Invalidate();
}

LPCTSTR CListHeaderItemUI::GetFocusedImage() const
{
    return m_diFocused.sDrawString;
}

void CListHeaderItemUI::SetFocusedImage(LPCTSTR pStrImage)
{
    if (m_diFocused.sDrawString == pStrImage && m_diFocused.pImageInfo != NULL) { return; }

    m_diFocused.Clear();
    m_diFocused.sDrawString = pStrImage;
    Invalidate();
}

LPCTSTR CListHeaderItemUI::GetSepImage() const
{
    return m_diSep.sDrawString;
}

void CListHeaderItemUI::SetSepImage(LPCTSTR pStrImage)
{
    if (m_diSep.sDrawString == pStrImage && m_diSep.pImageInfo != NULL) { return; }

    m_diSep.Clear();
    m_diSep.sDrawString = pStrImage;
    Invalidate();
}

void CListHeaderItemUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if (_tcscmp(pstrName, _T("dragable")) == 0) { SetDragable(ParseBool(pstrValue)); }
    else if (_tcscmp(pstrName, _T("align")) == 0)
    {
        CDuiString str = ParseString(pstrValue);

        if (str == _T("left"))
        {
            m_uTextStyle &= ~(DT_CENTER | DT_RIGHT);
            m_uTextStyle |= DT_LEFT;
        }
        else if (str == _T("center"))
        {
            m_uTextStyle &= ~(DT_LEFT | DT_RIGHT);
            m_uTextStyle |= DT_CENTER;
        }
        else if (str == _T("right"))
        {
            m_uTextStyle &= ~(DT_LEFT | DT_CENTER);
            m_uTextStyle |= DT_RIGHT;
        }
    }
    else if (_tcscmp(pstrName, _T("valign")) == 0)
    {
        CDuiString str = ParseString(pstrValue);

        if (str == _T("top"))
        {
            m_uTextStyle &= ~(DT_BOTTOM | DT_VCENTER);
            m_uTextStyle |= DT_TOP;
        }
        else if (str == _T("center"))
        {
            m_uTextStyle &= ~(DT_TOP | DT_BOTTOM);
            m_uTextStyle |= DT_VCENTER;
        }
        else if (str == _T("bottom"))
        {
            m_uTextStyle &= ~(DT_TOP | DT_VCENTER);
            m_uTextStyle |= DT_BOTTOM;
        }
    }
    else if (_tcscmp(pstrName, _T("endellipsis")) == 0)
    {
        if (ParseBool(pstrValue)) { m_uTextStyle |= DT_END_ELLIPSIS; }
        else { m_uTextStyle &= ~DT_END_ELLIPSIS; }
    }
    else if (_tcscmp(pstrName, _T("multiline")) == 0)
    {
        if (ParseBool(pstrValue))
        {
            m_uTextStyle  &= ~DT_SINGLELINE;
            m_uTextStyle |= DT_WORDBREAK;
        }
        else { m_uTextStyle |= DT_SINGLELINE; }
    }
    else if (_tcscmp(pstrName, _T("font")) == 0) { SetFont(ParseInt(pstrValue)); }
    else if (_tcscmp(pstrName, _T("textpadding")) == 0 || _tcscmp(pstrName, _T("padding")) == 0)
    {
        RECT rt = ParseRect(pstrValue);
        SetPadding(rt);
    }
    else if (_tcscmp(pstrName, _T("showhtml")) == 0) { SetShowHtml(ParseBool(pstrValue)); }
    else if (_tcscmp(pstrName, _T("normalimage")) == 0) { SetNormalImage(ParseString(pstrValue)); }
    else if (_tcscmp(pstrName, _T("hotimage")) == 0) { SetHotImage(ParseString(pstrValue)); }
    else if (_tcscmp(pstrName, _T("pushedimage")) == 0) { SetPushedImage(ParseString(pstrValue)); }
    else if (_tcscmp(pstrName, _T("focusedimage")) == 0) { SetFocusedImage(ParseString(pstrValue)); }
    else if (_tcscmp(pstrName, _T("sepwidth")) == 0) { SetSepWidth(ParseInt(pstrValue)); }
    else if (_tcscmp(pstrName, _T("sepcolor")) == 0)
    {
        DWORD clr = ParseColor(pstrValue);
        SetSepColor(clr);
    }
    else if (_tcscmp(pstrName, _T("sepimage")) == 0) { SetSepImage(ParseString(pstrValue)); }
    else if (_tcscmp(pstrName, _T("editable")) == 0) { SetEditable(ParseBool(pstrValue)); }
    else if (_tcscmp(pstrName, _T("comboable")) == 0) { SetComboable(ParseBool(pstrValue)); }
    else if (_tcscmp(pstrName, _T("checkable")) == 0) { SetCheckable(ParseBool(pstrValue)); }
    else if (_tcscmp(pstrName, _T("dragenable")) == 0) { DUITRACE(_T("不支持属性:dragenable")); }
    else if (_tcscmp(pstrName, _T("dragimage")) == 0) { DUITRACE(_T("不支持属性:drageimage")); }
    else if (_tcscmp(pstrName, _T("dropenable")) == 0) { DUITRACE(_T("不支持属性:dropenable")); }
    else if (_tcscmp(pstrName, _T("autowidth")) == 0) { DUITRACE(_T("不支持属性:autowidth")); }
    else { CContainerUI::SetAttribute(pstrName, pstrValue); }
}

void CListHeaderItemUI::DoEvent(TEventUI &event)
{
    if (!IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND)
    {
        if (m_pParent != NULL) { m_pParent->DoEvent(event); }
        else { CContainerUI::DoEvent(event); }

        return;
    }

    if (event.Type == UIEVENT_SETFOCUS)
    {
        if (IsEnabled()) { Invalidate(); }
    }

    if (event.Type == UIEVENT_KILLFOCUS)
    {
        if (IsEnabled()) { Invalidate(); }
    }

    if (event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_LBUTTONDBLDOWN)
    {
        if (!IsEnabled()) { return; }

        SetCapture();
        RECT rcSeparator = GetThumbRect();

        if (::PtInRect(&rcSeparator, event.ptMouse))
        {
            if (m_bDragable)
            {
                m_uButtonState |= UISTATE_CAPTURED;
                m_ptLastMouse = event.ptMouse;
            }
        }
        else
        {
			SetCheckState(!m_bChecked);//zm
            m_uButtonState |= UISTATE_PUSHED;
            // 2018-05-23 单击事件放在鼠标弹起时发送
            //m_pManager->SendNotify(this, DUI_MSGTYPE_HEADERCLICK);
            Invalidate();
        }

        return;
    }

    if (event.Type == UIEVENT_BUTTONUP && IsEnabled())
    {
        ReleaseCapture();

        if ((m_uButtonState & UISTATE_CAPTURED) != 0)
        {
            m_uButtonState &= ~UISTATE_CAPTURED;

            if (GetParent()) { GetParent()->NeedParentUpdate(); }
        }
        else if ((m_uButtonState & UISTATE_PUSHED) != 0)
        {
            m_uButtonState &= ~UISTATE_PUSHED;
            Invalidate();
        }

        return;
    }

    if (event.Type == UIEVENT_CLICK && IsEnabled())
    {
        // 2018-05-23 单击事件放在鼠标弹起时发送
        // 2018-07-15 单击事件放在单击消息中发送
        if (IsEnabled() && NULL != m_pManager) { m_pManager->SendNotify(this, DUI_MSGTYPE_HEADERCLICK, 0, (LPARAM)(GetIndex())); }//2021-10-02 zm添加表头位置
    }

    if (event.Type == UIEVENT_MOUSEMOVE)
    {
        if ((m_uButtonState & UISTATE_CAPTURED) != 0 && IsEnabled())
        {
            RECT rc = m_rcItem;

            if (m_iSepWidth >= 0)
            {
                rc.right -= m_ptLastMouse.x - event.ptMouse.x;
            }
            else
            {
                rc.left -= m_ptLastMouse.x - event.ptMouse.x;
            }

            if (rc.right - rc.left > GetMinWidth())
            {
                m_cxyFixed.cx = rc.right - rc.left;
                m_ptLastMouse = event.ptMouse;

                if (GetParent()) { GetParent()->NeedParentUpdate(); }
            }
        }

        return;
    }

    if (event.Type == UIEVENT_SETCURSOR)
    {
        if (m_bDragable && IsEnabled())
        {
            RECT rcSeparator = GetThumbRect();

            if (::PtInRect(&rcSeparator, event.ptMouse))
            {
                ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZEWE)));
                return;
            }
        }
    }

    if (event.Type == UIEVENT_MOUSEENTER)
    {
        if (IsEnabled() && ::PtInRect(&m_rcItem, event.ptMouse))
        {
            if ((m_uButtonState & UISTATE_HOT) == 0)
            {
                m_uButtonState |= UISTATE_HOT;
                Invalidate();
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

    CContainerUI::DoEvent(event);
}

SIZE CListHeaderItemUI::EstimateSize(SIZE szAvailable)
{
    if (m_cxyFixed.cy == 0) { return CDuiSize(m_cxyFixed.cx, m_pManager->GetDefaultFontInfo()->tm.tmHeight + 8); }

    return CContainerUI::EstimateSize(szAvailable);
}

RECT CListHeaderItemUI::GetThumbRect() const
{
    if (m_iSepWidth >= 0) { return CDuiRect(m_rcItem.right - m_iSepWidth, m_rcItem.top, m_rcItem.right, m_rcItem.bottom); }
    else { return CDuiRect(m_rcItem.left, m_rcItem.top, m_rcItem.left - m_iSepWidth, m_rcItem.bottom); }
}

void CListHeaderItemUI::PaintStatusImage(HDC hDC)
{
    if (IsFocused()) { m_uButtonState |= UISTATE_FOCUSED; }
    else { m_uButtonState &= ~ UISTATE_FOCUSED; }

    if ((m_uButtonState & UISTATE_PUSHED) != 0)
    {
        if (!DrawImage(hDC, m_diPushed))  { DrawImage(hDC, m_diNormal); }
    }
    else if ((m_uButtonState & UISTATE_HOT) != 0)
    {
        if (!DrawImage(hDC, m_diHot))  { DrawImage(hDC, m_diNormal); }
    }
    else if ((m_uButtonState & UISTATE_FOCUSED) != 0)
    {
        if (!DrawImage(hDC, m_diFocused))  { DrawImage(hDC, m_diNormal); }
    }
    else
    {
        DrawImage(hDC, m_diNormal);
    }

    if (m_iSepWidth > 0)
    {
        RECT rcThumb = GetThumbRect();
        m_diSep.rcDestOffset.left = rcThumb.left - m_rcItem.left;
        m_diSep.rcDestOffset.top = rcThumb.top - m_rcItem.top;
        m_diSep.rcDestOffset.right = rcThumb.right - m_rcItem.left;
        m_diSep.rcDestOffset.bottom = rcThumb.bottom - m_rcItem.top;

        if (!DrawImage(hDC, m_diSep))
        {
            if (m_dwSepColor != 0)
            {
                RECT rcSepLine = { rcThumb.left + m_iSepWidth / 2, rcThumb.top, rcThumb.left + m_iSepWidth / 2, rcThumb.bottom};
                CRenderEngine::DrawLine(hDC, rcSepLine, m_iSepWidth, GetAdjustColor(m_dwSepColor));
            }
        }
    }
}

void CListHeaderItemUI::SetEditable(bool bEditable)
{
    m_bEditable = bEditable;
}

bool CListHeaderItemUI::IsEditable()
{
    return m_bEditable;
}

void CListHeaderItemUI::SetComboable(bool bComboable)
{
    m_bComboable = bComboable;
}

bool CListHeaderItemUI::IsComboable()
{
    return m_bComboable;
}

void CListHeaderItemUI::SetCheckable(bool bCheckable)
{
    m_bCheckable = bCheckable;
}

bool CListHeaderItemUI::IsCheckable()
{
    return m_bCheckable;
}

void CListHeaderItemUI::SetCheckState(bool bChecked)
{
	m_bChecked = bChecked;
	Invalidate();
}

bool CListHeaderItemUI::GetCheckState() const
{
	return m_bChecked;
}

void CListHeaderItemUI::SetOwner(ICheckBoxUI* pCheckBox)
{
	m_pOwner = pCheckBox;
}

int CListHeaderItemUI::GetIndex()
{
	if (GetParent())
	{
		CContainerUI* pobjContainer = static_cast<CContainerUI*>(GetParent());
		return pobjContainer->GetItemIndex(this);
	}
	return -1;
}

void CListHeaderItemUI::PaintText(HDC hDC)
{
    if (m_dwTextColor == 0) { m_dwTextColor = m_pManager->GetDefaultFontColor(); }

    DWORD clrText = IsEnabled() ? m_dwTextColor : m_pManager->GetDefaultDisabledColor();

	if (m_sText.IsEmpty()) { return; }

	//2021-10-17 zm 
	RECT rcCheck = { 0 };
	if (IsCheckable() && m_pOwner && m_pManager)
	{
		rcCheck = m_pOwner->GetCheckBoxRect(m_rcItem);

		if (GetCheckState())
		{
			CRenderEngine::DrawImage(hDC, m_pManager, rcCheck, m_rcPaint, m_pOwner->GetSelImage());
		}
		else
		{
			CRenderEngine::DrawImage(hDC, m_pManager, rcCheck, m_rcPaint, m_pOwner->GetUnSelImage());
		}
	}

    RECT rcText = m_rcItem;
    rcText.left += (m_rcBorderSize.left + m_rcPadding.left + (rcCheck.right - rcCheck.left));
    rcText.top += (m_rcBorderSize.top + m_rcPadding.top);
    rcText.right -= (m_rcBorderSize.right + m_rcPadding.right);
    rcText.bottom -= (m_rcBorderSize.bottom + m_rcPadding.bottom);

    if (m_bShowHtml)
    { CRenderEngine::DrawHtmlText(hDC, m_pManager, rcText, m_sText, clrText, NULL, NULL, NULL, m_iFont, m_uTextStyle); }
    else
    { CRenderEngine::DrawText(hDC, m_pManager, rcText, m_sText, clrText, m_iFont, m_uTextStyle); }
}

/////////////////////////////////////////////////////////////////////////////////////
//
//

CListElementUI::CListElementUI() :
m_iIndex(-1),
m_iDrawIndex(0),
m_pOwner(NULL),
m_bSelected(false),
m_uButtonState(0),
m_bCustomBk(false),
m_bShiftEnble(false)//zm
{
}

LPCTSTR CListElementUI::GetClass() const
{
    return DUI_CTR_LISTELEMENT;
}

UINT CListElementUI::GetControlFlags() const
{
    return UIFLAG_WANTRETURN;
}

LPVOID CListElementUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcscmp(pstrName, DUI_CTR_ILISTITEM) == 0) { return static_cast<IListItemUI *>(this); }

    if (_tcscmp(pstrName, DUI_CTR_LISTELEMENT) == 0) { return static_cast<CListElementUI *>(this); }

    return CControlUI::GetInterface(pstrName);
}

IListOwnerUI *CListElementUI::GetOwner()
{
    return m_pOwner;
}

void CListElementUI::SetOwner(CControlUI *pOwner)
{
    if (pOwner != NULL) { m_pOwner = static_cast<IListOwnerUI *>(pOwner->GetInterface(DUI_CTR_ILISTOWNER)); }
}

bool CListElementUI::SetVisible(bool bVisible /*= true*/)
{
    // 2018-08-18 zhuyadong 添加特效
    if (!CControlUI::SetVisible(bVisible)) { return false; }

    if (!IsVisible() && m_bSelected)
    {
        m_bSelected = false;

        if (m_pOwner != NULL) { m_pOwner->SelectItem(-1); }
    }

    return true;
}

void CListElementUI::SetEnabled(bool bEnable)
{
    CControlUI::SetEnabled(bEnable);

    if (!IsEnabled())
    {
        m_uButtonState = 0;
    }
}

int CListElementUI::GetIndex() const
{
    return m_iIndex;
}

void CListElementUI::SetIndex(int iIndex)
{
    m_iIndex = iIndex;
}

int CListElementUI::GetDrawIndex() const
{
    return m_iDrawIndex;
}

void CListElementUI::SetDrawIndex(int iIndex)
{
    m_iDrawIndex = iIndex;
}

void CListElementUI::Invalidate()
{
    if (!IsVisible()) { return; }

    if (GetParent())
    {
        CContainerUI *pParentContainer = static_cast<CContainerUI *>(GetParent()->GetInterface(DUI_CTR_CONTAINER));

        if (pParentContainer)
        {
            RECT rc = pParentContainer->GetPos();
            RECT rcInset = pParentContainer->GetPadding();
            rc.left += rcInset.left;
            rc.top += rcInset.top;
            rc.right -= rcInset.right;
            rc.bottom -= rcInset.bottom;
            CScrollBarUI *pVerticalScrollBar = pParentContainer->GetVerticalScrollBar();

            if (pVerticalScrollBar && pVerticalScrollBar->IsVisible()) { rc.right -= pVerticalScrollBar->GetFixedWidth(); }

            CScrollBarUI *pHorizontalScrollBar = pParentContainer->GetHorizontalScrollBar();

            if (pHorizontalScrollBar && pHorizontalScrollBar->IsVisible()) { rc.bottom -= pHorizontalScrollBar->GetFixedHeight(); }

            RECT invalidateRc = m_rcItem;

            if (!::IntersectRect(&invalidateRc, &m_rcItem, &rc))
            {
                return;
            }

            CControlUI *pParent = GetParent();
            RECT rcTemp;
            RECT rcParent;

            while (pParent = pParent->GetParent())
            {
                rcTemp = invalidateRc;
                rcParent = pParent->GetPos();

                if (!::IntersectRect(&invalidateRc, &rcTemp, &rcParent))
                {
                    return;
                }
            }

            if (m_pManager != NULL) { m_pManager->Invalidate(invalidateRc); }
        }
        else
        {
            CControlUI::Invalidate();
        }
    }
    else
    {
        CControlUI::Invalidate();
    }
}

bool CListElementUI::Activate()
{
    if (!CControlUI::Activate()) { return false; }

    if (m_pManager != NULL) { m_pManager->SendNotify(this, DUI_MSGTYPE_ITEMACTIVATE); }

    return true;
}

bool CListElementUI::IsSelected() const
{
    return m_bSelected;
}

bool CListElementUI::Select(bool bSelect, bool bTriggerEvent)
{
    if (!IsEnabled()) { return false; }

	if (m_pOwner != NULL && m_bSelected) m_pOwner->UnSelectItem(m_iIndex, true);//zm

    if (bSelect == m_bSelected) { return true; }

    m_bSelected = bSelect;

    if (bSelect && m_pOwner != NULL) { m_pOwner->SelectItem(m_iIndex, bTriggerEvent); }

    Invalidate();

    return true;
}

bool CListElementUI::SelectMulti(bool bSelect)
{
	if (!IsEnabled()) { return false; }
	if (bSelect == m_bSelected) { return true; }

	m_bSelected = bSelect;

	if (bSelect && m_pOwner != NULL) m_pOwner->SelectMultiItem(m_iIndex);

	Invalidate();
	return true;
}

bool CListElementUI::IsExpanded() const
{
    return false;
}

bool CListElementUI::Expand(bool /*bExpand = true*/)
{
    return false;
}

void CListElementUI::DoEvent(TEventUI &event)
{
    if (!IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND)
    {
        if (m_pOwner != NULL) { m_pOwner->DoEvent(event); }
        else { CControlUI::DoEvent(event); }

        return;
    }

    if (event.Type == UIEVENT_DBLCLICK)
    {
        if (IsEnabled())
        {
            Activate();
            Invalidate();
        }

        return;
    }

    if (event.Type == UIEVENT_KEYDOWN)
    {
        if (IsKeyboardEnabled() && IsEnabled())
        {
            if (event.chKey == VK_RETURN)
            {
                Activate();
                Invalidate();
                return;
            }
        }
    }

    // 2018-08-28 zhuyadong 解决 List 不支持拖拽源、目的的问题
    if (event.Type == UIEVENT_MOUSEMOVE && (event.wParam & MK_LBUTTON) && m_bDragEnable)
    {
        if (IsEnabled()) { OnDoDragDrop(event); }

        return;
    }

    // An important twist: The list-item will send the event not to its immediate
    // parent but to the "attached" list. A list may actually embed several components
    // in its path to the item, but key-presses etc. needs to go to the actual list.
    if (m_pOwner != NULL) { m_pOwner->DoEvent(event); }
    else { CControlUI::DoEvent(event); }
}

void CListElementUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if (_tcscmp(pstrName, _T("selected")) == 0) { Select(ParseBool(pstrValue)); }
	else if (_tcscmp(pstrName, _T("bkcolor")) == 0)
    {
        // 2018-08-28 zhuyddong 优化属性取值
        m_dwBackColor = ParseColor(pstrValue);
        m_bCustomBk = true;
    }
    // 2018-08-28 zhuyadong 解决 List 不支持拖拽源、目的的问题，去掉属性拦截
    else if (_tcscmp(pstrName, _T("autowidth")) == 0) { DUITRACE(_T("不支持属性:autowidth")); }
    else { CControlUI::SetAttribute(pstrName, pstrValue); }
}

void CListElementUI::DrawItemBk(HDC hDC, const RECT &rcItem)
{
    ASSERT(m_pOwner);

    if (m_pOwner == NULL) { return; }

    TListInfoUI *pInfo = m_pOwner->GetListInfo();

    if (pInfo == NULL) { return; }

	DWORD iBackColor = 0;

	if (!pInfo->bAlternateBk || m_iDrawIndex % 2 == 0) { iBackColor = pInfo->dwBkColor; }

	if ((m_uButtonState & UISTATE_HOT) != 0)
	{
		iBackColor = pInfo->dwHotBkColor;
	}

	if (IsSelected())
	{
		iBackColor = pInfo->dwSelectedBkColor;
	}

	if (!IsEnabled())
	{
		iBackColor = pInfo->dwDisabledBkColor;
	}

	if (m_bCustomBk)
	{
		iBackColor = m_dwBackColor;
	}

    if (iBackColor != 0)
    {
        // 背景色
        CRenderEngine::DrawColor(hDC, rcItem, GetAdjustColor(iBackColor));

        // 边框
        if (IsSelected() && 0 != m_rcBorderSize.left)
        {
            CRenderEngine::DrawRect(hDC, m_rcItem, m_rcBorderSize.left, m_dwSelectedBorderColor);
        }

        if ((m_uButtonState & UISTATE_HOT) != 0 && 0 != m_rcBorderSize.left)
        {
            CRenderEngine::DrawRect(hDC, m_rcItem, m_rcBorderSize.left, m_dwHotBorderColor);
        }

        return;
    }

    if (!IsEnabled())
    {
        if (DrawImage(hDC, pInfo->diDisabled)) { return; }
    }

    if (IsSelected())
    {
        if (DrawImage(hDC, pInfo->diSelected)) { return; }
    }

    if ((m_uButtonState & UISTATE_HOT) != 0)
    {
        if (DrawImage(hDC, pInfo->diHot)) { return; }
    }

    if (!DrawImage(hDC, m_diBk))
    {
        if (!pInfo->bAlternateBk || m_iDrawIndex % 2 == 0)
        {
            if (DrawImage(hDC, pInfo->diBk)) { return; }
        }
    }
}

void CListElementUI::DrawItemImage(HDC hDC, const RECT &rcItem)
{

}

int CListElementUI::GetMouseColumn(POINT pt)
{
    CListHeaderUI *pHeader = ((CListUI *)m_pOwner)->GetHeader();

    if (NULL == pHeader) { return -1; }

    RECT rtColumn = m_rcItem;

    for (int i = 0; i < pHeader->GetCount(); ++i)
    {
        CControlUI *pHI = pHeader->GetItemAt(i);
        RECT rt = pHI->GetPos();
        rtColumn.left = rt.left;
        rtColumn.right = rt.right;

        if (::PtInRect(&rtColumn, pt)) { return i; }
    }

    return -1;
}

RECT CListElementUI::GetSubItemPos(int nColumn, bool bList)
{
    CListUI *pList = (CListUI *)m_pOwner;
    TListInfoUI *pInfo = m_pOwner->GetListInfo();

    if (nColumn >= pList->GetHeader()->GetCount()) { return RECT { 0, 0, 0, 0 }; }

    RECT rtColumn = { pInfo->rcColumn[nColumn].left, m_rcItem.top, pInfo->rcColumn[nColumn].right, m_rcItem.bottom };

    // 避免遮住滚动条
    CScrollBarUI *pSB = pList->GetVerticalScrollBar();

    if (NULL != pSB)
    {
        int nSBW = pSB->GetWidth();
        int nRight = pList->GetPos().right;
        nRight -= nSBW;
        rtColumn.right = (rtColumn.right > nRight) ? nRight : rtColumn.right;
    }

    if (bList)
    {
        // 相对当前List位置
        rtColumn.left -= m_rcItem.left;
        rtColumn.top -= pInfo->rcColumn[nColumn].top;
        rtColumn.right -= m_rcItem.left;
        rtColumn.bottom -= pInfo->rcColumn[nColumn].top;
    }

    return rtColumn;
}

void CListElementUI::SetShiftEnble(bool bShiftEnble)
{
	if (m_bShiftEnble == bShiftEnble) { return; }

	m_bShiftEnble = bShiftEnble;
}

bool CListElementUI::GetShiftEnble()
{
	return m_bShiftEnble;
}

/////////////////////////////////////////////////////////////////////////////////////
//
//

CListLabelElementUI::CListLabelElementUI() : m_bNeedEstimateSize(true), m_uFixedHeightLast(0),
    m_nFontLast(-1), m_uTextStyleLast(0)
{
    m_szAvailableLast.cx = m_szAvailableLast.cy = 0;
    m_cxyFixedLast.cx = m_cxyFixedLast.cy = 0;
    ::ZeroMemory(&m_rcTextPaddingLast, sizeof(m_rcTextPaddingLast));
}

LPCTSTR CListLabelElementUI::GetClass() const
{
    return DUI_CTR_LISTLABELELEMENT;
}

LPVOID CListLabelElementUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcscmp(pstrName, DUI_CTR_LISTLABELELEMENT) == 0) { return static_cast<CListLabelElementUI *>(this); }

    return CListElementUI::GetInterface(pstrName);
}

void CListLabelElementUI::SetOwner(CControlUI *pOwner)
{
    m_bNeedEstimateSize = true;
    CListElementUI::SetOwner(pOwner);
}

void CListLabelElementUI::SetFixedWidth(int cx)
{
    m_bNeedEstimateSize = true;
    CControlUI::SetFixedWidth(cx);
}

void CListLabelElementUI::SetFixedHeight(int cy)
{
    m_bNeedEstimateSize = true;
    CControlUI::SetFixedHeight(cy);
}

void CListLabelElementUI::SetText(LPCTSTR pstrText)
{
    m_bNeedEstimateSize = true;
    CControlUI::SetText(pstrText);
}

void CListLabelElementUI::DoEvent(TEventUI &event)
{
    if (!IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND)
    {
        if (m_pOwner != NULL) { m_pOwner->DoEvent(event); }
        else { CListElementUI::DoEvent(event); }

        return;
    }

    if (event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_RBUTTONDOWN)
    {
		//2021-10-16 zm添加键盘选选择
		if (IsEnabled() && (m_pOwner != NULL))
        {
            SetCapture();
            // 2018-05-23 单击事件放在鼠标弹起时发送
            //m_pManager->SendNotify(this, DUI_MSGTYPE_ITEMCLICK);

			Select();
			Invalidate();
        }

        return;
    }

    // 2018-05-23 单击事件放在鼠标弹起时发送
    if (event.Type == UIEVENT_BUTTONUP || event.Type == UIEVENT_RBUTTONUP)
    {
		if (IsEnabled() && NULL != m_pManager && (m_pOwner != NULL))
        {
            ReleaseCapture();

			if (IsSelected()) { m_pManager->SendNotify(this, DUI_MSGTYPE_ITEMCLICK); }
        }

        return;
    }

    // 2018-08-28 zhuyadong 解决 List 不支持拖拽源、目的的问题
    // if (event.Type == UIEVENT_MOUSEMOVE) { return; }

    if (event.Type == UIEVENT_MOUSEENTER)
    {
        if (IsEnabled() && ::PtInRect(&m_rcItem, event.ptMouse))
        {
            if ((m_uButtonState & UISTATE_HOT) == 0)
            {
                m_uButtonState |= UISTATE_HOT;
                Invalidate();
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

    CListElementUI::DoEvent(event);
}

SIZE CListLabelElementUI::EstimateSize(SIZE szAvailable)
{
    if (m_pOwner == NULL) { return CDuiSize(0, 0); }

    TListInfoUI *pInfo = m_pOwner->GetListInfo();

    if (pInfo == NULL) { return CDuiSize(0, 0); }

    if (m_cxyFixed.cx > 0)
    {
        if (m_cxyFixed.cy > 0) { return m_cxyFixed; }
        else if (pInfo->uFixedHeight > 0) { return CDuiSize(m_cxyFixed.cx, pInfo->uFixedHeight); }
    }

    if ((pInfo->uTextStyle & DT_SINGLELINE) == 0 &&
        (szAvailable.cx != m_szAvailableLast.cx || szAvailable.cy != m_szAvailableLast.cy))
    {
        m_bNeedEstimateSize = true;
    }

    if (m_uFixedHeightLast != pInfo->uFixedHeight || m_nFontLast != pInfo->nFont ||
        m_uTextStyleLast != pInfo->uTextStyle ||
        m_rcTextPaddingLast.left != pInfo->rcPadding.left ||
        m_rcTextPaddingLast.right != pInfo->rcPadding.right ||
        m_rcTextPaddingLast.top != pInfo->rcPadding.top ||
        m_rcTextPaddingLast.bottom != pInfo->rcPadding.bottom)
    {
        m_bNeedEstimateSize = true;
    }

    if (m_bNeedEstimateSize)
    {
        m_bNeedEstimateSize = false;
        m_szAvailableLast = szAvailable;
        m_uFixedHeightLast = pInfo->uFixedHeight;
        m_nFontLast = pInfo->nFont;
        m_uTextStyleLast = pInfo->uTextStyle;
        m_rcTextPaddingLast = pInfo->rcPadding;

        m_cxyFixedLast = m_cxyFixed;

        if (m_cxyFixedLast.cy == 0)
        {
            m_cxyFixedLast.cy = pInfo->uFixedHeight;
        }

        if ((pInfo->uTextStyle & DT_SINGLELINE) != 0)
        {
            if (m_cxyFixedLast.cy == 0)
            {
                m_cxyFixedLast.cy = m_pManager->GetFontInfo(pInfo->nFont)->tm.tmHeight + 8;
                m_cxyFixedLast.cy += pInfo->rcPadding.top + pInfo->rcPadding.bottom;
            }

            if (m_cxyFixedLast.cx == 0)
            {
                RECT rcText = { 0, 0, MAX_CTRL_WIDTH, m_cxyFixedLast.cy };

                if (pInfo->bShowHtml)
                {
                    CRenderEngine::DrawHtmlText(m_pManager->GetPaintDC(), m_pManager, rcText, m_sText, 0, NULL, NULL, NULL,
                                                pInfo->nFont, DT_CALCRECT | pInfo->uTextStyle & ~DT_RIGHT & ~DT_CENTER);
                }
                else
                {
                    CRenderEngine::DrawText(m_pManager->GetPaintDC(), m_pManager, rcText, m_sText, 0, pInfo->nFont,
                                            DT_CALCRECT | pInfo->uTextStyle & ~DT_RIGHT & ~DT_CENTER);
                }

                m_cxyFixedLast.cx = rcText.right - rcText.left + pInfo->rcPadding.left + pInfo->rcPadding.right;
            }
        }
        else
        {
            if (m_cxyFixedLast.cx == 0)
            {
                m_cxyFixedLast.cx = szAvailable.cx;
            }

            RECT rcText = { 0, 0, m_cxyFixedLast.cx, MAX_CTRL_WIDTH };
            rcText.left += pInfo->rcPadding.left;
            rcText.right -= pInfo->rcPadding.right;

            if (pInfo->bShowHtml)
            {
                CRenderEngine::DrawHtmlText(m_pManager->GetPaintDC(), m_pManager, rcText, m_sText, 0, NULL, NULL, NULL,
                                            pInfo->nFont, DT_CALCRECT | pInfo->uTextStyle & ~DT_RIGHT & ~DT_CENTER);
            }
            else
            {
                CRenderEngine::DrawText(m_pManager->GetPaintDC(), m_pManager, rcText, m_sText, 0, pInfo->nFont,
                                        DT_CALCRECT | pInfo->uTextStyle & ~DT_RIGHT & ~DT_CENTER);
            }

            m_cxyFixedLast.cy = rcText.bottom - rcText.top + pInfo->rcPadding.top + pInfo->rcPadding.bottom;
        }
    }

    return m_cxyFixedLast;
}

bool CListLabelElementUI::DoPaint(HDC hDC, const RECT &rcPaint, CControlUI *pStopControl)
{
    DrawItemBk(hDC, m_rcItem);
	DrawItemImage(hDC, m_rcItem);//zm
    DrawItemText(hDC, m_rcItem);
    return true;
}

void CListLabelElementUI::DrawItemText(HDC hDC, const RECT &rcItem)
{
    if (m_sText.IsEmpty()) { return; }

    if (m_pOwner == NULL) { return; }

    TListInfoUI *pInfo = m_pOwner->GetListInfo();

    if (pInfo == NULL) { return; }

    DWORD iTextColor = pInfo->dwTextColor;

    if ((m_uButtonState & UISTATE_HOT) != 0)
    {
        iTextColor = pInfo->dwHotTextColor;
    }

    if (IsSelected())
    {
        iTextColor = pInfo->dwSelectedTextColor;
    }

    if (!IsEnabled())
    {
        iTextColor = pInfo->dwDisabledTextColor;
    }

    RECT rcText = rcItem;
    rcText.left += pInfo->rcPadding.left;
    rcText.right -= pInfo->rcPadding.right;
    rcText.top += pInfo->rcPadding.top;
    rcText.bottom -= pInfo->rcPadding.bottom;

    if (pInfo->bShowHtml)
        CRenderEngine::DrawHtmlText(hDC, m_pManager, rcText, m_sText, iTextColor,
                                    NULL, NULL, NULL, pInfo->nFont, pInfo->uTextStyle);
    else
        CRenderEngine::DrawText(hDC, m_pManager, rcText, m_sText, iTextColor,
                                pInfo->nFont, pInfo->uTextStyle);
}


/////////////////////////////////////////////////////////////////////////////////////
//
//

CListTextElementUI::CListTextElementUI() : m_nLinks(0), m_nHoverLink(-1), m_pOwner(NULL), m_bCheckBoxSelect(0)
{
    ::ZeroMemory(&m_rcLinks, sizeof(m_rcLinks));
}

CListTextElementUI::~CListTextElementUI()
{
    CDuiString *pText;

    for (int it = 0; it < m_aTexts.GetSize(); it++)
    {
        pText = static_cast<CDuiString *>(m_aTexts[it]);

        if (pText) { delete pText; }
    }

    m_aTexts.Empty();
}

LPCTSTR CListTextElementUI::GetClass() const
{
    return DUI_CTR_LISTTEXTELEMENT;
}

LPVOID CListTextElementUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcscmp(pstrName, DUI_CTR_LISTTEXTELEMENT) == 0) { return static_cast<CListTextElementUI *>(this); }

    return CListLabelElementUI::GetInterface(pstrName);
}

UINT CListTextElementUI::GetControlFlags() const
{
    return UIFLAG_WANTRETURN | ((IsEnabled() && m_nLinks > 0) ? UIFLAG_SETCURSOR : 0);
}

LPCTSTR CListTextElementUI::GetText(int iIndex) const
{
    CDuiString *pText = static_cast<CDuiString *>(m_aTexts.GetAt(iIndex));

    if (pText) { return pText->GetData(); }

    return NULL;
}

void CListTextElementUI::SetText(int iIndex, LPCTSTR pstrText)
{
    if (m_pOwner == NULL) { return; }

    TListInfoUI *pInfo = m_pOwner->GetListInfo();

    if (iIndex < 0 || iIndex >= pInfo->nColumns) { return; }

    m_bNeedEstimateSize = true;

    while (m_aTexts.GetSize() < pInfo->nColumns) { m_aTexts.Add(NULL); }

    CDuiString *pText = static_cast<CDuiString *>(m_aTexts[iIndex]);

    if ((pText == NULL && pstrText == NULL) || (pText && *pText == pstrText)) { return; }

    // 2018-08-27 zhuyadong 修复多语言支持问题
    CDuiString str(pstrText);
    CPaintManagerUI::ProcessMultiLanguageTokens(str);

    if (pText) { pText->Assign(str); } //by cddjr 2011/10/20
    else       { m_aTexts.SetAt(iIndex, new CDuiString(str)); }

    Invalidate();
}

void CListTextElementUI::SetOwner(CControlUI *pOwner)
{
    if (pOwner != NULL)
    {
        m_bNeedEstimateSize = true;
        CListElementUI::SetOwner(pOwner);
        m_pOwner = static_cast<IListUI *>(pOwner->GetInterface(DUI_CTR_ILIST));
    }
}

CDuiString *CListTextElementUI::GetLinkContent(int iIndex)
{
    if (iIndex >= 0 && iIndex < m_nLinks) { return &m_sLinks[iIndex]; }

    return NULL;
}

void CListTextElementUI::DoEvent(TEventUI &event)
{
    if (!IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND)
    {
        if (m_pOwner != NULL) { m_pOwner->DoEvent(event); }
        else { CListLabelElementUI::DoEvent(event); }

        return;
    }

    // When you hover over a link
    if (event.Type == UIEVENT_SETCURSOR)
    {
        if (IsEnabled())
        {
            for (int i = 0; i < m_nLinks; i++)
            {
                if (::PtInRect(&m_rcLinks[i], event.ptMouse))
                {
                    ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
                    return;
                }
            }
        }
    }

	//2021-10-17 zm 添加键盘多选，鼠标框选
	if (event.Type == UIEVENT_BUTTONDOWN && m_pOwner)
	{
		SetCapture();

		if (m_pOwner->IsMultiSelect())
		{
			if ((GetKeyState(VK_CONTROL) & 0x8000))
			{
				SelectMulti(!IsSelected());
			}
			else if ((GetKeyState(VK_SHIFT) & 0x8000))
			{
				SetShiftEnble(true);
				m_pOwner->SelectMultiItem(m_iIndex);
			}
			else
			{
				Select();
				m_pOwner->DragBegin(event);
			}

			m_pOwner->DoEvent(event);
		}
		else
		{
			Select();
		}

		Invalidate();
		return;
	}

	//当多选之后，在多选区域右键弹出菜单时，希望多选的项不被取消
	if (event.Type == UIEVENT_RBUTTONDOWN && m_pOwner)
	{
		SetCapture();

		if (m_pOwner)
		{
			if (m_pOwner->IsPtInSelItem(event.ptMouse)) { return; }
	
			Select();
			Invalidate();
		}

		return;
	}

    if (event.Type == UIEVENT_BUTTONUP)
    {
		ReleaseCapture();

        TListInfoUI *pInfo = m_pOwner->GetListInfo();

        if (pInfo && pInfo->bCheckBox)
        {
            RECT rcItem = { pInfo->rcColumn[0].left, m_rcItem.top, pInfo->rcColumn[0].right, m_rcItem.bottom };
            rcItem.left += pInfo->rcPadding.left;
            rcItem.right -= pInfo->rcPadding.right;
            rcItem.top += pInfo->rcPadding.top;
            rcItem.bottom -= pInfo->rcPadding.bottom;

            if (PtInRect(&rcItem, event.ptMouse))
            {
                m_bCheckBoxSelect = !m_bCheckBoxSelect;
                Invalidate();
            }
        }

        for (int i = 0; i < m_nLinks; i++)
        {
            if (::PtInRect(&m_rcLinks[i], event.ptMouse) && NULL != m_pManager)
            {
                m_pManager->SendNotify(this, DUI_MSGTYPE_LINK, i);
                return;
            }
        }

		//2021-10-02 zm 与双击事件消息同步,方便获取行,行信息
		if (IsSelected() && m_pManager) { m_pManager->SendNotify((CListUI *)m_pOwner, DUI_MSGTYPE_ITEMCLICK, (WPARAM)this, (LPARAM)GetMouseColumn(event.ptMouse)); }

		//2021-10-17 zm
		if (m_pOwner && m_pOwner->IsMultiSelect())
		{
			if (GetShiftEnble()) { SetShiftEnble(false); }

			m_pOwner->DragEnd(event);

			m_pOwner->DoEvent(event);

		}

		return;
    }

    if (m_nLinks > 0 && event.Type == UIEVENT_MOUSEMOVE)
    {
        if (IsEnabled())
        {
            int nHoverLink = -1;

            for (int i = 0; i < m_nLinks; i++)
            {
                if (::PtInRect(&m_rcLinks[i], event.ptMouse))
                {
                    nHoverLink = i;
                    break;
                }
            }

            if (m_nHoverLink != nHoverLink)
            {
                Invalidate();
                m_nHoverLink = nHoverLink;
            }

        }
    }

	//2021-10-17 zm
	if (event.Type == UIEVENT_MOUSEMOVE)
	{
		if (m_pOwner && m_pOwner->IsMultiSelect())
		{
			m_pOwner->Draging(event);

			m_pOwner->DoEvent(event);
		}

		return;
	}

    if (m_nLinks > 0 && event.Type == UIEVENT_MOUSELEAVE)
    {
        if (IsEnabled())
        {
            if (m_nHoverLink != -1)
            {
                if (!::PtInRect(&m_rcLinks[m_nHoverLink], event.ptMouse))
                {
                    m_nHoverLink = -1;
                    Invalidate();

                    if (m_pManager) { m_pManager->RemoveMouseLeaveNeeded(this); }
                }
                else
                {
                    if (m_pManager) { m_pManager->AddMouseLeaveNeeded(this); }

                    return;
                }
            }
        }
    }

    if (event.Type == UIEVENT_DBLCLICK)
    {
        if (IsEnabled())
        {
            Select();
            Invalidate();
            // 编辑框 下拉框
            int nColumn = GetMouseColumn(event.ptMouse);
            CListHeaderItemUI *pHeader = dynamic_cast<CListHeaderItemUI *>(m_pOwner->GetHeader()->GetItemAt(nColumn));
            RECT rt = GetSubItemPos(nColumn, true);
            CDuiString sTxt = GetText(nColumn);

            if (pHeader->IsEditable())
            {
                m_pOwner->ShowEdit(GetIndex(), nColumn, rt, sTxt);
            }
            else if (pHeader->IsComboable())
            {
                m_pOwner->ShowCombo(GetIndex(), nColumn, rt, sTxt);
            }
            else
            {
                m_pOwner->HideEdit();
                m_pOwner->HideCombo();
                m_pManager->SendNotify((CListUI *)m_pOwner, DUI_MSGTYPE_ITEMDBCLICK, (WPARAM)this, nColumn);
            }
        }

        return;
    }

    if (event.Type == UIEVENT_SCROLLWHEEL || event.Type == UIEVENT_BUTTONDOWN)
    {
        // 隐藏编辑框、下拉框
        m_pOwner->HideEdit();
        m_pOwner->HideCombo();
    }

    CListLabelElementUI::DoEvent(event);
}

SIZE CListTextElementUI::EstimateSize(SIZE szAvailable)
{
    if (m_pOwner == NULL) { return CDuiSize(0, 0); }

    TListInfoUI *pInfo = m_pOwner->GetListInfo();

    if (pInfo == NULL) { return CDuiSize(0, 0); }

    SIZE cxyFixed = m_cxyFixed;

    if (cxyFixed.cx == 0 && pInfo->nColumns > 0)
    {
        cxyFixed.cx = pInfo->rcColumn[pInfo->nColumns - 1].right - pInfo->rcColumn[0].left;

        if (m_cxyFixedLast.cx != cxyFixed.cx) { m_bNeedEstimateSize = true; }
    }

    if (cxyFixed.cx > 0)
    {
        if (cxyFixed.cy > 0) { return cxyFixed; }
        else if (pInfo->uFixedHeight > 0) { return CDuiSize(cxyFixed.cx, pInfo->uFixedHeight); }
    }

    if ((pInfo->uTextStyle & DT_SINGLELINE) == 0 &&
        (szAvailable.cx != m_szAvailableLast.cx || szAvailable.cy != m_szAvailableLast.cy))
    {
        m_bNeedEstimateSize = true;
    }

    if (m_uFixedHeightLast != pInfo->uFixedHeight || m_nFontLast != pInfo->nFont ||
        m_uTextStyleLast != pInfo->uTextStyle ||
        m_rcTextPaddingLast.left != pInfo->rcPadding.left ||
        m_rcTextPaddingLast.right != pInfo->rcPadding.right ||
        m_rcTextPaddingLast.top != pInfo->rcPadding.top ||
        m_rcTextPaddingLast.bottom != pInfo->rcPadding.bottom)
    {
        m_bNeedEstimateSize = true;
    }

    CDuiString strText;
    IListCallbackUI *pCallback = m_pOwner->GetTextCallback();

    if (pCallback) { strText = pCallback->GetItemText(this, m_iIndex, 0); }
    else if (m_aTexts.GetSize() > 0) { strText.Assign(GetText(0)); }
    else { strText = m_sText; }

    if (m_sTextLast != strText) { m_bNeedEstimateSize = true; }

    if (m_bNeedEstimateSize)
    {
        m_bNeedEstimateSize = false;
        m_szAvailableLast = szAvailable;
        m_uFixedHeightLast = pInfo->uFixedHeight;
        m_nFontLast = pInfo->nFont;
        m_uTextStyleLast = pInfo->uTextStyle;
        m_rcTextPaddingLast = pInfo->rcPadding;
        m_sTextLast = strText;

        m_cxyFixedLast = m_cxyFixed;

        if (m_cxyFixedLast.cx == 0 && pInfo->nColumns > 0)
        {
            m_cxyFixedLast.cx = pInfo->rcColumn[pInfo->nColumns - 1].right - pInfo->rcColumn[0].left;
        }

        if (m_cxyFixedLast.cy == 0)
        {
            m_cxyFixedLast.cy = pInfo->uFixedHeight;
        }

        if ((pInfo->uTextStyle & DT_SINGLELINE) != 0)
        {
            if (m_cxyFixedLast.cy == 0)
            {
                m_cxyFixedLast.cy = m_pManager->GetFontInfo(pInfo->nFont)->tm.tmHeight + 8;
                m_cxyFixedLast.cy += pInfo->rcPadding.top + pInfo->rcPadding.bottom;
            }

            if (m_cxyFixedLast.cx == 0)
            {
                RECT rcText = { 0, 0, MAX_CTRL_WIDTH, m_cxyFixedLast.cy };

                if (pInfo->bShowHtml)
                {
                    CRenderEngine::DrawHtmlText(m_pManager->GetPaintDC(), m_pManager, rcText, strText, 0, NULL, NULL, NULL,
                                                pInfo->nFont, DT_CALCRECT | pInfo->uTextStyle & ~DT_RIGHT & ~DT_CENTER);
                }
                else
                {
                    CRenderEngine::DrawText(m_pManager->GetPaintDC(), m_pManager, rcText, strText, 0, pInfo->nFont,
                                            DT_CALCRECT | pInfo->uTextStyle & ~DT_RIGHT & ~DT_CENTER);
                }

                m_cxyFixedLast.cx = rcText.right - rcText.left + pInfo->rcPadding.left + pInfo->rcPadding.right;
            }
        }
        else
        {
            if (m_cxyFixedLast.cx == 0)
            {
                m_cxyFixedLast.cx = szAvailable.cx;
            }

            RECT rcText = { 0, 0, m_cxyFixedLast.cx, MAX_CTRL_WIDTH };
            rcText.left += pInfo->rcPadding.left;
            rcText.right -= pInfo->rcPadding.right;

            if (pInfo->bShowHtml)
            {
                CRenderEngine::DrawHtmlText(m_pManager->GetPaintDC(), m_pManager, rcText, strText, 0, NULL, NULL, NULL,
                                            pInfo->nFont, DT_CALCRECT | pInfo->uTextStyle & ~DT_RIGHT & ~DT_CENTER);
            }
            else
            {
                CRenderEngine::DrawText(m_pManager->GetPaintDC(), m_pManager, rcText, strText, 0, pInfo->nFont,
                                        DT_CALCRECT | pInfo->uTextStyle & ~DT_RIGHT & ~DT_CENTER);
            }

            m_cxyFixedLast.cy = rcText.bottom - rcText.top + pInfo->rcPadding.top + pInfo->rcPadding.bottom;
        }
    }

    return m_cxyFixedLast;
}

void CListTextElementUI::DrawItemText(HDC hDC, const RECT &rcItem)
{
    if (m_pOwner == NULL) { return; }

    TListInfoUI *pInfo = m_pOwner->GetListInfo();

    if (pInfo == NULL) { return; }

    DWORD iTextColor = pInfo->dwTextColor;

    if ((m_uButtonState & UISTATE_HOT) != 0)
    {
        iTextColor = pInfo->dwHotTextColor;
    }

    if (IsSelected())
    {
        iTextColor = pInfo->dwSelectedTextColor;
    }

    if (!IsEnabled())
    {
        iTextColor = pInfo->dwDisabledTextColor;
    }

    IListCallbackUI *pCallback = m_pOwner->GetTextCallback();

    m_nLinks = 0;
    int nLinks = LENGTHOF(m_rcLinks);

    if (pInfo->nColumns > 0)
    {
        for (int i = 0; i < pInfo->nColumns; i++)
        {
            RECT rcItem = { pInfo->rcColumn[i].left, m_rcItem.top, pInfo->rcColumn[i].right, m_rcItem.bottom };

            if (pInfo->iVLineSize > 0 && i < pInfo->nColumns - 1)
            {
                RECT rcLine = { rcItem.right - pInfo->iVLineSize / 2, rcItem.top, rcItem.right - pInfo->iVLineSize / 2, rcItem.bottom};
                CRenderEngine::DrawLine(hDC, rcLine, pInfo->iVLineSize, GetAdjustColor(pInfo->dwVLineColor));
                rcItem.right -= pInfo->iVLineSize;
            }

            CDuiString strText;//不使用LPCTSTR，否则限制太多 by cddjr 2011/10/20
			DWORD dwTextColor = 0;

			//2021-10-04 zm 添加文本和文本色动态修改
			if (pCallback)
			{
				strText = pCallback->GetItemText(this, m_iIndex, i);
				dwTextColor = pCallback->GetItemTextColor(this, m_iIndex, i);
			}

			if (strText.IsEmpty()) strText.Assign(GetText(i));
			if (0 == dwTextColor) dwTextColor = GetTextColor(i);

			//if (pCallback) { strText = pCallback->GetItemText(this, m_iIndex, i); }
	        //else { strText.Assign(GetText(i)); }
			//

			////2021-09-17 zm 增加单元格文本色
			//DWORD dwTextColor = 0;
			//DWORD dwTempTextColor = GetTextColor(i);
			//dwTextColor = (0 != dwTempTextColor) ? dwTempTextColor : iTextColor;

            if (0 == i && pInfo->bCheckBox)//只有首列才能设置checkbox
            {
                CListUI *pListUI = (CListUI *)m_pOwner;
                CListHeaderItemUI *pHeaderItem = (CListHeaderItemUI *)pListUI->GetHeader()->GetItemAt(0);
                RECT rt = pHeaderItem->GetPadding();

				//2021-10-17 zm 同步表头的checkbox
				RECT rcCheck = { 0 };
				if (pInfo->bCheckBox && pListUI && m_pManager)
				{
					rcCheck = pListUI->GetCheckBoxRect(rcItem);

					if (GetCheckBoxState())
					{
						CRenderEngine::DrawImage(hDC, m_pManager, rcCheck, rcItem, pListUI->GetSelImage());
					}
					else
					{
						CRenderEngine::DrawImage(hDC, m_pManager, rcCheck, rcItem, pListUI->GetUnSelImage());
					}
				}

				RECT rcText = rcItem;
				rcText.left += rt.left + (rcCheck.right - rcCheck.left);
				rcText.top += rt.top;
				rcText.right -= rt.right;
				rcText.bottom -= rt.bottom;

				if (pInfo->bShowHtml)
				{
					CRenderEngine::DrawHtmlText(hDC, m_pManager, rcText, strText.GetData(), dwTextColor, NULL, NULL, NULL, pInfo->nFont, pInfo->uTextStyle);
				}
				else
				{
					CRenderEngine::DrawText(hDC, m_pManager, rcText, strText.GetData(), dwTextColor, pInfo->nFont, pInfo->uTextStyle);
				}

				//CDuiString str;

				//if (m_bCheckBoxSelect == false)
				//{
				//	TDrawInfo &di = pListUI->GetUnSelImage();
				//	str.Format(_T("{x %d}{i %s}{x 1}%s "), rt.left, di.sDrawString.GetData(), strText.GetData());
				//}
				//else
				//{
				//	TDrawInfo &di = pListUI->GetSelImage();
				//	str.Format(_T("{x %d}{i %s}{x 1}%s "), rt.left, di.sDrawString.GetData(), strText.GetData());
				//}

				//strText = str.GetData();
				//DWORD dwWriteStype = DT_SINGLELINE | pInfo->uTextStyle;
				//dwWriteStype = (dwWriteStype & (~DT_CENTER)) | DT_LEFT;
				//CRenderEngine::DrawHtmlText(hDC, m_pManager, rcItem, strText.GetData(), dwTextColor,
				//	&m_rcLinks[m_nLinks], &m_sLinks[m_nLinks], &nLinks,
				//	pInfo->nFont, dwWriteStype);
            }
            else
            {
                rcItem.left += pInfo->rcPadding.left;
                rcItem.right -= pInfo->rcPadding.right;
                rcItem.top += pInfo->rcPadding.top;
                rcItem.bottom -= pInfo->rcPadding.bottom;

                if (pInfo->bShowHtml)
					CRenderEngine::DrawHtmlText(hDC, m_pManager, rcItem, strText.GetData(), dwTextColor,
                                                &m_rcLinks[m_nLinks], &m_sLinks[m_nLinks], &nLinks,
                                                pInfo->nFont, pInfo->uTextStyle);
                else
					CRenderEngine::DrawText(hDC, m_pManager, rcItem, strText.GetData(), dwTextColor,
                                            pInfo->nFont, pInfo->uTextStyle);
            }

            m_nLinks += nLinks;
            nLinks = LENGTHOF(m_rcLinks) - m_nLinks;
        }
    }
    else
    {
        RECT rcItem = m_rcItem;
        rcItem.left += pInfo->rcPadding.left;
        rcItem.right -= pInfo->rcPadding.right;
        rcItem.top += pInfo->rcPadding.top;
        rcItem.bottom -= pInfo->rcPadding.bottom;

        CDuiString strText;

		if (pCallback) { strText = pCallback->GetItemText(this, m_iIndex, 0); }
		else if (m_aTexts.GetSize() > 0) { strText.Assign(GetText(0)); }
		else { strText = m_sText; }

		//2021-09-17 zm 增加单元格文本色
		DWORD dwTextColor = GetTextColor(0);
		if (0 != dwTextColor) { iTextColor = dwTextColor; }

        if (pInfo->bShowHtml)
            CRenderEngine::DrawHtmlText(hDC, m_pManager, rcItem, strText.GetData(), iTextColor, \
                                        &m_rcLinks[m_nLinks], &m_sLinks[m_nLinks], &nLinks, pInfo->nFont, pInfo->uTextStyle);
        else
            CRenderEngine::DrawText(hDC, m_pManager, rcItem, strText.GetData(), iTextColor, \
                                    pInfo->nFont, pInfo->uTextStyle);

        m_nLinks += nLinks;
        nLinks = LENGTHOF(m_rcLinks) - m_nLinks;
    }

    for (int i = m_nLinks; i < LENGTHOF(m_rcLinks); i++)
    {
        ::ZeroMemory(m_rcLinks + i, sizeof(RECT));
        ((CDuiString *)(m_sLinks + i))->Empty();
    }
}

void CListTextElementUI::SetCheckBoxState(bool bSelect, int nColumn)
{
    if (m_bCheckBoxSelect == bSelect) { return; }

    m_bCheckBoxSelect = bSelect;
    Invalidate();
}

bool CListTextElementUI::GetCheckBoxState(int nColumn)
{
    return m_bCheckBoxSelect;
}

DWORD CListTextElementUI::GetItemColor(int iIndex) const
{
	if (m_pOwner == NULL) return 0;
	TListInfoUI* pInfo = m_pOwner->GetListInfo();

	if (iIndex < 0 || iIndex >= pInfo->nColumns || m_mapItemColors.GetSize() <= 0) return 0;

	TCHAR acBuffer[16];
	::ZeroMemory(acBuffer, sizeof(acBuffer));
	_itot(iIndex, acBuffer, 10);

	return (DWORD)m_mapItemColors.Find(acBuffer);
}

void CListTextElementUI::SetItemColor(int iIndex, DWORD dwItemColor)
{
	if (m_pOwner == NULL) return;
	TListInfoUI* pInfo = m_pOwner->GetListInfo();

	if (iIndex < 0 || iIndex >= pInfo->nColumns) return;

	TCHAR acBuffer[16];
	::ZeroMemory(acBuffer, sizeof(acBuffer));
	_itot(iIndex, acBuffer, 10);

	m_mapItemColors.Insert(acBuffer, (LPVOID)dwItemColor);
	Invalidate();
}

DWORD CListTextElementUI::GetTextColor(int iIndex) const
{
	if (m_pOwner == NULL) return 0;
	TListInfoUI* pInfo = m_pOwner->GetListInfo();

	if (iIndex < 0 || iIndex >= pInfo->nColumns || m_mapTextColors.GetSize() <= 0) return 0;

	TCHAR acBuffer[16];
	::ZeroMemory(acBuffer, sizeof(acBuffer));
	_itot(iIndex, acBuffer, 10);

	return (DWORD)m_mapTextColors.Find(acBuffer);
}

void CListTextElementUI::SetTextColor(int iIndex, DWORD dwTextColor)
{
	if (m_pOwner == NULL) return;
	TListInfoUI* pInfo = m_pOwner->GetListInfo();

	if (iIndex < 0 || iIndex >= pInfo->nColumns) return;

	TCHAR acBuffer[16];
	::ZeroMemory(acBuffer, sizeof(acBuffer));
	_itot(iIndex, acBuffer, 10);

	m_mapTextColors.Insert(acBuffer, (LPVOID)dwTextColor);
	Invalidate();
}

LPCTSTR CListTextElementUI::GetItemImage(int iIndex) const
{
	if (m_pOwner == NULL) return 0;
	TListInfoUI* pInfo = m_pOwner->GetListInfo();

	if (iIndex < 0 || iIndex >= pInfo->nColumns || m_mapItemImage.GetSize() <= 0) return 0;

	TCHAR acBuffer[16];
	::ZeroMemory(acBuffer, sizeof(acBuffer));
	_itot(iIndex, acBuffer, 10);

	return (LPCTSTR)m_mapItemImage.Find(acBuffer);
}

void CListTextElementUI::SetItemImage(int iIndex, LPCTSTR lpImage)
{
	if (m_pOwner == NULL) return;
	TListInfoUI* pInfo = m_pOwner->GetListInfo();

	if (iIndex < 0 || iIndex >= pInfo->nColumns) return;

	TCHAR acBuffer[16];
	::ZeroMemory(acBuffer, sizeof(acBuffer));
	_itot(iIndex, acBuffer, 10);

	m_mapItemImage.Insert(acBuffer, (LPVOID)lpImage);
	Invalidate();
}

void CListTextElementUI::DrawItemBk(HDC hDC, const RECT& rcItem)
{
	CListElementUI::DrawItemBk(hDC, rcItem);

	if (m_pOwner == NULL) return;
	TListInfoUI* pInfo = m_pOwner->GetListInfo();
	IListCallbackUI *pCallback = m_pOwner->GetTextCallback();

	for (int iIndex = 0; iIndex < pInfo->nColumns; iIndex++)
	{
		RECT rcItem = { pInfo->rcColumn[iIndex].left, m_rcItem.top, pInfo->rcColumn[iIndex].right, m_rcItem.bottom };

		DWORD dwItemColor = 0;
		if (pCallback) dwItemColor = pCallback->GetItemBkColor(this, m_iIndex, iIndex);
		if (0 == dwItemColor) dwItemColor = GetItemColor(iIndex);
		if (0 != dwItemColor) CRenderEngine::DrawColor(hDC, rcItem, GetAdjustColor(dwItemColor));
	}

	//for (int iNum = 0; iNum < m_mapItemColors.GetSize(); iNum++)
	//{
	//	LPCTSTR lpKey = m_mapItemColors.GetAt(iNum);
	//	DWORD dwBkColor = (DWORD)m_mapItemColors.Find(lpKey);
	//	if (0 != dwBkColor)
	//	{
	//		int iIndex = _ttoi(lpKey);
	//		RECT rcItem = { pInfo->rcColumn[iIndex].left, m_rcItem.top, pInfo->rcColumn[iIndex].right, m_rcItem.bottom };

	//		CRenderEngine::DrawColor(hDC, rcItem, GetAdjustColor(dwBkColor));
	//	}
	//}
}

void CListTextElementUI::DrawItemImage(HDC hDC, const RECT &rcItem)
{
	if (m_pOwner == NULL) return;
	TListInfoUI* pInfo = m_pOwner->GetListInfo();
	IListCallbackUI *pCallback = m_pOwner->GetTextCallback();

	for (int iIndex = 0; iIndex < pInfo->nColumns; iIndex++)
	{
		RECT rcItem = { pInfo->rcColumn[iIndex].left, m_rcItem.top, pInfo->rcColumn[iIndex].right, m_rcItem.bottom };

		TDrawInfo stDrawInfo = { 0 };
		if (pCallback) { stDrawInfo.sDrawString = pCallback->GetItemBkImage(this, m_iIndex, iIndex); }
		if (stDrawInfo.sDrawString.IsEmpty()) { stDrawInfo.sDrawString= GetItemImage(iIndex); }
		if (!stDrawInfo.sDrawString.IsEmpty()) { CRenderEngine::DrawImage(hDC, m_pManager, rcItem, rcItem, stDrawInfo); }
	}
}

/////////////////////////////////////////////////////////////////////////////////////
//
//

CListContainerElementUI::CListContainerElementUI() :
    m_iIndex(-1),
    m_iDrawIndex(0),
    m_pOwner(NULL),
    m_bSelected(false),
    m_bExpandable(false),
    m_bExpand(false),
    m_uButtonState(0),
	m_bShiftEnble(false)//zm
{
}

LPCTSTR CListContainerElementUI::GetClass() const
{
    return DUI_CTR_LISTCONTAINERELEMENT;
}

UINT CListContainerElementUI::GetControlFlags() const
{
    return UIFLAG_WANTRETURN;
}

LPVOID CListContainerElementUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcscmp(pstrName, DUI_CTR_ILISTITEM) == 0) { return static_cast<IListItemUI *>(this); }

    if (_tcscmp(pstrName, DUI_CTR_LISTCONTAINERELEMENT) == 0) { return static_cast<CListContainerElementUI *>(this); }

    return CContainerUI::GetInterface(pstrName);
}

IListOwnerUI *CListContainerElementUI::GetOwner()
{
    return m_pOwner;
}

void CListContainerElementUI::SetOwner(CControlUI *pOwner)
{
    if (pOwner != NULL) { m_pOwner = static_cast<IListOwnerUI *>(pOwner->GetInterface(DUI_CTR_ILISTOWNER)); }
}

bool CListContainerElementUI::SetVisible(bool bVisible /*= true*/)
{
    // 2018-08-18 zhuyadong 添加特效
    if (!CContainerUI::SetVisible(bVisible)) { return false; }

    if (!IsVisible() && m_bSelected)
    {
        m_bSelected = false;

        if (m_pOwner != NULL) { m_pOwner->SelectItem(-1); }
    }

    return true;
}

void CListContainerElementUI::SetEnabled(bool bEnable)
{
    CControlUI::SetEnabled(bEnable);

    if (!IsEnabled())
    {
        m_uButtonState = 0;
    }
}

int CListContainerElementUI::GetIndex() const
{
    return m_iIndex;
}

void CListContainerElementUI::SetIndex(int iIndex)
{
    m_iIndex = iIndex;
}

int CListContainerElementUI::GetDrawIndex() const
{
    return m_iDrawIndex;
}

void CListContainerElementUI::SetDrawIndex(int iIndex)
{
    m_iDrawIndex = iIndex;
}

void CListContainerElementUI::Invalidate()
{
    if (!IsVisible()) { return; }

    if (GetParent())
    {
        CContainerUI *pParentContainer = static_cast<CContainerUI *>(GetParent()->GetInterface(DUI_CTR_CONTAINER));

        if (pParentContainer)
        {
            RECT rc = pParentContainer->GetPos();
            RECT rcInset = pParentContainer->GetPadding();
            rc.left += rcInset.left;
            rc.top += rcInset.top;
            rc.right -= rcInset.right;
            rc.bottom -= rcInset.bottom;
            CScrollBarUI *pVerticalScrollBar = pParentContainer->GetVerticalScrollBar();

            if (pVerticalScrollBar && pVerticalScrollBar->IsVisible()) { rc.right -= pVerticalScrollBar->GetFixedWidth(); }

            CScrollBarUI *pHorizontalScrollBar = pParentContainer->GetHorizontalScrollBar();

            if (pHorizontalScrollBar && pHorizontalScrollBar->IsVisible()) { rc.bottom -= pHorizontalScrollBar->GetFixedHeight(); }

            RECT invalidateRc = m_rcItem;

            if (!::IntersectRect(&invalidateRc, &m_rcItem, &rc))
            {
                return;
            }

            CControlUI *pParent = GetParent();
            RECT rcTemp;
            RECT rcParent;

            while (pParent = pParent->GetParent())
            {
                rcTemp = invalidateRc;
                rcParent = pParent->GetPos();

                if (!::IntersectRect(&invalidateRc, &rcTemp, &rcParent))
                {
                    return;
                }
            }

            if (m_pManager != NULL) { m_pManager->Invalidate(invalidateRc); }
        }
        else
        {
            CContainerUI::Invalidate();
        }
    }
    else
    {
        CContainerUI::Invalidate();
    }
}

bool CListContainerElementUI::Activate()
{
    if (!CContainerUI::Activate()) { return false; }

    if (m_pManager != NULL) { m_pManager->SendNotify(this, DUI_MSGTYPE_ITEMACTIVATE); }

    return true;
}

bool CListContainerElementUI::IsSelected() const
{
    return m_bSelected;
}

bool CListContainerElementUI::Select(bool bSelect, bool bTriggerEvent)
{
    if (!IsEnabled()) { return false; }

    if (bSelect == m_bSelected) { return true; }

    m_bSelected = bSelect;

    if (bSelect && m_pOwner != NULL) { m_pOwner->SelectItem(m_iIndex, bTriggerEvent); }

    Invalidate();

    return true;
}

bool CListContainerElementUI::SelectMulti(bool bSelect)
{
	if (!IsEnabled()) { return false; }
	if (bSelect == m_bSelected) { return true; }

	m_bSelected = bSelect;

	if (bSelect && m_pOwner != NULL) m_pOwner->SelectMultiItem(m_iIndex);

	Invalidate();
	return true;
}

bool CListContainerElementUI::IsExpandable() const
{
    return m_bExpandable;
}

void CListContainerElementUI::SetExpandable(bool bExpandable)
{
    m_bExpandable = bExpandable;
}

bool CListContainerElementUI::IsExpanded() const
{
    return m_bExpand;
}

bool CListContainerElementUI::Expand(bool bExpand)
{
    ASSERT(m_pOwner);

    if (m_pOwner == NULL) { return false; }

    if (bExpand == m_bExpand) { return true; }

    m_bExpand = bExpand;

    if (m_bExpandable)
    {
        if (!m_pOwner->ExpandItem(m_iIndex, bExpand)) { return false; }

        if (m_pManager != NULL)
        {
            if (bExpand) { m_pManager->SendNotify(this, DUI_MSGTYPE_ITEMEXPAND, false); }
            else { m_pManager->SendNotify(this, DUI_MSGTYPE_ITEMCOLLAPSE, false); }
        }
    }

    return true;
}

void CListContainerElementUI::DoEvent(TEventUI &event)
{
    if (!IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND)
    {
        if (m_pOwner != NULL) { m_pOwner->DoEvent(event); }
        else { CContainerUI::DoEvent(event); }

        return;
    }

    if (event.Type == UIEVENT_DBLCLICK)
    {
        if (IsEnabled())
        {
            Activate();
            Invalidate();
        }

        return;
    }

    if (event.Type == UIEVENT_KEYDOWN)
    {
        if (IsKeyboardEnabled() && IsEnabled())
        {
            if (event.chKey == VK_RETURN)
            {
                Activate();
                Invalidate();
                return;
            }
        }
    }

    if (event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_RBUTTONDOWN)
    {
        if (IsEnabled())
        {
            SetCapture();

			//2021-10-17 zm 添加键盘多选和鼠标框选
			if (m_pOwner->IsMultiSelect())
			{
				if ((GetKeyState(VK_CONTROL) & 0x8000))
				{
					SelectMulti(!IsSelected());
				}
				else if ((GetKeyState(VK_SHIFT) & 0x8000))
				{
					SetShiftEnble(true);
					m_pOwner->SelectMultiItem(m_iIndex);
				}
				else
				{
					Select();
					m_pOwner->DragBegin(event);
				}

				m_pOwner->DoEvent(event);
			}
			else
			{
				Select();
			}

            Invalidate();
        }

        return;
    }

    // 2018-05-23 单击事件放在鼠标弹起时发送
    if (event.Type == UIEVENT_BUTTONUP || event.Type == UIEVENT_RBUTTONUP)
    {
        if (IsEnabled() && NULL != m_pManager)
        {
            ReleaseCapture();

            if (IsSelected()) { m_pManager->SendNotify(this, DUI_MSGTYPE_ITEMCLICK); }

			//2021-10-17 zm
			if (m_pOwner && m_pOwner->IsMultiSelect())
			{
				if (GetShiftEnble()) { SetShiftEnble(false); }

				m_pOwner->DragEnd(event);

				m_pOwner->DoEvent(event);

			}
        }

        return;
    }

    // 2018-08-28 zhuyadong 解决 List 不支持拖拽源、目的的问题
    if (event.Type == UIEVENT_MOUSEMOVE && (event.wParam & MK_LBUTTON) && m_bDragEnable)
    {
		//2021-10-17 zm
		if (m_pOwner && m_pOwner->IsMultiSelect())
		{
			m_pOwner->Draging(event);

			m_pOwner->DoEvent(event);
		}

        if (IsEnabled()) { OnDoDragDrop(event); }

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

    // An important twist: The list-item will send the event not to its immediate
    // parent but to the "attached" list. A list may actually embed several components
    // in its path to the item, but key-presses etc. needs to go to the actual list.
    if (m_pOwner != NULL) { m_pOwner->DoEvent(event); }
    else { CControlUI::DoEvent(event); }
}

void CListContainerElementUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if (_tcscmp(pstrName, _T("selected")) == 0) { Select(ParseBool(pstrValue)); }
    else if (_tcscmp(pstrName, _T("expandable")) == 0) { SetExpandable(ParseBool(pstrValue)); }
    // 2018-08-28 zhuyadong 解决 List 不支持 拖拽源、目的的问题，去掉属性拦截
    else if (_tcscmp(pstrName, _T("autowidth")) == 0) { DUITRACE(_T("不支持属性:autowidth")); }
    else { CContainerUI::SetAttribute(pstrName, pstrValue); }
}

bool CListContainerElementUI::DoPaint(HDC hDC, const RECT &rcPaint, CControlUI *pStopControl)
{
    DrawItemBk(hDC, m_rcItem);
    return CContainerUI::DoPaint(hDC, rcPaint, pStopControl);
}

void CListContainerElementUI::DrawItemText(HDC hDC, const RECT &rcItem)
{
    return;
}

void CListContainerElementUI::DrawItemBk(HDC hDC, const RECT &rcItem)
{
    ASSERT(m_pOwner);

    if (m_pOwner == NULL) { return; }

    TListInfoUI *pInfo = m_pOwner->GetListInfo();

    if (pInfo == NULL) { return; }

    DWORD iBackColor = 0;

    if (!pInfo->bAlternateBk || m_iDrawIndex % 2 == 0) { iBackColor = pInfo->dwBkColor; }

    if ((m_uButtonState & UISTATE_HOT) != 0)
    {
        iBackColor = pInfo->dwHotBkColor;
    }

    if (IsSelected())
    {
        iBackColor = pInfo->dwSelectedBkColor;
    }

    if (!IsEnabled())
    {
        iBackColor = pInfo->dwDisabledBkColor;
    }

    if (iBackColor != 0)
    {
        CRenderEngine::DrawColor(hDC, m_rcItem, GetAdjustColor(iBackColor));
    }

    if (!IsEnabled())
    {
        if (DrawImage(hDC, pInfo->diDisabled)) { return; }
    }

    if (IsSelected())
    {
        if (DrawImage(hDC, pInfo->diSelected)) { return; }
    }

    if ((m_uButtonState & UISTATE_HOT) != 0)
    {
        if (DrawImage(hDC, pInfo->diHot)) { return; }
    }

    if (!DrawImage(hDC, m_diBk))
    {
        if (!pInfo->bAlternateBk || m_iDrawIndex % 2 == 0)
        {
            if (DrawImage(hDC, pInfo->diBk)) { return; }
        }
    }
}

SIZE CListContainerElementUI::EstimateSize(SIZE szAvailable)
{
    TListInfoUI *pInfo = NULL;

    if (m_pOwner) { pInfo = m_pOwner->GetListInfo(); }

    SIZE cXY = m_cxyFixed;

    if (cXY.cy == 0)
    {
        cXY.cy = pInfo->uFixedHeight;
    }

    return cXY;
}

void CListContainerElementUI::SetCheckBoxState(bool bSelect, int nColumn)
{
    for (int i = 0; i < m_items.GetSize(); ++i)
    {
        CContainerUI *pRoot = dynamic_cast<CContainerUI *>((CControlUI *)m_items[i]);

        // 跳过非布局控件
        if (NULL == pRoot) { continue; }

        // 如果布局控件的子控件数 小于 指定的列号，则跳过
        if (nColumn >= pRoot->GetCount()) { continue; }

        if (CCheckBoxUI *pCtrl = dynamic_cast<CCheckBoxUI *>(pRoot->GetItemAt(nColumn)))
        {
            // nColumn列是复选框
            pCtrl->Selected(bSelect, false);
        }
        else if (pRoot = dynamic_cast<CContainerUI *>(pRoot->GetItemAt(nColumn)))
        {
            // nColumn列是布局，
            CCheckBoxUI *pCtrl = dynamic_cast<CCheckBoxUI *>(m_pManager->FindSubControlByClass(pRoot, DUI_CTR_CHECKBOX));
            pCtrl ? pCtrl->Selected(bSelect, false) : NULL; //lint !e62
        }
    }
}

bool CListContainerElementUI::GetCheckBoxState(int nColumn)
{
    for (int i = 0; i < m_items.GetSize(); ++i)
    {
        CContainerUI *pRoot = dynamic_cast<CContainerUI *>((CControlUI *)m_items[i]);

        // 跳过非布局控件
        if (NULL == pRoot) { continue; }

        // 如果布局控件的子控件数 小于 指定的列号，则跳过
        if (nColumn >= pRoot->GetCount()) { continue; }

        if (CCheckBoxUI *pCtrl = dynamic_cast<CCheckBoxUI *>(pRoot->GetItemAt(nColumn)))
        {
            // nColumn列是复选框
            return pCtrl->IsSelected();
        }
        else if (pRoot = dynamic_cast<CContainerUI *>(pRoot->GetItemAt(nColumn)))
        {
            // nColumn列是布局，
            CCheckBoxUI *pCtrl = dynamic_cast<CCheckBoxUI *>(m_pManager->FindSubControlByClass(pRoot, DUI_CTR_CHECKBOX));
            return pCtrl ? pCtrl->IsSelected() : false;
        }
    }

    return false;
}

void CListContainerElementUI::SetShiftEnble(bool bShiftEnble)
{
	if (m_bShiftEnble == bShiftEnble) { return; }

	m_bShiftEnble = bShiftEnble;
}

bool CListContainerElementUI::GetShiftEnble()
{
	return m_bShiftEnble;
}

/////////////////////////////////////////////////////////////////////////////////////
//
//

CListHBoxElementUI::CListHBoxElementUI()
{

}

LPCTSTR CListHBoxElementUI::GetClass() const
{
    return DUI_CTR_LISTHBOXELEMENT;
}

LPVOID CListHBoxElementUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcscmp(pstrName, DUI_CTR_LISTHBOXELEMENT) == 0) { return static_cast<CListHBoxElementUI *>(this); }

    return CListContainerElementUI::GetInterface(pstrName);
}

void CListHBoxElementUI::SetPos(RECT rc, bool bNeedInvalidate)
{
    if (m_pOwner == NULL) { return CListContainerElementUI::SetPos(rc, bNeedInvalidate); }

    CControlUI::SetPos(rc, bNeedInvalidate);
    rc = m_rcItem;

    TListInfoUI *pInfo = m_pOwner->GetListInfo();

    if (pInfo == NULL) { return; }

    if (pInfo->nColumns > 0)
    {
        int iColumnIndex = 0;

        for (int it2 = 0; it2 < m_items.GetSize(); it2++)
        {
            CControlUI *pControl = static_cast<CControlUI *>(m_items[it2]);

            if (!pControl->IsVisible()) { continue; }

            if (pControl->IsFloat())
            {
                SetFloatPos(it2);
                continue;
            }

            if (iColumnIndex >= pInfo->nColumns) { continue; }

            RECT rcMargin = pControl->GetMargin();
            RECT rcItem = { pInfo->rcColumn[iColumnIndex].left + rcMargin.left, m_rcItem.top + rcMargin.top,
                            pInfo->rcColumn[iColumnIndex].right - rcMargin.right, m_rcItem.bottom - rcMargin.bottom
                          };

            CListHeaderUI *pHeader = ((IListUI *)m_pOwner)->GetHeader();

            if (NULL != pHeader)
            {
                CListHeaderItemUI *pHItem0 = (CListHeaderItemUI *)pHeader->GetItemAt(it2);
                RECT rtInset = pHItem0->GetPadding();
                rcItem.left += rtInset.left;
                rcItem.right -= rtInset.right;
            }

            if (pInfo->iVLineSize > 0 && iColumnIndex < pInfo->nColumns - 1)
            {
                rcItem.right -= pInfo->iVLineSize;
            }

            pControl->SetPos(rcItem, false);
            iColumnIndex += 1;
        }
    }
    else
    {
        for (int it2 = 0; it2 < m_items.GetSize(); it2++)
        {
            CControlUI *pControl = static_cast<CControlUI *>(m_items[it2]);

            if (!pControl->IsVisible()) { continue; }

            if (pControl->IsFloat())
            {
                SetFloatPos(it2);
                continue;
            }

            RECT rcMargin = pControl->GetMargin();
            RECT rcItem = { m_rcItem.left + rcMargin.left, m_rcItem.top + rcMargin.top,
                            m_rcItem.right - rcMargin.right, m_rcItem.bottom - rcMargin.bottom
                          };
            pControl->SetPos(rcItem, false);
        }
    }
}

bool CListHBoxElementUI::DoPaint(HDC hDC, const RECT &rcPaint, CControlUI *pStopControl)
{
    ASSERT(m_pOwner);

    if (m_pOwner == NULL) { return true; }

    TListInfoUI *pInfo = m_pOwner->GetListInfo();

    if (pInfo == NULL) { return true; }

    DrawItemBk(hDC, m_rcItem);

    for (int i = 0; i < pInfo->nColumns; i++)
    {
        RECT rcItem = { pInfo->rcColumn[i].left, m_rcItem.top, pInfo->rcColumn[i].right, m_rcItem.bottom };

        if (pInfo->iVLineSize > 0 && i < pInfo->nColumns - 1)
        {
            RECT rcLine = { rcItem.right - pInfo->iVLineSize / 2, rcItem.top, rcItem.right - pInfo->iVLineSize / 2, rcItem.bottom};
            CRenderEngine::DrawLine(hDC, rcLine, pInfo->iVLineSize, GetAdjustColor(pInfo->dwVLineColor));
        }
    }

    return CContainerUI::DoPaint(hDC, rcPaint, pStopControl);
}

bool CListHBoxElementUI::GetCheckBoxState(int nColumn)
{
    if (nColumn >= m_items.GetSize()) { return false; }

    if (CCheckBoxUI *pCtrl = dynamic_cast<CCheckBoxUI *>((CControlUI *)m_items[nColumn]))
    {
        return pCtrl->IsSelected();
    }
    else if (CContainerUI *pRoot = dynamic_cast<CContainerUI *>((CControlUI *)m_items[nColumn]))
    {
        CCheckBoxUI *pCtrl = dynamic_cast<CCheckBoxUI *>(m_pManager->FindSubControlByClass(pRoot, DUI_CTR_CHECKBOX));
        return pCtrl ? pCtrl->IsSelected() : false;
    }

    return false;
}

void CListHBoxElementUI::SetCheckBoxState(bool bSelect, int nColumn /*= 0*/)
{
    if (nColumn >= m_items.GetSize()) { return; }

    if (CCheckBoxUI *pCtrl = dynamic_cast<CCheckBoxUI *>((CControlUI *)m_items[nColumn]))
    {
        pCtrl->Selected(bSelect, false);
    }
    else if (CContainerUI *pRoot = dynamic_cast<CContainerUI *>((CControlUI *)m_items[nColumn]))
    {
        CCheckBoxUI *pCtrl = dynamic_cast<CCheckBoxUI *>(m_pManager->FindSubControlByClass(pRoot, DUI_CTR_CHECKBOX));
        pCtrl ? pCtrl->Selected(bSelect, false) : NULL; //lint !e62
    }
}

} // namespace DuiLib
