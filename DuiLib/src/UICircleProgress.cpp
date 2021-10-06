#include "stdafx.h"
#include <shlwapi.h>

namespace DuiLib{
CCircleProgressUI::CCircleProgressUI() 
	: m_bClockwise(true)
	, m_nCircleWidth(5)
	, m_dwGradientColor(0)
	, m_pIndicator(nullptr)
{
}

CCircleProgressUI::~CCircleProgressUI()
{
	if (m_pIndicator)
	{
		delete m_pIndicator;
		m_pIndicator = nullptr;
	}
}

LPCTSTR CCircleProgressUI::GetClass() const
{
	return DUI_CTR_CIRCLEPROGRESS;
}

LPVOID CCircleProgressUI::GetInterface(LPCTSTR pstrName)
{
	if (_tcscmp(pstrName, DUI_CTR_CIRCLEPROGRESS) == 0) { return static_cast<CCircleProgressUI *>(this); }

	return CProgressUI::GetInterface(pstrName);
}

void CCircleProgressUI::SetClockwiseRotation(bool bClockwise)
{
	if (m_bClockwise == bClockwise) { return; }

	m_bClockwise = bClockwise;
	Invalidate();
}

bool CCircleProgressUI::IsClockwiseRotation()
{
	return m_bClockwise;
}

void CCircleProgressUI::SetCircleWidth(int nCircleWidth)
{
	m_nCircleWidth = nCircleWidth;
	Invalidate();
}

int CCircleProgressUI::GetCircleWidth()
{
	return m_nCircleWidth;
}

void CCircleProgressUI::SetGradientColor(DWORD dwGradientColor)
{
	if (m_dwGradientColor == dwGradientColor) { return; }

	m_dwGradientColor = dwGradientColor;
	Invalidate();
}

DWORD CCircleProgressUI::GetGradientColor()
{
	return m_dwGradientColor;
}

void CCircleProgressUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	if (_tcscmp(pstrName, _T("indicatorimage")) == 0) { SetIndicator(ParseString(pstrValue)); }
	else if (_tcscmp(pstrName, _T("circlewidth")) == 0) { SetCircleWidth(ParseInt(pstrValue)); }
	else if (_tcscmp(pstrName, _T("clockwise")) == 0) { SetClockwiseRotation(ParseBool(pstrValue)); }
	else if (_tcscmp(pstrName, _T("gradientcolor")) == 0) { SetGradientColor(ParseColor(pstrValue)); }
	else { CProgressUI::SetAttribute(pstrName, pstrValue); }
}

#ifdef USE_GDIPLUS
void CCircleProgressUI::SetIndicator(LPCTSTR lpImage)
{
	if (m_stDrawInfo.sDrawString == lpImage && m_stDrawInfo.pImageInfo != NULL) { return; }

	m_stDrawInfo.Clear();
	m_stDrawInfo.sDrawString = lpImage;

	TransImagePath(m_stDrawInfo);//解析xml获取图片地址
	CDuiString strImagepath = CPaintManagerUI::GetResourcePath() + m_stDrawInfo.sImageName;

	if (!::PathFileExists(strImagepath)) return;
	if (m_pIndicator) delete m_pIndicator;

	m_pIndicator = new Gdiplus::Image(strImagepath);
	if (Gdiplus::Ok == m_pIndicator->GetLastStatus())
	{	
		m_pIndicator->RotateFlip(m_bClockwise ? Gdiplus::Rotate90FlipNone : Gdiplus::Rotate270FlipNone);// 假定图片指向上
		Invalidate();
	}
}

Gdiplus::Image* CCircleProgressUI::GetIndicator()
{
	return m_pIndicator;
}

CPoint CCircleProgressUI::GetCenter()
{
	CPoint stCenter;
	stCenter.x = m_rcItem.left + (m_rcItem.right - m_rcItem.left) / 2;
	stCenter.y = m_rcItem.top + (m_rcItem.bottom - m_rcItem.top) / 2;

	return stCenter;
}

Gdiplus::RectF CCircleProgressUI::GetOuter()
{		
	// 控件矩形内的最大正方形的边界
	int nSide = min(m_rcItem.right - m_rcItem.left, m_rcItem.bottom - m_rcItem.top);
	if (m_nCircleWidth > nSide) m_nCircleWidth = nSide;

	//UiRect rcBorder;  仍然存在UiRect 到 RectF的转换，所以直接用gdi的RectF了
	CPoint stCenter = GetCenter();
	Gdiplus::RectF rectfOuter = TransRectF(stCenter.x - nSide / 2, stCenter.y - nSide / 2, nSide, nSide);

	if (m_pIndicator) 
	{
		rectfOuter.Inflate((Gdiplus::REAL)(-1.0F *m_pIndicator->GetWidth() / 2), (Gdiplus::REAL)(-1.0F * m_pIndicator->GetWidth() / 2));
	}
	else
	{
		rectfOuter.Inflate((Gdiplus::REAL)(-0.5 * m_nCircleWidth), (Gdiplus::REAL)(-0.5 * m_nCircleWidth));
	}
	rectfOuter.Inflate(-1, -1);

	return rectfOuter;
}

void CCircleProgressUI::TransImagePath(TDrawInfo& stDrawInfo)
{
	DWORD dwMask = 0;
	bool bHSL = false;
	CDuiString sResType;
	CRenderEngine::ParseDrawInfo(stDrawInfo, sResType, dwMask, bHSL);
}

Gdiplus::RectF CCircleProgressUI::TransRectF(int iX, int iY, int iWidth, int iHeight)
{
	Gdiplus::RectF stRectF;
	stRectF.X = (Gdiplus::REAL)iX;
	stRectF.Y = (Gdiplus::REAL)iY;
	stRectF.Width = (Gdiplus::REAL)iWidth;
	stRectF.Height = (Gdiplus::REAL)iHeight;

	return stRectF;
}

void CCircleProgressUI::PaintBkColor(HDC hDc)
{
	Gdiplus::Graphics graphics(hDc);
	graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
	Gdiplus::Pen bgPen(m_dwBackColor, (Gdiplus::REAL)m_nCircleWidth);

	graphics.DrawArc(&bgPen, GetOuter(), 270, 360);   //270从最上面开始递增，设为0的话，是最右边开始
}

void CCircleProgressUI::PaintForeColor(HDC hDc)
{
	Gdiplus::Graphics graphics(hDc);
	graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
	int nDirection = m_bClockwise ? 1 : -1;   //旋转方向

	if (0 == m_dwGradientColor)
	{
		Gdiplus::Pen fgPen(m_dwForeColor, (Gdiplus::REAL)m_nCircleWidth);
		graphics.DrawArc(&fgPen, GetOuter(), 270, (Gdiplus::REAL)(nDirection * 360 * (m_nValue - m_nMin) / (m_nMax - m_nMin)));
	}
	else//画渐变色
	{
		Gdiplus::REAL factors[4] = { 0.0f, 0.4f, 0.6f, 1.0f };
		Gdiplus::REAL positions[4] = { 0.0f, 0.2f, 0.8f, 1.0f };

		int nSide = min(m_rcItem.right - m_rcItem.left, m_rcItem.bottom - m_rcItem.top);
		if (m_nCircleWidth > nSide) m_nCircleWidth = nSide;
		Gdiplus::RectF rectfBorder = TransRectF(GetCenter().x - nSide / 2, GetCenter().y - nSide / 2, nSide, nSide);

		Gdiplus::LinearGradientBrush lgbrush(rectfBorder, m_dwForeColor, m_dwGradientColor, Gdiplus::LinearGradientModeVertical);
		lgbrush.SetBlend(factors, positions, 4);
		Gdiplus::Pen fgPen(&lgbrush, (Gdiplus::REAL)m_nCircleWidth);
		graphics.DrawArc(&fgPen, GetOuter(), 270, (Gdiplus::REAL)(nDirection * 360 * (m_nValue - m_nMin) / (m_nMax - m_nMin)));
	}
}

void CCircleProgressUI::PaintStatusImage(HDC hDc)
{
	if (nullptr == m_pIndicator) return;

	//画旋转指示器图标，需要用到矩阵
	Gdiplus::Graphics graphics(hDc);
	graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

	int nBordersize = 1;
	int nDirection = m_bClockwise ? 1 : -1;   //旋转方向

	Gdiplus::Matrix matrix;
	Gdiplus::PointF stPointF((Gdiplus::REAL)GetCenter().x, (Gdiplus::REAL)GetCenter().y);
	matrix.RotateAt((Gdiplus::REAL)(nDirection * 360 * (m_nValue - m_nMin) / (m_nMax - m_nMin)), stPointF, Gdiplus::MatrixOrderAppend);
	graphics.SetTransform(&matrix);

	Gdiplus::RectF rectf = TransRectF(GetCenter().x - m_pIndicator->GetWidth() / 2, 
										(int)(GetOuter().Y + nBordersize / 2 - m_pIndicator->GetHeight() / 2),
										m_pIndicator->GetWidth(),
										m_pIndicator->GetHeight());

	graphics.DrawImage(m_pIndicator, rectf);
}
#endif
}
