#if _MSC_VER >=1600 //VS2010版本号是1600
    #pragma execution_character_set("utf-8")
#endif
#pragma once

//
//      ┏┛ ┻━━━━━┛ ┻┓
//      ┃　　　　　　 ┃
//      ┃　　　━　　　┃
//      ┃　┳┛　  ┗┳　┃
//      ┃　　　　　　 ┃
//      ┃　　　┻　　　┃
//      ┃　　　　　　 ┃
//      ┗━┓　　　┏━━━┛
//        ┃　　　┃   神兽保佑
//        ┃　　　┃   代码无BUG！
//        ┃　　　┗━━━━━━━━━┓
//        ┃　　　　　　　    ┣┓
//        ┃　　　　         ┏┛
//        ┗━┓ ┓ ┏━━━┳ ┓ ┏━┛
//          ┃ ┫ ┫   ┃ ┫ ┫
//          ┗━┻━┛   ┗━┻━┛

#include <QtGlobal>
#include <QString>
#include <windef.h>
#include "StopWatch.h"

#define CAMERANUM 1
#define   TOP_CAMERA                   0     ///< 0号相机
#define   SIDE1_CAMERA                 1     ///< 1号相机
#define   SIDE2_CAMERA                 2     ///< 2号相机
#define   SIDE3_CAMERA                 3     ///< 3号相机
#define   SIDE4_CAMERA                 4     ///< 4号相机
#define   BOTTOM_CAMERA                5     ///< 5号相机
#define MAX_COLOR_SETTING 3
#define SERIAL_NUM_SEND_DATA 20
#define SERIAL_NUM_RESULT_DATA 10
#define SERIAL_EFFICTIVE_NUM_RESULT_DATA 6
#define SCRE_BAD_NUM            0   //病变
#define SCRE_BAD_RATIO          1   //干巴
#define SCRE_RED_RATIO          2   //畸形率
#define SCRE_APL_SHAPE          3   //裂果
#define SCRE_APL_DIAMETER       4   //直径mm
#define SCRE_APL_REDTYPE        5   //转色
#define SCRE_APL_SCARE          6
#define SCRE_APL_DASH2          7
#define SCRE_APL_DASH3          8
#define SCRE_APL_DASH4          9
//#define SCRE_S_PITCH        10

#define CAM_DEFAULT_CHECK_SIDE  1

#define MAX_COLOR_HINSYU  10
#define TABLE_MAX_ROW_COUNT 300
#define CUPDATA_BUFFER 100
#define IMAGE_BUFFER_CNT 60


#define ANGLE_90 90
#define ANGLE_210 210
#define ANGLE_30 -30

#define LBL_CAM_NUM 8

#define IMG_LOAD_NAME "img"
#define IMG_SAVE_PATH "\\GxImages"
#define IMG_CONTRAST_PATH "\\ImgCtrs"
#define IMG_CONTRAST_RAT "rat"
#define IMG_CONTRAST_TYPE "typ"


typedef enum
{
    SCAN_TYPE_OFF = 0,
    SCAN_TYPE_MANUAL,
    SCAN_TYPE_AUTO
} ScanType;

#define MENU_DISP_0  0X01         // =>  0000 0001
#define MENU_DISP_1  0X02         // =>  0000 0010
#define MENU_DISP_2  0X04         // =>  0000 0100
#define MENU_DISP_3  0X08         // =>  0000 1000
#define MENU_DISP_4  0X10         // =>  0001 0000
#define MENU_DISP_5  0X20         // =>  0010 0000
#define MENU_DISP_6  0X40         // =>  0100 0000
#define MENU_DISP_7  0X80         // =>  1000 0000

#define MENU_DISPCLR_0  0X01      // => 0 0001
#define MENU_DISPCLR_1  0X02      // => 0 0010
#define MENU_DISPCLR_2  0X04      // => 0 0100
#define MENU_DISPCLR_3  0X08      // => 0 1000
#define MENU_DISPCLR_4  0X10      // => 1 0000


// system setting
typedef struct TSYSetting
{
    int iRsPort;
    QString aCaption; //
    int iMinFreeDisk; //
    bool bFixMonitor; //
    int iMonitorWidth; //
    int iMonitorHeight; //
    int iHinNo;
    QString aHinName; //
    int idistance;
    int iFpsTest;
    int iIOCTLIdx;
    //    int iMeasType;
    TSYSetting()
    {
        iRsPort = 1;
        iMinFreeDisk =  100;
        aCaption = QString("外观检测软件");
        iHinNo =  0;
        bFixMonitor =  0;
        iMonitorWidth =  1280;
        iMonitorHeight =  1024;
        aHinName = QString::Null();
        idistance = 0;
        iFpsTest = 2;
        iIOCTLIdx = 0;
        //iMeasType = 0;
    }
} SYSetting;

// camera struct
// ------------------------------------------------
typedef struct TColorSetting
{
    QString aName;
    int iRed;
    int iGreen;
    int iHanni;
    int iWmin;
    int iWmax;
    int iRG_A1;
    int iRG_A2;
    int iRG_K;
    int iRG_Rmin;
    int iDispR;
    int iDispG;
    int iDispB;
    bool bUse;
    int iCount;
    TColorSetting()
    {
        aName = QString::Null();
        iRed = 0;
        iGreen = 0;
        iHanni = 0;
        iWmin = 0;
        iWmax = 0;
        iRG_A1 = 0;
        iRG_A2 = 0;
        iRG_K = 0;
        iRG_Rmin = 0;
        iDispR = 0;
        iDispG = 0;
        iDispB = 0;
        bUse = false;
        iCount = 0;
    }
} ColorSetting;

typedef struct TImageSetting
{
    int iBinaryMaxB; //
    int iBinaryMinG; //
    int iBinaryThreshold; //
    int iErodeDilateNum; //

    int iBinaryShift; // 255*(g-b+iBinaryShift) /(g+b)
    int iSealGHoseiX; // ラベルの設定色重心からラベル重心のX軸シフト
    int iSealGHoseiY; // ラベルの設定色重心からラベル重心のY軸シフト
    int iSealWidth; // ラベルの楕円幅
    int iSealHeight; // ラベルの楕円高さ
    int iSealMinSize; // ラベルの楕円高さ
    int iExpoTime;

    int iBinarySepLine; // 二値化、上下部境目Y値
    int iBinaryTopThr; // 上部二値化計算
    int iBinaryBotThr; // 下部二値化計算
    int iStripeThreshold; // 縞模様角度、G-B閾値
    double dStripeAngleWt; // 縞模様傾き角度割合
    double dCenterLineAngleWt; // 中央線傾き角度割合
    int iGHosei; // トレー隠れ面積画素数
    int iHeightHosei; // トレー隠れ高さ画素数
    int iShimaBlockSize; // 縞模様ブロックサイズ
    int iShimaThreshold; // 縞模様閾値
    int iShimaMinLen; // 縞模様最小長さ
    int iShimaMinArea; // 縞模様最小面積
    double dVolumeHoseiA; // 体積線形補正係数
    double dVolumeHoseiB; // 体積線形補正定数

    ColorSetting colorSetting[MAX_COLOR_SETTING];

    TImageSetting()
    {
        iBinaryMaxB = 0;
        iBinaryMinG = 120;
        iBinaryThreshold = 0;
        iErodeDilateNum = 0;
        iBinaryShift = 0;
        iSealGHoseiX = 10;
        iSealGHoseiY = 10;
        iSealWidth = 10;
        iSealHeight = 10;
        iSealMinSize = 10;
        iExpoTime = 20;
        iBinarySepLine = 0;
        iBinaryTopThr = 0;
        iBinaryBotThr = 0;
        iStripeThreshold = 0;
        dStripeAngleWt = 0.0;
        dCenterLineAngleWt = 0.0;
        iGHosei = 0;
        iHeightHosei = 0;
        iShimaBlockSize = 0;
        iShimaThreshold = 0;
        iShimaMinLen = 0;
        iShimaMinArea = 0;
        dVolumeHoseiA = 0.0;
        dVolumeHoseiB = 0.0;
    }
} ImageSetting;

// ------------------------------------------------

typedef struct TCamSetting
{
    int iTop; //
    int iLeft; //
    int iBottom; //
    int iRight; //

    double dPixSize; //
    int iMedianFilter; //
    int iKarikomi; //
    double dKaraThr; //
    int iMinLabelingSize; //
    double dTanshuku; //
    int iMinValue; //
    bool bRotate; //

    //bool bBaslerCamera; // Camera Maker  Basler or JAI
    int iWidth; //
    int iWidthMax; //
    int iWidthMin; //
    int iHeight; //
    int iHeightMax; //
    int iHeightMin; //
    int iGainMax; //
    int iGain; //
    int iGainMin; //
    bool bAreaWhiteBalance; //
    double dR_Gain; // R Gain--WhiteBalance
    double dG_Gain; // G Gain--WhiteBalance
    double dB_Gain; // B Gain--WhiteBalance
    double dR_Ratio; // R Ratio--WhiteBalance
    double dG_Ratio; // G Ratio--WhiteBalance
    double dB_Ratio; // B Ratio--WhiteBalance
    bool bTriggerMode; // TriggerMode
    int iTriggerSource; // TriggerSource
    int iRotateMode;
    TCamSetting()
    {
        iTop = 0; //
        iLeft = 0; //
        iBottom = 240; //
        iRight = 320; //
        dPixSize = 0.2;
        iMedianFilter = 3;
        iKarikomi = 3;
        dKaraThr = 30.0;
        iMinLabelingSize = 500;
        dTanshuku = 80.0;
        iMinValue = 15;
        bRotate = true;
        //bBaslerCamera = false;
        iWidth = 720;
        iWidthMax = 776; //
        iWidthMin = 64; //
        iHeight = 540; //
        iHeightMax = 582; //
        iHeightMin = 64; //
        iGainMax = 24; //
        iGain = 0; //
        iGainMin = 0; //
        bAreaWhiteBalance = false;
        dR_Gain = 0.0;
        dG_Gain = 0.0;
        dB_Gain = 0.0;
        dR_Ratio = 1.0;
        dG_Ratio = 1.0;
        dB_Ratio = 1.0;
        bTriggerMode = true;
        iTriggerSource = 0;
        iRotateMode = 0;
    }
} CamSetting;

// system setting
typedef struct ApeDataStruct
{
    double dCupData[SERIAL_NUM_RESULT_DATA];
    bool bKaraCup; //

    ApeDataStruct()
    {
        Clear();
    }

    void Clear()
    {
        bKaraCup = true;

        for (int i = 0; i < SERIAL_NUM_RESULT_DATA; i++)
        {
            dCupData[i] = 0.0;
        }
    }
} ApeDataSt;

//--------------------global variable define-----------------------
extern HANDLE g_camComplHnds[CAMERANUM];// cam capture ok event
extern HANDLE g_SerSendHnd, g_KaraCupHnd;
//--------------------global variable define-----------------------

//--------------------global other variable define-----------------
extern QString SYS_INI_FILE_NAME;
extern QString COLOR_INI_FILE_NAME;
extern QString SECTION_NAME[CAMERANUM];

extern const char *CAM_DEFINE_USERID[CAMERANUM];

extern QString SECTION_NAME_SYS;
extern QString SECTION_NAME_COLOR;
//--------------------global other variable define-----------------

extern CStopWatch g_stopWatch;// ///< 收到图像的时间
extern double g_dCaptureTime;// 开始发送软触发命令到成功收到图像历时
extern double g_dCalTime, g_dDrawTime; // 处理图像历时,描画历时
extern QString g_sResultName[SERIAL_NUM_RESULT_DATA];
extern ApeDataSt g_appleData;

extern int g_iSaveCount; //
extern int g_iLoadCount;

extern bool g_btrimode;
extern bool g_bTopCamKaraCup;//side camera use!!!
//-------------------------------------------------
#define IO_BOARD_IN_PORT_ZERO 0
extern quint8 g_portInStates[4], g_portOutStates[4];// default is 4
// ------------------------------------------------

#include <QObject>
typedef struct ResultData
{
    double dCupData[SERIAL_NUM_RESULT_DATA];
    bool bKaraCup;
    bool bResultOk;

    ResultData()
    {
        Clear();
    }

    void Clear()
    {
        bKaraCup = true;
        bResultOk = false;

        for (int i = 0; i < SERIAL_NUM_RESULT_DATA; i++)
        {
            dCupData[i] = 0.0;
        }
    }
} RESULTDATA;
Q_DECLARE_METATYPE(RESULTDATA);
