#pragma once

namespace DuiLib
{
	#define ROLLTEXT_LEFT		0
	#define ROLLTEXT_RIGHT		1
	#define ROLLTEXT_UP 		2
	#define ROLLTEXT_DOWN		3


	#define ROLLTEXT_TIMERID			40
	#define ROLLTEXT_TIMERID_SPAN		500U
	#define ROLLTEXT_TIMERID_STATE		1

	#define ROLLTEXT_ROLL_END			41
	#define ROLLTEXT_ROLL_END_SPAN		1000*6U
	#define ROLLTEXT_ROLL_END_STATE		2

	class DUILIB_API CRollTextUI : public CContainerUI
	{
	public:
		CRollTextUI(void);
		~CRollTextUI(void);

	public:
		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

	public:
		void SetFont(int index);
		int GetFont() const;
		BOOL IsUseRoll();
		void SetUseRoll(BOOL bUseRoll);
		int GetRollDirection();
		void SetRollDirection(int iRollDirction);
		UINT GetTimeSpan();
		void SetTimeSpan(UINT uiTimeSpan);
		UINT GetTimeEndSpan();
		void SetTimeEndSpan(UINT uiTimeEndSpan);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		virtual void DoEvent(TEventUI& event);
		virtual void SetText(LPCTSTR pstrText);

		virtual void PaintText(HDC hDC) override;

	public:
		void BeginRoll(LONG lTimeSpan = ROLLTEXT_TIMERID_SPAN, LONG lMaxTimeLimited = ROLLTEXT_ROLL_END_SPAN);
		void EndRoll();

	private:
		virtual void GetDrawItem(HDC hDC, UINT& uiTextStyle, RECT& rcItem);

	private:
		int m_nStep;
		int m_nScrollPos;
		BOOL m_bUseRoll;
		int m_nText_W_H;

		UINT m_uiTimeSpan;
		UINT m_uiTimeEndSpan;

		int m_nRollState;
		int m_nRollDirection;

		int     m_iFont;
		UINT    m_uTextStyle;
		BOOL    m_bTimeRoll;
	};

}	// namespace DuiLib
