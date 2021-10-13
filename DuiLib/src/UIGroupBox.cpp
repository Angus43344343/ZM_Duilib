#include "StdAfx.h"

namespace DuiLib
{
	//////////////////////////////////////////////////////////////////////////
	//
	CGroupBoxUI::CGroupBoxUI()
	: m_uTextStyle(DT_SINGLELINE | DT_VCENTER | DT_CENTER)
	, m_dwTextColor(0)
	, m_dwDisabledTextColor(0)
	, m_iFont(-1)
	{
	}

	CGroupBoxUI::~CGroupBoxUI()
	{
	}

	LPCTSTR CGroupBoxUI::GetClass() const
	{
		return DUI_CTR_GROUPBOX;
	}

	LPVOID CGroupBoxUI::GetInterface(LPCTSTR pstrName)
	{
		if (_tcsicmp(pstrName, DUI_CTR_GROUPBOX) == 0) return static_cast<CGroupBoxUI*>(this);
		return CVerticalLayoutUI::GetInterface(pstrName);
	}

	void CGroupBoxUI::SetTextColor(DWORD dwTextColor)
	{
		m_dwTextColor = dwTextColor;
		Invalidate();
	}

	DWORD CGroupBoxUI::GetTextColor() const
	{
		return m_dwTextColor;
	}

	void CGroupBoxUI::SetDisabledTextColor(DWORD dwTextColor)
	{
		m_dwDisabledTextColor = dwTextColor;
		Invalidate();
	}

	DWORD CGroupBoxUI::GetDisabledTextColor() const
	{
		return m_dwDisabledTextColor;
	}

	void CGroupBoxUI::SetFont(int index)
	{
		m_iFont = index;
		Invalidate();
	}

	int CGroupBoxUI::GetFont() const
	{
		return m_iFont;
	}

	void CGroupBoxUI::PaintText(HDC hDC)
	{
		CDuiString sText = GetText();
		if( sText.IsEmpty() )  { return; }

		DWORD dwBackColor = m_pManager->GetRoot()->GetBkColor();
		if( m_dwTextColor == 0 ) m_dwTextColor = m_pManager->GetDefaultFontColor();
		if( m_dwDisabledTextColor == 0 ) m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor();

		CDuiRect rcText = m_rcItem;
		rcText.Deflate(0,5);//扩展文本矩形的高(top - 5, bottom + 5)

		SIZE szText = CRenderEngine::GetTextSize(m_pManager->GetPaintDC(), m_pManager, sText, m_iFont, DT_CALCRECT | m_uTextStyle);

		//计算文字区域
		rcText.left += m_rcPadding.left + 15;
		rcText.top -= szText.cy / 2 - m_rcPadding.top;
		rcText.right = rcText.left + szText.cx;
		rcText.bottom = rcText.top + szText.cy;

		DWORD dwTextColor = m_dwTextColor;
		if(!IsEnabled()) dwTextColor = m_dwDisabledTextColor;
		CRenderEngine::DrawColor(hDC, rcText, GetAdjustColor(dwBackColor));//绘制文本背景色，遮住边框
		CRenderEngine::DrawText(hDC, m_pManager, rcText, sText, dwTextColor, m_iFont, m_uTextStyle);
	}

	void CGroupBoxUI::PaintBorder(HDC hDC)
	{
		if (0 == m_dwBorderColor || 0 == m_rcBorderSize.left) { return; }

		//调整区域大小
		RECT rcItem = m_rcItem;
		rcItem.top += m_rcPadding.top + 5;//调整上边框位置
		rcItem.bottom -= m_rcPadding.bottom;
		rcItem.left += m_rcPadding.left;
		rcItem.right -= m_rcPadding.right;

		if (m_cxyBorderRound.cx > 0 || m_cxyBorderRound.cy > 0)//画圆角边框
		{
			CRenderEngine::DrawRoundRect(hDC, rcItem, m_rcBorderSize.left, m_cxyBorderRound.cx, m_cxyBorderRound.cy, GetAdjustColor(m_dwBorderColor), m_nBorderStyle);
		}
		else//画直角边框
		{
			CRenderEngine::DrawRect(hDC, rcItem, m_rcBorderSize.left, GetAdjustColor(m_dwBorderColor), m_nBorderStyle);
		}

		PaintText(hDC);
	}

	void CGroupBoxUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcsicmp(pstrName, _T("textcolor")) == 0) { SetTextColor(ParseColor(pstrValue)); }
		else if (_tcsicmp(pstrName, _T("disabledtextcolor")) == 0) { SetDisabledTextColor(ParseColor(pstrValue)); }
		else if (_tcsicmp(pstrName, _T("font")) == 0) { SetFont(ParseInt(pstrValue)); }
		else { CVerticalLayoutUI::SetAttribute(pstrName, pstrValue); }
	}
}
