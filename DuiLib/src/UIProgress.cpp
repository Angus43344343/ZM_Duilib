﻿#include "stdafx.h"

namespace DuiLib {
	CProgressUI::CProgressUI() : m_uButtonState(0), m_bHorizontal(true), m_nMin(0), m_nMax(100), m_nValue(0), m_bShowText(false)
{
    m_uTextStyle = DT_SINGLELINE | DT_CENTER | DT_VCENTER;//zm
    SetFixedHeight(12);
}

LPCTSTR CProgressUI::GetClass() const
{
    return DUI_CTR_PROGRESS;
}

LPVOID CProgressUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcscmp(pstrName, DUI_CTR_PROGRESS) == 0) { return static_cast<CProgressUI *>(this); }

    return CLabelUI::GetInterface(pstrName);
}

bool CProgressUI::IsHorizontal()
{
    return m_bHorizontal;
}

void CProgressUI::SetHorizontal(bool bHorizontal)
{
    if (m_bHorizontal == bHorizontal) { return; }

    m_bHorizontal = bHorizontal;
    Invalidate();
}

int CProgressUI::GetMinValue() const
{
    return m_nMin;
}

void CProgressUI::SetMinValue(int nMin)
{
    m_nMin = nMin;
    Invalidate();
}

int CProgressUI::GetMaxValue() const
{
    return m_nMax;
}

void CProgressUI::SetMaxValue(int nMax)
{
    m_nMax = nMax;
    Invalidate();
}

int CProgressUI::GetValue() const
{
    return m_nValue;
}

void CProgressUI::SetValue(int nValue)
{
    m_nValue = nValue;

    if (m_nValue > m_nMax) { m_nValue = m_nMax; }

    if (m_nValue < m_nMin) { m_nValue = m_nMin; }

    Invalidate();
	UpdateText();
}

LPCTSTR CProgressUI::GetForeImage() const
{
    return m_diFore.sDrawString;
}

void CProgressUI::SetForeImage(LPCTSTR pStrImage)
{
    if (m_diFore.sDrawString == pStrImage && m_diFore.pImageInfo != NULL) { return; }

    m_diFore.Clear();
    m_diFore.sDrawString = pStrImage;
    Invalidate();
}

//LPCTSTR CProgressUI::GetForeDisabledImage() const
//{
//    return m_diForeDisabled.sDrawString;
//}
//
//void CProgressUI::SetForeDisabledImage(LPCTSTR pStrImage)
//{
//    if (m_diForeDisabled.sDrawString == pStrImage && m_diForeDisabled.pImageInfo != NULL) { return; }
//
//    m_diForeDisabled.Clear();
//    m_diForeDisabled.sDrawString = pStrImage;
//    Invalidate();
//}

void CProgressUI::SetEnabled(bool bEnable /*= true*/)
{
    CControlUI::SetEnabled(bEnable);

    if (!IsEnabled()) { m_uButtonState = 0; }
}

void CProgressUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if (_tcscmp(pstrName, _T("foreimage")) == 0) { SetForeImage(ParseString(pstrValue)); }
	else if (_tcscmp(pstrName, _T("showtext")) == 0) { SetShowText(ParseBool(pstrValue)); }//zm
    //else if (_tcscmp(pstrName, _T("foredisabledimage")) == 0) { SetForeDisabledImage(ParseString(pstrValue)); }
    else if (_tcscmp(pstrName, _T("hor")) == 0) { SetHorizontal(ParseBool(pstrValue)); }
    else if (_tcscmp(pstrName, _T("min")) == 0) { SetMinValue(ParseInt(pstrValue)); }
    else if (_tcscmp(pstrName, _T("max")) == 0) { SetMaxValue(ParseInt(pstrValue)); }
    else if (_tcscmp(pstrName, _T("value")) == 0) { SetValue(ParseInt(pstrValue)); }
    else if (_tcscmp(pstrName, _T("dragenable")) == 0) { DUITRACE(_T("不支持属性:dragenable")); }
    else if (_tcscmp(pstrName, _T("dragimage")) == 0) { DUITRACE(_T("不支持属性:drageimage")); }
    else if (_tcscmp(pstrName, _T("dropenable")) == 0) { DUITRACE(_T("不支持属性:dropenable")); }
    else if (_tcscmp(pstrName, _T("autowidth")) == 0) { DUITRACE(_T("不支持属性:autowidth")); }
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
    else { CLabelUI::SetAttribute(pstrName, pstrValue); }
}

void CProgressUI::PaintBkColor(HDC hDc)
{
	CLabelUI::PaintBkColor(hDc);
}

void CProgressUI::PaintForeColor(HDC hDc)
{
	if (m_nMax <= m_nMin) { m_nMax = m_nMin + 1; }

	if (m_nValue > m_nMax) { m_nValue = m_nMax; }

	if (m_nValue < m_nMin) { m_nValue = m_nMin; }

	RECT rc = m_rcItem;

	if (m_bHorizontal)
	{
		rc.right = m_rcItem.left + (m_nValue - m_nMin) * (m_rcItem.right - m_rcItem.left) / (m_nMax - m_nMin);
	}
	else
	{
		rc.top = m_rcItem.top + (m_rcItem.bottom - m_rcItem.top) * (m_nMax - m_nValue) / (m_nMax - m_nMin);
	}

	CRenderEngine::DrawColor(hDc, rc, GetAdjustColor(m_dwForeColor));
}

void CProgressUI::PaintBkImage(HDC hDC)
{
	CLabelUI::PaintBkImage(hDC);
}

void CProgressUI::PaintStatusImage(HDC hDC)
{
    if (m_nMax <= m_nMin) { m_nMax = m_nMin + 1; }

    if (m_nValue > m_nMax) { m_nValue = m_nMax; }

    if (m_nValue < m_nMin) { m_nValue = m_nMin; }

    RECT rc = {0};

    if (m_bHorizontal)
    {
        rc.right = (m_nValue - m_nMin) * (m_rcItem.right - m_rcItem.left) / (m_nMax - m_nMin);
        rc.bottom = m_rcItem.bottom - m_rcItem.top;
    }
    else
    {
        rc.top = (m_rcItem.bottom - m_rcItem.top) * (m_nMax - m_nValue) / (m_nMax - m_nMin);
        rc.right = m_rcItem.right - m_rcItem.left;
        rc.bottom = m_rcItem.bottom - m_rcItem.top;
    }

	m_diFore.rcDestOffset = rc;
	DrawImage(hDC, m_diFore);
}

void CProgressUI::SetShowText(bool bShowText)
{
	if (m_bShowText == bShowText) return;

	m_bShowText = bShowText;

	if (!m_bShowText) SetText(_T(""));
	else UpdateText();
}

bool CProgressUI::IsShowText()
{
	return m_bShowText;
}

void CProgressUI::UpdateText()
{
	if (m_bShowText)
	{
		CDuiString sText;
		sText.Format(_T("%.0f%%"), (m_nValue - m_nMin) * 100.0f / (m_nMax - m_nMin));
		SetText(sText);
	}
}

}
