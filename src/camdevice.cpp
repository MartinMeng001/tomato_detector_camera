#include "camdevice.h"
#include "DxImageProc.h"
#include <QtCore>

void ShowErrorString(GX_STATUS emErrorStatus)
{
    char     *pchErrorInfo = NULL;
    size_t    nSize        = 0;
    GX_STATUS emStatus     = GX_STATUS_ERROR;
    // 获取错误信息长度，并申请内存空间
    emStatus = GXGetLastError(&emErrorStatus, NULL, &nSize);
    pchErrorInfo = new char[nSize];

    if (NULL == pchErrorInfo)
    {
        return;
    }

    // 获取错误信息，并显示
    emStatus = GXGetLastError(&emErrorStatus, pchErrorInfo, &nSize);

    if (emStatus != GX_STATUS_SUCCESS)
    {
        qCritical() << QString("GXGetLastError调用失败，error status：%1").arg(emErrorStatus);
    }
    else
    {
        qInfo() << QString("GX接口调用失败，error msg：%1").arg(pchErrorInfo);
    }

    // 释放申请的内存空间
    if (NULL != pchErrorInfo)
    {
        delete[] pchErrorInfo;
        pchErrorInfo = NULL;
    }
}

CamDevice::CamDevice(int nID, ImageSetting *pms, CamSetting *pcs, QObject *parent) : QObject(parent)
{
    m_nID = nID;
    m_pstCam     = NULL;                                    //初始化相机参数结构体
    //    m_bViewID    = true;                                   //是否显示相机ID
    m_pCamsFps   = NULL;                                    //初始化设备Fps计数对象
    m_hDevice   = NULL;                                    //初始化相机句柄
    m_autoSave = false;
    m_stIPInfo = NULL;
    // 获取当前程序执行路径,设置图像保存的默认路径
    char    szName[MAX_PATH] = {0};
    QString strAppFullName = "";
    GetModuleFileNameA(NULL, szName, MAX_PATH);
    strAppFullName = szName;
    int nPos = strAppFullName.lastIndexOf('\\');
    m_strFilePath += strAppFullName.leftRef(nPos);
    m_strFilePath += IMG_SAVE_PATH;
    m_camSetting = pcs;
    m_imgSetting = pms;
}

CamDevice::~CamDevice()
{
    if (m_pCamsFps != NULL)
    {
        delete m_pCamsFps;
        m_pCamsFps = NULL;
    }

    if (m_pstCam != NULL)
    {
        if (m_pstCam->pImageBuffer != NULL)
        {
            delete[]m_pstCam->pImageBuffer;
            m_pstCam->pImageBuffer = NULL;
        }

        if (m_pstCam->pRawBuffer != NULL)
        {
            delete []m_pstCam->pRawBuffer;
            m_pstCam->pRawBuffer = NULL;
        }
    }

    delete m_pstCam;
    m_pstCam = NULL;
}

// 采集模式:连续采集,触发开关:开,触发源:软触发
void CamDevice::OpenCamera()
{
    GX_STATUS emStatus = GX_STATUS_SUCCESS;
    int64_t   nValue   = 0;
    bool      bIsImplemented = false;
    //    char      szIndex[10]    = {0};
    [[maybe_unused]]double    dShutterValue = 0.0;
    // 定义并初始化设备打开参数
    GX_OPEN_PARAM stOpenParam;
    stOpenParam.accessMode = GX_ACCESS_EXCLUSIVE;
    stOpenParam.openMode   = GX_OPEN_USERID;// GX_OPEN_INDEX;
    stOpenParam.pszContent = NULL;

    //第一个相机序号从1开始
    //    _itoa(m_nID + 1, szIndex, 10);

    // 若相机已被打开则先关闭
    if (m_hDevice != NULL)
    {
        emStatus = GXCloseDevice(m_hDevice);
        GX_VERIFY(emStatus);
        m_hDevice = NULL;
    }

    //打开相机
    //stOpenParam.pszContent = szIndex;
    stOpenParam.pszContent = const_cast<char *>(CAM_DEFINE_USERID[m_nID]);
    emStatus = GXOpenDevice(&stOpenParam, &m_hDevice);
    GX_VERIFY(emStatus);
    // 建议用户在打开网络相机之后，根据当前网络环境设置相机的流通道包长值，
    // 以提高网络相机的采集性能,设置方法参考以下代码。
    {
        bool     bImplementPacketSize = false;
        uint32_t unPacketSize         = 0;
        // 判断设备是否支持流通道数据包功能
        emStatus = GXIsImplemented(m_hDevice, GX_INT_GEV_PACKETSIZE, &bImplementPacketSize);
        GX_VERIFY(emStatus);

        if (bImplementPacketSize)
        {
            // 获取当前网络环境的最优包长值
            emStatus = GXGetOptimalPacketSize(m_hDevice, &unPacketSize);
            GX_VERIFY(emStatus);
            // 将最优包长值设置为当前设备的流通道包长值
            emStatus = GXSetInt(m_hDevice, GX_INT_GEV_PACKETSIZE, unPacketSize);
            GX_VERIFY(emStatus);
        }
    }
    m_pstCam->bIsOpen = TRUE;
    //获取相机Bayer转换类型及是否支持输出彩色图像
    emStatus = GXIsImplemented(m_hDevice, GX_ENUM_PIXEL_COLOR_FILTER, &bIsImplemented);
    GX_VERIFY(emStatus);
    m_pstCam->bIsColorFilter = bIsImplemented;

    if (bIsImplemented)
    {
        emStatus = GXGetEnum(m_hDevice, GX_ENUM_PIXEL_COLOR_FILTER, &nValue);
        GX_VERIFY(emStatus);
        m_pstCam->nBayerLayout = nValue;
    }

    // 获取图像宽
    emStatus = GXGetInt(m_hDevice, GX_INT_WIDTH, &nValue);
    GX_VERIFY(emStatus);
    m_pstCam->nImageWidth = nValue;
    m_camSetting->iWidth = nValue;
    // 获取图像高
    emStatus = GXGetInt(m_hDevice, GX_INT_HEIGHT, &nValue);
    GX_VERIFY(emStatus);
    m_pstCam->nImageHeight = nValue;
    m_camSetting->iHeight = nValue;
    // 已知当前相机支持哪个8位图像数据格式可以直接设置
    // 例如设备支持数据格式GX_PIXEL_FORMAT_BAYER_GR8,则可按照以下代码实现
    // bug:Failed to write enumeration value. Enum entry is not writable : AccessException thrown in node 'PixelFormat' while calling 'PixelFormat.SetIntValue()' (file 'Enumeration.cpp', line 149)
    //    emStatus = GXSetEnum(m_hDevice, GX_ENUM_PIXEL_FORMAT, GX_PIXEL_FORMAT_BAYER_GR8);
    //    GX_VERIFY(emStatus);
    // 不清楚当前相机的数据格式时，可以调用以下函数将图像数据格式设置为8Bit
    //    emStatus = SetPixelFormat8bit(m_hDevice);
    //    GX_VERIFY(emStatus);
    // 获取原始图像大小
    emStatus = GXGetInt(m_hDevice, GX_INT_PAYLOAD_SIZE, &nValue);
    GX_VERIFY(emStatus);
    m_pstCam->nPayLoadSise = nValue;
    InitCamParam();
    StartScan();
}

GX_STATUS CamDevice::SetPixelFormat8bit(GX_DEV_HANDLE hDevice)
{
    GX_STATUS emStatus    = GX_STATUS_SUCCESS;
    int64_t   nPixelSize  = 0;
    uint32_t  nEnmuEntry  = 0;
    size_t    nBufferSize = 0;
    GX_ENUM_DESCRIPTION  *pEnumDescription = NULL;
    // 获取像素点大小
    emStatus = GXGetEnum(hDevice, GX_ENUM_PIXEL_SIZE, &nPixelSize);

    if (emStatus != GX_STATUS_SUCCESS)
    {
        return emStatus;
    }

    // 判断为8bit时直接返回,否则设置为8bit
    if (nPixelSize == GX_PIXEL_SIZE_BPP8)
    {
        return GX_STATUS_SUCCESS;
    }
    else
    {
        // 获取设备支持的像素格式枚举数
        emStatus = GXGetEnumEntryNums(hDevice, GX_ENUM_PIXEL_FORMAT, &nEnmuEntry);

        if (emStatus != GX_STATUS_SUCCESS)
        {
            return emStatus;
        }

        // 为获取设备支持的像素格式枚举值准备资源
        nBufferSize      = nEnmuEntry * sizeof(GX_ENUM_DESCRIPTION);
        pEnumDescription = new GX_ENUM_DESCRIPTION[nEnmuEntry];
        // 获取支持的枚举值
        emStatus = GXGetEnumDescription(hDevice, GX_ENUM_PIXEL_FORMAT, pEnumDescription, &nBufferSize);

        if (emStatus != GX_STATUS_SUCCESS)
        {
            if (pEnumDescription != NULL)
            {
                delete []pEnumDescription;
                pEnumDescription = NULL;
            }

            return emStatus;
        }

        // 遍历设备支持的像素格式,设置像素格式为8bit,
        // 如设备支持的像素格式为Mono10和Mono8则设置其为Mono8
        for (uint32_t i = 0; i < nEnmuEntry; i++)
        {
            if ((pEnumDescription[i].nValue & GX_PIXEL_8BIT) == GX_PIXEL_8BIT)
            {
                emStatus = GXSetEnum(hDevice, GX_ENUM_PIXEL_FORMAT, pEnumDescription[i].nValue);
                break;
            }
        }

        // 释放资源
        if (pEnumDescription != NULL)
        {
            delete []pEnumDescription;
            pEnumDescription = NULL;
        }
    }

    return emStatus;
}

void CamDevice::OnFrameCallbackFun(GX_FRAME_CALLBACK_PARAM *pFrame)
{
    if (pFrame->status != 0)
    {
        return;
    }

    GX_STATUS emStatus    = GX_STATUS_SUCCESS;
    CamDevice *pf = (CamDevice *)(pFrame->pUserParam);

    // close led
    if ((pf->m_nID == TOP_CAMERA || pf->m_nID == BOTTOM_CAMERA) && !g_btrimode)
    {
        emStatus = GXSetBool(pf->m_hDevice, GX_BOOL_USER_OUTPUT_VALUE, true);
        GX_VERIFY(emStatus);
    }

    BYTE        *pImageBuffer = NULL;          //转换后的图像buffer
    BYTE        *pRawBuf      = NULL;          //转换前图像
    int64_t     nBayerLayout  = 0;             //Bayer格式
    int         nImgWidth     = 0;             //图像的宽
    int         nImgHeight    = 0;             //图像的高
    int         i             = 0;             //循环变量
    //初始化参数
    pf->m_pCamsFps->IncreaseFrameNum();
    nImgWidth    = (int)(pf->m_pstCam->nImageWidth);
    nImgHeight   = (int)(pf->m_pstCam->nImageHeight);
    pImageBuffer = pf->m_pstCam->pImageBuffer;
    pRawBuf      = pf->m_pstCam->pRawBuffer;
    nBayerLayout = pf->m_pstCam->nBayerLayout;
    //拷贝图像到pRawBuffer中
    memcpy(pf->m_pstCam->pRawBuffer, pFrame->pImgBuf, (size_t)(pf->m_pstCam->nPayLoadSise));

    if (pf->m_pstCam->bIsColorFilter)   //彩色相机
    {
        //        DxRaw8toRGB24(pRawBuf, pImageBuffer, nImgWidth, nImgHeight, RAW2RGB_NEIGHBOUR, DX_PIXEL_COLOR_FILTER(nBayerLayout), TRUE);
        DxRaw8toRGB24Ex(pRawBuf, pImageBuffer, nImgWidth, nImgHeight, RAW2RGB_NEIGHBOUR, DX_PIXEL_COLOR_FILTER(nBayerLayout), FALSE, DX_ORDER_BGR);
    }
    else         //黑白相机
    {
        //黑白图象需要图象翻转
        for (i = 0; i < nImgHeight; i++)
        {
            memcpy(pImageBuffer + i * nImgWidth, pRawBuf + (nImgHeight - i - 1) * nImgWidth, nImgWidth);
        }
    }

    // 1.set data;
    // 2.set signal for image process
    // --3.draw picture in main form
    emit pf->SetImageData(pf->m_nID, pImageBuffer);
    SetEvent(g_camComplHnds[pf->m_nID]);

    // continues show pic
    if (pf->m_camSetting->bTriggerMode == 0 && pf->m_camSetting->iTriggerSource == 0)
    {
        emit pf->ShowSrcImgFromCAMSig(pf->m_nID, pImageBuffer);
    }

    if (pf->m_autoSave)
    {
        pf->SaveImage();
    }
}

void CamDevice::RegisterCallback()
{
    GX_STATUS emStatus = GX_STATUS_ERROR;
    //注册回调
    emStatus = GXRegisterCaptureCallback(m_hDevice, this, OnFrameCallbackFun);
    GX_VERIFY(emStatus);
}

void CamDevice::InitCamParam()
{
    GX_STATUS emStatus    = GX_STATUS_SUCCESS;
    // 初始化曝光相关控件,获取当前值并将当前值更新到界面
    //emStatus = GXGetFloat(m_hDevice, GX_FLOAT_EXPOSURE_TIME, &dShutterValue);
    emStatus = GXSetFloat(m_hDevice, GX_FLOAT_EXPOSURE_TIME, m_imgSetting->iExpoTime);
    GX_VERIFY(emStatus);
    //m_imgSetting->iExpoTime = (int)dShutterValue;
    //引脚选择为Line0
    emStatus = GXSetEnum(m_hDevice, GX_ENUM_LINE_SELECTOR, GX_ENUM_LINE_SELECTOR_LINE1);
    //设置引脚方向为输出
    emStatus = GXSetEnum(m_hDevice, GX_ENUM_LINE_MODE, GX_ENUM_LINE_MODE_OUTPUT);
    //如果设置输出源为用户自定义输出,则如下代码
    emStatus = GXSetEnum(m_hDevice, GX_ENUM_LINE_SOURCE, GX_ENUM_LINE_SOURCE_STROBE);
    //还可以设置用户自定义输出值，如下代码
    emStatus = GXSetEnum(m_hDevice, GX_ENUM_USER_OUTPUT_SELECTOR,  GX_USER_OUTPUT_SELECTOR_OUTPUT0);
    //设置输出信号极性为高
    //当输出信号模式为闪光灯模式的时候代表上升沿或者下降沿；当输出信号模式为用户自定义模式的时候代表高电平或者低电平。
    emStatus = GXSetBool(m_hDevice, GX_BOOL_USER_OUTPUT_VALUE, false);
    //设置采集模式连续采集
    emStatus = GXSetEnum(m_hDevice, GX_ENUM_ACQUISITION_MODE, GX_ACQ_MODE_CONTINUOUS);
    GX_VERIFY(emStatus);
    //设置触发模式为开
    emStatus = GXSetEnum(m_hDevice, GX_ENUM_TRIGGER_MODE, GX_TRIGGER_MODE_ON);
    GX_VERIFY(emStatus);
    //选择触发源为软触发
    emStatus = GXSetEnum(m_hDevice, GX_ENUM_TRIGGER_SOURCE, GX_TRIGGER_SOURCE_SOFTWARE);
    //设置输出模式为闪光灯模式
    //闪光灯模式：在此模式下相机发送触发信号来激活闪光灯（触发信号分为上升沿、下降沿两种）；
    //用户自定义模式：在此模式下用户可以自己设定相机恒定的输出电平来做特别处理，比如控制LED灯（电平分为高电平或者低电平）。
    //    emStatus = GXSetEnum(m_hDevice, GX_ENUM_USER_OUTPUT_MODE, GX_USER_OUTPUT_MODE_STROBE);
    // 设置白平衡
    // 设置白平衡通道->red
    emStatus = GXSetEnum(m_hDevice, GX_ENUM_BALANCE_RATIO_SELECTOR, GX_BALANCE_RATIO_SELECTOR_RED);
    // 设置白平衡通道对应的白平衡系数
    emStatus = GXSetFloat(m_hDevice, GX_FLOAT_BALANCE_RATIO, m_camSetting->dR_Ratio);
    // 设置白平衡通道->green
    emStatus = GXSetEnum(m_hDevice, GX_ENUM_BALANCE_RATIO_SELECTOR, GX_BALANCE_RATIO_SELECTOR_GREEN);
    // 获取白平衡通道对应的白平衡系数
    emStatus = GXSetFloat(m_hDevice, GX_FLOAT_BALANCE_RATIO, m_camSetting->dG_Ratio);
    // 设置白平衡通道->blue
    emStatus = GXSetEnum(m_hDevice, GX_ENUM_BALANCE_RATIO_SELECTOR, GX_BALANCE_RATIO_SELECTOR_BLUE);
    // 获取白平衡通道对应的白平衡系数
    emStatus = GXSetFloat(m_hDevice, GX_FLOAT_BALANCE_RATIO, m_camSetting->dB_Ratio);
    //开启自动坏点校正
    emStatus = GXSetEnum(m_hDevice, GX_ENUM_DEAD_PIXEL_CORRECT, GX_DEAD_PIXEL_CORRECT_ON);

    //选择增益通道类型
    if (m_camSetting->iGain != 0)
    {
        emStatus = GXSetEnum(m_hDevice, GX_ENUM_GAIN_SELECTOR, GX_GAIN_SELECTOR_ALL);
        emStatus = GXSetFloat(m_hDevice, GX_FLOAT_GAIN, m_camSetting->iGain);
    }
    else if (!qFuzzyCompare(m_camSetting->dR_Gain, 0.0))
    {
        emStatus = GXSetEnum(m_hDevice, GX_ENUM_GAIN_SELECTOR, GX_GAIN_SELECTOR_RED);
        emStatus = GXSetFloat(m_hDevice, GX_FLOAT_GAIN, m_camSetting->dR_Gain);
    }
    else if (!qFuzzyCompare(m_camSetting->dG_Gain, 0.0))
    {
        emStatus = GXSetEnum(m_hDevice, GX_ENUM_GAIN_SELECTOR, GX_GAIN_SELECTOR_GREEN);
        emStatus = GXSetFloat(m_hDevice, GX_FLOAT_GAIN, m_camSetting->dG_Gain);
    }
    else if (!qFuzzyCompare(m_camSetting->dB_Gain, 0.0))
    {
        emStatus = GXSetEnum(m_hDevice, GX_ENUM_GAIN_SELECTOR, GX_GAIN_SELECTOR_BLUE);
        emStatus = GXSetFloat(m_hDevice, GX_FLOAT_GAIN, m_camSetting->dB_Gain);
    }

#ifdef QT_DEBUG
    /*
    注意：如果用户将心跳超时时间设置的非常长，当结束程序的时候没有调用GXCloseDevice接口来关闭设备，或者也没有调用GXCloseLib接口释放资源，
    这就会导致设备在心跳时间内无法复位，从而导致用户再次尝试打开设备的时候失败。可通过复位或重连设备解决此问题，
    有两种方式实现设备的复位或重连：1）在IP配置工具中直接选择复位设备或重连设备；2）通过接口GXGigEResetDevice实现设备的复位或重连。
    */
    GXSetInt(m_hDevice, GX_INT_GEV_HEARTBEAT_TIMEOUT, 300000); //设置心跳时间为5分钟,调试用
#endif
}

void CamDevice::CloseCamera()
{
    [[maybe_unused]]GX_STATUS emStatus = GX_STATUS_SUCCESS;

    if (m_pstCam->bIsSnap)
    {
        StopScan();
    }

    //关闭设备
    //    if (m_pstCam->bIsOpen) {
    emStatus = GXCloseDevice(m_hDevice);
    m_hDevice       = NULL;
    m_pstCam->bIsOpen = FALSE;
    //    }
}

void CamDevice::StartScan()
{
    GX_STATUS emStatus       = GX_STATUS_ERROR;
    bool      bIsColorFilter = m_pstCam->bIsColorFilter;
    // 初始化BMP头信息
    m_pstCam->pBmpInfo                             = (BITMAPINFO *)(m_pstCam->chBmpBuf);
    m_pstCam->pBmpInfo->bmiHeader.biSize           = sizeof(BITMAPINFOHEADER);
    m_pstCam->pBmpInfo->bmiHeader.biWidth          = (LONG)m_pstCam->nImageWidth;
    m_pstCam->pBmpInfo->bmiHeader.biHeight         = -(LONG)m_pstCam->nImageHeight;
    m_pstCam->pBmpInfo->bmiHeader.biPlanes         = 1;
    m_pstCam->pBmpInfo->bmiHeader.biBitCount       = bIsColorFilter ? 24 : 8; // 彩色图像为24,黑白图像为8;
    m_pstCam->pBmpInfo->bmiHeader.biCompression    = BI_RGB;
    m_pstCam->pBmpInfo->bmiHeader.biSizeImage      = 0;
    m_pstCam->pBmpInfo->bmiHeader.biXPelsPerMeter  = 0;
    m_pstCam->pBmpInfo->bmiHeader.biYPelsPerMeter  = 0;
    m_pstCam->pBmpInfo->bmiHeader.biClrUsed        = 0;
    m_pstCam->pBmpInfo->bmiHeader.biClrImportant   = 0;

    // 黑白相机需要初始化调色板信息
    if (!bIsColorFilter)
    {
        // 黑白相机需要进行初始化调色板操作
        for (int i = 0; i < 256; i++)
        {
            m_pstCam->pBmpInfo->bmiColors[i].rgbBlue     = i;
            m_pstCam->pBmpInfo->bmiColors[i].rgbGreen    = i;
            m_pstCam->pBmpInfo->bmiColors[i].rgbRed      = i;
            m_pstCam->pBmpInfo->bmiColors[i].rgbReserved = i;
        }
    }

    // 清空Buffer
    UnPrepareForShowImg();
    // 申请资源
    m_pstCam->pRawBuffer = new BYTE[(size_t)(m_pstCam->nPayLoadSise)];

    if (m_pstCam->pRawBuffer == NULL)
    {
        qCritical() << QString("申请RawBuffer资源失败!相机编号：%1").arg(m_nID + 1);
        return;
    }

    // 如果是彩色相机，为彩色相机申请资源
    if (bIsColorFilter)
    {
        m_pstCam->pImageBuffer = new BYTE[(size_t)(m_pstCam->nImageWidth * m_pstCam->nImageHeight * 3)];

        if (m_pstCam->pImageBuffer == NULL)
        {
            qCritical() << QString("申请彩色ImageBuffer资源失败!相机编号：%1").arg(m_nID + 1);
            delete []m_pstCam->pRawBuffer;
            m_pstCam->pRawBuffer = NULL;
            return;
        }
    }
    // 如果是黑白相机， 为黑白相机申请资源
    else
    {
        m_pstCam->pImageBuffer = new BYTE[(size_t)(m_pstCam->nImageWidth * m_pstCam->nImageHeight)];

        if (m_pstCam->pImageBuffer == NULL)
        {
            qCritical() << QString("申请黑白ImageBuffer资源失败!相机编号：%1").arg(m_nID + 1);
            delete []m_pstCam->pRawBuffer;
            m_pstCam->pRawBuffer = NULL;
            return;
        }
    }

    //注册回调
    RegisterCallback();
    //设置流层Buffer处理模式
    emStatus = GXSetEnum(m_hDevice, GX_DS_ENUM_STREAM_BUFFER_HANDLING_MODE, GX_DS_STREAM_BUFFER_HANDLING_MODE_OLDEST_FIRST);

    if (emStatus != GX_STATUS_SUCCESS)
    {
        UnPrepareForShowImg();
        ShowErrorString(emStatus);
        return;
    }

    //开始采集
    emStatus = GXSendCommand(m_hDevice, GX_COMMAND_ACQUISITION_START);

    if (emStatus != GX_STATUS_SUCCESS)
    {
        UnPrepareForShowImg();
        ShowErrorString(emStatus);
        return;
    }

    m_pstCam->bIsSnap = TRUE;
}

void CamDevice::StopScan()
{
    GX_STATUS emStatus = GX_STATUS_ERROR;
    //停止采集
    emStatus = GXSendCommand(m_hDevice, GX_COMMAND_ACQUISITION_STOP);
    GX_VERIFY(emStatus);
    //注销回调
    emStatus = GXUnregisterCaptureCallback(m_hDevice);
    GX_VERIFY(emStatus);
    //释放资源
    UnPrepareForShowImg();
    m_pCamsFps->Reset();
    m_pstCam->bIsSnap = FALSE;
}

void CamDevice::UnPrepareForShowImg()
{
    if (m_pstCam->pImageBuffer != NULL)
    {
        delete[]m_pstCam->pImageBuffer;
        m_pstCam->pImageBuffer = NULL;
    }

    if (m_pstCam->pRawBuffer != NULL)
    {
        delete []m_pstCam->pRawBuffer;
        m_pstCam->pRawBuffer = NULL;
    }
}

void CamDevice::UpdateAutoSaveFlag(bool bcheck)
{
    m_autoSave = bcheck;
}

void CamDevice::SaveImage()
{
    DWORD                dwImageSize = (DWORD)(m_pstCam->nImageWidth * m_pstCam->nImageHeight * 3);
    BITMAPFILEHEADER     stBfh;
    DWORD                dwBytesRead = 0;
    //    SYSTEMTIME           sysTime;
    QString              strFileName = QString("");
    std::string strTmp = m_strFilePath.toStdString();
    memset(&stBfh, 0, sizeof(BITMAPFILEHEADER));
    //创建保存图像的文件夹
    BOOL bRet = CreateDirectoryA(strTmp.c_str(), NULL);

    if (bRet)
    {
        SetFileAttributesA(strTmp.c_str(), FILE_ATTRIBUTE_NORMAL);
    }

    // 获取当前时间为图像保存的默认名称
    //    GetLocalTime(&sysTime);
    strFileName = QString("%1\\%2_%3_%4.bmp").arg(m_strFilePath).arg(IMG_LOAD_NAME).arg(g_iSaveCount).arg(m_nID);
    stBfh.bfType    = (WORD)'M' << 8 | 'B';          //定义文件类型
    stBfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);  //定义文件头大小彩色
    stBfh.bfSize    = stBfh.bfOffBits + dwImageSize; //文件大小
    strTmp = strFileName.toStdString();
    //创建文件
    HANDLE hFile = ::CreateFileA(strTmp.c_str(),
                                 GENERIC_WRITE,
                                 0,
                                 NULL,
                                 CREATE_ALWAYS,
                                 FILE_ATTRIBUTE_NORMAL,
                                 NULL);

    if (hFile != INVALID_HANDLE_VALUE)
    {
        ::WriteFile(hFile, &stBfh, sizeof(BITMAPFILEHEADER), &dwBytesRead, NULL);
        ::WriteFile(hFile, m_pstCam->pBmpInfo, sizeof(BITMAPINFOHEADER), &dwBytesRead, NULL);  //彩色
        ::WriteFile(hFile, m_pstCam->pImageBuffer, dwImageSize, &dwBytesRead, NULL);
        CloseHandle(hFile);
    }
}
