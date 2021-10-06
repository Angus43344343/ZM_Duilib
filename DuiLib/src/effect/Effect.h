#pragma once
#include "graph_def.h"
#include "graph_rect.h"
#include "image_process.h"
#include <vector>

namespace DuiLib {

class CFlipEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
};

class CMosaicEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
private:
    int      m_nWidth;                  // ͼƬ��
    int      m_nHeight;                 // ͼƬ��
    int      m_nMosaicW;                // �����˿�
    int      m_nMosaicH;                // �����˸�
    int      m_nWidthCount;             // �����˺������
    int      m_nHeightCount;            // �������������
    int     *m_pMosaicArray;            // ÿ���������Ƿ�ִ����
};

class CSlideEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
protected:
    void  CalculateFrame(int length);
protected:
    int      m_step;                    // ÿ�λ����ľ���(speed)
};

class CBandSlideEffect : public CSlideEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
private:
    int     m_bandNum;                  // ��״����
    int     m_bandSize;                 // ��״�Ŀ���߸�
};

class CBlindsEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
protected:
    int     m_bandNum;                  // ��Ҷ����
    int     m_bandSize;                 // ��Ҷ�Ŀ���߸�
};

class CSepcialBlindsEffect : public CBlindsEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
private:
    void CalculateFrame(int length);
private:
    std::vector<int>    m_specialBandSize;      // ��Ҷ�Ŀ���߸�
    int                 m_nowSumRowCol;         // ��ǰ������ʼ�л�����
};

class CScanEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
protected:
    void  CalculateFrame(int length);
protected:
    int      m_step;                    // ÿ��ɨ��ľ���(speed)
};

class CInertiaSlideEffect : public CSlideEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
private:
    Graph::real m_tInertiaTime;         // �����˶���ǰʱ��
    void   *m_inertiaInstance;          // ���Ը�����
    Graph::real m_tInertiacoeffecient;  // �����˶����ʵ��ڲ���
};

class CFadeEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
};

class CZoomEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
private:
    Graph::CdRect               m_rcNow;        // �任�굱ǰ�Ĵ�С
};

class CShrinkEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
private:
    Graph::CdRect               m_rcNow;        // �任�굱ǰ�Ĵ�С
};

class CSwingEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
};

class CFunnelEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
};

class CNoiseEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
private:
    int                         m_u;            // ����
};

class CCubeEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
};

class CGradualEraseEffect : public CScanEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
private:
    Graph::real     m_alphaStep;            // alpha����ֵ
    int             m_lastStart;            // ��һ�ν�����ʼ�л�����
    int             m_lastEnd;              // ��һ�ν�������л�����
};

class CPushEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
};

class CDissolveEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
private:
    BYTE           *m_pDissolveArray;       // ��ʾ�Ƿ��ѱ��ܽ�
    int             m_size;                 // ������������
    int             m_framePixel;           // ÿ֡�ܽ�ĸ���
};

class CCurtainEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
};

class CTriangularprismEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
};

class CCubeInsideEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
};

class CSpatialmotionEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
private:
    int         m_xyOffset;                     // x��y�Ჽ��
    int         m_zOffset;                      // z�Ჽ��
};

class CPatchFlipEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
};

class CSpatialRotateEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
};

class CDoorEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
};

class CWaveEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
private:
    CWater          *m_pWater;                      // ˮ������ϵͳ
};

class CScatterEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
private:
    int             m_vx[4];                        // ˮƽ�ٶ�
    int             m_vy[4];                        // ��ֱ�ٶ�
};

class CColorFadeEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
};

class CDampSlideEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
private:
    Graph::real m_tInertiaTime;         // �����˶���ǰʱ��
    void       *m_inertiaInstance;      // ���Ը�����
    Graph::real m_tInertiacoeffecient;  // �����˶����ʵ��ڲ���
    Graph::real m_ratio;                // ͼ��ĳ����
};

class CWhirlPoolEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
};

class CClockWipeEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
private:

    Bitmap     *m_pBitmap;                  // ����
    BYTE       *m_pBits;                    // ����
};

class CPinWheelEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
private:

    Bitmap     *m_pBitmap;                  // ����
    BYTE       *m_pBits;                    // ����
};

class CPagePeelEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
private:

    Bitmap     *m_pBitmap;                  // ����
    BYTE       *m_pBits;                    // ����
    real        m_nK;                       // ϵ��
    BOOL        m_intersectTop;             // �붥���ཻ
};

class CBlurEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
private:
};

class CZoomBlurEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
private:
};

class CFoldEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
private:
    int                     m_foldNum;                  // �۵�Ч��������
    std::vector<CdRect>     m_foldRect;                 // ÿ���۵�RECT
    std::vector<WarpRect>   m_warpRect;                 // Ť����rect
};

class CStackEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
private:
    int                 m_bandNum;                  // ��״������
    int                 m_bandSize;                 // ��״�Ŀ���߸�
    std::vector<int>    m_bandFrameNum;             // ÿ����֡��
    int                 m_bandAnimating;            // ��ǰ�˶��Ĵ�״���
    int                 m_bandFrameSum;             // �ڵ�ǰ��״ǰ��Ĵ�״�ۻ���ɵ�֡��
    int                 m_bandSubFrame;             // ��ǰ��״��ִ��֡��
};

/////////////////////////////////////////////////////////////////////////////////////////////
//��Ч����
IEffect *CreateEffect(DWORD animationEffect);


}
/////////////////////////////////////////////////////////////////////////////////////////////
