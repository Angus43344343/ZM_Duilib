#ifndef __UIPROGRESS_H__
#define __UIPROGRESS_H__

#pragma once

namespace DuiLib {
class DUILIB_API CProgressUI : public CLabelUI
{
public:
    CProgressUI();

    virtual LPCTSTR GetClass() const override;
    virtual LPVOID GetInterface(LPCTSTR pstrName) override;

    bool IsHorizontal();
    void SetHorizontal(bool bHorizontal = true);
    int GetMinValue() const;
    void SetMinValue(int nMin);
    int GetMaxValue() const;
    void SetMaxValue(int nMax);
    int GetValue() const;
    void SetValue(int nValue);
    LPCTSTR GetForeImage() const;
    void SetForeImage(LPCTSTR pStrImage);
    LPCTSTR GetForeDisabledImage() const;
    void SetForeDisabledImage(LPCTSTR pStrImage);

    virtual void SetEnabled(bool bEnable = true) override;
    virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;
	//2021-09-07 zm 这里使用前景图和前景禁用图来实现没有达到想要的效果
	//修改设计方案：1. 使用背景色和前景色设计
	//			 2. 使用背景图和前景图(状态图)来设计
	//这里重载下面的函数是为了让用户知道使用流程
	virtual void PaintBkColor(HDC hDc) override;
	virtual void PaintForeColor(HDC hDc) override;
	virtual void PaintBkImage(HDC hDC) override;
    virtual void PaintStatusImage(HDC hDC) override;

	void SetShowText(bool bShowText = false);
	bool IsShowText();

	void UpdateText();
	
protected:
    UINT m_uButtonState;
    bool m_bHorizontal;
    int m_nMax;
    int m_nMin;
    int m_nValue;

    TDrawInfo m_diFore;
    TDrawInfo m_diForeDisabled;

	bool m_bShowText;
};

} // namespace DuiLib

#endif // __UIPROGRESS_H__
