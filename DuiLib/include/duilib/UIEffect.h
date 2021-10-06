#pragma once

namespace DuiLib {

// ����������ID
enum EMTrigger
{
    TRIGGER_NONE = 0,
    TRIGGER_ENTER,                  // �������
    TRIGGER_LEAVE,                  // ����Ƴ�
    TRIGGER_CLICK,                  // ��굥��
    TRIGGER_SHOW,                   // ��ʾ
    TRIGGER_HIDE,                   // ����

    TRIGGER_COUNT,                  // ����������
    TRIGGER_ALL = 255,              // ���д�����
};

// ��������ID
enum EMEffectID
{
    EFFECT_FLIPLEFT = 0,                    // ����ת
    EFFECT_FLIPRIGHT,                       // ���ҷ�ת
    EFFECT_FLIPTOP,                         // ���Ϸ�ת
    EFFECT_FLIPDOWN,                        // ���·�ת
    EFFECT_MOSAIC = 4,                      // ������
    EFFECT_SLIDELEFT,                       // ���󻬶�
    EFFECT_SLIDERIGHT,                      // ���һ���
    EFFECT_SLIDETOP,                        // ���ϻ���
    EFFECT_SLIDEDOWN,                       // ���»���
    EFFECT_BANDSLIDEH = 9,                  // ˮƽ��״����
    EFFECT_BANDSLIDEV,                      // ��ֱ��״����
    EFFECT_BLINDSH,                         // ˮƽ��Ҷ
    EFFECT_BLINDSV,                         // ��ֱ��Ҷ
    EFFECT_SEPCIALBLINDSH,                  // ���ȸ�ˮƽ��Ҷ
    EFFECT_SEPCIALBLINDSV = 14,             // ���ȿ�ֱ��Ҷ
    EFFECT_SCANLEFT,                        // ����ɨ��
    EFFECT_SCANRIGHT,                       // ����ɨ��
    EFFECT_SCANTOP,                         // ����ɨ��
    EFFECT_SCANDOWN,                        // ����ɨ��
    EFFECT_INERTIASLIDELEFT = 19,           // ������Ի���
    EFFECT_INERTIASLIDERIGHT,               // ���ҹ��Ի���
    EFFECT_INERTIASLIDETOP,                 // ���Ϲ��Ի���
    EFFECT_INERTIASLIDEDOWN,                // ���¹��Ի���
    EFFECT_FADE,                            // ����/����
    EFFECT_ZOOM = 24,                       // �Ŵ�
    EFFECT_SHRINK,                          // ��С
    EFFECT_SWING,                           // �ڳ�/����
    EFFECT_FUNNEL,                          // ©��
    EFFECT_NOISE,                           // ����
    EFFECT_CUBELEFT = 29,                   // ��������
    EFFECT_CUBERIGHT,                       // ��������
    EFFECT_CUBETOP,                         // ��������
    EFFECT_CUBEDOWN,                        // ��������
    EFFECT_GRADUALERASELEFT,                // ���󽥱����
    EFFECT_GRADUALERASERIGHT = 34,          // ���ҽ������
    EFFECT_GRADUALERASETOP,                 // ���Ͻ������
    EFFECT_GRADUALERASEDOWN,                // ���½������
    EFFECT_PUSHLEFT,                        // �����Ƽ�
    EFFECT_PUSHRIGHT,                       // �����Ƽ�
    EFFECT_PUSHTOP = 39,                    // �����Ƽ�
    EFFECT_PUSHDOWN,                        // �����Ƽ�
    EFFECT_DISSOLVE,                        // �ܽ�
    EFFECT_CURTAIN,                         // Ļ
    EFFECT_TRIANGULARPRISMLEFT,             // ��������
    EFFECT_TRIANGULARPRISMRIGHT = 44,       // ��������
    EFFECT_TRIANGULARPRISMTOP,              // ��������
    EFFECT_TRIANGULARPRISMDOWN,             // ��������
    EFFECT_CUBEINSIDELEFT,                  // ����������
    EFFECT_CUBEINSIDERIGHT,                 // ����������
    EFFECT_CUBEINSIDETOP = 49,              // ����������
    EFFECT_CUBEINSIDEDOWN,                  // ����������
    EFFECT_SPATIALMOTIONLEFT,               // �ռ�����
    EFFECT_SPATIALMOTIONRIGHT,              // �ռ�����
    EFFECT_SPATIALMOTIONTOP,                // �ռ�����
    EFFECT_SPATIALMOTIONDOWN = 54,          // �ռ�����
    EFFECT_PATCHRANDOMFLIP,                 // �ֿ鷭ת
    EFFECT_SPATIALROTATELEFT,               // �ռ�������ת
    EFFECT_SPATIALROTATERIGHT,              // �ռ�������ת
    EFFECT_SPATIALROTATETOP,                // �ռ�������ת
    EFFECT_SPATIALROTATEDOWN = 59,          // �ռ�������ת
    EFFECT_DOOR,                            // ��/����
    EFFECT_WHIRLPOOL,                       // ����
    EFFECT_SCATTER,                         // ��ɢ
    EFFECT_COLORDADE,                       // ��ɫ
    EFFECT_DAMPSLIDE_DOWNLEFT = 64,         // ���½�(��ʼ����)���Ử��
    EFFECT_DAMPSLIDE_DOWNRIGHT,             // ���½�(��ʼ����)���Ử��
    EFFECT_DAMPSLIDE_TOPLEFT,               // ���Ͻ�(��ʼ����)���Ử��
    EFFECT_DAMPSLIDE_TOPRIGHT,              // ���Ͻ�(��ʼ����)���Ử��
    EFFECT_DAMPSLIDELEFT,                   // �������Ử��
    EFFECT_DAMPSLIDERIGHT = 69,             // �������Ử��
    EFFECT_DAMPSLIDETOP,                    // �������Ử��
    EFFECT_DAMPSLIDEDOWN,                   // �������Ử��
    EFFECT_CLOCKWIPE,                       // ���β���
    EFFECT_PINWHEEL,                        // �糵
    EFFECT_PAGEPEEL = 74,                   // ��ҳ
    EFFECT_GAUSSBLUR,                       // ģ������/��
    EFFECT_WAVE,                            // ���˻�
    EFFECT_ZOOMBLUR,                        // ��β
    EFFECT_FOLDV,                           // ��ֱ�۵�
    EFFECT_FOLDH = 79,                      // ˮƽ�۵�
    EFFECT_STACKLEFT,                       // ����ѻ�
    EFFECT_STACKRIGHT,                      // ���Ҷѻ�
    EFFECT_STACKTOP,                        // ���϶ѻ�
    EFFECT_STACKDOWN = 83,                  // ���¶ѻ�

    EFFECT_COUNT,                           // ��Ч����
};

class IEffect;

// ��������
struct TAniParam
{
    BYTE        byTrigger;          // ������Ч������          ��Ҫ�û�ָ�� Ĭ�� TRIGGGER_NONE
    BYTE        byEffect;           // ����ID��EMEffectID      ��Ҫ�û�ָ�� Ĭ�� EFFECT_FLIPLEFT
    WORD        wFrequency;         // ����֡ʱ�䣬��λ������  ��Ҫ�û�ָ�� Ĭ�� 150 ���룬ֵԽ�󣬶���Խ����
    bool        bDirection;         // ����˳��                ��Ҫ�û�ָ�� Ĭ�� true
    bool        bLoop;              // �Ƿ�ѭ��������Ч        ��Ҫ�û�ָ�� Ĭ�� false
    bool        bLastFrame;         // �Ƿ������һ֡
    BYTE        byCurFrame;         // ��ǰ��ʾ֡

    HBITMAP     hBitmap;            // ����ͼƬ��Դ
    HDC         hMemDC;             // ����ͼƬDC
    SIZE        szMemDC;            // ����ͼƬ�ߴ�
    BYTE       *pBmpData;           // ����ͼƬ�ڴ��ַ
    BYTE       *pBmpDataCopy;       // �ⲿ����ͼƬ��Դ����
    IEffect    *pEffect;            // ��Ч��ʵ��ָ��

    TAniParam(void);
    bool Init(CControlUI *pCtrl);
    void Release(void);
    bool IsRunning(void);
};

// ��Чִ�й����еĻص��ӿ�
class _declspec(novtable) IEffectCallBackUI
{
public:
    // ÿһ �� ������ʼʱ�ص�
    virtual void OnEffectBegin(TAniParam &data) = 0;
    // ÿһ �� ��������ʱ�ص�
    virtual void OnEffectEnd(TAniParam &data) = 0;
    // ÿһ ֡ ��������ʱ�ص�
    virtual void OnEffectDraw(TAniParam &data) = 0;
};

class DUILIB_API CEffectUI
{
public:
    CEffectUI(CControlUI *pCtrl);
    ~CEffectUI(void);

    // ��Ӷ���
    // ����1������������    ����2����ЧID    ����3����Ч֡���    ����4������˳��    ����5���Ƿ�ѭ��
    bool Add(BYTE byTrigger, BYTE byEffect, WORD wFrequency = 150, bool bDirection = true, bool bLoop = false);
    // ɾ��������������ڲ��ţ����Զ��ر�
    void Del(BYTE byTrigger);

    // ��ʼ����ָ������������Ч
    bool Start(BYTE byTrigger);
    // ֹͣ������Ч
    // ����: ����������
    //************************************
    void Stop(BYTE byTrigger = TRIGGER_ALL);

    // �����Ƿ����ڲ���ָ������Ч
    bool IsRunning(BYTE byTrigger = TRIGGER_NONE);

    bool HasEffectTrigger(BYTE byTrigger);

    // ��Ч����
    void OnElapse(BYTE byTrigger);

    // ����ָ����Ч���ڴ�HDC�����ڻ���
    HDC GetMemHDC(BYTE byTrigger);
    BYTE GetCurFrame(BYTE byTrigger);
    bool IsLastFrame(BYTE byTrigger);
protected:
    TAniParam *GetTriggerById(BYTE byTrigger);
    void RemoveTriggerById(BYTE byTrigger);

protected:
    CControlUI         *m_pControl;         // ���ö����Ŀؼ�ָ��
    CDuiValArray        m_aryAniParam;      // ������������
};

//////////////////////////////////////////////////////////////////////////
// �Զ�����Ч�ӿڣ�����ʵ���Զ�����Ч��
class _declspec(novtable) IEffect
{
public:
    // ����һ֡
    virtual void ComputeOneFrame(TAniParam *pData) = 0;
    // ��ʼ����Ч����
    virtual void InitEffectParam(TAniParam *pData) = 0;
    // ������Ч����
    virtual void ReleaseEffectParam() = 0;

protected:
    int      m_nFrame;
};

}