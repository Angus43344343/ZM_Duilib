/** @file CircleProgress.h
* @brief ���ͽ������ؼ���Բ���м�������ı�����85%��
* @copyright (c) 2019-2022, NetEase Inc. All rights reserved
* @author Xuhuajie
* @date 2019/8/14
* 2021-09-07 zm ��ֲ���׵�Բ�ν�����,xmlģ�ͣ�
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

	//���õ�������true˳ʱ�룬 flaseΪ��ʱ��
	void SetClockwiseRotation(bool bClockwise = true);
	bool IsClockwiseRotation();

	//����Բ�����
	void SetCircleWidth(int nCircleWidth);
	int GetCircleWidth();

	//���ý���������ɫ
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
	//���ý������ƶ�ͼ��
	void SetIndicator(LPCTSTR lpImage);
	Gdiplus::Image* GetIndicator();

	//��ȡԲ������
	CPoint GetCenter();

	//��ȡԲ������
	Gdiplus::RectF GetOuter();

	//����XML��ȡͼƬ��ַ
	void TransImagePath(TDrawInfo& stDrawInfo);

	//ת������
	Gdiplus::RectF TransRectF(int iX, int iY, int iWidth, int iHeight);
private:

	Gdiplus::Image*	m_pIndicator;
#endif
};
}


