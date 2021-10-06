#ifndef __UIDATETIME_H__
#define __UIDATETIME_H__

#pragma once

namespace DuiLib {
class CDateTimeWnd;

/// 时间选择控件
class DUILIB_API CDateTimeUI : public CLabelUI
{
    friend class CDateTimeWnd;
public:
    CDateTimeUI();
    virtual LPCTSTR GetClass() const override;
    virtual LPVOID GetInterface(LPCTSTR pstrName) override;
    virtual UINT GetControlFlags() const override;
    virtual HWND GetNativeWindow() const override;

    SYSTEMTIME &GetTime();
    void SetTime(SYSTEMTIME *pst);

    void SetReadOnly(bool bReadOnly);
    bool IsReadOnly() const;
	//2021-09-07 zm 增加时间格式
	void SetShowTime(bool bShowTime);
	bool IsShowTime() const;

    void UpdateText();

    virtual void SetPos(RECT rc, bool bNeedInvalidate = true) override;
    virtual void Move(SIZE szOffset, bool bNeedInvalidate = true) override;

    virtual void DoEvent(TEventUI &event) override;
    virtual SIZE EstimateSize(SIZE szAvailable) override;
    virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;

	//2021-09-07 zm 初始化时间界面
	virtual void DoInit() override;
protected:
    SYSTEMTIME m_sysTime;
    int        m_nDTUpdateFlag;
    bool       m_bReadOnly;
	bool	   m_bShowTime;//2021-09-07 zm

    CDateTimeWnd *m_pWindow;
};
}
#endif // __UIDATETIME_H__