#ifndef __UIHORIZONTALLAYOUT_H__
#define __UIHORIZONTALLAYOUT_H__

#pragma once

namespace DuiLib {
class DUILIB_API CHorizontalLayoutUI : public CContainerUI
{
public:
    CHorizontalLayoutUI();

    virtual LPCTSTR GetClass() const override;
    virtual LPVOID GetInterface(LPCTSTR pstrName) override;
    virtual UINT GetControlFlags() const override;

    void SetSepWidth(int iWidth);
    int GetSepWidth() const;
    void SetSepImmMode(bool bImmediately);
    bool IsSepImmMode() const;

    virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;
    virtual void DoEvent(TEventUI &event) override;
    virtual void SetPos(RECT rc, bool bNeedInvalidate = true) override;
    virtual void DoPostPaint(HDC hDC, const RECT &rcPaint) override;
    virtual SIZE EstimateSize(SIZE szAvailable) override;

    RECT GetThumbRect(bool bUseNew = false) const;

	//2021-09-08 zm 添加分隔符颜色,默认为黑色
	void SetSepImmColor(DWORD dwSepImmColor);
	DWORD GetSepImmColor();

	virtual void PaintBkColor(HDC hDC) override;

protected:
    int m_iSepWidth;
    UINT m_uButtonState;
    POINT m_ptLastMouse;
    RECT m_rcNewPos;
    bool m_bImmMode;

	DWORD m_dwSepImmColor;//2021-09-08
};
}
#endif // __UIHORIZONTALLAYOUT_H__
