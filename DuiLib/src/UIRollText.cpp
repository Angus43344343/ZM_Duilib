#include "StdAfx.h"

namespace DuiLib
{
	CRollTextUI::CRollTextUI(void)
	{
		m_nScrollPos = 0;
		m_nText_W_H = 0;
		m_nStep = 5;
		m_bUseRoll = FALSE;
		m_nRollDirection = ROLLTEXT_LEFT;

		m_nRollState = 0;
		m_uiTimeSpan = ROLLTEXT_TIMERID_SPAN;
		m_uiTimeEndSpan = ROLLTEXT_ROLL_END_SPAN;

		m_uTextStyle = DT_VCENTER;
		m_iFont = -1;
		m_bTimeRoll = TRUE;
	}

	CRollTextUI::~CRollTextUI(void)
	{
		EndRoll();
	}

	LPCTSTR CRollTextUI::GetClass() const
	{
		return DUI_CTR_ROLLTEXT;
	}

	LPVOID CRollTextUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcsicmp(pstrName, DUI_CTR_ROLLTEXT) == 0 ) return static_cast<CRollTextUI*>(this);
		return CContainerUI::GetInterface(pstrName);
	}

	void CRollTextUI::SetFont(int index)
	{
		m_iFont = index;
		Invalidate();
	}

	int CRollTextUI::GetFont() const
	{
		return m_iFont;
	}

	BOOL CRollTextUI::IsUseRoll()
	{
		return m_bUseRoll;
	}

	void CRollTextUI::SetUseRoll(BOOL bUseRoll)
	{
		if (m_bUseRoll == bUseRoll) { return; }

		m_bUseRoll = bUseRoll;
		IsVisible();
	}

	int CRollTextUI::GetRollDirection()
	{
		return m_nRollDirection;
	}

	void CRollTextUI::SetRollDirection(int iRollDirction)
	{
		if (m_nRollDirection == iRollDirction) { return; }

		m_nRollDirection = iRollDirction;
		Invalidate();
	}

	UINT CRollTextUI::GetTimeSpan()
	{
		return m_uiTimeSpan;
	}

	void CRollTextUI::SetTimeSpan(UINT uiTimeSpan)
	{
		m_uiTimeSpan = uiTimeSpan;
		m_nRollState |= ROLLTEXT_TIMERID_STATE;
		
		m_pManager->KillTimer(this, ROLLTEXT_TIMERID);
		m_pManager->SetTimer(this, ROLLTEXT_TIMERID, m_uiTimeSpan);
	}

	UINT CRollTextUI::GetTimeEndSpan()
	{
		return m_uiTimeEndSpan;
	}

	void CRollTextUI::SetTimeEndSpan(UINT uiTimeEndSpan)
	{
		m_uiTimeEndSpan = uiTimeEndSpan;
		m_nRollState |= ROLLTEXT_ROLL_END_STATE;

		m_pManager->KillTimer(this, ROLLTEXT_ROLL_END);
		m_pManager->SetTimer(this, ROLLTEXT_ROLL_END, m_uiTimeEndSpan);
	}

	void CRollTextUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcscmp(pstrName, _T("roll")) == 0) { SetUseRoll(ParseBool(pstrValue)); }
		else if (_tcsicmp(pstrName, _T("direction")) == 0) { SetRollDirection(ParseInt(pstrValue)); }
		else if (_tcsicmp(pstrName, _T("timespan")) == 0) { SetTimeSpan(ParseInt(pstrValue)); }
		else if (_tcsicmp(pstrName, _T("timeendspan")) == 0) { SetTimeEndSpan(ParseInt(pstrValue)); }
		else if (_tcsicmp(pstrName, _T("font")) == 0) { SetFont(ParseInt(pstrValue)); }
		else __super::SetAttribute(pstrName, pstrValue);
	}

	void CRollTextUI::BeginRoll(LONG lTimeSpan, LONG lMaxTimeLimited)
	{
		if (!m_bUseRoll) { return; }

		EndRoll();
		m_nText_W_H = 0;
			
		if (m_nRollState & ROLLTEXT_TIMERID_STATE)
		{
			m_pManager->SetTimer(this, ROLLTEXT_TIMERID, lTimeSpan);
		}

		if (m_nRollState & ROLLTEXT_ROLL_END_STATE)
		{
			m_pManager->SetTimer(this, ROLLTEXT_ROLL_END, lMaxTimeLimited);
		}
	}

	void CRollTextUI::EndRoll()
	{
		if (!m_bUseRoll) { return; }

		m_pManager->KillTimer(this, ROLLTEXT_ROLL_END);
		m_pManager->KillTimer(this, ROLLTEXT_TIMERID);
		
		Invalidate();
	}

	void CRollTextUI::SetText( LPCTSTR pstrText )
	{
		CContainerUI::SetText(pstrText);
		m_nText_W_H = 0;			//文本变化重新计算
	}

	void CRollTextUI::DoEvent(TEventUI& event)
	{
		if (!IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND)
		{
			if (m_pParent != NULL) { m_pParent->DoEvent(event); }
			else { CContainerUI::DoEvent(event); }

			return;
		}

		if (event.Type == UIEVENT_TIMER && event.wParam == ROLLTEXT_ROLL_END)
		{
			m_pManager->KillTimer(this, ROLLTEXT_ROLL_END);
			m_pManager->SendNotify(this, DUI_MSGTYPE_TEXTROLLEND);
		}

		if( event.Type == UIEVENT_TIMER && event.wParam == ROLLTEXT_TIMERID ) 
		{
			m_bTimeRoll = TRUE;
			Invalidate();

			return;
		}

		CContainerUI::DoEvent(event);
	}

	void CRollTextUI::GetDrawItem(HDC hDC, UINT& uiTextStyle, RECT& rcItem)
	{
		if (!m_bUseRoll || rcItem.right == rcItem.left ) { return; }//防止被除数为0

		SIZE szText = CRenderEngine::GetTextSize(hDC, m_pManager, m_sText, m_iFont, m_uTextStyle | DT_CALCRECT);

		if (m_nText_W_H > 0)
		{
			int nScrollRange = 0;
			CDuiRect duircItem = rcItem;

			if (m_nRollDirection == ROLLTEXT_LEFT || m_nRollDirection == ROLLTEXT_RIGHT)
			{	//左右移动
				nScrollRange = m_nText_W_H  + duircItem.GetWidth();

				duircItem.Offset((m_nRollDirection == ROLLTEXT_LEFT ? duircItem.GetWidth() : -duircItem.GetWidth()), 0);
				duircItem.Offset((m_nRollDirection == ROLLTEXT_LEFT ? -m_nScrollPos : m_nScrollPos), 0);

				if (m_nRollDirection == ROLLTEXT_LEFT) 
				{ duircItem.right += (m_nText_W_H - duircItem.GetWidth()); }
				else
				{ duircItem.left += (-m_nText_W_H + duircItem.GetWidth()); }
					
				duircItem.top += (duircItem.GetHeight() - szText.cy) / 2;//垂直居中显示
			}
			else
			{	//上下移动
				uiTextStyle |= DT_WORDBREAK;//允许多行显示
				nScrollRange = m_nText_W_H + duircItem.GetHeight();

				duircItem.Offset(0, (m_nRollDirection == ROLLTEXT_UP ? duircItem.GetHeight() : -duircItem.GetHeight()));
				duircItem.Offset(0, (m_nRollDirection == ROLLTEXT_UP ? -m_nScrollPos : m_nScrollPos));

				if (m_nRollDirection == ROLLTEXT_UP) 
				{ duircItem.bottom += (m_nText_W_H - duircItem.GetHeight()); }
				else
				{ duircItem.top += (-m_nText_W_H + duircItem.GetHeight()); }
			}

			//2021-10-12 zm解决使用ActiveX加载多媒体,会造成当前页面的重绘,导致timespan无效的问题
			if (m_bTimeRoll) m_nScrollPos += m_nStep;
			if (m_nScrollPos > nScrollRange) { m_nScrollPos = 0; } //重新开始滚动

			rcItem = duircItem;
		}
		else if (0 == m_nText_W_H)//第一次计算文本
		{
			if (m_nRollDirection == ROLLTEXT_LEFT || m_nRollDirection == ROLLTEXT_RIGHT)
			{
				m_nText_W_H = szText.cx;
			}
			else
			{
				m_nText_W_H = (szText.cx / (rcItem.right - rcItem.left) + 1) * szText.cy;//多行时,计算文本的高度
			}
		}

		m_bTimeRoll = FALSE;
	}

	void CRollTextUI::PaintText(HDC hDC)
	{
		if (m_sText.IsEmpty()) { return; }

		if (m_dwTextColor == 0) { m_dwTextColor = m_pManager->GetDefaultFontColor(); }
		if (m_dwDisabledTextColor == 0) { m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor(); }

		RECT rc = m_rcItem;
		rc.left += (m_rcBorderSize.left + m_rcPadding.left);
		rc.right -= (m_rcBorderSize.right + m_rcPadding.right);
		rc.top += (m_rcBorderSize.top + m_rcPadding.top);
		rc.bottom -= (m_rcBorderSize.bottom + m_rcPadding.bottom);

		RECT rcDst = { 0 };
		GetDrawItem(hDC, m_uTextStyle, rc);
		if (!::IntersectRect(&rcDst, &rc, &m_rcItem)) return;

		if (IsEnabled())
		{
			CRenderEngine::DrawText(hDC, m_pManager, rc, m_sText, m_dwTextColor, m_iFont, m_uTextStyle);
		}
		else
		{
			CRenderEngine::DrawText(hDC, m_pManager, rc, m_sText, m_dwDisabledTextColor, m_iFont, m_uTextStyle);
		}	
	}
}