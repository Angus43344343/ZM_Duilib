﻿#ifndef __UIEDIT_H__
#define __UIEDIT_H__

#pragma once

template <class CHART> class CRegexpT;

namespace DuiLib {
enum EMFilterType
{
    EFILTER_CHAR_UPPER      = 0x01,     // 大写英文字符
    EFILTER_CHAR_LOWER      = 0x02,     // 小写英文字符
    EFILTER_CHAR_BLANK      = 0x04,     // 英文空格
    EFILTER_SPECIAL_SYMBOL  = 0x08,     // 特殊字符
    EFILTER_CHAR_LINUX      = 0x10,     // Linux文件操作过滤字符 黑名单模式
    EFILTER_NUMBER_BIN      = 0x20,     // 二进制数字
    EFILTER_NUMBER_DEC      = 0x40,     // 十进制数字
    EFILTER_NUMBER_HEX      = 0x80,     // 十六进制数字
};


class CEditWnd;

class DUILIB_API CEditUI : public CLabelUI
{
    friend class CEditWnd;
public:
    CEditUI();
    virtual ~CEditUI();

    virtual LPCTSTR GetClass() const override;
    virtual LPVOID GetInterface(LPCTSTR pstrName) override;
    virtual UINT GetControlFlags() const override;
    virtual HWND GetNativeWindow() const override;

    virtual void SetEnabled(bool bEnable = true) override;
    virtual void SetText(LPCTSTR pstrText) override;
    void SetMaxChar(UINT uMax);
    UINT GetMaxChar();
    void SetReadOnly(bool bReadOnly);
    bool IsReadOnly() const;
    void SetPasswordMode(bool bPasswordMode);
    bool IsPasswordMode() const;
    void SetPasswordChar(TCHAR cPasswordChar);
    TCHAR GetPasswordChar() const;
    bool IsAutoSelAll();
    void SetAutoSelAll(bool bAutoSelAll);
    void SetNumberOnly(bool bNumberOnly);
    bool IsNumberOnly() const;
    int GetWindowStyls() const;

    LPCTSTR GetNormalImage();
    void SetNormalImage(LPCTSTR pStrImage);
    LPCTSTR GetHotImage();
    void SetHotImage(LPCTSTR pStrImage);
    LPCTSTR GetFocusedImage();
    void SetFocusedImage(LPCTSTR pStrImage);
    LPCTSTR GetDisabledImage();
    void SetDisabledImage(LPCTSTR pStrImage);

	DWORD GetSel();//2021-09-08 zm 
    void SetSel(long nStartChar, long nEndChar);
    void SetSelAll();
    void SetReplaceSel(LPCTSTR lpszReplace);

    virtual void SetPos(RECT rc, bool bNeedInvalidate = true) override;
    virtual void Move(SIZE szOffset, bool bNeedInvalidate = true) override;
    virtual bool SetVisible(bool bVisible = true) override;
    virtual void SetInternVisible(bool bVisible = true) override;
    virtual SIZE EstimateSize(SIZE szAvailable) override;
    virtual void DoEvent(TEventUI &event) override;
    virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;

    virtual void PaintStatusImage(HDC hDC) override;
    virtual void PaintText(HDC hDC) override;

    //2017-02-25 zhuyadong 添加提示文字及其颜色
    virtual void SetTipText(LPCTSTR pstrTip) ;
	virtual CDuiString GetTipText() const ;
    virtual void ReloadText(void) override;
    void SetTipColor(DWORD dwColor);
    DWORD GetTipColor();
    // 2017-07-21 zhuyadong 添加 minmaxnumber 属性
    void SetMinMaxNumber(int nMin, int nMax);
    int GetMinNumber();
    int GetMaxNumber();

    // 2018-01-28 zhuyadong 添加字符过滤功能
    void SetCharFilter(bool bFilter);
    bool IsCharFilter(void);
    void SetFilterMode(bool bWiteList);
    bool GetFilterMode(void);
    CDuiString GetFilterCharSet(void);
    void SetFilterCharSet(CDuiString sCharSet);
    void SetFilterCharSet(int nFilterType);
    void AppendFilterCharSet(CDuiString sCharSet);

    // 正则过滤
    void SetRegExpFilter(bool bRegExp);
    bool IsRegExpFilter(void);
    void SetRegExpPattern(CDuiString sRegExp);

protected:
    DWORD GetNativeEditBkColor() const;
    bool IsValidChar(TCHAR ch);
#ifndef UNICODE
    bool IsValidChar(LPCTSTR pstr);
#endif // UNICODE
    bool IsRegExpMatch(TCHAR ch);
    bool IsRegExpMatch(LPCTSTR pstr);

    // 编辑框内容变化时，是否延时通知，延时多少毫秒
    bool IsDelayTxtChange();
    DWORD GetDelayTxtChange();
    void  SetDelayTxtChange(DWORD dwMiliSec);

    // 数字编辑框越界检查延时时间
    DWORD GetDelayValidateTime();
    void SetDelayValidateTime(DWORD dwMiliSec);

protected:
    CEditWnd *m_pWindow;

    UINT m_uMaxChar;
    bool m_bReadOnly;
    bool m_bPasswordMode;
    bool m_bAutoSelAll;
    TCHAR m_cPasswordChar;
    UINT m_uButtonState;
    int m_iWindowStyls;

    TDrawInfo m_diNormal;
    TDrawInfo m_diHot;
    TDrawInfo m_diFocused;
    TDrawInfo m_diDisabled;

    //2017-02-25 zhuyadong 添加提示文字及其颜色
    CDuiString m_sTipText;
    CDuiString m_sTipTextOrig;
    DWORD m_dwTipColor;
    // 2017-07-21 zhuyadong 添加 minmaxnumber 属性
    int   m_nMinNumber;
    int   m_nMaxNumber;
    // 2018-01-28 zhuyadong 添加字符过滤功能
    // 分黑白名单模式，白名单设置允许的字符集；黑名单设备禁止的字符集
    // 正则表达式匹配与字符过滤，只能有一个启用。
    bool  m_bCharFilter;            // 启用字符过滤
    bool  m_bWiteList;              // true 表示白名单，false 表示黑名单。
    bool  m_bRegExp;                // true 表示启用正则表达式匹配
    CDuiString  m_sFilterCharSet;   // 字符过滤集
    CDuiString  m_sRegExp;          // 正则表达式
    CRegexpT<TCHAR>    *m_pRegExp;  // 正则表达式对象

    DWORD   m_dwDelayTxtChangeTime; // 编辑框内容变化时，延时多少毫秒通知。默认0,表示立即通知
    DWORD   m_dwDelayValidateTime;  // 数值范围越界检查延时毫秒，默认2000。仅数字编辑框有效
};

}
#endif // __UIEDIT_H__
