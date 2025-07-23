#pragma once

#include <QObject>
#include "GxIAPI.h"
#include "GxEx.h"
#include "Fps.h"
#include "def.h"

#define  GX_VERIFY(emStatus) \
    if(emStatus != GX_STATUS_SUCCESS) \
    {\
        ShowErrorString(emStatus); \
        return;\
    }  ///< 错误提示函数宏定义

///错误信息提示
void ShowErrorString(GX_STATUS emErrorStatus);

class CamDevice : public QObject
{
    Q_OBJECT
public:
    explicit CamDevice(int, ImageSetting *, CamSetting *, QObject *parent = nullptr);
    ~CamDevice();

    ///相机回调
    static void __stdcall OnFrameCallbackFun(GX_FRAME_CALLBACK_PARAM *pFrame);
    void OpenCamera();
    void CloseCamera();

    CAMER_INFO         *m_pstCam;           ///< 相机数据结构体
    //    BOOL                m_bViewID;          ///< 是否显示相机编号
    CFps                *m_pCamsFps;        ///< 设备Fps计数对象
    GX_DEV_HANDLE       m_hDevice;          ///< 设备句柄指针
    GX_DEVICE_IP_INFO   *m_stIPInfo;        ///< 设备IP信息

    ImageSetting *m_imgSetting;
    CamSetting *m_camSetting;
    bool m_autoSave;

signals:
    void SetImageData(int, BYTE *);
    void ShowSrcImgFromCAMSig(int nid, BYTE *);

public slots:
    void UpdateAutoSaveFlag(bool);

private:
    int                 m_nID;               ///< 操作设备ID
    QString m_strFilePath;

    void StartScan();
    void StopScan();

    ///设置设备的数据位深为8
    GX_STATUS SetPixelFormat8bit(GX_DEV_HANDLE hDevice);
    void UnPrepareForShowImg();
    void RegisterCallback();
    void InitCamParam();
    void SaveImage();
};

