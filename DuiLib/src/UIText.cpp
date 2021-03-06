#include "StdAfx.h"

namespace DuiLib {
CTextUI::CTextUI() : m_nLinks(0), m_nHoverLink(-1)
{
    m_uTextStyle = DT_WORDBREAK;
    ::ZeroMemory(m_rcLinks, sizeof(m_rcLinks));
}

CTextUI::~CTextUI()
{
}

LPCTSTR CTextUI::GetClass() const
{
    return DUI_CTR_TEXT;
}

LPVOID CTextUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcscmp(pstrName, DUI_CTR_TEXT) == 0) { return static_cast<CTextUI *>(this); }

    return CLabelUI::GetInterface(pstrName);
}

UINT CTextUI::GetControlFlags() const
{
    if (IsEnabled() && m_nLinks > 0) { return UIFLAG_SETCURSOR; }
    else { return 0; }
}

CDuiString *CTextUI::GetLinkContent(int iIndex)
{
    if (iIndex >= 0 && iIndex < m_nLinks) { return &m_sLinks[iIndex]; }

    return NULL;
}

void CTextUI::DoEvent(TEventUI &event)
{
    if (!IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND)
    {
        if (m_pParent != NULL) { m_pParent->DoEvent(event); }
        else { CLabelUI::DoEvent(event); }

        return;
    }

    if (event.Type == UIEVENT_SETCURSOR)
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

    if ((event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_LBUTTONDBLDOWN) && IsEnabled())
    {
        SetCapture();

        for (int i = 0; i < m_nLinks; i++)
        {
            if (::PtInRect(&m_rcLinks[i], event.ptMouse))
            {
                Invalidate();
                return;
            }
        }
    }

    if (event.Type == UIEVENT_BUTTONUP && IsEnabled())
    {
        ReleaseCapture();

        for (int i = 0; i < m_nLinks; i++)
        {
            if (::PtInRect(&m_rcLinks[i], event.ptMouse) && NULL != m_pManager)
            {
                m_pManager->SendNotify(this, DUI_MSGTYPE_LINK, i);
                return;
            }
        }
    }

    if (event.Type == UIEVENT_CONTEXTMENU && IsEnabled())
    {
        ReleaseCapture();

        //2017-02-25 zhuyadong 消息传递给父控件，用于复杂组合控件的上下文菜单响应。
        if (m_pParent != NULL) { m_pParent->DoEvent(event); }

        return;
    }

    if (event.Type == UIEVENT_MOUSEMOVE)
    {
        // When you move over a link
        if (m_nLinks > 0 && IsEnabled())
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
                m_nHoverLink = nHoverLink;
                Invalidate();
                return;
            }
        }
    }

    if (event.Type == UIEVENT_MOUSELEAVE)
    {
        if (m_nLinks > 0 && IsEnabled())
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

    CLabelUI::DoEvent(event);
}

void CTextUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if (_tcscmp(pstrName, _T("dragenable")) == 0) { DUITRACE(_T("不支持属性:dragenable")); }
    else if (_tcscmp(pstrName, _T("dragimage")) == 0) { DUITRACE(_T("不支持属性:drageimage")); }
    else { CLabelUI::SetAttribute(pstrName, pstrValue); }
}

void CTextUI::PaintText(HDC hDC)
{
	m_nLinks = LENGTHOF(m_rcLinks);
	CLabelUI::PaintText(hDC);
     //if (m_sText.IsEmpty())
     //{
     //    m_nLinks = 0;
     //    return;
     //}
    
     //if (m_dwTextColor == 0) { m_dwTextColor = m_pManager->GetDefaultFontColor(); }
    
     //if (m_dwDisabledTextColor == 0) { m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor(); }
    
     //m_nLinks = LENGTHOF(m_rcLinks);
     //RECT rc = m_rcItem;
     //rc.left += m_rcTextPadding.left;
     //rc.right -= m_rcTextPadding.right;
     //rc.top += m_rcTextPadding.top;
     //rc.bottom -= m_rcTextPadding.bottom;
    
     //if (IsEnabled())
     //{
     //       if (m_bShowHtml)
     //           CRenderEngine::DrawHtmlText(hDC, m_pManager, rc, m_sText, m_dwTextColor,
     //                                       GetRectLinks(), GetStringLinks(), GetLinksNum(),
     //                                       m_iFont, m_uTextStyle);
     //       else
     //       {
     //           CRenderEngine::DrawText(hDC, m_pManager, rc, m_sText, m_dwTextColor,
     //                                   m_iFont, m_uTextStyle);
     //       }
     //}
     //else
     //{
     //    if (m_bShowHtml)
     //        CRenderEngine::DrawHtmlText(hDC, m_pManager, rc, m_sText, m_dwDisabledTextColor, \
     //                                    m_rcLinks, m_sLinks, m_nLinks, m_iFont, m_uTextStyle);
     //    else
     //        CRenderEngine::DrawText(hDC, m_pManager, rc, m_sText, m_dwDisabledTextColor, \
     //                                m_iFont, m_uTextStyle);
     //}
}

int *CTextUI::GetLinksNum(void)
{
    return &m_nLinks;
}

RECT *CTextUI::GetRectLinks(void)
{
    return m_rcLinks;
}

DuiLib::CDuiString *CTextUI::GetStringLinks(void)
{
    return m_sLinks;
}

}
