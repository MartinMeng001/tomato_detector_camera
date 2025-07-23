#pragma once
#include "GxIAPI.h"
#include "GxEx.h"
#include "Fps.h"
#include "def.h"
#include "camdevice.h"
#include <qlist.h>

class CamCtrl: public QObject
{
    Q_OBJECT

public:
    CamCtrl(QObject *parent = nullptr);
    ~CamCtrl();

    void OpenAllCamera();
    void CloseAllCamera();
    void ManuCtlByCam(int, bool blgihton = false);
    void LEDCtrl(int, bool);
    void CamWhiteBalance(int);
    bool GetBalanceRatio(int, double &, double &, double &);
    void TriggerCtrl(int, int, int);

    int                 m_nOperateID;                   ///< 操作设备ID
    uint32_t            m_nDeviceNum;                   ///< 记录设备个数
    CamDevice           *m_camDevice[CAMERANUM];        ///< 相机对象
    QList<int>          m_FailCamList;                  ///< 设备打开失败列表
    GX_DEVICE_IP_INFO   *m_stIPInfos[CAMERANUM];
    int                 m_nAutoWhiteBalance[CAMERANUM]; ///< 自动白平衡当前选项

signals:
    void UpdateAutoSave(bool);
    void SetImageData(int, BYTE *);
    void ShowSrcImgFromCAMSig(int nid, BYTE *);

private:
    void EnumCamera();
    void UpdateDeviceList();
    bool AllocBufferForDevice();

};

