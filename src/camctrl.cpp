#include "camctrl.h"
#include "DxImageProc.h"
#include <stdio.h>
#include <QtGlobal>
#include <QtCore>
#include "mainwindow.h"

CamCtrl::CamCtrl(QObject *parent): QObject(parent)
{
    m_nOperateID = -1;                                      //操作相机编号
    m_nDeviceNum = 0;

    for (int i = 0; i < CAMERANUM; i++)
    {
        m_camDevice[i] = NULL;
        m_nAutoWhiteBalance[i] = GX_BALANCE_WHITE_AUTO_OFF;
        m_stIPInfos[i] = NULL;
    }

    m_FailCamList.clear();
    EnumCamera();
}

CamCtrl::~CamCtrl()
{
    for (uint i = 0; i < CAMERANUM; i++)
    {
        if (m_stIPInfos[i] != NULL)
        {
            delete m_stIPInfos[i];
            m_stIPInfos[i] = NULL;
        }

        CamDevice *dev = m_camDevice[i];

        if (dev != NULL)
        {
            delete dev;
            dev = NULL;
        }
    }
}

void CamCtrl::OpenAllCamera()
{
    uint32_t  i = 0;

    //如果相机没关闭，则关闭相机
    for (i = 0; i < CAMERANUM; i++)
    {
        m_camDevice[i]->OpenCamera();

        if (!m_camDevice[i]->m_pstCam->bIsOpen)
        {
            m_FailCamList << i + 1; // insert fail cam user id
        }
    }
}

void CamCtrl::CloseAllCamera()
{
    uint32_t  i = 0;

    //如果相机没关闭，则关闭相机
    for (i = 0; i < CAMERANUM; i++)
    {
        if (m_camDevice[i])
        {
            m_camDevice[i]->CloseCamera();
        }

        Sleep(100);
    }
}

/// 点击"软触发采图"按钮响应函数
void CamCtrl::ManuCtlByCam(int nid, bool blgihton)
{
    if (!m_camDevice[nid] || !m_camDevice[nid]->m_pstCam->bIsOpen)
    {
        return;
    }

    GX_STATUS  emStatus = GX_STATUS_SUCCESS;

    //发送软触发命令(在触发模式开启时有效)
    if (m_camDevice[nid]->m_pstCam->bIsOpen)
    {
        if ((nid == TOP_CAMERA || nid == BOTTOM_CAMERA) && !blgihton)
        {
            emStatus = GXSetBool(m_camDevice[nid]->m_hDevice, GX_BOOL_USER_OUTPUT_VALUE, false);
        }

        emStatus = GXSendCommand(m_camDevice[nid]->m_hDevice, GX_COMMAND_TRIGGER_SOFTWARE);

        if (emStatus != GX_STATUS_SUCCESS)
        {
            ShowErrorString(emStatus);
            m_camDevice[nid]->CloseCamera();

            if (!m_FailCamList.contains(nid + 1))
            {
                m_FailCamList << nid + 1;    // insert in failed list
            }
        }
    }
}

void CamCtrl::LEDCtrl(int nid, bool bcheck)
{
    if (!m_camDevice[nid] || !m_camDevice[nid]->m_pstCam->bIsOpen)
    {
        return;
    }

    bool result = false;
    GX_STATUS  emStatus = GX_STATUS_SUCCESS;

    if (bcheck)
    {
        result = true;
    }

    // 此开关仅在输出模式为闪光灯模式下才起作用，当设置为开的时候输出触发信号，当设置为关的时候不输出闪光灯信号。
    emStatus = GXSetBool(m_camDevice[nid]->m_hDevice, GX_BOOL_USER_OUTPUT_VALUE, !result);

    if (emStatus)
    {
        ShowErrorString(emStatus);
    }
}

void CamCtrl::CamWhiteBalance(int nid)
{
    if (m_camDevice[nid] && m_camDevice[nid]->m_hDevice != NULL)
    {
        GXSetEnum(m_camDevice[nid]->m_hDevice, GX_ENUM_BALANCE_WHITE_AUTO, GX_BALANCE_WHITE_AUTO_ONCE);
        m_nAutoWhiteBalance[nid] = GX_BALANCE_WHITE_AUTO_ONCE;
    }
}

bool CamCtrl::GetBalanceRatio(int nid, double &dr, double &dg, double &db)
{
    GX_STATUS  emStatus = GX_STATUS_SUCCESS;
    double    dValue   = 0.0;
    // 设置白平衡通道->red
    emStatus = GXSetEnum(m_camDevice[nid]->m_hDevice, GX_ENUM_BALANCE_RATIO_SELECTOR, GX_BALANCE_RATIO_SELECTOR_RED);

    if (emStatus != GX_STATUS_SUCCESS)
    {
        return false;
    }

    // 获取白平衡通道对应的白平衡系数
    emStatus = GXGetFloat(m_camDevice[nid]->m_hDevice, GX_FLOAT_BALANCE_RATIO, &dValue);

    if (emStatus != GX_STATUS_SUCCESS)
    {
        return false;
    }
    else
    {
        dr = dValue;
    }

    // 设置白平衡通道->green
    emStatus = GXSetEnum(m_camDevice[nid]->m_hDevice, GX_ENUM_BALANCE_RATIO_SELECTOR, GX_BALANCE_RATIO_SELECTOR_GREEN);

    if (emStatus != GX_STATUS_SUCCESS)
    {
        return false;
    }

    // 获取白平衡通道对应的白平衡系数
    emStatus = GXGetFloat(m_camDevice[nid]->m_hDevice, GX_FLOAT_BALANCE_RATIO, &dValue);

    if (emStatus != GX_STATUS_SUCCESS)
    {
        return false;
    }
    else
    {
        dg = dValue;
    }

    // 设置白平衡通道->blue
    emStatus = GXSetEnum(m_camDevice[nid]->m_hDevice, GX_ENUM_BALANCE_RATIO_SELECTOR, GX_BALANCE_RATIO_SELECTOR_BLUE);

    if (emStatus != GX_STATUS_SUCCESS)
    {
        return false;
    }

    // 获取白平衡通道对应的白平衡系数
    emStatus = GXGetFloat(m_camDevice[nid]->m_hDevice, GX_FLOAT_BALANCE_RATIO, &dValue);

    if (emStatus != GX_STATUS_SUCCESS)
    {
        return false;
    }
    else
    {
        db = dValue;
    }

    return true;
}

void CamCtrl::TriggerCtrl(int nid, int valmode, int val)
{
    // [[maybe_unused]]:纯粹为了消灭一个警告，不用在意，没什么特殊作用
    GX_STATUS  emStatus = GX_STATUS_SUCCESS;
    bool bIsWritable   = TRUE;

    if (m_camDevice[nid] == NULL)
    {
        return;
    }

    // 判断触发模式是否可写
    emStatus = GXIsWritable(m_camDevice[nid]->m_hDevice, GX_ENUM_TRIGGER_MODE, &bIsWritable);

    if (bIsWritable)
    {
        // 将当前选择的触发模式的值设置到相机中
        emStatus = GXSetEnum(m_camDevice[nid]->m_hDevice, GX_ENUM_TRIGGER_MODE, valmode);
        emStatus = GXSetEnum(m_camDevice[nid]->m_hDevice, GX_ENUM_TRIGGER_SOURCE, val);
    }
    else
    {
        qCritical() << QString("触发模式不可写。");
    }
}

void CamCtrl::UpdateDeviceList()
{
    GX_STATUS  emStatus   = GX_STATUS_SUCCESS;
    uint32_t   nDeviceNum = 0;
    //枚举设备
    emStatus = GXUpdateDeviceList(&nDeviceNum, 1000);

    if (emStatus != GX_STATUS_SUCCESS)
    {
        ShowErrorString(emStatus);
        return;
    }

    GX_DEVICE_BASE_INFO *pBaseinfo = new GX_DEVICE_BASE_INFO[nDeviceNum];
    size_t nSize = nDeviceNum * sizeof(GX_DEVICE_BASE_INFO);
    //获取所有设备的基础信息
    emStatus = GXGetAllDeviceBaseInfo(pBaseinfo, &nSize);

    //设备个数大于0,分配资源获取设备信息
    if (nDeviceNum > 0)
    {
        for (unsigned int i = 0; i < nDeviceNum; i++)
        {
            GX_DEVICE_IP_INFO *stIPInfo = new GX_DEVICE_IP_INFO();
            //获取设备的网络信息
            emStatus = GXGetDeviceIPInfo(i + 1, stIPInfo);

            if (strcmp(stIPInfo->szDeviceID, pBaseinfo[i].szDeviceID) == 0)
            {
                for (int j = 0; j < CAMERANUM; j++)
                {
                    if (strcmp(pBaseinfo[i].szUserID, CAM_DEFINE_USERID[j]) == 0)
                    {
                        m_stIPInfos[j] = stIPInfo;
                        break;
                    }
                }
            }

            // 设备重连
            emStatus = GXGigEResetDevice(stIPInfo->szMAC, GX_MANUFACTURER_SPECIFIC_RECONNECT);

            if (emStatus != GX_STATUS_SUCCESS)
            {
                ShowErrorString(emStatus);
                // 设备复位
                GXGigEResetDevice(stIPInfo->szMAC, GX_MANUFACTURER_SPECIFIC_RESET);
            }
        }
    }

    m_nDeviceNum = qMin(nDeviceNum, (uint32_t)CAMERANUM);
    delete []  pBaseinfo;
}

bool CamCtrl::AllocBufferForDevice()
{
    uint32_t i = 0;

    //---------------------申请空间-----------------------------
    for (i = 0; i < CAMERANUM; i++)
    {
        // create camera object 创建相机数据结构体
        CamDevice *camDevice = new CamDevice(i, ((MainWindow *)parent())->m_imgSettings[i], ((MainWindow *)parent())->m_camSettings[i]);
        m_camDevice[i] = camDevice;
        //---------------------初始化资源信息-----------------------------
        camDevice->m_pstCam = new CAMER_INFO();
        camDevice->m_pCamsFps = new CFps();
        camDevice->m_hDevice = NULL;
        camDevice->m_stIPInfo = m_stIPInfos[i];
        camDevice->m_pstCam->bIsColorFilter = false;
        camDevice->m_pstCam->bIsOpen        = FALSE;
        camDevice->m_pstCam->bIsSnap        = FALSE;
        camDevice->m_pstCam->pBmpInfo       = NULL;
        camDevice->m_pstCam->pRawBuffer     = NULL;
        camDevice->m_pstCam->pImageBuffer   = NULL;
        camDevice->m_pstCam->fFps           = 0.0;
        camDevice->m_pstCam->nBayerLayout   = 0;
        camDevice->m_pstCam->nImageHeight   = 0;
        camDevice->m_pstCam->nImageWidth    = 0;
        camDevice->m_pstCam->nPayLoadSise   = 0;
        memset(camDevice->m_pstCam->chBmpBuf, 0, sizeof(camDevice->m_pstCam->chBmpBuf));
        connect(this, &CamCtrl::UpdateAutoSave, camDevice, &CamDevice::UpdateAutoSaveFlag);
        connect(camDevice, &CamDevice::SetImageData, this, &CamCtrl::SetImageData, Qt::BlockingQueuedConnection);
        connect(camDevice, &CamDevice::ShowSrcImgFromCAMSig, this, &CamCtrl::ShowSrcImgFromCAMSig);
    }

    return true;
}

void CamCtrl::EnumCamera()
{
    // 停止显示设备信息
    m_nOperateID = -1;
    // 若设备被打开则先关闭
    CloseAllCamera();
    // 枚举设备
    UpdateDeviceList();

    // 设备个数为0则直接返回
    if (m_nDeviceNum <= 0)
    {
        return ;
    }

    // 为设备分配资源
    if (AllocBufferForDevice())
    {
        m_nOperateID = 0;
    }
    else
    {
        m_nOperateID = -1;
    }
}
