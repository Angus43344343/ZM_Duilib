/** @file CircleProgress.h
* @brief 环型进度条控件，圆环中间可以有文本（如85%）
* @copyright (c) 2019-2022, NetEase Inc. All rights reserved
* @author Xuhuajie
* @date 2019/8/14
* 2021-09-07 zm 移植网易的圆形进度条,xml模型：
* <CircleProgress name="cirproTest" width="100" height="100" min="0" max="100" value="10" showtext="true" circlewidth="10" clockwise="false" indicatorimage="res=&apos;progress\indicator.png&apos;" bkcolor="#ffBFEFFF" forecolor="#ffFFFF00" gradientcolor="#ffff66cc"/>
*/
#pragma once
#include <atltypes.h>

namespace DuiLib {
class DUILIB_API CCircleProgressUI : public CProgressUI
{
public:
	CCircleProgressUI();
	virtual ~CCircleProgressUI();

	virtual LPCTSTR GetClass() const override;
	virtual LPVOID GetInterface(LPCTSTR pstrName) override;

	//设置递增方向：true顺时针， flase为逆时针
	void SetClockwiseRotation(bool bClockwise = true);
	bool IsClockwiseRotation();

	//设置圆环宽度
	void SetCircleWidth(int nCircleWidth);
	int GetCircleWidth();

	//设置进度条渐变色
	void SetGradientColor(DWORD dwGradientColor);
	DWORD GetGradientColor();

	virtual void PaintBkColor(HDC hDc) override;
	virtual void PaintForeColor(HDC hDc) override;
	virtual void PaintStatusImage(HDC hDc) override;

	virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;

private:
	bool m_bClockwise;
	int m_nCircleWidth;
	DWORD m_dwGradientColor;
	TDrawInfo m_stDrawInfo;

#ifdef USE_GDIPLUS
public:
	//设置进度条移动图标
	void SetIndicator(LPCTSTR lpImage);
	Gdiplus::Image* GetIndicator();

	//获取圆形中心
	CPoint GetCenter();

	//获取圆弧坐标
	Gdiplus::RectF GetOuter();

	//解析XML获取图片地址
	void TransImagePath(TDrawInfo& stDrawInfo);

	//转换坐标
	Gdiplus::RectF TransRectF(int iX, int iY, int iWidth, int iHeight);
private:

	Gdiplus::Image*	m_pIndicator;
#endif
};
}


