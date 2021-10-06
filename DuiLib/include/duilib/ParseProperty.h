#ifndef __PARSEPROPERTY_H__
#define __PARSEPROPERTY_H__
#pragma once

namespace DuiLib {

bool ParseBool(LPCTSTR pstrVal);            // bool         ֵ���Խ���

BYTE ParseByte(LPCTSTR pstrVal);            // byte         ֵ���Խ�����֧�� ʮ������ д�� 0xE
WORD ParseWord(LPCTSTR pstrVal);            // WORD         ֵ���Խ�����֧�� ʮ������ д�� 0XE
INT ParseInt(LPCTSTR pstrVal);              // INT          ֵ���Խ�����֧�� ʮ������ д�� 0xE
DWORD ParseDWord(LPCTSTR pstrVal);          // DWORD        ֵ���Խ�����֧�� ʮ������ д�� 0XE
DWORD_PTR ParseDWordPtr(LPCTSTR pstrVal);   // DWORD_PTR    ֵ���Խ�����֧�� ʮ������ д�� 0xE
float ParseFloat(LPCTSTR pstrVal);          // float        ֵ���Խ���

CDuiString ParseString(LPCTSTR pstrVal);    // String/Image ֵ���Խ�����
SIZE ParseSize(LPCTSTR pstrVal);            // SIZE         ֵ���Խ�����֧�ֵ��� INT �� INT,INT
RECT ParseRect(LPCTSTR pstrVal);            // RECT         ֵ���Խ�����֧�ֵ��� INT �� INT,INT,INT,INT
TPercentInfo ParseRectPercent(LPCTSTR pstrVal); // DoubleRect   ֵ���Խ���
DWORD ParseColor(LPCTSTR pstrVal);          // Color        ֵ���Խ�����
TCHAR ParseTChar(LPCTSTR pstrVal);			// TCHAR		ֵ���Խ����� //2021-09-08 zm

struct TEffectProperty
{
    BYTE    m_byId;         // ������ЧID
    bool    m_byDirection;  // �������ŷ���
    bool    m_byLoop;       // �Ƿ�ѭ��
    WORD    m_byFrequency;  // ֡����ʱ�䣨���룩

    TEffectProperty(void) : m_byId(EFFECT_FLIPLEFT), m_byDirection(true), m_byLoop(false), m_byFrequency(150) { }
};

TEffectProperty ParseEffect(LPCTSTR pstrVal);       // ������Ч     ֵ���Խ���

}

#endif // __PARSEPROPERTY_H__
